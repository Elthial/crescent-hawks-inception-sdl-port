# `BTECH_0DAB` encounter origin and enemy infantry generation

## Reviewed range

- Address range: `0DAB:0D3D-0F0A`.
- Enemy 'Mech generation begins separately at `0DAB:0F0B`.

## Encounter search origin

The routine first refreshes the cached map origin around the party. It then
creates independent signed X and Y offsets. Each magnitude is `(Rand() & 7) +
10`, giving `10..17`, and a separate random bit chooses whether that axis is
positive or negative.

The later placement scan starts at:

```text
scan X = signed X offset + 26
scan Y = signed Y offset + 12
```

All of these values are native 16-bit signed locals. The earlier annotated C
incorrectly narrowed them to unsigned bytes, which destroyed the meaning of
the negation and could not faithfully describe the later scan.

## Enemy combatant reset

Combatant slots `12..23` are cleared before new enemies are placed:

- the word active/on-map entry at `3092:406A + id*2` becomes zero;
- the word Y coordinate at `3092:4036 + id*2` becomes `FFFF`;
- the word X coordinate at `3092:4004 + id*2` becomes `FFFF`.

`FFFF` is therefore the off-map coordinate sentinel. It must not be confused
with the byte-sized `FF` destroyed-'Mech/name sentinel, even though both are
represented as minus one at their native width.

## Enemy infantry records

The routine visits infantry records `8..15`. It first sets each record's name
byte to `FF` (dead/unused), then gives the slot an independent 50% chance of
being populated. Consequently a random encounter can initially generate zero
through eight infantry.

A populated record receives:

- name value `01` (the exact display semantics remain unresolved);
- piloting/riding state `08`, meaning on foot;
- Body and Dexterity from separate `2d6` calls;
- Health equal to `Body * 10`;
- seven consecutive skill bytes, Bows/Blade through Medical, each `Rand() & 3`;
- armour type `0..3`;
- armour value from two further `2d6` calls, giving `4..24`.

The call sequence matters for deterministic reconstruction: presence is rolled
first, followed by seven weapon rolls, Body, Dexterity, seven skills, armour
type, and the two armour-value rolls.

## Weapon selection table

The weapon selector is not a division, as the former pseudo-C suggested. It
sums seven independent `Rand() & 3` results, giving an index `0..21`, and looks
that index up in the byte table at `3EDB:2CF4-2D09`:

| Roll sum | Weapon ID | Weapon |
|---:|---:|---|
| 0 | `0A` | SRM |
| 1 | `0C` | Laser pistol |
| 2 | `09` | SMG |
| 3 | `07` | Pistol |
| 4 | `00` | Cudgel |
| 5 | `08` | Rifle |
| 6 | `01` | Knife |
| 7 | `03` | Vibroblade |
| 8 | `05` | Longbow |
| 9 | `07` | Pistol |
| 10 | `07` | Pistol |
| 11 | `08` | Rifle |
| 12 | `06` | Crossbow |
| 13 | `04` | Shortbow |
| 14 | `03` | Vibroblade |
| 15 | `02` | Sword |
| 16 | `00` | Cudgel |
| 17 | `00` | Cudgel |
| 18 | `08` | Rifle |
| 19 | `09` | SMG |
| 20 | `0D` | Laser rifle |
| 21 | `0B` | Inferno |

Because this is a sum of seven rolls rather than a uniform index, the middle
entries are much more common than the extreme entries. The unusual rare
weapons at sums 0 and 21 are therefore reachable, but only when all seven
two-bit rolls produce the same extreme value.

## Confidence

The control flow, widths, table bytes, and record offsets are directly proven
by the clean assembly and initialized data image. Only the semantics of name
value `01` remain unresolved. No Astra review is warranted for this block.
