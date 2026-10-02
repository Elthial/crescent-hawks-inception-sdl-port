# Sol: whole 1F3D segment annotation review

Sol: Fresh systematic confirmation is in [the1F3D audit](BTECH_1F3D_SYSTEMATIC_ASM_AUDIT.md).
The earlier loader model used a correct SS-local destination but did not prove
the C's ordinary local address retained SS.063B is now marked mismatched for
that binding;05BC's unassigned native result remains unresolved, not fully matched.
Historical reconstruction/check counts below are not fresh confirmation labels.

Scope: all seventeen original routine entries `0006–086A`. Evidence:
`Btech-Reko-expanded/BTECH.reko/BTECH_1F3D.asm`, its full-listing counterpart
`BTECH_code_0000.asm`, shared `BTECH_207F.asm`, and the matching expanded
`BTECH.EXE`. Only EGA bodies are maintained. This is research pseudo-C, not
a compilable or playable port. New/reconstructed comments are labelled Sol.

## Inventory and changes

| Entry | Purpose and native contract | Review result |
| --- | --- | --- |
| 0006 | Wait N retrace edges, WORD argument | Corrected inverted loop and BYTE truncation |
| 002F | Pending key OR active attractor replay; AX0/1 | Corrected replay polarity and invalid member syntax |
| 0053 | Unsigned WORD decimal into FAR3092:0012, display via1E56:03F5 | Corrected scratch alias to declared field |
| 0086 | Draw staged image rectangle | Existing A800-to-A000 EGA body confirmed; buffer argument ignored |
| 00D5 | Direct coloured text, FAR string plus four WORDs | Reconstructed absent EGA helper used by existing callers |
| 01FB | Inclusive filled rectangle | Reconstructed absent helper under existing legacy name |
| 0259 | Live keyboard, recording, or attractor replay | Existing annotated flow cross-checked; unchanged |
| 031C | Signed sort/clamp; vertical/horizontal lines only | Reconstructed absent helper used by existing callers |
| 03EB | Horizontal span: edge pixels, aligned groups, tail | Reconstructed absent EGA helper and native table views |
| 049D | BYTE marker then FAR encoded payload | Corrected DWORD marker/pointer extrapolation and stride |
| 0525 | Sixteen BYTE EGA palette entries | Existing signed conversion/retrace body confirmed |
| 05BC | DWORD request wrapper over WORD runtime allocator | Reconstructed missing research body; error-path uncertainty flagged |
| 063B | Length-prefixed external-file loader | Corrected stack-local prefix, payload count, disk retry, AX1 result |
| 06C3 | AC00-to-A000 map viewport transfer | Existing FAR buffers and216x200 viewport confirmed |
| 070A | Startup sprite snapshot allocation and39FA FAR table staging | Reconstructed absent EGA helper |
| 0814 | Raw fixed-count external-file loader | Corrected FAR types and missing failed-open disk retry |
| 086A | Fifty retraces, then drain keys | Corrected misleading50Hz comment/constant name |

Six helpers were absent from the maintained file, despite existing callers
or adjacent contracts. Their reconstruction contains only EGA-relevant logic;
it does not restore the deleted CGA/Tandy/other adapter implementations.
New207F primitive names are explicitly labelled ASM-backed **bindings**,
not newly invented host implementations. Decoder implementations22F8/2368
remain separate bindings too; toolkit asset decoders already serve extraction.

## Retrace and input

At0023, MOV saves the old count in AX, DEC modifies the stack WORD, and JNZ
tests the saved AX. Thus zero waits zero times,256 waits256 times, andFFFF
waits65535 times. A final failed test leaves the private argument FFFF.
The old equality-zero loop was a Reko transcription fault, not a game bug.
`086A` requests50 edges, not a50Hz mode or a guaranteed one-second pause.
The low-level0B40 edge choice consumes only the low BYTE of32AC (MOV BL),
despite the caller pushing the WORD field, and depends on hardware timing.
The full listing confirms086A's final RETF, truncated from the segment listing.

`002F` calls the real-key probe first. If no real key exists, nonzero3938
still reports pending input so the replay can advance. The legacy global
`DisableInput` means replay mode here; it is retained across callers rather
than renaming unrelated blocks in this review.458C enables recording.
0259 normalises recorded lowercase h to H; H replay tokens wait30 retraces
and fetch another byte. Replay bytes use CBW signed extension; a live key
interrupt sets0152 and is consumed. Each replay result waits one retrace;
P also sets0152. No new DEMOFILE instruction semantics are invented.

## Text and geometric helpers

00D5 sets foreground/background through2127, masks glyph bytes with7F,
and uses eight-byte glyphs through2251. Its row stride320 means eight EGA
scanlines times40 bytes, not one320-byte pixel row. CR resets the column and
advances the row without incrementing4FBE. Automatic column overflow uses
the signed comparison against WORD(4FB8-1), increments4FBE, and advances
the row. FAR string offset increments wrap without segment carry.

01FB does not sort or clamp: reversed Y bounds draw nothing; otherwise each
row goes through03EB.031C separately sorts signed WORD endpoints, then clamps
each to inclusive X0..319/Y0..199. Vertical takes priority for a single point;
horizontal is next; other diagonals draw nothing. This endpoint clamping is
not full geometric clipping: an entirely offscreen line can collapse to a
border point. Existing gauge callers deliberately rely on reversed Y rejection.

03EB aligns initial pixels using4FC4, shifts the WORD delta with SAR using
the low BYTE at4FD4, calls0780 for groups, then ANDs final X with4FCC and
emits inclusive tail pixels. The matching expanded EXE confirms these tables:

| Adapter index | Initial alignment mask4FC4 | Tail alignment mask4FCC | Group shift4FD4 |
| ---: | ---: | ---: | ---: |
| 0 | 0003 | 01FC | 2 |
| 1 | 0001 | 01FE | 1 |
| 2 (maintained EGA) | 0007 | 01F8 | 3 |
| 3 | 0000 | 01FF | Not read: native branch skips SAR |

The shift table has **three** WORDs, not four:4FDA begins the error string
`Alloc too big!`.4FE9 begins `Alloc: Null pointer return!`. These EXE strings
are named address views in the header, not copied external assets. The old
maximum-line-length interpretation of4FD4 was incorrect.
To reproduce these EXE reads: file offset = MZ header paragraphs*16 +
WORD(segment-0800h)*16 + native offset. Read little-endian WORDs for the
tables and NUL-terminated BYTEs for the two strings; no external files needed.

## File and decompression workflow

063B opens with the runtime's8000 binary/read-only flag through33D0. OnFFFF
it prompts for the selected disk at3EDB:014E and retries. On success,3580
first reads exactly two bytes into FAR SS:BP-2; the resulting little-endian
WORD is the next read's byte count into the caller's FAR destination.3336
closes the file;06BC returns AX1. Read/close results are ignored. A short
prefix read can leave part of the local undefined; AX1 is not integrity proof.

0814 uses the same failed-open prompt/retry but reads the caller-specified
count directly. No prefix is stripped. Its INC AX/JZ recognises handleFFFF
exactly. No explicit success return is asserted; read/close status is ignored.
Both filename and destination are16:16 FAR addresses, not DWORD scalar values
or pointers to pointers. Existing `&seg...field` calls are native address views
of buffers; their old pointer-shaped scratch declarations do not imply that
these loaders dereference a destination pointer stored there.

049D tests one BYTE marker, increments only its source offset WORD, and passes
the remaining stream to22F8 for marker1, otherwise2368. It does not test a
DWORD and skip four bytes, nor validate that the alternate marker is2. Its
adapter0 post-conversion is intentionally absent from this EGA-only body.

## Allocation and sprite capture

05BC receives a genuine32-bit requested size; DWORD here is **not** a Reko
pointer-width accident. A signed positive high WORD prints the oversize message
and waits for input. Negative high WORDs and high WORD zero instead pass only
the low WORD to3835. The positive-high-WORD path never assigns BP-6/BP-4 before
testing and returning those words. This apparent native uninitialised-result
path is preserved and queued as optional Astra/emulator confirmation, not
silently fixed or yet classified as a confirmed original-game bug.

070A computes low WORD(width*height), multiplies its AX by4, retains the low
WORD again, adds4 with WORD wrap, and passes that size with high WORD zero.
It stages the returned16:16 pointer at3092:39FA+SpriteId*4 before the null check.
Header BYTE1=height-1 and BYTE2=width; bytes0/3 are untouched here. The EGA
capture reads interleaved planar data at246C:244B+X*4+Y*160, with width*2 WORDs per row,
and source gap WORD(160-width*4), into allocation+4. The second buffer copy
at07D7 belongs to removed adapter0 and is not restored.
X/width describe eight-pixel cells (four plane bytes each); Y/height are
scanlines.160 bytes is one320-pixel interleaved row, not a40-byte single-plane
stride. Sol:startup03F5 converts MECHSHAP in place through207F:0572 BEFORE
capture; this supersedes this review's initial packed4bpp interpretation.

0931 narrows width/height to DL/DH; its decrement-after-copy row loop means
height BYTE zero runs256 rows. Caller allocation and source-gap arithmetic
still use WORD inputs, so malformed dimensions must not be treated as a safe
validated image API. Startup-valid sizes are the intended verification scope.
FAR offset wrap is explicit; no segment carry or safe bounds checks are added.

## Verification and remaining port work

`Verify-1F3DTranscriptions.ps1` exercises synthetic loader traces, marker dispatch,
FAR arithmetic, retrace counts, clipping, span coverage, text advancement,
palette signed conversion, allocation classification and snapshot arithmetic;
it also checks source/header contracts. Existing input, hardware, menu drawing,
text and tileset models provide regression checks.

These models do not execute the original EXE or annotated C. Hardware timing,
allocator heap mechanics3835, low-level EGA primitive bodies and real gameplay
remain separate port/trace work. No external copyrighted file is copied or
staged. Optional expensive confirmation is documented, not invoked.

Follow-up bounded work: reconcile0800's legacy MechSnapMemory flat-WORD
readers with the now-explicit39FA FAR table writer; reconstruct their native
header-byte adjustments and207F:0377 draw binding. Do not treat the maintained
0800 placeholders as a working implementation just because the writer is clear.

Passed379,857 new transcription/source checks and258,376 assertions across
nine existing input/hardware/text/tileset/menu/border regression scripts.
`git diff --check` passed. This count is not a gameplay coverage percentage.
