# `BTECH_0800` combatant sprite-family initialization

## Scope

This note covers `0800:4F0F-4F6B` in `Load_Game_Map_Data` and the data at
`3092:D55E-D575`. The assembly proves that this is one contiguous 24-byte
array, not the three overlapping variables previously inferred from Reko.

## Table layout

Each byte is indexed by the world/combatant ID used by the position and
animation tables:

| Combatant IDs | Meaning | Record-to-combatant conversion |
|---|---|---|
| `00-03` | friendly lance mechs | mech record ID unchanged |
| `04-0B` | party/on-foot infantry | infantry record ID `+ 4` |
| `0C-0F` | enemy mechs | mech record ID `+ 8` |
| `10-17` | enemy infantry/map NPCs | infantry record ID `+ 8` |

The earlier names at `D562` and `D566` were interior pointers into this table:
`D562` is entry four and `D566` is entry eight. They are useful address clues
in the assembly, but they must not be represented as independent C fields.

## Stored values

The byte is a sprite-family base added to the combatant's current animation
offset before indexing the far-pointer table at `3092:39FA`.

| Value | Meaning established so far |
|---|---|
| `00` | Locust-family mech artwork |
| `92` | upright/humanoid mech artwork (Commando, Wasp, Stinger, and similar) |
| `96` | Jason's light-blue on-foot artwork |
| `FE` | generic grey enemy/civilian infantry artwork |

These are byte offsets/indices, not 16- or 32-bit pointers. Reko's wider types
must not be carried into a C# port.

## New-game initialization

The block performs three operations:

1. It writes `96` to entry four, Jason's on-foot combatant slot.
2. It fills entries `10-17` with `FE` for enemy infantry and map NPCs.
3. For mech positions `0-3`, it makes two independent random tests: one for
   the friendly entry and one for the corresponding enemy entry at `+0C`.
   An odd random byte writes `92`.

The even-random path contains no store. It leaves the existing family value
unchanged rather than explicitly selecting Locust family `00`. That distinction
is exact to the executable and should be preserved until the surrounding data
initialization and every call path are understood.

## Cross-checks outside this block

- `1631` maps dead infantry record IDs into the same combatant-indexed table;
  it restores `96` for Jason and `FE` for enemy infantry.
- `0DAB`, `0FDC`, and `11B8` address the table through the old `D566` interior
  base. Adding eight to their mech/infantry record ID yields the combatant ID.
- `1AE8` writes `D56C` and `D56D` directly when spawning two arena UrbanMechs;
  these are combatant entries 14 and 15.

No Astra confirmation is currently required for this block: the addressing,
loop bounds, byte widths, and stored constants are explicit in the clean ASM.
