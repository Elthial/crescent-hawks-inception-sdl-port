# `BTECH_0FDC` BLD target reader

## Review boundary

This block covers `Read_Bld_Target_05F7` at `0FDC:05F7..0628`. It is the
interpreter's dedicated reader for absolute branch targets. `Mech_Mission_0629`
beginning at the next address is outside this block.

## Reconstructed operation

The argument is one 16:16 far pointer. It therefore occupies four stack bytes,
not the 32-bit flat-pointer type suggested by Reko. The routine reads two bytes,
zero-extends each into a native WORD, places the second byte in the high half,
and returns the result in `AX`:

```text
target = byte[0] | (byte[1] << 8)
```

The old annotated return expression merely added the two adjacent source
elements. For target bytes `09 16`, it would have produced `001F`; the executable
produces `1609`. The replacement preserves the little-endian shift explicitly.

## Role in the BLD VM

All calls originate from the common taken-branch path inside
`Execute_Bld_Bytecode_01C0`. The pointer may identify:

- a conditional or unconditional instruction's inline target WORD; or
- one selected entry in an inline `F3` or `F9` target table.

The returned WORD is a decoded-payload-relative offset. The interpreter assigns
it directly to its WORD cursor. It is neither a segmented code/data pointer nor
a relative displacement from the current instruction.

The similar `Read_Bld_Immediate_Word_19F6` helper also assembles a
little-endian WORD, but the original interpreter uses it for immediate numeric
operands and uses `05F7` for control-flow targets. The maintained transcription
preserves that semantic distinction even though their byte combination is the
same.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `05F7..0628`;
- the sole call site at the interpreter's common branch path, `028C..0300`;
- all branch and table paths converging on that call;
- shipped BLD targets decoded by InceptionTools.

The pointer width, byte order, return width, and branch-target role are directly
verified. No Astra review is required.
