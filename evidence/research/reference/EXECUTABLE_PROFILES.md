# Executable profiles and decompiler provenance

This project currently has two ignored executable inputs. They serve different
research roles and are not interchangeable.

## Profiles

| Profile | Path | Bytes | SHA-256 | Role |
|---|---|---:|---|---|
| `installed-reference` | `chinception/BTECH.EXE` | 152429 | `F2A9A023D79927B8072DE11DDD6E03DA6D3357D49181D8FBA6D12988BF8CC0EE` | Runtime compatibility target for the replacement executable. |
| `reko-expanded-2020` | `BTech-Reko-expanded/BTECH.EXE` | 260416 | `F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE` | Input paired with the clean, pre-annotation Reko output. |

The different hashes and sizes are expected. The distributed executable was
packed/encrypted and had to be unpacked into the expanded executable before
Reko could analyse its real program segments. This provenance is confirmed by
the project owner; the MZ metadata independently agrees with that workflow.

| MZ field | `installed-reference` | `reko-expanded-2020` |
|---|---:|---:|
| Declared file bytes | 152429 | 260416 |
| Header paragraphs / bytes | 32 / 512 | 825 / 13200 |
| Relocation entries | 0 | 3292 |
| Initial `CS:IP` | `2367:0010` | `187F:2D82` |
| Initial `SS:SP` | `3E0B:0080` | `3C5E:0800` |

The installed executable also contains the diagnostic text `Packed file is
corrupt`. Its zero-relocation packed image contrasts with the reconstructed
relocation table in the expanded analysis image.

These are two representations of the same executable workflow, not competing
candidate releases. They still require different address domains: packed file
offsets cannot be equated directly with offsets in the expanded image.

## Reko baseline contents

`BTech-Reko-expanded/BTECH.dcproject` identifies an MS-DOS `x86-real-16`
project. `BTech-Reko-expanded/BTECH.reko/` contains:

- segment ASM such as `BTECH_3092.asm` and `BTECH_3EDB.asm`;
- code-segment ASM, `.dis` intermediate output, and pseudo-C;
- generated globals and header output;
- the PSP view and an `X86Rw.tests` artifact.

The generated `BTECH.h` reports Reko version 0.9.2.3. This baseline predates the
human annotations maintained in the flattened C files under `Btech/`.

## Cross-reference workflow

For each bounded annotation block:

1. identify the segment/address and existing annotated function in `Btech/`;
2. inspect the corresponding clean ASM for operand widths, segment use, stack
   movement, calls, jumps, and memory addresses;
3. use `.dis` to understand Reko's recovered data flow when the pseudo-C looks
   suspicious;
4. compare clean pseudo-C with the annotated C to isolate human changes;
5. confirm important data or behaviour against `chinception` bytes or a runtime
   trace before calling it compatible with the installed profile;
6. record the source profile, address, evidence, confidence, and unresolved
   differences in the resulting note.

Do not treat the three Reko renderings as independent evidence: they are views
of one analysis. In particular, inferred 32-bit C types do not outweigh 16-bit
instructions.

## Address-transfer rules

- Never transfer raw file offsets between profiles.
- Segment addresses may be transferred only after representative code bytes or
  control flow match.
- Structure strides and table contents should be checked independently in both
  profiles when they are important to the port.
- If a function differs, give each profile a separate provenance entry rather
  than forcing one annotation to cover both.
- UnBattletech Reko addresses require their own executable fingerprint before
  address-level use. Its format research and emulator tooling can still be
  verified independently.

## Open verification task

Sol: The first post-unpack exploration capture now confirms116,416 bytes of
the code-region image after applying all3292 MZ relocations, with only14
documented writable music-state differences. Five static table views also
match. The live image base is017D for that run; this establishes broad address
transfer, not a compressed-file offset map or pseudo-C equivalence. See the
[reproducible dump confirmation](../investigations/SPICE86_EXPLORATION_CODE_CONFIRMATION.md).
The tasks below remain for other load contexts and detailed unpacking provenance.

Document the unpacking/relocation mapping well enough to reproduce it and to
connect packed executable offsets to expanded segment addresses when needed.
Representative embedded table/string anchors and already-understood functions
can serve as regression checks. The resolved provenance note is
[C-015](../investigations/CONTRADICTIONS.md).
