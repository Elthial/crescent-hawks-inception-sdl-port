# Sol: divisor sweep0643 and delay06F6

Scope1FC5:0643–0746. Evidence: original-version local1FC5 ASM.
0747/07DA remain separate. Research C, not compiled/native audio validation.

0643 stores centre divisor0000, half-span39F4, per-tone delay4000, sweep repeat
count4034 and divisor step0062, all WORD values. Setup046E occurs before the
outer signed count test; final04E4 speaker-off is unconditional. BP-4 counts
sweeps independently; BP-2 counts divisor steps independently. Reko incorrectly
overwrote the outer counter with the delay argument and added a nonexistent ds
argument to06F6. Both errors are removed, not classified as original bugs.

0694 computes low WORD(index*step) with signed IMUL and compares it against
WORD(half-span<<1), signed JLE. Continue only when signed offset<signed limit.
Emit WORD(centre-half-span+offset), then increment the WORD step index.
The start divisor is centre-half-span; the theoretical upper bound is excluded,
subject to native wrap/signed behavior. No host32-bit comparison or unbounded
mathematical multiplication substitutes for AX. Globals reload as in ASM.
Zero step with positive signed limit can loop indefinitely; wrapped products
can also change termination. No new guards were added to original semantics.

06F6 stores current divisor3FF6 and delay3246, always calls207F:0030 (including
divisorzero, encoding65536 PIT ticks), then performs ONE busy wait. Unlike047B,
there are no fifty outer passes and no zero-divisor skip. Delay iteration limit
is signed low WORD(delay*calibration5006), recomputed each test; high-bit orzero
products skip the loop. It does not itself set timer mode or silence the speaker.

Seven confirmed global fields are renamed consistently in setup/wrapper/helper
code: SweepCentrePitDivisor_0000, SweepHalfSpan_39F4,
SweepToneDelayMultiplier_4000, SweepRepeatCount_4034, SweepDivisorStep_0062,
SweepCurrentPitDivisor_3FF6, SweepCurrentDelayMultiplier_3246. Old names and
native offsets remain in header comments.3092:0000 is not3EDB:0000. Header
views remain overlapping notes, not a sequential compiled struct. Locals retain
104/102/51/44 suffixes; arguments retain wArg identifiers.

Synthetic checks cover every WORD step index against six step values, wrapped
divisor arithmetic and source contracts; fixed-tone arithmetic and wrapper/
dispatcher regressions are rerun. These do not certify audible output, exact
timing, termination for malformed parameters or original hardware behavior.

Passed:393,367 sweep assertions,1,179,904 fixed-tone assertions,65,578 wrapper
assertions,131,152 dispatcher assertions, and `git diff --check`.

Next:0747/07DA descending/ascending parameter sweeps, then final1FC5 consistency.
