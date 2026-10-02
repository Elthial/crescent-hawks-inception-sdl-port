# `BTECH_0800` game-settings menu

## Reviewed block

- Address range: `0800:3BD0-3D3F`.
- Maintained name: `Menu_Change_Game_Settings`.
- This covers the complete routine.

## Main menu

Menu template `0x21` returns one of these actions:

| Result | Action |
|---:|---|
| `0` | Change exploration movement rate. |
| `1` | Set combat speed. |
| `2` | Toggle sound. |
| `3` | Set outtake frequency. |
| `4` | Confirm quitting the game. |
| other | Cancel/return. |

The sound line is assembled from three original strings. The first ends with
`Turn Sound O`; the routine appends `ff` while sound is enabled and `n` while
it is disabled. The menu therefore describes the action that will occur, not
the current state.

## Exploration steps per input

`DS:015A` is a 16-bit step count, not an ordinal speed setting. Its choices
map as follows:

| Menu selection | Stored value | Label |
|---:|---:|---|
| `0` | `1` | One step |
| `1` | `2` | Two steps |
| `2` | `4` | Four steps |

The inverse mapping supplies the submenu's default selection. The old pseudo-C
used a post-decrement while calculating that default and therefore appeared to
alter the active setting before the menu ran. Assembly performs the decrement
only in AX and does not write `DS:015A` until after the selection.

If `CacheMapRoomLoaded` is nonzero, the routine forces the stored result back
to one step. This agrees with the main loop, which executes the exploration
movement/draw sequence `DS:015A` times for each input.

## Combat pacing and sound

Menu template `0x22` writes its result directly to the 16-bit word at
`DS:015E`:

| Value | Label |
|---:|---|
| `0` | Fastest |
| `1` | Faster |
| `2` | Normal |
| `3` | Slower |
| `4` | Slowest |
| `5` | Keypress |

`GameSpeed_RateControl` converts values below five to `value * 0x0C` vertical
retraces and requests keyboard input for value five. Some combat callers also
test the setting before calling that helper; those call sites should be
reviewed with their own blocks.

Sound remains a 16-bit Boolean at `DS:015C`, although the original toggle is
specifically `XOR BYTE PTR [015C],1`. Sound consumers test the complete word.

## Outtake frequency

Menu template `0x26` stores a three-way selection at saved byte `3092:D35B`:

| Value | Label | `DS:0AC4` mask | Approximate trigger rate |
|---:|---|---:|---:|
| `0` | Very Frequently | `0x0003` | 1 in 4 |
| `1` | Frequently | `0x000F` | 1 in 16 |
| `2` | Infrequently | `0x003F` | 1 in 64 |

The animation-scene loader indexes this table as 16-bit words and triggers
the optional outtake path when `(random & mask) == 0`. The rates assume the
random generator distributes the relevant low bits uniformly.

The old declarations called this a Boolean and described `DS:0AC4` as a
one-byte array. Both conflict with the menu's three results, the `BX * 2`
assembly indexing, and the executable's words `0003 000F 003F`.

## Quit path

Quit selection displays the two exact strings at `3EDB:0993` and `3EDB:09CF`,
then stores the yes/no result directly in the main-loop exit word at `DS:0152`.
A no response therefore leaves the loop running.

## Porting notes

- Preserve `DS:015A`, `DS:015C`, and `DS:015E` as 16-bit settings.
- Expose the exploration setting as the actual values 1, 2, and 4 rather than
  silently treating it as an enum.
- Preserve the keypress combat-pacing option separately from timed values.
- Validate a loaded outtake-frequency byte before indexing the three-word mask
  table in a memory-safe port.

No Astra review is requested. The choices, conversions, field widths, lookup
masks, exact strings, and branch destinations are explicit in the clean
assembly and expanded executable.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:3BD0-3D3F`.
- `BTech-Reko-expanded/BTECH.EXE`, strings at `3EDB:0896-09ED` and masks at
  `DS:0AC4`.
- `Btech/BTECH_0800.c`, `Menu_Change_Game_Settings`.
- `Btech/BTECH_1631.c`, `GameSpeed_RateControl`.
