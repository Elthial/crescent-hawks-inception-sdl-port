# Sol: original noise loops and register handoff

Converted complete1FC5:04F1..0642 from annotated methods and complete expanded
ASM. Retains shared WORD parameters, fixed seed1000 for04F1 versus caller seed
for059A, speaker-off preparation, signed warmup starting1, signed toggle count
and signed scale count. Neither method disables output on exit; the original
parent does. No waveform toolkit renderer or synthetic noise replaces these.

Native207F:007D returns evolved delay state in DX.207F:00A9 uses DX and does
not modify it. Host C expresses that register result as a WORD return and
an explicit WORD input, held unchanged through each countdown group. This
avoids an undefined Reko register/global or stateless helper that loses it.
The two hardware/delay helper implementations remain pending, not no-ops.

Tests isolate those boundaries and cover both original parents, scale/count
grids, every negative toggle count, maximum positive toggles with zero scale,
negative scales and exact shared parameter publication. Every countdown must
receive the returned state unchanged. This is loop/control-flow evidence, not
gate waveform, physical timing or audible playback confirmation.

All105 headless /123 SDL-local suites pass. Real executable linkage still has
eight unresolved symbols, now including207F toggle/countdown gateways in place
of the converted noise parents. External assets are neither embedded nor exported.
