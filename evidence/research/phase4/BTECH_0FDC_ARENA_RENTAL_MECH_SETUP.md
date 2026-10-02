# `BTECH_0FDC` Arena rental-Locust setup

## Review boundary

This block covers `Prepare_Rental_Locust_For_Arena`, `0FDC:1C9B-1D2F`.
Decoded `ARENA.BLD` action `0x21` invokes it after charging 250 C-Bills when
the player accepts the Arena's rental offer. Action `0x22` is the alternative
party-owned-mech selection path, and both paths converge on action `0x23` to
run the Arena mission.

The maintained C dispatch cases are one less than those BLD action bytes due
to the existing switch transcription, so this call appears under case `0x20`
in `Citadel_Building_Dialogs`.

## Complete slot-zero replacement

The setup copies exactly `0x7D` bytes in each direction:

```text
3092:C724 -> 3092:3780       preserve original live mech slot zero
2FE8:02F0 -> 3092:C724       install the reference Locust in slot zero
```

`2FE8:02F0` is the first record of the contiguous reference-mech table and its
stored name identifies it as the Locust. This resolves the former pseudo-C's
question about whether `MechRefs[MECH_REF_Locust]` was the correct source.

This helper does not assign the rental mech's crew. After either Arena setup
path returns, the enclosing action `0x23` dispatcher writes Jason's character
ID to slot zero's `PilotId` and writes mech index zero to Jason's
`MechAssignment` before playing the mech-startup animation.

## Temporary party state

As in the party-owned setup, two assembly stores use `C620 + i*0x11` and the
pre-biased alias `C60F + i*0x11` for indexes `1..7`. Their combined effect is
to set `MechAssignment = 0x03` in all eight party records.

The routine then saves party Name IDs `1..7` in the byte array at `3092:3FE9`
and replaces the live IDs with `0xFF`. Jason remains visible. The enclosing
dispatcher subsequently changes only Jason's assignment to slot zero, leaving
the hidden party members assigned to index three during the Arena fight. The
reason for choosing index three as that temporary value remains unresolved,
but the destinations and literal are exact.

Finally, the routine sets WORD `ArenaRentalMechMode_E48E`. This selects rental
cleanup and enables the Arena-specific second enemy/spectator behaviour.

## Corrected transcription defects

- The record backup and template installation are byte-for-byte copies, not
  invalid nested typed-array expressions.
- The source is the reference Locust at exact address `2FE8:02F0`.
- `C60F` is an alias into the character records, not a second array.
- The assignment writes affect party slots `0..7`; only Name IDs `1..7` are
  saved and hidden.
- Crew assignment belongs to the following dispatcher block, not this helper.
- The old generic `Mech_Mission_function_2` name obscured its Arena role.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:1C9B-1D2F`;
- `BTech-Reko-expanded/BTECH.reko/BTECH_1CD3.asm`, calls at `1052` and the
  common Arena launch beginning at `1062`;
- decoded shipped `chinception/ARENA.BLD`, rental branch at `04D7-06A5`;
- the reference-mech layout beginning at `2FE8:02F0`.

All copy bounds, source and destination addresses, widths, party effects, and
the BLD branch role are directly verified. Only the purpose of temporary
`MechAssignment = 0x03` remains unresolved. No Astra review is required.

The next reviewed routine is `Load_And_Decode_Indexed_BLD_1D30`, documented in
[`BTECH_0FDC_BLD_LOAD_AND_DECODE.md`](BTECH_0FDC_BLD_LOAD_AND_DECODE.md).
