# `BTECH_0800` movement-command decode

## Review block (`0800:218F-2211`)

`Character_Movement_On_Map` receives a 16-bit movement command and converts it
to signed X/Y deltas. It then gives the interaction and collision routine the
first opportunity to consume or block the proposed step. Actual movement and
map-cache maintenance begin at `0800:2212` and remain a later review block.

## Command-to-delta mapping

The routine recognises all eight compass directions:

| Commands | `deltaX` | `deltaY` |
|---|---:|---:|
| north | 0 | -1 |
| north-east | 1 | -1 |
| east | 1 | 0 |
| south-east | 1 | 1 |
| south | 0 | 1 |
| south-west | -1 | 1 |
| west | -1 | 0 |
| north-west | -1 | -1 |

An unrecognised value, including the zero passed by one mission path, leaves
both deltas zero. The interaction routine is still called with that zero step.

## Sign-extension trap in the clean assembly

The maintained source constants are 16-bit values such as `0xFFB8` for north.
Reko's clean assembly prints the corresponding comparisons as `0xB8`. This
does **not** mean the caller passes an unsigned byte.

Inspection of the expanded executable shows instructions such as:

```text
83 7E 06 B8    cmp word ptr [bp+6], imm8 B8
83 7E FC FF    cmp word ptr [bp-4], imm8 FF
```

Opcode `83` sign-extends its one-byte immediate to the 16-bit operand width.
The effective comparison values are therefore `0xFFB8` and `0xFFFF` (`-1`).
This also explains why the old pseudo-C's assignment of `0xFFFF` followed by a
comparison with apparent `0x00FF` could not describe working north or west
movement. The C annotation now uses a 16-bit command word and signed 16-bit
deltas. The command itself remains unsigned so its `0xFFxx` bit pattern also
compares correctly under modern C integer-promotion rules.

This is a useful warning for later review: an `83` word operation shown with a
byte-sized hexadecimal operand must be interpreted using sign extension.

## Interaction gate

The call order confirms that the arguments to
`Map_Interactables_Building_Or_Items` are `(deltaX, deltaY)`: the original
pushes `deltaY` first and `deltaX` second under the C calling convention.

If that routine returns non-zero, execution skips directly to the final
position-grid refresh at `0800:22FA`; no world coordinate is moved. A zero
result permits the directional movement and map-cache update beginning at
`0800:2212`.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:218F-2211`.
- Expanded `BTECH.EXE` bytes at file offsets `0x5532` onward, establishing the
  sign-extending `83` instruction forms.
- `COMMAND_MOVE_*` definitions in `BTech/BTECH.h`.

The command width, delta mapping, and gate behaviour are explicit in the
original instructions and do not need Astra confirmation.
