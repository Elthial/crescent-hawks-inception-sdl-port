#include "backend.h"
#include <SDL3/SDL.h>
#include <limits.h>
#include <setjmp.h>
static jmp_buf applicationReturn;
static int applicationActive;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static uint32_t scanoutPixels[ScreenWidth*ScreenHeight];
static uint64_t previousPresentation;
static int graphicsRuntimeActive;
static void RefreshEgaScreen(int force);
static SDL_AudioStream *audio;
static SDL_Mutex *speakerMutex;
static uint32_t speakerDivisor=65536;
uint8_t SDLBackend_SpeakerControl;
uint16_t SDLBackend_SpeakerDelayState,SDLBackend_SpeakerDelayMask,SDLBackend_SpeakerDelayMinimum;
static uint64_t speakerPhase;
static int16_t retainedAudio[AudioQueueCapacity];
static size_t retainedRead,retainedCount;
static uint64_t speakerSpanFraction;
static unsigned soundEffectDepth;
static uint64_t effectStarted,effectDuration;
/* DOSBox-X's PC-speaker channel slews each digital transition, then applies
 * two 14kHz one-pole low-pass stages. Its mixer slowly recentres the
 * speaker's native unipolar 0..10000 signal. Keep that physical response at
 * the SDL output boundary: the recovered PIT/gate stream remains exact. */
static int32_t speakerResponseLevel,speakerLowpass[2];
static int64_t speakerDcOffsetQ16;
/* The sweep delay is matched to the supplied DOSBox-X Mech-beam recording at
 * 1510 cycles/ms (its 500->100 Hz descent, rather than just its first edge).
 * Fixed-tone and noise costs remain provisional until separately matched. */
enum { FixedToneCycleTenths=180,SweepCycleTenths=80,NoiseCycleTenths=105,
    NanosecondsPerMillisecondPerCycleTenth=100000 };
uint32_t SDLBackend_SoundCpuCyclesPerMillisecond=SDLBackend_SoundCpuIbmPc;
uint32_t SDLBackend_FixedToneIterationNs=75000;
uint32_t SDLBackend_SweepIterationNs=33333;
uint32_t SDLBackend_NoiseIterationNs=43750;
uint64_t SDLBackend_TimedEffectSamples,SDLBackend_TimedEffectNonzeroSamples;
static void CheckApplicationClose(void);
enum { PITInputHz=1193182, SpeakerAmplitude=4096, SpeakerResponseAmplitude=10000,
    /* Effective48kHz response measured from the supplied DOSBox-X transition:
     * 1217,3610,5905,7587... after its first fractional event sample. */
    SpeakerResponseSlewPerSample=4260, SpeakerLowpassAlphaQ16=35127,
    AudioChunkSamples=512 };
uint32_t SDLBackend_RetracesPerSecond=70; /* host timing setting; gameplay does not set it implicitly */
static int16_t NextSpeakerSample(void)
{
    uint64_t period=(uint64_t)AudioSampleRate*speakerDivisor;
    int16_t sample=(SDLBackend_SpeakerControl&2) ?
        (int16_t)(!(SDLBackend_SpeakerControl&1) || speakerPhase<period/2 ? SpeakerAmplitude : -SpeakerAmplitude) : 0;
    /* Sol: reject ultrasonic PIT tones before sampling, not after aliasing.
     * Original music's divisor14 produces ~85kHz, not an audible rest whistle.
     * Keep its divisor/gate/phase semantics; direct software-gate output stays. */
    if ((SDLBackend_SpeakerControl&3)==3 && UINT64_C(2)*PITInputHz>=period)
        sample=0;
    speakerPhase=(speakerPhase+PITInputHz)%period;
    return sample;
}
static int16_t ApplySpeakerResponse(int16_t raw)
{
    int32_t target=raw>0?SpeakerResponseAmplitude:0;
    int32_t difference=target-speakerResponseLevel;
    int32_t filteredInput;
    if(difference>SpeakerResponseSlewPerSample) difference=SpeakerResponseSlewPerSample;
    else if(difference<-SpeakerResponseSlewPerSample) difference=-SpeakerResponseSlewPerSample;
    speakerResponseLevel+=difference;
    filteredInput=speakerResponseLevel;
    for(unsigned stage=0;stage<2;++stage) {
        int64_t delta=(int64_t)(filteredInput-speakerLowpass[stage])*
            SpeakerLowpassAlphaQ16;
        speakerLowpass[stage]+=(int32_t)(delta/65536);
        filteredInput=speakerLowpass[stage];
    }
    /* DOSBox-X's optional DC correction uses an approximately one-second
     * time constant. Q16 retains the sub-sample adjustment near zero. */
    int64_t responseQ16=((int64_t)speakerLowpass[1]<<16)+speakerDcOffsetQ16;
    int64_t response;
    speakerDcOffsetQ16-=responseQ16/AudioSampleRate;
    response=responseQ16>>16;
    if(response>INT16_MAX) response=INT16_MAX;
    else if(response<INT16_MIN) response=INT16_MIN;
    return (int16_t)response;
}
void SDLBackend_RenderSpeaker(int16_t *samples,size_t count)
{
    size_t i;
    if(speakerMutex) SDL_LockMutex(speakerMutex);
    for(i=0;i<count;++i) {
        if(retainedCount) {
            samples[i]=retainedAudio[retainedRead];
            retainedRead=(retainedRead+1)%AudioQueueCapacity;
            --retainedCount;
            continue;
        }
        samples[i]=soundEffectDepth?0:NextSpeakerSample();
    }
    if(speakerMutex) SDL_UnlockMutex(speakerMutex);
}
static void SDLCALL FeedAudio(void *unused,SDL_AudioStream *stream,int additional,int total)
{
    int16_t samples[AudioChunkSamples];
    (void)unused; (void)total;
    while(additional>0) {
        int count=additional/(int)sizeof samples[0];
        if(count>AudioChunkSamples) count=AudioChunkSamples;
        if(!count) break;
        SDLBackend_RenderSpeaker(samples,(size_t)count);
        for(int index=0;index<count;++index) samples[index]=ApplySpeakerResponse(samples[index]);
        if(!SDL_PutAudioStreamData(stream,samples,count*(int)sizeof samples[0])) break;
        additional-=count*(int)sizeof samples[0];
    }
}
void SDLBackend_Close(void)
{
    graphicsRuntimeActive=0;
    SDLBackend_RestoreSystemTimer(0xFFFF);
    SDL_DestroyAudioStream(audio); audio=NULL;
    SDL_DestroyMutex(speakerMutex); speakerMutex=NULL;
    retainedRead=retainedCount=0;
    speakerSpanFraction=0;
    speakerResponseLevel=speakerLowpass[0]=speakerLowpass[1]=0;
    speakerDcOffsetQ16=0;
    soundEffectDepth=0;
    SDL_DestroyTexture(texture); texture=NULL;
    SDL_DestroyRenderer(renderer); renderer=NULL;
    SDL_DestroyWindow(window); window=NULL;
    SDL_Quit();
}
int SDLBackend_Open(void)
{
    SDL_AudioSpec spec={SDL_AUDIO_S16,1,AudioSampleRate};
    if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)) return 0;
    if(!SDL_CreateWindowAndRenderer("Crescent Hawks Inception - preservation",ScreenWidth*3,ScreenHeight*3,SDL_WINDOW_RESIZABLE,&window,&renderer)) goto Failed;
    if(!SDL_SetRenderLogicalPresentation(renderer,ScreenWidth,ScreenHeight,SDL_LOGICAL_PRESENTATION_LETTERBOX)) goto Failed;
    texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,ScreenWidth,ScreenHeight);
    if(!texture || !SDL_SetTextureScaleMode(texture,SDL_SCALEMODE_NEAREST)) goto Failed;
    speakerMutex=SDL_CreateMutex();
    if(!speakerMutex) goto Failed;
    audio=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,FeedAudio,NULL);
    if(!audio || !SDL_ResumeAudioStreamDevice(audio)) goto Failed;
    return 1;
Failed:
    SDL_Log("Platform startup failed: %s",SDL_GetError()); SDLBackend_Close(); return 0;
}
int SDLBackend_PollQuit(void)
{
    SDL_Event event;
    SDL_PumpEvents();
    /* Do not discard gameplay/menu keys or treat menu ESC as app shutdown. */
    return SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_EVENT_QUIT,SDL_EVENT_QUIT)>0;
}
int SDLBackend_RunApplication(void (*entry)(void))
{
    /* Original game code is C. Only main-thread SDL input/retrace boundaries
     * jump here; audio callbacks never unwind the game stack or own SDL teardown.
     * The caller closes video/audio after return, outside original game frames. */
    if(setjmp(applicationReturn)!=0) {
        applicationActive=0;
        return SDLBackend_ApplicationClosed;
    }
    applicationActive=1;
    entry();
    applicationActive=0;
    return SDLBackend_ApplicationReturned;
}
static void CheckApplicationClose(void)
{
    if(applicationActive && SDLBackend_PollQuit()) longjmp(applicationReturn,1);
}
uint16_t SDLBackend_InputPending(void)
{
    CheckApplicationClose();
    RefreshEgaScreen(0);
    SDLBackend_ServiceMusicTimer();
    SDL_Event event;
    SDL_PumpEvents();
    if(SDL_PeepEvents(&event,1,SDL_PEEKEVENT,SDL_EVENT_KEY_DOWN,SDL_EVENT_KEY_DOWN)>0) return 255;
    if(SDL_PeepEvents(&event,1,SDL_PEEKEVENT,SDL_EVENT_QUIT,SDL_EVENT_QUIT)>0) return 255;
    return 0;
}
int SDLBackend_Present(const uint32_t *rgba)
{
    return SDL_UpdateTexture(texture,NULL,rgba,ScreenWidth*4) &&
        SDL_RenderClear(renderer) && SDL_RenderTexture(renderer,texture,NULL,NULL) && SDL_RenderPresent(renderer);
}
int SDLBackend_PresentEgaScreen(void)
{
    SDLBackend_DecodeEgaScreen(scanoutPixels);
    return SDLBackend_Present(scanoutPixels);
}
static void RefreshEgaScreen(int force)
{
    uint64_t now=SDL_GetTicksNS();
    if(!graphicsRuntimeActive || !renderer) return;
    /* Presentation cadence is a host choice, not an original gameplay timer.
     * Input/music busy loops must not upload 64000 pixels on every probe. */
    if(!force && now-previousPresentation<UINT64_C(1000000000)/70) return;
    previousPresentation=now;
    if(!SDLBackend_PresentEgaScreen()) SDL_Log("EGA presentation failed: %s",SDL_GetError());
}
void SDLBackend_CommitEgaViewportTransfer(void)
{
    uint64_t now,period,elapsed;
    if(!graphicsRuntimeActive || !renderer) return;
    period=SDLBackend_RetracesPerSecond?
        UINT64_C(1000000000)/SDLBackend_RetracesPerSecond:0;
    now=SDL_GetTicksNS();
    /* Sol: real EGA scanout exposed successive full-viewport missile copies.
     * SDL's buffered renderer otherwise collapses an unpaced burst to its last
     * frame. Wait only when the preceding presentation is still inside one
     * display interval; native five-retrace holds therefore gain no extra tick. */
    if(period && previousPresentation) {
        elapsed=now-previousPresentation;
        if(elapsed<period) SDL_DelayNS(period-elapsed);
    }
    CheckApplicationClose();
    SDLBackend_ServiceMusicTimer();
    RefreshEgaScreen(1);
}
uint64_t SDLBackend_Milliseconds(void) { return SDL_GetTicks(); }
void SDLBackend_Delay(uint32_t milliseconds) { SDL_Delay(milliseconds); }
int SDLBackend_QueueAudio(const int16_t *samples,size_t count)
{
    size_t index,write;
    if(count>AudioQueueCapacity || (!samples && count)) return 0;
    if(speakerMutex) SDL_LockMutex(speakerMutex);
    if(count>AudioQueueCapacity-retainedCount) {
        if(speakerMutex) SDL_UnlockMutex(speakerMutex);
        return 0;
    }
    write=(retainedRead+retainedCount)%AudioQueueCapacity;
    for(index=0;index<count;++index) {
        retainedAudio[write]=samples[index];
        write=(write+1)%AudioQueueCapacity;
    }
    retainedCount+=count;
    if(speakerMutex) SDL_UnlockMutex(speakerMutex);
    return 1;
}
int SDLBackend_QueueSpeakerSpan(uint64_t nanoseconds)
{
    uint64_t units,count;
    size_t index,write;
    /* Bound before multiplying; no overflowing or silently clipped duration. */
    if(nanoseconds>(uint64_t)AudioQueueCapacity*UINT64_C(1000000000)/AudioSampleRate) return 0;
    if(speakerMutex) SDL_LockMutex(speakerMutex);
    units=nanoseconds*AudioSampleRate+speakerSpanFraction;
    count=units/UINT64_C(1000000000);
    if(count>AudioQueueCapacity-retainedCount) {
        if(speakerMutex) SDL_UnlockMutex(speakerMutex);
        return 0;
    }
    write=(retainedRead+retainedCount)%AudioQueueCapacity;
    for(index=0;index<(size_t)count;++index) {
        retainedAudio[write]=NextSpeakerSample();
        if(soundEffectDepth && retainedAudio[write]!=0) ++SDLBackend_TimedEffectNonzeroSamples;
        write=(write+1)%AudioQueueCapacity;
    }
    retainedCount+=(size_t)count;
    if(soundEffectDepth) SDLBackend_TimedEffectSamples+=count;
    speakerSpanFraction=units%UINT64_C(1000000000);
    if(speakerMutex) SDL_UnlockMutex(speakerMutex);
    return 1;
}
void SDLBackend_BeginSoundEffect(void)
{
    if(!audio) return; /* Headless rule fixtures do not enable physical timing. */
    SDL_LockMutex(speakerMutex);
    if(!soundEffectDepth) {
        effectStarted=SDL_GetTicksNS(); effectDuration=0;
        SDLBackend_TimedEffectSamples=SDLBackend_TimedEffectNonzeroSamples=0;
    }
    ++soundEffectDepth;
    SDL_UnlockMutex(speakerMutex);
}
static void CaptureSoundDelay(uint64_t nanoseconds)
{
    if(!soundEffectDepth || !nanoseconds) return;
    while(nanoseconds) {
        uint64_t capacity=(uint64_t)AudioQueueCapacity*UINT64_C(1000000000)/AudioSampleRate;
        uint64_t span=nanoseconds>capacity?capacity:nanoseconds;
        while(!SDLBackend_QueueSpeakerSpan(span)) {
            CheckApplicationClose(); RefreshEgaScreen(0); SDL_Delay(1);
        }
        effectDuration+=span; nanoseconds-=span;
    }
}
int SDLBackend_SoundEffectActive(void) { return soundEffectDepth!=0; }
void SDLBackend_SetSoundCpuCyclesPerMillisecond(uint32_t cyclesPerMillisecond)
{
    if(!cyclesPerMillisecond) return;
    SDLBackend_SoundCpuCyclesPerMillisecond=cyclesPerMillisecond;
    SDLBackend_FixedToneIterationNs=(uint32_t)((uint64_t)FixedToneCycleTenths*
        NanosecondsPerMillisecondPerCycleTenth/cyclesPerMillisecond);
    SDLBackend_SweepIterationNs=(uint32_t)((uint64_t)SweepCycleTenths*
        NanosecondsPerMillisecondPerCycleTenth/cyclesPerMillisecond);
    SDLBackend_NoiseIterationNs=(uint32_t)((uint64_t)NoiseCycleTenths*
        NanosecondsPerMillisecondPerCycleTenth/cyclesPerMillisecond);
}
void SDLBackend_ToneDelayIteration(int sweep)
{
    CaptureSoundDelay(sweep?SDLBackend_SweepIterationNs:SDLBackend_FixedToneIterationNs);
}
void SDLBackend_EndSoundEffect(void)
{
    size_t pending;
    if(!soundEffectDepth) return;
    if(soundEffectDepth>1) {
        SDL_LockMutex(speakerMutex); --soundEffectDepth; SDL_UnlockMutex(speakerMutex);
        return;
    }
    do {
        CheckApplicationClose(); RefreshEgaScreen(0); SDLBackend_ServiceMusicTimer();
        SDL_LockMutex(speakerMutex); pending=retainedCount; SDL_UnlockMutex(speakerMutex);
        if(pending || SDL_GetTicksNS()-effectStarted<effectDuration) SDL_Delay(1);
    } while(pending || SDL_GetTicksNS()-effectStarted<effectDuration);
    SDL_LockMutex(speakerMutex); soundEffectDepth=0; SDL_UnlockMutex(speakerMutex);
}
void SDLBackend_ConfigureSpeaker(void)
{
    /* PIT mode3 is fixed in this backend. Control word alone changes no divisor. */
    if(speakerMutex) SDL_LockMutex(speakerMutex);
    speakerPhase=0;
    if(speakerMutex) SDL_UnlockMutex(speakerMutex);
}
void SDLBackend_SpeakerOn(uint16_t divisor)
{
    if(speakerMutex) SDL_LockMutex(speakerMutex);
    /* A PIT divisor write changes the running oscillator's period. Restarting
     * its phase for every sweep step pins slow tones high: the next write
     * arrives before the waveform can reach its falling edge. */
    speakerDivisor=divisor ? divisor : 65536; SDLBackend_SpeakerControl|=3;
    speakerPhase%=((uint64_t)AudioSampleRate*speakerDivisor);
    if(speakerMutex) SDL_UnlockMutex(speakerMutex);
}
void SDLBackend_SpeakerOff(void)
{
    if(speakerMutex) SDL_LockMutex(speakerMutex);
    SDLBackend_SpeakerControl&=(uint8_t)~3;
    if(speakerMutex) SDL_UnlockMutex(speakerMutex);
}
void SDLBackend_SetSpeakerDivisor(uint16_t divisor)
{
    if(speakerMutex) SDL_LockMutex(speakerMutex);
    speakerDivisor=divisor?divisor:65536;
    speakerPhase%=((uint64_t)AudioSampleRate*speakerDivisor);
    if(speakerMutex) SDL_UnlockMutex(speakerMutex);
}
void SDLBackend_SetSpeakerGate(int enabled)
{
    if(speakerMutex) SDL_LockMutex(speakerMutex);
    if(enabled)SDLBackend_SpeakerControl|=3;
    else SDLBackend_SpeakerControl&=(uint8_t)~3;
    if(speakerMutex) SDL_UnlockMutex(speakerMutex);
}
uint16_t SDLBackend_ToggleSpeaker(uint16_t seed)
{
    /* Original207F:007D: store seed, XOR only speaker bit1, then ADD/ROR
     * as WORD. Bit0 remains clear for direct software noise after0067. */
    SDLBackend_SpeakerDelayState=seed;
    if(speakerMutex)SDL_LockMutex(speakerMutex);
    SDLBackend_SpeakerControl^=2;
    if(speakerMutex)SDL_UnlockMutex(speakerMutex);
    uint16_t mixed=(uint16_t)(SDLBackend_SpeakerDelayState+0x9248);
    SDLBackend_SpeakerDelayState=(uint16_t)((mixed>>3)|(mixed<<13));
    return SDLBackend_SpeakerDelayState;
}
void SDLBackend_SpeakerCountdown(uint16_t delayState,uint16_t mask,uint16_t minimumBits)
{
    /* Original207F:00A9: DX unchanged. LOOP with initialCX0 still runs
     *65536 times. Active effects capture that count using the documented SDL
     *duration profile; native physical CPU timing remains uncalibrated. */
    SDLBackend_SpeakerDelayMask=mask;SDLBackend_SpeakerDelayMinimum=minimumBits;
    uint16_t remaining=(uint16_t)((delayState&mask)|minimumBits);
    if(soundEffectDepth) {
        uint32_t iterations=remaining?remaining:65536;
        CaptureSoundDelay((uint64_t)iterations*SDLBackend_NoiseIterationNs);
        return;
    }
    do {--remaining;}while(remaining);
}
void SDLBackend_SetVideoMode(uint16_t mode)
{
    if(mode==2) SDLBackend_Close(); /* original shutdown text mode */
    else if(!window && !SDLBackend_Open()) SDL_Log("Cannot replace BIOS video mode%u",(unsigned)mode);
}
void SDLBackend_InitializeGraphicsRuntime(void)
{
    /* Original startup enters graphics and clears the visible screen. Host
     * display timing belongs to WaitRetrace; DOS interrupt/error handlers and
     * compiler-library file buffers have no host counterpart to install. */
    if(!window && !SDLBackend_Open()) {
        SDL_Log("Cannot initialize preservation graphics: %s",SDL_GetError());
        return;
    }
    SDLBackend_EgaMapMask=EgaAllPlanesMask;
    SDLBackend_EgaRasterOperation=EgaRaster_Replace;
    SDLBackend_EgaRotateCount=0;
    SDLBackend_EgaEnableSetReset=0;
    SDLBackend_EgaSetReset=0;
    SDLBackend_EgaReadMapSelect=0;
    SDLBackend_ClearEgaScreen();
    graphicsRuntimeActive=1;
    RefreshEgaScreen(1);
}
void SDLBackend_WaitRetrace(uint8_t phase)
{
    CheckApplicationClose();
    RefreshEgaScreen(0);
    SDLBackend_ServiceMusicTimer();
    (void)phase; /* port-edge fidelity pending; host wait preserves requested count */
    if(SDLBackend_RetracesPerSecond) SDL_DelayNS(UINT64_C(1000000000)/SDLBackend_RetracesPerSecond);
}
int16_t SDLBackend_ReadKeyboard(void)
{
    SDL_Event event;
    RefreshEgaScreen(1); /* show the completed menu before blocking for a key */
    for(;;) {
        uint8_t value;
        SDLBackend_ServiceMusicTimer();
        RefreshEgaScreen(0);
        if(!SDL_WaitEventTimeout(&event,8)) continue;
        if(event.type==SDL_EVENT_QUIT) {
            if(applicationActive) longjmp(applicationReturn,1);
            return 27; /* unscoped low-level keyboard compatibility */
        }
        if(event.type!=SDL_EVENT_KEY_DOWN) continue;
        /* Sol: SDL keypad keycodes are not ASCII. Preserve DOS keypad digits
         * with Num Lock, navigation scan codes otherwise; Shift reverses Num
         * Lock for this keypress. These feed the original command conversion. */
        if(event.key.scancode>=SDL_SCANCODE_KP_1 && event.key.scancode<=SDL_SCANCODE_KP_0) {
            static const uint8_t digits[]="1234567890";
            static const uint8_t scans[]={0x4F,0x50,0x51,0x4B,0x4C,0x4D,0x47,0x48,0x49,0x52};
            size_t index=(size_t)(event.key.scancode-SDL_SCANCODE_KP_1);
            int numeric=((event.key.mod&SDL_KMOD_NUM)!=0)!=((event.key.mod&SDL_KMOD_SHIFT)!=0);
            return numeric ? (int16_t)digits[index] : -(int16_t)scans[index];
        }
        switch(event.key.scancode) {
        /* Sol: either physical Enter key supplies the same DOS carriage return.
         * SDL keypad keycodes are non-ASCII and would otherwise be discarded. */
        case SDL_SCANCODE_KP_ENTER: return '\r';
        case SDL_SCANCODE_UP: return -0x48;
        case SDL_SCANCODE_DOWN: return -0x50;
        case SDL_SCANCODE_LEFT: return -0x4B;
        case SDL_SCANCODE_RIGHT: return -0x4D;
        case SDL_SCANCODE_HOME: return -0x47;
        case SDL_SCANCODE_END: return -0x4F;
        case SDL_SCANCODE_PAGEUP: return -0x49;
        case SDL_SCANCODE_PAGEDOWN: return -0x51;
        default: break;
        }
        if(event.key.key>127) continue; /* non-ASCII/remaining extended keys pending */
        value=(uint8_t)event.key.key;
        if(value>='a' && value<='z') {
            int shift=(event.key.mod & SDL_KMOD_SHIFT)!=0;
            int caps=(event.key.mod & SDL_KMOD_CAPS)!=0;
            if(shift!=caps) value=(uint8_t)(value-'a'+'A');
        } else if(event.key.mod & SDL_KMOD_SHIFT) {
            static const char normal[]="1234567890-=[];'/.,`\\";
            static const char shifted[]="!@#$%^&*()_+{}:\"?><~|";
            size_t i;
            for(i=0;normal[i];++i) if(value==(uint8_t)normal[i]) {value=(uint8_t)shifted[i];break;}
        }
        return (int16_t)(int8_t)value;
    }
}
