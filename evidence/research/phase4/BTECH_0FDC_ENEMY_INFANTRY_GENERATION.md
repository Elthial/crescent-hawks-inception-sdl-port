# `BTECH_0FDC` enemy infantry generation

## Review boundary

This block covers `Mission_GenerateEnemies` from `0FDC:1208` through its return
at `1345`. It allocates requested on-foot enemies after the mech and pilot
records constructed by the preceding blocks.

## Shared eight-record capacity

Enemy mech pilots and enemy infantry share compact infantry records `8..15`.
The next-free record index begins at eight and is incremented once for every
generated enemy mech. The low seven bits of the infantry request are then added
to that index, with the exclusive end capped at 16.

For the expected enemy-mech request range `0..4`, the number of on-foot enemies
actually produced is:

```text
min(requested infantry, 8 - generated enemy mechs)
```

The original tests the complete request WORD for zero before masking bit 7. A
request containing only flag `0x80` enters the allocation branch but creates no
infantry because its masked count is zero.

## Record and combatant mapping

For each allocated infantry record `r` in `8..15`, the common combatant ID is
`r + 8`, producing enemy-infantry combatants `16..23`. The routine writes:

- record name/status byte `0`;
- `MechAssignment = 8`, meaning on foot;
- grey enemy-infantry sprite-family offset `0xFE`;
- sprite frame `0x1C`;
- animation cursor `2FE8:02E0`, the west-facing direction stream within the
  infantry walk-animation block;
- spawn-state byte and animation selector `6`;
- active/on-map WORD `1`.

The spawn-state table at `3092:3928` is addressed with the compact record ID,
whereas the sprite, animation, active, and position tables use the common
combatant ID. The old pseudo-C mixed those index spaces and relied on interior
array aliases.

## Spawn formation

World coordinates are the previously selected encounter origin plus signed
byte offsets indexed by the compact infantry record. The eight possible
on-foot slots use:

| Infantry record | Combatant ID | X offset | Y offset |
|---:|---:|---:|---:|
| 8 | 16 | 1 | 2 |
| 9 | 17 | 2 | 2 |
| 10 | 18 | 0 | 3 |
| 11 | 19 | 1 | 3 |
| 12 | 20 | 0 | 0 |
| 13 | 21 | 0 | 1 |
| 14 | 22 | 4 | 3 |
| 15 | 23 | 4 | 5 |

These values come from overlapping address views at `3EDB:1642` and `164E`.
The upper X entries reuse bytes at the start of the Y region, and the upper Y
entries reuse bytes at the start of the damage table at `165A`. This is valid
original data packing, not evidence of separate sixteen-byte arrays in the
executable.

## Generated statistics and PRNG order

The routine consumes random values in this order for each on-foot enemy:

1. six `Rand() & 1` results for record offsets `+0x04..+0x09`;
2. one `2D6` result for Body;
3. one `Rand() % 14` result for Weapon.

Health is set to `Body * 10` without another random call. The six randomized
skills are Bows/Blade, Pistol, Rifle, Gunnery, Piloting, and Tech, each either
zero or one. Medical at `+0x0A` is not included. Weapon IDs are selected as a
random byte modulo 14, producing `0..13` and excluding only infantry weapon ID
14, the Flamer. Because 256 is not divisible by 14, IDs `0..3` have a slight
modulo bias: each has 19 source-byte values while IDs `4..13` each have 18.

Unlike random-encounter generation in `BTECH_0DAB`, this routine does not write
Dexterity, Charisma, Medical, ArmourType, ArmourValue, or TrainingFlags. Those
bytes retain their prior contents. The instruction behaviour is verified; it
is not yet established whether callers deliberately precondition these fields
or scripted enemies can inherit stale saved/random-encounter statistics. A C#
port should expose this as an explicit compatibility decision rather than
accidentally depending on object defaults.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:1208-1345`;
- initialized data bytes at `3EDB:1642-1660`;
- the verified 17-byte infantry record and 24-combatant array mappings;
- comparison with the separately reviewed `BTECH_0DAB` random-encounter
  generator.

The capacity rule, indexes, address overlaps, formation values, initialized
fields, and PRNG call order are directly verified. Whether retaining the other
record fields is intentional remains open. No Astra review is required.

The next function, which restores the exploration view after training combat,
is documented in
[`BTECH_0FDC_POST_COMBAT_VIEW_RESTORE.md`](BTECH_0FDC_POST_COMBAT_VIEW_RESTORE.md).
