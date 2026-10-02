# DOS hardware, graphics, and input constants

This extracts low-level constants from `BTECH.h` without treating every old name
as correct.

## Video adapter IDs

The original executable supported multiple adapters, but the hand-maintained
decompile has deliberately had the Tandy, CGA, and other non-EGA pipelines
removed. These IDs are retained as historical executable context; the
preservation port targets the remaining EGA pipeline.

| ID | Adapter label | Confidence |
|---:|---|:---:|
| `0x00` | CGA | P |
| `0x01` | Tandy | P |
| `0x02` | EGA/VGA | P |
| `0x03` | MCGA | P |

## I/O ports and registers

| Value | Current meaning | Confidence |
|---:|---|:---:|
| `0x03CE` | VGA graphics-controller index port | V |
| `0x03C4` | VGA sequencer index port | V |
| `0x03DA` | Input-status register | V |
| `0x08` | Vertical-retrace bit mask | V |
| `0x08` | Graphics-controller bit-mask register index | V |
| `0x0205` | Packed index/data write selecting VGA write mode 2 | P |

The constants `0x8008`, `0x4008`, ... `0x0108` appear to pack a mask byte with
register index `0x08` for 16-bit port output. Their names currently count bits in
reverse human order; preserve raw values during annotation.

## Display segments and buffers

The scratch header lists `A000`, `A400`, `A800`, `AC00`, `B800`, and `0A00` as
graphics memory/buffer values. Some are conventional physical video segments;
others may be allocated segment selectors, page offsets, or decompiler-confused
values. Do not expose them as a single `VideoMemoryAddress` enum until their use
sites are traced.

## Keyboard constants

Most standard ASCII values in the header are straightforward. The project-owner
correction fixes the game-action label `KEY_B` at `0x41` and removes `KEY_W`.
The unchanged historical scratch pad still shows `KEY_B = 0x42` and
`KEY_W = 0x57`; do not propagate those declarations into maintained code. The
shared value with ASCII `A` means the semantic action label must be kept
separate from a generic ASCII-letter enum until its input call site is reviewed.

Movement command words are `FFB8`, `FFB3`, `FFB0`, `FFB5`, `FFB7`, `FFB9`,
`FFAF`, and `FFB1`. These are likely sign-extended transformed scan codes rather
than ASCII. Direction assignments remain **Probable** until checked against the
keyboard remapper.

## Timing and dimensions

- `0x32` and `0x3C` are labelled 50 Hz and 60 Hz refresh counts. **Probable**
- `SCREEN_RES_X = 319` is a zero-based 320-pixel bound.
- `SCREEN_RES_Y = 119` is a zero-based 120-line bound. No retained code currently
  references the symbol. It may have belonged to one of the deleted non-EGA
  graphics paths or a 120-line viewport, so its historical owner remains
  **Unknown** even though the `120 - 1` arithmetic is valid.

## EGA palette IDs

IDs `0x00..0x0F` follow the conventional 16-colour EGA ordering recorded in
`BTECH.h`: black, blue, green, cyan, red, magenta, brown, light grey, dark grey,
bright blue, bright green, bright cyan, bright red, bright magenta, bright
yellow, and bright white. Hardware palette-register remapping is a separate
operation.
