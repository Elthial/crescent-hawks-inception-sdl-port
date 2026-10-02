# Sol: noise/gate loops04F1 and059A

Scope1FC5:04F1–0642. Evidence: original-version local1FC5 ASM plus already
reviewed207F:0067/007D/00A9 bindings. No0643/0747/07DA algorithm changes;
their confirmed shared-field references are renamed consistently only.

Both routines store WORD mask006C, minimum-delay bits3776 and toggle count4312.
04F1 sets delay seed398A to1000;059A accepts it as a fourth WORD argument.
1000 here is007D's delay-state seed, not Hz, milliseconds or a stream command.
Names now describe these verified roles, with old names/native offsets retained
in header comments. Argument identity wArg04/06/08/0A and inherited059A wArg94
is preserved; local numeric suffixes remain recognizable.

Both clear port61 bits0/1 via0067, then busy-wait with an independent WORD
counter starting1 while signed counter<signed calibration5006. For stable
positive calibration C, warmup is max(C-1,0) increments. High-bit calibration
values are negative and skip this warmup.

Outer WORD counter startszero and increments once per bit1 toggle, while signed
4312>counter. Every toggle passes398A to007D; this writes no PIT divisor.
007D computes wrapped(seed+9248), rotates right3 and leaves evolved delay state
in DX. Inner independent WORD counter startszero and calls00A9 calibration
times for positive calibration; nonpositive calibration skips only these calls,
not a qualifying outer toggle. Global limits/arguments reload as in ASM.

00A9 receives006C/3776 as values and uses implicit DX: countdown=(DX&mask)|bits.
Its LOOP countzero means65536 iterations. The CPU-register research binding
remains explicit in207F; it is not falsely converted into a C return value or
new argument here. A portable synth must preserve that dependency intentionally.

Reko had merged the independent counters with seed/mask/parameter registers,
overwriting them and assigning next counter values from parameters. The restored
C has separate counted loops, signed WORD tests and descriptive names. No
“RandomNumber” role exists in the initial059A warmup. Neither helper performs
timer setup or final speaker-off itself; parent wrappers own that lifecycle.

Synthetic tests cover every calibration WORD encoding, signed warmup/inner-loop
counts, representative seed/mask/minimum states, and both source argument/counter
contracts. They do not execute original audio/hardware or certify elapsed time.
Wrapper and stream-dispatch regressions are rerun. Reko errors are not original
game bugs; no expensive model confirmation was required for direct ASM evidence.

Passed:65,620 noise-loop assertions,65,578 wrapper assertions,131,152
dispatcher assertions, and `git diff --check`.

Next:0643/06F6 divisor sweep and its low-WORD arithmetic/delay, then0747/07DA
descending/ascending parameter sweeps and final1FC5 consistency.
