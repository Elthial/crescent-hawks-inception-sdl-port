# `BTECH_0800` directional movement and map streaming

## Review block (`0800:2212-22F9`)

After the interaction gate accepts a movement attempt,
`Character_Movement_On_Map` applies its signed Y delta, drains any map regions
queued by that move, then does the same for X. The final position-grid rebuild
at `0800:22FA` remains the next review block.

## Directional helpers

The four calls are now named from their instruction-level effects:

| Address | Maintained name | Effect |
|---|---|---|
| `207F:158C` | `Map_Position_Move_North_158C` | Decrement packed Y. |
| `207F:163B` | `Map_Position_Move_South_163B` | Increment packed Y. |
| `207F:16E3` | `Map_Position_Move_West_16E3` | Decrement packed X. |
| `207F:17C5` | `Map_Position_Move_East_17C5` | Increment packed X. |

This corrects an old error in the maintained `0800:218F` pseudo-C: its X
calls were reversed. The assembly calls `17C5` for `deltaX == 1` and `16E3`
for `deltaX == -1`.

Each helper changes only the low byte while movement remains inside the current
map region. At a `00`/`7F` boundary it also changes the packed region component,
shifts the retained 3-by-3 region cache, and describes the three newly exposed
regions in two parallel byte arrays.

## Pending map-load queue (`246C:09F3-09F8`)

The arrays have been named:

```text
PendingMapGridSlot_09F3[3]     destination slot in the cached 3x3 grid
PendingMapRegionIndex_09F6[3] world-region index used at 2FE8:0030
```

`0xFF` in a destination entry means that queue position is unused. The
directional helpers produce these destination patterns:

| Move | Newly exposed destinations | Region-index offsets from the new centre |
|---|---|---|
| north | `0, 1, 2` (top row) | `-0x11, -0x10, -0x0F` |
| south | `6, 7, 8` (bottom row) | `+0x0F, +0x10, +0x11` |
| west | `0, 3, 6` (left column) | `-0x11, -0x01, +0x0F` |
| east | `2, 5, 8` (right column) | `-0x0F, +0x01, +0x11` |

The centre region index is formed by ORing the high bytes of packed X and Y.
Those components occupy different nibbles, so the result is the row-major
world-region index used to look up a map-file number at `2FE8:0030`.

The old helper pseudo-C assigned words such as `0x0100`, `0x0706`, `0x0300`,
and `0x0502` to byte-array elements. Those were reconstructed word stores. The
maintained annotations now spell out all three destination bytes separately.

## Why the load loop occurs twice

The two nearly identical three-entry loops are present in the assembly; they
are not duplicated Reko output. The first follows Y movement and the second
follows X movement. This ordering is required for a diagonal step across a
region corner: the X helper would overwrite the same three queue entries that
the Y helper just populated unless the Y entries were consumed first.

For each non-`0xFF` destination, the loop:

1. uses the parallel region index to obtain the map-file number;
2. loads that file into the indicated 3-by-3 destination when the number is
   non-zero;
3. rebuilds the derived nine-grid cache after a successful load; and
4. clears the destination entry to `0xFF`, including when the lookup was zero.

A zero file-number lookup therefore means that no map file is assigned and does
not leave a request pending. Whether every such entry is intended as an empty
world region has not yet been established.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:2212-22F9`.
- Direction helpers in `BTECH_207F.asm`, `207F:158C-1885`.
- `DOS_Load_Map_Files` at `0800:2DA8`, whose first argument selects one of the
  nine cached grid destinations.

The direction mapping, queue layout, and need for two passes are explicit in
the original assembly and do not require Astra confirmation.
