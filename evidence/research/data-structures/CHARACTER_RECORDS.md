# Infantry and character records

## Record layout

Infantry characters use a fixed **0x11-byte (17-byte)** record. **V** for the
stride: the annotated party array advances from `3092:C614` through eight party
records and eight enemy records to `C724`.

| Offset | Size | Confidence | Field | Notes |
|---:|---:|:---:|---|---|
| `0x00` | 1 | V | `NameId` | Indexes the character-name pointer table in segment `3EDB`; `0xFF` marks an unused/dead record. |
| `0x01` | 1 | V | `Body` | Character attribute; maximum health is compared with `Body × 10`. |
| `0x02` | 1 | V | `Dexterity` | Character attribute. |
| `0x03` | 1 | V | `Charisma` | Character attribute. |
| `0x04` | 1 | V | `BowsAndBladesSkill` | Skill level selected by weapon skill index `0`. |
| `0x05` | 1 | V | `PistolSkill` | Skill level selected by weapon skill index `1`. |
| `0x06` | 1 | V | `RifleSkill` | Skill level selected by weapon skill index `2`. |
| `0x07` | 1 | V | `GunnerySkill` | Mech skill selected by weapon skill index `3`. |
| `0x08` | 1 | V | `PilotingSkill` | Mech piloting skill. |
| `0x09` | 1 | V | `TechSkill` | Technical skill; the training workflow increments this byte. |
| `0x0A` | 1 | V | `MedicalSkill` | Medical skill; the training workflow increments this byte. |
| `0x0B` | 1 | V | `WeaponTableIndex` | Direct index into the `0x11`-byte weapon table. |
| `0x0C` | 1 | V | `MechAssignment` | `0x00..0x07` select a live mech record; `0x08` means on foot. Pilot and rider receive the same mech index. |
| `0x0D` | 1 | V | `ArmourType` | Infantry armour/equipment type. |
| `0x0E` | 1 | V | `ArmourValue` | Current armour value. |
| `0x0F` | 1 | V | `Health` | Current health in tenths of a Body point; this is the byte changed by the case `0x2E` health adjustment. |
| `0x10` | 1 | V | `TrainingFlags` | Persistent training/event bits; it is not rider state. Bit meanings are below. |

## Arrays

| Location | Count | Confidence | Purpose |
|---|---:|:---:|---|
| `3092:C614` | 8 records | V | Player party characters; Jason is slot 0 and Rex is slot 1 in current notes. |
| `3092:C69C` | 8 records | V | Enemy infantry records. |

The complete 16-record block occupies `0x110` bytes and ends immediately before
the mech block at `3092:C724`.

## Scripted Crescent Hawk recruitment

BLD opcode `E9` calls `Recruit_Crescent_Hawk_Agent` (`11B8:0D58`). The routine
uses the first party record whose `NameId` is `0xFF`. It assigns the Name ID held
at `3092:D456`, increments that byte, and wraps it from `0x0A` to `0x02` after
assigning ID 9. The same Name ID multiplied by `0x0187` becomes the two-WORD
PRNG state used to generate the recruit.

Body, Dexterity, and Charisma are independent two-die rolls. Health begins as
`Body × 10`; all seven skills begin randomly at zero or one, Piloting is then
forced to zero, and the opcode-selected specialty becomes Good (`3`). A
Piloting specialist loses `Body × 2` health, leaving `Body × 8`.

The shipped call sites establish the operand meanings:

| Script | Operand | Specialty |
|---|---:|---|
| `JAIL.BLD` | `4` | Piloting |
| `REPAIR.BLD` | `5` | Tech |
| `HOSPITAL.BLD` | `6` | Medical |

The recruit is placed as the rider in the first non-destroyed party mech with a
free `RiderId`; otherwise `MechAssignment` remains `8` (on foot). If no traitor
has yet been selected, a subsequent random bit gives this new party slot a 50%
chance of becoming the future traitor. See
[`BTECH_11B8_CRESCENT_HAWK_RECRUITMENT.md`](../phase4/BTECH_11B8_CRESCENT_HAWK_RECRUITMENT.md).

## Rex's scripted record

Action-dispatch value `1E` calls `Recruit_Rex_And_Start_KuritaParty_Ambush`
(`11B8:104E`). Rex occupies party record 1 and receives the current Name ID at
`D456` before that byte is incremented. In a normal new game this assigns Name
ID 1, the Rex entry.

The exact initialized record is:

| Field | Value |
|---|---:|
| Body / Dexterity / Charisma | `12 / 9 / 8` |
| Bows & Blades / Pistol / Rifle | `1 / 3 / 2` |
| Gunnery / Piloting / Tech / Medical | `4 / 4 / 1 / 0` |
| Weapon | `7` (Pistol) |
| Mech assignment | `8` (on foot) |
| Armour type / value | `0 / 0` |
| Health | `120` |
| Training flags | `0` |

The values at `C630..C635` are Weapon through TrainingFlags. Earlier notes
misread them as skill bytes, which made the otherwise coherent pistol, health,
and on-foot initialization look like temporary garbage. Rex's actual seven
skills are copied from initialized bytes `3EDB:1E7A..1E80`. See
[`BTECH_11B8_REX_KURITA_AMBUSH.md`](../phase4/BTECH_11B8_REX_KURITA_AMBUSH.md).

Scripted mission generation at `0FDC:1208-1345` uses enemy records `8..15` as
a shared pool for mech pilots and on-foot opponents. It randomizes only Body,
Health, Weapon, and the six skill bytes `+0x04..+0x09` for an on-foot enemy.
Dexterity, Charisma, Medical, ArmourType, ArmourValue, and TrainingFlags are not
initialized by that routine. See
[`BTECH_0FDC_ENEMY_INFANTRY_GENERATION.md`](../phase4/BTECH_0FDC_ENEMY_INFANTRY_GENERATION.md).

The compact sidebar converts `Health` to displayed Body units with signed
division by ten, truncating toward zero. It promotes a zero quotient to one
before calculating the inclusive red damage overlay; this makes zero health
cover the complete Body bar. See the
[character B/D/C sidebar row](../phase4/BTECH_0800_CHARACTER_BDC_SIDEBAR.md).

## Enumerated values

Current skill levels range from `0x00` (unskilled) through `0x04` (excellent or
grand master, depending on presentation text). Infantry weapon-table indexes
occupy `0x00` through `0x0E`, ending with the Flamer. Index `0x0F` begins the
mech-only portion of the same weapon table with the Small Laser.

The compact inspection screen uses the five-entry far-pointer table at
`DS:0194`. Its exact labels are `Unskilled`, `Amateur`, `Adequate`, `Good`, and
`Excellent`. A separate verbose table at `3EDB:4EA2` uses longer prose, which
accounts for the earlier “grand master” description without changing the
stored `0x00..0x04` scale.

### Mech assignment (`+0x0C`)

The assignment byte identifies the mech occupied by the character, not the
character's role inside it:

| Value | Meaning |
|---:|---|
| `0x00..0x07` | Index into the eight live mech records at `3092:C724`. |
| `0x08` | Character is on foot. |

`Assign_Pilot_and_rider_to_Mechs` writes the same mech index to both character
records. Pilot versus rider is recorded by `PilotId` and `RiderId` at mech
offsets `+0x79` and `+0x7A` respectively. This explains why earlier names such
as `Piloting`, `Riding`, and `OnFootOrPiloting` each captured only part of the
observed behaviour.

The complete assignment workflow at `1467:0002` uses compact WORD menu maps
to resolve actual party and mech slots. Replacing a pilot moves the old pilot
into the rider seat and dismounts a previous rider; replacing a rider puts that
rider on foot. Unskilled characters may ride but cannot be selected as pilots.
The nonzero entry argument resets all assignments, rather than selecting a
character. See the [1467 segment review](../phase4/BTECH_1467_SEGMENT_REVIEW.md)
for abandonment, completion and the separate mech-selector name-lookup oddity.

### Armour transfer (`+0x0D/+0x0E`)

The armour shop passes a purchased type and its full durability to
`Distribute_Purchased_Armour`. Giving it to a party member is an exchange: the
recipient receives the held type/value pair and their displaced pair becomes
the next item to distribute or drop. With no more than one living party member,
the routine instead equips Jason directly and discards his previous armour.
See
[`BTECH_0FDC_ARMOUR_DISTRIBUTION.md`](../phase4/BTECH_0FDC_ARMOUR_DISTRIBUTION.md).

### Weapon transfer (`+0x0B`)

`Distribute_Weapon_To_Party` exchanges the held weapon with the selected
living party member's `WeaponTableIndex`; the displaced weapon then becomes
the next item to give away or drop. With no more than one living party member,
the routine equips Jason directly and discards his old weapon.

Weapon ID zero (`Cudgel`) doubles as the transfer loop's empty sentinel. It
cannot be offered through this routine, and displacing a Cudgel ends the chain.
See
[`BTECH_0FDC_WEAPON_DISTRIBUTION.md`](../phase4/BTECH_0FDC_WEAPON_DISTRIBUTION.md).

### Training flags (`+0x10`)

The verified bits are:

| Mask | Confidence | Meaning |
|---:|:---:|---|
| `0x01` | V | Tech/MechLab training has been purchased or completed. |
| `0x02` | V | Medical training has been purchased or completed. |
| `0xFC` | U | No meaning established. |

The training code tests each bit before offering the corresponding event. It
deducts 500 C-Bills, increments `TechSkill` or `MedicalSkill`, then sets the
matching bit. Character initialization clears the entire byte.

## Decompiler alias trap at `3092:C60F`

`BTECH.h` declares an eight-byte `InfantryValueArray_C60F`, but no such array
can exist: only five bytes separate `C60F` from the first character record at
`C614`.

The clean assembly uses `C60F` only with an index already multiplied by the
character stride, and the loop starts at one:

```text
C60F + i × 0x11 = C620 + (i - 1) × 0x11, for i = 1..7
```

`C620` is character zero's `MechAssignment`. The paired stores through `C620`
and the pre-biased `C60F` base therefore cover the same `+0x0C` field across all
eight player records. Literal address `C60F` is never touched by these loops.
Treat `C60F` as an address-expression artefact, not storage preceding the
character array.

The Arena party-mech setup at `0FDC:1A26` is a concrete example. Its two
assembly stores use those overlapping bases with loop indexes `1..7`, so their
combined effect is to write literal `0x03` to `MechAssignment` in all eight
party records. The exact value is verified; why the Arena workflow chooses
assignment index three while staging the combat mech in slot zero remains
unresolved. The same routine separately saves party Name IDs `1..7` and writes
`0xFF` to hide those characters, leaving Jason visible.

The rental-Locust setup at `0FDC:1C9B` performs the identical assignment and
Name-ID staging. Immediately afterward, the enclosing Arena dispatcher changes
Jason's assignment from `0x03` to staged mech slot zero; hidden party members
retain `0x03` for the fight.

The corresponding cleanup at `0FDC:1B41` again uses both address expressions.
Its combined stores set all eight `MechAssignment` bytes to `0x08` (on foot),
then it restores the saved Name IDs for party slots `1..7`. It does not restore
the pre-Arena assignment bytes; Arena completion deliberately dismounts the
whole party.

## Queued annotation correction

In annotated `Btech/BTECH_1CD3.c`, case `0x2E` currently treats `3092:C624`
(`+0x10`) as a riding value and subtracts four from it. The underlying clean
assembly compares and subtracts from `3092:C623`, which is `Health` at `+0x0F`.
That pseudo-C block must eventually be corrected when source annotations are
edited; it is deliberately left unchanged during this documentation phase.

## Sources

- `BTech-Reko-expanded/BTECH.reko/BTECH_1467.asm`: pilot/rider assignment and
  mech-index dereference behaviour.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1CD3.asm`: training-bit tests, skill
  increments, and the `C623` health adjustment.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`: paired `C60F`/`C620`
  assignment loops that expose the pre-biased alias.
- `Btech/BTECH.h`: `Infantry` typedef, record constants, `Seg3092_t` arrays, and
  late `C614` field notes; names contradicted by assembly are retained only as
  historical evidence.
- `InceptionTools/Class/Infantry.cs`: existing parser.
- `UnBattletech-main/docs/reference/MEMORY_MAP.md`: imported matching field list.
