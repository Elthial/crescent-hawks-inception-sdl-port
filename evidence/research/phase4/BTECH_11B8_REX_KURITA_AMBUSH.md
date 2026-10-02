# `BTECH_11B8` Rex recruitment and Kurita-party ambush

## Review boundary

This note covers `Recruit_Rex_And_Start_KuritaParty_Ambush`, from `11B8:104E`
through the call to the combat controller at `137E`. BLD action dispatcher value
`1E` invokes the routine; the shipped call is `PARTY.BLD:178B`. The dispatcher subtracts one before indexing its
jump table, so the corresponding zero-based table index is `1D`; treating the
pseudo-C case label as the raw BLD operand would be off by one.

## Rex's record

The routine loads `3092:D456`, increments it, and stores the old value in party
record 1's Name byte. New-game setup seeds `D456` with 1, so the normal result
is Rex. This confirms the previous block's interpretation of `D456` as the next
scripted recruit Name ID.

Rex receives Body 12, Dexterity 9, Charisma 8, a Pistol, no armour, 120 Health,
zero training flags, and assignment 8 (on foot). His seven skill bytes are
copied from `3EDB:1E7A`:

```text
Bows/Blade  Pistol  Rifle  Gunnery  Piloting  Tech  Medical
    1          3      2       4         4       1      0
```

The old pseudo-C incorrectly labelled addresses `C630..C635` as skills. In a
17-byte infantry record they are Weapon, MechAssignment, ArmourType,
ArmourValue, Health, and TrainingFlags. Thus values `07,08,00,00,78,00` are a
coherent final initialization, not writes overwritten by the skill loop.
`3092:D563` is likewise not a standalone Boolean: it is Rex's entry (combatant
ID 5) in the sprite-family table beginning at `D55E`.

## Rex's Commando and the hidden-name table

The immediately preceding `PARTY.BLD` dialogue says Katrina left Rex a new
25-ton Commando parked in the garage. This explains why the routine adds the
mech to hidden party storage while staging Rex himself on foot for the alley
shootout.

The routine finds the first `FF` entry in `3092:D452..D455`, copies the complete
`0x7D`-byte reference Commando from `2FE8:0467` into the corresponding friendly
mech record, saves `C` in the selected D452 entry, marks live `Name[0]` as
`FF`, assigns Rex as `PilotId`, and selects sprite family `92`.

This establishes D452-D455 as `StoredPartyMechNameInitial_D452`, not a free-
standing sprite-ID array. Other workflows put the first character of a hidden
mech's name there and later restore it to live `Name[0]`. The sprite-family
choice is derived separately at D55E.

The search has an original bounds defect. If none of the four entries is free,
index 4 is used anyway, crossing from friendly to enemy mech storage and from
D455 into D456. This is recorded as BUG-011; ordinary-play reachability is not
yet established.

## Combat-state reset

Before reusing the eight enemy-infantry combatant slots, the routine saves
their world positions into the eight `0x1A`-stride roaming-NPC records at
`D390`. It then:

- marks all four enemy mech records and all eight enemy infantry records absent;
- clears the wreck and active WORDs for all 24 combatant IDs;
- writes `FFFF` to every combatant X and Y position;
- activates Jason and Rex's on-foot combatant IDs 4 and 5;
- clears their combat action-state bytes at `3998` and `3999`.

The position, active, and wreck arrays are the same 24-entry common tables
already verified in `Mission_GenerateEnemies`; the old `MapPosX`, `MapPosY`,
and subgroup expressions hid their true indexes.

## Ambush generation

Three random calls choose:

1. X origin `0A72` or `0A73`;
2. Y origin `805D` or `805E`;
3. an exclusive enemy-record end from 10 through 13.

Enemy records begin at 8, so the third value creates 2 through 5 soldiers—not
10 through 17. Their formation offsets are initialized bytes at `1E82..1E86`
for X (`0,1,2,0,1`) and `1E8A..1E8E` for Y (`1,1,1,2,2`). Each soldier uses:

- compact infantry record `8..12` and combatant ID `16..20`;
- on-foot assignment and grey enemy sprite family `FE`;
- sprite frame `1C`, west-facing animation cursor `2FE8:02E0`, and selector 6;
- six independently randomized skills `0..1` from Bows/Blade through Tech;
- `2D6` Body, Health equal to Body times 10, and weapon `Rand() % 8`.

Weapon IDs are therefore limited to Cudgel through Pistol. Medical, Dexterity,
Charisma, armour, and training flags are not initialized here and retain their
previous record bytes, matching the stale-field behaviour in the general
mission enemy generator.

Finally the routine passes the WORD `FFFF` to `Combat_Parent_000A`. That callee
uses a WORD argument and treats any nonzero value as pre-arranged combat,
preserving the active/position state staged here instead of generating a random
encounter.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `104E-137E`;
- initialized data at `3EDB:1E70-1E91`;
- reference Commando record at `2FE8:0467`;
- comparison with the reviewed `0FDC:0D49-1345` enemy setup;
- WORD tests of `[BP+06]` in `BTECH_183B.asm`.

Record offsets, widths, skills, counts, combatant mappings, formation values,
PRNG order, and final combat argument are directly verified. Only natural-game
reachability of the full-four-mech overflow remains unresolved. No Astra review
is required for this block.
