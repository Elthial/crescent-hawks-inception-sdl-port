# Phase 2E — lossless BLD disassembly

## Review boundary

This block adds a read-only disassembler for the verified BLD container,
transformation, fixed-width operands, absolute branch targets, and inline text.
It does not execute scripts or assign speculative gameplay meanings.

`F3` and `F9` have variable inline word tables with no encoded length. A later
review of all original scripts established a corpus rule: each observed table
is a consecutive sequence of in-payload absolute target words, and the word
immediately after it is out of range. The disassembler now applies that rule,
labels the boundary as inferred, lists every target, and continues through the
rest of the payload. It retains the conservative `ambiguous-tail` stop when no
in-range table entry can be established.

## Command

```text
InceptionTools disassemble-bld FILE [--game-dir PATH] [--offset N] [--json]
```

Offsets are always relative to decoded payload zero; the corresponding file
offset is two bytes greater. Branch destinations are checked against the
payload boundary. JSON retains both the complete encrypted stored payload and
complete decoded payload, as well as the bytes owned by each output item.

Text output abbreviates long byte sequences but prints decoded inline dialogue
and typed operands. JSON is the lossless interchange form.

## Verification

The current synthetic harness passes **156 assertions**. The BLD cases verify
container length, exact decode, inline text, absolute target handling, inferred
variable-table targets, lossless item coverage, human-readable output, and
start-offset bounds. No original BLD content is included in the fixture.

All **36** F3/F9 tables in the **26** ignored original BLD files were decoded
without an unresolved tail. This validates the rule for the local executable
and data profile; it is not yet proof that another release uses the same
terminating convention.

The existing Astra review item `A-001` remains appropriate for higher-level
control-flow recovery and for judging whether a more general table-boundary
proof is needed before supporting another executable profile.
