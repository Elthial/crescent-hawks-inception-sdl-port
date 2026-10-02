#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned prepared,togglesSeen,delaysSeen;
static uint16_t seedExpected;
static void check(int ok){if(!ok){fprintf(stderr,"Noise contract mismatch\n");exit(1);}}
void PC_speaker_OFF_2(void){check(!prepared && !togglesSeen && !delaysSeen);++prepared;}
uint16_t PC_Speaker_XOR_ptr_freq(uint16_t seed){check(prepared==1 && seed==seedExpected);++togglesSeen;return 0xFACE;}
void Sound_Freq_Countdown_Loop(uint16_t state,uint16_t mask,uint16_t minimum){check(prepared==1 && togglesSeen && state==0xFACE && mask==0x1234 && minimum==0x4321);++delaysSeen;}
static void run(unsigned seeded,uint16_t scale,uint16_t toggles){
    prepared=togglesSeen=delaysSeen=0;BusyWaitScale=scale;seedExpected=(uint16_t)(seeded?321:1000);
    if(seeded)Sound_Play_Seeded_Noise_Toggles(0x1234,0x4321,toggles,321);
    else Sound_Freq_Loop(0x1234,0x4321,toggles);
    unsigned expected=(int16_t)toggles>0?toggles:0;
    check(prepared==1 && togglesSeen==expected && delaysSeen==expected*((int16_t)scale>0?(unsigned)scale:0u));
    check(NoiseCountdownMask==0x1234 && NoiseMinimumDelayBits==0x4321 && NoiseToggleCount==toggles && NoiseToggleDelaySeed==seedExpected);
}
int main(void){
    for(unsigned seeded=0;seeded<2;++seeded){
        for(unsigned scale=0;scale<9;++scale)for(unsigned toggles=0;toggles<9;++toggles)run(seeded,(uint16_t)scale,(uint16_t)toggles);
        for(unsigned negative=0x8000;negative<65536;++negative)run(seeded,5,(uint16_t)negative);
        run(seeded,0x8000,10);run(seeded,0xFFFF,10);run(seeded,0,0x7FFF);
    }
    puts("Original noise loops preserve signed limits and unchanged DX handoff.");return 0;
}
