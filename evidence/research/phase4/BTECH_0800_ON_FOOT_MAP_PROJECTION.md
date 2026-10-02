# `BTECH_0800` on-foot movement map projection

## Review block (`0800:1CCA-1D8D`)

This second slice of `Map_Interactables_Building_Or_Items` begins the scan of
the eight compacted on-foot party slots at `3092:4072`. Inactive slots are
skipped. Each active slot's formation-relative position plus the requested
movement delta is converted to an index in the cached interaction map.

This slice stops after fetching the destination tile code. Interpretation and
dispatch of that code begin at `0800:1D8E` and remain the next review block.

## Static probe tables

The original data segment contains four signed 16-bit formation-probe arrays:

| Address | Entries | Use |
|---:|---:|---|
| `DS:045C` | 4 | Friendly-mech X offsets |
| `DS:0464` | 8 | On-foot X offsets |
| `DS:0474` | 4 | Friendly-mech Y offsets |
| `DS:047C` | 8 | On-foot Y offsets |

The current slice uses the two eight-entry tables. It adds screen/cache biases
`0x1A` and `0x0C` to produce `ProjectedX` and `ProjectedY`.

## Cached-map addressing

`246C:07AD-09EC` is exactly `0x240` bytes and is addressed as a 24-by-24
tile/interaction-code buffer. The ten words at `DS:048C` are row starts:

```text
0000, 0018, 0030, 0048, 0060, 0078, 0090, 00A8, 00C0, 00D8
```

The initial relative cell offset is:

```text
rowOffset   = rowOffsets[floor(ProjectedY / 2)]
column      = signedShiftRight(ProjectedX - 0x0D, 1)
cellOffset  = rowOffset + column
```

The assembly implements the row selection by clearing bit zero of Y and using
that even value as a byte offset into the word table. These are equivalent
operations.

The grid is staggered. When projected X is even and packed party X is odd, the
cell offset advances once. When projected Y and packed party Y are both odd,
it advances one full 24-cell row.

Finally, the word at `246C:09ED` is added as an origin index before reading:

```text
cellIndex        = mapOriginIndex + cellOffset
destinationCode = mapBuffer[cellIndex]
```

The former maintained expression instead added `09ED` to a byte already read
from `CombatMap_07AD`. That reversed the intended indexing operation.

## Corrections to maintained pseudocode

- Reconstructed the malformed row/column expression with the assembly's
  subtraction, signed shift, and parentheses.
- Identified `246C:07AD-09EC` as a `0x240`-byte/24-by-24 buffer rather than one
  byte.
- Identified `246C:09ED` as its current origin index.
- Named the four movement-probe arrays and the row-offset table.
- Corrected `3092:4072` to an eight-word active-slot array and repaired its
  stale declaration punctuation.
- Renamed the fetched value from the overly specific
  `StarLeague_Cache_Flag` to `DestinationTileCode`; it is also used outside
  Star League Cache handling.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1CCA-1D8D`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, signed probe words at
  `3EDB:045C-048B` and row offsets at `3EDB:048C-049F`.
- Address span `246C:07AD-09EC` and repeated 24-cell row increments in this
  routine and the exploration sprite compositor.

The arithmetic and table extents are directly evidenced and do not require an
Astra confirmation pass.
