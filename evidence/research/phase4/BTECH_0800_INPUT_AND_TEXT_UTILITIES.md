# `BTECH_0800` input and text utilities

This note covers the bounded utility cluster at `0800:29F5-2A92`. The next
routine, `Draw_Persistent_Map_Effects_2A93`, is outside this review block.

## `Get_CBill_Allowance_Wealth_Limit` (`0800:29F5`)

The routine reads two adjacent bytes and assembles them little-endian:

- `3092:D33F` is the low byte.
- `3092:D340` is the high byte.
- `DX` is explicitly cleared before the far return.

The resulting ABI value is therefore a zero-extended 16-bit allowance in
`DX:AX`. The C return type remains `unsigned long` to represent that calling
convention; it is not evidence that the stored field is 32-bit.

Confidence: **confirmed** from the clean assembly.

## `Drain_Pending_Keyboard_Input_2A2B` (`0800:2A2B`)

The old annotation called this `Check_For_Input` and inverted its gate. The
actual control flow is:

1. If word `3092:3938` (`DisableInput`) is nonzero, return without polling.
2. Call `Pending_Input` (`1F3D:002F`).
3. While it returns nonzero, call `Keyboard_Get_ASCII_Hex_Input`
   (`1F3D:0259`) to consume one item and poll again.

The routine is consequently a queue-drain helper, not a general input test.
All known source call sites now use the behavioural name. This conclusion does
not by itself establish every higher-level meaning of `DisableInput`; it only
records the branch visible here.

Confidence: **confirmed** from `cmp word ptr es:[3938h],0` followed by `jnz`
to the return.

## Fixed text helpers (`0800:2A4F-2A92`)

These routines pass near offsets in the current data segment to
`Display_Text_From_Memory`; the referenced bytes are static strings, not
runtime pointer variables.

| Address | Source name | Data address | Exact string | Behaviour |
|---|---|---|---|---|
| `0800:2A4F` | `Prompt_And_Wait_For_Key_2A4F` | `3EDB:050D` | `"\rPress a key."` | Displays the prompt, then reads one input item. |
| `0800:2A69` | `Display_Plural_Suffix_2A69` | `3EDB:051B` | `"s"` | Appends a plural suffix; no leading space is stored. |
| `0800:2A7E` | `Display_Sentence_Period_2A7E` | `3EDB:051D` | `"."` | Appends a sentence-ending period. |

The previous C transcription lost the prompt's leading carriage return and
period, added a false space before the plural `s`, and treated `051D` as a
pointer-valued field. The initialized data bytes and immediate offsets in the
clean assembly resolve all three cases.

Confidence: **confirmed** from the immediate offsets in the clean assembly and
the initialized `3EDB` data image.

## Porting implications

- Preserve the input-drain routine as a distinct operation; do not silently
  fold it into a generic key-read API.
- Preserve the zero-extension in the allowance getter even though the original
  C ABI returns a 32-bit `long`.
- Treat these three text fragments as executable-owned strings. They may be
  represented directly in replacement source under the project's distribution
  policy.

No Astra review is requested for this block because its branches, widths,
arguments, and referenced bytes are direct rather than inferential.
