# `BTECH_0800` animated map-tile updater

## Review block (`0800:240B-24C1`)

The routine formerly named `Combat_MemoryPtr_Copy_240B` is the shared updater
for animated map tiles. It has been renamed `Update_Animated_Map_Tiles_240B`.
It runs from the main loop and several mission/combat presentation loops, but
does work only while the normal BattleTech tileset (`TilesetId == 0`) is active.

## Source layout and frame selection

`Load_And_Draw_ANIMATE_ICN` retains the decompressed `0x0780`-byte payload at
`3092:D582-DD01`, then loads the base `BTTLTECH.ICN` graphics. The payload is
ten tile groups, each containing three `0x80`-byte planar images:

`source = D582 + (frame * 0x80) + (tileIndex * 0x180)`

The word at `3EDB:5800` is incremented modulo three before it is used. Thus a
zero-initialized counter selects frames `1, 2, 0`, then repeats. This corrects
the old pseudo-C, which dereferenced the computed source offset as a word and
assigned `0x180` after each tile instead of adding it.

## Destination tiles

The executable's initialized byte table at `3EDB:04B0` contains the ten base
tiles replaced on every update:

| Table index | BTTLTECH tile ID |
|---:|---:|
| 0 | `57` |
| 1 | `58` |
| 2 | `59` |
| 3 | `5A` |
| 4 | `5B` |
| 5 | `69` |
| 6 | `6A` |
| 7 | `6B` |
| 8 | `6F` |
| 9 | `71` |

On EGA/VGA (`GraphicsAdapter == 2` in the original), `207F:0A9F` copies the
selected 16-by-16 planar image into off-screen EGA tile storage at
`A400:(tileId << 5)`. The old transcription incorrectly used the loop index as
the tile ID; this would have overwritten slots `00-09` rather than the ten
animated slots.

The known visual uses, identified by the owner from `ANIMATE.png`, are the
Citadel electric security fence and the animated Starport arena crowd.
`11B8:137F` now resolves four members of the latter group: Arena combat setup
installs IDs `6A`, `6B`, `6F`, and `71`, then `11B8:1441` removes them after a
normal fight. This strongly indicates that the second group beginning at `69`
is the crowd set and leaves the contiguous first group `57-5B` as the fence
set; the four IDs used by Arena code are verified, while `69` is assigned by
table grouping. Tile `70` is an unanimated companion used at the ends of the
temporary crowd patches.

## Removed original graphics branch

The original executable has a second path for `GraphicsAdapter != 2`. It copies
each selected `0x80`-byte image to `3092:4614 + (tileId * 0x80)`, patching the
in-memory base-tile buffer rather than EGA planes. The maintained annotated
source is intentionally EGA-only, so this path remains documented but is not
restored.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:240B-24C1`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, initialized bytes at
  `3EDB:04B0` and frame word at `3EDB:5800`.
- EGA planar copy helper `207F:0A9F`.
- `Load_And_Draw_ANIMATE_ICN` at `0800:320B`.

The control flow, offsets, destination IDs, and copy strides are explicit in
the executable and do not require Astra confirmation.
