# Sol: original divisor and noise-mask sweeps

Converted original1FC5:0643..06F5,0747..086C using annotated methods and full
expanded ASM. The truncated ascending ASM tail is supplied by the original
.dis: signed stop greater than current repeats, otherwise restores SI/BP
and returns. No alternative audio renderer enters these original routines.

Retains separate WORD counters, low-WORD products, doubled-half-span wrapping,
strict signed stop comparisons, wrapped minimum-bit subtraction, and live
shared parameters. Only divisor-sweep0643 owns timer setup/speaker shutdown;
noise-mask sweeps leave them to the surrounding repetition parent. Native
continuing zero-step cases remain nonterminating, not silently clamped.

Tests cover positive direction/step/length grids, equality, signed ordering
across8000, negative ranges, shared-state mutation, centre-divisor underflow,
signed-negative doubled spans, negative repeat counts and skipped zero spans.
A descending800A..8000 step3 case emits43694 calls: skipping the minimum wraps
to positive and continues until the WORD product eventually reaches the stop.
This specifically rejects invented monotonic stopping/iteration caps.

These tests call the real three parents but isolate low-level generators:
waveform, elapsed delay and audible effect playback are not yet confirmed.
All103 headless /121 SDL-local suites pass; executable linkage has10 missing
dependencies, including four remaining tone/noise bodies. No assets exported.
