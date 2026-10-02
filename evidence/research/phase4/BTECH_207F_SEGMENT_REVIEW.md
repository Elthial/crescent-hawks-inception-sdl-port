# BTECH_207F — shared hardware, rendering, map and runtime helpers

Sol: Current checkpoint (2026-09-17): later1E56/1CD3 and EGA caller reviews
resolve the listed stock-value, border-descriptor/tile-address and named box
caller follow-ups. Earlier next-step prose remains historical. This is not a
certificate for every caller; see the current TODO_REVIEW.md dispositions.

## Scope and logical review chunks

Sol: owner requested large logical chunks, not single tiny helpers. Use the
primary expanded-EXE ASM/.dis as evidence; do not restore deleted non-EGA
pipelines or treat Reko32-bit types as proof of pointer width. This remains
research pseudo-C with overlapping DOS header views, not compiled game code.
No expensive-model review was run.

| Chunk | Routines / boundaries | Status |
| --- | --- | --- |
| Early planar EGA pipeline | 0260,0313,0377,0572/05BF | Reconstructed and checked |
| Strings, numeric formatting, input status, abs | 3B22..3C80 | Reconstructed and checked;3C82 begins a separate DOS routine |
| Speaker/DOS/keyboard/retrace/random/direction | retained001C..00D0,014C,0971,0A9F..0BFA;3C82 | Reviewed; missing/deleted entries classified, not restored |
| Procedural descriptor construction | 0BFB..0FED,104E..11BA | Reconstructed; literal subdivision quirks preserved |
| Map adjacency, addressing and cache movement | 11BB..18EE,1DA8/1DF8 | Reviewed and synthetically checked |
| Overhead map reduction | 1F04/1F51 | Reconstructed and checked; prior1F51 interpretation tightened to valid BYTE IDs |
| Framebuffer transfers and effects buffer | 18EF..1DA7,1ECE,1FBE | Retained EGA transfers/cache reviewed; omitted adapter paths and FAR registration classified |
| Boxes, tiles and menu drawing | retained245C,24D7,275C,2B87 | Retained EGA branches reviewed; absent routines and deleted adapter variants remain evidence-only |
| Memory/runtime multiword arithmetic | 0A76,3D1C/3D44/3D6C,3E2E/3E62/3EC4 | Retained bodies reviewed; absent runtime exports remain outside scope |

This inventory covers retained source clusters, not every ASM export or
deleted CRT/hardware routine. It is not a claim that the whole207F is finished.

## Sol:0260..05CF — packed pixels, capture, sprite clipping and planar conversion

### Call contracts and geometry

| Entry | Actual arguments/data | Result / side effects |
| --- | --- | --- |
| 0260 | source16:16, destination segment WORD | 32000 packed-nibble bytes ->8000 EGA byte-columns; offset0 |
| 0313 | destination offset/segment, X byte-column, Y tile-row | 32 bytes: eight rows, plane0/1/2/3 per row, from A800 |
| 0377 | destination offset/segment, source offset/segment, signed X/Y WORDs | clipped transparent sprite into selected EGA planes |
| 0572 | source16:16, destination16:16, WORD count | groups=count>>1; each reads/stores four bytes |
| 05BF | AL/DH/DL/BH/BL internal register flow, near RET | consume one nibble and rotate bits into plane accumulators |

Geometry constants preserve context: EGA_Plane_Count4,
EGA_PlaneScreen_RowBytes40(320 pixels/8), Height200, ByteCount8000.
Far pointers here are encoded DOS segment:offset values, never host addresses.
Each offset wraps as WORD without invented segment normalization.

### 0260 packed full-screen image transfer

ASM sets graphics write mode2, iterates8000 destination byte-columns and
reads four packed bytes per column. High then low nibble select the eight
pixel bits through masks80/40/20/10/08/04/02/01. Retained volatile destination
reads at original positions prime latches; do not optimize them away as unused
reads in a managed/hardware adapter. Final graphics register write is0008:
bit-mask index8,data0, not fullFF mask. The legacy VGA name is retained, but
this is the original planar EGA transfer, not a256-colour chunky draw.
Removed fake pointer-to-word slicing and byte-incorrect source increments.

### 0313 planar tile capture

Source index=Ytile*0140h+Xbyte, where0140h=8*40. This argument named SourceSeg
in old C is not a segment. Source is fixed A800; destination is the supplied
16:16. Each of eight rows selects read plane0..3 and STOSB writes four
successive destination bytes; source advances40 per row. Corrected old
destination stride3 and invalid pointer operations. Caller1E56:0ACD..0ADF
pushes destination offset/segment and two coordinates, ADD SP8; restored its
missing FAR segment-word argument without rewriting the rest of1E56.
Read-map select remains plane3 at exit; no extra restoration was invented.

### 0377 sprite header and clipping

Source header+1 is a height-minus-one BYTE; INC AL wrapsFF to0. Header+2
is byte-column width. Source pixels begin+4 with four interleaved plane bytes
per column. Header bytes0/3 are not interpreted by this routine. Original
source-row stride=width*4, preserved separately from clipped visible width.
New scratch WORD views0268 columns,026A stride,026C remaining rows match ASM;
0266's bogus struct initializer is removed.0262/0264 are WORD storage.

Signed SAR3 gives floor(X/8), including negatives. Top clipping skips
(-Y)*originalStride and shortens rows; bottom clipping limits to200-Y. Left
clipping advances source by(-floor(X/8))*4 and shortens columns; right clipping
limits to40-column. Retain original ordering, signed/unsigned branch differences,
16-bit narrowing and no added validation. Destination offset is base+Y*40+
column; original MUL DL uses AL, safe for accepted Y0..199.0264 is then reused
as destination row skip40-visibleColumns. Source row skip is originalStride-
visibleColumns*4, not the old invented division by3.

### Transparency, plane writes and pixel shifts

OR all four source plane bytes to get occupied bits: colour0 is transparent.
Shift (occupied<<8) right by X&7, invert WORD to obtain left/right preservation
bytes. Select each matching read/write plane, AND destination with preserve
mask, then OR shifted source plane. Spill byte is written only while screen
column<39; do not spill rightmost column into next row. Four-plane unrolled
ASM is represented as one explicit plane loop; source/destination order remains
the same. Rows and columns use original do/LOOP behavior; malformed zero
dimensions can wrap counters, not an invented early safe return.

Cleanup writes0F02h to DX. Normal drawing has DX=03C4h. Fully clipped early
exits may retain caller DX, or top-clipping MUL's high-product WORD instead.
Research binding CleanupPort=dx and MUL updates deliberately preserve this
original bug, not a new callable argument. See BUG-019 in the
[original-game register](../investigations/ORIGINAL_GAME_BUGS.md).

### 0572/05BF planar transposition

Each iteration reads four packed bytes(eight pixels), then writes BL,BH,DL,DH
as plane0,1,2,3. Two05BF calls consume each byte's high/low nibble. Rotation
order DH,DL,BH,BL takes pixel bits3,2,1,0; eight rotations overwrite every
prior accumulator bit. The C helper models register state using an array and
AL return, not imaginary DOS stack parameters. In-place conversion remains
safe because all four source bytes are read before destination stores.

Old BytesToRead parameter is now WordsToConvert: actual groups=count>>1;
0800 animation count0790h processes0F20h packed/output bytes. Odd final word
is discarded. No zero-count guard exists: count0/1 falls into LOOP with CX0
and executes65536 groups. This is a caller/input-contract hazard, not yet a
verified reachable original gameplay fault. No guard was silently added.

### Existing callers and remaining boundaries

1631:1F73 already encodes destination AC00:0000 and now passes signed WORD
coordinates to the recovered0377 signature. Updated its stale dependency note.
Several0800 callers still use legacy pointer-table aliases and ambiguous
decompressed-buffer scratch declarations. Their FAR-source construction/table
loading and exact table extent need a dedicated shared-view/caller pass; do not
claim those callers became compilable just because the callee was reconstructed.
No deleted Tandy/CGA/alternate draw body was restored.

### Verification and confidence

Primary evidence: clean207F ASM0260..05CF,1E56 caller0ACD and1631 caller1FB3.
Raw expandedEXE bytes at0568 are B8-02-0F-EF followed by pops/RETF: MOV AX,
0F02h; OUT DX,AX, no port load. Mapping uses MZ header+(207F-0800)*16+local.
Original DOS selector/segment relocation is kept distinct from host pointers.

Synthetic scripts: Verify-EgaTranscriptions.ps1 **1646 assertions** on all256
packed-byte patterns, per-pixel transparency versus WORD mask composition,
clipping/source offsets, cleanup DX, geometry and zero/odd LOOP count.
Combat transcription3034 and InceptionTools182 checks also pass. These are
mathematical/regression checks, not compiled pseudo-C or DOS hardware playback.
BUG-019's visible impact and latches/timing still need runtime observation.
No copyrighted external files were changed/staged; rawEXE was read locally only.
No new Astra run was needed for locally resolved bit/offset flow.

## Sol:3B22..3C80 — strings and WORD/DWORD numeric formatting

### Recovered contracts

| Entry | Contract | Result |
| --- | --- | --- |
| 3B22 | FAR destination, FAR source | strcat: append source including NUL; original destination DX:AX |
| 3B68 | FAR destination, FAR source | strcpy: overwrite destination including NUL; original destination DX:AX |
| 3B9E | FAR text | strlen: WORD length excluding NUL |
| 3BB6 | WORD value, FAR destination, WORD radix | signed decimal WORD / unsigned other bases; destination DX:AX |
| 3BD2 | DWORD value, FAR destination, WORD radix | signed decimal DWORD / unsigned other bases; destination DX:AX |
| 3BDC | incoming DS and DS:5366 buffered-input state | AX=00FF if buffered input pending; otherwise DOS availability AL, AH=0 |
| 3C15 | wrapper register state and saved frame | common formatting tail body, not an independent FAR ABI |
| 3C6C | signed WORD value | signed WORD absolute value, including original 8000h overflow |

The requested3C85 boundary falls inside3C82's DOS interrupt-vector installer.
Its body extends through3CA4, and was deferred intact at the text checkpoint;
now reviewed below with DOS helpers.3C81 is padding.3BEC..3C06 are separate
buffered-character reader entries, not a continuation of the status function.

### String scans and copies

REPNE SCASB searches **for zero**, not for the first nonzero byte. CX begins
FFFFh (65535 bytes), not a255-byte counter. NOT CX yields bytes scanned,
including the terminator; strlen then subtracts one. Unterminated input exhausts
the scan and producesFFFEh as length; no safety/error sentinel was invented.
Explicit segment bases and WORD offsets model DOS16:16 access, not host pointers.

3B22 first finds destination NUL, scans source and copies source over that NUL.
3B68 only scans/copies source: its legacy name Append_Large_Text_To_Memory is
misleading; there is no append or special large-text mode. Retain public names
for caller continuity and make their actual contracts explicit in comments.
Preserved existing human call inventories and3B9E's previously verified finding.

Both use an optional leading MOVSB, REP MOVSW and an optional trailing MOVSB.
strcat tests source SI for alignment, strcpy tests destination DI. The shared
research copy model reads both bytes before each word store; overlap is not
memmove-safe. String instructions assume clear DF; formatting explicitly CLD.
These helper models are not new recovered executable entry points. No caller
buffer capacities or malformed cross-boundary word accesses are certified here.

### Decimal sign and real long values

3BB6 loads value from BP+6, destination from BP+8/+A and WORD radix from BP+C.
Only radix10 executes CWD, sign-extending the WORD; other bases zero-extend it.
3BD2/3C08 loads DWORD BP+6/+8, destination BP+A/+C, radix BP+E. Both set BL=1
and tail-jump to3C15. Caller1631:2043 pushes radix0Ah, destination3092:0012,
balance high/low WORDs, then ADD SP,0Ah;1CD3 callers confirm this layout.
Restored radix10 in all retained money call sites. This is a genuine unsigned
long balance **value**, not a file/segment pointer.

The common body interprets a negative high WORD as a sign only in decimal.
NEG low / ADC high,0 / NEG high gives the magnitude, including80000000h.
Each digit uses two unsigned WORD DIVs: high quotient/remainder followed by
(high remainder:low)/radix. Stored digits use lowercase a..z for bases up to36.
Emit least-significant-first, always emit one digit for zero, then NUL and
reverse the digit portion without moving '-'. Original destination returns
in DX:AX. The core remains a research model of a shared epilogue, not a newly
callable C export.

Examples: WORD FFFFh -> '-1' at radix10, 'ffff' at16; DWORD00010000h -> '65536';
DWORD80000000h -> '-2147483648'; DWORDFFFFFFFFh -> '-1' at10, 'ffffffff' at16.
Stored unsigned balances above7FFFFFFFh therefore display as negative in
decimal. This is verified formatter behavior, not yet a reachable gameplay bug.
No currency separators/prefixes are inserted.3C6C likewise is signed WORD abs,
not unsigned BYTE; abs(-32768) remains8000h, as original NEG.

No buffer-size/radix validation exists. Radix0 causes DIV failure; radix1
does not terminate for nonzero input. Valid observed money callers use10.
These are contract hazards, not newly claimed original gameplay faults.

### Input status and caller boundaries

3BDC reads DS:5366, tests AH and sets AL=FFh. If the buffered-state high byte
is zero, it returns00FFh immediately. Otherwise DOS INT21h/AH=0Bh supplies
AL=00h/FFh and the helper clears AH. It neither prints nor consumes input.
Old C ignored the DOS result and always returned available; corrected the
research adapter to return AL. `ds` remains an explicit incoming CPU-register
binding, not an uninitialized pointer or a new ABI parameter.

1CD3 stock calls that still pass `&Stock[...]` are tagged Sol TODO: ASM pushes
the DWORD stock value, not its address. Their inconsistent array indexing/view
needs its own caller pass; no speculative stock-record rewrite was bundled here.
Legacy fp expressions in other text callers likewise remain separate work.

### Verification

Primary clean207F ASM3B22..3C80 plus1631:2043 and1CD3 money calls establish
the scans, widths, signedness, digit algorithm and stack contracts. Synthetic
Verify-TextTranscriptions.ps1 passes6443 assertions: WORD/DWORD values at bases
2/8/10/16 against .NET conversion, sign/carry boundaries, deterministic varied
DWORDs, string lengths beyond255, source/destination alignment, NUL/canaries,
append prefixes, WORD scan wrap/ceiling and buffered/DOS availability.
These do not execute pseudo-C, DOS interrupts or hardware. No original external
assets changed or embedded; no Astra invocation was needed for resolved flow.

Next at this checkpoint was speaker/DOS/keyboard/retrace/random/direction; reviewed below.

## Sol: retained hardware/input, packed direction and DOS vector installation

### PIT and direct-speaker timing:001C..00D0

001C writes B6h to PIT control43h: channel2, low/high divisor loading, mode3,
binary. Removed the old100Hz claim: no divisor is loaded by this routine.
0030 loads a WORD divisor low/high into42h, then enables port61h bits0/1.
The argument is a divisor, not frequency in Hz; zero denotes65536 PIT ticks.
0051 and0067 both clear61h bits0/1 (FCh), preserving all other bits. Duplicate
speaker-off exports genuinely exist in ASM; they are not Reko duplication.

007D toggles only61h bit1 and computes ROR16(argument+9248h,3), storing it at
246C:0252 **and leaving it in DX**.00A9 receives two WORD arguments and implicit
DX, computes CX=(DX & argument1) | argument2, and executes LOOP. Old C omitted
the DX AND entirely. Initial CX0 means65536 iterations, not no delay. This is
CPU-speed-dependent direct-speaker timing, not a millisecond sleep or PIT reload.
1FC5:0587/0630 call007D; subsequent0563/060C delay calls load arguments without
replacing evolved DX. New named0252/0254/0256 scratch views reflect the flow.
Research `dx` bindings document this machine-register dependence rather than
inventing an extra stack argument; native-C-looking calls alone do not model it.
1FC5's loop-local widths/order still need their own review, not bundled here.

014C's earlier verified DOS drive selection is retained: DX holds drive index,
AH0Eh/INT21h, A=0. Its returned AL drive-count is ignored by retained C callers.
Portable replacement should select original-asset sources, not change host drives.

### Packed coordinate direction:0971..0A25

All four arguments and scratch0238/023A/023C/023E are WORDs, not BYTE/int32.
ASM computes northward Y from signed low-byte(y0-y1), then compares the high
nibbles of the coordinate high bytes to force/clear bit7 across regions. X uses
WORD(x1-x0), comparing full high bytes and forcing/clearing bit7 on change.
Retain the exact signedness/correction ordering; replacing this with plain
Cartesian subtraction silently breaks packed-coordinate crossings.

Signed WORD doubled differences are compared with opposite-axis magnitudes.
The factor2 defines overlapping diagonal sectors (boundary equality included).
North/South/East/West flags8/4/2/1 index a **16-byte table**, not the final return.
Raw expanded EXE at246C:0240 provides:

`FF 06 02 FF 04 05 03 FF 00 07 01 FF FF FF FF FF`

Thus N=0,NE=1,E=2,SE=3,S=4,SW=5,W=6,NW=7. CBW sign-extends tableFF to-1;
coincident positions set all flags and return-1. Previous C returned flags,
omitted packed-map correction and could set arbitrary directions for zero
differences. Added an overlapping lookup view, retained the old0240 alias with
its corrected meaning, and corrected function return to signed WORD.
No claim is made that every legacy caller now handles invalid/coincident results.

### Animated-tile EGA upload:0A9F..0B25

Three WORDs are source offset, source segment, destination offset. Destination
is fixed A400, not an unspecified buffer. Configure write mode0, bit maskFF,
disable enable-set/reset, then16 groups of two byte-columns, each four planes.
Read destination to prime latches before each MOVSB. Source advances128 bytes,
destination32 byte-columns; final sequencer map mask enables all four planes.
Replaced invented structs/pointer increments and the wrong bit-mask expression
with explicit WORD offsets and planar loops. Existing0800 animated-tile caller
already passes source segment3092. No deleted adapter code restored.

### Retrace and BIOS keys:0B40/0B8A

Port3DAh bit3 is the observed vertical-retrace status. Exactly mode1 waits for
the opposite level0 then desired level8 (next rising edge); **all other modes**
wait for8 then0 (falling edge). Starting at the desired level does not permit
immediate return. Removed misleading fixed50Hz/horizontal-retrace commentary.
Polling remains unbounded; a future managed platform should supply display-edge
timing, not physical DOS port polling.

0B8A blocks on BIOS INT16h/AH0. For ASCII0, negate the scan code in AL; otherwise
keep ASCII. CBW sign-extends the final BYTE to AX, so the actual return is signed
WORD, including ASCII with bit7 set. Old char-return declaration omitted AX's
sign extension. No key is fetched by this helper's status counterpart3BDC.

### Carry-accurate random generator:0BC0..0BFA

State consists of **three BYTEs at3EDB:4FC0/4FC1/4FC2**, not two WORDs. Keep
existing overlapping WORD aliases because0800 initializes1325h and0090h;
initial active bytes therefore25h/13h/90h, with4FC3 untouched by RNG.
11B8's seed installation also still uses these WORD aliases.

Read byte0, SHR twice (carry=original bit1); RCL byte2 using carry, then RCL
byte1 using byte2's old bit7. Complement byte1's old bit7, SBB it and original
byte0 from shifted AL, narrow to BYTE, then SHR AL. That last SHR's carry—not
SBB's borrow—feeds RCR byte0. Return updated byte0 XOR updated byte1; AH stays0.
Explicit algebra replaces the broken Reko/experimental carry pseudo-C while
retaining byte operation order. No standard PRNG name, period/uniformity or
equivalence to a modern RNG is claimed. The removed speculative rotation
comments contradicted the primary RCL-left/RCR-right instruction sequence.

### DOS critical-error installer:3C82..3CA4

FAR callback offset/segment is saved at incoming DS:5368/536A. DOS INT21h/AH25h,
AL24h installs **CS:3CA5**, not that callback directly; DS is temporarily CS
for installation, then restored. XOR AX,AX returns0. Recovered retained caller
0DAB:0D09 already names this helper and passes its0D26 callback.
The register-saving3CA5 trampoline and3CD8's nonlocal error response require a
runtime-stack review. Do not pretend the callback is a host exception handler,
or that old-vector preservation/restoration occurs in3C82: it does not.

### Missing/deleted exports and confidence boundary

0000 belongs to a neighboring-segment tail, not a new retained207F helper.
00D1 lookup generation,0163 conversion and01D7 row transfer are missing graphics
paths;0A26 is a missing nibble-mask conversion. Do not restore deleted adapters
just to make address ranges look contiguous.0213 is DOS AH06 direct console I/O,
022A attribute-controller access,0B26 status sample (AX=IN3DAh &8),0B73 BIOS mode,
and0BA7 CGA palette: missing retained definitions stay pending/evidence-only.
Map and framebuffer groups remain pending; this completes only the retained
hardware/input cluster plus the requested3C82 routine.

### Verification

Verify-HardwareTranscriptions.ps1 passes6659 synthetic assertions: carry-stepped
RNG reference versus algebra at edge and deterministic varied states, geometric
direction sectors and packed seams, signed scan-code encoding, speaker-bit
preservation, rotate arithmetic, actual decrement-loop counts including zero,
retrace sampled sequences, and original MOVSB/DEC-DI tile-upload geometry.
Also rerun EGA1646, text6443, combat3034 and toolkit182 checks. These do not
execute pseudo-C, calibrate DOS timing, emulate BIOS or produce a hardware trace.
Primary ASM and local expanded-EXE lookup bytes resolve these flows without Astra;
no copyrighted external files changed/staged. New research adapters/register
bindings remain explicitly non-runnable host-C boundaries.

Next large logical chunk: map construction/addressing/cache movement.

## Sol:11BB..18EE,1DA8/1DF8 — adjacency, addressing, rendering and cache shifts

### Scope and remaining construction boundary

This earlier checkpoint reconstructed the cache/addressing half of the map
group. At that point0BFB/0D07/0D79/0DF8 and104E remained damaged dependencies.
Those bodies and1F04/1F51 are now recovered in the next section below. The
research remains pseudo-C, not a compiled/runnable game generator.

### Two parallel block caches

| Memory view | Layout | Meaning |
| --- | --- | --- |
| 246C:0324..0563 | nine64-byte blocks | adjacency/template selectors |
| 246C:0564..07A3 | nine64-byte blocks | source terrain descriptors |
| 246C:07A4..07AC | nine bytes | combined metadata returned by each local render |
| 246C:07AD..09EC | 24x24 bytes | rendered local tile/interaction IDs |

Named nine descriptor grids retain their existing human names. Aggregate views
and adjacency/reflection scratch aliases were added to the header for clarity;
these overlap, not sequential fields of a compilable struct. The same-cell
descriptor address is adjacency address+0240h (576 bytes).

### Adjacency construction:11BB..1313 and1886

1886 calls11BB nine times, source0564+slot*64, destination0324+slot*64.
Each block's8x8 descriptors become equality flags W8/S4/E2/N1. Internal row
neighbours use +/-8, column neighbours +/-1. Across block edges, left/right
are -39h/+39h (-64+7/+64-7), top/bottom -88h/+88h (-192+56/+192-56).
12BA/12D9 handle horizontal block edges;12F2/1303 vertical block edges.
These are NEAR register helpers, not imaginary stack/FAR pointer ABIs.

High-bit descriptors encode direct selectors: output=value-80h, no equality
tests. Preserve the unusual **80h exception**: top/bottom interior columns still
perform equality tests, while corners and interior rows treat80h directly.
This is actual branch behavior, not a uniform high-bit policy invented for cleanup.
Old unsigned comparisons against zero had made these branches impossible.
11BB leaves SI/DI at the final cell (base+63);1886 reloads them each call.
Recovered executable-entry functions retain ascending11BB/12BA/12D9/12F2/1303
source order; model helpers/prototypes do not assert new executable exports.

### Local addressing and block rendering:1314..158B

1314 stores packed X/Y WORDs and shifts only low bytes by4 to find local block
coordinates. It renders neighbour X/Y offsets -1,0,+1 into nine8x8 outputs.
13D9 advances DI by8 per block; after three blocks1314 addsA8h (168=7*24) to
start the next eight-row band. This fills24x24 output without holes or overlaps.
The original FAR wrapper preserves caller DI; the research model does likewise.
Previous C captured returned metadata but lost this destination-register flow.

13D9 starts at adjacency centre block0424h, computes **BYTE(Y*8)**, then X.
Y=FF redirects to039Ch (top-centre last row), Y=8 to04E4h (bottom-centre first
row); X=FF chooses previous block+column7, X=8 following block+column0.
Retain BYTE arithmetic: do not replace it with a full WORD shift or linear
position decode. Scratch02C9 records selector;02CA records descriptor/terrain.

Template address is0C1Dh+selector*64, not selector>>2. (X+Y)&3 selects optional
reflection: bit1 horizontal, bit0 vertical. For reflected selectors and ordinary
template bytes below40h, horizontal lookup211Dh and vertical213Dh apply; both
reflections use vertical **then** horizontal. Output pixel positions mirror the
same axes into20DDh's64-byte scratch block. Fixed IDs>=40h are not remapped.
Header reflection views expose observed lookup domains, not proven allocations;
the two address-domain views overlap. Phase0272's computed bit4 is loaded into
AH later but has no found use in the tile loop; preserve it without a guessed role.

Terrain descriptor bit7 selects direct template copy. Value10h selects the
dedicated block209Dh (internal terrain-base marker70h), bypassing reflection.
Other nonzero terrain values subtract10h as BYTE and clamp results>=31h to30h.
Normal template bytes<40h become (tile&0Fh)+terrainBase, with BYTE narrowing;
fixed IDs>=40h pass through. Return AL=selector|originalTerrain, not a tile ID.
Destination row stride is24, source eight bytes; after eight rows subtractB8h
(184) from the advanced destination so the caller receives inputDI+8.

### Page movement and cache preservation:158C..1885

Packed locals normally range0..127. ASM DEC/INC AL followed by JS decides page
crossing, not old-local==0 for every direction. This corrects east/south gating,
the broken south loop and its accidental unconditional final assignment.
The reconstructed predicates preserve the actual sign-bit behavior, not extra
input validation for malformed locals. Within a page only coordinate changes.

X page changes by1, Y page high byte by10h; western/northern crossings set local
7Fh, eastern/southern crossings local0. World limits are X-page00..0F and
Y-page00..F0. At the relevant outer limit the position and caches remain unchanged.

| Direction | Preserved block movement | Exposed slots | Region indices relative to new centre |
| --- | --- | --- | --- |
| North | old top/middle -> middle/bottom, backwards384 bytes | 0,1,2 | -11h,-10h,-0Fh |
| South | old middle/bottom -> top/middle, forwards384 bytes | 6,7,8 | +0Fh,+10h,+11h |
| West | middle->right before left->middle,64 bytes each per row | 0,3,6 | -11h,-01h,+0Fh |
| East | middle/right->left/middle,128 bytes per row | 2,5,8 | -0Fh,+01h,+11h |

North/south original MOVSW countC0h means384 **bytes**; west20h means64;
east40h means128. Old byte loops copied half the required horizontal extents.
Within valid aligned cache storage the byte research loops preserve the original
word-copy result/order. North's backwards copy prevents overlap corruption;
west must copy the middle block first. No speculative LOD subsystem is involved.

Region index combines X/Y high bytes. Each exposed block takes four vertex
bytes via18D8: source+0/+1/+10h/+11h -> lattice02D3+0/+8/+48h/+50h.
Source vertices form a16-wide lattice; work corners span9-wide rows. Source
address subtraction remains WORD before loading corners; pending region-index
metadata narrows to BYTE separately. Bind SI/DI explicitly for0BFB
construction, then1886 rebuilds **all nine** adjacency blocks. Finally publish
the three exposed slots/region indices at09F3/09F6. Existing prior queue notes
are retained in this table and source end comments, not replaced by unexplained numbers.

### Verified unchanged wrappers:1DA8/1DF8

1DA8 is the FAR DS=246C wrapper around1886, as already annotated.
1DF8's earlier origin interpretation matches ASM: row=((lowY>>1)&7)+2;
rowOffset=row*24 (8+16 multiplication); column=((lowX>>1)&7)+2;
index=rowOffset+column. No invented map decode or caller rewrite was required.

### Verification and unresolved boundaries

Verify-MapCacheTranscriptions.ps1 passes23115 assertions: every valid local/page
for all four directional movements on both axes, rebuild/world-edge gates,
six-block preservation and complete byte extents, varied centre-block adjacency
against geometric neighbours including80h exceptions, reflection write walks,
nine-block output coverage, DI+8 and origin row arithmetic. Existing hardware6659,
text6443, EGA1646, combat3034 and toolkit182 regressions also pass.
These are synthetic math/register checks, not execution of annotated C or the
procedural generator, template lookup contents, game maps or hardware timing.
Outer cache adjacency reads and raw template/reflection allocation extents still
need caller/data-load tracing; no blanket buffer-safety certification is made.
Primary ASM establishes the reproduced instructions. No Astra run or original
external asset changes were needed. No new reachable original-game fault is
claimed from corrected pseudo-C errors.

## Sol:0BFB..0FED,104E..11BA,1F04/1F51 — procedural construction and overview reduction

### Full cache construction and the9x9 work lattice

104E takes a WORD centre-region index. Its SUB BX,11h selects the preceding
vertex row/column; corrected old0Bh subtraction. For each of nine row-major
slots, copy four BYTE vertices from0B0Bh+region-11h+row*10h+column into lattice
02D3h corners0,8,48h,50h; generate one64-byte descriptor block at0564h+slot*40h.
1886 then builds all parallel adjacency selectors. Addresses narrow as WORD,
not BYTE region-wrap before addressing. Existing FAR caller ABI stays one WORD.

0BFB takes DS:SI output and DS:DI lattice through NEAR registers. Save four
corners, clear40 WORDs=80 bytes toFFh, then restore corners, including byte80
which was not cleared. This represents a9x9 lattice (81 BYTEs), not WORD cells.
After subdivision, output the upper-left8x8 cells with each byte maskedF0h.
The final lattice row/column are omitted; bit7/direct descriptor state is retained.
Original end registers are SI=lattice+47h and DI=output+40h. The cache dispatcher
now calls recovered construction rather than an unresolved placeholder.

### Edge subdivision and deterministic seed order

Outer edges are processed top, bottom, left, right. Before each, set09F9 WORD
to the **BYTE-wrapped sum of its two endpoint values**, high byte zero. Read
noise bytes at09FBh+index and increment only the index's low BYTE. Interior
subdivision continues from the right edge's final index; there is no reseed.
This uses the startup-filled GameSeeds lookup, not a fresh0BC0 PRNG call.

0279h upward is an explicit LIFO stack of BYTE endpoint pairs/quads. Horizontal
terminal span is1; vertical terminal span9 (the row stride). Push first-half
then second-half pairs: the right/lower half is processed first. Only FFh
midpoints are generated and consume seeds; existing shared points are retained.

Use WORD endpoint sum/2. Horizontal amplitude is2*halfSpan. Vertical amplitude
is2*(halfOffsetSpan>>3), with literal scale9 corrected to8. Preserve the shift
and correction rather than inventing division by the lattice stride9.
Noise=((seed & BYTE(2*amplitude-1))-amplitude), narrowed to BYTE. Add to mean,
narrow to BYTE, then replace values>=80h with0. This is neither signed clamp
nor saturation to127: wrapped results below128 remain. Some high corner values
are intentional direct descriptors; no blanket 'terrain heights only' claim.

### Rectangle subdivision and preserved register quirks

0DF8 pops TL/TR/BL/BR offsets. Span1 terminates. If missing, centre gets WORD
four-corner sum/4, without perturbation or high-bit clamp. Fill top, left,
bottom, right edge midpoints only if FFh; then push TL/TR/BL/BR child quads so
BR is processed first. Ordinary8x8 traversal peaks at ten quad records/40
scratch bytes; header stack view is an observed domain, not allocation proof.

Two unusual original register effects are preserved explicitly:

1. Left-edge perturbation stores its BYTE noise in DH. Bottom and right means
   then overwrite DL, not DH, before ADD DX,BX / SHR DX. Retained high bits
   therefore affect subsequent mean low bytes; full16-bit overflow also matters.
   The recovered EdgeDX model retains this state instead of computing independent
   clean averages. If left midpoint already exists, DH stays zero from its mean.
2. Right-edge perturbation uses AL, still the **horizontal half-span**, for the
   vertical-amplitude calculation. Left uses the vertical half-span at0278h.
   In normal subdivision widths4/2/1, AL>>3 is0: amplitude0 makes the maskFFh
   and the entire seed BYTE becomes the offset. Wrapped result/clamp still apply.

Primary ASM shows both effects. Raw expandedEXE0FC1 starts8A-F0-D0-EE-D0-EE-
D0-EE (MOV DH,AL; SHR DH three times);0F34/0F8B overwrite DL without clearing
DH before their WORD mean calculations. Intent/visible terrain impact remains
unconfirmed; [BUG-020](../investigations/ORIGINAL_GAME_BUGS.md) records a probable
register-state bug and [A-008](../investigations/ASTRA_REVIEW.md) queues optional
Astra Medium confirmation after a runtime trace. No Astra invocation was made.
Do not 'fix' either behavior in a compatibility port without a separate decision.

### Overhead reduction:1F04..1F50

Reduce centre descriptor block0664h with parallel selector block0424h into
inline buffer244Bh+WORD offset.244B is data here, not a pointer to dereference,
and the routine does not load TinyLand assets. Write eight columns, then skip
20h bytes: **40-byte output row stride**, not a32-byte gap after every pixel.

Descriptor10h becomes dedicated overview tile40h, bypassing adjacency OR.
Other descriptors<90h subtract10h if>=20h, then OR the selector.90h+ IDs pass
through as dynamic overview tiles. Source descriptors/selectors each advance64
bytes. Existing source names/callers retained; corrected broken per-pixel stride
and a scalar field used instead of reading the current adjacency byte.

### Dynamic overview pixel packing:1F51..1F9B

The earlier verified32-byte packing loop remains:64 map-tile IDs -> XLAT through
215Dh -> two nibble colours per byte, destination642Bh. Restored literal WORD
ID-90h, byte-swap, SHR twice, BYTE AH+40h, then WORD+244Bh source arithmetic.
For observed BYTE IDs90h..FFh this is644Bh+(ID-90h)*64; do not call that
simplification exact for arbitrary WORD IDs. Lookup/read offsets explicitly
wrap as WORD, without invented segment normalization or new validation.
Header215D view records the BYTE index domain, not proven palette allocation.

### Verification and remaining work

Verify-MapConstructionTranscriptions.ps1 passes14449 assertions. Synthetic
iterative/LIFO register-mean models agree with recursive right/bottom-first,
independently carry-factored reference on all81 cells,64 output descriptors,
seed-read order/final index and scratch peak for64 varied seeds/corner sets.
Additional checks cover retained-DH carry/overflow boundaries, asymmetric
amplitudes, BYTE wrap/clamp, all descriptor/selector combinations for1F04,
valid dynamic BYTE-ID address arithmetic,40-byte row placement and nibble packing.
Shared midpoint/noise helpers are exercised by both traversals; agreement is
not an independent full executable/hardware oracle. Scripts do not use game
assets or execute pseudo-C. Existing map-cache23115, hardware6659, text6443,
EGA1646, combat3034 and toolkit182 regressions pass.

No original external files changed/staged. Local EXE bytes were read only.
Map data-load extents, reflection/palette contents and original terrain rendering
still merit live traces before a C# port.0FEE..104D is a separate absent BIOS
palette helper, not restored as procedural construction. At this checkpoint, framebuffer,
menu drawing and memory/runtime clusters remain, plusA-008 optional confirmation.

## Framebuffer transfers and effects cache — 18EF..1DA7 / 1ECE / 1FBE

Sol: Cross-reference is the project's clean expanded-EXE ASM, not UnBattletech.
Only the retained EGA pipeline is transcribed. Typed wrapper arguments model
register inputs; they are not new DOS stack signatures. No routines were
reordered outside this bounded group.

### Viewport composition and hardware latch copies

18EF..1AA7 composes a 216x200-pixel map viewport at AC00:000D: byte columns
13..39 (pixels104..319), with40 bytes per screen row. Tile data is read from
A400, at tileID*32: sixteen rows, two bytes per row, in each EGA plane.
The cached tile IDs come from07AD plus the origin index at09ED. The origin
uses ((low coordinate >>1)&7)+2 on each axis, with a24-byte cache row.
The old pseudo-C mistakenly treated the first cached tile value as its address.

Graphics-controller write-mode2 and bit mask zero are intentional here.
A source read loads all four plane latches; the destination write copies their
contents rather than interpreting the CPU byte as an ordinary colour. These
volatile FAR accesses remain hardware research notation, not a portable
framebuffer implementation. Sequencer plane-enable is inherited.

Each horizontal band consumes fourteen cache cells: thirteen full-width tiles
and one half-width edge tile. OddX selects a right half at the left viewport
edge; evenX selects a left half at the right edge. OddY first draws the lower
eight rows of the top tile band; evenY finishes with the upper eight rows of
the bottom band. Between those edges are twelve16-row bands. Cache increments
by24 per band; destination advances by320 or640 bytes. A454=613 and A456=293
are the band advances minus the27 written byte columns, not arbitrary gaps.

Header scratchpad aliases retain the old overlapping views for other callers:

| Address | Reviewed meaning |
| --- | --- |
| A44F BYTE | Half-height: nonzero selects8 rows rather than16; not byte/WORD copy width |
| A450 BYTE | Nonzero selects left byte-column; zero selects right |
| A451 BYTE | Nonzero starts at lower half of tile |
| A452 WORD | EGA full-tile destination width:2 byte columns |
| A454 WORD | Full-band gap:16*40-27 |
| A456 WORD | Half-band gap:8*40-27 |
| A458 BYTE | Twelve full bands remaining; finishes at zero |

1AA8/1ACE/1AF4 preserve the caller's destination and cache registers while
dispatching side-half, bottom-half or full-width copies. Leaves1B71/1BDF/1C83
use source row stride2 and destination row stride40. Final write-mode/mask and
scratch flags remain as in the original; no cleanup/reset was invented.
Deleted non-EGA1B1A/1B94/1BFC and1CB8..1D89 paths were not restored.
1D8C..1DA7 stores the caller's FAR buffer address itself at026E/0270, without
dereferencing it. It is classified only: registration and file-load extent
still need their own caller/data-loading review.

### Effects cache and clear extent

1ECE takes a FAR external buffer and a WORD direction. Zero exports the24x24
map cache at07AD; any nonzero WORD restores it. The native0120h MOVSW count
means288 WORDs /576 bytes, not288 bytes. The transcription retains forward
load-before-store order rather than introducing overlap-safe memmove. Other
legacy caller pointer declarations and pathological segment-boundary/overlap
behaviour remain outside this pass.

1FBE's EGA branch clears A000:0000..3E7F: CX=1F40h means8000 STOSW writes,
or16000 byte offsets. Its bit mask is FF, unlike the zero-mask latch transfers.
Plane-enable is inherited, so this does not promise all planes are selected.
The former pseudo-C's64KiB clear and zero bit mask were transcription errors,
not established original-game bugs. A400 tile storage, AC00 composition and
A000 clear extent are distinct views; no unsupported page-flip interpretation
is asserted. Deleted adapter branches remain absent.

### Verification

Verify-FramebufferTranscriptions.ps1 passes87804 synthetic assertions: all
four viewport parity combinations against independent tile-lattice geometry,
destination guards/unique writes, all256 tile IDs and side/height selectors,
mode2 mask factoring,576-byte cache export/restore (including direction0100h),
and clear extent with inherited plane selections and other-bank guards.
The schedule model is not execution of the annotated C. Leaf stride checks
partly share arithmetic and the latch model is not an independent EGA emulator;
live hardware traces remain necessary before the port. No original assets are
used by these checks or changed/staged by this work.

Regression checks also pass: EGA1646, combat3034, text6443, hardware6659,
map-cache23115, map-construction14449 and InceptionTools182 assertions.
Git diff whitespace validation passes. These validate research models and
tooling, not compilation or gameplay execution of the annotated source.

Next: boxes, tiles and menu drawing at245C..2B87, then memory/runtime helpers.

## Sol: Boxes, tiles and menu drawing — retained245C..2B87

Primary evidence: clean expanded-EXE ASM. Changes stay within the existing
four drawing functions; no omitted drawing bodies or deleted adapters were
restored. Their FAR pointers represent16:16 DOS addresses, not host DWORD
element arrays. Native register/latch accesses remain research notation.

245C stores source and destination FAR addresses at B78A/B78C and B78E/B790.
The four subsequent WORDs are X, Y, width and height. Existing x1/y1 scratchpad
names remain for continuity, but their comments now identify extents rather
than opposite corners. EGA X/width are byte-column units, Y/height pixel rows.
24EB..2568 clips extent sums against40 byte-columns and200 rows using wrapped
WORD addition. Both buffer offsets include Y*40+X; the former undefined ax_40
destination expression was a transcription error. The copy uses write-mode1:
source reads load all plane latches, and destination writes transfer them.
Both buffers advance with40-byte row stride. Bounds checks are WORD comparisons,
not the prior truncating BYTE checks.

275C's EGA branch284D..28A7 draws an8x8 tile from32 row-interleaved source
bytes: plane0,1,2,3 for each of eight rows. X is a byte-column and Y an8-pixel
tile-row; Y*320 means eight40-byte rows, not a pixel index. The original reads
the destination before each plane write. Write-mode0, bit mask FF and disabled
enable-set/reset are explicitly configured. Plane enables are selected one
at a time, then restored to all four. Data-rotate state is inherited.

2B87's EGA body2CB0..2CE0 takes four WORDs: X byte-column, Y tile-row, width
byte-columns, colour. Its old y1 argument was not a height. This draws eight
rows in A000 using write-mode2, mask FF and logical XOR. Every destination
read loads its existing latches before the colour write. Each set bit in the
low colour nibble inverts that entire plane byte, so repeating the highlight
removes it. Data-rotate is restored to replace/no rotation afterward. The
legacy BYTE return is the low colour byte; reviewed callers use the drawing
side effect. It performs no clipping.

### Native edge cases, not silently repaired

Zero-width box REP MOVSB performs no writes, but zero-height DEC/JNZ loops
65536 rows. The menu's LOOP executes once before decrement, so zero width
performs65536 writes per row. Wrapped extent sums can defeat clipping for
malformed input. These are original instruction behaviours, not evidence that
ordinary game callers trigger a visible bug. Any safe C# API validation should
be an explicit later port decision, not concealed in these transcriptions.

### Absent routines and verification limits

28A8..28EA is a separate raw FAR-buffer copy,64 MOVSW /128 bytes, without
graphics-controller setup.28EB begins a separate clipped/masked sprite routine
with EGA and deleted-adapter branches. Neither exists in the retained annotated
source here; this pass does not claim to implement them or all intervening ASM.
Likewise, adapter bodies256B onward and non-EGA tile branches are not restored.

Verify-MenuDrawingTranscriptions.ps1 checks synthetic clipped rectangle address
walks, tile plane ordering/placement, all colours/plane-byte XOR round trips and
native zero-count underflow. Some stride models share arithmetic, and these
checks do not execute the pseudo-C or validate graphics hardware state. Live
traces and caller geometry checks remain necessary before the C# port.

229105 menu-drawing assertions pass, along with framebuffer87804 and EGA1646
regressions. Other regression suites remain part of the shared verification
baseline; no copyrighted assets were changed or added to these tests.

Next: memory/runtime multiword arithmetic, including stock helpers; keep real
DWORD arithmetic distinct from incorrectly inferred32-bit pointers.

## Sol: Memory/runtime arithmetic and stock helpers

Primary evidence: clean expanded-EXE ASM0A76..0A9E and3D1C..3ECF.
Only retained functions were changed, without reordering other code. Legacy
names remain recognisable; recovered numeric locals retain their suffixes.

0A76 takes source FAR, destination FAR and a WORD **word count**. REP MOVSW
copies twice that many bytes. The previous BytesToRead name and pointer-element
expressions were misleading. Each WORD is loaded before storing, with both
offsets advancing or retreating by2 according to incoming DF. No CLD appears
in the native routine. The source uses an explicit incoming FLAGS research
binding; it is not a new stack parameter. Zero count performs no copy. This is
not overlap-safe memmove, and segment-boundary bus details remain unverified.

3D1C/3D44 take a FAR DWORD lvalue and a DWORD operand (RETF8). They load two
WORDs, call unsigned multiplication/division, then store returned DX:AX back
into the lvalue.3D6C takes FAR lvalue plus WORD shift count (RETF6). These are
real32-bit numbers, not inferred pointers or unrelated segment/offset inputs.
The existing0800 stock loop already expresses their operations correctly:
increase multiplies by100 then divides by96/90/48; decline multiplies by the
factor then divides by110. BakPhar above18000 after increase shifts right2.
Intermediate multiplication retains only32 bits, before subsequent division.

3E2E returns the low32 product bits. Low*low supplies the low result WORD and
carry; the two cross-products contribute to the high WORD. High*high is above
the retained result.3E62 returns an unsigned DWORD quotient, not remainder.
For a16-bit divisor, two WORD DIVs carry the first remainder into the second
dividend. For a larger divisor, SHR/RCR scales both DWORDs until divisor high
WORD becomes zero, estimates a16-bit quotient, then multiplies by the original
divisor and decrements an excessive estimate. The transcription includes the
carry beyond32 bits in that comparison. Native division by zero faults; no
substitute result or validation was invented.

3EC4 clears CH and repeatedly SHR DX / RCR AX. Thus only count low BYTE matters:
zero leaves value unchanged;32..255 shifts yield zero;256 means zero shifts.
This is not a C# shift-count-modulo32 operation. The recovered function returns
the shifted DWORD, and its wrapper writes that result back into the lvalue.

Absent3D92 onward signed/runtime exports between these retained functions were
not restored. Other DOS/allocator/runtime exports are not covered merely by
this review. Caller pointer declarations outside this block remain a separate
consistency pass; no original assets were changed or staged.

Verify-RuntimeArithmeticTranscriptions.ps1 passes49980 synthetic assertions:
WORD-factored unsigned products and quotients against numeric oracles over
boundary values and2048 seeded samples, CL-only shift semantics, and MOVSW
direction/count strides. Multiplication oracle uses BigInteger to avoid host
floating-point promotion losing low product bits. These do not execute the
pseudo-C or certify runtime ABI, DF callers or segment-boundary hardware.

## Sol: Final retained207F consistency pass — callers and contracts

The header now records13 reviewed cross-segment contracts for memory copying,
effects-cache export/restore, boxes, single tiles, menu highlights, numeric
formatting and unsigned stock arithmetic. These are DOS16:16 FAR pointer and
WORD/DWORD research declarations, not a claim that the scratchpad header or
annotated project compiles. Register-only tile-copy models remain local to207F.

Confirmed caller fixes, cross-referenced against clean expanded-EXE ASM:

- 1AE8 cache export pushes FAR3092:4314, not its first value or an unbound name.
- 1E56:08EE menu scroll supplies source A000:0140 (corrected by subsequent0852/0870 branch audit) and destination A000:0000,
  followed by X/Y/width/height. The old transcription supplied only five arguments.
- 1E56 menu highlights push WORD values at record+94 and+9C with16-byte record
  stride. All three calls now read those values rather than passing addresses
  or using the old BYTE-array views. The raw305B segment view keeps this bounded
  fix separate from a future complete menu-record reconstruction.
- 1CD3 formatter pushes two balance WORDs from D374/D376 plus ID*4. The typed
  Stock[3] view therefore uses index ID, not ID*4, and passes a DWORD value,
  not a FAR address, on investment/sale displays. Those queued TODOs are resolved.
- 0800's animated-tile copy count0780 means1920 WORDs /3840 bytes, consistent
  with three1280-byte frames; a caller comment now makes the units explicit.

### Remaining caller work / review limits

1E56 border drawing still has malformed descriptor and tile-address expressions:
native DS:4FA4 selects a FAR descriptor of WORD tile IDs;275C needs tileset
base+ID*32, not a fetched DWORD element. Source TODOs identify this follow-up.
Reconstruct the whole bounded border caller block before changing its indexing
or related DrawCall_Border signature. The old MenuUnknown BYTE-array scratchpad
views are also not a trustworthy complete menu-record layout.

Other drawing callers still use legacy segment constants as if they were FAR
addresses, including0DAB/1F3D box wrappers. Their intent is recorded by the
reviewed contract, but they need a dedicated explicit segment:offset caller
pass; no blanket cast was used to conceal those errors. Numeric formatter
calls elsewhere contain additional record-index TODOs unrelated to its ABI.
No full static compilation/call-graph oracle exists for this pseudo-C.

All retained207F logical clusters in the inventory have now been reviewed.
This does not cover every original ASM export: absent graphics/DOS/signed-runtime
functions, caller reconstruction and emulator validation remain. No expensive
model was invoked and no copyrighted external assets were changed or staged.

Next bounded block:1E56 border descriptor/tile callers and their WORD/FAR views,
then explicit framebuffer-address cleanup in0DAB/1F3D wrappers.

## Sol: Border callers and explicit framebuffer addresses

1E56:0004..0280 reconstructed against clean ASM. DS:4FA4 is a FAR descriptor
pointer table, stride4; descriptor entries are WORD tile IDs. Entries0..3 are
corners. Starting at entry4, top/left/right/bottom edge patterns each terminate
with WORD00FF.01E7 repeats the current pattern to fill the edge, then scans its
terminator and returns the next pattern start in AX. The four calls now chain
that return value instead of substituting coordinates or restarting at entry4.
The scan still occurs for zero/negative signed WORD lengths, as native JGE
requires; missing/empty patterns remain unchecked, not silently repaired.

Tile addresses use byte-base4066/4068 plus ID*32 with WORD offset wrap and no
segment carry. The header's tileset is now FAR byte pointer, not DWORD array.
Descriptor reads likewise use WORD byte offsets. Origins/dimensions are retained
under their existing names, with comments identifying interior extents. The
only changed1E56 bodies are the border parent and edge helper plus local research
address helpers; other menu routines were not rewritten.

0DAB panel copy and1F3D:0086 EGA copy now explicitly use FAR A800:0000 ->
A000:0000.1F3D's viewport wrapper uses AC00:0000 -> A000:0000, X13,width27,
height200. Segment constants alone were not valid encoded FAR addresses.
Important ABI distinction:0086 DOES receive FAR file buffer plus four WORDs.
Its EGA branch ignores the buffer, but callers still push it and discard12
argument bytes. The branch reads rectangle arguments from BP+0A..10. That
buffer argument was preserved, not removed; deleted adapter conversion bodies
remain absent. Updated declarations reflect this distinction.

Verify-BorderCallerTranscriptions.ps1 passes8848 synthetic assertions covering
pattern repetition/next-start chaining, tile-offset wrap with segment retention,
descriptor WORD scaling and position advances. These address/sequence models
do not execute pseudo-C or inspect original tile assets; complete descriptor
inventory and live rendering remain follow-ups. Existing regression suites
also pass. No game assets were modified or staged.
