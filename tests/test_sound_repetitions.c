#include "game.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned calls,timers,stops,selected,mutate;
static uint16_t parameter;
static void check(int ok){if(!ok){fprintf(stderr,"Sound repetition mismatch generator%u call%u\n",selected,calls);exit(1);}}
void A_Timer(void){check(!timers && !calls && !stops);++timers;}
void A_PC_Speaker_OFF(void){check(timers==1 && !stops);++stops;}
static void called(unsigned generator){
    check(selected==generator && timers==1 && !stops);++calls;
    if(mutate && calls==1){SoundCommandOrRepeatCount=2;parameter=0xABCD;
        FixedTonePitDivisor=NoiseCountdownMask=SweepCentrePitDivisor=NoiseSweepStartMask=parameter;}
}
void Sound_Play_Fixed_Tone_Delay(uint16_t divisor,uint16_t delay){check(divisor==parameter && delay==2);called(0);}
void Sound_Play_Seeded_Noise_Toggles(uint16_t mask,uint16_t minimum,uint16_t toggles,uint16_t seed){check(mask==parameter && minimum==2 && toggles==3 && seed==4);called(1);}
void Sound_Play_Divisor_Sweep(uint16_t centre,uint16_t span,uint16_t delay,uint16_t repetitions,uint16_t step){check(centre==parameter && span==2 && delay==3 && repetitions==4 && step==5);called(2);}
void Sound_Streaming_1(uint16_t start,uint16_t stop,uint16_t subtract,uint16_t toggles,uint16_t step){check(start==parameter && stop==2 && subtract==3 && toggles==4 && step==5);called(3);}
void Sound_Streaming_2(uint16_t start,uint16_t stop,uint16_t subtract,uint16_t toggles,uint16_t step){check(start==parameter && stop==2 && subtract==3 && toggles==4 && step==5);called(4);}
static void run(void (*parent)(void),uint16_t repeat,unsigned mutation){
    SoundCommandOrRepeatCount=repeat;calls=timers=stops=0;mutate=mutation;parameter=1;
    FixedTonePitDivisor=NoiseCountdownMask=SweepCentrePitDivisor=NoiseSweepStartMask=1;
    FixedToneDelayMultiplier=NoiseMinimumDelayBits=SweepHalfSpan=NoiseSweepStopMask=2;
    NoiseToggleCount=SweepToneDelayMultiplier=NoiseSweepMinimumBitsSubtract=3;
    NoiseToggleDelaySeed=SweepRepeatCount=NoiseSweepToggleCount=4;
    SweepDivisorStep=NoiseSweepMaskStep=5;
    parent();check(timers==1 && stops==1);
    check(calls==(mutation?2u:((int16_t)repeat>0?(unsigned)repeat:0u)));
}
int main(void){
    void (*parents[5])(void)={Sound_Play_Fixed_Tone_Repetitions,Sound_Play_Seeded_Noise_Repetitions,Sound_Play_Divisor_Sweep_Repetitions,Sound_Play_Descending_Noise_Sweep,Sound_Play_Ascending_Noise_Sweep};
    for(selected=0;selected<5;++selected){
        for(unsigned repeat=0;repeat<=64;++repeat)run(parents[selected],(uint16_t)repeat,0);
        run(parents[selected],0x7FFF,0);
        for(unsigned repeat=0x8000;repeat<=0xFFFF;++repeat)run(parents[selected],(uint16_t)repeat,0);
        run(parents[selected],10,1);
    }
    puts("Five original sound parents preserve signed repeats and live parameter reads.");return 0;
}
