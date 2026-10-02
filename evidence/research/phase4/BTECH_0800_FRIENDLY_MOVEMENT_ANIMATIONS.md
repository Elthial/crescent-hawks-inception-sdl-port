# `BTECH_0800` friendly movement animations

## Review block (`0800:231D-240A`)

The routine formerly called `Cycle_Through_Characters_231D` does not select or
cycle the controlled character. It has been renamed
`Update_Friendly_Movement_Animations_231D`: a recognised movement command
selects a compass-direction walk stream for every friendly combatant and
advances each applicable stream by one frame.

## Direction selection

The input is a 16-bit command word. `DS:0160` contains the eight movement
commands in this order:

| Index | Direction | Command |
|---:|---|---:|
| 0 | north | `FFB8` |
| 1 | north-east | `FFB7` |
| 2 | east | `FFB3` |
| 3 | south-east | `FFAF` |
| 4 | south | `FFB0` |
| 5 | south-west | `FFB1` |
| 6 | west | `FFB5` |
| 7 | north-west | `FFB9` |

If none matches, the routine changes no animation state. The old `char`
signature would truncate these values, so this routine and the keyboard
converter are now annotated as returning/accepting 16-bit words.

## Animation pointer tables

Three consecutive far-pointer tables are now represented explicitly in
`BTECH.h`:

| Address range | Entries | Meaning |
|---|---:|---|
| `3EDB:01F6-0259` | 25 | Mutable animation cursor for combatant/effect IDs `00-18`. |
| `3EDB:025A-0279` | 8 | Mech walk-stream start by direction. |
| `3EDB:027A-0299` | 8 | Infantry walk-stream start by direction. |

Each entry is a four-byte 16:16 far pointer. The mech table points to
`2FE8:0270, 0278, ... 02A8`; the infantry table points to
`2FE8:02B0, 02B8, ... 02E8`.

This also resolves two false standalone fields at `3EDB:022E` and `0232`.
They are cursor entries 14 and 15 within the 25-entry table—the combatant IDs
used by the two arena UrbanMechs—not separate UrbanMech data pointers.

The former `MapPos_0279` through `MapPos_027C` declarations were also attached
to `3EDB` incorrectly and overlapped these verified pointer bytes. Map-renderer
scratch bytes with those offsets belong to segment `246C`; their source
references and declarations have been moved there.

## Friendly combatant passes

The first pass covers friendly mech IDs `0-3`. A mech participates only when
the first byte of its `0x7D`-byte record is not `0xFF`, the destroyed/no-mech
sentinel. The old C multiplied an already typed `Mechs[]` index by `0x7D` and
compared the entire name array; both errors have been removed.

The second pass covers friendly infantry IDs `4-11`. The original performs no
active/on-map check in this pass.

For either kind of unit:

1. compare its selector at `3092:396C + combatantId` with the matched direction;
2. if different, install the corresponding walk-stream pointer at
   `3EDB:01F6 + combatantId * 4`;
3. call `Advance_Combatant_Animation_Stream_1732`; and
4. store the returned `AL` frame ID at `3092:409A + combatantId`.

The stream begins with `FD direction`, so advancing a newly selected stream
also updates the selector before returning its first visible frame. When the
direction is unchanged, preserving the current cursor lets the existing walk
cycle continue instead of restarting every input tick.

The arrays based at `3092:396C` and `409A` are 24-byte tables indexed by the
complete combatant ID. They are now represented in `BTECH.h` as
`CombatantAnimationSelector_396C` and `CombatantSpriteFrame_409A`.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:231D-240A`.
- Initialized command and far-pointer tables in `BTECH_3EDB.asm`,
  `3EDB:0160-0299`.
- Walk byte streams in `BTECH_2FE8.asm`, `2FE8:0270-02EF`.
- Stream decoder `0800:1732-17BA`.

The routine, direction order, pointer tables, and frame update are explicit in
the original executable and do not require Astra confirmation.
