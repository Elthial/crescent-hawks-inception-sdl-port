#include "music.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
    assert(SDLBackend_Open());
    uint8_t stream[]={60,61,62,0,0};
    uint64_t previousSystemTicks=SDLBackend_SystemTimerTicks;
    Install_Music_Timer();
    PC_Speaker_Music_Control(MusicControl_Start,stream,4);
    SDLBackend_Delay(80);
    SDLBackend_InputPending(); /* actual input boundary services elapsed IRQs */
    assert(Music_Playback_Finished() && MusicStreamOffset==3);
    assert(SDLBackend_SystemTimerTicks>=previousSystemTicks+2);
    int16_t samples[32]; SDLBackend_RenderSpeaker(samples,32);
    for (unsigned i=0;i<32;++i) assert(samples[i]==0);
    Restore_System_Timer();
    uint64_t stoppedTicks=SDLBackend_SystemTimerTicks;
    SDLBackend_Delay(20); SDLBackend_ServiceMusicTimer();
    assert(SDLBackend_SystemTimerTicks==stoppedTicks);
    SDLBackend_Close();
    puts("SDL elapsed PIT interrupts drive original music through termination and timer removal");
    return 0;
}
