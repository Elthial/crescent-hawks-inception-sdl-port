#include "game.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned cursor,calls;
static void check(int ok){if(!ok){fprintf(stderr,"Sound command mismatch at WORD%u\n",cursor);exit(1);}}
static void command(unsigned algorithm){
    unsigned marker=SoundLibrary[cursor++];check(algorithm==(marker<=1000?0:marker-1000));
    if(!algorithm){check(SoundCommandOrRepeatCount==marker);check(FixedTonePitDivisor==SoundLibrary[cursor++]);check(FixedToneDelayMultiplier==SoundLibrary[cursor++]);}
    else{
        check(SoundCommandOrRepeatCount==SoundLibrary[cursor++]);
        if(algorithm==1){check(NoiseCountdownMask==SoundLibrary[cursor++]);check(NoiseMinimumDelayBits==SoundLibrary[cursor++]);check(NoiseToggleCount==SoundLibrary[cursor++]);check(NoiseToggleDelaySeed==SoundLibrary[cursor++]);}
        else if(algorithm==2){check(SweepCentrePitDivisor==SoundLibrary[cursor++]);check(SweepHalfSpan==SoundLibrary[cursor++]);check(SweepToneDelayMultiplier==SoundLibrary[cursor++]);check(SweepRepeatCount==SoundLibrary[cursor++]);check(SweepDivisorStep==SoundLibrary[cursor++]);}
        else{check(NoiseSweepStartMask==SoundLibrary[cursor++]);check(NoiseSweepStopMask==SoundLibrary[cursor++]);check(NoiseSweepMinimumBitsSubtract==SoundLibrary[cursor++]);check(NoiseSweepToggleCount==SoundLibrary[cursor++]);check(NoiseSweepMaskStep==SoundLibrary[cursor++]);}
    }
    ++calls;
}
void Sound_Play_Fixed_Tone_Repetitions(void){command(0);}
void Sound_Play_Seeded_Noise_Repetitions(void){command(1);}
void Sound_Play_Divisor_Sweep_Repetitions(void){command(2);}
void Sound_Play_Descending_Noise_Sweep(void){command(3);}
void Sound_Play_Ascending_Noise_Sweep(void){command(4);}
int main(void){
    const unsigned offsets[18]={0,10,20,30,40,50,71,88,112,122,132,156,173,200,252,273,294,304};
    const unsigned counts[18]={1,1,1,1,1,6,2,3,1,1,3,2,4,7,6,6,1,1};
    for(unsigned effect=0;effect<18;++effect){cursor=offsets[effect];calls=0;
        Sound_Setup((uint16_t)(effect+1));check(calls==counts[effect]);
        check(SoundLibrary[cursor]==1 && !SoundLibrary[cursor+1] && !SoundLibrary[cursor+2]);
        check(cursor+3==(effect<17?offsets[effect+1]:SoundLibraryWords));
    }
    puts("All18 original sound effects dispatch their exact command parameters.");return 0;
}
