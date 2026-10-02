#include "game.h"
#include "dos.h"
#ifdef CHI_SDL_SOUND_TIMING
#include "backend.h"
#endif

/* Sol: complete original1FC5:047B..04E3 and06F6..0746, ASM checked.
 * Low-WORD signed products and live global reloads are retained. SDL builds
 * redirect each physical delay iteration to retained timed speaker samples;
 * headless rule builds omit physical output. Timing is an explicit portable
 * approximation calibrated to the selected DOSBox-compatible CPU profile. */
void Sound_Play_Fixed_Tone_Delay(uint16_t divisor,uint16_t delayMultiplier)
{
    enum { FixedToneDelayPassCount=50 };
    FixedTonePitDivisor=divisor;FixedToneDelayMultiplier=delayMultiplier;
    if(FixedTonePitDivisor) PC_Speaker_ON_ptr_freq(FixedTonePitDivisor);
    /* Divisor0 leaves current speaker state alone; it does not disable it. */
    for(uint16_t pass=0;pass<FixedToneDelayPassCount;++pass) {
        for(uint16_t iteration=0;
            (int16_t)(uint16_t)((uint32_t)BusyWaitScale*FixedToneDelayMultiplier)>(int16_t)iteration;
            ++iteration) {
#ifdef CHI_SDL_SOUND_TIMING
            SDLBackend_ToneDelayIteration(FALSE); /* Sol: physical delay -> SDL; native signed loop unchanged. */
#endif
        }
    }
}
void Sound_Play_Sweep_Tone_Delay(uint16_t divisor,uint16_t delayMultiplier)
{
    SweepCurrentPitDivisor=divisor;SweepCurrentDelayMultiplier=delayMultiplier;
    PC_Speaker_ON_ptr_freq(SweepCurrentPitDivisor); /*0 means65536 PIT ticks*/
    for(uint16_t iteration=0;
        (int16_t)(uint16_t)((uint32_t)BusyWaitScale*SweepCurrentDelayMultiplier)>(int16_t)iteration;
        ++iteration) {
#ifdef CHI_SDL_SOUND_TIMING
        SDLBackend_ToneDelayIteration(TRUE); /* Sol: physical delay -> SDL; native signed loop unchanged. */
#endif
    }
}
