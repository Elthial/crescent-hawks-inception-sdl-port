# `BTECH_0800` new-game combatant display state

## Reviewed block

- Address range: `0800:5025-50C7`.
- Parent routine: currently `Load_Game_Map_Data`.
- `Start_Game` begins at `0800:50C8`.

This is the final block of new-game construction. It initializes animation
state for every combatant, draws the starting units, and establishes the
initial story/runtime flags.

## Two complete 24-byte tables

The addressing resolves two sets of misleading, overlapping declarations in
`BTECH.h`:

| Address | Maintained field | Meaning |
|---|---|---|
| `396C-3983` | `CombatantAnimationSelector_396C[24]` | current/requested animation-stream selector |
| `409A-40B1` | `CombatantSpriteFrame_409A[24]` | current frame byte returned by that stream |

`3978` and `40A6` are exactly 12 bytes after their respective bases. They are
the enemy halves of the arrays, not independent fields. Likewise, old names at
`3974`, `397C`, `40A8`, and `40AA` were interior address views.

The loop interleaves writes to equivalent positions in the friendly and enemy
halves:

| Combatant IDs | Group | Initial frame |
|---|---|---:|
| `00-03` | friendly mechs | `00` |
| `04-0B` | party infantry | `10` |
| `0C-0F` | enemy mechs | `00` |
| `10-17` | enemy infantry/map NPCs | `10` |

Every animation selector is set to `FF`. The animation-stream reader treats a
changed selector as a request to select/restart the appropriate stream, so the
sentinel forces first-use initialization rather than naming a sprite frame.

## Initial draw and flags

After initializing the two arrays, `0800:051B` draws infantry and mechs over
the terrain prepared by the preceding block. The routine then establishes:

| State | Initial value |
|---|---:|
| viewed holodisk (`2FE8:0064`, byte) | `0` |
| transmitted Cache discovery (`3EDB:01A8`, word) | `0` |
| draw Jailbreak parked mechs (`3092:398E`, word) | `0` |
| traitor warning (`3092:374A`, word) | `0` |
| current map (`2FE8:00FC`, byte) | Citadel / `1` |
| main characters alive (`3EDB:014A`, word) | `1` |

The assembly explicitly uses byte stores for the holodisk and current-map
fields and sign-extends `AL=0/1` before the word stores. No Astra confirmation
is needed for the table boundaries, initialization values, or flag widths.
