# `BTECH_0800` movement-cache finalization

## Review block (`0800:22FA-231C`)

The final block of `Character_Movement_On_Map` rebuilds all local map data
derived from the current packed X/Y position. It runs after a successful move,
after an interaction blocks or consumes a move, and after a zero-delta call.

## Local map rebuild (`207F:1314`)

`PosXY_OffsetGrid` first retains the two packed coordinates at `246C:02CF` and
`246C:02D1`. It then extracts local 16-unit block coordinates:

```text
localBlockX = lowByte(packedX) >> 4
localBlockY = lowByte(packedY) >> 4
```

This corrects an important Reko extrapolation. The assembly shifts only `AL`
and `BL`; shifting the complete 16-bit coordinates would incorrectly mix the
coarse region bytes into these local values.

The helper visits the 3-by-3 neighbourhood centred on that local block. Its
nine calls to `207F:13D9` both:

- write combined terrain/tile flags to `246C:07A4-07AC`, now named
  `LocalTerrainFlags_07A4[9]`; and
- render the corresponding nine 8-by-8 blocks into the 24-by-24 interaction
  map at `246C:07AD-09EC`.

The `13D9` renderer contains the detailed tile transformation and edge-region
selection logic. That internal routine has not been fully re-annotated in this
block.

## Current interaction-map origin (`207F:1DF8`)

The second helper has been renamed `Update_Cached_Map_Origin_1DF8`. It derives
the current position inside the 24-byte-wide interaction map from the low bytes
of the packed coordinates:

```text
column    = ((lowByte(packedX) >> 1) & 7) + 2
row       = ((lowByte(packedY) >> 1) & 7) + 2
rowOffset = row * 24
origin    = rowOffset + column
```

The `+2` values place the current position within the surrounding cache margin.
The three words are now identified separately:

| Address | Maintained name | Meaning |
|---|---|---|
| `246C:09ED` | `CachedMapOriginIndex_09ED` | Linear origin used by collision and sprite projections. |
| `246C:09EF` | `CachedMapOriginColumn_09EF` | Column within the 24-byte-wide map. |
| `246C:09F1` | `CachedMapOriginRowOffset_09F1` | Row multiplied by 24. |

The previous name described this as combat-only, but exploration movement and
interaction code use the same origin extensively.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:22FA-231C`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_207F.asm`, `207F:1314-158B` and
  `207F:1DF8-1E36`.
- Consumers of `246C:09ED` in the exploration sprite, collision, and building
  interaction paths.

The finalization flow and origin calculations are explicit in the assembly and
do not require Astra confirmation. The internals of `207F:13D9` remain a
separate future review block.
