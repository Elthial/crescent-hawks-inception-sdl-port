# `BTECH_0DAB` wreck candidate selection

## Reviewed block

- Address range: `0DAB:054C-05E9`.
- Parent routine: `Salvage_Mechs_Dialog`.
- Success continues at `05EA`; an unrecoverable candidate jumps to `07B5`.

This block selects and consumes one whole-'Mech wreck, translates its combatant
ID into the compact live `Mechs[]` record array, and decides whether the chosen
technician can recover it.

## Circular wreck scan

The random seed created immediately before this block has the form:

```text
(random & 1) * 0x0C + (random & 3)
```

It is therefore in player-'Mech range `00..03` or enemy-'Mech range `0C..0F`.
Control enters the test at `055A`, so the random seed itself is examined first.
While the word at `3092:393C + combatantId * 2` is zero, the loop increments the
candidate and wraps `10` to `00`. A nonzero word marks a wreck available for
inspection.

The old annotated C had this condition reversed and consequently skipped marked
wrecks in favour of an unmarked entry. The assembly is unambiguous about the
zero test and backward branch. The initial jump from `0926` to `055A` is also
important: only subsequent unmarked entries take the increment path at `054C`.

Once selected, the wreck word is immediately cleared. This happens before the
damage/skill test, so a rejected wreck has still been consumed and will not be
offered again. The code then recounts all nonzero entries across the sixteen-word
table; that remaining count later selects the "no more salvageable 'Mechs" path.

The scan has no empty-table escape. It therefore relies on its caller reaching
this block only while at least one wreck flag is set. Treat that as a control-flow
invariant until the combat caller has been reviewed, rather than as a confirmed
original-game bug.

## Combatant ID to mech-record index

The battlefield combatant table and the eight-entry `Mechs[]` array use different
numbering:

| Combatant ID | `Mechs[]` record |
| --- | --- |
| `00..03` | `0..3` |
| `0C..0F` | `4..7` (`combatantId - 8`) |

The circular scan technically visits `04..0B`, but the mapping does not handle
those IDs. The routine consequently depends on infantry combatant entries never
being marked in this whole-'Mech wreck table.

## Catastrophic-damage gate

The selected 0x7D-byte mech record is rejected when both conditions hold:

1. at least one catastrophic condition is present:
   - `EngineHits == 3` at record offset `+0x75`;
   - `GyroHits == 2` at `+0x76`; or
   - `CurrentStructure[3] == 0` at `+0x1F`;
2. the selected technician's Tech skill is below `Excellent` (`4`).

Thus an Excellent technician can attempt even a catastrophically damaged wreck,
whereas any technician may proceed when none of those three conditions exists.
The mechanical index and offsets are proven by the assembly. The anatomical
name of `CurrentStructure[3]` remains intentionally unresolved under Astra
candidate A-005.

No new Astra review is required for this block.
