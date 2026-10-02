# Research method

## Confidence levels

| Mark | Level | Meaning |
|---|---|---|
| **V** | Verified | Directly demonstrated by original bytes, disassembly, or a repeatable runtime trace. |
| **P** | Probable | Multiple pieces of evidence agree, but execution or every field has not been demonstrated. |
| **H** | Hypothesis | A useful interpretation with incomplete or ambiguous evidence. |
| **U** | Unknown | Location or value is known but its meaning is not. |

Confidence belongs to individual claims, not whole files. A record length may be
verified while the meaning of one byte within it remains unknown.

## Evidence order

When sources conflict, use this order while also considering the quality of the
specific observation:

1. Original file/executable bytes and repeatable runtime traces, bound to a
   named executable profile.
2. Assembly and address-level data-flow analysis. For annotation review, begin
   with `BTech-Reko-expanded/BTECH.reko/*.asm` and `*.dis`, then compare the
   corresponding hand-annotated block.
3. The hand-annotated `Btech/` Reko output and late `BTECH.h` address notes.
4. Existing InceptionTools parsing behaviour that can be matched to original
   bytes.
5. Imported UnBattletech analysis, generated code, and documentation, only
   after checking that the claim is format-independent or version-matched.
6. Game rules or external BattleTech knowledge used as a plausibility check.

External tabletop rules must never be used to invent binary semantics.

## 16-bit data model

The original is 16-bit little-endian DOS code written in 16-bit C with some
embedded assembly. The Reko project was operating with a 32-bit model while
decompiling it, so uncertain C types commonly defaulted to 32-bit integers or
pointers. Use the following as the initial model when interpreting output:

- byte: 1 byte;
- C `int`, `unsigned int`, word, or near offset: normally 2 bytes;
- near pointer: normally 2 bytes;
- dword: 4 bytes;
- segmented far pointer (`segment:offset`): 4 bytes;
- packed on-disk and save records: use the width demonstrated by byte/word
  instructions and the record stride, not the CPU's native width alone.

This distinction matters in `BTECH.h`: a decompiler may give an access a C type
that is convenient or wrong, while the instruction (`mov al`, `mov ax`, byte or
word pointer), index multiplication, adjacent addresses, and fixed record length
show the actual storage width. Conversely, two adjacent bytes must not be split
merely because a parser currently indexes them separately if the original code
loads them as one little-endian word.

Never accept Reko `int`, pointer, `long`, or `unsigned long*` syntax as width
evidence by itself. Confirm using the original instructions, parameter stack
movement, return registers, segment-register use, pointer normalization, array
stride, and adjacent known addresses. A four-byte pointer requires evidence of a
far `segment:offset` value; uncertainty in Reko is not that evidence.

## Recording a finding

A durable finding should include:

- the claim and confidence;
- the relevant file/record/segment offset;
- the functions that read or write it;
- a short description of the observed transformation;
- competing interpretations;
- a reproducible inspection or trace where possible.

## Original-files policy

- `chinception/` is the local, ignored reference installation.
- Do not commit or upload original external game files or derived asset copies.
- The original `.EXE` remains an uncommitted research input, but code, strings,
  constants, lookup tables, and data arrays embedded in it may be reconstructed
  in source as part of the replacement executable.
- Tools take a user-provided game directory; during local development that may
  be `chinception/`.
- Documentation may contain filenames, byte layouts, addresses, short byte
  sequences needed to identify a format, and hashes used for validation.
- Synthetic fixtures should be minimal and must not reconstruct external
  copyrighted game files.

See [DISTRIBUTION_POLICY.md](DISTRIBUTION_POLICY.md) for the complete project
boundary.

## Imported UnBattletech policy

`UnBattletech-main/` may be read and its local tooling may be run or adapted for
research. New authoritative documentation and maintained code belong outside
that imported snapshot unless a temporary local experiment specifically needs
to modify it. There is no requirement to update its wiki or upstream repository.

UnBattletech's decompiler addresses are not used for direct annotation
comparison unless its input executable is independently fingerprinted and
matched. Its BLD loader, opcode documentation, interpreter, opcode names, and
derived control flow are specifically excluded: the incorrect payload origin
invalidates the decoded input and everything inferred downstream. Its unrelated
runtime and emulator setup may still be used as experimental tooling.

## Clean Reko baseline policy

`BTech-Reko-expanded/` preserves the Reko output before the human annotations in
`Btech/`. Treat the three generated views differently:

- `.asm` is the preferred static instruction reference;
- `.dis` preserves Reko's intermediate data-flow and is useful for diagnosing
  how pseudo-C was formed;
- `.c` is convenient for control-flow comparison, but inferred types and
  expressions remain hypotheses.

The directory's `BTECH.dcproject` declares `x86-real-16` and Reko 5 project
schema; the generated header identifies Reko 0.9.2.3. Its included executable
does not hash or size-match `chinception/BTECH.EXE`, so conclusions must name
the executable profile and address equivalence must be established from code
bytes or control flow. See
[reference/EXECUTABLE_PROFILES.md](reference/EXECUTABLE_PROFILES.md).

## Optional Astra confirmation

Sections whose correctness depends on unusually deep control-flow recovery,
alias analysis, or reconciliation of several plausible interpretations are
listed in [investigations/ASTRA_REVIEW.md](investigations/ASTRA_REVIEW.md). Astra
is an optional confirmation pass, not a substitute for binary evidence. No such
review is started without an explicit decision to spend it on a listed block.
