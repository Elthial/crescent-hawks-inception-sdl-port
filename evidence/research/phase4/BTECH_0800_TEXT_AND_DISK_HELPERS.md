# `BTECH_0800` text and game-disk helpers

## Review block (`0800:2867-29F4`)

This block contains two small text-state helpers and two DOS game-disk routines
that were absent from the annotated C despite being distinct functions in the
expanded executable.

## Positioned white text (`0800:2867`)

`Display_Text_At_2867` accepts a 16:16 far text pointer followed by 16-bit
column and row values. It stores the coordinates at `3092:3748` and `374E`,
sets colour `0x0F` (EGA bright white), and calls the normal text renderer.

The previous `unsigned int*` parameter was a Reko host-width inference, not the
original pointer representation.

## Bright-green text (`0800:28A2`)

`Set_Text_Colour_Bright_Green_28A2` sets `3092:37FE` to `0x0A`. The original
executable substitutes palette index `1` when the graphics adapter is CGA. The
maintained transcription remains EGA-only, so the historical branch is noted
but not restored.

## Logical disk and DOS drive selection (`0800:28CC`)

`Select_Game_Disk_And_Drive_28CC` records the requested logical game disk at
`3EDB:014E`:

| Logical value | Prompt name |
|---:|---|
| `1` | Game Disk |
| `2` | Disk 2 |

When installed to a hard disk (`3092:D580 != 0`), no DOS drive is changed.
Otherwise DOS function `INT 21h/AH=0Eh` selects a zero-based drive according to
the installation configuration:

| Floppy configuration | Game Disk | Disk 2 |
|---|---|---|
| One drive | A | A |
| Second drive available | A | B |

The low-level wrapper `DOS_Select_Default_Drive_014C` has also been restored to
the annotated runtime source. A portable C# implementation should map these
logical media requests to configured original-asset locations; it need not
imitate DOS drive switching.

## Disk insertion prompt (`0800:2913`)

`Request_Game_Disk_2913` performs the selection above, saves the current menu
layout index from `3092:4600`, switches to layout four, and draws a bright-red
prompt assembled from initialized strings at `3EDB:04BA-0504`:

`Put the BattleTech [Game Disk|Disk 2] in [the drive|drive A:|drive B:] and press a key.`

For a two-drive installation it modifies character six of the writable
`"drive A:"` string at `3EDB:0504` to select A or B. It then clears pending
input state, waits for and returns one 16-bit keyboard result, redraws the top
sidebar, and restores the previous menu layout.

The prompt uses EGA bright red (`0x0C`). The original CGA override to palette
index `2` remains documented rather than restored.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:2867-29F4`.
- Initialized prompt strings in
  `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, `3EDB:04BA-050C`.
- DOS drive selector `207F:014C`.
- Installation-state setup in `0D27:007E-010B`.

The functions, widths, strings, and drive-selection matrix are explicit in the
assembly and do not require Astra confirmation.
