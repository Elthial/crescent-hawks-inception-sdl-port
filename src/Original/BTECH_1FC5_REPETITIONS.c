#include "game.h"

/* Sol: complete original1FC5:02A3..046D, checked against expanded ASM.
 * Each parent has its own signed WORD loop, rereading shared limits/params.
 * Timer setup and speaker-off occur even when the loop does not run. */
void Sound_Play_Fixed_Tone_Repetitions(void)
{
    A_Timer();
    uint16_t repetition=0;
    while((int16_t)SoundCommandOrRepeatCount>(int16_t)repetition) {
        Sound_Play_Fixed_Tone_Delay(FixedTonePitDivisor,FixedToneDelayMultiplier);
        ++repetition;
    }
    A_PC_Speaker_OFF();
}
void Sound_Play_Seeded_Noise_Repetitions(void)
{
    A_Timer();
    uint16_t repetition=0;
    while((int16_t)SoundCommandOrRepeatCount>(int16_t)repetition) {
        Sound_Play_Seeded_Noise_Toggles(NoiseCountdownMask,NoiseMinimumDelayBits,NoiseToggleCount,NoiseToggleDelaySeed);
        ++repetition;
    }
    A_PC_Speaker_OFF();
}
void Sound_Play_Divisor_Sweep_Repetitions(void)
{
    A_Timer();
    uint16_t repetition=0;
    while((int16_t)SoundCommandOrRepeatCount>(int16_t)repetition) {
        Sound_Play_Divisor_Sweep(SweepCentrePitDivisor,SweepHalfSpan,SweepToneDelayMultiplier,SweepRepeatCount,SweepDivisorStep);
        ++repetition;
    }
    A_PC_Speaker_OFF();
}
void Sound_Play_Descending_Noise_Sweep(void)
{
    A_Timer();
    uint16_t repetition=0;
    while((int16_t)SoundCommandOrRepeatCount>(int16_t)repetition) {
        Sound_Streaming_1(NoiseSweepStartMask,NoiseSweepStopMask,NoiseSweepMinimumBitsSubtract,NoiseSweepToggleCount,NoiseSweepMaskStep);
        ++repetition;
    }
    A_PC_Speaker_OFF();
}
void Sound_Play_Ascending_Noise_Sweep(void)
{
    A_Timer();
    uint16_t repetition=0;
    while((int16_t)SoundCommandOrRepeatCount>(int16_t)repetition) {
        Sound_Streaming_2(NoiseSweepStartMask,NoiseSweepStopMask,NoiseSweepMinimumBitsSubtract,NoiseSweepToggleCount,NoiseSweepMaskStep);
        ++repetition;
    }
    A_PC_Speaker_OFF();
}
