# BTECH_1E56 menu and text workflow review

Sol: Correction2026-09-17: [the systematic audit](BTECH_1E56_SYSTEMATIC_ASM_AUDIT.md)
supersedes the converter's earlier brace mapping. Native007B passes unchanged,
007C maps west and007E maps north; the latter table WORD was truncated in ASM.
The former claim below of correct complete lowercase mappings is not confirmation.

Sol: Research pseudo-C, not executable host C. Cross-reference is the clean
expanded-EXE ASM. Source comments retain human annotations and distinguish
WORD offsets from false Reko pointer arithmetic. No original assets are staged.

## Reviewed boundaries

0004..0280 border parent/edge sequences were reconstructed in the preceding
207F caller pass; see BTECH_207F_SEGMENT_REVIEW.md. This next bounded change
covers0281..0387 menu-context switching and0388..03F4 interior clearing only.
03F5 text output, scrolling/page handling and choice input remain pending.

## Menu context records

305B records have stride10h /16 bytes, all fields WORD-sized:

| Offset | Meaning / active destination |
| --- | --- |
| +00 | Interior X byte-column origin,3092:39A0 |
| +02 | Interior Y8-pixel cell origin,3092:39A4 |
| +04 | Interior width in cells,3092:3990 |
| +06 | Interior height in cells,3092:393A |
| +08 | Text foreground colour,3092:37FE |
| +0A | Text/interior background colour,3092:377E |
| +0C | Horizontal text cursor offset,3092:3748 |
| +0E | Vertical text cursor offset,3092:374E |

0281 tests incoming DS:4FA2 initialization WORD. First call sets it to1 and
skips saving outgoing context. Later calls save ONLY the previous record's
four mutable text-state WORDs, using active index3092:4600, before selecting
the requested record and restoring all eight fields. Same-index selection
therefore retains current cursor/colours. This is not a stack push/pop of menus.
The original has no index validation. Native SHL4 and address addition wrap
as WORD; local raw accessors preserve that rather than relying on the incomplete
scratchpad struct's physical layout. Older MenuUnknown BYTE views are retained
for unreviewed callers, not asserted as correct record definitions.

The new377E background-colour header view overlaps legacy w377E/t377E notes;
it does not add storage or imply the scratchpad header is a packed struct.

## Interior clearing

0388 calls1F3D:01FB with the whole current interior rectangle and377E colour.
Pixel origins are cell origins*8; inclusive ends are(origin+extent)*8-1.
WORD wrap is explicit. It then zeros BOTH cursor offsets. Despite the existing
Draw_Top_Graphic_Sidebar name this is not limited to a top sidebar; the name
is preserved for caller continuity. No zero-extent guard or clipping was added.

## Verification / limitations

Verify-MenuLayoutTranscriptions.ps1 passes104 synthetic assertions covering
first selection, repeated selection, outgoing geometry preservation, high-byte
WORD values, cursor/colour restoration and inclusive rectangle arithmetic.
The state model shares basic indexing with the transcription; it is not an
independent ASM execution oracle. Border8848, menu-drawing229105 and text6443
regressions pass; whitespace validation passes. Live layout switching, actual
record inventory and draw-call behaviour remain necessary before the port.

Next:03F5 text dispatch in bounded logical blocks, then scrolling/page state
and menu input. Do not infer complete menu semantics from the old BYTE arrays.

## Sol:03F5 opening dispatch and wrapping trace (partial reconstruction)

Input is FAR text, fetched through LES.0752 stores AL into BP-04; the former
undefined al_38 assignment was corrected to the fetched text byte.042E strips
bit7 and sets BP-02 stop state. The marked byte is processed once, then the next
loop entry stops and0768 flushes pending text. The duplicate mask which undid
the strip was removed. NUL also enters final flush. No whole-function correctness
claim is made: this pass fixes the input/marker dispatch and adds bounded notes.

Stack frame size62h is98 bytes. BP-5E starts a pending token buffer, BP-32 the
accumulated line, BP-60 pending line width, BP-08 token count and BP-06 input
index. Existing fp-96/fp-52 expressions are bad Reko stack extrapolations, NOT
file pointers. Exact local buffer models and07CB's FAR text+WORD advance flag
still need reconstruction together; old calls which pass ss as a flag are wrong.

Confirmed dispatch: CR flushes with line advance;02/06 flush without advance
and consume next BYTE as background/foreground colour;09 sets absolute cursor
column from next BYTE;13 pads toward a next-BYTE target. Spaces delimit words.
Space overflow flushes the accumulated line, copies the pending word to the next
line, and skips consecutive spaces. Long-word handling at070C checks token count
against width-1, flushes a nonempty line, then appends/flushes the token. Signed
WORD comparisons and byte buffers matter; no generic host word-wrap was inserted.

Scrolling/page transitions live in07CB, not in the opening03F5 dispatch itself.
Next pass must replace damaged buffer/flush control flow, then review07CB.

### Sol: Named locals and ASM-confirmed stack buffers

Follow-up replaces bLoc06_556 / wLoc08_550 / wLoc04_548 / wLoc0A_562 with
TextControlByte_556 / TextReadIndex_550 / StopAfterMarkedByte_548 /
PendingTokenLength_562. Numeric suffixes remain traceable to Reko annotations.
RawTextByte replaces the misleading file-open name.

043D and subsequent token stores use BP+index-5E: the NUL store is now
PendingTokenBuffer[PendingTokenLength_562]=0. The old fp-96 was wrong by two
bytes: BP-60 is pending-width scalar. The accumulated line starts BP-32 (50
bytes below BP), not fp-52. Both buffer bases and all related stores/appends
are now explicit arrays, with pointer-valued flush temporaries corrected to
FAR byte pointers. The malformed final terminator expression is corrected.

44-byte token and40-byte line research regions are inferred from adjacent
stack addresses, not a recovered source declaration. Only the line's first
BYTE is initialized, matching native code; no bounds checks were invented.
Damaged flush selection/gotos and07CB calls remain pending: passing ss as an
advance flag is still wrong and must be reconstructed against branch-specific
ASM. These buffer/name fixes do NOT certify the complete text routine.

### Sol: ASM-confirmed branch temporaries, advance flags and stack correction

066C is now Append_Pending_Token_To_Line_066C, retaining its source address.
The other local labels identify pending-line reset, append/advanced flush and
advanced flush. Pointer temporaries now identify background/foreground-change
text or advanced-line text, not AX registers. Cursor/padding targets, pending
end column, next read index and pending line width have semantic names while
their original numeric suffixes remain.377E uses the background-colour alias.

04B5 sets the flush pointer to the accumulated line on the fits branch; the
previous transcription left it uninitialized. Overflow flushes that line with
advance1 then flushes token with advance0. Colour/cursor changes and final
partial flush use0; CR and forced wrap use1. SS pushes belong to the FAR text
argument, NOT the advance flag.07CB's text argument is now a FAR byte pointer.
Its internal scrolling/page body remains a separate pending review.

071A tests line[0]!=0 before forced-wrap flush;076E tests nonempty accumulated
line before final flush. Both replaced false placeholders.0730 increments token
count before append/advance. Control operands use CBW sign extension, including
colour/cursor/padding bytes; comparisons follow signed WORD JG/JGE/JL semantics.

Correction to the preceding inferred buffer footprint:0631 stores the padding
target WORD at BP-34, so the token region BP-5E..-35 is42 bytes, NOT44.
The source array is corrected to42. This was revealed by the padding branch;
the earlier44-byte estimate did not account for its local. No bounds checks or
new empty-buffer clearing were invented. Full workflow/emulator validation is
still needed; synthetic helper regressions do not execute this pseudo-C.

### Sol: CHECK ASM audit in the current text block

All nine markers in1E56 were cross-checked. Eight03F5 append/flush sites now
carry verified source-address comments:0534,05B6,05D0,05E6,066C,0455,078C,07A6.
Append direction is line destination / token source, with overflow advance1
and partial flush advance0 as documented above. These site checks are not a
whole-function correctness claim.

The remaining07CB padding marker exposed real errors:07ED takes strlen(text),
0803 writes spaces through FAR text pointer while signed length<width, then081E
writes NUL at text[width]. The old code started from border height, reversed
the comparison and indexed the advance flag. Those expressions are corrected.

Correction to earlier scroll-source notes:0852 EGA enters0870 with source offset
0140h, then08EE pushes it with A000 segment.0140h=8*40 bytes.0A00h comes from
adapter3's distinct08B7 path and must not be used by the EGA-only caller. The
named define now points to A000:0140. The earlier A000:0A00 annotation was our
branch-selection mistake, not a Reko or original-game defect.

Repository-wide twelve CHECK ASM markers remain in135D,1431,1CD3 and1F3D.
They are preserved pending bounded ASM review, particularly1CD3 stock and
personnel expressions. This audit resolves1E56 markers only; do not treat the
remaining markers as cosmetic comments or remove them without evidence.

## Sol:07CB..0A3A text consumption and scrolling

Reconstructed retained EGA body with FAR mutable text and WORD AdvanceLine_08.
There is no keyboard page pause here. Within the interior, text is drawn at
origin+cursor; without advance,09F5 adds strlen to cursorX (not absolute drawX).
At/beyond the bottom, the original text length is saved before padding/truncation
to width. EGA copies the interior up eight pixels from A000:0140, clears the
bottom strip with colour0, then draws padded text on the last visible row.
Without advance,098C adds ORIGINAL text length and sets cursorY=height-1.
With advance,0985 bypasses that Y reset and increments incoming cursorY.
Both paths reset X/increment Y when advance is requested or signed X>=width.
Every exit writes NUL at text[0], consuming the buffer for later reuse.

08F6's strip clear right endpoint is(originX+width)*8-8, not conventional last
pixel minus1. Exact native arguments are preserved;1F3D:01FB semantics remain
a separate review. No clipping or zero-height validation was added. Deleted
adapter branches remain absent. Scroll padding offsets wrap as WORD.

Verify-TextBoxTranscriptions.ps1 passes591 synthetic state/padding assertions.
Text6443 and menu-layout104 regressions pass, as does whitespace validation.
These checks do not execute annotated C or prove original hardware rendering.
Next bounded block:0A3B/0AE5 tile capture/tileset construction, then0B95 choices.

## Sol:0A3B..0B5D tileset capture/construction

0A3B receives FAR source and FAR destination plus X byte-column/Y tile-row.
Retained EGA0ACD ignores source argument and calls207F:0313, which reads A800
and emits32 row-interleaved plane bytes. Other adapter capture bodies remain
absent. Destination is a FAR byte buffer, not arbitrary size interpreted as pointer.

0AE5 allocates through1F3D:05BC, stores original FAR result, and captures tiles
into a moving destination offset. The former annotated body omitted allocation
and return, used tileCount*32 as a destination address and narrowed count to BYTE.
Count/loop are signed WORD compared by JL; native allocation size SHL5 wraps
as WORD then CWD sign-extends to DWORD. Destination offset advances32 without
segment carry. SourceX increments then signed JLE39 controls wrapping to X0/Y+1.
0B54 returns original allocation, not moving cursor.

Allocator is an explicitly declared research binding, not host malloc or a
new allocator implementation. Its native failure policy remains a separate
1F3D review. Startup0D27:0288 requests8 tiles and stores result4066/4068 border
base;030A requests66 tiles and stores4588/458A tiny-map base. Valid requests
are256 and2112 bytes. No new allocation/bounds checks or copyrighted assets.

Verify-TileSetConstructionTranscriptions.ps1 passes3155 synthetic address/size
assertions, including column wrap, destination offset wrap and startup sizes.
Models partly share arithmetic and do not exercise real allocator/rendering.
EGA1646 and border8848 regressions plus whitespace validation pass.
Next:0B5E menu choices and selection/input loop (not0B95 entry).

## Sol:0B5E..0D1C selection/input loop

WORD selector and WORD AX result, not prior BYTE signature.305B WORD fields
at index*16+92(row offset),94(highlight width),96(option count),98(saved index),
9C(colour) replace invalid BYTE-array/address arithmetic. Saved index is reset
when signed index>=count or3938/458C input flags are nonzero. These flags reset
selection; they do NOT skip the input loop. Initial index is also saved locally.

Initial highlight is drawn then pending input drained. Converted WORD key is
read until Enter/Space.FFB8 decrements,FFB0 increments; old highlight is erased
by XOR, signed wrap applies, new highlight is drawn. Other keys are ignored.
Confirmed selection is saved at+98. Post-loop ESC branch restores original
local index, not+9C colour. It is unreachable by ordinary local loop control,
which never exits on ESC. No invented cancellation/highlight erase was added.

Sol: corrected native interpretation:0C40/0C46 compareFFB8/FFB0, matching
the converter. EXE bytes83 7E F8 B8/B0 use opcode83's sign-extended immediate;
the generated ASM printout lost this extension, while .dis retains it. Local
EXE bytes8B46065DCB at0E71 confirm full-WORD return, not truncation. Letter
and keypad navigation therefore matches natively. The earlier claimed menu
mismatch was a transcription error, not an original-game bug.

Valid lists assumed; zero/negative count behaviour was not guarded. Record
helpers provide WORD offset wrap; whole menu-record allocation remains under
review. Width/colour/count are cached locally here because this synchronous
loop has no observed record-mutating calls; unexpected external mutation would
require native reload semantics. No expensive model or game assets used.

Verify-MenuSelectionTranscriptions.ps1 covers wrap in both directions for32
option counts,ignoredFFxx/ESC commands and confirmation keys. Synthetic models
do not execute input adapters or prove gameplay. Next:0D1D keyboard conversion
and1F3D:0259 input bridge, to resolve this command-width discrepancy.

## Sol:0D1D conversion and1F3D:0259 input bridge

Converter rebuilt as explicit mappings from clean ASM comparisons and lowercase
CS:0E35 jump table. A/a/4/{ west; C/c/3 southeast; D/d/6 east; E/e/9 northeast;
Q/q/7/backslash northwest; W/w/8 north; X/x/2/backtick south; Z/z/1 southwest.
Native0C andFF0C also map east. Other keys, includingESC, pass unchanged.
No incorrect nested unsigned comparisons or out-of-range lowercase switch remain.
Direction constants retain FFxx WORD values; obsolete KEY_W is not reintroduced.

0259:3938 zero selects live BIOS input, nonzero replay (old C reversed this).
458C nonzero records live input low BYTE at3092:00A0+replayIndex, incrementing
WORD index. Recording lowercase h becomes H; H is recorded but read loop
continues. Replay CBW sign-extends stored BYTE. H waits30 retraces then reads
another byte. After non-H, pending real input sets exit0152 and is consumed;
one retrace follows. Replay P also sets exit. Initial replay index2710 gives
first byte27B0, not an untyped pointer index. Bridge returns original signed
WORD local in AX, not a truncated BYTE.

Menu navigation accepts the converter's FFB8/FFB0 values, including those
produced by high-byte replay tokens. The former positive-token interpretation
was corrected against EXE opcode bytes. ESC remains ignored by the menu local
loop, not remapped to west.

Verify-InputBridgeTranscriptions.ps1 checks source lowercase mappings against
transcribed jump targets, unchanged menu tokens, all BYTE sign extensions and
delay-marker consumption. Synthetic models are not BIOS/replay execution.
Hardware6659 and menu-selection3175 regressions pass. No game assets modified.

## Sol: Retained1E56 consistency checkpoint

All retained logical function groups now have a first ASM-based review, including
border descriptors, context switching, interior fill, text parsing/scrolling,
tileset capture, choices and key conversion. Missing cross-segment text/input
contracts are recorded in BTECH.h, and startup4588/458A has a FAR byte-base
TINYLAND_TileSet overlapping view. The notes are linked from docs/README.md.

03F5 stream reads now use Menu_Text_Read_Byte: native LES+WORD ADD preserves
segment and wraps offset, rather than relying on ambiguous FAR-pointer arithmetic
from host C. This applies to ordinary bytes, signed control operands and skipped
spaces. It is a local research helper, not a new original ABI or safe-string API.

Input296, text6443, text-box591, menu-layout104, menu-selection3175, tileset3155,
border8848, menu-drawing229105 and hardware6659 helper regressions pass, along
with whitespace validation. Scripts mostly exercise synthetic models rather
than the annotated function bodies; this is NOT gameplay/runtime validation.

Remaining risks: original00xx/FFxx menu discrepancy and unreachable ESC branch;
no runtime menu traces; bounded buffers and malformed-data behaviour; exact
rectangle helper endpoint semantics; absent allocator/error policy; complete
menu-table inventory and external caller record indexing. The text routine's
labelled control flow is still research notation, not cleaned executable C#.
No claim that all original exports or port work are complete.

Sol:Follow-up1F3D whole-segment review reconstructs05BC and its native error
policy, superseding the "absent allocator" item above. Oversize-path stack
result uncertainty remains queued as A-009; it is not a safe host allocator.

Next bounded source work: queued1CD3 CHECK ASM stock/personnel expressions,
starting with DWORD stock balance/index arithmetic, before further port changes.
