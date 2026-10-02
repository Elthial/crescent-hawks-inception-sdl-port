# `BTECH_0FDC` jailbreak mission loop

## Scope

This note covers `0FDC:0AA8-0BF1`, the Mission09-specific part of the common
mission loop. It ends before the
[`0FDC` common mission teardown](BTECH_0FDC_MISSION_TEARDOWN.md) begins at
`0FDC:0BF2`.

## Periodic combat timer (`0AA8-0AE1`)

The jailbreak increments the WORD timer at `BP-14` on every processed world
update. The instruction order is significant:

```text
oldTimer = MissionTimer
MissionTimer++
if ((signed short)oldTimer >= 0x0050)
    generate and run an encounter
```

Thus a timer reset to zero triggers combat on the 81st processed update, not the
80th. On a trigger the game calls `Mission_GenerateEnemies(0x00, 0x88)`, enters
combat through `Combat_Parent_000A(0x03)`, and resets the timer to zero. If
`3EDB:014A` reports that the main character has not survived combat, both
`ExitMissionLoop` and `MissionObjectiveResolved` are set. The following survival
test then prevents any parked-mech interaction on that update.

This is a gameplay-update counter, not proof of an elapsed real-time interval.

## Parked 'Mech positions (`0AE2-0B42`)

Interaction is considered only while the player is at packed Y coordinate
`702D`. Four WORD attempt flags correspond to four X coordinates:

| Index | Rendered 'Mech | Interaction tile |
|---:|---|---|
| 0 | `0D13:702C` | `0D14:702D` |
| 1 | `0D17:702C` | `0D18:702D` |
| 2 | `0D1B:702C` | `0D1C:702D` |
| 3 | `0D1F:702C` | `0D20:702D` |

The rendered positions are independently used by the exploration overlay in
`BTECH_0800.c`. Each interaction tile is one packed coordinate east and one
south of its rendered 'Mech. The four machines are spaced by four X units.

`JailMechAttempted[index]` is tested before the boarding sequence and set on the
first visit. Revisiting the same machine therefore does not advance the attempt
counter or replay its messages.

## Boarding sequence and successful attempt (`0B43-0BF1`)

For a new machine the game draws menu layout 6, the top sidebar, and menu border,
then prints the boarding-ladder and cockpit messages at `3EDB:155D` and
`3EDB:159A`. It performs a timed wait followed by a blocking key read before
printing the activation-switch message at `3EDB:15D4`.

The success comparison is another post-increment operation:

```text
attemptsBeforeThis = JailDistinctMechAttempts
JailDistinctMechAttempts++
if (attemptsBeforeThis == 2)
    the 'Mech starts
else
    the 'Mech refuses to start
```

Consequently the first two distinct machines attempted fail and the third
distinct machine starts. The successful machine is determined by the player's
attempt order; it is not tied to one fixed courtyard position. Earlier source
and exploration-renderer notes that described the *second* distinct attempt as
successful were transcription errors caused by treating the counter as a
pre-increment value.

On failure, `Play_Failed_Mech_Startup_Scene_16B2()` runs and the refusal message at
`3EDB:1611` is displayed. On success, animation `O00` runs with game-view
restoration, the success message at `3EDB:15F8` is displayed, and both objective
and loop-exit flags are set. Both paths then make the explicit blocking keyboard
read at `0FDC:0BEE`; the display/wait wrapper does not consume that input itself.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0AA8-0BF1`;
- the timer `MOV`, `INC`, then signed `JL` sequence at `0AA8-0AB1`;
- the attempt-counter `MOV`, `INC`, then `JZ` sequence at `0B96-0B9F`;
- the four WORD stack slots at `BP-0C..BP-06` and index scaling at
  `0B0E-0B42`;
- the matching fixed overlay coordinates in `BTECH_0800.c`.

The control flow, timer semantics, coordinates, unique-attempt flags, third-try
success, animations, messages, and input calls are directly verified. The
encounter generator's arguments are preserved but their domain meaning remains
outside this block. No Astra review is required.
