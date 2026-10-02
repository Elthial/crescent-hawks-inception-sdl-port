# `BTECH_0DAB` salvage candidate state handling

## Reviewed block

- Address range: `0DAB:080E-094A`.
- Parent routine: `Salvage_Mechs_Dialog`.
- The common party-index increment is at `084D`; pilot eligibility testing
  resumes at `0850`.

This block coordinates the nested wreck-candidate and party-pilot loops using
two native 16-bit stack words.

## State values

`SalvageLoopState` has three observed values:

| Value | Meaning |
| --- | --- |
| `0` | Current wreck was rejected; try another candidate immediately. |
| `1` | A wreck was successfully recovered. |
| `9` | The wreck table is exhausted. |

Consequently this word is not strictly Boolean. When it remains zero and
`RemainingWreckCount` is nonzero, control jumps directly back to random
candidate selection at `0926`. There is no keypress between failed candidates.

When `RemainingWreckCount` reaches zero, both `SalvageLoopState` and the current
party index are set to `9`. The nonzero state leaves the candidate loop and
causes a keypress pause. The routine then redraws the sidebar, displays:

```text
There are no more salvageable 'Mechs on the battlefield.
```

and waits for a second keypress. Party index `9` is deliberately above the
eight-record range; the common increment at `084D` changes it to `10`, after
which the signed `< 8` test terminates the outer scan.

A successful recovery (`state == 1`) receives the first keypress pause but, if
wrecks remain, skips the exhaustion message and advances to the next eligible
on-foot pilot.

## Pilot scan and prompt

The outer loop walks all eight party records using a 16-bit index. A party
member is eligible only when:

- `Name != 0xFF` (living record);
- `Skill_Piloting != 0`;
- `Piloting >= 8`, where `8` is the on-foot assignment.

For an eligible record, the character-name ID is resolved through the far
character-name pointer table and the player is asked whether to salvage a wreck
for that pilot. Declining advances to the next party member without inspecting
a wreck.

After acceptance, the selected technician's name and the inspection message are
displayed, followed by a `0x78` vertical-retrace delay. The candidate state is
reset to zero and two random calls choose the initial combatant ID: the first
provides the low `0..3` index and the second chooses base `0` or `0x0C`.

Control jumps to `055A`, which tests that random seed directly. It does **not**
increment before the first test; increment/wrap at `054C` occurs only when the
tested wreck word is zero. This corrects the earlier isolated-block reading.

No Astra review is required: state assignments and branches are explicit in
the assembly.
