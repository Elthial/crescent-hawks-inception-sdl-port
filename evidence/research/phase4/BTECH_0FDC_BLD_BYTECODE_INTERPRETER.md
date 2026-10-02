# `BTECH_0FDC` BLD bytecode interpreter

## Review boundary

This block covers `Execute_Bld_Bytecode_01C0` at `0FDC:01C0..05F6`. It
reconstructs the bytecode cursor, dispatch table, operand consumption, branch
rules, and executable calls for decoded BLD payloads. The little-endian target
reader at `0FDC:05F7` is reviewed separately in
[`BTECH_0FDC_BLD_TARGET_READER.md`](BTECH_0FDC_BLD_TARGET_READER.md).

The canonical portable instruction reference remains
[`../formats/BLD_OPCODES.md`](../formats/BLD_OPCODES.md). This note records why
the maintained C now has that shape and which apparent behaviours were Reko
errors.

## Interpreter model

The input is one 16:16 far pointer to a decoded byte stream. The interpreter
uses two native WORD locals as its persistent control state:

- a payload-relative `Cursor`, initially zero;
- an `ExitInterpreter` flag, initially zero.

Each iteration fetches `payload[cursor]`, increments the cursor by one, and
dispatches only bytes `E4..FF`. Every other byte is ignored as a one-byte no-op
when it reaches the outer loop. Operands remain byte-sized unless the opcode
explicitly calls one of the two little-endian WORD readers. Immediate values
use `0FDC:19F6`; control-flow targets use `0FDC:05F7`.

The original Reko output did not express this model. It reversed the loop test,
switched on a cursor-like value rather than the fetched opcode, converted the
far byte pointer into 32-bit-looking array arithmetic, and numbered cases
`00..1B` after losing the jump table's `E4` base.

## Control-flow rules

Every encoded target is an absolute offset from the start of the decoded BLD
payload; it is not relative to the current instruction and is not a flat
pointer. A taken branch replaces `Cursor` with the decoded target. An untaken
conditional branch advances over its two-byte target.

`F3` and `F9` branch through inline WORD tables:

- `F3` obtains the table index from byte state at `3092:D30C + index`;
- `F9` obtains it from `Display_Menu_Choices_And_Check(menuId)`;
- both multiply the selected index by two, read the selected target WORD, and
  replace the cursor with it.

Neither opcode encodes a table length. The corpus-based boundary inference used
by InceptionTools is documented in the canonical opcode reference and is not an
interpreter guarantee.

## Persistent BLD state

Opcodes `F1`, `F3`, `F4`, and `F7` address `3092:D30C` with byte instructions.
The maintained memory scratchpad now names the complete `D30C..D36F` window
`PersistentBldState_D30C[0x64]`. This raw address view overlaps the individually
named story and runtime fields already recorded beneath it; it is not additional
sequential storage.

## Important corrections and confirmations

- `EA` calls `0800:48B7(scene, argument)` only when WORD `3092:3938`
  (`DisableInput`) is zero. The old pseudo-C inverted this condition.
- `E5`, `EE`, and `EF` first read a 16-bit amount and use `CWD` where it enters
  the 32-bit C-Bill calculation. The source therefore sign-extends a WORD; the
  reader itself does not produce a 32-bit value.
- `EE` saturates the C-Bill balance to zero when the requested subtraction
  cannot be made. `E5` and `EE` both call `1631:1FDF` afterward.
- `ED` scans exactly eight player-character records. A record participates when
  its name byte is not `FF`; the tested skill byte begins at record offset
  `+04`.
- `F0` uses `CBW` on both byte operands before writing the two text-layout WORDs.
- `FC` calls the text renderer at `1E56:03F5`, then the length helper at
  `207F:3B9E`, and advances by the returned length plus the NUL byte. Reko's
  current `void` declaration for that helper is false and remains follow-up
  cleanup in its own source block.
- `19F6` is `Read_Bld_Immediate_Word_19F6`: it combines two bytes as an
  unsigned little-endian WORD and does not increment the supplied far pointer.
  Its six call sites read C-Bill amounts, map coordinates, and an X-coordinate
  comparison operand.

## Resolved and unresolved callees

Opcode `E9` is now verified as `RECRUIT_CRESCENT_HAWK`. Its operand selects the
generated agent's Good-level specialty; see the
[`11B8:0D58` recruitment review](BTECH_11B8_CRESCENT_HAWK_RECRUITMENT.md).
The stable names and contracts of the timing/input helper (`1F3D:086A`) and
action dispatcher (`1CD3:0004`) remain in the executable-target register.

The action dispatcher itself subtracts one from the F5 operand before indexing
its 47-WORD jump table at `1CD3:1762`. Consequently the existing zero-based
pseudo-C case labels are one below the raw BLD action values. For example,
table-index case `1D` is raw action `1E`, the Rex/Kurita ambush action.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `01C0..05F6`;
- the 28-entry jump table at `05B1..05E7`;
- direct far calls and byte/WORD memory instructions in each handler;
- [`../formats/BLD_OPCODES.md`](../formats/BLD_OPCODES.md);
- [`../investigations/BLD_EXECUTABLE_TARGETS.md`](../investigations/BLD_EXECUTABLE_TARGETS.md).

The loop model, opcode mapping, operand widths, branches, and direct call
addresses are high confidence. Gameplay names explicitly retained as open above
are not promoted by this review. No Astra review is currently required.
