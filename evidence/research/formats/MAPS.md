# MTP map files

## Standard map header

`MAP1.MTP` through `MAP14.MTP` share a fixed `0x21D`-byte metadata prefix:

| Offset | Size | Confidence | Meaning / runtime destination |
|---:|---:|:---:|---|
| `0x000` | 1 | V | Reserved/unknown header byte; read but not subsequently used. |
| `0x001` | 1 | V | X offset within the selected 8x8 map-block descriptor grid. |
| `0x002` | 1 | V | Y offset within the selected 8x8 map-block descriptor grid. |
| `0x003` | 1 | V | Width in tiles. |
| `0x004` | 1 | V | Height in tiles. |
| `0x005` | `0x80` | V | Eight fixed 16-byte map-character name slots at `246C:A461`. |
| `0x085` | `0x100` | V | Sixteen fixed 16-byte building-name slots at `246C:A561`. |
| `0x185` | `0x20` | V | Sixteen 16-bit interaction/map X values at `3092:4564`. |
| `0x1A5` | `0x20` | V | Sixteen 16-bit interaction/map Y values at `3092:4596`. |
| `0x1C5` | `0x20` | V | Sixteen 16-bit character/map X values at `3092:39B4`. |
| `0x1E5` | `0x20` | V | Sixteen 16-bit character/map Y values at `3092:39D4`. |
| `0x205` | `0x10` | V | Sixteen-byte alternate-BLD lookup array at `3092:4602`. |
| `0x215` | `0x08` | V | Eight-byte roaming-NPC waypoint/link table at `3092:3768`. |
| `0x21D` | width × height | V | Tile IDs. |

All fifteen locally preserved standard-map samples have zero in header bytes
`0x000-0x002`. The X/Y placement behaviour is nevertheless explicit in the
loader: it adds `Y * 8 + X` to the selected descriptor-grid pointer.

The file sizes prove this boundary exactly:

| Maps | Dimensions | Tile bytes | Total bytes |
|---|---:|---:|---:|
| 1, 2, 11, 14 | 64×64 | 4096 | 4637 |
| 3–10 | 32×32 | 1024 | 1565 |
| 12, 13 | 8×8 | 64 | 605 |

## Tile ordering

The current InceptionTools mapping is:

- block ordering: maps 1, 2, 11–14;
- linear 8×8-block ordering: maps 3–10.

Both transformations use 8×8 blocks. `MtpMapExporter` preserves them and its
portable output matches all fifteen legacy reference renders pixel-for-pixel.
The original loader now confirms the 8×8-block organization: it divides the
header dimensions by eight and writes sequential descriptors beginning at
`0x90`, one per block in block-row order. The two within-block transformations
remain **Probable** pending review of the downstream block-expansion routine;
render parity proves migration fidelity, not which side of that routine owns
the reordering.

## Star map

`MAP15.MTP` is exactly 768 bytes, equal to 32×24, and has no `0x21D` metadata
prefix. It is a raw tile grid using the special star-map path. **Verified** for
size and absence of the standard header; **Probable** for current remapping.

## Code references

- `InceptionTools/Records/MtpMapRecord.cs`: maintained bounded record model.
- `InceptionTools/Maps/MtpMapExporter.cs`: portable profile selection, tile
  composition, metadata, and PNG export.
- `Btech/BTECH_0800.c`: map-loading callers and setup.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:2EF0-30BA`:
  original fixed reads and descriptor-grid construction.
