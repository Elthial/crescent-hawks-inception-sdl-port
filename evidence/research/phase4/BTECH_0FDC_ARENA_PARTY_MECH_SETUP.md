# `BTECH_0FDC` Arena party-mech setup

## Review boundary

This block covers `Prepare_Party_Mech_For_Arena`, `0FDC:1A26-1B40`. The old
name, `Select_Mech_For_Training`, was wrong: the shipped `ARENA.BLD` invokes
this routine when Jason declines a rental and chooses one of the party's own
'Mechs for Arena combat.

## Selection and record staging

The routine first preserves the four live party mech records' byte-sized
`PilotId` and `RiderId` fields in scratch arrays at `3092:430E` and
`3092:3FFA`. It then prints the executable's exact prompt:

```text
Which 'Mech will you take into combat?\r
```

`Display_Text_Mech_Names` records the chosen live-mech index in `305B:0068`.
The routine saves all `0x7D` bytes of live slot zero at `3092:3780`, then
copies all `0x7D` bytes of the selected live record into slot zero. Arena code
can consequently treat slot zero as its combatant without losing Jason's
original record.

After the copy, the first byte of the staged mech name is restored from the
four-byte first-name-initial cache at `3092:D452`. This cache survives workflows that mark live
record names unavailable with `0xFF`.

## Temporary solo-party state

The executable contains paired writes based at `C620` and the pre-biased
address `C60F`. They are aliases, not two arrays. Across loop indexes `1..7`,
they jointly set the `MechAssignment` byte (`+0x0C`) in party records `0..7`
to `0x03`.

The semantic reason for assignment index three is still open; the literal and
destination are exact. The routine then saves the Name IDs for party slots
`1..7` at `3092:3FE9`, replaces those live IDs with `0xFF`, and leaves Jason as
the only visible party character. The staged slot-zero mech receives Jason as
its pilot and `0xFF` as its rider.

Finally, the WORD at `3092:E48E` is cleared. The parallel rental setup at
`0FDC:1C9B` sets it, and the intervening restoration routine uses the value to
choose between party-owned and rental-mech cleanup. It is now named
`ArenaRentalMechMode_E48E`; other combat reads use the same flag to enable the
Arena's special spectator handling.

## Corrections to the former pseudo-C

- The four pilot and rider snapshots are byte arrays, not WORD arrays.
- Mech slots are indexed as four `0x7D`-byte records, not by adding a byte
  offset to a typed array index.
- The record copy is byte-for-byte and includes every field.
- `C60F` is a pre-biased alias into the party records, not an independent
  `InfantryValueArray`.
- Only party slots `1..7` have their Name IDs saved and hidden.
- The displayed string includes the apostrophe in `'Mech` and a carriage
  return.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:1A26-1B40`;
- `BTech-Reko-expanded/BTECH.reko/BTECH_1467.asm`, mech-name menu and selected
  index write;
- decoded shipped `ARENA.BLD`, action `0x22` in the own-mech branch;
- restoration and rental routines immediately following this block.

The routine's control flow, widths, copy bounds, selected-record source, party
hiding, and Arena role are verified. Only the higher-level meaning of the
temporary `MechAssignment = 3` values remains unresolved. No Astra review is
needed for this block.

The next reviewed routine is
[`Restore_Party_After_Arena_Combat`](BTECH_0FDC_ARENA_POST_COMBAT_RESTORE.md).
