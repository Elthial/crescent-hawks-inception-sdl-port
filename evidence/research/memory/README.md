# Segmented memory reference

This folder normalizes the memory discoveries previously collected in
`Btech/BTECH.h`. The original header remains the historical scratch pad; these
documents are the readable reference.

## Sol: current coverage, 2026-09-17

Record arrays, save boundaries and many combat/pointer views have ASM-backed
evidence. This is **not a complete memory map**: unnamed bytes, scratch-buffer
ownership, DS/SS aliasing and live post-unpack address mapping remain open.
Confidence belongs to individual views, not an entire segment. The scratch
header is not a compilable layout.

Synthetic assertions are not gameplay validation. Spice86 smoke tests produced
private execution dumps, not confirmation of workflows, rendering or music.
See [runtime validation](../investigations/RUNTIME_VALIDATION.md),
[deep-review questions](../investigations/ASTRA_REVIEW.md) and
[the refreshed TODO review](../investigations/TODO_REVIEW.md).

Sol: Subsequent exploration-dump comparison establishes a first live mapping:
analysis segments minus0683, image load base017D for the captured run. The
code-region image and five static table views match after relocation, except
known writable music state. Full gameplay/DS-SS/ownership questions remain;
see [code confirmation](../investigations/SPICE86_EXPLORATION_CODE_CONFIRMATION.md).

- [Segment roles and code map](SEGMENTS.md)
- [Mutable game/save state in segment 3092](SEGMENT_3092_STATE.md)
- [Overlay, pointer, and type rules](TYPE_AND_ALIAS_RULES.md)

## Address notation

- `3092:C614` means segment `0x3092`, offset `0xC614`.
- `record+0x11` means a byte offset inside a packed record.
- A range ending at another address is half-open unless explicitly described as
  inclusive: `C614..C724` contains `0x110` bytes and does not include `C724`.
- File offsets are always labelled `file+0xNN`.

Addresses describe the game's segmented address space, not flat host pointers.
