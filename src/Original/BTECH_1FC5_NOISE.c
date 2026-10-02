#include "game.h"
#include "dos.h"
/* Sol: complete1FC5:04F1..0642, ASM checked. Native DX delay state from
 * 007D is an explicit returned WORD passed to00A9, which does not modify it.
 * No undefined CPU-register global or new research algorithm is required. */
void Sound_Freq_Loop(uint16_t mask, uint16_t minimumBits, uint16_t toggles)
{
	// Sol:007D's evolved DX survives every00A9 call as the explicit delayState WORD.
	NoiseCountdownMask = mask; // Sol:countdown mask, not a frequency/pointer
	NoiseMinimumDelayBits = minimumBits;
	NoiseToggleCount = toggles;
	NoiseToggleDelaySeed = SoundNoiseDefaultToggleSeed;
	PC_speaker_OFF_2(); // Sol:clear port61 bits0/1 before direct bit1 toggles; no PIT divisor write
	for (uint16_t warmup = 1; (int16_t)warmup < (int16_t)BusyWaitScale; ++warmup)
		; // Sol:WORD warmup starts1; signed comparison, calibration reloaded
	for (uint16_t toggle = 0; (int16_t)NoiseToggleCount > (int16_t)toggle; ++toggle)
	{
		uint16_t delayState=PC_Speaker_XOR_ptr_freq(NoiseToggleDelaySeed); // Sol:007D toggles bit1 and leaves evolved delay state in DX for00A9, not a PIT frequency update
		for (uint16_t countdown = 0; (int16_t)countdown < (int16_t)BusyWaitScale; ++countdown)
			Sound_Freq_Countdown_Loop(delayState,NoiseCountdownMask,NoiseMinimumDelayBits); // Sol:mask/minimum WORD values do not overwrite either counter; implicit DX binding retained
	}
	// Sol:No speaker-off at this helper's exit; the parent wrapper owns final cleanup.
}

// 1FC5:059A: void Sound_Play_Seeded_Noise_Toggles(Stack int16 wArg94, Stack int16 wArg06, Stack int16 wArg08, Stack int16 wArg0A)
// Called from:
//      Sound_Play_Seeded_Noise_Repetitions
// Sol: Summary: Generate a seeded masked-delay noise sequence.
// Sol: Outline: Seed the delay state, store the parameters, and repeat the native countdown and speaker-gate loop.
void Sound_Play_Seeded_Noise_Toggles(uint16_t mask, uint16_t minimumBits, uint16_t toggles, uint16_t delaySeed)
{
	NoiseCountdownMask = mask; // Sol:countdown mask, not a frequency/pointer
	NoiseMinimumDelayBits = minimumBits;
	NoiseToggleCount = toggles;
	NoiseToggleDelaySeed = delaySeed;
	PC_speaker_OFF_2(); // Sol:clear port61 bits0/1 before direct bit1 toggles; no PIT divisor write
	for (uint16_t warmup = 1; (int16_t)warmup < (int16_t)BusyWaitScale; ++warmup)
		; // Sol:WORD warmup starts1; signed comparison, calibration reloaded
	for (uint16_t toggle = 0; (int16_t)NoiseToggleCount > (int16_t)toggle; ++toggle)
	{
		uint16_t delayState=PC_Speaker_XOR_ptr_freq(NoiseToggleDelaySeed); // Sol:007D toggles bit1 and leaves evolved delay state in DX for00A9, not a PIT frequency update
		for (uint16_t countdown = 0; (int16_t)countdown < (int16_t)BusyWaitScale; ++countdown)
			Sound_Freq_Countdown_Loop(delayState,NoiseCountdownMask,NoiseMinimumDelayBits); // Sol:mask/minimum WORD values do not overwrite either counter; implicit DX binding retained
	}
	// Sol:No speaker-off at this helper's exit; the parent wrapper owns final cleanup.
}
