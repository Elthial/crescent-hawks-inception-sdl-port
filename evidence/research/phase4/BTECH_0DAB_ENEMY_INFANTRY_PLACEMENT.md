# `BTECH_0DAB` enemy infantry placement

## Reviewed range

- Address range: `0DAB:0FBF-11F4`.
- The transition into enemy 'Mech placement begins at `0DAB:11F5`.

## Shared placement scan

The routine retains the signed X/Y scan coordinates created during encounter
initialization. It visits generated enemy infantry records `8..15`, skipping
records whose name/status byte is `FF`.

For each live record it searches forward until it finds a candidate tile which
satisfies all three tests:

```text
tile code > 0x0F
tile code < BlockingTileCodeThreshold_0150
signed map-cell index >= 0
```

The candidate index is projected into the 24-by-24 expanded combat-map cache
using the cached origin at `246C:09ED`, with parity corrections for the packed
player and candidate coordinates. An invalid candidate advances X. After 17
candidate columns (`0..16`) the scan restores its initial X and advances Y.
There is no fixed row limit in this search loop.

After every live record is processed, the scan advances once more. Dead/unused
records do not consume a candidate position. This prevents two retained enemy
infantry records from receiving the same coordinate.

## Cache-bound rejection

After finding a passable byte, the routine separately calculates:

- a signed cache column, required to be `0..23`;
- a signed cache row offset, required to be `0..575` (`0x000..0x23F`).

If either falls outside that range, the generated infantry record is discarded:
its name/status becomes `FF`, its two combatant coordinates become `FFFF`, and
its active/on-map word becomes zero. The routine does not resume searching for
an alternative in-range tile for that record.

## Packed world-coordinate normalization

An accepted candidate becomes a packed world position relative to the player's
packed position. If local-coordinate bit 7 becomes set, the routine crosses a
coarse map-page boundary and restores the canonical `0..0x7F` local range.

X uses a `0x0100` coarse-page step, expressed by the original as either masking
with `0x0F7F` or adding `0x0080` to the already noncanonical value. Y uses the
corresponding high-nibble page step, masking with `0xF07F` or adding `0x0F80`.
This is the same packed-coordinate scheme documented in
[PACKED_MAP_COORDINATES.md](../data-structures/PACKED_MAP_COORDINATES.md).

## Activated combat state

Enemy infantry record IDs `8..15` map to combatant IDs `16..23` by adding
eight. A successfully placed record receives:

- its packed X/Y words in the all-combatant position tables;
- sprite-frame byte `0x10`;
- active/on-map word `1`.

The original also increments a successful-placement count local, but never
reads that local again before returning from the encounter routine. Another
local is initialized alongside it and never used at all; these are retained as
compiler/source-history evidence rather than active encounter rules.

## Out-of-range read defect

The executable reads `CombatMap_07AD[candidateIndex]` before checking whether
the signed index is negative. It also has no upper-bound check before this
read. The later projection bounds reject the record, but cannot undo the
earlier out-of-range access. This is recorded as BUG-005 in
[ORIGINAL_GAME_BUGS.md](../investigations/ORIGINAL_GAME_BUGS.md).

A C# port should validate `0 <= candidateIndex < 0x240` before indexing while
preserving the subsequent placement/discard rules as a separate compatibility
decision.

## Confidence

Control flow, word/byte widths, scan bounds, tile predicates, packed-coordinate
normalization, and combatant-table addresses are directly verified from the
clean assembly. No Astra review is required.
