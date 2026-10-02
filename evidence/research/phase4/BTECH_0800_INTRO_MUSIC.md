# `BTECH_0800` intro-music controller

## Review boundary

This block covers `0800:476D-48B6`, now named
`Load_And_Play_Intro_Music_476D`. The animation-scene routine starts at
`0800:48B7`.

## File loading

The routine selects disk 2 and retries opening the external `WWOODBT.SIF`
after the normal disk prompt. It seeks to EOF to obtain the exact 16-bit byte
length, returns to offset zero, and reads the complete file at `246C:244B`.

Four zero bytes are written at `payload + length`. The prior annotated fields
`246C:48B7-48BA` were false: the assembly uses the runtime length in `BX`, so
these are moving terminator addresses, not fixed state variables.

## Playback selection

`204B:0048` replaces IRQ0 and programs PIT channel 0 with divisor `0x0FFF`.
The controller then uses one of two callback-table dispatchers:

- adapter 1 starts the four-byte-frame Tandy consumer with cadence `0x14`;
- every other adapter starts the sequential PC-speaker consumer with cadence
  `0x04`.

Selector zero resets/stops either dispatcher; selector one starts it with the
SIF far pointer and cadence left on the caller's stack. This unusual variadic
calling convention is intentional.

The active interrupt callback is stored as `204B:0016-0018`. The setup records
point it at `204B:024F` for Tandy or `204B:0298` for PC speaker. Both consumers
restore the idle `RETF` at `204B:0186` after reading the appended terminator.
Therefore `204B:033C` compares a callback offset, not a frequency divisor.

The foreground loop ends on either that completion signal or keyboard input.
It stops the selected dispatcher, restores the previous IRQ0 vector, and
returns PIT channel 0 to divisor `0xFFFF`.

The headerless four-channel encoding and both playback interpretations are
documented in `docs/formats/DEMO_AND_SOUND.md`; InceptionTools already exposes
inspection, WAV export, and playback commands. `WWOODBT.SIF` remains an
external original-game asset.

No Astra review is needed for this controller. Exact Tandy waveform fidelity
remains a separately documented audio-tooling question.
