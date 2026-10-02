# Sol: original executable sound dispatcher

Converted complete1FC5:0002..02A2 from annotated method and complete expanded
ASM. Retains one-based WORD ID decrement, separator-based scanning, signed
command-base1000 comparisons, original record strides, shared parameter
assignment order and the five original generator entry points. No toolkit
audio renderer or synthetic effect algorithm replaces the game dispatcher.

The313-WORD table3EDB:5008..5279 was read directly from the expanded EXE and
retains all18 effects and delimiters. This is EXE-owned data, not an external
asset. The existing FixedTonePitDivisor is reused rather than duplicated.
Other original shared sound parameters have named WORD globals, assigned
before valid dispatch calls. This does not yet certify arbitrary raw-index
aliases or entry directly into generators with unresolved native state.

Tests select every original effect against independently listed start offsets
and command counts, checking every parameter, wrapper choice and delimiter.
They isolate generator parents: this confirms parsing/dispatch, NOT timing,
waveform or audible playback. Production generators remain unresolved rather
than no-op definitions. The next conversion is the five repetition parents,
then original sweep/noise bodies and their implicit delay-register state.

All101 headless /119 SDL-local suites pass. The real executable now reports
11 unresolved symbols: converting the dispatcher exposes five generator
dependencies in place of one missing dispatcher. This is expected discovery,
not proof of a playable game or complete sound preservation.
