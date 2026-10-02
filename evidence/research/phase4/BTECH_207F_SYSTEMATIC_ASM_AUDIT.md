# Sol: systematic BTECH_207F C-to-ASM comparison — complete

Checkpoint2026-09-17:all82 retained implementations checked. First14 against
baseline `8fa75ec`; sprite0377 against `b17d544`; conversion0572/05BF against
`a555f33`; direction0971/copy0A76/upload0A9F against `2ca7b20`;
next20 retrace/input/random/map implementations against `45bb34d`;
next20 cache movement/framebuffer/overview implementations against `d30ff40`;
final22 rendering/string/runtime implementations against `6e2db93`.
76 locally matched under the limits below;6 checked mismatches. This completes
comparison of the retained implementations, not omitted native bodies or
all-FLAGS equivalence, compilation or gameplay validation. Executable C unchanged.

## Evidence

Private `BTech-Reko-expanded/BTECH.reko/BTECH_207F.asm` SHA256:
`024516E9D43F91D6D1B9DF4BCE40375FD75DABD9AA9AAD81A7F716EF3676A693`.
Read the entire native bodies/ranges listed below, including adapter dispatch
and shared exits, and compared meaningful arguments, stores, access widths,
arithmetic, branches, loops, IO order and returns. Compiler frames/register
restoration are abstracted, except DX's explicitly documented sound handoff.
Deleted non-EGA branches remain deliberate omissions, not restored here.

## Checked group

| Implementation / native range | Result | Purpose and checked operations |
| --- | --- | --- |
| Set_Text_Colours_2127 2127–21A7 | Matched EGA | Store foreground/background low BYTEs at246C:B772/B775; adapter2 skips packed-colour expansion. |
| EGA_Draw_Vertical_Pixel_Run_05D0 05D0–077F | Matched EGA | Stage four WORDs; mode2/mask; initial offset40*lowBYTE(Y0)+(X>>3); draw first pixel before signed end test. |
| EGA_Draw_Aligned_Span_0780 0780–08A0 | Matched EGA | Stage four WORDs, mode2/full mask; same low-BYTE Y offset; latch read/STOSB/LOOP count; zero count65536 writes. |
| EGA_Draw_Glyph_2251 2251–22A4 | Matched | Font246C:A661+glyph offset, destinationA000:row+column; eight mask/foreground/complement/background writes,40-byte row stride. |
| Copy_Strided_Word_Rows_0931 0931–0970 | Matched | Stage source segment025A; narrow word-count DL/row-count DH; MOVSW rows, source gap only, zero height BYTE256 rows. |
| Timer_8253_5 001C–002F | Matched | WriteB6 to PIT43; no divisor write. |
| PC_Speaker_ON_ptr_freq 0030–0050 | Matched | PIT42 low/high divisor bytes then port61 OR3, including divisor zero. |
| PC_Speaker_OFF 0051–0066 | Matched | Port61 read/AND FC/write. |
| PC_speaker_OFF_2 0067–007C | Matched | Same port61 gate clearing, distinct native entry. |
| PC_Speaker_XOR_ptr_freq 007D–00A8 | Matched | Store seed0252, port61 XOR2, WORD seed+9248 rotated right3, store evolved state and retain DX. |
| Sound_Freq_Countdown_Loop_00A9 00A9–00D0 | Matched | Store mask0254/minimum0256; CX=(DX&mask)|minimum; LOOP zero65536 iterations; DX untouched. |
| DOS_Select_Default_Drive_014C 014C–0162 | Matched replacement binding | INT21 AH0E with DL=drive; retained helper binds OS service, not a new host implementation. |
| DrawCall_Image_To_VGA_Memory 0260–0312 | Matched |8000 destination columns, four source BYTEs per column, high/low nibble mode2 writes and latch reads; final mask zero. |
| DrawCall_Read_EGAMemory 0313–0376 | Matched | SourceA800 at WORD(row*320+column); eight rows/four read planes, contiguous32 destination bytes, source stride40. |
| DrawCall_EGA_CharacterPos_0377 0377–0571 | Matched | Read sprite header, clip signed pixel coordinates, combine four planes with transparent colour0, preserve native early-exit cleanup port. |
| VGA_Inline_ASM_Loop 0572–05BE | Matched | Transpose four packed BYTEs/eight pixels into plane0/1/2/3; count>>1 groups, unguarded LOOP and offset wrapping. |
| EGA_Animation_Bitshift 05BF–05CF | Matched internal register model | SHL AL/RCL DH,DL,BH,BL consumes the high nibble; updated AL and four accumulator BYTEs represented by C return/array, not original stack ABI. |
| Get_Target_Compass_Direction 0971–0A25 | Matched | Packed-coordinate region correction, wrapped signed direction-sector tests, lookup BYTE/CBW result. |
| Copy_From__Memory_Source_to_Dest 0A76–0A9E | Matched | FAR REP MOVSW including zero count, incoming DF forward/backward and ordinary-RAM WORD overlap order. |
| EGA_registers_0A9F 0A9F–0B25 | Matched DF-clear | Fixed128-BYTE interleaved upload into32 consecutive A400 columns, plane selection and final map-mask restoration. |

## Native and shared contracts

- All arithmetic is native WORD/BYTE narrowing, unnormalised FAR offsets and
  logical named memory views, not host pointer-sized arithmetic or sequential
  packed structs. Matching is local semantic agreement, not compilation or
  successful emulator execution. EGA primitives are retained reference effects
  whose backend will be replaced in a portable version.
- Pixel/span loops intentionally lack clipping/validation. Vertical executes
  once even for reversed bounds; its loop counter retains full Y0 although the
  initial multiply uses only its low BYTE. Aligned span zero executes65536
  writes. Offset increments wrap without selector carry. No safe guard invented.
- Glyph native XCHG reads EGA latches while writing the requested colour. C's
  explicit volatile read/write models that hardware effect, not atomic exchange.
  Font bytes and colour state must remain stable during the eight rows; native
  caches colours in BL/BH whereas C reloads globals. Bitmask is left complemented
  last glyph row; no register restoration was added.
- Full-image input is a stable packed RAM source, separate from destination
  framebuffer. C reads the source BYTE before configuring its pixel masks;
  native reads it after the first latch read. The retained intended RAM-to-EGA
  contract makes that ordering abstraction equivalent, not arbitrary overlapping
  hardware-source aliasing. Final GC index8/data0 is intentional.
- Tile capture sets write mode2/mask0 before selecting read planes0..3 for
  each row. Exit read-map select stays3; no restoration is invented. Its last
  arguments are coordinates, not a source FAR address or selector.
- Row copying assumes DF clear, ordinary RAM/FAR buffers and WORD accesses
  that do not straddle offsetFFFF; it is not a generic CPU bus-boundary model.
  Both pointers advance two bytes per WORD with offset wrap. The supplied gap
  advances source only; destination remains contiguous. Width is low BYTE of
  WordsPerRow, height low BYTE of Rows, and height BYTE0 executes256 rows.
- The speaker toggle parameter is a delay SEED, not a control mask/frequency.
 007D's explicit DX binding must survive00A9; countdown works on CX only.
  Sound parent/local agreement requires that research register contract, not
  an ordinary C ABI. Each zero initial countdown runs65536 LOOP iterations.
- Drive selection's binding must reproduce the native low DL drive index.
  DOS return AL is unpromised because the retained function returns void.
  The eventual port replaces this service with original-asset source selection.

## Remaining work

Continue with retrace/keyboard/random0B40–0BC0, then procedural maps,
terrain/cache transfers, menus/strings and runtime arithmetic. The82 retained
definitions include extracted map/string research helpers; those need caller
instruction accounting, not invented original ABIs. Keep their registry cells
blank until checked. Historical reviews are not fresh confirmation evidence.

Verification uses the existing hardware/framebuffer static models, sound field
handoff checks and inventory freshness/comment-only invariant. Such models do
not execute this C or prove EGA/PC-speaker timing, gameplay or asset integrity.
No private EXE/listing/external asset is staged and no expensive review invoked.

Passed6,659 hardware witnesses,87,804 framebuffer witnesses,39 sound-field
checks and340 inventory checks. Inventory verifies312 current definitions and
comment-only C changes. `git diff --check` passes. These counts do not certify
the remaining62 implementations.

## Sprite renderer0377 — full comparison

Read the full0377–0571 ASM body, every clipping branch/shared cleanup, and all
four unrolled plane paths. Arguments are destination offset/segment (represented
as encoded DWORD in research C), source offset/segment, signed X/Y WORDs. Header
BYTE1 increments with BYTE wrap into row count0..255, BYTE2 is width in eight-pixel
columns; source pixels begin offset+4. Native reads BYTE1/2 with one LODSW. The C
separate reads are certified for stable RAM headers without a WORD straddling
offsetFFFF, not a generic CPU boundary model. Source and destination must not
overlap; some C source reads are reordered around port/destination operations.

Signed SAR3 gives floor(X/8). Negative Y rejects if WORD(-Y)>=rows; otherwise MUL
skips clipped rows with low WORD added to SI and high WORD left in DX. Remaining
height is rows+Y; successful top clip resets Y0. Bottom clip computes200-Y and
rejects signed negative or zero before an unsigned row limit. Left clip adds the
negative byte-column coordinate to visible width, adds WORD(-column*4) to SI,
rejects if that skip>=original source stride, then sets screen column0. Right clip
computes40-column, rejects <=0, and limits width unsigned. Original source stride
remains width*4, not clipped width*4.

Drawing adds lowBYTE(Y)*40+clippedColumn to the destination offset and repurposes
scratch0264 as40-visibleWidth destination-row gap. It sets GC mode0 and maskFF.
Shift remains original X&7, including left-clipped negative X. Every row resets
the screen-column counter and visible width. Four source plane BYTEs are ORed
into occupancy; shifted occupancy builds a WORD preserve mask with complemented
high/low bytes. Each plane selects read-map0..3 and sequencer write-map1/2/4/8,
ANDs destination with preserve mask and ORs shifted source. The second destination
BYTE is written only when the unsigned screen-column counter<39. This is why
rightmost-column spill is suppressed rather than wrapping into the next row.

The inner width LOOP and outer row decrement are both unguarded: malformed zero
width/height retains native65536-iteration wrapping behaviour. No safe bounds
guard or normalised pointer was introduced. End of row adds source stride minus
visibleWidth*4 and destination gap; offset-only WORD arithmetic preserves selectors.
Native BP is repurposed as screen-column counter during drawing; this is not a
missing global or stack-local reference in C.

Cleanup0568 unconditionally outputs0F02 to current DX. Successful plane drawing
leaves DX=03C4, restoring sequencer write map to all planes. Before drawing, early
exits retain incoming DX, or top-clipping MUL's high WORD. The C CleanupPort
models both; BUG-019 is preserved, not silently fixed. Read-map select stays3
after drawing; mode0/maskFF remain. No extra EGA restoration is invented.

All local paths match under these explicit memory/register/hardware contracts.
Synthetic bitwise/clipping/port witnesses do not execute C or reproduce full
EGA latches, validate malformed sprites safely, or demonstrate BUG-019's visible
impact. Its runtime observation remains separate.

Sprite checkpoint: passed159,581 focused sprite witnesses,132,772 existing0800
sprite-pointer checks and340 inventory checks; comment-only C invariant and
`git diff --check` pass. No new executable correction is required by this local
comparison. Next group: packed-plane conversion0572/05BF.

## Packed-plane conversion0572/05BF — full comparison

Read both complete native bodies, eight calls per iteration, source/destination
selector setup, both STOSW stores, LOOP and cleanup.0572 takes FAR source/FAR
destination and a WORD count. Source DS is bound from the incoming source
segment held in BX BEFORE BX becomes the plane0/1 accumulator. ES remains the
destination selector; SI/DI increments wrap independently, without selector carry.

Each LODSB calls05BF twice, first high nibble then low nibble. The helper is an
internal near RET operation on AL and DH/DL/BH/BL, not a FAR C call with stack
parameters. Four SHL AL operations each replace CF; the corresponding RCL
shifts that colour bit into plane3/2/1/0. Existing accumulator bits shift out
after eight pixels. Thus C's group-local zero initialisation does not change
any complete group output; it is not permission to zero accumulators on every
single helper call. C correctly retains accumulator state across the eight calls
and returns BYTE(AL<<4) so the second call consumes the former low nibble.

The first pixel ends in bit7 of each plane BYTE, the eighth in bit0. Stores
are little-endian BX then DX, hence BL/BH/DL/DH = plane0/1/2/3. C's four BYTE
stores reproduce ordinary RAM results of these two WORD stores, under a
non-straddling WORD destination contract; this is not a generic bus-boundary
or MMIO model. Intermediate flags and final clobbered CPU registers are not
public interfaces of the retained conversion API.05BF's mathematical/register
flow is certified through its actual0572 caller, not a new original array ABI.

Native SHR CX,1 forms the group count. For count>=2, floor(count/2) groups
consume/produce four bytes each; an odd final WORD is untouched. Counts0/1
enter the unguarded loop with CX0 and run65536 groups (262144 byte operations,
four full traversals of each offset-WORD space). The C do/decrement loop agrees.
No count/bounds validation was invented. This malformed-count behaviour is not
proved reachable gameplay and is not a safe extraction API promise.

All four source BYTEs of a group are read before either native store, so exact
in-place conversion is supported. Arbitrary shifted overlapping buffers still
may corrupt future groups: neither implementation is a memmove-like conversion.
Keep source/destination as ordinary stable RAM views; conversion does not use
EGA hardware ports despite the legacy VGA name. Existing animation caller
WORD0790 processes0F20 bytes; this is a units check, not ANM decoding validation.

Focused verification compares SHL/RCL to the C register model, independently
packs full group pixels by bit position, starts with nonzero accumulators,
checks all WORD group counts and verifies exact in-place/odd-tail behaviour.
These static models do not compile or execute the research C or original game.
No executable correction or expensive confirmation is needed by this comparison.

Conversion checkpoint: passed262,665 focused conversion witnesses,159,581 sprite
regression witnesses and340 inventory checks. Comment-only C and current body
hash invariants pass, as does `git diff --check`. Next group: direction0971,
then copy0A76/rectangle0A9F.

## Direction0971, WORD copy0A76 and tile upload0A9F

Read each complete native body, all region/comparison branches and exits.
No executable correction needed under these explicit contracts.

0971 stages its four WORD coordinates at0238–023E. Y difference subtracts low
BYTE(y1) from low BYTE(y0), sign-extends the result, then compares high-byte
F0 region nibbles: target region greater ORs WORD0080, smaller ANDs WORD007F.
X difference is WORD(x1-x0); greater target high BYTE ANDs WORD007F, smaller
ORs WORD0080. These corrections are not a corrected-byte CBW operation, and
must not be replaced by ordinary Cartesian subtraction across packed seams.
The C preserves that unusual but native distinction.

Signed negative differences are NEGated as WORDs for their magnitude, including
8000's wrapped self-negation. Four sector tests compare signed doubled WORD
differences against the other magnitude; equality includes the direction bit.
Flags8/4/2/1 mean north/south/east/west and index the16-BYTE lookup0240. Its result
is CBW sign-extended, not the raw flags or an unsigned direction. Coincident
positions set all four bits and use lookup entry15 (FF/-1 in this image). Modern
C negative shifts/signed overflow are not certified by this pseudo-C notation;
portable code must implement explicit wrapped WORD arithmetic. Static seam and
sector tests do not prove the entire coordinate representation on real maps.

0A76 loads FAR source DS:SI/destination ES:DI, WORD count CX and executes REP
MOVSW. It does not CLD: C's FLAGS research binding correctly selects +2/-2.
Zero count copies nothing. Each WORD is read completely before its store;
overlap proceeds in the inherited direction, not memmove-safe automatic choice.
Pointers wrap offset WORDs without changing selectors. The split BYTE model
certifies ordinary RAM transfers without a WORD straddling offsetFFFF, not
generic MMIO/CPU bus-boundary behaviour. No promise is made for final clobbered
CPU registers or processor flags beyond the retained native ABI abstractions.

0A9F has only source offset/segment and destination offset: there are no width,
height or stride arguments. It writes GC mode0/maskFF/enable-set-reset0, fixes
destination selectorA400 and repeats sixteen groups of TWO byte-columns. Each
column selects sequencer planes1/2/4/8 and performs latch read then MOVSB; DEC DI
after planes0/1/2 keeps their destination address unchanged. After plane3 the
destination advances one. For DF clear this exactly matches the C's128 source
reads and32 consecutive per-plane destination BYTEs. No40-byte screen-row stride
occurs: this is packed tile-storage upload, despite the earlier rectangle summary.
Final sequencer0F02 enables every write plane; mode0/maskFF/set-reset0 remain.

Native MOVSB inherits DF, and unconditional DEC DI makes a DF-set execution
quite different (destination retreats7 per column and source retreats4).
The retained C matches the intended DF-clear contract only; this routine does
not itself enforce it. Do not infer all-FLAGS equivalence or silently insert CLD
in a preservation pass. The other forward-only native string/sprite/capture/
conversion models also require DF clear unless explicitly modelled otherwise.
Source is stable ordinary RAM separate from the EGA destination; port/latch
effects are hardware reference contracts, not host framebuffer writes.

The two inaccurate human outlines were corrected without renaming methods or
changing executable C. Focused witnesses cover signed comparison flags, region
correction cases, both copy directions/overlap/wrap and unrolled tile upload
addresses. They are static models, not execution of C or original gameplay.

Checkpoint: passed330,036 focused direction/copy/upload witnesses (including
profile-checked lookup bytes),6,659 hardware witnesses,49,980 runtime arithmetic
witnesses and340 inventory checks. Comment-only C/current body-hash invariants
and `git diff --check` pass. Next: retrace0B40, keyboard0B8A and random0BC0.

## Twenty-implementation checkpoint: retrace through local map expansion

Read all15 native bodies, including shared exits, and checked five extracted
research helpers against their inline caller instructions. Result:18 local
matches and2 mismatches. No executable C changes. Method summaries now distinguish
terrain descriptors, adjacency selectors and the expanded24-by24 tile view.

| Implementation / native range | Result | Checked operations |
| --- | --- | --- |
| Wait_For_Retrace 0B40–0B72 | Matched platform binding | Mode1 waits opposite level then rising edge; every other BYTE waits falling edge; repeated port3DA bit3 reads; native DX preserved. |
| Keyboard_GetKey 0B8A–0BA6 | Matched platform binding | INT16 AH0; AL zero selects negated scan BYTE; CBW sign extension including high-bit ASCII. |
| Rand_0x00_to_0xFF 0BC0–0BFA | Matched | SHR/RCL/CMC/SBB/SHR/RCR carry chain, three state BYTEs, reverse write order and XOR result;4FC3 untouched. |
| Map_NextConstructionSeed_Research inline0D07/0D79/0DF8 | Matched extraction | WORD index09F9, wrapped09FB offset and low-BYTE-only increment preserving index high BYTE. |
| Map_Noise_Research inline edge calculations | Matched extraction | BYTE mask2*amplitude-1 and wrapped seed/mask/subtraction; no added zero-amplitude guard. |
| Map_PerturbedMean_Research inline midpoint writes | Matched extraction | WORD mean plus BYTE noise; wrap before high-bit clear to zero, not saturation. |
| Map_VerticalAmplitude_Research inline0D79/0DF8 | Matched extraction | BYTE half-span SHR3, literal9 correction and SHL1, not division by lattice stride9. |
| Map_Pos_Struct_Loop_Operations_0BFB 0BFB–0D06 | Mismatch | Corner preservation,80-byte FF clear, reseeded outer edges, continuing interior seed and F0-masked64-byte output agree; missing native CLD effects. |
| Map_Pos_0D07 0D07–0D78 | Matched | Pair stack, span1 leaf, right-first traversal, WORD mean, horizontal amplitude and FF-only seed consumption. |
| Map_Pos_0D79 0D79–0DF7 | Matched | Pair stack, span9 leaf, bottom-first traversal and native vertical perturbation. |
| Map_Pos_0DF8 0DF8–0FED | Matched | Quad stack, centre mean, scratch bytes, top/left/bottom/right gates, retained DH and unusual right-edge amplitude. |
| Map_data_structure 104E–11BA | Matched local call flow | Centre minus11h,16-wide vertex stride, nine64-byte outputs, four corner copies, adjacency rebuild and saved SI/DI. |
| Map_Memory_Research caller DS binding | Matched extraction | Logical relocated246C:0000 view, not an allocation or independent native method. |
| Map_fn207F_11BB 11BB–12B9 | Matched register model | All64 cells; W8/S4/E2/N1 equality, direct selectors and top/bottom interior80h exception. |
| Map_fn207F_12BA 12BA–12D8 | Matched register model | Left wrap-39h, right+1, high-bit shared12D3 exit; C return represents BL. |
| Map_Struct_internal_Ops_12D9 12D9–12F1 +12D3 | Matched register model | Left-1, right wrap+39h and shared direct-selector exit; parent separately reads AL's descriptor value. |
| Map_Struct_Internal_Ops_2_12F2 12F2–1302 | Matched register model | North-88h INC BL, south+8 OR4; BYTE increment wrap. |
| Map_Struct_Internal_Ops_3_1303 1303–1313 | Matched register model | North-8 INC BL, south+88h OR4; caller AL/BL represented as parameters. |
| PosXY_OffsetGrid 1314–13D8 | Matched local call flow | Full position WORD stores, low-BYTE block shifts, nine coordinate pairs/metadata stores,8-row output-band strides and saved DI. |
| PosXY_GridSegment 13D9–158B | Mismatch | Cache wrap, terrain/template branches, reflection, tile-ID transformation, eight strided rows and combined metadata agree; missing native153A CLD. |

### Two queued mismatches: direction-flag side effects

0BFB explicitly executes CLD before its REP STOSW lattice clear and again before
the LODSW/STOSW descriptor output.13D9 executes CLD at153A on every output path.
The forward-only C loops give the intended bytes regardless of incoming DF,
but neither updates the research `flags` binding. That is observably incomplete
because the retained0A76 WORD-copy model reads that binding to choose direction.
For incoming DF set, native exits with DF clear while these C models leave it set.
Add explicit DF-clear modelling in the correction pass; do not invent a game bug
or change native behaviour. TODOs are beside both method bodies.

104E and1314 match their own calls and staging, not the complete behaviour of
their mismatching children. Their registry scopes retain that dependency limit.
Other processor condition-code/clobber details remain abstracted unless part
of an explicitly represented caller handoff; this finding concerns an existing
represented input to another C helper, not a request to emulate every CPU flag.

### Construction arithmetic and memory contracts

0BFB clears80 bytes and restores all four corners of the81-byte lattice.
Top/bottom/left/right outer edges each set seed-index WORD from a BYTE endpoint
sum; the interior continues the right edge's index. Output takes the upper-left
8-by8 cells, excluding shared bottom/right borders and preserving descriptor bit7.

0D07/0D79 push both child endpoint pairs before testing the midpoint. A value
other than FF consumes no seed. Inline native seed increments follow the lattice
store, whereas the extraction increments before it; equivalence requires the
intended separate lattice, index/table and work-stack buffers.0DF8 similarly
interleaves child-stack construction with edge calculations, while C batches
children afterwards. Those buffers and scratch0272–0278 must not alias.

0DF8 centre uses a four-corner WORD mean without noise/clamp. Left noise remains
in native DH, even when the output DL is cleared by the high-bit check. Bottom
and right replace only DL before WORD addition/shift; the C explicitly retains
that high byte and WORD wrap. Right-edge amplitude uses residual half-width,
not half-height. Both asymmetries match ASM and must not be cleaned away as
decompiler damage. This comparison does not establish their gameplay impact.

### Adjacency and local tile expansion

11BB's native unrolled code treats80h unusually: top/bottom interior columns
fall through to neighbour equality after subtraction produces zero; corners
and middle-row cells use direct selector zero. Boundary offsets39h and88h cross
64-byte block columns and192-byte cache rows respectively. Native INC BL for
north equality is preserved rather than generalising it to OR1. The register
helpers expose meaningful output BL through C returns, not original stack ABIs.
The sole parent1886 reloads SI/DI before each call, verified as a dependency;
11BB's final register offsets are not exposed by the parameterised C model.

1314 expands nine8-by8 blocks into the24-by24 buffer07AD and writes nine BYTE
metadata values07A4–07AC.13D9 forms phase from wrapped BYTE X+Y, redirects FF/8
coordinates to adjacent cache blocks, and reads selector plus terrain at+240h.
High-bit terrain bypasses reflection/tile translation. Terrain10h uses fixed
template209D/base70h; other nonzero terrain subtracts10h as BYTE and clamps
values at/above31h to30h. Reflection uses vertical213D and horizontal211D lookup
tables for selectors and IDs below40h, with spatial row/column reversal into20DD.
Fixed IDs40h and above remain unchanged. Each output row writes8 bytes then skips
16, with final DI=inputDI+8. Return is raw selector OR terrain metadata, not a
rendered tile ID. Template, reflection scratch and destination are intended
separate stable RAM buffers; no arbitrary overlap or MMIO guarantee is claimed.

No private assets or extracted game bytes are added by this checkpoint.
Synthetic regression results are recorded below; they are not C execution or
interactive gameplay validation. Next: descriptor regeneration/cache shifts,
then native map movement158C onward.

Checkpoint verification:11,268 focused random/seed/DF assertions (including an
expected divergence witness for the uncorrected CLD omission),6,659 hardware,
14,449 construction and23,115 cache assertions passed. Inventory verification
passed312 annotated definitions/340 checks and the comment-only C invariant.
Registry entries use current body hashes; `git diff --check` passed.

## Twenty-implementation checkpoint: cache movement through overhead packing

Read all18 native bodies and checked two extracted helpers against the four
movement callers.17 locally matched under the following contracts;3 checked
mismatches, all omitted represented DF clears. Executable C remains unchanged.

| Implementation / native range | Result | Checked operations |
| --- | --- | --- |
| Map_RegenerateDescriptor_Research inline movement callers | Matched local adapter | Four-corner copy, descriptor SI/lattice DI setup and0BFB call; offset preservation abstracts parent reloads. Child DF omission excluded. |
| Map_ShiftDescriptorCache_Research inline158C–1885 | Mismatch | Four descriptor-copy schedules, three vertex/output offsets, all-nine adjacency rebuild, pending BYTE slot/region tables agree; native shift CLD side effect omitted. |
| Map_Position_Move_North_158C 158C–163A | Matched local parent | DEC AL/JS, page0 gate, page-10h/local7F and new northern row; child shift/build DF omissions excluded. |
| Map_Position_Move_South_163B 163B–16E2 | Matched local parent | INC AL/JS, pageF0 gate, page+10h/local0 and southern row; child omissions excluded. |
| Map_Position_Move_West_16E3 16E3–17C4 | Matched local parent | DEC AL/JS, page0 gate, page-1/local7F and western column; child omissions excluded. |
| Map_Position_Move_East_17C5 17C5–1885 | Matched local parent | INC AL/JS, page0F gate, page+1/local0 and eastern column; child omissions excluded. |
| Map_NineGrid_1886 1886–18D7 | Matched register model | Nine source0564/target0324 block pairs at64-byte strides, fresh offsets per call. No origin/position writes. |
| Struct_Copy_Operation_18D8 18D8–18EE | Matched register model | Four ordered BYTE corner transfers, vertex stride16 to lattice stride9; no offset-register advance. |
| Copy_Data_To_GraphicsMemory 18EF–1AA7 | Mismatch EGA | Port setup, origin, parity cropping, tile schedule and scratch flags agree; native1959 CLD side effect omitted. |
| Graphic_Memory_To_Destination 1AA8–1ACD | Matched EGA/DF-clear | Zero DL, tile DH,1C83 call, retained parent destination/cache index. |
| Graphic_Memory_To_Destination_1ACE 1ACE–1AF3 | Matched EGA/DF-clear | Zero DL,1BDF lower-half call, retained parent values. |
| EGA_Memory_1AF4 1AF4–1B19 | Matched EGA/DF-clear | Zero DL,1B71 full-width/variable-height call, retained parent values. |
| EGA_Copy_A400_Memory_Word_Or_Byte_To_Destination_1B71 1B71–1B93 | Matched EGA/DF-clear | WORD source>>3, BYTE height test,16/8 rows, two MOVSB and destination gap38. |
| EGA_MemoryCopy_Byte_1BDF 1BDF–1BFB | Matched EGA/DF-clear | Source>>3 plus16, eight rows/two MOVSB/gap38. |
| EGA_Copy_A400_Memory_Word_Or_Byte_To_Destination 1C83–1CB7 | Matched EGA/DF-clear | Bottom/side/height BYTE tests, one MOVSB plus source INC and destination gap39. |
| Map_NineGrid_Parent_1DA8 1DA8–1DBA | Matched FAR adapter | DS246C binding,1886 call, saved caller registers; no origin update or descriptor regeneration. |
| Update_Cached_Map_Origin_1DF8 1DF8–1E36 | Matched | Low-BYTE half-coordinate/mask7/+2; WORD row*24 at09F1, column09EF and sum09ED. |
| Combat_1ECE 1ECE–1F03 | Mismatch | FAR buffer/WORD direction, zero export versus any-nonzero import, segment/index swap and288 load-before-store MOVSW agree; native CLD omitted. |
| Overhead_Map_1F04 1F04–1F50 | Matched DF-clear | Centre descriptor/adjacency block, terrain thresholds,10h dedicated40h case,64 outputs/40-byte stride; no native CLD. |
| Pack_Dynamic_Overhead_Tile_1F51 1F51–1F9B | Matched DF-clear | WORD subtraction/swap/shifts, BYTE AH addition, wrapped source, two XLAT colours per output,32 packed bytes at642B; no native CLD. |

### Cache movement and extraction limits

Movement operates on local BYTE AL: DEC/INC followed by JS, not full-WORD
coordinate decrement/increment. Non-crossing and blocked world-edge paths do
not clear DF. Page transitions use Y steps10h and X steps1. Invalid position
BYTEs are not normalised or checked beyond the native branches.

The northern shift uses192 backwards MOVSW from06E2 to07A2, then CLD. The southern
shift uses192 forward MOVSW from0624 to0564. West copies middle-to-right before
left-to-middle,32 WORDs each in all three rows. East copies64 WORDs per row
middle/right-to-left/middle. The BYTE models preserve resulting intended RAM
and known overlap behaviour; they are not generic MMIO/WORD-bus emulation.
Vertex offsets use WORD arithmetic from0B0B+region, minus11h/plus0Fh/minus0Fh as
appropriate; pending region values separately use BYTE wrap. Newly exposed slots
are north0/1/2, south6/7/8, west0/3/6, east2/5/8. All nine adjacency blocks are
rebuilt after generating the three new descriptor blocks.

The shift extraction omits the represented final DF clear from native STD/CLD
or CLD. This is one mismatch in the shared extracted implementation, not four
independent misread coordinate gates. The parent statuses are explicitly local
and exclude that child mismatch, plus the already queued0BFB DF omission.
The regeneration adapter saves registers differently from a literal native
entry to provide C parameter semantics; caller offset dependencies are retained,
not every native register clobber. No separate native export is invented.

### Viewport and leaf-transfer contracts

18EF's EGA branch writes mode2/mask0 and composes from A400 into AC00 starting
byte-column13. The27-byte-column viewport uses13 two-column tiles plus one
single-column side tile, over12 sixteen-row bands and one eight-row band.
Odd X takes the initial right half; even X takes a final left half. Odd Y begins
with bottom-eight source rows; even Y ends with top-eight rows. Cache rows
consume14 cells then skip10. Destination gaps265h/125h give640/320-byte advances.
Half/side/bottom scratch BYTEs and final band count agree with native.

Native1959 CLD is omitted by the renderer. Leaf wrappers themselves do not CLD;
their forward models are matched only with inherited DF clear, EGA adapter2,
configured latch-transfer mode/mask, stable tile data and normal logical video
memory addressing. They are not host framebuffer operations. For DF set,
two-MOVSB leaves retreat source and use a different destination row advance;
the one-column leaf's MOVSB retreat cancels its explicit INC SI. Do not silently
insert CLD into those leaves or certify all-FLAGS equivalence. Parent BX/DI
preservation is represented by value parameters; native register clobbers are
not additional host-state writes.

Combat_1ECE always executes CLD before its576-byte transfer, unlike these leaves.
The C preserves native WORD read-before-store overlap ordering and FAR direction
selection but does not clear represented DF; this is the third queued mismatch.
Ordinary RAM and WORD accesses not straddling offsetFFFF remain the established
copy contract. No memmove-style overlap reversal is introduced.

### Overhead representation

1F04 reduces descriptor values, not graphics pixels:10h becomes40h without
adjacency OR,20h..8Fh subtract10h and OR adjacency, other values below90h OR
adjacency unchanged,90h..FFh pass through. Input centre blocks0664/0424 are
contiguous; output at244B+caller offset writes eight rows separated by32-byte
gaps.1F51 resolves source through the literal full-WORD transform; only observed
BYTE IDs90h..FFh simplify to644B+(ID-90h)*64. Two BYTE-indexed XLAT reads map
tile IDs to colours; first shifts four bits, second ORs unmasked, final BYTE
narrows. Arbitrary lookup values are not forcibly masked to four bits.
Both native routines inherit DF, so local matches require DF clear and intended
stable RAM source/lookup/output buffers. No asset data is embedded in this audit.

Next: screen clear1FBE, rectangle/tile/menu rendering and string/runtime helpers.

Checkpoint verification passed87,804 framebuffer,23,115 cache,14,449 construction
and11,268 random/seed/DF assertions. The DF witnesses reproduce the expected
uncorrected omission, not an implementation fix. Inventory passed312 definitions/
340 checks and comment-only C invariants. All60 registry body hashes are current;
55 local matches/5 mismatches. `git diff --check` passed. No gameplay validation.

## Final22: screen/rectangle/tile/menu rendering, strings and runtime arithmetic

Read the20 native implementations, including EGA dispatch/shared returns and
numeric tail-entry3C08, plus two extracted string helpers against inline ASM.
21 local matches under explicit contracts; one new mismatch in numeric3C15.

| Implementation / native scope | Result | Checked operations |
| --- | --- | --- |
| Graphics_Set_Screen_To_Black 1FBE–200D | Matched EGA/DF-clear | Mode2/maskFF, A000 offset0,8000 WORD stores/16000-byte extent, inherited sequencer/rotate state; no native CLD. |
| EGA_DrawBox_Operation 245C–24D6 | Matched EGA wrapper | Two FAR addresses/four WORDs, staged pointer components/rectangle fields, unchanged EGA X/width and24D7 call. |
| DrawCall_EGA_DrawBox_24D7 EGA24EB–2568/shared2629 | Matched EGA/DF-clear | Wrapped bounds/clipping, rejected origins, low-BYTE Y multiply after gate, mode1, REP MOVSB rows/stride and zero-count semantics. |
| DrawCall_SingleTile 275C/EGA284D/shared2819 | Matched EGA/DF-clear | FAR source selector store, mode0/maskFF/set-reset0, WORD screen origin,8 rows/four plane stores/latch reads and final0F map mask. |
| DrawCall_Combat_Menu 2B87/EGA2CB0/shared2C7B | Matched EGA | Staged origin/width, WORD colour-low-BYTE, XOR/mode2/maskFF,8 scanlines/unconditional width LOOP, explicit INC DI and reset rotate index3/data0. |
| DOS_Text_ScanRemaining inline3B22/3B68/3B9E | Matched extraction/DF-clear | FFFF maximum BYTE scan, NUL consumption, updated WORD offset and remaining CX. |
| DOS_Text_CopyBytes inline3B22/3B68 | Matched extraction/DF-clear | Alignment BYTE copy/count decrement, WORD pairs read-before-store and carry-derived trailing BYTE. |
| Append_Text_To_Memory 3B22–3B67 | Matched DF-clear | Destination/source scans, destination terminator overwrite, source count including NUL, source alignment and original DX:AX destination return. |
| Append_Large_Text_To_Memory 3B68–3B9D | Matched DF-clear | FAR strcpy, source count including NUL, destination alignment and original pointer return; not a large-text/append mode. |
| Loop_Until_TextPtr_Null 3B9E–3BB4 | Matched DF-clear | REPNE SCASB, NOT/DEC count, AX WORD length, FFFE result if no NUL found. |
| ASM_Text_Formatting_3BB6 3BB6–3BD1 | Matched local wrapper | BL1, radix WORD, decimal CWD/nondecimal zero high, FAR destination/tail jump; child CLD omission excluded. |
| CBill_Text_Formatting 3BD2–3BDB/shared3C08–3C14 | Matched local wrapper | BL1, DWORD value, FAR destination, radix WORD; signed decimal long interpretation; child omission excluded. |
| Check_Input_For_Character 3BDC–3BEB | Matched DOS binding | Incoming DS:5366 high BYTE, buffered00FF fast path or INT21/AH0B, AH cleared; no consume/print. |
| ASM_fn207F_3C15 3C15–3C6A | Mismatch | Optional decimal sign, two WORD DIVs, BYTE digit mapping, NUL/reversal and original-pointer return agree; native3C18 CLD omitted. |
| abs 3C6C–3C80 | Matched | Signed WORD compare/NEG, minimum8000 wrapping back to itself. |
| Install_DOS_Interrupt_24_Handler_3C82 3C82–3CA4 | Matched replacement binding | FAR callback words to incoming DS5368/536A, install CS3CA5 at vector24 using DOS AH25, AX0 return. |
| ComStar_Stocks_multiplication 3D1C–3D43 | Matched RAM adapter | FAR lvalue/DWORD factor,3E2E call, returned DWORD stored; native RETF8. |
| rotate_with_carry_do_while_parent 3D44–3D6B | Matched RAM adapter | FAR lvalue/DWORD divisor,3E62 quotient, final DWORD store; native high-then-low store order abstracted, RETF8. |
| rotate_with_carry_loop_parent 3D6C–3D90 | Matched RAM adapter | FAR lvalue/WORD count, register DX:AX shift call, returned DWORD store, RETF6. |
| If_Arg_multiplication_return_func 3E2E–3E61 | Matched native arithmetic | Zero-high fast path and three WORD partial products/low32 result. |
| rotate_with_carry_do_while 3E62–3EC2 | Matched native arithmetic | Divisor-high0 two-DIV path; paired DWORD SHR/RCR normalisation, estimate/product carry/unsigned correction and quotient DX:AX. |
| rotate_with_carry_loop_and_return 3EC4–3ECE | Matched native arithmetic | XOR CH, JCXZ and repeated SHR DX/RCR AX; low BYTE count, not host modulo32 shift. |

### Rendering and string limits

Rectangle copying is not box-edge drawing. X1/Y1 fields represent width/height.
Clipping uses wrapped unsigned WORD sums, then rejects X>=40 or Y>=200. Native
MUL DL uses low BYTE Y, equivalent after that gate. Zero width has empty REP;
zero height underflows through65536 rows. No validation policy is added.
The EGA rectangle and screen clear inherit DF; matching requires DF clear.
Single-tile EGA dispatch does not CLD although deleted adapter branches do.
With DF clear its four MOVSB/three DEC DI sequence writes one common byte-column
per row, then advances40. Stable separate source data and configured logical
video-memory/latch contracts apply; no deleted adapter implementation restored.
The EGA menu uses explicit load/store/INC rather than string instructions and
is independent of DF. Width0 executes65536 writes per scanline; colour is the
low argument BYTE, and the BYTE return is that colour. Final data-rotate0 is
preserved. No clipping/safety guard or sequencer restoration is invented.

String helpers scan at mostFFFF bytes, consuming NUL where present; no terminator
returns the native exhausted count, not an error. FAR selector is unchanged
when WORD offsets wrap. strcat aligns source, strcpy destination, then MOVSW
loads each complete WORD before its store. The BYTE extraction excludes WORD
bus accesses straddling offsetFFFF and generic MMIO. These are forward DF-clear
models with native overlap order, not validated buffer-safe/memmove operations.
If the copy extraction were called with count0/odd alignment, native BYTE copy
then DEC underflows; no extra guard is implied. It is not a new native export.

### Numeric conversion, DOS and DWORD helpers

The WORD wrapper sign-extends only decimal radix10; other bases zero-extend.
The money wrapper supplies both WORDs and interprets bit31 as decimal sign.
Core sign handling uses wrapped NEG/ADC/NEG, including minimum signed DWORD.
Two WORD divisions maintain quotient/remainder; digit mapping uses BYTE ADD30h,
comparison39h and ADD27h with native BYTE wrap. Zero emits one digit. NUL precedes
the mandatory first reversal swap; the original FAR destination is returned.
Radix0 divide fault and radix1/nonzero nontermination are not corrected here.
Native3C18 CLD clears DF for both output and later callers; the C omits that
represented flag write. Record the core mismatch once; wrapper statuses are
local staging/call matches, not full conversion-flow confirmation.

Input-status binding checks a runtime buffer then DOS standard input, not BIOS
queue polling. Handler installation stores callback selector then offset in
native RAM, while C reverses those independent stores. Its local contract is
ordinary stable RAM plus an OS service binding; not volatile store-order or
critical-error trampoline validation. Omitted3CA5/3CD8 runtime bodies remain
unreviewed dependencies and DOS replacement candidates, not retained methods
silently counted as checked.

DWORD here means two native WORDs: unsigned-long multiplication retains low32
bits; division returns quotient, not remainder; shift count retains low BYTE.
Divide assignment stores high then low in ASM; typed C assignment confirms the
final RAM value only, not hardware-visible ordering or a host pointer ABI.
Native arithmetic uses wrapped widths and divide faults; portable C/C# must
make those policies explicit instead of inheriting wider host operations.

## Complete-file result and correction queue

All82 retained implementations now have current explicit audit records:
76 locally matched and6 mismatches. The six missing represented DF-clear
effects are0BFB,13D9, extracted cache shift158C–1885,18EF,1ECE and3C15.
Each has a `Sol:TODO ASM mismatch` beside its body. Review these together with
the explicit DF contracts of forward-only string/EGA models before correction.
Non-crossing movement paths and leaf copies must not receive invented CLD calls.

No executable C changed, and no private game assets/listings were added.
Completion concerns retained methods under documented contracts, not all native
entries, compiled functionality or emulator/gameplay validation.

Final checkpoint verification passed1,646 EGA,6,443 text,49,980 arithmetic,
87,804 framebuffer and11,268 random/seed/DF assertions. The DF witness remains
an expected-divergence test, not a correction. Inventory passed312 annotated
definitions/340 checks and the comment-only C invariant. All82 registry body
hashes are current,76 locally matched/6 mismatches, and the ASM profile hash
is unchanged. `git diff --check` passed. No gameplay validation claimed.
