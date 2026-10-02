# Mech record

## Summary

Mechs use a fixed **0x7D-byte (125-byte)** record. **V** for the stride: the
annotated live arrays advance from `3092:C724` to `C7A1`, and the reference
templates advance by `0x7D` beginning at `2FE8:02F0`.

The normalized layout below follows the clean 16-bit instructions, the explicit
late address notes at the end of `Btech/BTECH.h`, and the indexes used by the
old InceptionTools `Mech` parser. Clean code multiplies record indexes by
`0x7D`, accesses exactly two current actuator bytes at `+0x24/+0x25`, then uses
`+0x26` as the heat-sink count. The older four-byte actuator typedef is
therefore superseded.

## Byte layout

| Offset | Size | Confidence | Field | Notes |
|---:|---:|:---:|---|---|
| `0x00` | `0x10` | V/P | `NameAndStatus` | Null-padded mech name. The high bit of the first byte is probably the no-mech/destroyed marker; `0xFF` is the observed empty/destroyed sentinel. Name storage and sentinel use are verified, while the broader high-bit interpretation is probable. |
| `0x10` | 1 | V | `Tonnage` | Unsigned tonnage. |
| `0x11` | 11 | V | `CurrentArmour` | Eight front locations plus three rear-torso values. |
| `0x1C` | 8 | V | `CurrentStructure` | RA, RL, RT, head, CT, LA, LL, LT order in current notes. |
| `0x24` | 1 | V | `CurrentActuatorsLeft` | Packed left-side state: low nibble left leg, high nibble left arm. |
| `0x25` | 1 | V | `CurrentActuatorsRight` | Packed right-side state: low nibble right leg, high nibble right arm. |
| `0x26` | 1 | V | `EngineHeatSinks` | Subtracted from accumulated heat in combat. |
| `0x27` | 10 | V | `CurrentAmmo` | Ten bins. |
| `0x31` | 1 | V | `WalkMove` | Walking movement points. |
| `0x32` | 1 | V | `JumpMove` | Jump movement points. |
| `0x33` | 7 | V | `CriticalLeftArm` | Component IDs; high bit is used for destroyed state. |
| `0x3A` | 7 | V | `CriticalLeftTorso` | Component slots. |
| `0x41` | 7 | V | `CriticalRightArm` | Component slots. |
| `0x48` | 7 | V | `CriticalRightTorso` | Component slots. |
| `0x4F` | 2 | V | `CriticalLeftLeg` | Component slots. |
| `0x51` | 2 | V | `CriticalRightLeg` | Component slots. |
| `0x53` | 2 | V | `CriticalCenterTorso` | Component slots. |
| `0x55` | 1 | V | `CriticalHead` | Component slot. |
| `0x56` | 11 | V | `MaximumArmour` | Same location order as current armour. |
| `0x61` | 8 | V | `MaximumStructure` | Same location order as current structure. |
| `0x69` | 1 | V | `MaximumActuatorsLeft` | Repair/status reference for `+0x24`; same packed-nibble interpretation. |
| `0x6A` | 1 | V | `MaximumActuatorsRight` | Repair/status reference for `+0x25`. |
| `0x6B` | 10 | V | `MaximumAmmo` | Ten bins. |
| `0x75` | 1 | V | `EngineHits` | Incremented by engine criticals; three hits destroy the centre torso/mech. Also contributes `hits × 5` heat. |
| `0x76` | 1 | V | `GyroHits` | Incremented by gyro criticals; two hits destroy the centre torso/mech. |
| `0x77` | 1 | V | `SensorHits` | Incremented by head criticals; capped at two. |
| `0x78` | 1 | P | `LifeSupportState` | Selected by head-critical rolls 1 and 6; a non-zero initial value is replaced with `0xFF` on a hit. |
| `0x79` | 1 | V | `PilotId` | Used to index the character array; `0xFF` means no pilot. |
| `0x7A` | 1 | V | `RiderId` | Optional second occupant; `0xFF` means no rider. |
| `0x7B` | 1 | P | `UpgradePackageBase` | Values `0..3` select the chassis' first-stage package; values above three route to no available package. |
| `0x7C` | 1 | V | `UpgradeLevelFlags` | `0` unmodified, bit `0` first-stage upgrade, bit `1` second-stage upgrade; observed completed value `3`. |

The ranges above total exactly `0x7D` bytes.

## Encounter pre-damage

Enemy generation at `0FDC:0F55-1207` copies the complete reference record and
then subtracts a severity-dependent amount from each of the eleven current
armour bytes. Severity levels five and six also attempt five random critical
slot destructions; level six additionally initializes the engine, sensor, and
gyro hit bytes to independent random values of zero or one. See
[`BTECH_0FDC_ENEMY_MECH_GENERATION.md`](../phase4/BTECH_0FDC_ENEMY_MECH_GENERATION.md)
for the exact table and PRNG order.

## Sol: armour and structure location indices

The EXE-owned hit-location offsets at `3EDB:3242` and their accompanying
text pointers at `324E` establish these names. Front armour and structure
share indices0..7; rear armour has no additional structure entries.
This does not resolve the separate critical-group anatomy dispute.

| Array index | Location | Current armour offset | Current structure offset |
| --- | --- | --- | --- |
|0|Left arm|11h|1Ch|
|1|Left torso|12h|1Dh|
|2|Left leg|13h|1Eh|
|3|Head|14h|1Fh|
|4|Center torso|15h|20h|
|5|Right arm|16h|21h|
|6|Right torso|17h|22h|
|7|Right leg|18h|23h|
|8|Rear left torso|19h|Uses left torso structure|
|9|Rear center torso|1Ah|Uses center torso structure|
|10|Rear right torso|1Bh|Uses right torso structure|

Preservation C `mech.h` now provides `MechLocation_*` indices and the
`MechOffset_Armour_*` / `MechOffset_Structure_*` constants required by the
raw-offset transfer routine. They are derived from `offsetof`, not pointers
or replacement data tables. An array index must not be passed where the
original routine expects a record BYTE offset.

`1631:1122` transfers destroyed limb structure to the corresponding FRONT
torso armour, then destroyed side-torso structure to FRONT center-torso
armour. Those original destinations remain unchanged, even though they are
not direct structure-to-structure transfers. Head/center-torso structure
sets the destruction flag. The existing execution test keeps independent
hexadecimal expected offsets for every defined case rather than deriving
its expected values from these new names.

## Name/status high bit

The first byte of the name has two roles. Its text bits normally begin the
chassis name, while bit `0x80` is **Probable** as a no-mech or destroyed-mech
marker. Completely unused/destroyed slots use the verified `0xFF` sentinel.
New-game reset at `0800:4DC7` writes `0xFF` specifically to byte zero of all
eight live mech records at `3092:C724 + index * 0x7D`. Saved enemy records
include literal sequences such as `FF 4F 43 55 53 54`
(`FF` + `OCUST`), so setting the marker can overwrite the original first letter
rather than merely OR `0x80` into it. This explains text-only decodes such as
`?OCUST` and means the missing letter must not be guessed generically.

InceptionTools recognizes a missing first letter only when the remaining suffix
exactly matches one of the eight verified reference chassis names. This is a
bounded presentation/editing convenience rather than evidence that the byte
itself retains the lost character.

Until the complete lifecycle is traced, readers must preserve the raw first byte
and expose the bit separately. They must not assume that every set high bit means
one narrower state such as merely hostile, salvageable, or permanently absent.

## Packed actuator bytes

Repair code at `11B8:011F` compares `+0x24` with `+0x69` and `+0x25` with
`+0x6A`, and later copies those maximum bytes back during repair. BTSTATS at
`0DAB:1F86..21BE` labels the low nibble of `+0x24` Left Leg, the low nibble of
`+0x25` Right Leg, the high nibble of `+0x24` Left Arm, and the high nibble of
`+0x25` Right Arm. This proves both the limb ownership and the four-logical-limb
representation packed into two bytes.

Within a nibble, set bits represent available/working actuators and cleared bits
represent damage or absence. Combat movement tests bit `0x08` on both current
bytes, while `Combat_Check_Mech_Actuator_Status` counts missing bits `0x04`,
`0x02`, and `0x01` as penalties. A full bit-to-specific-actuator map—hip,
upper/lower leg, foot, shoulder, upper/lower arm, and hand—still needs to be
matched to critical-hit cases.

BTSTATS treats a leg as intact when its current low nibble is `0x0F`. It treats
an arm as intact when its current high nibble equals the corresponding maximum
high nibble. The maximum comparison is significant because reference designs
use both `0xC0` and `0xF0` for intact arm configurations.

The Mech-Lube repair diagnostic compares each complete current packed byte with
its corresponding maximum byte. A mismatch in the left byte sets repair flag
bit zero and a mismatch in the right byte sets bit one. These flags identify
sides requiring actuator repair; they are not four separate limb fields. See
[`BTECH_11B8_MECHLUBE_REPAIR_DIAGNOSTICS.md`](../phase4/BTECH_11B8_MECHLUBE_REPAIR_DIAGNOSTICS.md).

If either flag is set, the repair transaction charges one flat 200-C-bill fee
and copies both maximum packed bytes back to their current fields—right first,
then left. It does not fill them with `0xFF`; chassis maxima such as `0xC0`
must be preserved. See
[`BTECH_11B8_MECHLUBE_ACTUATOR_REPAIR.md`](../phase4/BTECH_11B8_MECHLUBE_ACTUATOR_REPAIR.md).

## Terminal damage and occupancy bytes

The last eight bytes are runtime state rather than unexplained padding:

- `EngineHits`, `GyroHits`, and `SensorHits` are explicit critical-damage
  counters with destruction thresholds of three, two, and two respectively;
- `LifeSupportState` is reached by the two life-support positions in the
  six-way head critical table. The association is **Probable** because the
  instruction flow matches the BattleTech head table exactly, although the game
  stores an unusual `1` to `0xFF` transition;
- `PilotId` and `RiderId` are repeatedly dereferenced into the `0x11`-byte
  character array during mounting, dismounting, combat, and party changes;
- `UpgradePackageBase` selects the chassis-specific modification package;
- `UpgradeLevelFlags` is written by `Mechlube_Upgrade_Mech`: first-stage cases
  assign `1`, second-stage cases OR in `2`, and value `3` blocks further upgrades.

Modification setup supports package-base values `0..3`, mapped respectively to
Locust, Wasp, Stinger, and Commando. Exact level value `1` adds four to select
the corresponding second-stage package; exact value `3` refuses further work.
Any package-base value above three is clamped to sentinel selector `8`. The
native routine then performs an original out-of-bounds price read for that
sentinel; see
[`BTECH_11B8_MECHLUBE_MODIFICATION_SETUP.md`](../phase4/BTECH_11B8_MECHLUBE_MODIFICATION_SETUP.md)
and original-game `BUG-010`.

The Locust first-stage case replaces `Critical_L_Arm[0]` and
`Critical_R_Arm[0]` with medium lasers, sets current and maximum ammo-state
slots zero and one to the `0xFF` energy-weapon sentinel, and assigns
`UpgradeLevelFlags = 1`. See
[`BTECH_11B8_MECHLUBE_LOCUST_STAGE_ONE.md`](../phase4/BTECH_11B8_MECHLUBE_LOCUST_STAGE_ONE.md).

All eight package mutations are now instruction-verified. First-stage cases
assign `UpgradeLevelFlags = 1`; second-stage cases preserve stage one and OR in
bit `2`, producing the terminal value `3`. Armour replacement draws from either
the reference Locust's eleven-byte profiles or the embedded Commando stage-one
profile at `3EDB:1E24`. See
[`BTECH_11B8_MECHLUBE_UPGRADE_PACKAGES.md`](../phase4/BTECH_11B8_MECHLUBE_UPGRADE_PACKAGES.md).

The Mech-Lube checks `EngineHits`, `GyroHits`, and `SensorHits` as damage its
facility cannot repair. It checks them only when armour, structure, weapon,
heat-sink, and actuator damage are all absent, and reports only the first
nonzero counter in that order.

## Mech-Lube armour repair

The shop totals the differences between the eleven maximum and current armour
bytes, quotes exactly four C-bills per missing point, and repairs the locations
in stored record order. Each point is an independent transaction: subtract
four from the 32-bit balance, increment the current armour byte, and decrement
the WORD total. Consequently, running out of money retains every point already
installed and leaves the remainder damaged. See
[`BTECH_11B8_MECHLUBE_ARMOUR_REPAIR.md`](../phase4/BTECH_11B8_MECHLUBE_ARMOUR_REPAIR.md).

Internal structure follows the same transaction model across its eight bytes,
but costs nine C-bills per point. The shop reads `MaxStructure`, increments
`CurrentStructure`, and retains partial work if funds run out. It does not use
either armour array for this stage. See
[`BTECH_11B8_MECHLUBE_STRUCTURE_REPAIR.md`](../phase4/BTECH_11B8_MECHLUBE_STRUCTURE_REPAIR.md).

## Important arrays

| Location | Count | Confidence | Purpose |
|---|---:|:---:|---|
| `2FE8:02F0` | 8 records | P | Reference mech templates: Locust, Wasp, Stinger, Commando, Chameleon, Jenner, Spectator, UrbanMech. |
| `3EDB:13E2` | 11 far pointers | V | Enemy template lookup indexed by `2D6 - 2`; see the [spawn-selection review](../phase4/BTECH_0FDC_ENEMY_SPAWN_SELECTION.md). |
| `3EDB:140E` | 11 bytes | V | Sprite-family offsets paired with the `13E2` template pointers. |
| `3EDB:2DF8` | 5 far pointers | V | Selected template pointers: Locust, Wasp, Stinger, Commando, and Jenner. This is not a pointer to every contiguous reference record. |
| `3092:C33C` | 4 records | P | Destroyed/saved mech records. |
| `3092:C724` | 4 records | V | Player lance mech slots. |
| `3092:C918` | 4 records | V | Enemy mech slots. |

The eighth reference record begins at `0x065B`, exactly seven strides after
`0x02F0`, so its zero-based template index is `0x07`. Arena-spawn code copies
exactly `0x7D` bytes directly from `2FE8:065B` into two enemy records. The
historical `MECH_REF_UrbanMech = 0x08` definition is not supported as a template
array index and should not be used by future loaders.

## Arena staging lifecycle

The Arena temporarily uses live party-mech slot zero as its player combatant.
For a party-owned entrant, setup saves slot zero, copies the selected complete
record into slot zero, and installs Jason as pilot. Normal cleanup copies all
post-combat bytes back to the selected roster slot, propagates a destroyed
`Name[0]` to the cached roster marker at `3092:D452 + selected`, hides the live
selected record with `Name[0] = 0xFF`, and restores the original slot zero when
the selection came from another slot. It then restores all four original
pilot/rider pairs.

For a rental entrant, setup copies the complete reference Locust at
`2FE8:02F0` into slot zero. Cleanup simply replaces that temporary record with
the complete slot-zero backup. See
[`BTECH_0FDC_ARENA_RENTAL_MECH_SETUP.md`](../phase4/BTECH_0FDC_ARENA_RENTAL_MECH_SETUP.md)
and
[`BTECH_0FDC_ARENA_POST_COMBAT_RESTORE.md`](../phase4/BTECH_0FDC_ARENA_POST_COMBAT_RESTORE.md).

## Component IDs

Known component IDs currently cover `0x10` through `0x22`: small/medium/large
laser, PPC, autocannons, machine gun, flamer, LRM and SRM launchers, kick, and
heat sink. `0x80` is used as a destroyed flag, so readers must preserve the raw
byte and expose base ID and state separately.

Mech-Lube recognizes a destroyed heat sink only as the exact critical-slot byte
`0xA2` (`HeatSink 0x22 | Destroyed 0x80`). Repair costs 800 C-bills per slot
and replaces that byte with intact `0x22`; it does not alter the separate
`EngineHeatSinks` byte at `+0x26`. See
[`BTECH_11B8_MECHLUBE_HEAT_SINK_REPAIR.md`](../phase4/BTECH_11B8_MECHLUBE_HEAT_SINK_REPAIR.md).

Destroyed weapon bytes `0x90..0xA0` are counted by component type and offered
at 300 C-bills per counted slot. The original transaction has two bookkeeping
defects: one selection clears every matching raw slot while decrementing the
count only once, and its remaining-type scan omits SRM-6 index `0x10`. See
[`BTECH_11B8_MECHLUBE_WEAPON_REPAIR.md`](../phase4/BTECH_11B8_MECHLUBE_WEAPON_REPAIR.md)
and original-game `BUG-008/009`.

See [WEAPON_RECORDS.md](WEAPON_RECORDS.md) for the verified conversion between
mech component IDs and zero-based weapon-table indexes.

## InceptionTools implementation

Phase 3D adds the canonical `Records.MechRecord` parser. It validates the
`0x7D` length, reads all 16 name bytes, exposes `EngineHits` through
`UpgradeLevelFlags`, retains the raw record, exposes the probable first-byte
no-mech/destroyed marker without discarding the raw value, and provides neutral
packed actuator byte/nibble views. The neutral API predates the later verified
left/right and arm/leg mapping and remains a lossless representation. The old
`Mech.Actuators` four-entry dictionary is retained only as an obsolete
compatibility view and must not be used for new logic.

## Sources

- `Btech/BTECH.h`: mech constants and offsets; typedef; `seg2FE8_t` reference
  records; `Seg3092_t` live arrays; late `C724` address-by-address notes.
- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`: repair comparisons and the
  modification-package workflow.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1631.asm`: critical damage, terminal
  counters, life-support branch, actuator nibble operations, and `0x7D` strides.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1AE8.asm`: combat actuator and UrbanMech
  template-copy evidence.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0DAB.asm`: BTSTATS limb labels, status
  comparisons, and status strings.
- `InceptionTools/Data/Battlemechs/*.cs`: captured template records.
- `InceptionTools/Class/Mech.cs`: existing byte indexes.
- `UnBattletech-main/docs/reference/MEMORY_MAP.md`: imported competing layout.
