# `BTECH_0FDC` Arena post-combat restoration

## Review boundary

This block covers `Restore_Party_After_Arena_Combat`, `0FDC:1B41-1C9A`. Its
only call is in the Arena mission branch of `Citadel_Building_Dialogs`, after
the mission and map-effect cleanup complete normally.

## Rental-mech cleanup

`3092:E48E` is a WORD flag set only by
[`Prepare_Rental_Locust_For_Arena`](BTECH_0FDC_ARENA_RENTAL_MECH_SETUP.md) at
`0FDC:1C9B`. When it is nonzero, the Arena record in live mech slot zero does
not belong to the party. The routine discards that complete temporary record
and restores all `0x7D` bytes previously saved at `3092:3780`.

The flag also controls the Arena-specific spectator and enemy behaviour in
combat, so the maintained name is `ArenaRentalMechMode_E48E`, rather than the
old `bool_JasonIsOnlyMember` description.

## Party-owned-mech writeback

When rental mode is clear, live slot zero contains the selected party-owned
mech's post-combat state. Cleanup performs these operations in order:

1. Copy all `0x7D` bytes from slot zero back to the selected roster slot.
2. If the combat record's first name/status byte is `0xFF`, set the selected
   cached roster marker at `3092:D452 + selected` to `0xFF` as well.
3. Set the selected live record's first byte to `0xFF`. Its cached first byte
   remains at `D452` when the mech survived.
4. If the selected slot was not zero, restore the original complete slot-zero
   record from `3092:3780`.
5. Restore the byte-sized `PilotId` and `RiderId` fields for all four party
   mech slots from `3092:430E` and `3092:3FFA`.

Step 3 initially looks destructive, but it matches the game's roster staging
scheme: `D452..D455` preserve the first mech-name byte while live records are
marked inactive outside their active map/combat placement. A destroyed Arena
entrant instead leaves `0xFF` in both locations.

## Party restoration

Both mech paths finish identically. The assembly again writes through bases
`C620` and pre-biased `C60F` for indexes `1..7`; together those aliases set
`MechAssignment = 0x08` in party records `0..7`. Thus the whole party is
dismounted after the Arena rather than returned to its former assignments.

Finally, Name IDs for party slots `1..7` are restored from the byte array at
`3092:3FE9`. Jason's slot-zero Name ID was never hidden or saved.

## Corrected transcription defects

- Typed mech-array indexes were incorrectly mixed with raw byte offsets.
- Several record member expressions contained invalid address-of and field
  syntax.
- The two assignment writes were represented as distinct arrays rather than
  overlapping views covering all eight party records.
- Pilot and rider restores were incorrectly expressed as scaled typed-array
  indexes; both are single bytes in each `0x7D`-byte mech record.
- The old routine name did not identify its Arena cleanup role.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:1B41-1C9A`;
- sole call at `BTECH_1CD3.asm:1150` in the Arena mission branch;
- own-mech setup at `0FDC:1A26` and rental setup at `0FDC:1C9B`;
- combat reads of `3092:E48E` that reserve enemy combatant 13 as an Arena
  spectator in rental mode.

All widths, bounds, aliases, copy directions, and branch roles are directly
verified. No Astra review is required for this block.

The next reviewed routine is
[`Prepare_Rental_Locust_For_Arena`](BTECH_0FDC_ARENA_RENTAL_MECH_SETUP.md).
