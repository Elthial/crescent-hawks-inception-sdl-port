# DOSBox-X audio comparison (1510 cycles/ms)

The supplied `btech_003.wav` and `btech_000.wav` are private reference recordings. The clips and spectrograms below live only in ignored `chinception/validation/sound-analysis/`; do not commit or redistribute them.

## Mech beam / laser

The combat effects path plays original sound ID **2** for a Mech beam; ID 9 is used for *personnel* lasers. The first repeated descending chirp in `btech_003.wav` was clipped from 2.00 to 2.85 seconds as `dosbox-laser.wav`. The equivalent C17 ID-2 effect was recorded through SDL's disk audio device as `c17-id2-calibrated/c17-effect-02.wav` at 1510 cycles/ms.

Before correction, the SDL PIT oscillator phase reset on each divisor write. The sweep rapidly reached a low pitch whose half-cycle was longer than each individual delay; repeated phase resets then held the speaker at one level, creating a long drone. Retaining phase across divisor writes restores the descending pitch curve. The prior provisional sweep delay also made the effect about 1.4 seconds long; the reference chirp is roughly 0.35 seconds. The sweep delay is now calibrated independently; fixed-tone and noise delays are **not** yet calibrated.

Approximate dominant pitch in successive 40-ms windows (Hz):

| Reference time after clip start | DOSBox-X | C17 time after start | C17 |
| ---: | ---: | ---: | ---: |
| 0.10 s | 531 | 0.08 s | 583 |
| 0.15 s | 290 | 0.13 s | 306 |
| 0.20 s | 197 | 0.18 s | 205 |
| 0.25 s | 150 | 0.23 s | 154 |
| 0.30 s | 120 | 0.28 s | 123 |
| 0.35 s | 101 | 0.33 s | 103 |

This is a close *pitch and duration* match after an approximately 20-ms onset alignment, not a claim of sample-identical timbre or a match for all other effects. The reference and candidate WAVs and spectrograms are available in the private folder for listening and visual inspection.

## Intro excerpt

The supplied `btech_000.wav` is preserved privately as `dosbox-intro.wav`; the first four seconds and a four-second section from the earlier C17 intro capture are clipped as `dosbox-intro-4s.wav` and `c17-intro-4s.wav`, with corresponding spectrograms. These sections have **not** been established as the same musical phrase: the supplied recording can begin mid-song, whereas the C17 capture includes its own startup timing. The provisional comparison shows substantially different active-tone occupancy (about 64% versus 26% of 10-ms windows above 2000 RMS), but that figure cannot identify a music-code fault without phrase alignment and confirmation of the DOSBox-X sound-device setting. No SIF or intro-timer changes are justified by this comparison yet. The sound-effect sweep correction does not change the music timer or note conversion.

Next focused audio task: capture the same opening phrase from both programs with the DOSBox-X sound-device setting recorded; then align by note transitions before touching the intro path. Separately capture a known missile, machine-gun and Mech-destruction action to calibrate their fixed-tone/noise paths rather than applying the beam-sweep factor to them.
