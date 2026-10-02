# Sol: systematic BTECH_1431 C-to-ASM comparison

Checked2026-09-17 against baseline `922e2be`: the one retained method,
`Heal_Characters`, locally matches. No executable C changes or new mismatches.
This is a whole-local-flow comparison, not compilation or gameplay validation.

## Evidence and boundaries

- Private `BTech-Reko-expanded/BTECH.reko/BTECH_1431.asm` SHA256
  `EEFBE47F29FC228AE04C714A21B52D248F2CAAF562D4FE2C8E43699D522BACEA`.
- Corresponding `.dis` SHA256
  `726AEB81CFD53295DE839111308494E8D74E2FA830B22F63047CF2F7E74F8B23`.
- Static3EDB listing confirms selectors5522/5526=3092,5524=3EDB and the
  text-pointer/dice tables. The `.dis` completes035C's return; next segment's
  initial bytes5D/CB are POP BP/RETF. Do not mistake the truncated ASM for a
  missing return in the original program.

Compared000A through035C and its complete FAR-return epilogue: all meaningful
arguments, reads/writes, temporary computations, calls, branches and loops.
Compiler stack probe, frame allocation and saved registers are abstracted.
Named views assume native BYTE/WORD and16:16 FAR addresses, including WORD
offset wrap for malformed indexes; ordinary host-C arrays alone do not provide
that contract. Shared callee behavior is a boundary, not a certificate for those
files. This method has no adapter-specific branch requiring an EGA omission.

## Checked workflow

| Native blocks | Local semantics confirmed |
| --- | --- |
| 000A–0023,02FB–0357 | Nonzero recovery BYTE immediately draws panel; eight living slots checked for signed Health!=signed Body*10; selects25A4/25CF text, wrapper and key; no dice/timer reset |
| 0026–007F | Initial skill0/slot0; greatest signed medical BYTE wins by strict improvement, first tie retained; action17 injury query; no-injury exit |
| 0082–011F | Menu/sidebar calls; cloth1, owned medkit2, field kit3 only if skill>=3; nonzero paid tier overrides skill and uses medkit or facilities4 by signed tier>2; party medic name via FAR pointer otherwise |
| 011F–01B4 | Zero-skill prose versus skill/equipment FAR descriptions; skill1/2 uses25DE+skill*4,3/4 uses25DA+skill*4,5+ uses25EE; common text and green colour |
| 020A–02A7 | Eight17-byte records; dead FF skipped; signed Body*10 WORD maximum and signed Health equality gate; character name/text; BYTE2601+skill*4+equipment CBW; high-nibble multiplier and low-nibble dice count |
| 01BC–0207 | Test old count then decrement; D6 sum WORD wrap; low-WORD multiply; signed Health plus gain WORD; signed upper clamp only; low BYTE write and next slot |
| 02AF–02DC | Text-colour WORD37FE=15; wait/prompt; sidebar argument0; recovery BYTE D335=63 |
| 02DC–02FB | No injury: paid/nonzero returns silently; own-party/zero panel,2591 text, keyboard drain then key; no recovery reset |
| 035C / split epilogue | Restore saved registers/frame and FAR return |

## Important equivalence qualifications

- CharacterNameList starts3EDB:01CA; medical/equipment descriptions are FAR
  pointers, not scalar WORD addresses. Dice matrix starts2602, eight rows of four;
  current biased indexing reproduces native2601+skill*4+equipment.
- CBW followed by SAR4/AND0F extracts packed multipliers even for high-bit bytes.
  Entries with a zero high nibble have dice count0..15; with a nonzero nibble the
  count is masked0..15. Thus the post-decrement loop never starts negative,
  including all256 possible table BYTE values.
- The initial C cached Health before display/dice whereas native reloads it
  after dice. The preservation cleanup now explicitly reloads Health at that
  native position, removing this particular helper non-mutation dependency.
- Signed Body/Health, low-WORD arithmetic and low-BYTE writes are intentional.
  Overfull health counts as injured and can be reduced; there is no lower clamp.
  Best medic defaults to slot0 even if dead when no positive skill wins. Nonzero
  service values with the high WORD bit set use signed equipment-tier comparisons
  and potentially wrapped/out-of-range table addresses, not new validation.
- Healing itself neither charges money nor clears equipment inventory. Treatment
  resets recovery to63 after the pass even if no gain occurs; early exits do not.
  Hospital script gating remains a separate caller-level finding.

Verification: existing healing models plus fresh selector, description-index,
signed clamp/BYTE-store and one-record checkpoint witnesses. These are static
models, not execution of the original EXE or gameplay certification. Source-body
hash in the registry binds this result to the checked annotated method.

2026-09-17 preservation update: rechecked the complete routine after the
[in-place healing cleanup](../preservation/1431_HEALING_CLEANUP.md). Named
constants, WORD locals, unsigned packed-nibble decoding and the native health
reload retain the match under the same native-memory bounds. Actual-source C17
tests supplement, but do not replace, the ASM comparison or gameplay validation.
