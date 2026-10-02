# BLD opcode executable-target register

## Purpose

This is a follow-up register for connecting bytecode operations to the original
16-bit executable and, later, to stable methods or tags in the reconstructed
code. Addresses use `segment:local-offset` notation. They are code addresses,
not 32-bit flat pointers; any Reko type implying otherwise must be checked
against the original assembly.

The opcode widths and immediate behaviour remain documented in
[`../formats/BLD_OPCODES.md`](../formats/BLD_OPCODES.md). This file tracks the
executable side of the boundary without inventing final C# architecture early.

## Known entry points and follow-up work

| Opcode | Original target or state | Current annotation | Later method/tag work |
|---:|---|---|---|
| interpreter | `0FDC:01C0` | `Execute_Bld_Bytecode_01C0` | Verified: WORD cursor/exit loop and all `E4..FF` cases are labelled in the maintained C. Preserve absolute payload-offset branches in the eventual C# VM. |
| `E4` | `0800:19BF` | `Play_Sound_If_Enabled` | Verified: widen the opcode byte to a one-based 16-bit ID and call the sound service through the `DS:015C` enable gate. |
| `E5` | C-Bill state plus `1631:1FDF` | `ADD_C_BILLS` / `Display_Text_CBill_Balance` | Verified: sign-extend the operand WORD, add it to the 32-bit balance, then call the refresh routine. |
| `E8` | `207F:0BC0` plus inline branch | `BRANCH_IF_RANDOM_MASK` / `Rand_0x00_to_0xFF` | Verified: branch when the random byte and operand mask have any common set bit. A stable random-service name can follow review of the callee. |
| `E9` | `11B8:0D58` | `RECRUIT_CRESCENT_HAWK` / `Recruit_Crescent_Hawk_Agent` | Verified: create an agent in the first free party slot; operand selects the Good-level specialty. Shipped values are Piloting `4`, Tech `5`, and Medical `6`. |
| `EA` | `0800:48B7` | `CONDITIONAL_SCENE_ACTION` / `Display_Animation_Scene` | Verified: consume `scene` and a second argument, and call only while `3092:3938` is zero. The second argument's complete semantic range remains open. |
| `ED` | character records at `3092:C614`, skills `+0x04..+0x0A` | `BRANCH_IF_PARTY_SKILL` | Tag the character-scan block and decide whether it becomes a helper or stays inside the interpreter. |
| `EE` | C-Bill state plus `1631:1FDF` | `SUBTRACT_C_BILLS` / `Display_Text_CBill_Balance` | Verified: signed-WORD-to-DWORD comparison/subtraction, saturation to zero, then refresh. |
| `F2` | `1F3D:086A` | `TIMED_WAIT_INPUT_CHECK` | Recover the routine's stable name and input/timing dependencies. |
| `F3` | `3092:D30C` and `0FDC:05F7` | `BRANCH_STATE_TABLE` / `Read_Bld_Target_05F7` | Verified: read the state byte, double it as a WORD-table index, read the selected little-endian absolute target, and assign it to the cursor. |
| `F5` | `1CD3:0004` | `CALL_ACTION_DISPATCHER` | `PARTY.BLD:178B` uses action `1E`, verified as `RECRUIT_REX_AND_START_KURITA_AMBUSH`. The native dispatcher subtracts one before indexing its jump table; map the remaining values before naming them. |
| `F6` | `0800:1A13` | `Prompt_Yes_No` | Verified: call with default Yes; returned 16-bit Yes follows the encoded target and No continues after it. |
| `F9` | `1E56:0B5E` and `0FDC:05F7` | `MENU_BRANCH_TABLE` / `Display_Menu_Choices_And_Check` / `Read_Bld_Target_05F7` | Verified interpreter path; trace the menu-state fields indexed by the operand during the menu callee's review. |
| `FA` | `1E56:0004` | `Draw_Menu_Border` | Confirm the border/layout ID contract. |
| `FB` | `1F3D:0259` | `WAIT_FOR_KEY` | Recover the precise keyboard routine name and blocking behaviour. |
| `FC` | `1E56:03F5` and `207F:3B9E` | `DISPLAY_TEXT` / `Display_Text_From_Memory` | Verified: render the inline NUL string, obtain its length from `207F:3B9E`, then advance by length plus one. Correct the false `void` helper declaration during that callee's review. |
| `FD` | `1E56:0388` | `Draw_Top_Graphic_Sidebar` | Decide whether redraw and text-offset reset are one portable method or separate operations. |
| `FE` | `1E56:0281` | `Menu_Memory_Variables` | Confirm the layout-record base, stride, and field meanings before defining a C# layout type. |

## Rules for closing entries

For each row, record the clean-assembly call or branch instruction, the callee's
entry address, its calling convention and 16-bit argument widths, the annotated
decompile label, and the final stable method/tag name. A matching Reko pseudo-C
call alone is insufficient because near/far pointers and integer widths may
have been extrapolated as 32-bit values.

Indirect helpers and inline case blocks should receive tags even when they do
not become separate C# methods. Update the canonical opcode table only after the
address evidence and behaviour agree.
