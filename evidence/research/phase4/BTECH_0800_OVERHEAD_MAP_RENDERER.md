# `BTECH_0800` overhead-map renderer

This note covers `0800:3FAE-45C1`, reconstructed as
`Overhead_Map_Draw_3FAE`. The next routine begins at `0800:45C2`.

## Purpose

The routine builds and draws the 320x192-pixel navigational map, adds the
party marker and optional objective-direction glyph, then waits for and
returns one raw key. It is both a renderer and the input half of the
overhead-map controller.

The display is 40x24 tiny tiles. Each tiny tile is 8x8 pixels, giving the
full 320x192 map area. The bottom eight scan lines contain the instruction
text.

## View window

The packed world position supplies a coarse region column and row. The
renderer backs up two columns and one row, then clamps the top-left region:

- column: `((X & 0x0F00) >> 8) - 2`, clamped to `0x00..0x0B`;
- row high byte: `((Y & 0xF000) >> 8) - 0x10`, clamped to `0x00..0xD0`.

The resulting window covers five 8-tile-wide regions by three 8-tile-high
regions. These fifteen expansions form the 40x24 tile-ID buffer at
`246C:244B`.

## Normal-map reconstruction

For each of the fifteen centre regions, the routine scans the surrounding
3x3 entries in `MapFile`. Nonzero entries are one-based `MAP<n>` numbers.
The nine far pointers at `3EDB:0170` select the corresponding cached 8x8
descriptor grids.

Four tables describe MAP1 through MAP13:

| Address | Meaning |
| --- | --- |
| `3EDB:0A38` | X offset within the cached 8x8 descriptor grid |
| `3EDB:0A46` | Y offset within that grid |
| `3EDB:0A54` | number of descriptor columns |
| `3EDB:0A62` | number of descriptor rows |

Each byte table occupies fourteen bytes. The first thirteen bytes describe
MAP1..MAP13 and the last zero byte is alignment padding, not a MAP14 entry.

The centre grid receives sequential dynamic tiny-tile IDs beginning at
`0x90`. Neighbour-grid cells which have not already received a dynamic ID
are linked to the next centre ID. `Map_NineGrid_Parent_1DA8` and
`Overhead_Map_1F04` then expand the descriptors into the output buffer.

The centre region's `MAP<n>.MTP` is opened from the appropriate original
game disk. A `0x021D`-byte read consumes the fixed header into scratch
memory; the following read deliberately overwrites that header with tile
bytes. This relies on the DOS file position advancing after the first read.

The word table at `3EDB:0A70` is used both as the second read request and as
the scratch-buffer stride:

`1000, 1000, 0400, 0400, 0400, 0400, 0400, 0400, 0400, 0400, 1000, 0200, 0200`

It should therefore be understood as an **overhead read/reservation span**,
not necessarily the MTP payload length. MAP12 and MAP13 are 8x8 and contain
only `0x0040` tile bytes; DOS reaches EOF after those bytes even though this
routine requests and reserves `0x0200`.

## Star League Cache path

The Cache bypasses normal reconstruction. It copies the complete
`0x1080`-byte MTP workspace from `246C:101D` to the scratch area at
`246C:644B`, fills the first `0x03C0` output bytes with
`CacheMapRoomLoaded - 0x30`, and calls `Overhead_Map_1F04(0x0150)`.

The subtraction is present in the executable. With the observed true value
of one it wraps to byte `0xD1`; it is not a decompiler arithmetic error.

## Fog and tiny-tile drawing

The fog table is addressed as sixteen bytes per coarse map row. Each byte
covers eight horizontal cells, most-significant bit first. A visible tile ID
below `0x90` selects a 32-byte tile directly from the loaded `TINYLAND` tile
set. IDs `0x90` and above are synthesized by
`Build_Dynamic_Overhead_Tile_45C2` into
the fixed 32-byte buffer at `246C:642B` before drawing.

## Overlay and input

When `3092:D33B` is nonzero, outside the Cache, and the viewport is not in
region `0x8A`, the renderer computes a direction toward packed coordinate
`0A38:8038`. It draws the resulting one-byte glyph at the direction-specific
coordinates stored at `3EDB:0A8A` and `3EDB:0A92`. The story meaning of this
objective flag and fixed destination remains unconfirmed.

The party marker converts the saved packed position into a pixel within the
current five-by-three region window and blinks as a random-colour 2x2 block.
Normal play waits for pending keyboard input. Attract/demo input instead
waits for at most 600 vertical retraces, after which the routine obtains the
recorded raw key and drains the pending-input queue.

The instruction text permits arrow-key scrolling only outside the Cache and
when `Kurita_DestroyedCitadel` is nonzero. This agrees with the controller:
before that story flag is set, and inside the Cache, the map is a one-screen
display.

## Porting notes

- Preserve all coordinates and counters as 16-bit values unless a wider host
  type is used with explicit masks; the executable contains no 32-bit C
  pointers here.
- Model `3EDB:0170` as nine 16:16 far pointers, not a 32-bit flat-pointer
  array inferred by Reko.
- Preserve short DOS reads at EOF. A host loader should not require the
  number of returned bytes to equal the padded read request for MAP12/13.
- Original MTP and TINYLAND assets remain external requirements and must not
  be copied into the replacement executable or repository.
