#include "game.h"
#include <stdio.h>
#include <stdlib.h>
static uint16_t expected[65536],minimum,delay,toggles;
static unsigned count,calls,timers,stops,mutation;
static void check(int ok){if(!ok){fprintf(stderr,"Sound sweep mismatch call%u/%u\n",calls,count);exit(1);}}
void A_Timer(void){check(!timers && !calls);++timers;}
void A_PC_Speaker_OFF(void){check(timers==1 && !stops && calls==count);++stops;}
void Sound_Play_Sweep_Tone_Delay(uint16_t divisor,uint16_t multiplier){check(calls<count && divisor==expected[calls] && multiplier==delay);++calls;}
void Sound_Freq_Loop(uint16_t mask,uint16_t bits,uint16_t toggleCount){
    check(calls<count && mask==expected[calls] && bits==(uint16_t)(mask-minimum) && toggleCount==toggles);++calls;
    if(mutation && calls==1){NoiseSweepStartMask=20;NoiseSweepStopMask=24;NoiseSweepMaskStep=2;}
}
static void reset(void){count=calls=timers=stops=mutation=0;minimum=0xFFF0;delay=7;toggles=0x8000;}
int main(void){
    for(unsigned descending=0;descending<2;++descending)for(unsigned step=1;step<8;++step)for(unsigned length=0;length<32;++length){
        reset();uint16_t start=(uint16_t)(descending?100+length:100),stop=(uint16_t)(descending?100:100+length);
        for(unsigned advance=0;advance<length;advance+=step)expected[count++]=(uint16_t)(descending?start-advance:start+advance);
        if(descending)Sound_Streaming_1(start,stop,minimum,toggles,(uint16_t)step);
        else Sound_Streaming_2(start,stop,minimum,toggles,(uint16_t)step);
        check(calls==count && !timers && !stops);
    }
    reset();Sound_Streaming_1(0x8000,0x7FFF,minimum,toggles,1);check(!calls);
    reset();Sound_Streaming_2(0x7FFF,0x8000,minimum,toggles,1);check(!calls);
    reset();expected[0]=0x800A;expected[1]=0x8008;expected[2]=0x8006;expected[3]=0x8004;expected[4]=0x8002;count=5;
    Sound_Streaming_1(0x800A,0x8000,minimum,toggles,2);check(calls==count);
    /* Skipping the signed minimum wraps positive and continues until the
     * stop is eventually reached. Do not impose a monotonic/clamped limit. */
    reset();
    for(unsigned index=0;index<65536;++index){uint16_t mask=(uint16_t)(0x800A-index*3);
        if(mask==0x8000)break;expected[count++]=mask;}
    check(count==43694);
    Sound_Streaming_1(0x800A,0x8000,minimum,toggles,3);check(calls==count);
    reset();expected[0]=0xFFF0;expected[1]=0xFFF3;expected[2]=0xFFF6;count=3;
    Sound_Streaming_2(0xFFF0,0xFFF8,minimum,toggles,3);check(calls==count);
    reset();mutation=1;expected[0]=10;expected[1]=22;count=2;
    Sound_Streaming_2(10,100,minimum,toggles,1);check(calls==count);
    for(unsigned span=0;span<32;++span)for(unsigned step=1;step<6;++step)for(unsigned repeat=0;repeat<4;++repeat){
        reset();
        for(unsigned sweep=0;sweep<repeat;++sweep)for(unsigned offset=0;offset<span*2;offset+=step)
            expected[count++]=(uint16_t)(1-span+offset);
        Sound_Play_Divisor_Sweep(1,(uint16_t)span,delay,(uint16_t)repeat,(uint16_t)step);
        check(calls==count && timers==1 && stops==1);
    }
    reset();Sound_Play_Divisor_Sweep(100,0x4000,delay,1,1);check(!calls && timers==1 && stops==1);
    reset();Sound_Play_Divisor_Sweep(100,10,delay,0x8000,1);check(!calls && timers==1 && stops==1);
    reset();Sound_Play_Divisor_Sweep(100,0,delay,1,0);check(!calls && timers==1 && stops==1);
    puts("Original signed noise and wrapped divisor sweep arithmetic verified.");return 0;
}
