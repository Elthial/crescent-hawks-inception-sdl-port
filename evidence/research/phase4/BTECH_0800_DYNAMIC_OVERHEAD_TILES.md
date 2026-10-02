# `BTECH_0800` dynamic overhead tiles

## Review boundary

This block covers the complete routine at `0800:45C2-4620`, now named
`Build_Dynamic_Overhead_Tile_45C2`, and its single-purpose data converter at
`207F:1F51-1F9B`, now named `Pack_Dynamic_Overhead_Tile_1F51`.

## Why dynamic tiles exist

The loaded `TINYLAND` artwork supplies fixed 8x8 overview tiles with IDs below
`0x90`. The overhead-map renderer assigns IDs beginning at `0x90` to blocks
derived from the current MAP file. These dynamic IDs allow the overview to
represent the actual 8x8 groups of terrain cells found in the original MTP
asset rather than requiring a pre-drawn overview image for every map.

`Overhead_Map_Draw_3FAE` stages each relevant MTP tile payload at
`246C:644B`. Every dynamic ID selects one consecutive 64-byte block:

```text
dynamicIndex = dynamicTileId - 0x90
source        = 246C:644B + dynamicIndex * 0x40
```

The assembly implements the multiplication with a byte exchange followed by
two right shifts. For the valid one-byte ID range this is exactly a 16-bit
`dynamicIndex << 6`; it is not a 32-bit address or structure operation.

## Map cells to a tiny tile

The selected 64-byte block is an 8x8 array of normal map-tile IDs. For each
source byte, `207F:1F51` performs an `XLAT` through the lookup based at
`246C:215D`. The active table occupies `0xFA` bytes and maps detailed terrain
tile IDs `0x00..0xF9` to overview palette colours. All tile payloads in the
preserved MAP1..MAP14 files stay within that range; their observed maximum is
`0xF9`.

Two translated four-bit colours are packed into each byte, first colour in
the high nibble and second colour in the low nibble. The 64 source cells thus
become the 32-byte chunky 8x8 tile at `246C:642B`:

```text
output[i] = lookup[source[i * 2]] << 4
          | lookup[source[i * 2 + 1]]
```

## Adapter conversion

The wrapper then selects an adapter-specific in-place conversion:

- adapter `0` (CGA) calls `207F:0163` with final word arguments `2` and
  `0x10`; their higher-level meanings have not been assigned here;
- adapter `2` (EGA/VGA) calls `207F:0572` for `0x10` words;
- adapters `1` and `3` perform no conversion in this wrapper.

The maintained pseudocode keeps only the adapter-2 branch, consistent with
the project's EGA-only target. `207F:0572` consumes the same 32 bytes and
transposes each eight chunky pixels into four EGA plane bytes in place. The
caller immediately draws that temporary tile and may overwrite it for the
next dynamic ID.

## Porting notes

- A C# renderer does not need to emulate the EGA plane transpose when its
  drawing surface accepts palette indices. The useful portable result is the
  8x8 array produced by the `246C:215D` lookup.
- Preserve the lookup as data recovered from the executable. It is not an
  external art asset, while each source MTP remains an original-game asset.
- Validate that `dynamicTileId >= 0x90` and that the selected 64-byte block is
  within the staged MTP data. The DOS routine assumes its caller has already
  established both conditions.
- The exact instruction chain is short and unambiguous, so this block does
  not need an Astra confirmation pass.
