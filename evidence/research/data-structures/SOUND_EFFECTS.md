# Executable sound effects

## Table structure

The game contains 18 PC-speaker effects in the expanded executable at
`3EDB:5008..5279`. This is a table of 313 little-endian 16-bit words, not an
external asset. Each effect is a sequence of variable-width commands followed
by the three-word delimiter `1, 0, 0`.

The complete 626-byte transcription has SHA-256
`065BFB0DBA2A02575BB77BD00C7CDD010931E2A957AB4DD7337563AF860FCC79`,
matching the clean segment dump.

| First word | Total words | Verified dispatch |
|---:|---:|---|
| `0..1000` | 3 | Fixed PIT-divisor tone. |
| `1001` | 6 | Repeated gate/noise operation. |
| `1002` | 7 | Repeated 16-bit wrapping divisor sweep. |
| `1003` | 7 | Descending parameter sweep. |
| `1004` | 7 | Ascending parameter sweep. |

Command width, parameters, delimiter handling, and sound-ID traversal are
verified against `1FC5:0002`. Exact busy-loop duration and the timbre produced
by the gate-toggling algorithms remain approximate in portable WAV output.

## Sound IDs

Names are based on the maintained call-site annotations and are **Probable**;
the numeric IDs, table starts, and command sequences are verified.

| ID | Tool name | Table start | Probable use |
|---:|---|---|---|
| `0x01` | `missile` | `3EDB:5008` | Missile launch or flight. |
| `0x02` | `mech-energy-weapon` | `3EDB:501C` | BattleMech laser/PPC; confirmed at 1AE8:19EA..19FE, not a kick sound. |
| `0x03` | `repeating-projectile` | `3EDB:5030` | Repeating projectile weapon. |
| `0x04` | `infantry-impact` | `3EDB:5044` | Infantry combat impact/noise. |
| `0x05` | `vibroblade` | `3EDB:5058` | Vibroblade. |
| `0x06` | `single-shot-projectile` | `3EDB:506C` | Single-shot projectile weapon. |
| `0x07` | `arena-destruction` | `3EDB:5096` | Arena destruction. |
| `0x08` | `terrain-damage` | `3EDB:50B8` | Terrain damage. |
| `0x09` | `personnel-laser` | `3EDB:50E8` | Personnel laser pistol/rifle; confirmed at 1AE8:19EA..19FE. |
| `0x0A` | `cache-door` | `3EDB:50FC` | Star League cache grinding door. |
| `0x0B` | `bow-string` | `3EDB:5110` | Bow string. |
| `0x0C` | `mech-startup` | `3EDB:5140` | Successful mech startup. |
| `0x0D` | `blade-impact` | `3EDB:5162` | Blade impact. |
| `0x0E` | `mech-startup-failed` | `3EDB:5198` | Failed mech startup. |
| `0x0F` | `map-interaction` | `3EDB:5200` | Map interaction. |
| `0x10` | `password-accepted` | `3EDB:522A` | Password accepted. |
| `0x11` | `password-incorrect` | `3EDB:5254` | Password incorrect. |
| `0x12` | `squished-by-mech` | `3EDB:5268` | Infantry squished by a mech. |

Sound ID zero is not an effect record. `0800:19BF` checks the global sound
setting and passes IDs to `1FC5:0002`; BLD opcode `E4` uses the same API.

## Runtime sound gateway (`0800:19BF-19DC`)

`Play_Sound_If_Enabled` is a synchronous enable gate around the executable
sound-effect interpreter at `1FC5:0002`:

```text
if (word DS:015C != 0)
    Sound_Setup_0002(soundId)
```

`DS:015C` is a 16-bit Boolean setting. Its initialized bytes are `01 00`, and
the following game-speed word begins at `DS:015E`. The settings menu toggles
only its low bit with `XOR BYTE PTR [015C],1`, while sound consumers test the
complete word.

`soundId` is also a 16-bit stack argument, despite every known effect fitting
in one byte. The gateway forwards it unchanged and performs no range check,
queueing, mixing, or effect lookup. `1FC5:0002` decrements the word argument and
walks the executable table, confirming that effect IDs are one-based. BLD
opcode `E4` supplies an unsigned byte ID which is widened to this call contract.

For a replacement executable, this routine is the natural game-facing audio
service boundary: preserve the enable check and one-based ID contract, while
the portable implementation may replace the synchronous PC-speaker interpreter.

## InceptionTools commands

```text
InceptionTools list-sound-effects [--json]
InceptionTools export-sound-effect-wav ID|NAME [--output FILE.wav] [--force]
InceptionTools play-sound-effect ID|NAME
```

IDs accept decimal or `0x` notation. Stable names such as `cache-door` are also
accepted. The catalog, renderer, and in-memory WAV player are reusable library
APIs for a future UI or replacement executable.

The WAV renderer preserves table order, PIT-divisor pitch, repeat counts,
16-bit wrapping sweep arithmetic, and deterministic noise. Its delay scale and
gate/noise waveform are explicitly provisional because the original uses
calibrated 8086 busy loops. Direct playback currently uses the Windows `winmm`
API; WAV export remains portable.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, `3EDB:5008..5279`:
  complete executable table bytes.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1FC5.asm`, `1FC5:0002..086D`:
  ID traversal, command dispatch, parameters, 16-bit arithmetic, and speaker
  algorithms.
- `BTech-Reko-expanded/BTECH.reko/BTECH_207F.asm`, `207F:001C..00D0`:
  PIT programming, speaker gate operations, and delay loops.
- Maintained call sites in `Btech/`: probable gameplay labels.
