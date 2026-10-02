# Sol: five sound repetition wrappers

Scope1FC5:02A3–046D. Evidence: original-version local BTECH_1FC5.asm.
046E timer binding and playback helpers047B onward remain outside this block.

All five wrappers call046E timer setup, initialize an independent WORD local
BP-2 tozero, call their playback helper while signed global4612>signed counter,
increment the counter once per call, then unconditionally call04E4 speaker-off.
JG at02D5/032F/0392/03F5/0458 proves signedness. Stable repetition values
0000/8000..FFFF producezero calls;0001..7FFF produce1..32767 calls. Setup
and speaker-off still occur when playback is skipped. The global limit is
reloaded each loop test, not cached in a new local.

The old C reassigned the repetition counter from a helper's last argument,
then passed that corrupted counter as the parameter and incremented it. ASM
pushes the parameter globals directly without overwriting BP-2. Those errors
were Reko variable/register merging, not original nonterminating game loops.
Renamed RepetitionIndex locals retain38/46/50 suffixes from the original C.

| Wrapper | Playback helper | Arguments in C/native order |
| --- | --- | --- |
| 02A3 | 047B | WORD3984,E48C |
| 02EB | 059A | WORD006C,3776,4312,398A |
| 0345 | 0643 | WORD0000,39F4,4000,4034,0062 |
| 03A8 | 0747 | WORD398C,39A2,39F6,3FF2,009C |
| 040B | 07DA | same five WORDs as03A8 |

Native stack pushes these right-to-left. Arguments are values, not pointer
dereferences or loop counters. Offset names remain neutral until helper bodies
confirm their role. The header records setup/wrapper declarations and4612's
WORD address view:0002 uses it for command staging and repetition; wrappers
use it as signed repeat limit. It is adjacent to4614 but not a file pointer.
Scratchpad fields still overlap and are not a compiled sequential layout.

Synthetic checks cover all65536 count encodings, representative independent
parameter values, skipped-loop lifecycle and source contracts for every helper
argument list. These do not execute sound hardware or the damaged deeper
helper bodies; fixing the wrappers alone does not certify portable synthesis.

Passed:65,578 wrapper assertions,131,152 dispatcher regression assertions,
and `git diff --check`.

Next:046E–04F0 timer binding, fixed-tone playback and speaker-off, including
WORD delay loops; then noise/sweep algorithms in separate bounded blocks.

## Sol: requested variable naming follow-up

Established roles are renamed consistently in setup, wrappers and the existing
fixed-tone helper without changing their algorithms:

- w4612 -> SoundCommandOrRepeatCount_4612: dual command staging/repetition WORD.
- w3984 -> FixedTonePitDivisor_3984:047B passes its WORD to207F:0030's PIT divisor writer.
- wE48C -> FixedToneDelayMultiplier_E48C:047B multiplies it by5006 in fifty delay passes.

Original names/native addresses remain in header comments. Other noise/sweep
parameter fields stay neutral pending their helper ASM audit: calling them
frequency, duration or milliseconds now would imply unverified semantics.
Existing damaged deeper helper loops were not repaired as part of naming.
