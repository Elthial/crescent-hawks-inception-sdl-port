#include "game.h"

/* Sol: original1FC5:0643..06F5,0747..086C. Expanded ASM and ascending
 * .dis tail checked. Retain low-WORD products, signed strict comparisons,
 * live globals and independent sweep counters. Zero steps with a continuing
 * condition remain nonterminating, not sanitized into successful playback. */
void Sound_Play_Divisor_Sweep(uint16_t centre,uint16_t halfSpan,uint16_t delayMultiplier,uint16_t repetitions,uint16_t step)
{
    SweepCentrePitDivisor=centre;SweepHalfSpan=halfSpan;
    SweepToneDelayMultiplier=delayMultiplier;SweepRepeatCount=repetitions;SweepDivisorStep=step;
    A_Timer();
    for(uint16_t sweep=0;(int16_t)SweepRepeatCount>(int16_t)sweep;++sweep) {
        for(uint16_t position=0;;++position) {
            uint16_t offset=(uint16_t)((uint32_t)position*SweepDivisorStep);
            if((int16_t)(uint16_t)(SweepHalfSpan*2)<=(int16_t)offset) break;
            uint16_t divisor=(uint16_t)(SweepCentrePitDivisor-SweepHalfSpan+offset);
            Sound_Play_Sweep_Tone_Delay(divisor,SweepToneDelayMultiplier);
        }
    }
    A_PC_Speaker_OFF();
}
void Sound_Streaming_1(uint16_t startMask,uint16_t stopMask,uint16_t minimumSubtract,uint16_t toggles,uint16_t step)
{
    NoiseSweepStartMask=startMask;NoiseSweepStopMask=stopMask;
    NoiseSweepMinimumBitsSubtract=minimumSubtract;NoiseSweepToggleCount=toggles;NoiseSweepMaskStep=step;
    for(uint16_t position=0;;++position) {
        uint16_t offset=(uint16_t)((uint32_t)position*NoiseSweepMaskStep);
        uint16_t mask=(uint16_t)(NoiseSweepStartMask-offset);
        if((int16_t)mask<=(int16_t)NoiseSweepStopMask) break;
        Sound_Freq_Loop(mask,(uint16_t)(mask-NoiseSweepMinimumBitsSubtract),NoiseSweepToggleCount);
    }
}
void Sound_Streaming_2(uint16_t startMask,uint16_t stopMask,uint16_t minimumSubtract,uint16_t toggles,uint16_t step)
{
    NoiseSweepStartMask=startMask;NoiseSweepStopMask=stopMask;
    NoiseSweepMinimumBitsSubtract=minimumSubtract;NoiseSweepToggleCount=toggles;NoiseSweepMaskStep=step;
    for(uint16_t position=0;;++position) {
        uint16_t offset=(uint16_t)((uint32_t)position*NoiseSweepMaskStep);
        uint16_t mask=(uint16_t)(NoiseSweepStartMask+offset);
        if((int16_t)mask>=(int16_t)NoiseSweepStopMask) break;
        Sound_Freq_Loop(mask,(uint16_t)(mask-NoiseSweepMinimumBitsSubtract),NoiseSweepToggleCount);
    }
}
