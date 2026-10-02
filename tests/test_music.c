#include "music.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
static unsigned gate, pitchWrites, chained, installed, restored;
static uint16_t pitch;
static void (*timerCallback)(void);
void SDLBackend_SetSpeakerGate(int enabled) { gate=(unsigned)enabled; }
void SDLBackend_SpeakerOff(void) { gate=0; }
void SDLBackend_SetSpeakerDivisor(uint16_t divisor) { pitch=divisor; ++pitchWrites; }
void SDLBackend_InstallMusicTimer(void (*callback)(void),uint16_t divisor)
{
    assert(divisor==0xFFF); timerCallback=callback; ++installed;
}
void SDLBackend_RestoreSystemTimer(uint16_t divisor) { assert(divisor==0xFFFF); ++restored; }
void SDLBackend_ChainSystemTimer(void) { ++chained; }
int main(void)
{
    const uint16_t frequencies[12]={4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902};
    for (unsigned note=0;note<256;++note) {
        uint16_t expected=(uint16_t)(1193182/frequencies[note%12]);
        unsigned shift=(uint8_t)(8-note/12);
        for (unsigned i=0;i<shift;++i) expected=(uint16_t)(expected*2);
        assert(Music_Note_To_Pit_Divisor((uint8_t)note)==expected);
    }
    Install_Music_Timer(); assert(installed==1 && Music_Playback_Finished());
    uint8_t stream[]={60,0,61,0x80,0,0};
    PC_Speaker_Music_Control(MusicControl_Start,stream,4);
    assert(gate==1 && MusicTicksUntilNextNote==1 && !Music_Playback_Finished());
    timerCallback(); assert(pitchWrites==1 && pitch==Music_Note_To_Pit_Divisor(60));
    assert(MusicStreamOffset==1 && chained==1);
    for (unsigned i=0;i<3;++i) timerCallback(); assert(pitchWrites==1);
    timerCallback(); assert(MusicStreamOffset==3 && pitchWrites==2);
    for (unsigned i=0;i<4;++i) timerCallback(); assert(pitch==14 && gate==1 && MusicStreamOffset==4);
    for (unsigned i=0;i<4;++i) timerCallback();
    assert(Music_Playback_Finished() && gate==0 && MusicStreamOffset==4 && pitch==14);
    for (unsigned i=0;i<4;++i) timerCallback(); assert(chained==1);
    timerCallback(); assert(chained==2); /* first, then eighteenth IRQ */
    MusicSystemTickDivider=0xAB01; timerCallback(); assert(MusicSystemTickDivider==0xAB00);
    timerCallback(); assert(MusicSystemTickDivider==16 && chained==3);
    PC_Speaker_Music_Control(MusicControl_Start,stream,256); /* low cadence BYTE zero */
    Music_Pc_Stream_Tick(); assert(MusicStreamOffset==1 && MusicTicksUntilNextNote==0);
    for (unsigned i=0;i<255;++i) Music_Pc_Stream_Tick(); assert(MusicStreamOffset==1);
    Music_Pc_Stream_Tick(); assert(MusicStreamOffset==3);
    PC_Speaker_Music_Control(13); assert(gate==1 && !Music_Playback_Finished());
    PC_Speaker_Music_Control(MusicControl_Stop); assert(gate==0 && Music_Playback_Finished());
    Restore_System_Timer(); assert(restored==1);
    puts("Original PC music pitch, cadence, zero skip/stop, high-bit14, dispatcher arguments and17-IRQ clock passed");
    return 0;
}
