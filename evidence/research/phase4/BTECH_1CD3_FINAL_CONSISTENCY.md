# Sol: 1CD3 final consistency and helper contracts

Sol: Correction2026-09-17: the raw2F training-flag interpretation below is
superseded by [the systematic audit](BTECH_1CD3_SYSTEMATIC_ASM_AUDIT.md).
Native C623 is Health (+0F), not TrainingFlags (+10=C624). The signed threshold
and subtract4 instructions were read correctly, but attached to the wrong field.

Evidence: original-version local ASM, 1F3D:0525–05BB and
1CD3:0027–01C2,174E–175E,17C6–1832; inline bytes in BTECH_3EDB.asm.
This closes the queued first-pass consistency review, not a compilation or
runtime correctness certification of the research C/header scratchpad.

## Palette FAR contract and maintained callers

LES SI,[BP+6] at057E proves the parameter is a16:16 FAR byte pointer.
The corrected helper and header declaration retain that pointer. The maintained
EGA body waits for retrace, loops sixteen WORD register indices, CBW-sign-extends
each colour byte, and adds8 only when signed colour>7. Normal colours8..15 map
to hardware16..23. High-bit inputs remain negative WORDs, not unsigned colours
that receive an extra8. Native022A receives two WORD arguments.

All five maintained calls were checked: title2FE8:0000, stats3EDB:1348,
stats restore2FE8:0000, ending3058:0000, ending restore2FE8:0000.
The title's remaining offset-only call was corrected. Obsolete palette TODOs
were removed. Historical MCGA sources0010/1358 use WORD palettes and a separate
native body; removed adapter code was not reconstructed or sent through EGA.
The inherited low-level022A helper name remains a research hardware binding,
not an assertion that this repository builds as C.

## Raw2F training flags: superseded field interpretation

174E compares Jason's BYTE C623 with5, signed JLE exits;175A subtracts4.
Thus0..5 and80..FF remain unchanged;06..7F become02..7B. In the ordinary
0..7 range, only6 and7 lose bit2. This is NOT equivalent to always clearing
bit2:4 and5 survive, and wider values can differ too. Tech/Medical bits0/1
are known; bit2's narrative purpose remains unconfirmed. Constants identify
the unknown bit value and exact threshold without inventing a story meaning.
Bad address-of comparison/assignment was removed; no gameplay bug claim added.

## Opening-branch consistency corrections

0027 selects full125-byte FAR templates: selector0 Locust2FE8:02F0,
selector2 Chameleon04E4, every other nonzero selector Wasp036D. The sprite
family remains Locust for0, Commando-family for all nonzero selections.
007C copies indices0..124 into slot0 C724..C7A0; the old3959 termination
and undeclared Reko pointer pieces were false transcriptions. Jason's C620
assignment and mech PilotId C79D are BYTE values, not address expressions.

00C3 sets animation cursor0 to FAR2FE8:0280 (direction stream2 within0270).
0145/015F store the animation helper's returned AL in409A, not constant0.
0170 sign-extends the mission BYTE before the WORD call. Existing four world-X
steps C3C..C3F, six redraws per step, world Y C04F and timing behavior remain.

0187's Locust branch displays only the Locust string through the common path;
Wasp/Chameleon first display their name then the suffix48EE. Locust's shared
text variable was previously uninitialized. Assignable FAR text arrays were
changed to pointer variables; removed KEY_W is replaced by native ASCII'W'.
The unused ax_6665 declaration was removed; stock text offset ax_1849 was
renamed StockDialogueTextOffset_1849, preserving its original numeric suffix.

## End helpers

17C6: layout7, top sidebar, border0. No input wait.
17EA: forwards FAR string BP+6/+8 to1E56:03F5, then calls1F3D:086A.
Its previous ushort parameter lost the segment. It does not itself call the
ASCII key reader; inherited naming is documented rather than globally renamed.
1809 passes inline cannot-afford text4F7E. 181E passes inline4FA0, whose native
bytes are0D,00 (carriage return, terminator), not a stored pointer. A named
segment field replaces the undefined ptr4FA0 dereference.

## Verification and remaining port obligations

Synthetic checks cover all256 colour bytes and flag bytes, FAR palette segment
identity/offset wrap, all256 template selectors and125-byte copy boundaries;
source guards protect the corrected contracts. Prior staging, scenario and
text regressions are rerun. These do not execute the original game or hardware.

Passed: 914 helper-contract assertions, 1,385 staging/ending assertions,
131,590 scenario/name assertions, 6,443 text assertions, and `git diff --check`.

The first-pass1CD3 review is now covered by the linked block audits. Remaining
work is cross-project port preparation, not “all code is correct”: scratchpad
overlapping fields, inherited low-level hardware/runtime bindings, string literal
FAR provenance, shared-label flow and gameplay traces still need a compilable
state model and integration verification. Scenario health-cap intent and bit2
meaning remain explicitly uncertain. No original-game bug was inferred from
Reko-only errors. No expensive model review was needed for these direct ASM fixes.
