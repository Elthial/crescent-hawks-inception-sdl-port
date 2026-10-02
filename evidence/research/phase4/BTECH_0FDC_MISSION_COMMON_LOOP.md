# `BTECH_0FDC` common mission loop

## Review boundary

This block covers `Mech_Mission_0629` from `0FDC:0883` through `0998`. It
establishes the common loop exit state, handles the training-centre return gate,
paces keyboard and idle updates, and redraws the exploration view. The
mission-specific objective checks beginning at `0999` continue in
[`BTECH_0FDC_MISSION_OBJECTIVES_00_01.md`](BTECH_0FDC_MISSION_OBJECTIVES_00_01.md).

## Loop-exit state

The WORD at `BP-10` was previously named as another copy of
`Kurita_AttackFlag`. It is broader than that. The routine seeds it from global
WORD `3092:3772`, but it is also set when:

- mission 8 bypasses the interactive loop;
- a resolved training objective returns to the hangar gate;
- later jailbreak combat reports the required-character state as failed; or
- the working jailbreak mech is found.

It is therefore named `ExitMissionLoop`. A nonzero global Kurita flag means a
mission whose combat was interrupted by the attack goes directly to teardown,
but the local should not be mistaken for persistent attack state.

## Training-centre return gate

The loop recognizes the hangar when packed X is at most `0C3C` and packed Y is
within `C049..C04F`, inclusive. These are unsigned WORD comparisons in the
assembly.

If `MissionObjectiveResolved` is nonzero, entering that region sets the exit
latch. Otherwise the code shows “Don't Come back here until you complete your
mission!” once. `PrematureReturnWarningShown` prevents repeated messages while
the player remains in or revisits the gate; it does not mark mission success.

## Input and idle pacing

`MissionUpdateCountdown` is a signed WORD at `BP-1E`, not the unsigned byte
previously inferred by Reko. Each loop iteration follows one of two paths:

1. If input is pending, read one key, translate it to a movement command, drain
   additional pending keyboard input, advance the friendly movement animation,
   and apply the command.
2. Otherwise wait one vertical retrace, decrement the signed countdown, and
   request a world update only when the result becomes negative.

The countdown starts at zero, so the first idle iteration immediately changes
it to `-1` and redraws. Each completed world update resets it to `0x000A`.
Consequently the next idle-only redraw occurs after eleven decrement/retrace
iterations (`0A` down through `00`, then `-1`), not after ten. Keyboard input
requests an update immediately regardless of the current countdown.

This corrects an impossible old condition: an `unsigned char` can never satisfy
`countdown < 0`.

## Shared world update

When an update is due, the loop:

1. resets the countdown to `0x000A`;
2. advances the three-frame animated map tiles;
3. increments WORD `DS:5802`;
4. updates roaming map NPCs only when that incremented phase equals one;
5. masks the phase with `0003`, producing a repeating `0..3` cycle;
6. copies the prepared view, draws infantry and mechs, and presents the EGA
   frame.

`DS:5802` is therefore named `MissionNpcUpdatePhase_5802`. Roaming NPC logic
runs once per four shared world updates. The phase is not reset on entry here,
so its cadence continues from the existing global value.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0883..0998`;
- direct WORD accesses at `BP-10`, `BP-12`, and `BP-1E`;
- unsigned packed-position branches `JA` and `JC` at `08A7..08CA`;
- the signed `JNS` countdown test at `0945..0956`;
- the `DS:5802` increment, equality test, and mask at `0964..097E`.

The exit-latch role, hangar bounds, input ordering, timer signedness, redraw
cadence, and NPC phase are directly verified. No Astra review is required.
