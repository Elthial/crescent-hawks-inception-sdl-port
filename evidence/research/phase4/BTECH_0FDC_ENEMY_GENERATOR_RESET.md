# `BTECH_0FDC` enemy-generator reset

## Review boundary

This block covers the prefix of `Mission_GenerateEnemies`, from `0FDC:0D49`
through `0E44`. It saves roaming NPC positions, clears prior enemy records and
live combatant state, then selects whether Jason begins combat in his mech or on
foot. Mission-dependent spawn-origin and template selection begins at `0E45`
and is reviewed in
[`BTECH_0FDC_ENEMY_SPAWN_SELECTION.md`](BTECH_0FDC_ENEMY_SPAWN_SELECTION.md).

## Native widths and initial state (`0D49-0D6F`)

Both caller arguments are WORDs at `[BP+06]` and `[BP+08]`. Their low seven bits
are later used as requested enemy counts while bit 7 selects alternate setup
paths. The old byte parameter declarations were Reko extrapolations.

The routine initializes a far template pointer at `BP-04:BP-02` to
`2FE8:02F0`, the reference Locust record. WORD `BP-06`, later used as the enemy
mech sprite-family offset, starts at zero. Combat action-state byte `3092:3994`
for Jason is also cleared. The exact wider meaning of the `3994` action table is
left for the combat-controller review.

## Preserve roaming NPC positions (`0D70-0DA9`)

Before clearing live combatant positions, the routine saves the positions of
combatant IDs `16..23`. These are the eight enemy-infantry/roaming-NPC slots:

```text
source X = 3092:4004 + combatantId * 2 = 4024..4032
source Y = 3092:4036 + combatantId * 2 = 4056..4064
destination slot = 3092:D390 + npcSlot * 0x1A
destination X = slot+00
destination Y = slot+02
```

This confirms that the old `MapPosX` and `MapPosY` byte-array expressions were
incorrect. `4024` and `4056` are interior addresses in the two 24-WORD live
combatant-position tables. `D390` is an eight-slot, `0x1A`-stride record view,
not a contiguous coordinate array.

## Clear enemy records (`0DAA-0DE9`)

The four enemy mech records begin at `3092:C918`, compact mech-record index 4.
For each `0x7D`-byte record, setup writes `FF` only to `Name[0]`; it does not
erase the remaining record bytes.

The eight enemy infantry records are compact infantry-record indexes `8..15`,
at `C69C..C713`. Their `Name` bytes are likewise set to `FF`. These compact
record indexes must not be confused with live combatant IDs: enemy mechs occupy
combatant IDs `12..15`, and enemy infantry occupy IDs `16..23`.

## Clear live combatant state (`0DEA-0E25`)

One 24-iteration WORD loop resets four parallel tables for every combatant ID:

| Table | Reset value |
|---|---:|
| wreck/death state at `393C` | `0000` |
| active/on-map state at `406A` | `0000` |
| world Y at `4036` | `FFFF` |
| world X at `4004` | `FFFF` |

The previous pseudo-C placed a semicolon after the `for` statement. That made
the block execute once after the loop and referred to an index outside its
scope. Assembly proves the four writes occur inside every iteration. It also
proves `CombatantCasualtyFlags` is a 24-WORD address view, not the former
16-element declaration.

## Select Jason's active representation (`0E26-0E44`)

The second WORD argument is compared with `0x0080` using signed `JGE`. For all
verified callers, values below `0x80` activate combatant ID 0, Jason's mech.
The jailbreak caller supplies `0x88`, activating combatant ID 4, Jason's on-foot
representation. The old source assigned `TRUE` to the `OnMap_Infantry` array as
a whole; address `4072` proves this is specifically index 4 of the common
24-WORD active table.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0D49-0E44`;
- WORD parameter access at `[BP+06]` and `[BP+08]`;
- source/destination stride arithmetic at `0D70-0DA9`;
- record strides `007D` and `0011` at `0DAF-0DE9`;
- the four parallel WORD writes and 24-iteration bound at `0DEF-0E25`;
- exact active-table destinations `406A` and `4072` at `0E2D-0E44`.

The widths, table extents, record indexes, position preservation, reset loop,
and Jason deployment choice are directly verified. The broader purpose of
combat action table `3994` remains unresolved. No Astra review is required.
