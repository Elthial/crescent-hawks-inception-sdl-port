# Demo input stream and sound-interface data

This page records the two runtime data families that do not use the graphics,
map, animation, building, or save formats. Both remain external assets supplied
by the user's original installation.

## `DEMOFILE`

`DEMOFILE` is a 1023-byte (`0x03FF`) instruction/input stream used by the
arcade-style attractor: if the player does not touch the intro screen, the game
starts a deterministic self-playing demonstration. The startup path reads
exactly `0x03FF` bytes into `3092:27B0`, fixes the random generator to a known
state, disables live input, loads the game map, and enters the ordinary main
game loop.

The keyboard-input wrapper then consumes one byte at a time from this stream.
The unusual address expression in the assembly is intentional:

```text
initial replay index = 0x2710
byte address         = 3092:00A0 + replay index
first byte address   = 3092:27B0
```

The index advances after each read. This is another pre-biased-base pattern,
not evidence that the demo starts at `3092:00A0`. A value `0x48` is consumed in
a timed loop before the next actionable input; its precise semantic label
(extended-key code, delay token, or both in this input abstraction) remains
**Unknown**.

| Property | Confidence | Value |
|---|:---:|---|
| File size and read length | V | `0x03FF` bytes. |
| Destination | V | `3092:27B0`. |
| Data role | V | Deterministic instruction/input stream for the self-playing attractor demo. |
| Element width | V | One byte. The old `short[0x03FF]` declaration is wrong. |
| Random seed/state | V | `3EDB:4FC0 = 0x1325`, `3EDB:4FC2 = 0x0090`. |
| `0x48` token meaning | U | Timed/skipped by the replay input loop. |

## `WWOODBT.SIF`

`WWOODBT.SIF` is the external sound file for the intro song. The local reference
file is 4352 bytes (`0x1100`) and contains 1088 headerless four-byte note frames.
The loader:

1. opens the file read-only;
2. seeks to the end to obtain its exact length;
3. seeks back to the start;
4. reads that many bytes at `246C:244B`;
5. writes four zero bytes immediately after the payload;
6. installs and starts either the PC-speaker or Tandy playback path.

The installed file contains no structural header or terminator. The loader's
four appended zero bytes terminate either playback path safely.

### Note encoding

`0x80` is the silent/rest code used throughout the file. Other bytes encode a
chromatic pitch as `octave = value / 12` and `semitone = value % 12`. The
routine at `204B:0139` uses the semitone to select one of twelve executable
frequency values at `204B:0121`, divides the 1,193,182 Hz PIT clock, and shifts
the divisor by `8 - octave`. InceptionTools reproduces this integer conversion.

The local file uses 14 sounding codes from `0x24` through `0x4C` and 2629 rest
bytes. Per-channel sounding-byte counts are 826, 443, 438, and 16.

### Two original playback interpretations

The high-rate timer is programmed with divisor `0x0FFF`, about 291.375
interrupts per second.

Sol:whole-path audit confirms the saved system IRQ0 chains immediately, then
every17 music IRQs (old-AL zero test followed by reload16), not every16. This
does not change the note cadences below. See
[startup/music audit](../phase4/BTECH_STARTUP_MUSIC_PLATFORM_AUDIT.md).

The active music callback is a 16:16 pointer at `204B:0016-0018`. Offset
`0x0186` is an idle `RETF`, not a timer divisor. Starting playback replaces the
offset with `0x024F` for Tandy or `0x0298` for PC speaker. Reaching the appended
zero terminator restores `0x0186`, which is how the foreground controller
detects completion.

- **PC speaker:** `204B:0298` consumes one byte every four interrupts. All four
  logical parts are therefore played sequentially as a rapid monophonic
  arpeggiation. The local song lasts about 59.74 seconds before sample rounding.
- **Tandy:** `204B:024F` consumes one four-byte frame every twenty interrupts.
  Bytes 0, 2, and 3 feed the three programmable tone channels; byte 1 feeds the
  PIT speaker path. The local song lasts about 74.68 seconds.

The portable Tandy WAV renderer currently mixes four square-wave voices. It
preserves the verified notes, rests, channel grouping, and cadence, but does not
yet emulate the exact SN76489/PIT electrical timbre.

Sol:native high-bit PC notes write divisor14 rather than disabling the gate;
Tandy80h produces period4 rather than a volume mute. The renderer's silence
for the documented rest BYTE is an audible approximation, not a literal chip
register translation. Single-zero PC skips and double-zero termination are
runtime stream semantics, not a new on-disk SIF header or frame structure.

The embedded sound-effect library used by routines in segment `1FC5` is a
separate executable-resident table and must not be conflated with this file.
Its documented transcription and tooling are in
[Executable sound effects](../data-structures/SOUND_EFFECTS.md).

## Non-runtime installation files

`BTECH.TXT`, the `BTchi.jpg` box art, the `CHI-MAP.GIF` navigation map, and the
`CHICODE1.GIF` mech-component quiz diagram are player documentation/reference
material. `$VERIFY.EXE` is a separate DOS utility. None is a runtime-loaded game
asset. Their roles and the missing late-game star-code image are documented in
[Copy protection and player references](../reference/COPY_PROTECTION_AND_PLAYER_REFERENCES.md).

## Sources

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`: exact file opens, seek/read
  lengths, destinations, replay initialization, and SIF terminator writes.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1F3D.asm`: byte-at-a-time replay input
  using the pre-biased `00A0 + replayIndex` expression.
- `BTech-Reko-expanded/BTECH.reko/BTECH_204B.asm`, `204B:0020..033B`: timer
  callback setup, both SIF consumers, note conversion, channel writes, and
  playback cadence.
- `Btech/BTECH_0800.c` and `Btech/BTECH_1F3D.c`: semantic annotations and call
  context, checked against the clean assembly for widths and addresses.
- Ignored `chinception/` installation: filenames and observed file sizes only.
