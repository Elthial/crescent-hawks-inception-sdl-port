#include "game.h"
#ifdef CHI_SDL_SOUND_TIMING
#include "backend.h"
#endif

/* Sol: complete original1FC5:0002..02A2, ASM checked. Native one-based
 * IDs and valid EXE stream addresses are the contract; no new ID guards. */
void Sound_Setup(uint16_t soundId)
{
#ifdef CHI_SDL_SOUND_TIMING
    SDLBackend_BeginSoundEffect(); /* Sol: retain physical output, not a second command interpreter. */
#endif
	// Sol:Fresh audit scope:valid original313-WORD stream and its one-based IDs; array syntax does not certify malformed/out-of-range native DS reads.
	// Sol: The original decrements the stack argument with DEC WORD PTR [BP+06].
	uint16_t soundsToSkip = soundId - 1;
	uint16_t wordIndex = 0; // Sol:BP-4 WORD cursor, native byte address5008+index*2; offsets below are WORD indices

	while (SoundLibrary[wordIndex] != 0x00 && soundsToSkip != 0x00)
	{
		if (SoundLibrary[wordIndex + 1] == 0x00 && SoundLibrary[wordIndex + 2] == 0x00)
			soundsToSkip--;

		// Sol:0002 signed JG1000 distinguishes direct records from command markers.
		uint16_t soundValue = SoundLibrary[wordIndex];

		if ((int16_t)soundValue <= SoundStream_CommandBase)
			wordIndex += 0x03;
		else if (soundValue == 0x03E9) // Sol:1001: repetition + four seeded-noise parameters (six WORDs including marker)
			wordIndex += 0x06;
		else if (soundValue == 0x03EA || (soundValue == 0x03EB || soundValue == 0x03EC)) // Sol:1002 divisor sweep;1003/1004 descending/ascending noise-mask sweep (seven WORDs)
			wordIndex += 0x07;
		else
			wordIndex++;
	}
	// Sol:Separator examines following WORDs, not command itself. No new bounds/ID guards.
	if (soundsToSkip == 0x00)
	{
		while (SoundLibrary[wordIndex + 1] != 0x00 || SoundLibrary[wordIndex + 2] != 0x00)
		{
			SoundCommandOrRepeatCount = SoundLibrary[wordIndex];
			wordIndex++;
	
			if ((int16_t)SoundCommandOrRepeatCount <= SoundStream_CommandBase)
			{
				FixedTonePitDivisor = SoundLibrary[wordIndex];
				FixedToneDelayMultiplier = SoundLibrary[wordIndex + 0x01];
				Sound_Play_Fixed_Tone_Repetitions();
				wordIndex += 0x02;
			}
			else
			{
				SoundCommandOrRepeatCount -= SoundStream_CommandBase;
				if (SoundCommandOrRepeatCount == 0x01)
				{
					SoundCommandOrRepeatCount = SoundLibrary[wordIndex];
					NoiseCountdownMask = SoundLibrary[wordIndex + 0x01];
					NoiseMinimumDelayBits = SoundLibrary[wordIndex + 0x02];
					NoiseToggleCount = SoundLibrary[wordIndex + 0x03];
					NoiseToggleDelaySeed = SoundLibrary[wordIndex + 0x04];
					Sound_Play_Seeded_Noise_Repetitions();
					wordIndex += 0x05;
				}
				else
				{
					if (SoundCommandOrRepeatCount == 0x02)
					{
						SoundCommandOrRepeatCount = SoundLibrary[wordIndex];
						SweepCentrePitDivisor = SoundLibrary[wordIndex + 0x01];
						SweepHalfSpan = SoundLibrary[wordIndex + 0x02];
						SweepToneDelayMultiplier = SoundLibrary[wordIndex + 0x03];
						SweepRepeatCount = SoundLibrary[wordIndex + 0x04];
						SweepDivisorStep = SoundLibrary[wordIndex + 0x05];
						Sound_Play_Divisor_Sweep_Repetitions();
						wordIndex += 0x06;
					}
					else
					{
						if (SoundCommandOrRepeatCount == 0x03 || SoundCommandOrRepeatCount == 0x04)
						{
							uint16_t repeatCount = SoundLibrary[wordIndex];
							NoiseSweepStartMask = SoundLibrary[wordIndex + 0x01];
							NoiseSweepStopMask = SoundLibrary[wordIndex + 0x02];
							NoiseSweepMinimumBitsSubtract = SoundLibrary[wordIndex + 0x03];
							NoiseSweepToggleCount = SoundLibrary[wordIndex + 0x04]; // Sol:toggle count passed through0747/07DA to04F1, not a sound ID
							NoiseSweepMaskStep = SoundLibrary[wordIndex + 0x05];
							wordIndex += 0x06;

							if (SoundCommandOrRepeatCount == 0x03)
							{
								SoundCommandOrRepeatCount = repeatCount;
								Sound_Play_Descending_Noise_Sweep();
							}
							else
							{
								SoundCommandOrRepeatCount = repeatCount;
								Sound_Play_Ascending_Noise_Sweep();
							}
						}
					}
				}
			}
		}
	}
#ifdef CHI_SDL_SOUND_TIMING
    SDLBackend_EndSoundEffect();
#endif
}
