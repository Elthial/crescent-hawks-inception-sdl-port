# BLD decoded opcode reference

This table describes decoded bytes processed by `0FDC:01C0`. **V** means the
operand width and operation are directly established by the clean assembly.
Semantic names remain deliberately narrow where the called routine needs later
annotation work.

Executable callees and outstanding method/tag mapping are tracked separately in
the [BLD executable-target register](../investigations/BLD_EXECUTABLE_TARGETS.md).

Notation:

- `u8`: one byte;
- `wordLE`: two-byte little-endian word;
- `target`: absolute decoded-payload offset encoded as `wordLE`;
- `state[n]`: byte at `3092:D30C + n`;
- `textZ`: NUL-terminated decoded text.

## Opcode table

| Byte | Operands following opcode | Confidence | Canonical operation |
|---:|---|:---:|---|
| `E4` | `soundId:u8` | V | Call `0800:19BF` (`Play_Sound_If_Enabled`). |
| `E5` | `amount:wordLE` | V | Sign-extend the word, add it to the 32-bit C-Bill balance, then refresh the balance display/state. |
| `E6` | `x:wordLE, y:wordLE` | V | Set `246C:A44B` and `246C:A44D`. |
| `E7` | `x:wordLE, target:wordLE` | V | Branch to `target` if `246C:A44B == x`; otherwise continue after the target word. |
| `E8` | `mask:u8, target:wordLE` | V | Branch if `(RandByte() & mask) != 0`; otherwise continue after the target word. |
| `E9` | `specialtySkill:u8` | V | Recruit a generated Crescent Hawk agent; set skill index `specialtySkill` to Good. Shipped scripts use Piloting `4`, Tech `5`, or Medical `6`. |
| `EA` | `scene:u8, argument:u8` | V | If `3092:3938 == 0`, call `0800:48B7(scene, argument)`; always consume both bytes. |
| `EB` | `target:wordLE` | V | Branch if byte `3092:D451` is nonzero. |
| `EC` | `target:wordLE` | V | Branch if byte `3092:D450` is nonzero. |
| `ED` | `skillIndex:u8, minimum:u8, target:wordLE` | V | Branch if any nonempty player character has `skill[skillIndex] >= minimum`; skill base is character offset `+0x04`. |
| `EE` | `amount:wordLE` | V | Subtract amount from the 32-bit C-Bill balance, saturating to zero, then refresh balance display/state. |
| `EF` | `amount:wordLE, target:wordLE` | V | Branch if the 32-bit C-Bill balance is at least amount. |
| `F0` | `left:u8, right:u8` | V | Set text/layout words `3092:3748` and `3092:374E`. |
| `F1` | `index:u8, value:u8` | V | Add value to `state[index]`, with byte storage/wrap behaviour. |
| `F2` | none | V | Call `1F3D:086A`, the timed wait/input-check routine. |
| `F3` | `index:u8, table:wordLE[]` | V | Use `state[index]` as a word-table index, read a target from the inline table, and branch. |
| `F4` | `index:u8, value:u8` | V | Assign `state[index] = value`. |
| `F5` | `action:u8` | V | Call executable-side BLD action dispatcher `1CD3:0004(action)`. Action `1E` recruits Rex and begins the scripted Kurita-party ambush. |
| `F6` | `target:wordLE` | V | Present the standard yes/no prompt via `0800:1A13(1)` and branch on a nonzero/yes result. |
| `F7` | `index:u8, target:wordLE` | V | Branch if `state[index]` is nonzero. |
| `F8` | `target:wordLE` | V | Unconditional absolute branch. |
| `F9` | `menuId:u8, table:wordLE[]` | V | Call `1E56:0B5E(menuId)`; use its returned selection as an index into the inline target table and branch. |
| `FA` | `borderId:u8` | V | Draw a menu border through `1E56:0004(borderId)`. |
| `FB` | none | V | Call `1F3D:0259`, the keyboard/input wait routine. |
| `FC` | `text:textZ` | V | Display text beginning at the current script offset, then advance by `strlen(text) + 1`. |
| `FD` | none | V | Call `1E56:0388`, which redraws/clears the top graphic/sidebar region and resets text offsets. |
| `FE` | `layoutId:u8` | V | Apply menu/layout variables through `1E56:0281(layoutId)`. |
| `FF` | none | V | Set the interpreter exit flag. |

## Branch encoding

`Read_Bld_Target_05F7` at `0FDC:05F7` reads a target as:

```text
target = payload[offset] | (payload[offset + 1] << 8)
```

The interpreter then replaces its current payload-relative offset with that
value. Conditional instructions always consume or skip their inline target word,
so fall-through begins immediately after it.

For example:

```text
F7 27 09 16
```

means “if `state[0x27]` is nonzero, branch to payload offset `0x1609`; otherwise
continue with the byte after this four-byte instruction.”

## Variable inline tables

`F3` and `F9` are instruction-plus-data constructs. Their target tables begin
immediately after the fixed operand, but the interpreter does not encode a table
length in the instruction:

- `F3` obtains the selected table index from a state byte.
- `F9` obtains it from the executable-side menu routine and `menuId` metadata.

The local 26-file corpus supplies a reliable boundary rule even though the
instruction does not: all 36 observed tables consist of consecutive `wordLE`
values smaller than the decoded payload length, and the following word is out
of range. InceptionTools uses that rule and marks the resulting boundary as
inferred. It leaves the remainder ambiguous if it cannot establish even one
in-range target.

This is not a scan for “the next opcode.” Target words may legally contain
bytes in the opcode range, and several real tables do. The in-range-word rule
is verified for the local expanded-executable/data profile but should be
revalidated before being assumed for a different release.

## Text and non-opcode bytes

Decoded bytes outside `E4..FF` are not dispatched when encountered by the outer
loop. Most meaningful prose is consumed as the inline `textZ` operand of `FC`
and is ordinary decoded text, including renderer control characters. A structural
byte such as decoded `00` can act as an outer-loop no-op, but should still appear
in lossless output.

There is no verified interpreter-level narrative-mode meaning for raw or decoded
bytes `9B`, `9C`, `9E`, `A0`, `BA`, `BB`, or `C0`. Claims assigning such meanings
must be demonstrated at a consuming routine rather than inferred from byte
frequency.

## Signedness caution

`Read_Bld_Immediate_Word_19F6` reconstructs a 16-bit little-endian word; it
does not advance its far-pointer argument or itself return a 32-bit value.
Individual callers subsequently sign-extend with `CWD` where needed for 32-bit
C-Bill arithmetic. Portable code should retain the raw `ushort` first and
apply signedness according to the specific opcode.
