# `BTECH_0800` map-interaction boundary check

## Review block (`0800:1C12-1CC9`)

This is the first bounded slice of the larger routine at `0800:1C12`. The
routine's two arguments are signed movement deltas in the range `-1..+1`, not
absolute map coordinates. Its eventual 16-bit return value tells
`Character_Movement_On_Map` whether the move was blocked or consumed by an
interaction. A false result allows ordinary movement to continue.

## Boundary mask

The routine takes the low nibble of the current packed coordinates at
`246C:A44B` and `246C:A44D`. It builds a direction mask only when the requested
step crosses an edge of the current 16-by-16 terrain subcell:

| Bit | Direction | Condition |
|---:|---|---|
| `08` | North | `deltaY < 0` and local nibble Y is `0` |
| `04` | South | `deltaY > 0` and local nibble Y is `F` |
| `02` | West | `deltaX < 0` and local nibble X is `0` |
| `01` | East | `deltaX > 0` and local nibble X is `F` |

Diagonal movement can set one vertical and one horizontal bit. The static
table at `DS:04A0` translates the mask to a row-major index in the cached
neighbour table at `246C:07A4`:

```text
0 1 2     northwest  north  northeast
3 4 5     west       centre east
6 7 8     southwest  south  southeast
```

The observed mappings include east `1 -> 5`, west `2 -> 3`, south `4 -> 7`,
north `8 -> 1`, and the corresponding diagonal indexes.

## Deep-water rejection

When a boundary is crossed and the selected neighbour's terrain value is
`0x0F`, the routine:

1. sets the interaction/message flag at `3092:D55C`;
2. sets its local return Boolean to true;
3. draws the standard message box; and
4. displays the executable string at `3EDB:0410`, “The water is too deep that
   way.”, followed by the normal retrace/input wait.

It then skips the remaining interaction scans and returns true, preventing the
caller from applying the requested movement. The message itself establishes
the meaning of `0x0F` in this particular neighbour-terrain check; it should not
be generalized to unrelated tables which also contain the numeric value
`0x0F`.

## Corrections to maintained pseudocode

- Both arguments are signed 16-bit words. Their old unsigned declarations made
  the north and west comparisons impossible.
- The return local and return type are 16-bit, matching `[BP-4]` and `AX`.
- `DS:04A0` is now named `NeighbourGridIndexByDirectionMask` rather than the
  anonymous `a04A0`.
- The packed coordinate words are sampled only for their low nibbles in this
  slice; they are not copied as new absolute positions.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1C12-1CC9`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, direction table at
  `3EDB:04A0` and deep-water message at `3EDB:0410`.
- `Btech/BTECH_207F.c`, the row-major construction of `246C:07A4-07AC`.
- Caller `0800:218F`, which performs ordinary movement only after a false
  return.

This slice is direct control-flow and table evidence and does not require an
Astra confirmation pass.
