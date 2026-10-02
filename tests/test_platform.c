/* Test runner, not a made-up game entry point. Calls original wrappers. */
#define _CRT_SECURE_NO_WARNINGS
#include "dos.h"
#include "game.h"
#include "backend.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
static void check(int ok) {++checks;if(!ok){fprintf(stderr,"SDL redirect check%u failed\n",checks);exit(1);}}
static unsigned soundCallReturned;
static void playEffectUntilClose(void)
{
    Sound_Setup(7); /* Original explosion stream: longer than close-test allowance. */
    soundCallReturned=1;
}
static Uint32 SDLCALL requestSoundClose(void *unused,SDL_TimerID timer,Uint32 interval)
{
    SDL_Event quit;
    (void)unused; (void)timer; (void)interval;
    memset(&quit,0,sizeof quit); quit.type=SDL_EVENT_QUIT;
    (void)SDL_PushEvent(&quit);
    return 0;
}
int main(int argc,char **argv)
{
    static uint32_t pixels[ScreenWidth*ScreenHeight];
    int16_t samples[32];
    char path[256];
    uint8_t bytes[4]={1,2,3,4},readBack[8]={0};
    int16_t handle;
    SDL_Event key;
    unsigned i;
    check(argc==1 || (argc==2 && !strcmp(argv[1],"--sound-close")) ||
        ((argc>=2 && argc<=4) && !strcmp(argv[1],"--sound-timing")));
    Initialize_Graphics_Runtime(); check(SDLBackend_Present(pixels));
    check(SDLBackend_SoundCpuCyclesPerMillisecond==SDLBackend_SoundCpuIbmPc);
    /* Faster emulated CPUs shorten every busy-wait delay. Do not duplicate
     * the current tuning constants here: only audio comparison can validate
     * their values against the original recording. */
    const uint32_t profiles[]={SDLBackend_SoundCpuIbmPc,SDLBackend_SoundCpu286,
        SDLBackend_SoundCpu1510,SDLBackend_SoundCpuDosBoxNostalgia};
    uint32_t previousFixed=UINT32_MAX,previousSweep=UINT32_MAX,previousNoise=UINT32_MAX;
    for(size_t profile=0;profile<sizeof profiles/sizeof profiles[0];++profile) {
        SDLBackend_SetSoundCpuCyclesPerMillisecond(profiles[profile]);
        check(SDLBackend_SoundCpuCyclesPerMillisecond==profiles[profile]);
        check(SDLBackend_FixedToneIterationNs>0 && SDLBackend_FixedToneIterationNs<previousFixed);
        check(SDLBackend_SweepIterationNs>0 && SDLBackend_SweepIterationNs<previousSweep);
        check(SDLBackend_NoiseIterationNs>0 && SDLBackend_NoiseIterationNs<previousNoise);
        previousFixed=SDLBackend_FixedToneIterationNs;
        previousSweep=SDLBackend_SweepIterationNs;
        previousNoise=SDLBackend_NoiseIterationNs;
    }
    SDLBackend_SetSoundCpuCyclesPerMillisecond(0);
    check(SDLBackend_SoundCpuCyclesPerMillisecond==SDLBackend_SoundCpuDosBoxNostalgia);
    SDLBackend_SetSoundCpuCyclesPerMillisecond(SDLBackend_SoundCpuIbmPc);
    if(argc==2 && !strcmp(argv[1],"--sound-close")) {
        Uint64 started=SDL_GetTicksNS();
        SDL_TimerID timer=SDL_AddTimer(100,requestSoundClose,NULL);
        check(timer!=0);
        check(SDLBackend_RunApplication(playEffectUntilClose)==SDLBackend_ApplicationClosed);
        (void)SDL_RemoveTimer(timer); /* One-shot may already be removed. */
        check(!soundCallReturned && SDLBackend_TimedEffectSamples>0);
        check(SDL_GetTicksNS()-started<UINT64_C(2000000000));
        SDLBackend_Close();
        check(!SDLBackend_SoundEffectActive());
        PC_Speaker_OFF(); SDLBackend_RenderSpeaker(samples,32);
        for(i=0;i<32;++i) check(samples[i]==0); /* No retained effect after teardown. */
        check(SDLBackend_Open());
        Sound_Setup(6);
        check(SDLBackend_TimedEffectSamples>0 &&
            SDLBackend_TimedEffectNonzeroSamples>0 &&
            SDLBackend_TimedEffectNonzeroSamples<=SDLBackend_TimedEffectSamples);
        SDLBackend_Close();
        puts("Original timed effect: close interrupts playback, clears output and permits reopening.");
        return 0;
    }
    if(argc>=2) {
        uint16_t first=1,last=18;
        if(argc>=3) {
            char *end;
            unsigned long selected=strtoul(argv[2],&end,10);
            check(*end==0 && selected>=1 && selected<=18);
            first=last=(uint16_t)selected;
        }
        if(argc==4) {
            char *end;
            unsigned long cycles=strtoul(argv[3],&end,10);
            check(*end==0 && cycles>0 && cycles<=UINT32_MAX);
            SDLBackend_SetSoundCpuCyclesPerMillisecond((uint32_t)cycles);
        }
        /* Real original commands/delays, not InceptionTools' separate renderer.
         * Check that playback emits and drains audio; pitch/timbre fidelity is
         * established only by comparing the captured waveform to DOSBox-X. */
        printf("effect,busy_wait_scale,elapsed_ms,retained_samples,nonzero_samples\n");
        for(uint16_t effect=first;effect<=last;++effect) {
            Uint64 start=SDL_GetPerformanceCounter();
            Sound_Setup(effect);
            Uint64 elapsed=SDL_GetPerformanceCounter()-start;
            printf("%u,%u,%.6f,%llu,%llu\n",effect,BusyWaitScale,
                (double)elapsed*1000.0/(double)SDL_GetPerformanceFrequency(),
                (unsigned long long)SDLBackend_TimedEffectSamples,
                (unsigned long long)SDLBackend_TimedEffectNonzeroSamples);
            check(SDLBackend_TimedEffectSamples>0);
            check(SDLBackend_TimedEffectNonzeroSamples<=SDLBackend_TimedEffectSamples);
            check(!SDLBackend_SoundEffectActive());
            SDL_Delay(200); /* Separate effects when run with an audible device. */
        }
        SDLBackend_Close(); return 0;
    }
    /* Initialization is a real original startup redirect, also safe after a
     * display exists. Native clear fills16000 bytes per plane (including the
     * second screen), not the tileset at A400. Reset hardware mode first. */
    const uint8_t startupGlyph[8]={255,255,255,255,255,255,255,255};
    SDLBackend_DrawEgaGlyph(startupGlyph,0,0,15,0);
    check(SDLBackend_ReadEgaPlaneByte(EgaApertureSegment,0,0)==255);
    SDLBackend_EgaMapMask=1;
    SDLBackend_EgaRasterOperation=EgaRaster_Or;
    Initialize_Graphics_Runtime();
    check(SDLBackend_EgaMapMask==15 && SDLBackend_EgaRasterOperation==EgaRaster_Replace);
    for(i=0;i<EgaPlaneCount;++i) {
        check(SDLBackend_ReadEgaPlaneByte(EgaApertureSegment,0,(uint8_t)i)==0);
        check(SDLBackend_ReadEgaPlaneByte(EgaApertureSegment,EgaScreenPlaneBytes-1,(uint8_t)i)==0);
    }
    A_Timer(); PC_Speaker_ON_ptr_freq(0);
    SDLBackend_RenderSpeaker(samples,32); check(samples[0]!=0);
    A_PC_Speaker_OFF(); SDLBackend_RenderSpeaker(samples,32);
    for(i=0;i<32;++i)check(samples[i]==0);
    /* Original fixed tone0 keeps speaker state; sweep tone0 enables the
     * PIT's65536-tick divisor. Delay scale0 avoids claiming host calibration. */
    uint16_t savedBusyWaitScale=BusyWaitScale;BusyWaitScale=0;
    PC_Speaker_ON_ptr_freq(1200);Sound_Play_Fixed_Tone_Delay(0,1);
    SDLBackend_RenderSpeaker(samples,32);check(samples[0]!=0);
    PC_Speaker_OFF();Sound_Play_Fixed_Tone_Delay(0,1);
    SDLBackend_RenderSpeaker(samples,32);check(samples[0]==0);
    Sound_Play_Sweep_Tone_Delay(0,1);SDLBackend_RenderSpeaker(samples,32);check(samples[0]!=0);
    BusyWaitScale=savedBusyWaitScale;
    PC_speaker_OFF_2();check(SDLBackend_SpeakerControl==0);
    for(unsigned seed=0;seed<65536;++seed){
        uint16_t mixed=(uint16_t)(seed+0x9248);
        uint16_t expectedState=(uint16_t)(mixed/8+(mixed%8)*8192);
        check(PC_Speaker_XOR_ptr_freq((uint16_t)seed)==expectedState);
        check(SDLBackend_SpeakerDelayState==expectedState && SDLBackend_SpeakerControl==(uint8_t)((seed&1)?0:2));
    }
    uint16_t noiseState=PC_Speaker_XOR_ptr_freq(1000);
    SDLBackend_RenderSpeaker(samples,32);
    for(i=0;i<32;++i)check(samples[i]==4096); /*direct static level, NOT PIT square wave*/
    Sound_Freq_Countdown_Loop(noiseState,0x1234,0x4321);
    check(SDLBackend_SpeakerDelayState==noiseState && SDLBackend_SpeakerDelayMask==0x1234 && SDLBackend_SpeakerDelayMinimum==0x4321);
    Sound_Freq_Countdown_Loop(noiseState,0,0); /*native zero counter is65536 iterations*/
    check(SDLBackend_SpeakerDelayState==noiseState && SDLBackend_SpeakerControl==2);
    (void)PC_Speaker_XOR_ptr_freq(1000);SDLBackend_RenderSpeaker(samples,32);
    for(i=0;i<32;++i)check(samples[i]==0);
    BusyWaitScale=0;
    for(uint16_t effect=1;effect<=18;++effect){Sound_Setup(effect);check(SDLBackend_SpeakerControl==0);}
    BusyWaitScale=savedBusyWaitScale;
    PC_Speaker_ON_ptr_freq(1200); PC_speaker_OFF_2();
    SDLBackend_RenderSpeaker(samples,32); check(samples[0]==0);
    SDLBackend_RetracesPerSecond=100000;
    Wait_For_N_Vertical_Retraces(0); Wait_For_N_Vertical_Retraces(2);
    /* Original palette orchestration -> original port redirect -> SDL state. */
    uint8_t palette[EgaPaletteRegisterCount]={0,7,8,15,127,128,255,1,2,3,4,5,6,9,10,11};
    Set_Palette_registers(palette);
    for (i=0;i<EgaPaletteRegisterCount;++i) {
        int16_t colour=(int8_t)palette[i];
        if (colour>7) colour+=8;
        check(SDLBackend_GetEgaPaletteRegister((uint8_t)i)==((uint16_t)colour&0x3F));
    }
    EGA_Set_Palette_Registers_in_out(3,0xFFAB);
    check(SDLBackend_GetEgaPaletteRegister(3)==0x2B);
    /* SDL event -> original signed keyboard WORD. */
    memset(&key,0,sizeof key);key.type=SDL_EVENT_KEY_DOWN;
    key.key.key=SDLK_A; key.key.scancode=SDL_SCANCODE_A;key.key.mod=SDL_KMOD_SHIFT;
    check(SDL_PushEvent(&key));
    check(Check_Input_For_Character()==255);check(Check_Input_For_Character()==255);
    check(SDLBackend_PollQuit()==0);check(Keyboard_GetKey()=='A');
    check(Check_Input_For_Character()==0);
    DOS_Select_Default_Drive(0x0101);check(SDLBackend_DefaultDrive==1);
    DOS_Select_Default_Drive(0xFF00);check(SDLBackend_DefaultDrive==0);
    /* AH06/FF consumes pending input without waiting; never prints FF. */
    check(SDL_PushEvent(&key));
    Platform_Write_Startup_Character(-1);check(Check_Input_For_Character()==0);
    Platform_Write_Startup_Character(0x01FF);check(Check_Input_For_Character()==0);
    /* Sol: standalone enhanced navigation keys must become original negative
     * scan WORDs, not FFE0 (the native emulator capture's lost Down input).
     * Check actual SDL -> DOS wrapper -> original movement conversion. */
    {
        static const SDL_Scancode navigationScans[]={SDL_SCANCODE_UP,SDL_SCANCODE_DOWN,
            SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT,SDL_SCANCODE_HOME,SDL_SCANCODE_END,
            SDL_SCANCODE_PAGEUP,SDL_SCANCODE_PAGEDOWN};
        static const int16_t originalScans[]={-0x48,-0x50,-0x4B,-0x4D,-0x47,-0x4F,-0x49,-0x51};
        static const uint16_t directions[]={COMMAND_MOVE_North,COMMAND_MOVE_South,
            COMMAND_MOVE_West,COMMAND_MOVE_East,COMMAND_MOVE_NorthWest,COMMAND_MOVE_SouthWest,
            COMMAND_MOVE_NorthEast,COMMAND_MOVE_SouthEast};
        key.key.key=SDLK_DOWN; /* non-ASCII: physical scancode must win */
        key.key.mod=0;
        for(unsigned index=0;index<sizeof navigationScans/sizeof navigationScans[0];++index) {
            key.key.scancode=navigationScans[index];
            check(SDL_PushEvent(&key)); check(Keyboard_GetKey()==originalScans[index]);
            check(Check_Input_For_Character()==0);
            check(SDL_PushEvent(&key));
            check(Keyboard_Convert_To_MoveCommands(Keyboard_Get_ASCII_Hex_Input())==directions[index]);
            check(Check_Input_For_Character()==0);
        }
    }
    key.key.key=SDLK_KP_ENTER;key.key.scancode=SDL_SCANCODE_KP_ENTER;
    check(SDL_PushEvent(&key));check(Check_Input_For_Character()==255);
    check(Keyboard_GetKey()=='\r');check(Check_Input_For_Character()==0);
    /* Every keypad digit/navigation code goes through the actual DOS wrapper.
     * A shifted Num Lock state must not turn diagonal movement into a lost key. */
    {
        static const int16_t scans[]={-0x4F,-0x50,-0x51,-0x4B,-0x4C,-0x4D,-0x47,-0x48,-0x49,-0x52};
        static const char digits[]="1234567890";
        static const uint16_t directions[]={
            COMMAND_MOVE_SouthWest,COMMAND_MOVE_South,COMMAND_MOVE_SouthEast,
            COMMAND_MOVE_West,0,COMMAND_MOVE_East,COMMAND_MOVE_NorthWest,
            COMMAND_MOVE_North,COMMAND_MOVE_NorthEast,0};
        const SDL_Keymod modes[]={0,SDL_KMOD_NUM,SDL_KMOD_SHIFT,SDL_KMOD_NUM|SDL_KMOD_SHIFT};
        unsigned mode,index;
        check(DisableInput==FALSE && AttractModeRecordingActive==FALSE);
        for(mode=0;mode<4;++mode) for(index=0;index<10;++index) {
            int16_t expected=(mode==1 || mode==2) ? (int16_t)digits[index] : scans[index];
            key.key.scancode=(SDL_Scancode)(SDL_SCANCODE_KP_1+index);
            key.key.key=SDLK_KP_1; /* translation uses physical keypad position */
            key.key.mod=modes[mode];
            check(SDL_PushEvent(&key));
            check(Keyboard_GetKey()==expected);
            check(Check_Input_For_Character()==0);
            check(SDL_PushEvent(&key));
            check(Keyboard_Convert_To_MoveCommands(Keyboard_Get_ASCII_Hex_Input())==
                (directions[index] ? directions[index] : (uint16_t)expected));
            check(Check_Input_For_Character()==0);
        }
    }
    key.key.key=SDLK_ESCAPE;key.key.scancode=SDL_SCANCODE_ESCAPE;
    check(SDL_PushEvent(&key));check(SDLBackend_PollQuit()==0);check(Keyboard_GetKey()==27);
    memset(&key,0,sizeof key);key.type=SDL_EVENT_QUIT;
    check(SDL_PushEvent(&key));check(Check_Input_For_Character()==255);
    check(SDLBackend_PollQuit()==1);check(Check_Input_For_Character()==0);
    /* Synthetic local fixture only, never original game saves/assets. */
    check(snprintf(path,sizeof path,"chi-sdl-io-%llu.tmp",(unsigned long long)SDL_GetPerformanceCounter())>0);
    handle=Get_FileHandle((uint8_t *)path,0x8000);check(handle==-1);
    handle=Get_FileHandle((uint8_t *)path,0x8101,0x0180);check(handle>=0);
    check(DOS_write_memory_to_save_file((uint16_t)handle,bytes,4)==4);
    check(DOS_close_file((uint16_t)handle)==0);
    handle=Get_FileHandle((uint8_t *)path,0x8101,0x0180);check(handle>=0);
    check(DOS_write_memory_to_save_file((uint16_t)handle,bytes,1)==1);
    check(DOS_close_file((uint16_t)handle)==0);
    handle=Get_FileHandle((uint8_t *)path,0x8000);check(handle>=0);
    check(DOS_set_file_position((uint16_t)handle,0,0,2)==4); /* create didn't truncate */
    check(DOS_set_file_position((uint16_t)handle,0,0,0)==0);
    check(DOS_read_file_handler((uint16_t)handle,readBack,8)==4);
    check(!memcmp(bytes,readBack,4));check(DOS_read_file_handler((uint16_t)handle,readBack,1)==0);
    check(DOS_close_file((uint16_t)handle)==0);check(DOS_close_file((uint16_t)handle)==-1);
    check(remove(path)==0);
    Restore_BIOS_Text_Mode();
    printf("Passed %u original DOS/hardware redirect checks.\n",checks);return 0;
}
