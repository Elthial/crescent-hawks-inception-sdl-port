# Sol: systematic BTECH_1E56 C-to-ASM comparison

Checked2026-09-17 against baseline `1c39509`: all15 retained implementations,
comprising ten native routines and five extracted research helpers.13 locally
matched; two mismatched. Executable C is unchanged. Comments, summaries, fresh
registry records and corrected static keyboard evidence document this audit.

## Evidence and boundaries

Private `BTech-Reko-expanded/BTECH.reko/BTECH_1E56.asm` SHA256:
`B1D4F6A903C9ACDEEF8D7DF0A0BC85954539122D6DC4D35AE6978AF6F03BAB47`.
Corresponding `.dis` SHA256:
`461269F62BDCC4AC971A9CA56B18557312115D334865F4FDC93208F66252878A`.
Expanded EXE SHA256:
`F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE`.

Read the full ASM segment, every branch/loop/shared tail, the `.dis` WORD255
sentinels and converter return,3EDB selectors/table addresses,305B descriptor
and layout bytes, and EXE bytes for the converter's missing final table entry.
File mapping is MZ header paragraphs*16 + (native segment-0800)*16 + offset,
consistent with the repository's confirmed image profile. No original binary
or asset bytes are staged. Compared meaningful arguments/returns/locals, access
widths, arithmetic, calls, conditions, loops and side effects. Compiler stack
probes and frame/register restoration are abstractions.

Matched is local semantic agreement, not compiled C or gameplay validation.
Shared calls retain their native contracts; EGA adapter2 is the maintained
graphics target. Other adapter capture/scroll/palette paths are deliberate
omissions. State/tile pointers are proper BYTE/WORD/native FAR views, not a
packed host realization of the overlapping scratchpad header. Native arithmetic
wraps WORD offsets without carrying into the pointer's selector.

The five research helpers have no original ABI or separate ASM body: their
records certify the extracted addressing operations at the callers below.
WORD reads/writes are certified for the even-addressed native descriptor/layout
accesses, not arbitrary odd-offset helper arguments or a WORD spanning offsetFFFF.
Original descriptors0320/0338/0350 are even; native layout field offsets are
even. Do not infer a generic processor boundary policy from the helper's two
separate BYTE accesses. Drawing/input callees must not mutate the menu's stable
geometry/count/highlight metadata; native rereads these fields whereas C caches
some of them. Mutable cursor/colour/selection state is accounted for separately.

## Whole-implementation ledger

| Implementation / native evidence | Result | Checked operation |
| --- | --- | --- |
| Border_Descriptor_Word /0004,01E7 loads | Matched for native aligned FAR WORD accesses | Index*2 plus descriptor offset wraps; low/high bytes and selector retained |
| Border_Tile_Address /0004,01E7 SHL5/ADD | Matched | WORD tile*32 plus4066 offset,4068 selector unchanged |
| Draw_Menu_Border /0004–01E6 | Matched | DS4FA4 index*4 FAR descriptor, four corners, top/left/right/bottom chains and return |
| DrawCall_Border /01E7–0280 | Matched | Signed length loop, tile/position increment,00FF sequence rewind, terminator scan and next-index return |
| Menu_Layout_Read_Word /0281,0B5E loads | Matched for native aligned accesses |305B16-byte stride plus field offset WORD wrap |
| Menu_Layout_Write_Word /0281,0B5E stores | Matched for native aligned accesses | Same addressing, full WORD value stores |
| Menu_Memory_Variables /0281–0387 | Matched | DS4FA2 first-call gate, outgoing four mutable fields, current index, incoming eight fields |
| Draw_Top_Graphic_Sidebar /0388–03F4 | Matched | Background interior rectangle, pixel endpoints, both cursor clears; no border draw |
| Menu_Text_Read_Byte /03F5 LES/add loads | Matched | Text offset plus read index wraps, selector unchanged, BYTE read |
| Display_Text_From_Memory /03F5–07CA | Mismatch | Entire control/word-wrap/long-word/final-flush tree agrees logically; near stack arrays lose explicit native SS FAR binding and do not reproduce frame-overwrite behavior |
| Display_Text_In_TextBox /07CB–0A3A | Matched under mutable FAR/EGA contract | In-panel drawing, padding/truncation/scroll/strip draw, cursor accounting and buffer consumption |
| TileSet_Memory_Operation /0A3B–0AE4 | Matched under EGA contract | Ignore source pointer, forward destination offset/selector and X/Y to207F:0313 |
| Create_TileSet_Array /0AE5–0B5D | Matched | Wrapped count*32/CWD allocation, saved original pointer, signed count loop, row rollover, destination offset advance, original DX:AX return |
| Display_Menu_Choices_And_Check /0B5E–0D1C | Matched under stable metadata/shared input contract | WORD menu fields, selection reset, marker/drain, confirm/navigation loop, wrap, saved selection and full WORD return |
| Keyboard_Convert_To_MoveCommands /0D1D–0E75 | Mismatch | Signed compare tree, full30-entry lowercase/alias table, FFxx identity and default return; three incorrect/missing C aliases |

## Text03F5: SS binding and buffer limits

Native passes FAR SS:BP-5E token and SS:BP-32 line addresses at every append,
copy and flush site. The current independent ordinary `unsigned char` arrays
are near stack pointers; converting them to a FAR parameter using DS3EDB loses
SS. FAR pointer-valued temporaries do not repair that initial conversion.
Make explicit SS/native-address bindings in preservation C, or use actual buffer
objects in the C# port. This is the remaining whole-method discrepancy even
though control flow and advance flags were previously substantially repaired.

The42-byte token region ends before padding WORD BP-34; the40-byte line region
ends before other locals. Native has no bounds checks; overflow can overwrite
adjacent locals/regions. Separate host arrays and host undefined out-of-bounds
accesses do not specify that original frame behavior. Valid non-overflowing text
is the contract for the logical subflow comparison. No new game-bug reachability
claim or automatic size clamp is introduced.

Checked logical subflows:

- Fetch increments WORD read index before NUL/stop processing. A high-bit byte
  is masked and processed once; the next byte is fetched before the stop check.
- CR terminates token, appends if signed width fits, otherwise flushes line then
  token; both paths advance and reset pending width/token count.
-02/06 flush without advance, consume a CBW operand into background/foreground
  WORD and restart pending width from updated cursor. Operand bit80 is signed
  data, not another stop marker.
-09 flushes without advance, consumes an absolute CBW column and, if signed
  column>=width, resets column/pending width without an invented row advance.
-13 consumes signed padding target; appends spaces until decremented target
  reaches CURRENT global cursor, then appends pending token and adds its length
  to pending width. This is not a conventional tab-stop or width clamp.
- Spaces append one delimiter unless pending end is zero or exactly width.
  Overflow appends space/NUL, flushes old line with advance1, copies pending
  token into it, skips consecutive input spaces and updates pending width.
- Long-word threshold is signed width-1<=token length. A nonempty line is flushed,
  the new character and NUL are appended, then the shared advance path runs.
- End flush tests token count OR line[0]; fits appends/flushes0, overflow flushes
  line1 then token0. Call07CB consumes each mutable text buffer's first BYTE.

## Border/layout/capture details confirmed

01E7 compares descriptor WORD with00FF, notFFFF; `.dis` explicitly confirms
the WORD value255. Signed nonpositive lengths skip drawing but still scan the
sentinel. Pattern repetitions chain next-sequence indices; no sentinel/bounds
validation or empty-pattern repair is native. Four corners precede top/left/
right/bottom edges. Tile addressing uses the FAR4066/4068 base with SHL5 WORD
wrap; descriptor-table addressing uses incoming DS, not an invented fixed global.

0281 first call sets DS4FA2=1 without saving outgoing garbage. Later calls save
only foreground/background/cursorX/cursorY in the previous305B record, then
restore all eight incoming fields. Same-index activation preserves latest mutable
state.0388 fills the whole interior at origin*8 through(origin+extent)*8-1 and
zeros both cursors. Its inherited name and old human summary implied border
drawing; summary is corrected, executable calls unchanged.

0A3B's EGA capture ignores supplied DisplayPtr and reads the preconverted A800
source through207F:0313.0AE5 always allocates; MemoryLocation is source, not a
fallback destination. Native count*32 wraps as a WORD BEFORE CWD to32-bit size;
negative signed counts capture nothing. Destination offset alone advances32,
source column increments and signed X>39 rolls to0 with Y++; return is original
allocation, not the final cursor. Allocator error/oversize behavior belongs to
the existing1F3D:05BC contract, including its queued runtime uncertainty.

## Scrolling07CB confirmed

Signed cursorY<height draws at origin+cursor. Without advance, strlen is added
to cursorX; with advance it resetsX/incrementsY. At/beyond bottom, native saves
ORIGINAL strlen, pads to signed width and terminates exactly at text[width].
EGA scroll uses source A000:0140, destination A000:0000, rectangle height*8-8,
width cells, originX cells and originY*8. Bottom strip is colour0 with exact
right=(originX+width)*8-8, not a substituted last-pixel-1. Padded text draws on
last visible row. Without advance, ORIGINAL length is added toX and Y becomes
height-1 before signed full-row testing. Advance bypasses that Y normalization
and increments incomingY. Every return writes text[0]=NUL. No page/key pause,
clipping/negative-size guard or immutable-text contract was invented.

## Menu selection0B5E confirmed, with existing runtime concern

Fields are WORDs at305B:(index*16)+92/94/96/98/9C. Signed saved selection>=count
or either input flag resets saved state0; negative saved selection is not rejected.
B782 renderer WORD is cleared, initial marker is drawn and pending keys drained.
Only Enter/Space exit the ordinary loop. Navigation accepts signedFFB8/FFB0,
XOR-erases old marker, decrements/increments WORD selection, applies signed wrap
and redraws. ESC post-loop branch exists but is unreachable through normal local
exit logic; no ESC cancellation was added. Selection marker remains drawn.

Sol: corrected after checking EXE bytes83 7E F8 B8/B0 at0C40/0C46.
Opcode83 sign-extends the immediate to FFB8/FFB0, matching the converter.
The generated ASM text omitted that sign extension; the intermediate .dis
retains it. The earlier claimed native menu-token inconsistency was false.

## Converter0D1D: corrected full table evidence

The printed ASM ends at table0E6D; `.dis` supplies the0E71 return but not the
last table WORD. Read EXE0E6F: its target is0D81 (north). The30-entry table is
indexed by key-61 through1D inclusive, so punctuation aliases must not be shifted.

| Key | Native table slot / target | Native return | C at original audit checkpoint |
| --- | --- | --- | --- |
|007B `{` |1A /0E69 ->0E71 |007B unchanged |FFB5 west |
|007C vertical bar |1B /0E6B ->0D69 |FFB5 west |007C unchanged |
|007E `~` |1D /0E6F ->0D81 |FFB8 north |007E unchanged |

All other WORD inputs agree with the current switch under the header direction
constants: letters A/C/D/E/Q/W/X/Z (both cases), digits1/2/3/4/6/7/8/9,
backslash northwest, backtick south,0C andFF0C east; native encoded directions
remain identical. Unrecognized inputs return their original full WORD.
Earlier braces/complete-table claims and the synthetic oracle were wrong.
Verify-InputBridgeTranscriptions.ps1 has the corrected30 entries and
originally asserted the three pending source discrepancies instead of false
full agreement. Follow-up: the annotated switch now passes007B through,
maps007C west and007E north, matching the preservation C implementation.
Both transcription verifiers now require zero discrepancies, including the
systematic verifier's comparison of all65536 WORD inputs. The private EXE
check confirms all30 table entries. The registry counts below remain the
historical audit checkpoint, not a new certification of the entire file.

## Validation and follow-up

Verify-1E56SystematicAudit.ps1 checks every WORD converter input, native-aligned
address witnesses, signed size semantics,15 registry records and optional private
EXE table bytes. Existing border/layout/text-box/menu/input models are regressions;
none executes C or original graphics/keyboard gameplay. Source-body hashes bind
records to these final comments. No expensive Astra run is needed for the directly
visible table/binding corrections; interactive menu traces and buffer/allocator
compatibility policy remain separate validation work.
