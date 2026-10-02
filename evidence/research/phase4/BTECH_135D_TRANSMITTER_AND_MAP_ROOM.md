# Sol: transmitter gating and map-room entry

Scope135D:04AB–079B. Evidence: original-version local BTECH_135D.asm.
No puzzle-solving or return-path changes in this block; research C remains
an annotated transcription, not a compiled port.

## Transmitter04AB

WORD X+1 wraps, then X/Y are masked with7E. Only local(4,4) or(2,0E)
continues. Ordered tests: power BYTE D34C must be nonzero, WHITE BYTE D34A
must equal exactly1, parts-found BYTE D34D must be nonzero. The last flag
is parts discovery, not the WHITE-code puzzle result. Existing gates match ASM.

Failure selects power-off, incorrect-WHITE or missing-parts dialogue, displays
the FAR string through17EA, explicitly reads an ASCII key, sets MessageBox_Open
WORDD55C=1. The invalid assignable text array is now a FAR pointer. Success
runs WINSCENE.BLD(ID19h), then sets MessageBox_Open and transmitted-cache
WORD01A8=1, without the failure-path message/key handling. No prior transmitted
guard is present. Script behavior itself is not reinterpreted in this block.

## Map-room055A

Local(7C,4) runs FINDIT.BLD(ID18h), sets parts-found BYTE D34D=1 and message
WORDD55C=1. There is no prior parts-found guard. The hole test is independent:
local(48,38) opens a box and sets D55C=1. WHITE exactly1 only displays the
already-have-code message/key; any other BYTE value starts the descent.

Descent displays two messages with explicit key reads, sets exploration steps
WORD015A=1, party world position C04/C022, selects disk2 and fills all576
descriptor BYTEs0564..07A3 withD1. Original adapter0 conversion2FE8:01D0
remains omitted. Compatibility WORD4FBC becomes1 before the external asset load.

MAP.ICN loads compressed into246C:244B and decodes into3092:4614. Adapter2
uploads tiles toA400. Only AFTER that upload,06BF overwrites the first768
BYTEs of3092:4614 with the current cache tiles from246C:101D. This is an
intentional reusable-buffer backup, not indexing a DWORD/pointer array.
The corrected byte view preserves that crucial order; external assets stay local.

Tileset becomes3, MAP15.MTP is read into the map-tile buffer with requested
length768, and CacheMapRoomLoaded BYTE D34E=1. Twelve centre-grid descriptor
positions are patched: rows0..2, columns0..3 at pitch8 receive90..9B. Native
descriptor ID and row/column counters are WORDs; stores are BYTEs. Descriptive
locals retain319/320/326 suffixes. Remaining centre-grid bytes remainD1.

The map cache is rebuilt/projected/copied and drawn; collision-threshold WORD
0150 becomes8B. Original adapter0 display-mode handling remains omitted. Entry
does not award the WHITE code or restore the outer cache. These belong to later
bounded routines. Existing named coordinate constants and thresholds are retained.

## Verification / next

Synthetic checks cover gate ordering/exact-one semantics with representative
nonzero flag values, all WORD X mask inputs,768-byte backup and its boundary,
twelve descriptor patches and untouched cells, plus source workflow ordering.
They do not run original scripts, file reads, rendering or gameplay. Reko-only
errors are not classified as original-game bugs.

Passed:72,779 transmitter/entry assertions,11,532 cache-setup regression
assertions, and `git diff --check`.

Next:135D:079C cache/map-room return, followed by08xx map-room puzzle handling.
