# `BTECH_0DAB` internal-structure salvage

## Reviewed block

- Address range: `0DAB:018D-0258`.
- Parent routine: `Salvage_Armour_Dialog`.
- Heat-sink salvage is gated at `0DAB:0259` and begins at `0262`.

This optional pass pools surviving internal structure from enemy mech wrecks
and applies it to damaged player-lance mechs. It runs only when the selected
party technician has Tech level 4 (`SkillLevel_Excellent`).

## Collecting structure

The enemy loop again visits combatant IDs `0x0C..0x0F` and checks the
word-sized wreck flags at `3092:393C + combatantId * 2`. For each flagged wreck,
it sums the eight bytes at mech-record offsets `0x1C..0x23` into a 16-bit pool.
Those bytes are `CurrentStructure[0..7]` in the normalized mech record.

As in the preceding armour pass, the assembly's expression
`C33C + combatantId * 0x7D` is a pre-biased route to the live enemy records at
`C918..CA8F`. The maintained C uses the equivalent and bounds-safe
`Mechs[4 + EnemyMechId]` form.

## Applying structure

Repair priority is:

1. `CurrentStructure[7]` down through `CurrentStructure[0]`, corresponding to
   raw record offsets `0x23` down through `0x1C`;
2. friendly mech slots `0..3` for each location.

An empty/destroyed friendly slot whose first name/status byte is `0xFF` is
skipped. For other slots, the routine computes:

```text
deficit = MaximumStructure[location] - CurrentStructure[location]
repair  = min(deficit, remaining salvaged structure)
```

It adds the repair byte to current structure and removes the same amount from
the 16-bit pool. The original continues visiting all remaining locations and
slots even after the pool reaches zero.

The former annotated C used raw offsets `0x1C..0x23` as indexes into the armour
arrays. Clean assembly shows maximum structure at record `+0x61..+0x68` and
current structure at `+0x1C..+0x23`; the corrected code therefore uses
`MaxStructure[0..7]` and `CurrentStructure[0..7]`.

No Astra confirmation is required: the Tech threshold, field ranges, record
addressing, word pool, and repair order are explicit in the clean ASM.
