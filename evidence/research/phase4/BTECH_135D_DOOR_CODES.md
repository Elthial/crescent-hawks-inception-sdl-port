# Sol: door lookup, consumption and replay

Scope135D:0AB6 (final routine, including0D44 exit). Original-version local BTECH_135D.asm
is authoritative. This reconstructs the damaged research transcription; it is
not native gameplay validation or a compilable memory-layout declaration.

Negative WORD argument searches twelve coordinate-table BYTE entries247A/2486.
X is WORD(X+1)&7F; Y is WORDY&7E. Matching uses signed CBW Y and BYTE
X parity-cleared equality. First match wins even if codes fail; the reversed
old while condition and comparison of the escaped counter instead of door ID
were transcription errors. Nonnegative argument directly indexes the door tables
and bypasses validation. No new bounds guards or already-opened tests are added.

Required colour IDs come from BYTE tables2492/249E/24AA: CBW then WORD DEC.
Zero therefore means signed-1, not unsigned255. Selected D347/D348/D349 bytes
are also CBW-sign-extended before comparisons. The special entrance ID11 skips
all colour comparisons. Otherwise each mismatch prints its colour's error;
retrace/key/message flag follow, and the door is not changed.

Successful normal doors mark all three REQUIRED codes used at45DE+ID, including
direct replay. Selected slots remain unchanged. There is no check of those used
flags here; terminal offering suppresses used codes in03AA. Entrance11 instead
clears all three selected slots toFF and performs no used-code writes. Every
success writesD34F+door=1. Hence the named latch view has twelve bytes through
D35A, while cache-entry01C1 replays ONLY eleven doors0..10. Earlier eleven-byte
replay documentation remains correct for that caller, not for all writes.

RequiredCode[3] in the corrected C is a readability grouping of three independent
native WORD locals, not a claim that ASM contains that exact array layout.
Named BYTE table views replace undefined ds/w-prefixed aliases. Existing numeric
suffixes are retained on renamed recognizable locals.

## Correction: native playback sentinel is FFFF

Sol: corrected by the fresh whole-method audit on 2026-09-17. Lookup dispatch
uses signed<0; known0800 callers and cache entry passFFFF(-1). The intermediate
`.dis` confirms all three playback comparisons use WORD FFFF, not00FF. The
abbreviated ASM operand concealed sign extension of an immediate BYTE.
Exactly-1 enables sound and three frames; other negative lookup arguments and
nonnegative replay arguments draw final frame2 without sound/delay. The current
C's CacheDoor_AnimatedArgument_00FF is a transcription error queued for correction,
NOT an original-game defect. Earlier synthetic checks endorsed the wrong premise.

Door tile offset preserves native Y-region stride32, unlike star-toggle0913's
stride16. Each frame loads four bytes from246E+frame*4. Start tile column
(X>>1)&7 advances; crossing8 adds56 to the normal increment, moving to the
next64-byte map block. Each frame redraws projected map/units.FFFF playback
waits20 retraces per frame. Old frame+tile indexing and undefined ax counter
were incorrect; the supplied tile values remain EXE data, no external art copies.

Synthetic checks cover all256 encoded required-code bytes, validation/replay/
entrance combinations, all eight starting columns and sentinel distinction.
No original scripts/hardware or out-of-range direct IDs are executed.

Passed:486 door assertions,148,993 terminal regression assertions,11,532
cache-entry regression assertions, and `git diff --check`.

Next: final135D caller/table consistency review, then remaining sound/hardware
segments or cross-project annotation coverage as appropriate.
