# `BTECH_0DAB` whole-mech record copy

## Reviewed block

- Address range: `0DAB:06BD-070C`.
- Parent routine: `Salvage_Mechs_Dialog`.
- Restoration of the name initial, pilot assignment, sprite selection, and
  minimum viable damage state begins at `070D`.

The routine scans only player mech records `0..3`. A record is available when
its first name byte at record offset `+0x00` is the destroyed/empty marker
`0xFF`. If no record is available, control skips installation and reaches the
post-candidate state checks at `080E`.

For the first empty record, the code copies bytes `+0x01` through `+0x7C` from
the selected wreck's 0x7D-byte `Mech` record. Offset `+0x00` is deliberately
excluded because the wreck record contains the `0xFF` destruction marker there.
The next block restores that byte from `DestroyedMechNameInitial_323E`.

All loop counters and record offsets are native 16-bit words. The former C
multiplied indexes by `MECH_RecordSize` even after expressing storage as a typed
`Mechs[]` array; the reviewed form performs the bytewise copy through each
already-indexed record instead.

No Astra review is required: bounds, byte offsets, and the omitted first byte
are explicit in the assembly.
