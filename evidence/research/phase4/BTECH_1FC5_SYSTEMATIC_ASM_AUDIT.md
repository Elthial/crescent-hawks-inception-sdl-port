# Sol: systematic BTECH_1FC5 C-to-ASM comparison

Checked2026-09-17 against baseline `f3f0c4f`: all15 retained native routines.
All15 locally matched under the contracts below. No executable C changes.
Corrected the fixed-tone helper's misleading human outline: its parent owns
speaker-off. Fresh registry hashes record this comparison, not the earlier review.

## Evidence and scope

Read the complete private `BTech-Reko-expanded/BTECH.reko/BTECH_1FC5.asm`:
SHA256 `7539A495DEBEFBE92F73A5D9BA6C30D8B141AFD4598199B2AAFE6DBB13BF2F60`.
Corresponding `.dis`:
`E80EE3F3CE06F6254982643DE48665252C1E6339DA69B98D36C53823460B0F32`.
Expanded EXE:
`F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE`.

Compared every meaningful local operation/path: argument widths, shared WORD
stores, stream cursor, record classification, field order, helper arguments,
signed branch conditions, wrapped products/additions, independent counters,
timer/speaker calls and exit cleanup. Compiler frame/probe/register restoration
is abstracted. The per-segment ASM truncates07DA at its last ES load. `.dis`
completes its signed comparison/continuation and0868 exit; private EXE bytes
confirm that tail and RETF086C. Leading bytes0000/0001 belong to the preceding
segment's final call operand/return, not another retained function.

Matching means local semantic agreement, not compilation or audible fidelity.
This remains research pseudo-C with native16-bit unsigned wrap/signed casts,
proper16:16 FAR address views and overlapping globals, not a packed host struct.
Use original valid sound-library records/one-based IDs. Generic C array bounds
do not reproduce arbitrary invalid-ID reads outside the313-WORD DS5008 view;
no malformed-stream safety or universal termination claim is made. Shared
callee functionality/hardware timing are separate audits. Private assets are
neither copied nor staged.

## Whole-routine ledger

| Routine / native range | Result | Simple purpose and checked operation |
| --- | --- | --- |
| Sound_Setup_0002 0002–02A2 | Matched | Find the requested one-based effect, decode its records and stage generator parameters. Signed marker/skip widths, following-zero separator and all dispatch branches checked. |
| Sound_Operation_02A3 02A3–02EA | Matched | Repeat fixed tone: setup timer, signed repetition loop with live shared limit, two WORD arguments, unconditional speaker-off. |
| Sound_Operation_02EB 02EB–0344 | Matched | Repeat seeded noise: same parent lifecycle, four WORD arguments and independent repetition counter. |
| Sound_Operation_0345 0345–03A7 | Matched | Repeat divisor sweep: five WORD arguments, signed live repetition bound, cleanup even when skipped. |
| Sound_Operation_03A8 03A8–040A | Matched | Repeat descending mask sweep: five WORD arguments and unconditional parent cleanup. |
| Sound_Operation_040B 040B–046D | Matched | Repeat ascending mask sweep with the same parent lifecycle. |
| A_Timer 046E–047A | Matched | Bind207F:001C PIT setup; no divisor or new game timer here. |
| Sound_Ptr_Operation_047B 047B–04E3 | Matched | Store tone parameters; enable only a nonzero divisor; fifty signed calibrated delay passes; no local speaker-off. |
| A_PC_Speaker_OFF 04E4–04F0 | Matched | Bind207F:0051 speaker disable, without restoring PIT settings. |
| Sound_Freq_Loop 04F1–0599 | Matched | Set default seed1000, clear speaker gate, warm up, toggle then run masked countdown calls. |
| Sound_Operation_059A 059A–0642 | Matched | Same noise loop with a caller-provided seed; four WORD stores and all signed loops checked. |
| Sound_Operation_0643 0643–06F5 | Matched | Stage sweep parameters, setup timer, run independent sweep/step loops and unconditional speaker-off. |
| Sound_Ptr_Multiply_06F6 06F6–0746 | Matched | Always write divisor, including zero; one calibrated signed low-WORD delay loop. |
| Sound_Streaming_1 0747–07D9 | Matched | Descending mask: start minus wrapped index*step, strict signed current>stop, pass mask/minimum/toggle count. |
| Sound_Streaming_2 07DA–086C | Matched | Ascending mask: start plus wrapped index*step, strict signed current<stop; complete truncated native tail checked. |

## Dispatcher accounting

Native DEC WORD SoundId is C's narrowed SoundId-1. Both traversal and playback
inspect following WORDs at+1/+2 for a zero pair. Traversal first tests the current
WORD for zero, then the remaining-ID count; playback has no invented current-zero
or EOF guard. Signed command<=1000 is a three-WORD direct record, including
high-bit negative WORDs.1001 is six WORDs;1002/1003/1004 seven WORDs; unknown
positive extended commands advance one WORD. No bounds/ID guard is introduced.

Direct records stage repetition/divisor/delay.1001 stages repeat/mask/minimum/
toggle count/seed;1002 repeat/centre/half-span/delay/sweep count/step;
1003/1004 preserve the subtype until choosing descending/ascending, then replace
command scratch4612 with the local repeat WORD. Their other fields are start/
stop/subtract/toggle count/step, not sound IDs or frequencies.

ASM increments the private BP-4 cursor before each parameter read/call; C batches
some additions after the call. For valid records and callees not corrupting the
caller's stack, this yields identical parameter order and next-record position.
It does not justify generic host-array arithmetic for indices near offset wrap.
Shared sound fields are reloaded where native loop bounds are live; independent
loop locals do not alias the global repeat count.

## Generator and register contracts

- Every repetition wrapper calls timer setup even when signed repeats<=0,
  and speaker-off after its loop. Divisor sweep0643 independently owns setup/
  cleanup too; those nested calls are not redundant transcription errors.
- 047B skips enabling on divisor zero; it does not explicitly disable an
  already enabled speaker. Its fifty outer passes each reset the inner counter
  to zero. Delay bound is signed WORD(low(scale*multiplier)), recomputed each
  test.06F6 always enables/writes even divisor zero (PIT encoding65536), and
  has one such delay pass. Neither helper independently switches off.
- 04F1/059A warm up from1 while signed counter<calibration5006. Their toggle
  loop starts0 with signed live4312 bound; each toggle resets a separate inner
  counter to0 and calls00A9 while signed counter<calibration.04F1 seed is1000;
  059A uses its caller seed. Both clear gate bits via0067 before warmup and
  leave final speaker-off to their parent.
- Shared007D toggles bit1 and sets evolved DX to ROR16(seed+9248,3).00A9 copies
  DX to CX, ANDs mask, ORs minimum bits and counts down CX; it does not consume
  or update DX. Every countdown in one toggle therefore sees the same DX.
  The existing207F C retains an explicit `dx` research binding. This is a
  required shared-register contract, not an ordinary portable C calling ABI.
  A C# implementation should pass that state explicitly. This local audit does
  not newly certify207F bodies or its misleading legacy helper names.
- 0643 compares signed WORD(2*half-span) against low WORD(stepIndex*step),
  and plays centre-half-span+offset with WORD wrap. Inner step and outer sweep
  count are distinct.0747/07DA strictly exclude equality at the stop mask and
  pass WORD(currentMask-subtract), not an unwrapped difference.
- Zero steps can prevent progress; signed wrap may produce nonmonotonic
  sweeps or unexpectedly skipped delays. Preserve native behaviour in research
  code; safe API validation is a separate portable-toolkit decision. Busy waits
  depend on CPU/emulator speed, and source agreement does not prove audible
  PC-speaker/Tandy emulation or toolkit renderer fidelity.

## Verification

Run the seven existing sound dispatch/repetition/field/fixed-tone/noise/divisor/
parameter-sweep scripts and the new systematic audit witness script. Inventory
verification enforces current body hashes and comment-only C edits. Optional
private EXE witnesses hash-check the image then verify the missing final native
tail without staging it. All tests are synthetic/source/byte witnesses, not
gameplay or actual playback. No new expensive Astra review is needed for these
direct instruction comparisons.

Passed327,687 new systematic witnesses (including the profile-checked native
tail),3,801,726 existing sound assertions,39 caller/field checks and340 inventory
checks. Inventory confirms312 definitions with no executable C changes.
`git diff --check` passes. These counts are not gameplay coverage percentages.
