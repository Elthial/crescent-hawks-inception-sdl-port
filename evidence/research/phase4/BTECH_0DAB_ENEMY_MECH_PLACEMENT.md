# `BTECH_0DAB` enemy 'Mech placement

## Reviewed range

- Address range: `0DAB:11F5-1466`.
- This completes the random encounter generator beginning at `0DAB:0D3D`.
- The unrelated human-inspection routine begins at `0DAB:1467`.

## Live-record scan

The routine advances the shared encounter scan by one unit after infantry
placement, then visits enemy 'Mech record IDs `4..7`. Records whose first
name/status byte is `FF` are skipped without consuming another position.

Enemy 'Mech record IDs `4..7` map to combatant IDs `12..15` by adding eight.
The active, position, and current-frame writes all use those combatant IDs.

## Two-cell footprint

Unlike infantry, a 'Mech candidate requires two passable bytes in the expanded
24-by-24 combat map. The primary cache index is calculated from the signed
scan coordinates and cached origin using the same parity adjustments as the
infantry path.

The second footprint cell is horizontally adjacent in the linear cache:

```text
same parity:       adjacent = primary + 1
different parity:  adjacent = primary - 1
```

Here “parity” compares the low bit of the candidate X coordinate with the low
bit of the player's packed X coordinate. Both primary and adjacent tiles must
be greater than `0x0F` and below `BlockingTileCodeThreshold_0150`.

An invalid footprint advances X by one. The search wraps to its starting X and
advances Y after 17 attempted columns (`0..16`). As in infantry placement,
there is no fixed row limit.

## Projection and packed position

After finding a two-cell footprint, the routine projects the primary anchor
back into cache column and row-offset values. The anchor must have column
`0..23` and row offset `0..575`. Failure destroys/discards the generated enemy
record by writing:

- `FFFF` to both combatant coordinates;
- `FF` to the 'Mech record's first name/status byte;
- zero to its active/on-map word.

A retained record receives the normalized packed world X/Y coordinates,
current sprite-frame byte zero, and active/on-map word one. Its packed page-
crossing corrections are identical to the infantry placement path: X crosses
by `0x0100`, while Y crosses by `0x1000` through the original mask/add forms.

After each live record is retained or discarded, the scan advances by three
units. If the associated counter exceeds eight, X returns to the row start and
Y advances. This spaces successive 'Mech anchors more widely than the one-unit
invalid-candidate search.

## Footprint bounds defect

The primary and adjacent tile bytes are both read before their signed-negative
checks and neither receives an upper-bound check. Moreover, the later cache-
projection check covers only the primary anchor. It does not prove that the
adjacent linear index remains on the same 24-byte row.

Consequently a primary cell at column 23 with a `+1` neighbour, or column zero
with a `-1` neighbour, can treat the next or previous row's edge byte as a
horizontal footprint cell. This extends BUG-005 in
[ORIGINAL_GAME_BUGS.md](../investigations/ORIGINAL_GAME_BUGS.md).

A safe port should validate both indexes against `0..0x23F` and require their
quotients on division by 24 to match before reading either tile.

## Confidence

The record bounds, parity branch, two tile reads, terrain predicates, scan
increments, packed-coordinate writes, and function endpoint are directly
verified from the clean assembly. No Astra review is needed.
