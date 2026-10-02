# Sol: descending/ascending noise parameter sweeps

Scope0747/07DA through final1FC5 return. Evidence: original-version local
BTECH_1FC5.asm; its07DA listing truncates before the final comparison. The
same output set's full BTECH_code_0000.asm confirms CMP39A2,SI then signedJG
for the ascending body. This is not a comparison with another game version.

Both helpers store five WORD parameters: start398C, stop39A2, minimum-bits
subtraction39F6, toggle count3FF2, step009C. These are noise countdown masks
and bit fields, not PIT divisor/frequency parameters. Confirmed globals are
named NoiseSweepStartMask_398C, NoiseSweepStopMask_39A2,
NoiseSweepMinimumBitsSubtract_39F6, NoiseSweepToggleCount_3FF2,
NoiseSweepMaskStep_009C. Old w-names/native offsets remain in header comments;
setup/wrappers and test contracts are updated consistently. Header declarations
document all five WORD arguments. Overlapping scratchpad views remain research.

Independent WORD BP-4 index startszero. Compute low WORD(index*step) withIMUL;
descending current=WORD(start-offset), ascending current=WORD(start+offset).
Descending calls04F1 while signed current>signed stop (0747 CMPstop,current/JL).
Ascending calls04F1 while signed current<signed stop (07DA CMPstop,current/JG).
Equality stops without emitting a call. The old C reversed both comparisons,
tested an undefined current variable before computation and replaced the index
with the toggle-count parameter. All are Reko transcription errors, not native
game bugs. ParameterSweepIndex_78 and CurrentMask_40 retain old numeric suffixes.

Each step calls04F1(current, WORD(current-subtraction), togglecount), then
increments the independent WORD index. Globals reload each iteration.04F1
interprets these as countdown mask, minimum-delay OR bits and toggle count;
its seed remains1000. Neither sweep sets timer mode or switches the speaker
off itself; parent03A8/040B wrappers own that lifecycle. BP-2's native copy of
SI is unused in the emitted instructions and needs no extra behavioral state.

Zero step with a qualifying starting mask can repeat indefinitely; wrapped
products and signed mask crossings govern other unusual inputs. No safety
clamp or invented iteration limit is added. Synthetic wrap/strict-boundary
models and prior helper regressions do not certify audible output, elapsed
time, malformed-input termination or original gameplay.

Passed:1,966,105 sweep assertions,65,620 noise-loop assertions,65,578 wrapper
assertions,131,152 dispatcher assertions, and `git diff --check`.

Next:final1FC5 caller/field consistency and annotation completion review.
