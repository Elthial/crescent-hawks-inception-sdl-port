# `BTECH_0FDC` enemy mech generation

## Review boundary

This block covers `Mission_GenerateEnemies` from `0FDC:0F55` through `1207`.
It constructs the requested enemy mech and pilot records, applies encounter
damage, initializes their combat display state, and installs the special
Spectator record. Enemy infantry generation begins at `1208` and is outside
this review.

The request decoding, spawn origin, and template selection are documented in
[`BTECH_0FDC_ENEMY_SPAWN_SELECTION.md`](BTECH_0FDC_ENEMY_SPAWN_SELECTION.md).

## Record and combatant numbering

Each loop iteration consumes one compact infantry record as the enemy mech's
pilot. Pilot records begin at index 8. The record's `+0x0C` byte is set to mech
record index `4 + slot`; the current C field name `Piloting` is historical and
does not describe this assignment use.

The copied mech occupies live mech record `4 + slot`, while its common
combatant ID is `12 + slot`. These are separate index spaces and should remain
separate in the C# port.

## Template copying and special cases

The normal path copies exactly `0x7D` bytes from the template selected by the
preceding block. A Kurita interruption instead copies the Jenner template for
every generated mech and clears the pre-existing-damage level to zero.

When rental-mech Arena flag `3092:E48E` is set, every nonzero loop slot overwrites
enemy mech record 5 with the `Spectator` template. The preceding count logic
limits this case to two mechs, so the effective target is slot 1. The original
performs the Spectator overwrite inside the byte-copy loop; preserving that
order avoids silently changing behaviour if the surrounding assumptions later
prove incomplete.

## Pre-existing damage

Byte `3092:D30F` is a damage-severity level, not a template selector. The
armour subtraction table at `3EDB:165A` is:

| Level | Armour removed from each current-armour byte | Additional damage |
|---:|---:|---|
| 0 | 0 | None |
| 1 | 1 | None |
| 2 | 3 | None |
| 3 | 5 | None |
| 4 | 10 | None |
| 5 | 14 | Five random critical-destruction attempts |
| 6 | 20 | Five attempts, then random engine, sensor, and gyro hits |

Each of the eleven current-armour bytes is saturated at zero rather than
allowed to underflow. A critical attempt chooses raw mech-record offset
`0x34 + (randomByte & 0x1F)`, covering `0x34..0x53`. It sets bit `0x80` only
when the chosen byte is nonzero. Empty slots and repeated choices mean five
attempts need not destroy five distinct components.

At level six the game consumes three more random bytes in this exact order:

1. `EngineHits` at `+0x75`;
2. `SensorHits` at `+0x77`;
3. `GyroHits` at `+0x76`.

Each stores only the random byte's low bit.

The assembly treats the severity byte as signed. Values `7..127` are clamped
to six, but bytes `0x80..0xFF` compare as negative and are sign-extended before
the table lookup, reading before `3EDB:165A`. Valid game state appears to be
`0..6`; a safe port should validate imported state explicitly rather than copy
this out-of-range memory read.

## Combat display state and formation

The generic initialization assigns:

- sprite frame `0x0C`;
- the sprite-family offset selected by the preceding block;
- animation cursor `2FE8:02A0`;
- spawn-state byte `6` and animation selector `6`;
- active/on-map word `1`;
- world position equal to the chosen origin plus the slot's signed formation
  offsets.

For the four possible enemy mechs, the verified formation offsets are
`(0,0)`, `(4,0)`, `(0,4)`, and `(4,4)`: a two-by-two arrangement around the
spawn origin.

## Special Spectator override

Special solo mode then replaces combatant 13's display state with:

- sprite frame `0x10` and sprite-family offset `0`;
- animation cursor `3EDB:4DD8`;
- spawn-state byte and animation selector `2`;
- fixed world position `0A06:8066`.

The combat target scanner separately excludes combatant 13 in this mode, so
this is a non-targetable Spectator rather than an ordinary second opponent.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:0F55-1207`;
- raw damage table bytes at `3EDB:165A`;
- the verified `0x7D` mech-record layout;
- combatant-array address mappings established in earlier `0800` and `0DAB`
  reviews.

The loop bound, record indexes, byte-copy size, damage table, critical range,
PRNG order, formation offsets, and Spectator override are directly verified.
The story-level reason for the Spectator remains open, but its non-targetable
runtime role is verified. No Astra review is required.

The final on-foot enemy generation block is documented in
[`BTECH_0FDC_ENEMY_INFANTRY_GENERATION.md`](BTECH_0FDC_ENEMY_INFANTRY_GENERATION.md).
