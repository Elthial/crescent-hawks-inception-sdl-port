#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned calls,mutate;
static uint16_t seen;
static void check(int ok){if(!ok){fprintf(stderr,"Tone contract mismatch\n");exit(1);}}
void PC_Speaker_ON_ptr_freq(uint16_t divisor){seen=divisor;++calls;
    if(mutate){FixedToneDelayMultiplier=SweepCurrentDelayMultiplier=0xFFFF;}}
int main(void){
    check(BusyWaitScale==5);BusyWaitScale=0;
    for(unsigned divisor=0;divisor<65536;++divisor){
        calls=0;Sound_Play_Fixed_Tone_Delay((uint16_t)divisor,0xABCD);
        check(calls==(unsigned)(divisor!=0) && FixedTonePitDivisor==divisor && FixedToneDelayMultiplier==0xABCD);
        if(divisor)check(seen==divisor);
        calls=0;Sound_Play_Sweep_Tone_Delay((uint16_t)divisor,0xFEDC);
        check(calls==1 && seen==divisor && SweepCurrentPitDivisor==divisor && SweepCurrentDelayMultiplier==0xFEDC);
    }
    /* Exercise terminating positive, negative and low-WORD wrapping products.
     * No assertion about CPU elapsed time or loop-count observability. */
    const uint16_t scales[]={0,1,5,0x8000,0xFFFF},multipliers[]={0,1,7,0x8000,0xFFFF};
    for(unsigned scale=0;scale<5;++scale)for(unsigned multiplier=0;multiplier<5;++multiplier){
        BusyWaitScale=scales[scale];calls=0;
        Sound_Play_Fixed_Tone_Delay(123,multipliers[multiplier]);check(calls==1);
        Sound_Play_Sweep_Tone_Delay(0,multipliers[multiplier]);check(calls==2 && seen==0);
    }
    mutate=1;BusyWaitScale=5;Sound_Play_Fixed_Tone_Delay(123,7);check(FixedToneDelayMultiplier==0xFFFF);
    Sound_Play_Sweep_Tone_Delay(123,7);check(SweepCurrentDelayMultiplier==0xFFFF);
    puts("Original fixed-tone divisor and shared-state contracts verified.");return 0;
}
