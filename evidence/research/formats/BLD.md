# BLD building scripts

BLD files contain encrypted scripts that combine text, conditional branches,
menus, presentation commands, state operations, and calls into executable-side
gameplay handlers. The local installation contains 26 files, from 362 bytes
(`BARRACK2.BLD`) to 8501 bytes (`HUT.BLD`).

## File layout

| File offset | Size | Confidence | Meaning |
|---:|---:|:---:|---|
| `0x00` | 2 | V | Little-endian encrypted-payload length; equals file length minus two in all 26 local files. |
| `0x02` | stored length | V | Encrypted script payload. Script offset zero corresponds to file offset `0x02`. |

There is no separate signature, content-type header, metadata block, or
uninterpreted prefix after the length word. The frequently observed raw bytes
`EE C6 EB EA` at file offset `0x02` are the encrypted standard script prologue.
They decode to executable instructions `FE 06 FD FA`.

## Original loading contract

`Load_File_To_Memory` at `1F3D:063B` performs two reads:

1. read the first two file bytes into a local 16-bit length;
2. read exactly that many remaining bytes into the caller's destination.

`0FDC:1D30` supplies destination `3092:00A0`, so the address relationship is:

```text
file offset     = 0x02 + scriptOffset
runtime address = 3092:(0x00A0 + scriptOffset)
```

The literal `0xA0` is a mutable-memory destination, **not a file offset**.
`0FDC:0008` passes `3092:00A0` directly to the interpreter after loading and
decoding.

The filename lookup at `3EDB:4EC2` contains 26 native 16:16 far pointers, one
for each contiguous BLD ID `0x00..0x19`. Before loading, `0FDC:1D30` selects
logical Game Disk 1, then changes to Disk 2 for IDs below `0x02` or at least
`0x11`. This calls the drive selector, not the user-facing disk-insertion
prompt.

## Byte transformation

For each stored payload byte, the executable performs:

```text
decoded = ((stored + 0x29) & 0xFF) XOR 0xE9
stored  = ((decoded XOR 0xE9) - 0x29) & 0xFF
```

The first expression is the verified load-time decode order: addition occurs
before XOR. Decoded dialogue is ordinary text, not a second substitution cipher.

The original loop transforms `0x2328` (9000) bytes beginning at `3092:00A0`
regardless of the individual payload length. This reaches beyond every local
BLD payload; `HUT.BLD`, the largest, has an 8499-byte payload. A safe portable
loader should decode only the stored payload and preserve the original fixed
loop as a documented compatibility quirk unless evidence shows code depends on
post-file memory.

## Standard decoded prologue

Every local file begins with raw payload bytes `EE C6 EB EA C0 xx`. The common
four bytes and observed following pairs decode as:

| Stored payload bytes | Decoded bytes | Interpretation |
|---|---|---|
| `EE C6 EB EA` | `FE 06 FD FA` | Select menu/layout preset 6, redraw the top/sidebar area, then draw border style 0. |
| `C0 EC` | `00 FC` | No-op followed by display of the NUL-terminated text beginning next. |
| `C0 F5` | `00 F7` | No-op followed by conditional state branch. |
| `C0 F4` | `00 F4` | No-op followed by state assignment. |
| `C0 F3` | `00 F5` | No-op followed by executable-side action dispatch. |
| `C0 DA` | `00 EA` | No-op followed by conditional animation/display action. |

Consequently the old `C0 EC`/`C0 F5` “content type” classification describes
encrypted prologue families, not header values or format subtypes.

## Interpreter model

The loop at `0FDC:01C0` maintains a 16-bit script offset relative to the decoded
payload base:

- decoded bytes `0xE4..0xFF` dispatch through a 28-entry jump table;
- other bytes are skipped when reached by the outer instruction loop;
- operands are consumed by their owning instruction and may contain any value;
- `0xFC` consumes and displays a NUL-terminated text string;
- branches decode an inline little-endian target and replace the script offset;
- `0xFF` exits.

Branch targets are absolute decoded-payload offsets, not raw file offsets and
not relative displacements:

```text
target runtime address = 3092:(0x00A0 + target)
target file offset     = 0x02 + target
```

See [BLD opcode reference](BLD_OPCODES.md) for operand widths and control flow.

## Excluded imported implementation

Do not use UnBattletech's BLD loader, opcode documentation, interpreter, opcode
names, or derived control-flow output as research evidence. Its loader begins
from the wrong interpretation of the two-byte payload length and payload origin;
the resulting byte stream is wrong, so every downstream opcode interpretation
is discarded rather than retained as a hypothesis. Its unrelated emulator and
decompiler setup may still be useful as tooling.

## Requirements for InceptionTools

The future parser/disassembler must:

- retain original stored bytes and exact file offsets;
- validate the 16-bit stored length against the actual file length;
- expose decoded-payload offsets separately from file offsets;
- use the verified addition-then-XOR transformation;
- decode every known operand with explicit bounds checking;
- retain unknown/non-opcode bytes rather than discarding them from output;
- validate absolute branch targets against the payload boundary;
- represent `F3` and `F9` inline jump tables without guessing their size;
- produce a lossless disassembly before attempting gameplay execution.

## Evidence

- All 26 ignored `chinception/*.BLD` files: stored length and decoded prologues.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1F3D.asm`, `1F3D:063B`: two-stage
  length-prefixed file loader.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:1D30`: destination,
  disk selection, filename lookup, fixed decode loop, and transformation order.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:01C0` and `05F7`:
  dispatch range, operand access, absolute branch targets, and little-endian
  word decoder.
- `Btech/BTECH_0FDC.c`: semantic names and surrounding gameplay context, checked
  against the clean instructions.
