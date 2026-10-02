# Packed map coordinates

The game does not store a world position as one ordinary linear X value and one
ordinary linear Y value. Each 16-bit coordinate packs together:

1. the coarse map-region coordinate; and
2. the local position within that region.

“Packed” describes this coordinate representation. It does **not** mean that
the map file is compressed.

## Memory locations

The current map-view position is stored as two 16-bit words:

| Address | Meaning |
|---|---|
| `246C:A44B` | Packed X coordinate |
| `246C:A44D` | Packed Y coordinate |

Combatant world positions at `3092:4004` and `3092:4036` use the same
representation.

## Bit layout

The coarse map is a 16-by-16 grid. Its column occupies the low nibble of X's
high byte, while its row occupies the high nibble of Y's high byte. The low
seven bits of each coordinate hold the local position:

```text
Packed X word                       Packed Y word

15             8 7              0  15             8 7              0
+----------------+----------------+ +----------------+----------------+
| 0000 | column |0|    local X   | |  row  | 0000  |0|    local Y   |
+----------------+----------------+ +----------------+----------------+
```

Canonical local coordinates range from `00` through `7F`. Bit 7 is therefore
clear in a normalized position. The movement routines use the sign result from
incrementing or decrementing the low byte to detect that this boundary has
been crossed.

The coarse map-cell identifier combines the two high bytes:

```text
mapCellId = highByte(packedY) | highByte(packedX)
```

For example:

```text
packedX = 0x0D13  -> coarse column D, local X 13
packedY = 0x702C  -> coarse row    7, local Y 2C

mapCellId = 0x70 | 0x0D = 0x7D
```

Equivalent zero-based linear coordinates can be calculated as:

```text
worldX = (coarseColumn * 128) + localX
worldY = (coarseRow    * 128) + localY
```

These linear values are useful in a new implementation, but the original
packed values must still be understood when loading state or reproducing the
original arithmetic.

## Crossing a region boundary

The original movement helpers normalize the packed representation while
crossing between coarse regions:

```text
Move east:  X 0x0D7F + 1 -> 0x0E00
Move west:  X 0x0D00 - 1 -> 0x0C7F

Move south: Y 0x707F + 1 -> 0x8000
Move north: Y 0x7000 - 1 -> 0x607F
```

X changes its coarse component by `0x0100`; Y changes its coarse component by
`0x1000`. This asymmetry exists because the X column and Y row occupy different
nibbles of the combined map-cell identifier.

Crossing one of these boundaries has more consequences than changing the
coordinate. The routines at `207F:158C`, `163B`, `16E3`, and `17C5` also shift
and refill the cached 3-by-3 neighbourhood of map regions. Code which needs
those side effects must step through the original movement operation rather
than directly assigning the destination words.

## Local interaction-map origin

After movement, `207F:1314` uses only the low bytes of the packed coordinates
to rebuild the local 3-by-3 block neighbourhood and expanded 24-by-24
interaction map. The companion routine at `207F:1DF8` calculates the current
origin in that map:

```text
column    = ((localX >> 1) & 7) + 2
row       = ((localY >> 1) & 7) + 2
rowOffset = row * 24
origin    = rowOffset + column
```

The results are stored at `246C:09EF`, `246C:09F1`, and `246C:09ED`
respectively. Collision and sprite-projection code add signed relative offsets
to the origin at `09ED` before indexing the 24-by-24 map.

## Screen projection

Rendering code subtracts the packed map-view position from a combatant's packed
world position, adds the screen anchor, masks the result with `0x7F`, and then
converts it to pixels. The surrounding page checks decide whether the packed
subtraction represents the current or an adjacent coarse region.

This is why expressions involving `& 0x7F`, apparently unusual signed limits,
and separate comparisons of high bytes recur throughout the sprite compositors.
They are handling the boundary between the coarse and local components rather
than conventional signed Cartesian coordinates.

## Porting guidance

A C# port can expose a clearer value type containing `RegionX`, `RegionY`,
`LocalX`, and `LocalY`, with conversion to and from the original two words.
Asset/save loaders should retain the packed representation at their boundary,
while gameplay code may use normalized or linear coordinates internally.

Do not treat the original words as signed 16-bit coordinates. Comparisons in
`0800:17BB` use unsigned `JC`/`JA`, and Reko's inferred host-sized `int` types
do not supersede that 16-bit instruction evidence.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_207F.asm`, movement routines
  `207F:158C`, `163B`, `16E3`, and `17C5`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, relocation routine
  `0800:17BB-1816`, movement finalization at `0800:22FA-231C`, and sprite
  projection routines.
- `Btech/BTECH_0800.c`, annotated exploration and combatant compositors.
