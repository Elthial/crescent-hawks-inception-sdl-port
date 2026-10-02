# Sol: BTECH_0D27 systematic C-to-ASM comparison

2026-09-17. First fresh file audit following the confidence reset. Source
baseline `62ab3d0`; new C changes are audit comments only. Matching evidence:
local `BTECH_0D27.asm`, its `.dis` return tail, and narrowly inspected
`207F:00D1` / `2CE1` caller contracts. No other file is promoted as audited.
Method-body and ASM hashes are stored in `reference/ASM_AUDIT_RECORDS.json`.

## Results

| Annotated implementation | Checked original blocks | Result |
| --- | --- | --- |
| `Startup_Print_Nul_String_000A` | `000A..0043` | Fully matched with platform-output binding and compiler stack-probe abstraction |
| `Capture_Startup_Sprite_Table_0410` | Extracted native-main block `040B..0833` | Fully matched extracted block: all376 IDs, coordinates, dimensions and capture order |
| `Setup_Game` | `0044..040A`, capture delegation, `0834` through return | Fully matched retained EGA path with explicit adapter/platform abstractions |

This is semantic accounting, not literal line correspondence. It does not claim
the overlapping-header pseudo-C compiles or that called loaders, renderers and
allocators are correct. Their contracts are checked here, not their full bodies.
No new runtime, pixel or music fidelity claim is made.

## String printer: every meaningful operation

- `000A..001C`: native BP frame, two-byte stack probe at207F:2FDC, saved SI,
  WORD cursor zeroing and first terminator test. Compiler stack checking and
  register-save machinery are deliberately represented by C locals.
- `0033..003D`: BX=cursor, LES loads the argument, BYTE comparison at ES:[BX+SI]
  controls the loop. C preserves the segment and wraps offset addition to WORD.
- `001D..002D`: use old cursor in BX, increment WORD cursor, load BYTE, CBW
  sign-extension and WORD argument to207F:0213. C's post-increment read and
  signed-char conversion reproduce the read, next cursor and output argument.
- `003F..0043`: restore frame and return without a result. No game write lost.

`Platform_Write_Startup_Character_0213` substitutes the native output service;
this confirmation assumes stable text and the same character-output contract.

## Sprite block: all captures compared

Native pushes height, width, sourceY, sourceX, spriteID. X/width are eight-pixel
cells; Y/height are scanlines. Signed native WORD counters are nonnegative and
bounded here, making the C unsigned loop comparisons equivalent.

| ASM cluster | IDs | Source X / Y / width / height | Result |
| --- | --- | --- | --- |
| `040B..0435` |0..11|3*id /0 /3 /24|Match |
| `0436..0464` |12..15|3*id-36 /24 /3 /24|Match |
| `0465..0490` |16..35|id-4 /24 /1 /8|Match |
| `0491..04DA` |36..119|column12..23 /row9..15 times8 /1 /8|Match |
| `04DB..05F7` |120..129|Ten explicit rectangles|All IDs and geometry match |
| `05F8..063B` |130..145|column4..7 /row18..21 times8 /1 /8|Match |
| `063C..066B` |146..157|3*(id-146) /48 /3 /24|Match |
| `066C..069B` |158..161|3*(id-158) /72 /3 /24|Match |
| `069C..070F` |162..165|Four explicit3x24 rectangles|All match |
| `0710..0737` |166..185|id-154 /32 /1 /8|Match |
| `0738..0781` |186..269|column24..35 /row9..15 times8 /1 /8|Match |
| `0782..07B2` |270..289|id-258 /40 /1 /8|Match: native and C loop index266..285, spriteID=index+4 |
| `07B3..07FC` |290..373|column12..23 /row16..22 times8 /1 /8|Match |
| `07FD..0833` |374..375|X0/2 /144 /2 /11|Both match |

The row formulas encode sprite IDs, not source-column normalisation. For
example, sprite36 uses X12, sprite130 uses4, sprite186 uses24, sprite290 uses12.
C correctly passes the loop column unchanged in each of those groups.

The wrapper is extracted research code, not an original FAR routine at0410.
Extraction preserves this block: native temporaries are not consumed after it;
the next calls enter Start_Game and restore text mode. Thus its contract is a
376-call sequence, not a new original function ABI.

An initial commentary mistakenly alleged four X discrepancies by confusing
sprite-ID and source-X expressions. Direct source inspection disproved that
claim. No discrepancy patch was applied and no game bug is recorded.

## Startup: all retained EGA operations and omitted paths

| Blocks | Comparison / intentional abstraction |
| --- | --- |
| `0044..007D` | Banner retained. Adapter prompt/input intentionally removed; C fixes adapter2 instead of accepting ASCII1..4. Compiler stack probe omitted. |
| `007E..0111` | Signed drive choice31..33; WORD3FFE=choice-31; hard-disk choice clears3FFE and sets WORD D580. Both hard-disk instruction strings/waits, second-drive string/wait and retry behaviour match. Native completion local is redundant. |
| `0112..0143` | Adapter ASCII normalisation subsumed by forced EGA index. Graphics init and disk2 selection match; adapter0-only0BA7 absent intentionally. |
| `0144..01EC` | INFOCOM filename, FAR3092:4614 compressed buffer, FAR246C:244B output, A800 EGA staging, palette2FE8:0000, rectangle0/0/40/200 and input drain match. Adapter3's alternate palette intentionally absent. |
| `01ED..0210` | Old WORD countdown tested then decremented, one retrace per body and pending-input cancellation match the C post-decrement loop, including final unusedFFFF.700 is retraces, not milliseconds. |
| `0211..029D` | Drain input; title call46A7; BTBORDER load/decompress/stage; count8 and FAR return stored4066/4068 match. |
| `029E..031F` | TINYLAND load/decompress/stage; count66 and FAR return stored4588/458A match. |
| `0320..040A` | MECHSHAP load/decompress and EGA in-place0572 conversion,16000 WORDs, match. Adapter0 temporary switch/copy/conversions intentionally absent. |
| `040B..0833` | Capture block extracted into the separately checked method above. |
| `0834` through return | Start_Game50C8 then text-mode restore0D12 match. Segment ASM ends at MOV SP,BP; matching `.dis` supplies POP BP and return. |

Four omitted calls014C/0223/02A5/0327 to207F:00D1 are accounted for.
That helper returns without translation-table writes when246C:B764 is nonzero.
Graphics setup passes adapter2 to207F:2CE1, which writes B764. Therefore its
tables at2FE8:0230/0130/0190/0170 have no retained EGA effect. This is a
checked restriction, not an unexplained missing call.

Native DS-based filename/banner pointers are represented by expanded-image
string values and3EDB references. Working buffers use their **addresses**, not
the values of pointer-shaped header fields. Native DX:AX returns are represented
as research FAR values. The port replaces file/UI/video services, not formats.

## Verification and remaining boundaries

The new structural check ties actual C capture expressions to the native
unadjusted-X pushes and key argument patterns. Existing startup/music tests
cover geometry and native arithmetic models. Neither executes this pseudo-C;
fresh confidence comes from complete manual block accounting above.

All3 local implementations are matched under documented native contracts and
intentional EGA/platform abstractions. Remaining uncertainty is in dependent
files and runtime presentation, not an unaccounted local block in this file.

Next file: `BTECH_0800.c`, in bounded blocks. Do not carry caller confirmation
into its Start_Game, loader, map, drawing or sound routines.
