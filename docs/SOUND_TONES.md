# Sol: original fixed-tone bodies

Converted complete1FC5:047B..04E3 and06F6..0746, checking all expanded ASM
stores, calls and loops. BusyWaitScale retains the EXE-owned initial WORD5
at3EDB:5006. Both methods keep their original shared parameter stores and
low-WORD multiplication followed by signed delay comparisons with live reads.

047B skips the divisor write when it is zero, leaving speaker state untouched,
then runs fifty delay passes.06F6 always writes the divisor (zero encodes65536
PIT ticks) and runs one delay pass. Neither performs timer setup/speaker-off.
The original caller chain still owns those actions.

Tests exercise every65536 divisor, parameter publication, zero asymmetries,
terminating signed/wrapped delay products and parameter mutation during the
speaker boundary call. They do not instrument every unobservable increment
or assert host elapsed time. The SDL integration test checks actual rendered
speaker samples for fixed-zero preserving enabled/disabled state and sweep-zero
enabling output, with scale temporarily zero for that boundary test only.

The loops remain original CPU-speed-dependent loops, not invented millisecond
durations. Modern CPU/compiler optimization can change physical delay and the
SDL audio callback may miss very short states. Emulator timing calibration
and audible comparison therefore remain required before sound preservation
can be declared complete. No generated WAV or toolkit synthesis substitutes
for the original game methods in production.

All104 headless /122 SDL-local suites pass. Executable link has eight unresolved
dependencies, including the two noise parents and their implicit delay-register
hardware contracts. No external assets included/exported.
