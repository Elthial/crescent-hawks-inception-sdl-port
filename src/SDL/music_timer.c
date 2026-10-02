#include "backend.h"
#include <SDL3/SDL.h>
static void (*musicTimerCallback)(void);
static uint64_t previousTime, accumulatedClockUnits;
static uint16_t musicTimerDivisor;
uint64_t SDLBackend_SystemTimerTicks;
enum { PitInputFrequency=1193182 };

/* Single-threaded replacement for IRQ0: service from input/retrace boundaries.
 * No audio-thread access to game globals and no approximate millisecond cadence.
 * Catch up every elapsed interrupt using the original rational PIT frequency. */
void SDLBackend_InstallMusicTimer(void (*callback)(void),uint16_t divisor)
{
    musicTimerCallback=callback; musicTimerDivisor=divisor;
    previousTime=SDL_GetTicksNS(); accumulatedClockUnits=0;
}
void SDLBackend_ServiceMusicTimer(void)
{
    if (!musicTimerCallback) return;
    uint64_t now=SDL_GetTicksNS(), elapsed=now-previousTime;
    previousTime=now;
    accumulatedClockUnits+=elapsed*PitInputFrequency;
    uint64_t interval=UINT64_C(1000000000)*(musicTimerDivisor?musicTimerDivisor:65536);
    while (musicTimerCallback && accumulatedClockUnits>=interval) {
        accumulatedClockUnits-=interval;
        musicTimerCallback();
    }
}
void SDLBackend_RestoreSystemTimer(uint16_t divisor)
{
    /* Original restores BIOS IRQ0 and FFFF divisor. SDL supplies normal host
     * time independently; remove the music binding, not an emulated DOS vector. */
    (void)divisor;
    musicTimerCallback=NULL; accumulatedClockUnits=0;
}
void SDLBackend_ChainSystemTimer(void) { ++SDLBackend_SystemTimerTicks; }
