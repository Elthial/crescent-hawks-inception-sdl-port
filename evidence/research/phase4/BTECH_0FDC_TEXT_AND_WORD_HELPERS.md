# `BTECH_0FDC` stock suffix and BLD immediate-word reader

## Review boundary

This block covers two adjacent complete helpers:

- `Display_No_Stock_Transaction_Text`, `0FDC:19E1-19F5`;
- `Read_Bld_Immediate_Word_19F6`, `0FDC:19F6-1A25`.

Both are straightforward assembly and require no Astra review.

## No-stock-transaction suffix

The first helper has one caller in the Citadel action dispatcher's stock-market
workflow. That caller has already printed either ` You invest` or ` You sell`.
The helper appends the exact stored text:

```text
 nothing this time.
```

The leading space is intentional and the period is present in the executable
at `3EDB:17A3`. The former C transcription omitted that period and gave the
helper a much broader Citadel-oriented name than its only use supports.

## Immediate WORD reader

`Read_Bld_Immediate_Word_19F6` accepts one 16:16 far pointer and reads exactly
two bytes:

```text
value = bytes[0] | (bytes[1] << 8)
```

The pointer is not incremented. The interpreter advances its own cursor after
the call according to the containing opcode.

The assembly applies `CBW` to each source byte while filling two WORD locals,
but later takes only each local's low byte, explicitly clears the other halves,
and combines them. The observable result is therefore an unsigned 16-bit
little-endian value. Reko's old expression, `*ptr + 1`, lost the second-byte
read, invented an increment, used a 32-bit-looking pointer, and gave the helper
the wrong return width.

## Six interpreter call sites

The immediate reader is used for:

- the WORD amount added to C-Bills (`E5`);
- the X and Y map coordinates written by `E6`;
- the expected X coordinate tested by `E7`;
- the WORD amount subtracted from C-Bills (`EE`);
- the minimum C-Bill amount tested by `EF`.

The three financial paths subsequently execute `CWD`, treating the raw WORD as
a signed 16-bit value before the 32-bit balance operation. Coordinate paths use
the returned bits directly.

## Relationship to `Read_Bld_Target_05F7`

`Read_Bld_Target_05F7` performs the same little-endian byte combination. The
original compiler retained two helpers because the interpreter funnels branch
and menu-table targets through the earlier routine while ordinary WORD operands
call `19F6`. Keeping distinct semantic names makes those roles visible even
though a future C# implementation may share one low-level `ReadUInt16LE`
primitive.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:19E1-1A25`;
- immediate-reader calls at `0FDC:039F`, `03D9`, `050E`, `053A`, `0558`, and
  `0578`;
- stock call at `BTECH_1CD3.asm:0717`;
- stored text at `BTECH_3EDB.asm:1791-17A4`.

The exact text, pointer width, byte order, result width, lack of pointer
mutation, and all call-site roles are directly verified.

The next reviewed routine is
[`Prepare_Party_Mech_For_Arena`](BTECH_0FDC_ARENA_PARTY_MECH_SETUP.md).
