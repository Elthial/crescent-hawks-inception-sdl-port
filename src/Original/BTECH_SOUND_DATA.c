#include "game.h"
uint16_t BusyWaitScale=5; /*EXE-owned3EDB:5006*/
uint16_t SweepCurrentPitDivisor; /*3092:3FF6*/
uint16_t SweepCurrentDelayMultiplier; /*3092:3246*/
uint16_t SoundCommandOrRepeatCount;
uint16_t FixedToneDelayMultiplier;
uint16_t NoiseCountdownMask;
uint16_t NoiseMinimumDelayBits;
uint16_t NoiseToggleCount;
uint16_t NoiseToggleDelaySeed;
uint16_t SweepCentrePitDivisor;
uint16_t SweepHalfSpan;
uint16_t SweepToneDelayMultiplier;
uint16_t SweepRepeatCount;
uint16_t SweepDivisorStep;
uint16_t NoiseSweepStartMask;
uint16_t NoiseSweepStopMask;
uint16_t NoiseSweepMinimumBitsSubtract;
uint16_t NoiseSweepToggleCount;
uint16_t NoiseSweepMaskStep;
/* Sol: EXE-owned313 WORD sound stream3EDB:5008..5279, unchanged. */
const uint16_t SoundLibrary[SoundLibraryWords]={
    1002,1,1000,500,100,1,10,1,0,0,1002,1,6000,7000,10,1,
    10,1,0,0,1002,5,10,800,20,3,50,1,0,0,1004,1,
    50,1000,5,5,175,1,0,0,1004,1,10,50,5,5,1,1,
    0,0,1,2000,2,2,3000,2,2,4000,2,1,5000,2,1,7000,
    2,1,10000,2,1,0,0,1004,1,50,1000,5,5,40,1004,1,
    10,2000,10,5,50,1,0,0,1004,1,500,1000,2,2,10,1004,
    1,500,1500,2,2,20,1004,1,500,2000,2,2,40,1,0,0,
    1003,4,500,400,300,1,1,1,0,0,1002,20,100,50,1,100,
    40,1,0,0,1002,10,10,10,10,10,10,1002,10,30,10,10,
    10,10,1002,10,50,10,10,10,10,1,0,0,1004,1,1400,1800,
    1,5,150,1003,1,1200,500,1,5,20,1,0,0,1001,10,200,
    100,1,10,1001,10,300,100,1,10,1001,10,400,100,1,10,1001,
    10,500,100,1,10,1,0,0,1003,1,900,875,1,8,40,1004,
    1,850,900,1,8,40,1003,1,900,875,1,8,40,1004,1,850,
    900,1,8,40,1003,1,900,875,1,8,40,1004,1,850,900,1,
    8,40,1004,1,900,1300,1,8,80,1,0,0,20,830,1,20,
    680,1,20,600,1,20,830,1,20,680,1,20,600,1,1,0,
    0,30,550,1,30,450,1,30,390,1,30,500,1,30,400,1,
    30,340,1,1,0,0,1003,40,150,20,10,2,10,1,0,0,
    1001,5,4000,2000,1,1,1,0,0,
};
