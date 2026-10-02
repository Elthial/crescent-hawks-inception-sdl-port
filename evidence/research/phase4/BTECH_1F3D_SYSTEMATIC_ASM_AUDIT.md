# Sol: systematic BTECH_1F3D C-to-ASM comparison

Checked 2026-09-17 against baseline `07b6352`: all18 retained implementations,
comprising seventeen native routines and one extracted FAR-address helper.
16 locally matched, one mismatched, one unresolved return contract. Executable
C is unchanged; fresh comments, summaries and registry records document the audit.

## Evidence and limits

Read the complete private `BTech-Reko-expanded/BTECH.reko/BTECH_1F3D.asm`:
SHA256 `CC32C9A9CC0E6E07AE0FDED4B33D436DDD906C74398E65B91552E0475C517176`.
Corresponding `.dis`:
`007CBBEA400D2986A3D6A83EF68FCFE21D2ECE3BCC040482F615F4D211F8FD43`.
Expanded EXE:
`F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE`.

Compared arguments, returns, access widths, signed conditions, arithmetic,
branch/loop ordering, local state, calls and writes. The per-segment ASM truncates
086A after its final call; `.dis` supplies the return, and expanded EXE offset0881
is RETF (CB). Its mapping is MZ header paragraphs*16 + (1F3D-0800)*16 + offset.
The leading converter return fragment belongs to the previous1E56 routine,
already audited there; it is not an eighteenth original1F3D routine.

Only adapter2/EGA is maintained. Deleted other-adapter branches are intentional
omissions, not transcription discrepancies. Stack probes, saved registers and
compiler frames are abstractions. Matching assumes native16-bit WORD narrowing,
signed WORD comparisons, unnormalised16:16 FAR pointers, and correctly bound
overlapping logical memory views. The header is not a packed, runnable host
struct. These statuses certify local transcription, not compilation, shared
callees, hardware timing, successful reads or gameplay. No private file is staged.

## Implementation ledger

| Implementation / evidence | Result | Simple operation and checked contract |
| --- | --- | --- |
| Advance_Far_Offset_1F3D (0139/013F/049D/079D) | Matched helper | Advance the pointer's offset WORD and retain its segment; no independent native ABI/body. |
| Wait_For_N_Vertical_Retraces 0006–002E | Matched | Wait N retraces: test old argument after decrement, so zero waits zero and256 waits256; final private countFFFF. |
| Pending_Input 002F–0052 | Matched | Probe real input first; return WORD1 for a real key or replay mode3938, otherwise0; consume neither. |
| Display_Text_Dynamic_Value 0053–0085 | Matched | Format unsigned WORD in base10 into FAR3092:0012, then render that same shared buffer. |
| Draw_GraphicsFile_In_Memory 0086–00D4 | Matched EGA | Copy A800:0000 to A000:0000 with the four caller WORD bounds; ignore incoming file buffer in this branch. |
| Draw_EGA_Text_To_Screen 00D5–01FA | Matched EGA | Set colours, scan FAR string, handle CR, mask glyph7F, signed column-wrap test, call glyph2251. |
| Draw_Horizontal_EGA_Line 01FB–0258 | Matched EGA | Fill inclusive Y rows through03EB, without sorting/clipping; reversed signed Y bounds emit nothing. |
| Keyboard_Get_ASCII_Hex_Input 0259–031B | Matched | Live input, recording and signed replay bytes; H delay tokens, real-key interrupt, P exit and exact return WORD. |
| Draw_Clipped_Axis_Aligned_EGA_Line 031C–03EA | Matched | Signed sort then independent endpoint clamps; vertical wins for a point, horizontal next, diagonal emits nothing. |
| Draw_EGA_Horizontal_Span_03EB 03EB–049C | Matched EGA | Initial alignment pixels, signed shifted group count, unconditional tail alignment in bulk branch, inclusive last pixels. |
| Decompress_File_Into_Memory 049D–0524 | Matched EGA | Test one marker BYTE, offset increment1; marker1 calls22F8, every other marker2368. |
| Set_Palette_registers 0525–05BB | Matched EGA | Wait retrace then16 palette BYTEs; CBW and signed >7 add8; push register/value WORDs to022A. |
| Allocate_Far_Buffer_05BC 05BC–063A | Unresolved return | Normal allocation and null reporting match; oversize branch reads unassigned native stack WORDs. |
| Load_File_To_Memory 063B–06C2 | Mismatch | Open retry/read prefix/payload/close/AX1 match, but FAR SS:BP-2 prefix destination is not explicitly bound in C. |
| EGA_DrawBox_Wrapper 06C3–0709 | Matched EGA | Copy AC00:0000 to A000:0000, bounds13/0/27/200; no incoming arguments. |
| Capture_Combat_Sprite_070A 070A–0813 | Matched EGA | Allocate wrapped width*height*4+4, publish FAR sprite pointer, write two header BYTEs, copy interleaved rows. |
| DOS_Load_File_to_memory 0814–0869 | Matched | Failed-open prompt/retry, then raw caller count read and close; no prefix or explicit success return. |
| Wait_For_50Hz_Then_Check_Input 086A–0881 | Matched | Wait50 retrace edges then drain pending keys; no50Hz clock configuration. |

## Mismatch: length-prefix destination must preserve SS

At0665 native code pushes2, obtains BP-2 with LEA, explicitly pushes SS and the
offset, then the handle, and calls3580. The next read uses that local WORD as
its byte count. C's ordinary `&PayloadByteCount_BP02` does not explicitly carry
the stack selector into the FAR buffer contract. Near-to-FAR conversion using
the data segment is not equivalent to SS:BP-2; relying on a future compiler's
stack-aware convention is not an explicit research transcription.

`Sol:TODO ASM-1F3D-0665` records the correction. Use a real stack-address binding
or an explicit two-byte temporary and little-endian conversion when correcting
the source. Do not fix this by changing the read into the caller's destination:
the prefix itself is not payload. The existing synthetic loader model already
uses SS-local addressing, so passing it never proved the C address was right.
Both read results and close result are ignored; a partial prefix can leave an
indeterminate native count, and AX1 is merely completion of the open-success path.

## Unresolved: allocator oversize return

05BC accepts a genuine DWORD request. A signed negative high WORD goes to the
normal allocator; zero high WORD accepts every low WORD (the unsigned <=FFFF
test is tautological); positive signed high WORD prints the oversize message
and waits for input. The error branch never stores BP-6/BP-4 before060B tests
them and0631 returns them as DX:AX. A null normal result prints another message
and waits, without retrying, terminating or substituting a pointer.

The C retains an uninitialised pointer, which is undefined C behaviour rather
than a defined snapshot of native stack bytes. Do not label this routine fully
matched or invent NULL on the oversize branch. Normal allocation/null paths are
locally matched. Existing Astra candidate A-009 covers the native return policy;
no expensive review was invoked. `Sol:TODO ASM-1F3D-05BC` makes this limit visible.

## Additional contracts accounted for

- Text row offset is WORD(320*Row), or eight EGA scanlines of40 bytes. CR
  advances320 and resets the original column but does not increment global4FBE.
  Automatic column wrap compares signed Column to WORD(4FB8-1), increments4FBE,
  and advances320. Column/row increments narrow to WORD. String offset wraps
  independently of its segment. Callees must not rewrite the scanned text or
  stable resolution while the C expression caches/reloads character values.
- 01FB's loop is signed and inclusive. It intentionally has no clipping or
  overflow guard. 031C clamps each signed endpoint to X0..319/Y0..199 after
  sorting; wholly offscreen lines can collapse to an edge point. This is not
  general geometric line clipping.
- 03EB reads the adapter2 WORD alignment masks4FC8=7 and4FD0=01F8 and low BYTE
  shift4FD8=3. SAR applies to the narrowed signed difference, not an unsigned
  difference. Native rereads some shared tables; matching assumes they and the
  adapter selection stay fixed. Synthetic coverage is limited to valid screen
  spans, not malformed bounds or arbitrary shift counts.
- Recording writes the low BYTE at3092:00A0+WORD39F8 and increments that WORD.
  Lowercase h becomes H; recording H loops to another real key. Replay CBW
  sign-extends each byte, H waits30 before another byte; a real key sets0152 and
  is consumed, every completed replay value waits1, P sets0152, then local WORD
  returns. There is no invented EOF or replay bounds guard.
- Palette indexing uses native offset-WORD arithmetic, fixed16 iterations and
  signed CBW: high-bit inputs are negative and must not gain8. It is not a
  validated portable palette API.
- Sprite allocation narrows IMUL width*height before MUL4, adds4 with WORD
  wrap, and passes high WORD zero. Store Offset/Segment at39FA+SpriteId*4 BEFORE
  null testing. BYTE1=low(height)-1, BYTE2=low(width); BYTE0/3 are untouched.
  For maintained startup IDs0..375, the research array maps the native table;
  out-of-range IDs/host array bounds are not certified.
- Source246C:244B is the buffer ADDRESS, not a pointer value read from the
  scratchpad member. Source X*4+Y*160 wraps to WORD; width*2 WORDs per row,
  gapWORD(160-width*4), destination allocation offset+4 without selector carry.
 0931 narrows width/height to BYTE, height BYTE zero executes256 rows. Do not
  add a zero-size guard or call malformed dimensions safe. Shared0931, EGA
  primitives, runtime3835 and decoder internals are separate audits.
- Both file loaders replace OS-specific IO in the port while retaining prefix
  vs raw-count contracts,8000 library open flags and disk-prompt retry order.
  0814 has no promised AX1 result. Allocation/retrace/keyboard services also
  need portable bindings; replacement classification is not ASM confirmation.

## Verification

`Verify-1F3DTranscriptions.ps1` supplies existing synthetic arithmetic, EGA span,
loader, text, palette, retrace and source/header witnesses. The new systematic
audit script checks18 fresh records, native-profile RETF and alignment table
bytes (optional private EXE), signed replay bytes, sprite index/address wrapping
and the explicit unresolved/mismatch notes. Inventory verification enforces
comment-only C changes and current body hashes. These are static witnesses,
not execution of the annotated C, emulator gameplay or successful asset reads.

Passed131,717 new audit witnesses (including private-profile checks),379,857
existing1F3D transcription assertions,297 input-bridge checks,591 text-box
checks and340 inventory checks. The inventory verifies312 definitions and
comment-only C changes; `git diff --check` passes. No gameplay claim follows
from these counts.
