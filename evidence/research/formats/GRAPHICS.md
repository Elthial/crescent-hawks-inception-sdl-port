# CMP/ICN graphics and RLE

## Container header

All twelve local `.CMP` and `.ICN` samples have the same three-byte prefix:

| Offset | Size | Confidence | Meaning |
|---:|---:|:---:|---|
| `0x00` | 2 | V | Little-endian count of all remaining bytes; exactly `fileLength - 2`. |
| `0x02` | 1 | V | Compression format (`0x01` or `0x02`). |
| `0x03` | remainder | V | RLE token stream. |

This corrects the imported description of the first word as decompressed size.

## Observed files

| Format 01 | Bytes | Format 02 | Bytes |
|---|---:|---|---:|
| `ANIMATE.ICN` | 3613 | `BTSTATS.CMP` | 12637 |
| `BTBORDER.CMP` | 287 | `BTTITLE.CMP` | 27488 |
| `BTTLTECH.ICN` | 28861 | `ENDMECH.CMP` | 16215 |
| `DESTRUCT.ICN` | 29706 | `INFOCOM.CMP` | 6983 |
| `MAP.ICN` | 12135 | `MECHSHAP.CMP` | 16918 |
| `STARLEAG.ICN` | 27227 |  |  |
| `TINYLAND.CMP` | 2032 |  |  |

The extension does not select the RLE format; the byte at `0x02` does.

## ANIMATE overlay tiles

`ANIMATE.ICN` is an animation supplement to the base `BTTLTECH.ICN` tileset,
not a standalone tileset. `Load_And_Draw_ANIMATE_ICN` retains its decompressed
`0x0780` bytes and then loads `BTTLTECH.ICN`. At runtime, `0800:240B` cycles
through three frames for ten destination tiles. Each tile image is `0x80`
bytes, so the three alternatives for successive animated tiles are spaced
`0x180` bytes apart in the animation data.

The initialized destination-tile table at `3EDB:04B0` is
`57, 58, 59, 5A, 5B, 69, 6A, 6B, 6F, 71`. For frame `f` and table index `i`,
the EGA path copies the planar tile at `3092:D582 + f * 0x80 + i * 0x180` into
the off-screen tile slot at `A400:(tileId * 0x20)`. The frame word at
`3EDB:5800` is incremented modulo three before the copy, yielding selection
order `1, 2, 0` when it begins at zero.

The known visual uses (identified from the extracted `ANIMATE.png`) are the
electric security fence in the Citadel and the animated crowd in the Starport
arena. The exact allocation of the ten destination tile IDs between those two
effects remains to be matched against the map data.

## RLE token grammar

The following token stream is verified instruction-by-instruction against
`207F:22F8` and `207F:2368`:

| Signed control byte | Following data | Output |
|---|---|---|
| `+N` (`1..127`) | `N` bytes | Copy `N` literal bytes. |
| `-N` (`-1..-128`) | 1 byte `V` | Emit `V` exactly `N` times. |
| `0` | little-endian word `N`, then byte `V` | Emit `V` exactly `N` times. |

The decoder produces `0x7D00` (32000) packed bytes for a full 320×200,
four-bit image: two pixels per output byte.

## Format 01 versus Format 02

- Format 01 writes decoded bytes sequentially.
- Format 02 uses the same token grammar but stores the logical stream by output
  column. The old decoder writes at indexes `0, 160, 320, ...` for 200 rows,
  then returns to index `1`; the normalized packed buffer is 160 bytes × 200
  rows.

The later planar/pixel conversion is a separate operation and must not be
conflated with RLE expansion.

## MECHSHAP sprite catalog

`MECHSHAP.CMP` is a format-2 320×200 packed image. The game does not treat it
as a regular equal-cell grid: startup at `0D27:0410..082C` registers 376 source
rectangles with widths/heights of 24×24, 8×8, 16×14, 16×16, and 16×11. IDs
`0x000..0x091` form the Locust-relative block and shared effects; Commando
sprites begin at `0x092`; the final extracted ID is `0x177`.

The portable exporter reorganizes these rectangles into uniform 24×24 output
cells and writes exact source/output mappings to JSON. See
[MECHSHAP sequence spritesheet export](../phase3/MECH_SPRITESHEET_EXPORT.md).

## Portable general-image export

`CompressedGraphicsExporter` provides maintained single-file and batch PNG
exports for all twelve `.CMP`/`.ICN` sources. CMP files use the normalized
320×200 view. ICN files use a 16×4000 vertical strip so the 250 sequential
16×16 tiles retain the layout expected by map tooling. Known BTTITLE, INFOCOM,
and ENDMECH palette substitutions from the legacy extractor are retained as
named compatibility profiles. See
[portable CMP/ICN graphics export](../phase3/PORTABLE_GRAPHICS_EXPORT.md).

## Code references

- `Btech/BTECH_1F3D.c`, `1F3D:049D`: selects format 01 when the first byte of
  the loaded payload is `0x01`, otherwise format 02.
- `Btech/BTECH_0800.c`, `0800:240B` and `0800:320B`: applies the three-frame
  `ANIMATE.ICN` overlays to ten `BTTLTECH.ICN` tile slots.
- `InceptionTools/Graphics/CompressedImageDecoder.cs`: bounded verified token
  handling and format-02 write order.
- `InceptionTools/Graphics/CompressedGraphicsExporter.cs`: portable inspection,
  palette profiles, and single/batch indexed PNG export.
- `InceptionTools/Graphics/MechShapeSpriteCatalog.cs`: verified source
  rectangle map.
- `InceptionTools/Graphics/MechSpriteSheetComposer.cs`: sequence-row layout.
- `InceptionTools/Graphics/MechSpriteSheetExporter.cs`: PNG/JSON export.
