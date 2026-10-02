# Sol: original sound repetition and hardware gateway routines

Converted complete1FC5:02A3..046D from the annotated template, checked against
every expanded ASM loop, argument load and exit. Each of the five parents
retains its independent WORD loop. The global repetition count is compared
as signed WORD against the local index, reloaded every iteration. Parameter
globals are likewise reloaded for every generator call; no captured preset
or generic toolkit sound algorithm replaces the original methods.

Original1FC5:046E..047A and04E4..04F0 retain their calls to original207F timer
and speaker-off hardware entry points. The separate platform object binds
these to the existing SDL-backed redirects. Timer setup and speaker-off are
unconditional, including zero/negative repeat counts. No PIT divisor reset,
frequency change, speaker-state restoration or retrigger policy was added.

Focused tests exercise all32768 negative WORD repeat values for every parent,
zero, positive counts1..64 and32767; verify each parameter and call ordering;
and mutate limit/first parameter during the first call to prove live reads.
The real SDL platform test now runs through both1FC5 gateways, checking the
speaker output becomes silent after the original shutdown chain.

All102 headless /120 SDL-local suites pass. The real executable still reports
11 unresolved dependencies, now reaching original tone/noise/sweep bodies
rather than the converted repetition parents. Audible waveform and emulator
timing validation remain pending; tests isolate generator bodies and do not
prove complete playback. No production no-op or synthetic sound substitution
was introduced.
