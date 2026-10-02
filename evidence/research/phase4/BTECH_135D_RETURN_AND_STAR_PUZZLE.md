# Sol: map-room return and star selection

Scope135D:079C–0AB5. Evidence: original-version local BTECH_135D.asm.
Door-code routine0AB6 remains separate. Research transcription, not native
gameplay validation; no original-game bug claim from these Reko errors.

079C displays the ladder message and explicitly reads a key. Party world
position becomesC45/C039. Original adapter0 colour conversion remains omitted;
disk2 selection is restored. Fill all576 descriptor bytes withD0, then restore
768 map-tile BYTEs from3092:4614 BEFORE decoding STARLEAG.ICN into that same
buffer. The old pointer-array indexing was a width error. WORD4FBC and D55C
become1 before loading; adapter2 uploads decoded tiles toA400. Tileset becomes2.

08A0 loops64 entries, storing low BYTE(index-70): IDs90..CF in centre grid0664.
The old loop boundC1 and constant -70 were false transcriptions; CHECK ASM is
resolved. Rebuild/project/copy/draw the map, clear CacheMapRoomLoaded D34E,
restore collision threshold WORD0150=21. Exploration steps015A are NOT restored
here; no extra timer/state reset was invented.

0913 maps world coordinates into the block-major map-tile BYTE buffer. X uses
WORD increment then7F mask. Tile offset combines Y region bits*16, local Y
bits*4, X region bits*4, and (X>>1)&7. Native BYTE INC/XOR1/DEC toggles an
odd tile ID to the next even ID (selected), or an even ID to the previous odd
ID; FF wraps to00. Play Sound_MapInteraction. Existing formula is retained
with explicit BYTE wrapping, not replaced with an incorrect XOR-only toggle.

0980 reads seven WORD target offsets from DS3EDB:241E, not246C:241E.
First every required tile must have its low bit clear. If that passes, scan
768 map bytes for even tile IDs97..F0 and reject any whose offset is not in
the target list. This requires all targets and no extra selected stars in
the candidate range, not just seven arbitrary selections. Unsigned BYTE tile
values and native WORD counters/flags are retained. Named constants preserve
the meaning of97/F0 and seven; descriptive locals retain163/175 suffixes.

Membership match's original counter8 then increment/exit is represented by
break, with no remaining membership-loop side effects. Success opens the box,
plays accepted sound, displays accepted message, sets WHITE BYTE D34A=1.
Failure plays incorrect sound/message and decrements every even97..F0 tile,
resetting candidate selections; it does NOT clear a prior WHITE flag. Both
paths explicitly read an ASCII key and set MessageBox_Open WORDD55C=1.
No copyrighted external art/map files were copied into source or tests.

Synthetic checks cover all256 toggle/reset input bytes, all128 required-star
subsets with/without extras,64 descriptor IDs and768-byte restore boundaries.
These are semantic models, not rendering, full map fixtures or gameplay traces.

Passed:1,601 return/puzzle assertions,72,779 transmitter/entry regression
assertions, and `git diff --check`.

Next:135D:0AB6 door lookup, colour-code consumption and replay behavior.
