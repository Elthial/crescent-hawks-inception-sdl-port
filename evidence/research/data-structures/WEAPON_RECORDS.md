# Weapon records and component arrays

## Record shape

The expanded executable contains **33 weapon records with a `0x11`-byte
(17-byte) stride** at `3EDB:2ED8..3108`; the exclusive end is `3EDB:3109`.
The last record, Kick, begins at `3EDB:30F8`. The table bytes are visible
directly in the clean `BTECH_3EDB.asm` segment dump and match the captured bytes
in `InceptionTools/Data/WeaponsArray.cs`.

The old `BTECH.h` field comments beginning at `2ED2` are stale. The actual first
record begins at `2ED8`, so its fields occupy `2ED8..2EE8`.

## Verified field layout

| Offset | Size | Confidence | Field | Observed use |
|---:|---:|:---:|---|---|
| `0x00` | 11 | V | `Name[11]` | Fixed-width, NUL-padded display name. Ten-character names use the eleventh byte as the terminator. |
| `0x0B` | 1 | V | `DamageEncoding` | Direct damage for ordinary mech attacks; nibble-packed dice expression for personnel weapons. |
| `0x0C` | 1 | V | `AttackCountOrClusterColumn` | Bit `0x80` selects personnel damage encoding. The low seven bits are a repeated-attack count for personnel weapons or the missile cluster-table column selector. |
| `0x0D` | 1 | V/U | `HeatAndEffect` | Low nibble is heat added on firing. Meaning of the high nibble is unknown. |
| `0x0E` | 1 | V | `PackedRangeThresholds` | Bits `7..5` are the short-range threshold; bits `4..0` are the medium-range threshold. |
| `0x0F` | 1 | V | `MaximumRange` | Compared directly with calculated target distance to establish long versus out-of-range. |
| `0x10` | 1 | V | `SkillIndex` | Index into the attacking character's skill array. |

This resolves the former 10-byte-name interpretation: combat instructions read
the first record's fields at `2EE3..2EE8`, which are offsets `+0x0B..+0x10` from
the verified `2ED8` base.

## Damage and attack-count encoding

`Combat_Mechanics` at `1AE8:000C` branches on bit `0x80` of
`AttackCountOrClusterColumn`:

- if the bit is set, the low nibble of `DamageEncoding` is a fixed damage
  bonus and its high nibble is the number of D6 rolls added to that bonus. The
  low seven bits of `AttackCountOrClusterColumn` repeat that complete attack,
  explaining `0x81` for most personnel weapons and `0x84` for MachineGun;
- if the bit is clear and the low seven bits are `0` or `1`,
  `DamageEncoding` is applied directly;
- if the bit is clear and the low seven bits are `2..8`, that value selects a
  column in the missile cluster-hit table and the resulting hit count is
  multiplied by `DamageEncoding`.

### Missile cluster-hit table

The combat calculation uses this exact index expression:

```text
hitCount = byte[3EDB:2E5E + (TwoD6Roll * 7) + clusterColumn]
damage   = hitCount * DamageEncoding
```

`TwoD6Roll` returns `2..12`, and cluster columns `2..8` make the effective
77-byte table occupy `3EDB:2E6E..2EBA`. The column meanings follow directly
from the launchers that store each selector:

| 2D6 | `2` SRM 2 | `3` SRM 4 | `4` LRM 5 | `5` SRM 6 | `6` LRM 10 | `7` LRM 15 | `8` LRM 20 |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 2 | 1 | 1 | 1 | 2 | 3 | 5 | 6 |
| 3 | 1 | 2 | 2 | 2 | 3 | 5 | 6 |
| 4 | 1 | 2 | 2 | 3 | 4 | 6 | 9 |
| 5 | 1 | 2 | 3 | 3 | 6 | 9 | 12 |
| 6 | 1 | 2 | 3 | 4 | 6 | 9 | 12 |
| 7 | 1 | 3 | 3 | 4 | 6 | 9 | 12 |
| 8 | 2 | 3 | 3 | 4 | 6 | 9 | 12 |
| 9 | 2 | 3 | 4 | 5 | 8 | 12 | 16 |
| 10 | 2 | 3 | 4 | 5 | 8 | 12 | 16 |
| 11 | 2 | 4 | 5 | 6 | 10 | 15 | 20 |
| 12 | 2 | 4 | 5 | 6 | 10 | 15 | 20 |

## Range encoding

`Combat_Calculate_RangeBracket` at `1631:0F24` decodes the fields as follows:

```text
shortThreshold  = PackedRangeThresholds >> 5
mediumThreshold = PackedRangeThresholds & 0x1F
maximumRange    = MaximumRange
```

For mech weapons—`AttackCountOrClusterColumn < 0x80`, excluding the Kick table
record—the short and medium thresholds are multiplied by three. Their stored
maximum ranges are already in the scaled distance domain and are not
multiplied. The comparisons are strict `<` for short and medium, and
`MaximumRange > distance` for long; distance exactly equal to a threshold enters
the next bracket.

Examples from the original table:

| Weapon | Packed byte | Short | Medium | Maximum |
|---|---:|---:|---:|---:|
| Shortbow | `0x66` | 3 | 6 | 9 |
| Small laser | `0x43` | 2 × 3 = 6 | 3 × 3 = 9 | 12 |
| Medium laser | `0x87` | 4 × 3 = 12 | 7 × 3 = 21 | 30 |
| PPC | `0xED` | 7 × 3 = 21 | 13 × 3 = 39 | 57 |

## Table indexes and component IDs

The weapon table index and the mech critical-slot component ID are different
domains. Treating them as one ID caused several historical off-by-one comments.

- table indexes are `0x00..0x20` for the 33 records;
- infantry equipment records `0x00..0x0E` use their direct table index;
- mech critical-slot IDs `0x10..0x21` map to table indexes `0x0F..0x20` using
  `tableIndex = componentId - 1`;
- heat sink component ID `0x22` has no weapon-table record;
- critical-slot component bit `0x80` marks a destroyed component and must be
  removed before interpreting its base component ID.

The clean combat code explicitly compares a weapon table index against
`Mech_Kick - 1`, corroborating the conversion already warned about in
`BTECH.h`.

| Table index | Component/equipment ID | Stored name |
|---:|---:|---|
| `0x00` | `0x00` | Cudgel |
| `0x01` | `0x01` | Knife |
| `0x02` | `0x02` | Sword |
| `0x03` | `0x03` | VibroBlade |
| `0x04` | `0x04` | Shortbow |
| `0x05` | `0x05` | Longbow |
| `0x06` | `0x06` | Crossbow |
| `0x07` | `0x07` | Pistol |
| `0x08` | `0x08` | Rifle |
| `0x09` | `0x09` | MachineGun |
| `0x0A` | `0x0A` | SR Missile |
| `0x0B` | `0x0B` | Inferno |
| `0x0C` | `0x0C` | LaserPistl |
| `0x0D` | `0x0D` | LaserRifle |
| `0x0E` | `0x0E` | Flamer |
| `0x0F` | `0x10` | SmallLaser |
| `0x10` | `0x11` | Med Laser |
| `0x11` | `0x12` | LargeLaser |
| `0x12` | `0x13` | PPC |
| `0x13` | `0x14` | AutoCann/2 |
| `0x14` | `0x15` | AutoCann/5 |
| `0x15` | `0x16` | AutoCann10 |
| `0x16` | `0x17` | AutoCann20 |
| `0x17` | `0x18` | MachineGun |
| `0x18` | `0x19` | Flamer |
| `0x19` | `0x1A` | LRMissile5 |
| `0x1A` | `0x1B` | LRMissil10 |
| `0x1B` | `0x1C` | LRMissil15 |
| `0x1C` | `0x1D` | LRMissil20 |
| `0x1D` | `0x1E` | SRMissile2 |
| `0x1E` | `0x1F` | SRMissile4 |
| `0x1F` | `0x20` | SRMissile6 |
| `0x20` | `0x21` | Kick |

Names in this table preserve the spellings stored in the executable rather than
silently correcting or expanding them.

## Mech-Lube weapon-name lookup

The repair menu uses a zero-based mech weapon-type index `0x00..0x10`, adds
table index `0x0F`, then reads the name from the resulting `0x11`-byte record.
Its first display record is therefore SmallLaser at `3EDB:2FD7`, and its last
is SRMissile6 at `3EDB:30E7`. The parallel damaged-component byte is obtained by
adding `0x90`, producing raw bytes `0x90..0xA0`.

See
[`BTECH_11B8_MECHLUBE_WEAPON_REPAIR.md`](../phase4/BTECH_11B8_MECHLUBE_WEAPON_REPAIR.md)
for the menu mapping and the original transaction bookkeeping defects.

## Related mech arrays

Each mech record has ten current-ammo bytes at `record+0x27`, ten maximum-ammo
bytes at `record+0x6B`, and critical component IDs in `record+0x33` through
`record+0x55`. `Mechlube_Buy_Ammo` scans those critical bytes in address order,
collects at most ten weapon-valued bytes (`0x10..0x20` after masking the
destroyed bit), and uses each collected ordinal directly as the index into both
ammo arrays. This verifies the local array association. Whether repeated
critical bytes represent separate weapons or multi-slot occupancy remains an
explicit investigation item; see
[`BTECH_11B8_MECHLUBE_AMMO_PURCHASE.md`](../phase4/BTECH_11B8_MECHLUBE_AMMO_PURCHASE.md).

The combat UI at `1543:0004` instead collects up to twelve weapon-valued
critical bytes and has twelve planned target/action slots per combatant. Slot
11 is also used for kicking. These are not twelve ammo bins: raw ammo reads
for slots ten and eleven alias `WalkMove` and `JumpMove`, and a full twelve-entry
weapon list loses its string terminator (BUG-012). See the
[1543 segment review](../phase4/BTECH_1543_SEGMENT_REVIEW.md); A-004 retains the
weapon-instance/repeated-critical-occupancy question.

## InceptionTools implementation

Phase 3D adds the canonical `Records.WeaponRecord` parser. It validates the
`0x11` length, reads all eleven name bytes, retains the raw record, exposes the
packed damage/attack/heat/range fields, and keeps table indexes distinct from
component IDs. The legacy `Weapon` class now delegates to this model.

## Sources

- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`: verified table bytes at
  `3EDB:2ED8..3108`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1631.asm`: range decoding in
  `1631:0F24`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1AE8.asm`: damage, heat, skill, and
  attack-count/cluster accesses in `Combat_Mechanics` at `1AE8:000C`.
- `Btech/BTECH_1631.c` and `Btech/BTECH_1AE8.c`: hand-annotated interpretations
  cross-checked against the clean ASM.
- `Btech/BTECH.h`: record size, IDs, historical field names, and the mech-ID
  offset warning.
- `InceptionTools/Data/WeaponsArray.cs`: captured 33-record byte table and
  current parser behaviour.
- `UnBattletech-main/docs/reference/MEMORY_MAP.md`: superseded competing layout;
  retained as historical supporting research rather than address authority.
