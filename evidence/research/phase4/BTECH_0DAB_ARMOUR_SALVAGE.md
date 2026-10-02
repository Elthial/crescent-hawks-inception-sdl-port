# `BTECH_0DAB` armour salvage and field repair

## Reviewed block

- Address range: `0DAB:00CB-018C`.
- Parent routine: `Salvage_Armour_Dialog`.
- Internal-structure salvage is gated at `0DAB:018D` and begins at `0196`.

This block collects every remaining point of armour from enemy mech wrecks and
spends that common pool repairing the four player-lance mech slots.

## Wreck flags and enemy records

The wreck test is a 16-bit read from:

```text
3092:393C + combatantId * 2
```

Enemy mech combatant IDs are `0x0C..0x0F`, so their four flag words occupy
`3092:3954..395A`. The old header declared the base as a byte array even though
the `SHL BX,1` and `CMP word ptr` instructions prove its word width.

For every nonzero enemy wreck flag, the original loops over record offsets
`0x11..0x1B` and adds all eleven current-armour bytes to a 16-bit pool. It also
increments a 16-bit wreck count which is used later by whole-'Mech salvage.

The native enemy-record expression is:

```text
3092:C33C + combatantId * 0x7D
```

This is pre-biased addressing, not an array of sixteen records beginning at
`C33C`. With the first enemy combatant ID:

```text
0xC33C + 0x0C * 0x7D = 0xC918
```

`C918` is the first of four live enemy mech records and is equivalently
`Mechs[4]` in the eight-record array at `C724`. The maintained C now uses
`Mechs[4 + EnemyMechId]`, avoiding the former out-of-bounds
`DestroyedMechs[combatantId]` notation.

## Repair order

The pooled armour is applied in this exact order:

1. armour locations from record offset `0x1B` down through `0x11`, equivalent
   to `CurrentArmour[10]` down through `CurrentArmour[0]`;
2. for each location, friendly mech slots `0..3` in ascending order.

Slots whose first name/status byte is `0xFF` are skipped. For every other slot,
the routine computes `MaximumArmour - CurrentArmour`, caps that deficit to the
remaining salvage pool, adds the result to current armour, and subtracts it
from the pool. It does not stop early when the pool reaches zero.

This ordering matters when armour is insufficient: rear left-torso armour is
offered repair first across the entire lance, followed by rear centre torso,
rear right torso, and then the remaining locations in reverse record order.
A port should preserve the ordering unless deliberately changing game rules.

## Header corrections

The reviewed accesses also align with the already documented `0x7D`-byte mech
record layout. `BTECH.h` now uses two packed current-actuator bytes and two
packed maximum-actuator bytes, places maximum armour at `+0x56`, and names the
eight terminal state bytes at `+0x75..+0x7C`. These corrections make the C
structure total exactly `0x7D` bytes rather than shifting maximum armour and
all subsequent fields by two bytes.

No Astra confirmation is required: loop limits, byte/word widths, address
equivalence, repair order, and field offsets are explicit in the clean ASM.
