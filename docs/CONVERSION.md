# Sol: preservation conversion log

### Complete failed jailbreak Mech startup scene — 2026-09-18

Converted original11B8:16B2..1761 into
src/Original/BTECH_11B8_FAILED_START.c, re-reading the full expanded ASM. This keeps
the specialized O0.ANM prefix rather than substituting the general scene player:
select disk1, load3E80 bytes, clear exactly1E78 frame-workspace bytes, reset frame
and native stream offset42F6, decode at least once, inspect the NEXT control BYTE,
then failure sound14,60 retraces and layout4/sidebar restoration. Restored the
native signed JL49 comparison; controls80..FF are negative and continue the loop.

Tests run the actual complete scene with controlled disk/file/decoder/hardware
boundaries. All256 next-control values exercise the signed termination rule,
including the do/while minimum decode. Check exact clear range, file survival,
frame/stream resets and full presentation call order. These do not substitute
for local O0 decoder/artwork or visible emulator comparison.

All97 headless and115 SDL/local-asset suites pass. The actual startup executable
link now exposes11 unresolved symbols, down from12, but this reduction is not
proof that only11 original methods remain. Larger parents may expose more
callees. Round mechanics, exploration interactions, statistics, component
salvage, weapon UI, effects and sound dispatch remain among the link gaps.

### Real executable link gate and original presentation gateways — 2026-09-18

Added src/SDL/main.c as the platform-only entry, calling the existing original
Setup_Game then closing the SDL backend. The executable target is excluded
from the default build until missing original methods link. Build.ps1 -Game
checks that target after the ordinary build/tests; no production mocks fill
gaps. docs/PLAYABLE_BUILD.md records command, scope and actual unresolved symbols.
The first real link exposed15 missing symbols, including small wrappers
previously supplied by unit-test boundaries.

Converted complete1CD3:17C6/17EA/1809 and0800:19BF, checked against expanded ASM:
standard message layout7/sidebar/border0, text then timed input bridge without
an extra blocking read, the affordability string with its native final period,
and WORD sound-setting gate forwarding the original WORD ID unchanged.
SoundEffectsEnabled's original initialized WORD1 moved from its method object
to BTECH_DATA.c solely to avoid pulling unrelated settings/UI implementations
into boundary tests. Value, ownership and use sites remain unchanged.

Tests verify presentation order, exact text pointer and punctuation, no
invented waits, all65536 sound IDs under zero/nonzero/high-bit/high-byte WORD
settings. The original1FC5 dispatcher remains pending, not stubbed. All96
headless and114 SDL/local-asset suites pass. The real executable still fails
with12 unresolved symbols; this is evidence of incomplete preservation work,
not a playable-build claim. Parent conversions may expose more missing methods.

### Original arithmetic combat position restoration — 2026-09-18

Converted the complete0800:186F..191A into
src/Original/BTECH_0800_COMBAT_POSITION.c, checked against every expanded ASM
instruction and branch. This is not a direct coordinate assignment and does
not call cache-shifting/map-loading helpers. It finishes unsigned Y comparisons
before X, retaining WORD decrement/increment and native packed-region masks/
carry additions. A noncanonical target can be skipped and return at the next
canonical boundary; unreachable targets retain possible nontermination.

The controller test now links this real method rather than its former direct
assignment boundary. Separate tests cover32768 canonical source/target pairs
across every region on both axes,3840 ascending/descending noncanonical cases,
and unchanged cache bytes/origin WORDs. Test expected canonical endpoints use
independent logical-cell packing, not a duplicated stepping implementation.

Re-read complete0DAB:0002..04F8 and its annotated component-salvage parent for
the next conversion. Confirmed weapon IDs10..1F zero16 SS bytes atBP-24..BP-15;
ID20 accessesBP-14, which is not assigned elsewhere in this method. Its initial
value cannot faithfully be replaced by a zero-initialized ordinary local or a
host uninitialized byte. A defined original stack-frame representation is still
needed to retain this SRM-6 bug. The method remains pending rather than inserting
a safe salvage replacement or a production no-op. This review is not a claim
that component salvage has been ported.

### Complete encounter controller — 2026-09-18

Converted all of original183B:000A..1481 into
src/Original/BTECH_183B_CONTROLLER.c from the annotated parent, re-reading the full
expanded ASM. Retained original setup/formation writes, consent and forced
engagement, settings, manual/AI planning, mechanics handoff, all betrayal
branches, enemy withdrawal, scripted Kurita messages, post-combat salvage/
healing, training/arena outcomes and common NPC restoration. No newly invented
combat engine substitutes for these branches.

Applied the audit discrepancies:3402/341B retain CR;34B8 retains its period;
character-name indices explicitly sign-extend native BYTEs. Native formation
activation increments WORDs rather than assigningTRUE, saved outer coordinates
and round coordinates now have distinct names, and plans/orders use their
existing sizes/sentinels. Kurita's five message strings were extracted from
the EXE-owned FAR table3EDB:3A2E. Arena outcome binds the real persistent BYTE
D32D. Friendly personnel withdrawal remains distinct from enemy flight.

Defined host initial values for consent, arena escape latch and Kurita message
cursor avoid compiler path warnings: native assignments precede observable
use under valid caller paths. KuritaAttackFlag must remain stable in a combat
encounter; turning it on after setup would expose original unassigned stack
state, which is not represented by a host uninitialized read.

Tests call the actual whole parent with controlled callee boundaries. Cover
no encounter/unreachable force, declined/forced engagement, exact1..3-round
dispatch, ordinary retreat, training and arena results, death, salvage gates,
allnine terrain veto bytes, injuries, BUG-017 dead technician qualification,
on-foot betrayal and common restoration (including which animation entries
are intentionally untouched). These are orchestration tests, not full round
mechanics or emulator validation. Whole-Mech salvage tests check invocation,
not replace its already converted original implementation.

Still incomplete: original1AE8:000C mechanics,0800:186F position restore and
0DAB:0002 component/armour salvage are declared pending, with NO production
stubs added. The static library can compile without pretending a playable
executable links. Raw signed character-name indexing requires a valid original
name-table entry; raw negative weapon-target IDs require the native neighbouring
3092 bytes, particularly BUG-016's FAR sprite data, not host pointers. Current
controller contract excludes those unrepresented addresses without sanitizing
or masking the original signed index. Those native-memory edge cases remain
unresolved and must not be certified by these normal-path tests.

### Combat side-wide computer planning — 2026-09-18

Converted the complete original183B:1482..14C2 handoff in
src/Original/BTECH_183B_COMPUTER_GROUP.c, checked instruction-by-instruction against
expanded ASM. Both parent calls supply a side start (0 or12). It walks twelve
actors, reads each current WORD active flag, and calls the existing original
1631:03AB planner with previewFALSE. This includes infantry, not just Mechs;
any nonzero WORD qualifies. No precomputed active snapshot or actor-index clamp
was introduced. Valid native caller ranges remain the contract.

The test runs this actual dispatcher over all8192 friendly/enemy active masks,
using high-bit and high-byte-only flags, verifies exact call order/arguments and
untouched opposite-side flags, then tests changes made during the first planning
call. The single-actor planner is a controlled boundary in this dispatch test;
its original implementation is independently tested elsewhere. This is not
an end-to-end combat sequence and does not complete the large183B encounter
controller or1AE8 mechanics parent.

### Complete random encounter generation — 2026-09-18

Converted the entire original0DAB:0D3D..1466 into
src/Original/BTECH_0DAB_ENCOUNTER.c, following the annotated parent and the expanded
ASM. This is not a replacement spawn algorithm or a research helper. The
22-entry weapon table is EXE-owned3EDB:2CF4; the three FAR templates at2DF8
bind directly to the original Locust/Wasp/Stinger records. External artwork
and levels remain local and are not embedded.

Preserved four origin RNG calls, eight independent infantry chances, seven
weapon rolls and seven skill rolls per generated infantry, four ordered dice
calls, four Mech chances even when friendly counterparts are absent, complete
125-byte template copies, and untouched character/actor fields. The placement
loops remain separate, with their original scan widths, transition without
wrap testing, three-unit Mech spacing and packed-coordinate carry arithmetic.
WORD subtraction truncates before signed arithmetic halving; terrain threshold
comparisons are signed, correcting the annotated unsigned extrapolation.

BUG-005 remains: both Mech terrain reads occur before negative rejection,
neighbouring cells may cross a row, and no search row cap or upper tile bound is
introduced. Reads use the whole contiguous native map storage, including
already reconstructed neighbours. Addresses must stay within the represented
246C:02D3..A44E window; unrepresented native memory is not fabricated. All-blocked
terrain may loop indefinitely as in the binary, and is not exercised by running
an unbounded test.

Tests call this complete parent with real records/templates and controlled
RNG/dice/map-anchor boundaries. They exercise all22 weapon sums, all3 templates,
four origin sign combinations, exact roll consumption, seven skill bytes,
preserved charisma and friendly actors, missing friendly Mechs, blocked-tile
scan advancement, cache-edge discards and BUG-005 row crossing. The test fixture
uses a controlled cache origin rather than claiming a whole gameplay capture.
All92 headless and110 SDL/local-asset suites pass. This does not constitute
emulator validation or a finished playable preservation build: combat parents,
weapon UI and full executable integration remain to be converted.

### Native adjacent map runtime storage — 2026-09-18

Encounter0DAB:0D3D's placement loops must retain BUG-005: terrain reads precede
negative-index rejection and have no upper bound/row cap. The former independent
576-byte CombatMap member cannot express reads into its native neighbours through
an ordinary C subscript. This is a representation prerequisite for that complete
routine, not a safe/clamped replacement placement algorithm.

Joined the ALREADY represented original246C:02D3..A44E cache,09ED/09EF/09F1
origin WORDs,09F3/09F6 queue BYTEs,09F9 seed WORD,09FB..244A EXE-owned static map
data,244B..A44A graphics workspace andA44B/A44D party-position WORDs into one
MapRuntimeStorage union. Existing named globals/arrays are views of the exact
same fields, preserving their values/lengths and all method call sites. Original
static map initializer is unchanged inside the aggregate. Only this known window
has storage; no fabricated prefix/unknown FAR-memory contents or game algorithms
were added. Native little-endian WORD representation remains the target contract.

Compile-time assertions fix every boundary and exclude padding. Synthetic tests
verify literal native addresses, all256 patterned cache/static/workspace contents,
WORD byte order, queues/seed/coordinates and backward terrain reads into the last
terrain flag/descriptor BYTE. Defined whole-object BYTE access avoids invalid
host subarray arithmetic while retaining the original memory adjacency. Existing
map, save/load, rendering and startup suites remain green:91 headless and109
SDL/local-asset suites. The encounter parent itself is NOT converted yet; its
complete ASM body and terrain search will be the next step. Native accesses
outside this represented window, wrap into unclassified memory and arbitrary
corrupt inputs are not certified. Full combat/story integration remains pending.

### Full building-script dispatcher and arena terrain — 2026-09-18

Converted original1CD3:0004..17C5, all47 one-based script actions, as ONE
parent method using the annotated template and the complete exported ASM body
including its jump table/shared tails. Restored typed globals/EXE tables and
readable exact EXE dialogue/control bytes. Incorporated queued discrepancies:
Mech name INITIAL accesses, zero-gated training fire cleanup, signed calibration
comparison, native right alignment and full WORD stock rows, actual FAR text
table bindings, arena WORD position/active reads and raw2F's Health deduction
instead of TrainingFlags. Raw0 and48..FFFF are rejected by the native wrap gate.
Labels describe dialogue, balance, repair result and shared sprite/crew tails.
Action/constants use training, finance, equipment and mission terminology.
Cross-checking the already converted1467:0002 exposed an obsolete annotation:
raw27 passes resetAssignments=TRUE, not character slot1/Rex. Its original callee
clears ALL party/Mech assignments before the crew UI. Sprite rebuild instead
passes FALSE to synchronize existing seats. Port names describe this real flag;
historical1CD3 audit/action-summary slot wording is superseded here.

Retained debit-before-distribution, unsigned DWORD clamp/wrap, BYTE skill wrap,
excellent-level equality, reused D31A count/slot/purchase-result storage, medical
mask2 rather than canonical TRUE1, hospital insufficient-funds credit25, armour
point-by-point/partial payment, arena escape's original-record relocation or
full-lance discard, NPC staging and signed randomized health CAP. No modular
replacement service functions or annotation research helpers were introduced.
Ending flow uses original asset loaders/decompressors and separate SDL graphics
contracts; external image/level content remains local and ignored.

Also converted complete11B8:137F..1440 and1441..152E terrain helpers against
ASM. Corrected the remembered selector to246C:0010, not annotated3092:0010.
Four setup variants, exact patch endpoints/interiors and teardown RNG order
remain; zero/unknown selectors change nothing, and no backup, selector reset
or cache refresh was invented. Terrain constants identify positions, strides,
lengths and original tile IDs; patch tile artwork itself is not embedded.

Tests call the full dispatcher with original record/EXE table storage and actual
number/text formatting, substituting its external method boundaries only. All47
actions are explicitly exercised, along with every invalid WORD action. Cases
cover high-bit/DWORD money and transfer wrap, exact/refused purchases, all BYTE
specialist skill increments, school purchases/refusal, medical fees/refunds,
partial armour costs, every signed Body/Health injury tuple, 16 arena saved-name
patterns, marching/redraw timing, text alignment/full WORD rows, NPC staging,
laser health caps, final Health mutation and ending call/buffer order. Separate
real terrain-helper tests verify all256 RNG seeds, every affected/unaffected
map BYTE and all65536 teardown selector values. 90 headless and108 SDL/local
asset suites pass. Caller-boundary tests are NOT an end-to-end playable story
or combat certificate; those parents/integration remain unfinished.

Native-valid table/menu/name/character indices, stable buffers and DF-clear are
the represented calling contracts. Arbitrary signed-invalid indices need native
adjacent memory, not sanitized host arrays. The DOS CPU-speed calibration remains
replaced at the SDL runtime boundary: the original WORD has storage and its
signed gate is tested, but no host spin benchmark manufactures a DOS value.
Combined visible ending/training/menu pacing still needs emulator comparison.

### Complete overhead terrain and marker renderer — 2026-09-18

Converted original0800:3FAE..45C1 using its complete exported ASM body and
the annotated template. Retained five-by-three region assembly, signed BYTE
metadata coordinates/extents, central dynamic descriptor allocation, disk
selection/open retries, two reads at the same scratch pointer, ignored read/
close errors, original reduction and dynamic tile routines, MSB-first fog,
objective direction glyph, footer, marker blink and keyboard draining.
The demo loop performs601 retraces, preserving its post-decrement timeout.
Party coordinates restore before marker/input polling; viewport controls fog
and terrain independently. No replacement terrain algorithm or production
helper was added. TinylandTileset's original global definition moved to the
data object to avoid pulling unrelated startup code into renderer tests.

The exact98 EXE-owned metadata BYTEs are one contiguous table: thirteen WORD
read spans precede eight X and eight Y glyph coordinates. Map14's span reads
the first two glyph-X BYTEs (20,23), yielding0x1714 rather than an invented
fourteenth zero span. Cache copying retains0x1080 bytes, including128 beyond
the4096-byte tile subregion in the existing contiguous template storage.

Whole-method tests use real original reduction, packing, planar conversion
and text/number formatting, with test-only file/map/input/drawing boundaries.
They check all256 fog BYTE patterns, exact visible-cell order and positions
for all256 coarse viewport regions, metadata/file sequence and scratch writes,
open retry, ignored error returns, fifteen dynamic allocations, Cache copy/
background, direction glyph, visible/offscreen markers, three interactive
polls,601 demo polls and the Map14 span overlap. 88 headless and106 SDL/local
asset suites pass. These are synthetic boundary tests, not visible gameplay
validation. Arbitrary corrupt/high-bit map IDs, invalid metadata extents,
FAR-offset wrap and incoming DF-set conditions remain outside the represented
native-valid storage contract. Full story/combat parents and a playable game
executable remain unfinished.

### Original overview reduction and dynamic tiles — 2026-09-18

Converted whole207F:1F04..1F50 descriptor reduction and1F51..1F9B colour packing,
plus0800:45C2..4620 maintained EGA-only wrapper, against complete ASM bodies.
Centre descriptor/adjacency64 BYTEs reduce into eight40-byte-pitch rows;
category10 bypasses adjacency to tile40,20..8F subtract10 before OR,90..FF
pass through. Two XLAT colours pack each output BYTE, including unmasked
high-bit lookup values. Retained literal WORD subtract/swap/shifts/AH40
addressing for native-valid dynamic BYTE IDs90..FF. Workspace244B provides
scratch644B and packed642B directly; no new buffers or research helpers.
Adapter2 converts16 WORDs in place through the original planar converter.
Removed CGA conversion remains absent, matching the maintained annotations.

Corrected converted Setup_Game's missing GraphicsAdapter=2 assignment from
the maintained EGA-only original main. Leaving EXE initial0 had selected the
wrong adapter branch for dynamic overview tiles. Startup suite now verifies2
before entering hardware initialization on all drive-option paths.

Tests cover every256 descriptor and256 adjacency combination, exact row gaps,
all112 dynamic IDs with256 patterned sources/lookup values,32 packed outputs,
preceding byte preservation and four adapter branches with independent EGA
bit-plane oracle. 87 headless and105 SDL/local-asset suites pass. Arbitrary
WORD IDs outside the dynamic BYTE range and FAR-offset wrapping remain outside
represented workspace contracts. The parent0800:3FAE terrain/marker renderer,
full story/combat parents and playable executable integration remain missing.

### Complete overhead map controller — 2026-09-18

Converted original0800:3D40..3FAD against the complete274-line exported ASM
body. Party marker coordinates remain fixed across renders; scrolling globals
are restored from separate viewport locals after every non-Space key. Native
two-region steps and unsigned north/south/west/east limits, diagonal gates,
pre-destruction/Cache single-key exit, Cache fog D178 bit7 clear and colour-
table swap calls retained. World restoration rebuilds nine regions, CBW-loads
nonzero map BYTEs with linear region bounds, resets three pending SLOT bytes,
rebuilds tile cache and redraws exploration/UI in original call order. Cache
paths skip world reload and swap back only after OverheadMapActive clears.
No extracted navigation helper or substituted renderer added.

Tests execute the whole controller with renderer/map/drawing boundaries and
real original key conversion. All65536 WORD coordinate values for eight
directions verify viewport changes and restoration, plus all256 key BYTEs in
single-screen story/Cache modes, Space exit, fixed marker, signed map arguments,
pending resets, fog, cache swaps and six redraw calls. 86 headless and104
SDL/local-asset suites pass. Original0800:3FAE terrain/marker renderer is still
missing, so this is controller verification, not a rendered playable overview.
Remaining story/combat parents and executable integration remain incomplete.

### Save-to-load original-controller round trip — 2026-09-18

Added a distinct save/load round-trip gate executing BOTH converted original
controllers. Save_Game's actual four writes populate one synthetic file image;
then every saved live BYTE and both camera coordinates are destroyed before
Load_Game consumes that same image through its four original read calls. All
six slots and256 patterned state seeds round-trip all3908 saved state bytes,
both packed coordinates, C-bills/stocks/Mechs/personnel/NPC/effect overlaps,
signed outtake selection, derived Citadel/holodisk flags, transient security
clear and unsaved menu-byte preservation. No new production code or serializer.

85 headless and103 SDL/local-asset suites pass. File transport and map/render
calls are explicit synthetic boundaries in this gate: it proves controller
compatibility and restore behaviour, NOT actual SDL-disk round-trip, original
save gameplay, or full playable game. Original zero-byte marker residual-stack
case, story/combat parents and executable integration remain unfinished.

### Complete original load controller — 2026-09-18

Converted original0800:32B3..35D2 against its complete329-line ASM listing.
Exact EXE menu/text controls, shared filename, disk/media retry, missing-file
path and marker BYTE test retained. Valid marker0C restores one0F44 state
block and two coordinates through original DOS/SDL read interfaces, clears
33 unsaved security flags, CBW-restores outtake menu state, derives current
Citadel/holodisk flags and calls original cache entry or nine-region map
construction/loader/compositor. Map numbers are CBW, region IDs remain linear
and zero/missing neighbours are not cleared. Original four friendly Mech
sprite/frame/direction/selector/cursor resets, traitor-warning clear and stale
cache tileset replacement occur on successful AND invalid-marker paths.

Preserved invalid-marker file-handle leak and unchecked subsequent read counts.
Failed marker read supplying no BYTE remains dependent on uninitialized native
SS:BP-2; host indeterminate data is NOT a certified original-stack model. This
method currently requires that the marker read supplies a BYTE. Do not describe
truncated/zero-length saves as verified or initialize the marker to an invented
default. Explicit residual-state treatment remains required for that case.

New tests execute whole Load_Game with file/menu/map/render boundaries over
seven selections, all256 marker BYTEs, missing-file gate,0..2 media retries,
eight cache/Citadel/holodisk flag combinations, native raw state, signed map
BYTE arguments, all nine grid slots, invalid-marker leak/reset, exact clear
flags/cursors and common exit. 84 headless and102 SDL/local-asset suites pass.
These are synthetic controller tests, not real save-to-gameplay integration.
Shared original filename storage moved into original data object to avoid
linking Save_Game merely to access its global. Full story/combat parents and
playable integration remain incomplete.

### Complete original save controller — 2026-09-18

Converted original0800:35D3..378C after reading the complete exported ASM body.
Restored exact EXE-owned menu rows and Game0 filename. Original map-room BYTE
gate, six-slot/Cancel selection, slot filename BYTE arithmetic, disk3 selection,
INFOCOM media-presence retry/request/close and binary create mode8101 with
permission180 retained. Four original writes are marker0C, contiguous0F44
state bytes, packed X WORD and packed Y WORD: total0F49. No serializer,
alternate file format or gameplay helper added. Existing DOS IO entry points
already redirect through SDL.

Preserved all four writes after any write failure, ignored close status, error
message controls06/0F, final menu/sidebar/disk1 exit on every path, and the
native adapter0 exception selecting GREEN2 for failure (other adapters RED4).
The latter was missing from the annotated template's unconditional red.

Tests execute complete original Save_Game with explicit file/menu/UI boundaries:
six slots plus Cancel, all16 write-failure masks, create failure,0..2 media
retries, both colour branches, exact four buffers/counts/order and3913-byte
native image, ignored failing closes, state unchanged and all255 nonzero
map-room BYTEs. 83 headless and101 SDL/local-asset suites pass. The new suite
uses only synthetic state and file boundaries; it does not alter original
local assets/saves. Load_Game and larger story/combat parents remain missing.

### Original contiguous saved-state storage — 2026-09-18

Consolidated original C614..D55B storage into one typed union backing, with
native0F44-byte saved span C614..D557 and four adjacent unsaved menu BYTEs.
Characters, existing WorldMapState, C-bills/stocks,16 unclassified economy
bytes, NPCs and effects retain their existing names through direct views.
This is storage layout, not a new gameplay method or serializer. Compile-time
offset/size checks require native character/world/economy/NPC/effect/cursor/
menu addresses with no inserted padding. Current raw transfer target is little
endian; the new test checks that explicitly. External files are not embedded.

Restored a previously missing alias: effects D457..D45F are the final nine
bytes of NPC7's native record. Name/inventory saved aliases remain within that
same NPC record. Corrected NextMapEffectSlot to its actual BYTE D557, adjacent
to MechSlotByMenuRow[0] D558; original0800:4DC7 reset's native MOV byte clears
only D557. The former independent WORD's invented high byte is gone. Existing
new-game test now verifies the real D558 menu BYTE survives rather than a
separate simulated high byte. Future methods doing native WORD operations
at D557 must load/store both real BYTEs explicitly, not use unaligned host
pointers or silently reinterpret this cursor as an independent WORD.

Added raw/typed memory tests for all256 patterned state images: original single
block copy updates all views, unsaved menu bytes survive, every overlapping
NPC/effect BYTE works bidirectionally, little-endian cash and typed gameplay
writes appear at exact raw offsets. All82 headless and100 SDL/local-asset
suites pass, including existing repair/salvage/recruitment/world workflows.
Save_Game/Load_Game conversion can now use the original raw block directly;
those methods and the larger story/combat parents are not yet implemented.

### Graphics runtime startup redirect — 2026-09-18

Original0DAB:0C8F..0D11 hardware/runtime method now redirects to the separate
SDL backend. This is deliberately an OS/hardware replacement, not an ASM-
equivalent instruction-speed calibration. SDL opens presentation/audio once,
resets EGA controller state to replacement mode defaults, invokes the existing
native16000-byte screen clear and presents black. DOS file-buffer registration,
BIOS adapter modes, busy-loop calibration, compiler graphics runtime and
interrupt24 registration are not emulated. Existing Setup_Game now has its
real hardware startup dependency, rather than a missing implementation.

The platform suite calls the original method on a closed and existing display,
checks a previously drawn glyph is cleared, mode map-mask/ROP reset and plane
contents, then continues original input/file/sound checks. All99 SDL/local-
asset suites pass. The game is still not executable/playable end-to-end.

Next load/save prerequisite discovered during dependency inspection: native
Save_Game writes one0F44-byte block C614..D557, but current host Characters,
WorldMapState, cash/stocks, NPCs and effects still have separate backing. Do
not copy0F44 bytes from Characters or add a different save format. Consolidate
the original adjacent storage: Characters C614..C723, WorldMapState C724..D36F,
CBills D370..D373, three stocks D374..D37F,16 remaining bytes D380..D38F,
eight26-byte NPCs D390..D45F, effects D457..D556 overlapping the final NPC
record, and cursor BYTE D557. Current NextMapEffectSlot WORD view also reaches
MechSlotByMenuRow[0] D558; preserve that alias explicitly rather than creating
an independent high BYTE or relying on unaligned host uint16_t pointers. The
save block excludes D558. Then convert full original load/save methods with
their four raw transfers and native error paths. Random encounter generator
also needs native cache-before/after tile reads preserved (BUG-005), not a new
bounded spawn search. Full story/combat parent methods remain next major work.

### Original unsigned decimal entry — 2026-09-18

Converted complete1543:0CDE..0EE5 after reading its full exported ASM. The
FAR3092:0012 scratch string is the existing DynamicString, not a new buffer.
Preserved saved text cursor redraw, adapter0 blue/other green colour, updated
cursor row clearing with WORD coordinate wrap, signed key/length comparisons,
seven-digit gate BEFORE stripping zeroes with original overlapping strcpy,
Backspace-to-empty, Escape reset without cancellation and Enter acceptance.
Decimal arithmetic uses uint32_t for native DX:AX multiply/add/subtract;
digit loads retain CBW/CWD meaning. Final text colour is white15, not the
caller's previous colour. Restored original GraphicsAdapter WORD3EDB:4FBA
with EXE-confirmed initial0; hardware detection remains a platform concern.

Tests execute the original complete editor and its real original string-copy
and length methods. Explicit keyboard/text/row-clear boundaries verify each
call and cursor/colour state. All65536 input WORDs, ignored high-bit keys,
adapter colour gates, leading-zero stripping,9999999, eighth-digit rejection,
editing at capacity, repeated empty Backspace, Escape then continued input,
scratch bounds and wrapped row-clear coordinates pass. 81 headless and99
SDL/local-asset suites pass. These are UI behaviour tests, not interactive
shop gameplay validation. Main story/combat parent conversion and playable
executable integration remain incomplete.

### Complete Rex recruitment and Kurita ambush setup — 2026-09-18

Converted original11B8:104E..137E against the complete exported ASM body and
annotated template. Exact seven skills and overlapping X/Y spawn views come
from EXE-owned3EDB:1E7A..1E91. Recruit name is loaded, incremented without
cycling, then assigned to Rex; fixed attributes/equipment, seven skills and
on-foot state retain original order. Commando copy is bytewise125 bytes.
Preserved missing full-Lance guard: slot4 receives the hidden enemy Commando,
and saved-name D452+4 aliases D456, replacing NextRecruitNameId with'C'. The
encompassing native NPC record byte view expresses that write without C array
overrun. No gameplay guard or repair was added.

Original setup saves eight NPC positions, hides enemy names only, clears24
position/active/casualty entries, activates Jason/Rex and clears just their two
action BYTEs. Three initial RNG calls choose origin and two-to-five soldiers;
each soldier changes six skills (Medical untouched), Body/Health and weapon
in native call order, with signed coordinate BYTE loads and hardcoded walk
cursor2FE8:02E0. Native FFFF prearranged-combat handoff remains unchanged.

Tests run the whole setup with RNG/dice and combat entry as explicit boundaries:
all16 vacancy masks, four enemy-count rolls and256 RNG/name values; exact
Commando record including full-Lance corruption, recruit wrap/fixed skills,
NPC backup before RNG,24 reset positions, shared soldier fields/coordinates,
unchanged Medical/armour, animation bindings and RNG/dice counts. 80 headless
and98 SDL/local-asset suites pass. This confirms setup, not actual combat;
original combat parents, full story dispatcher and playable integration remain.

### Original jailbreak wrapper and Stinger award — 2026-09-18

Converted complete11B8:152F..16B1 after reading the full exported ASM body.
Preserved seven companion name BYTE backups (slot0 untouched), four friendly
Mech name-initial backups, entry0D10:7024, Jason's on-foot assignment, original
Mission9 call, escape0D00:7014, name-only restoration and clearing stored Mech
initials toFF. Nonzero survival WORD awards the exact125-byte reference Stinger
to the first vacant friendly slot. A full Lance receives nothing. The method
does not embark Jason, assign a pilot, or restore other fields changed during
the mission. Native FAR3092/2FE8 record bindings become their actual existing
typed arrays, not near-pointer pseudo-code or copied research structures.

Tests run the complete wrapper with Mission9 as an explicit boundary: all16
vacancy masks, survival0/1/8000, all256 companion name bases, exact full-record
comparisons, untouched saved-name slot0, name-only restoration and persistent
mission mutations. 79 headless and97 SDL/local-asset suites pass. This is not
an end-to-end escape or combat gameplay validation; full story dispatch,
combat parents and executable integration remain unfinished.

### Complete mission enemy generation — 2026-09-18

Converted original0FDC:0D49..134A against all584 exported ASM lines and the
annotated template. Original NPC position staging precedes clearing all24
position/active/casualty entries. Enemy records are hidden by their name BYTE
only. Preserved signed infantry-deployment gate, both consumed training rolls
before jailbreak/fixed-X overrides, arena band's conditional extra RNG call,
2D6-2 template selection and rental's subsequent3..6 override, Kurita four-Jenner
replacement, pilot writes BEFORE bytewise template copy/damage RNG, signed
severity clamp, eleven armour bytes, five critical attempts and engine/sensor/
gyro RNG order. Rental's nonzero-slot override always writes spectator SLOT1,
not the current slot; no modernized correction was made.

Restored all11 EXE-owned template FAR bindings/sprite bytes and three-byte
spectator loop. Preserved native hardcoded enemy walk-stream addresses02A0/
02E0 rather than reading mutable friendly cursor tables. Spawn X1642 and Y164E
are overlapping views12 bytes apart; armour loss165A is another overlapping
view. Shared31 BYTE storage preserves infantry record8..15's real offsets,
including the final Y positions reading armour-table bytes. No independent
zero-filled coordinate arrays were invented. Infantry generation shares its
eight records with pilots, changes six skill BYTEs but not Medical, and retains
untouched body/armour/etc fields for pilots/unused records.

Tests execute actual generation with original templates/animation/data and
controlled RNG/dice: all normal Mech/infantry count combinations, NPC backup,
all24 reset/spawn positions, stale fields, positive severity0..127/clamp6,
critical/system damage bytes and pilot-before-RNG order, all eleven2D6 chassis
entries, both arena bands/four rental choices/spectator, Kurita, jailbreak,
mission2 X override, signed8000 deployment, every infantry weapon RNG byte
and all2D6 Body/Health values. 78 headless and96 SDL/local-
asset suites pass. These are rules/state tests, not rendered combat validation.

Existing shared mission flags and action-state storage moved into original
combat data to avoid pulling entire unrelated parents when linking state.
Native-valid Mech count<=4, table roll and damage data windows remain caller
contracts; corrupt negative severity needs the wider native memory view, not
an invented clamp. Full combat parent/mechanics and story dispatcher still
prevent a playable executable and remain required for goal completion.

### Complete training/arena/jailbreak mission controller — 2026-09-18

Converted original0FDC:0629..0D48 against all821 exported ASM lines and the
annotated template, including all ten setup destinations and every shared
world-loop/outcome/cleanup path. Restored original EXE mission dialogue and
eight signed rubble coordinate pairs. Native WORD timer/countdown/attempts,
signed terrain SAR3, block-major rubble addressing, hangar bounds, four-phase
WORD NPC scheduler and keyboard/world-redraw order remain explicit.

Preserved mission4 fall-through/no-computer handoff, mission6 coin-flip and
mission7 forced Kurita attack, final16 tile replacement, disabled enemy's
actuator/ammo/movement writes, mission8's world-loop bypass, Locust inability
to pick up rubble, signed time test after Locust50 subtraction, skill increases
even on failure, Kurita survival flag's non-clearing behaviour, and common BLD
reload after BTSTATS displaces it. Jail combat uses old timer>=80 (81st update);
repeat attempts do not count; third DISTINCT parked Mech starts. No premature
hangar-return escape, modern mission cap or bug correction was introduced.

Controller suite drives all ten original paths: southeast pass boundary for
both chassis, all256 terrain penalties, immediate/eleven-idle-probe cadence,
eight rubble targets and both chassis, all256 signed skill values, combat
survival/withdrawal outcomes, computer-control timing, Kurita interruption
tiles, arena deployment, repeated jailbreak attempt and81st-update death,
and shared-buffer reload. Generator/combat/world/UI are explicit test
boundaries, not replacement production algorithms or gameplay proof.
77 headless and95 SDL/local-asset suites pass.

Moved the existing0090 DisableComputerControl definition into original shared
combat data so this global does not pull the planning UI into a controller
test. No storage meaning or menu behaviour changed. Mission numbers0..9 and
original data views are the caller contract; arbitrary invalid mission IDs
are not certified. Full enemy generation, combat parent/mechanics, story
dispatcher and playable executable are still outstanding.

### Mission return-to-world view — 2026-09-18

Converted complete0FDC:134B..13DD against all expanded ASM branches/calls.
Assignment BYTE8 alone selects Jason's infantry coordinates; every other byte
uses Mech0, even an assignment to another slot. Mech Y gets two WORD increments,
then bit7 gates the original0F80 packed-row normalization. Infantry Y has no
adjustment. Original four-call redraw chain remains intact; no actor, crew or
cache-origin helper writes were invented.

Actual body tested over all256 assignment bytes and65536 Y WORDs, using an
independent page/low-seven-bit carry oracle. Tests inspect four-call ordering,
restored camera, every actor coordinate and every character byte. Redraw
methods are explicit boundaries, not rendered mission validation. 76 headless
and94 SDL/local-asset suites pass. This removes a dependency of the original
mission parent; the full training mission and action dispatcher remain pending.

### Dispatcher dependencies: occupants and stock suffix — 2026-09-18

Converted complete0FDC:17B9..19F5 against all ASM branches/calls. Occupants
come from the real26-byte FAR roaming records and externally loaded names.
Preserved the last-WAITING-NPC waypoint Rick gate, even when that NPC is in
another building; Done/no-occupant returns leave the previous Rick flag alone.
Conversation choice uses compact marked-slot DEC/JNS traversal; destroyed
Citadel ignores selected identity and chooses RNG&7. Corrected missing second
space after the greeting's question mark. Restored all11 activity and8 reply
EXE-owned FAR text entries and the leading-space/period stock suffix1791.

The last-waypoint local has an unobservable initialization for host warning
cleanliness: any path reading it necessarily assigned it during the scan;
zero occupants return earlier. No residual native stack value is used here.

Tests execute the original bodies across all256 waiting masks/every valid
choice, eight off-building last-waypoint Rick cases, all256 reply RNG values,
missing exterior mapping, menu count, destination slot, preserved early-return
flag and stock suffix. Text/menu/input/RNG are explicit boundaries. 75 headless
and93 SDL/local-asset suites pass. These remove original dependencies of the
still-unconverted whole1CD3 dispatcher; they do not substitute a partial
dispatcher or establish playable original-script conversations.

### Building entry parent — 2026-09-18

Converted complete0FDC:0008..01BF against every expanded ASM branch/call,
including the signed ID gate and entry-scene CBW correction. Requested ID is
saved before alternate-script selection; recursive VIEWDISK overwrites that
global without restoration. Preserved the unreachable copyright-check block's
unconditional bypass, native disk/load/decode/interpreter calls, count-party
action14, both distinct holodisk chaining conditions, sidebar restoration,
and eight entrance fog writes followed by cache setup. No original bug fix
or stand-in story dispatcher was added. EXE-owned141A's26 scene bytes restored.

Expanded interpreter suite runs the actual entry parent, BLD loader/decoder
and interpreter together using synthetic encoded EXIT payloads. Covers all26
resolved script IDs, disk selection, every garage gate combination, BARRACK2
chaining without inventory/viewed checks, direct VIEWDISK identity, all256
entrance flags/all2048 fog bytes and all256 signed entry-animation values.
Native action14, rendering, input, file content and cache setup are explicit
test boundaries; the full action dispatcher and original-script gameplay are
not validated by this fixture. 74 headless and92 SDL/local-asset suites pass.

Alternate-building storage is still the16-byte loaded map view; signed IDs
or admitted map indices outside that view need the adjacent-memory contract
already identified in the ASM audit. Callers within that represented view use
the original signed logic, without inventing a clamp. Next: full1CD3 story
action dispatcher and remaining mission/combat parents, then playable startup.

### Building bytecode execution — 2026-09-18

Converted original0FDC:01C0..05F6, all28 E4..FF dispatch entries, against
the full expanded ASM and annotated template. Corrected the audit's operand
CBW mismatches, signed skill comparisons, signed state-table displacement,
DWORD C-bill wrapping/unsigned comparisons and conditional scene-argument
read. Kept unknown bytes as one-byte no-ops, native blocking input, unrestricted
script execution and original call relationships. Signed negative state indices
address the real preceding fog bytes through the existing shared storage.

Actual interpreter/WORD readers tested with all65536 money immediates,
all256 signed call/layout/scene operands, every opcode, both branch outcomes,
each living/dead party slot, negative state index/value and WORD menu-table
displacement wrap. UI/input/native actions are explicit test boundaries, not
production substitutes. 74 headless and92 SDL/local-asset suites pass.

Remaining address contract: normal scripts operate in the loaded payload and
declared original data views. Arbitrary FAR offset wrapping with a nonzero
payload base, positive state indices beyond99, or skills reaching outside the
character storage still require the wider native-memory representation; no
sanitizing clamp or guessed adjacent bytes were added. No shipped-script
reachability claim is made by these synthetic tests. The building entry parent,
full action dispatcher and playable story workflows remain to be connected.

### Cache overview swap — 2026-09-18

Converted complete135D:0327..03A9 against every ASM instruction/branch.
Swaps250 BYTEs with the cache or map-room palette, testing D34E for nonzero;
no rendering,256-entry swap or hidden reset. Extended the contiguous EXE-owned
map storage through246C:244A, retaining the real215D overlap with existing
templates, and initialized all new bytes from the expanded EXE. The256-byte
XLAT view consequently overlaps the first six bytes of the next250-byte table.

New test checks the original750-byte EXE table hash53BC240A, all256 room flag
values, every byte of the extended storage and double-swap restoration.
Existing map-cache hash still independently checks its original0C1D..217C
span. 73 headless and91 SDL/local-asset suites pass. This completes the cache
file's converted methods, not the unconverted0800 overhead presenter or the
game as a whole. Next major dependency is0FDC's building-script parent and
bytecode execution, followed by the still-missing full combat parents.

### Cache entry and secret passage — 2026-09-18

Converted complete original135D:0004..0287 from the annotated template and
expanded ASM. Cache entry saves only the four Mech name initials at3248,
hides those bytes without changing other Mech fields, temporarily assigns
all party members to Mech0, loads map14/artwork and presents the entrance,
then assigns everyone on foot before INSTRUCT.BLD. It replays opened doors
0..10, not entrance11, and does not itself change the cache story flag.
Retained the EGA pipeline; the omitted native palette call returns unchanged
for EGA. Secret passage copies the EXE-owned18 BYTE pattern20BC into six
three-byte rows with eight-byte destination pitch, redraws and displays the
original discovery text. No once-only guard was added.

New boundary test checks call order, temporary assignments, every unchanged
Mech byte, pending grid versus region fields, all eleven replays, and all4096
map tile bytes after repeated passage calls. 72 headless and90 SDL/local-asset
suites pass. Rendering, map assembly and script execution are explicit test
boundaries here; these tests do not prove playable cache gameplay. The project
still lacks the complete combat/story parents and a playable executable.

### Cache map-room transitions and star puzzle — 2026-09-18

Converted four complete original135D methods055A..0AB5 from annotated
templates and all exported ASM branches/calls. Maintained EGA-only map-room
entry/return, native buffers244B/4614 and tile field101D: upload decoded MAP
artwork before overwriting its first768 bytes with cache tiles; restore those
tiles before STARLEAG decoding overwrites the backup. Entry fills descriptors
D1 then center3x4 IDs90..9B, sets tileset3/room flag and threshold8B AFTER
presentation. Return fills D0 then center64 IDs90..CF, sets tileset2, clears
room flag and restores threshold21 AFTER presentation. Original CGA-only00D1
and0BA7 paths remain omitted, consistent with the maintained EGA source.

Parts location invokes FINDIT on every qualifying interaction. WHITE==1 alone
suppresses room entry; other BYTE values do not. Original entry forces one
exploration step/input and return does NOT restore the earlier movement-rate
setting. Recovered native text22E8 'painted' correction and full23B2 success
dialogue directly from EXE, plus seven EXE-owned WORD target offsets241E.
Star toggle retains BYTE INC/XOR1/DEC wrapping and native Y stride16, NOT door
stride32. Password is a membership set: required offsets need even parity
without category check; extra even star-category tiles in first768 reject.
Failure deselects selected stars but does NOT clear an already latched WHITE.

71 headless and89 SDL/local-asset suites pass under C17 /W4 /WX. Map-room
suite verifies full descriptor writes, untouched tile tails, all WHITE BYTE
gates, repeated parts script calls, exact backup/restore/decode/upload ordering
and flag/threshold presentation timing. Puzzle suite checks all128 required
subsets, every extra tile position in768-byte field, all256 category bytes,
parity-only targets, scan boundary, retained WHITE, all256 toggle values and
all16,384 local coordinate pairs against independent block/row addressing.
UI/file/decode/rendering are explicit boundaries in these new workflow tests;
existing local-asset suites independently exercise actual MAP.ICN/STARLEAG.ICN
loading and decoding. No new production scripts or platform fakes were added.
Full cache setup/overview/passage, original BLD parent and complete executable
remain pending; passing these suites is not a claim of live ending gameplay.
No copyrighted external artwork or level content was exported/staged.

### Original Star League door-code consumption and replay — 2026-09-18

Converted complete135D:0AB6..0D48 StarLeague_Key_Codes from the annotated
template, complete exported ASM, .dis and EXE tail. .dis and exact83-7E-0A-FF
bytes at0C61/0CB7/0D26 establish WORD-1 animation gates, correcting the old
annotated00FF constant. The truncated textual ASM's missing loop-back/epilogue
is confirmed by EXE bytes0D38..0D48 and .dis. Recovered all six12-byte
EXE-owned tile/position/required-colour tables246E..24B5 directly; no external
asset data was embedded. Original persistent door flags alias D34F..D35A.

Any negative argument searches coordinates; FIRST match terminates searching
even on failed colour validation. All required codes are CBW/DEC one-based
terminal numbers; selected codes are signed BYTE. Only EXACT-1 plays sound10,
three frames and20 retraces per frame; other negative arguments do lookup but
render only final frame. Nonnegative replay bypasses selected-code validation
yet consumes the requirements again. Entrance11 instead clears selected codes
and writes its adjacent D35A flag, although cache setup replays only0..10.
No consumed-code recheck or already-open guard was added. Packed tile addressing
retains native Y region stride32 and horizontal8-column/64-byte-block crossing,
with named constants replacing unexplained shifts/gaps.

69 headless and87 SDL/local-asset suites pass under C17 /W4 /WX. New door
suite checks all12 lookups/replays, repeated consumption, exact-1 versus-2 and
-32768, all seven wrong-colour combinations per ordinary door, no-match exit,
first-match rejection with a duplicate fixture, original texts/input ordering,
sound/retrace/frame counts, and complete4096-byte tile-field comparisons after
each frame against independent block/row addressing. Terminal suite now links
both actual original routines: imprint all three required codes, open door0,
then revisit each consumed terminal and receive its refusal. Rendering/input
remain explicit test boundaries; this is not live gameplay validation or a
claim the still-unconverted full ending/startup/combat parents are complete.
Copyrighted external files remain ignored/local and were never staged/exported.

### Star League cache terminal interactions — 2026-09-18

Converted five complete original135D routines0288/02A8/02D2/03AA/04AB from
annotated templates, checking complete raw ASM bodies0288..0559 except the
separate0327 overview-buffer swap (not converted here). Recovered33 X/Y/colour
entries and three colour-text FAR pointers directly from the expanded EXE;
these are EXE-owned constants, not external map/art assets. Persistent code,
WHITE, Phoenix Hawk, power and parts flags alias their actual D347..D34D bytes.

Phoenix discovery invokes ENDMECH only while its flag is zero, leaving writes
to the script. Gyro discovery preserves native double space. Power switch
uses WORD increment/even-coordinate mask and inclusive0C..11 Y bounds, with
repeated interaction allowed. Terminal scans all33 entries, accepts three
X cells, imprints zero-based code into selected colour without consuming the
one-use code, reloads colour after confirmation, and restores native CR and
06/0F text controls omitted in the template. Transmitter retains both locations
and prerequisite priority: power nonzero, WHITE EXACTLY1, parts nonzero;
WINSCENE runs before notification/transmitted-state writes.

68 headless and86 SDL/local-asset suites pass under C17 /W4 /WX. New terminal
suite checks every discovery-flag byte, all65,536 local coordinate pairs for
power and transmitter with varied upper packed bits, every terminal's three
cells and exclusions, refusal/used-code handling, colour reload and repeated
matches, every WHITE byte with representative power/parts bytes at both sites.
UI and currently unconverted original BLD parent are explicit test boundaries;
there is no production BLD stub and no claim the ending is playable yet.

Also re-read whole183B parent ASM000A..1481 and the weapon-menu stack setup.
The personnel-flight negative target lookup requires native FAR pointer words,
not host pointer bytes, and the full weapon list's strlen crosses unassigned
native frame bytes. A focused A-002/A-003 follow-up is recorded in
docs/investigations/ASTRA_REVIEW.md alongside mechanics/effects residual locals.
No expensive model run was started, guessed zero/fixed list length introduced,
or host undefined behaviour substituted for original native reads. These are
remaining implementation contracts, not reasons to declare preservation done.

### Original combat effects support — 2026-09-18

Converted entire1631:1DF8..1FDE (four original routines) and207F:1ECE..1F03
from annotated templates after checking every raw ASM branch/call/argument.
The effects position conversion steps unsigned packed coordinates across page
boundaries around the native26,12 screen anchor, updating the EXISTING shared
E486/E488 movement scratch globals. Direction selection retains signed WORD
negation (8000 remains negative), native tie rules and zero-vector octant6.

The original cache transfer is576 generated TILE bytes at246C:07AD, NOT the
descriptor cache0564 despite the old annotation summary. Any nonzero flag
restores; forward WORD load-before-store preserves overlap behaviour rather
than memmove. Native CLD is embodied by forward C accesses, consistent with
the existing preservation runtime, not a fabricated CPU research flag.
Restore/render wrapper retains exact call order: copy tiles, focus Jason's
assigned Mech or foot combatant4 using signed BYTE gate, animate tiles,
prepare framebuffer, render world. Sprite wrapper resolves one native FAR
sprite entry to one host pointer and passes signed pixel WORDs to existing
EGA renderer0377 at named MapViewportSegment. Non-EGA paths remain removed
as in the user's annotated source, not silently reintroduced.

67 headless and85 SDL/local-asset suites pass under C17 /W4 /WX. New support
suite checks1,441,792 signed octant cases against an independent signed-word
oracle,144 packed-world position pairs including all page-edge directions,
unaligned external buffer sentinels, nonzero restore flags, per-word overlap
in both directions, all valid Jason assignments0..8, exact render ordering
and every sprite-table entry's pointer/WORD coordinates. Renderer/camera/
animated-tile adapters are isolated in this suite; existing SDL suites test
their own implementations. This does not yet validate full attack playback,
which still contains native uninitialized stack state requiring an explicit
preservation contract rather than host undefined behaviour or a guessed zero.
No copyrighted external assets were embedded, exported or staged.

### Combat encounter map/probe/restoration handoff — 2026-09-18

Converted original183B:2835..2ADB, all three original routines, from the
annotated template and complete ASM bodies. Checked .dis/EXE for2AA3's
truncated textual ASM tail: actual final call is207F:1DF8, followed by return.
EXE-owned FAR table3EDB:0170 confirms nine sequential64-byte descriptor
blocks246C:0564..0764; no guessed pointer flattening.

Map loading retains linear world-region edge crossing, signed map-number CBW,
first descriptor90 as populated rather than map-identity test, skipped-slot
contents and unconditional nine-grid rebuilding. Reachability uses Jason's
foot/assigned-Mech position and FIRST active enemy; at most30 probe80 attempts.
Already at the destination returns false; arrival after a step returns true,
including the thirtieth attempt. Restores camera/cache only, leaving movement
scratch position/budget/facing/blockage state as native. No new pathfinder or
production placeholder was introduced.

66 headless and84 SDL/local-asset suites pass under C17 /W4 /WX. New isolated
handoff suite covers all arrival budgets, no enemy, already at target, first
enemy versus nearer later enemy, Jason's assigned Mech, signed map bytes,
populated slot skipping and world-edge wrapping. Existing movement integration
now calls the actual encounter probe, direction/step/terrain chain on clear
and fully impassable terrain. This caught an incorrect test expectation:
terrain rejection does not set the occupancy blockage latch; probe80 bypasses
occupancy. Cache/disk boundaries remain isolated in these tests. Synthetic
checks are not a completed combat controller or gameplay/emulator validation.
Full weapon UI, round mechanics and combat parent remain conversion work;
the preservation library is not yet a playable executable. Copyrighted local
assets were only read by the existing local test suites, never staged/exported.

### Whole original per-unit combat AI — 2026-09-18

Converted entire1631:03AB..0BB4 Combat_Computer_Control from annotated
template and all948 raw ASM lines; checked .dis's actualFFFF fallback operand
where ASM text misleadingly printsFF. One original method retains inline
target search, approach/withdrawal, close-contact facing and weapon targeting.
No extracted research helpers, new pathfinder or gameplay rules were added.

Saves/recenters/restores camera, clears all48 actor-order bytes, normalizes
enemy ID and sets friendly action state. Running is chosen only for exactly
zero-heat Mechs; personnel walk. Closest active opponent is selected with first
tie retained, native second distance call, rental13 exclusion and protected
traitor exclusion. Failed Mech scan falls back to personnel; absent/excluded
all-opponent cases retain the original unsafe further scan precondition.

Mech approach uses RAW component code as weapon-table index, not minus-one,
and packed short-range bits shifted3 (fourfold encoded short range). Personnel
decode the usual top three bits. Direct blocked/no-range destinations, fixed
axis approach steps and their packed boundary handling are preserved. Infantry
against Mechs withdraw six cells, setting the native side/flight flags; these
globals have provisional role names pending combat-parent integration.

Personnel assign slot0 only, with protected-traitor firing suppression. Mechs
at close contact cancel movement/update facing; all12 target slots clear,
available ten weapon ordinals receive the target and Kick slot11 is assigned
unconditionally below signed heat30. Destroyed weapon ordinal values still
count as available when not00FF. Heat>=30 cancels movement/targets. Actual path
generation always follows; preview is friendly-only when requested.

Tests cover side/record normalization, raw range policy, direct/approach orders,
ties, rental/traitor filters, fallback, withdrawal, every enemy heat BYTE value,
close-contact facing, blocked paths, ammo absence/destroyed ordinals and camera
restoration. Real budgets/distance/ordinal/heading execute; targeted dispatch
test isolates path/render boundaries. Existing combat-plan integration adds two
whole-AI scenarios through ACTUAL terrain clearance, budget, path generation,
direction, step and occupancy, with cache/display boundary explicit. Native
enemy-infantry budget-ID inconsistency is preserved/documented as probable
BUG022. All65 headless and83 SDL/local suites pass. No external assets added.
Next: larger combat execution and weapon menu/native-stack handling.

### Original hit-location text, missed-shot fire and step clearing — 2026-09-18

Converted complete1631:1B8F..1BFD armour-location display,1AE8:1E46..1EB3
missed-shot fire and1467:0D7E..0DC3 step clearing. Checked every raw ASM
instruction; .dis plus original EXE tail bytes confirm the latter two routines'
returns where ASM text truncates. Read all eleven native location BYTE keys
and FAR text targets directly from EXE-owned data. Hit text compares signed
keys to the full WORD, copies first matching name, appends period and uses the
original verbosity filter. Unmatched offsets leave scratch text untouched.

Fire consumes the actual2D6 routine. Rolls outside5..9 register sprite7C at
actor packed position plus WORD offsets; bit7-triggered X0F7F/YF07F masking
retains the native truncation, without replacing it with movement normalization.
Step clear writes exactly576 generated step BYTES to0, not02 sentinel and
not1152 destination-order bytes. Moved those existing two shared allocations
from preview code to original combat data. Heat/menu tests now use production
movement-order storage without pulling graphics dependencies; no duplicate
test state allocation is required. No new gameplay/helper algorithms added.

Tests cover all36 dice outcomes using real dice with scripted RNG, all65,536
WORD fire positions/underflow/masks, all eleven location names in all verbosity
modes, every unmatched WORD offset, signed keys/first duplicate and every step
byte cleared while destination orders remain unchanged. Original string and
message methods execute; effect registration/display are explicit test-only
boundaries. All64 headless and82 SDL/local suites pass. External copyrighted
assets remain local. Hit-location selection and wreck placement remain inline
in the larger original combat methods; no extracted substitute was invented.
Next: those larger combat bodies and native stack-dependent weapon menu.

### Original Mech destruction and occupant ejection — 2026-09-18

Converted whole1543:0A35..0C71 Combat_Mech_Eject_0A35 from annotated
template and complete raw ASM; read the two EXE-owned messages. Marks the
combatant inactive/casualty, clears only EXACT signed target references over
all24/twelve slots (high-bit references remain), normalizes enemy Mech record
IDs, saves the name initial and sets name[0] toFF without clearing the record.

Arena record4 destruction also destroys record5/disables combatant13, without
marking its casualty or saving its initial. Arena record5 invokes the original
Starport patch with FALSE: applies/saves, not restores. Destruction message is
suppressed for arena combatant13 or message mode None. Notification and
arm-shot-off WORD flags clear; final text colour returns to bright white.

Friendly destruction displays Men eject even with no occupants. Unsigned
pilot/rider IDs other thanFF dismount, become active and inherit the Mech's
packed location; rider is one column right, with native BYTE carry test and
full-WORD addition80. Same pilot/rider IDs let the rider placement overwrite
the pilot placement. Seat bytes remain stored. Timing uses the signed WORD
speed<5 gate and applies even with no occupants; no enemy ejection is added.

Tests cover all eight Mech records and exact/high-bit targets in every slot,
all81 pilot/rider/empty combinations, all65,536 packed X values including WORD
wrap, stored-seat retention, arena companion side effects, actual Starport
patch integration, message suppression and signed speed gating. Original
ejection, message filter and map patch execute; only display/timing are test
boundaries and RNG is forbidden. All63 headless and81 SDL/local suites pass.
No external copyrighted assets added. Full combat round and playable build
remain incomplete; next continue weapon/AI and damage execution dependencies.

### Original end-of-round heat and casualty display update — 2026-09-18

Converted whole1631:0C63..0F23 Combat_Mech_HeatLevels from the annotated
template and complete raw ASM. Receives the execution parent's24 successful
movement-step BYTEs, not a Mech ID. Clears target high bits and drops inactive
targets over all24 rows/twelve slots, retaining exactFF and no invented
target-ID bounds filter. Original caller must supply valid target IDs.

Processes friendly/enemy record banks separately. Movement heat is signed
order BYTE+1; Jump contributes at least3, using signed step BYTE when greater.
Unsigned engine-hit count adds5 per hit, engine sinks subtract their BYTE
count, intact22 critical sinks subtract1 each, and signed weapon heat adds.
Heat additions wrap to BYTE before signed clamps. Inferno contributes6 and
decrements its duration only for surviving Mechs; eligible overlapped terrain
tiles0..15 subtract4. Destroyed records skip changes but still clear weapon
heat. BUG013 remains: the upper30 clamp indexes the friendly slot even when
updating its enemy bank partner, so enemy heat can exceed30.

Dead personnel except Jason lose packed coordinates; enemies receiveFE corpse
sprite family, Jason retains coordinates and receives96. This method does
NOT clear active flags or change dead friendly companions' sprite family.
Moved existing shared targeting/range and terrain-tile storage to original
BTECH_COMBAT_DATA.c alongside heat globals; storage only, no added algorithm,
and avoids forcing graphics/UI method dependencies into round-rule tests.

Tests cover all131,072 heat/weapon-heat BYTE combinations across both banks,
signed Jump step cases, walking/running heat, engine/sink/Inferno/terrain
contributions and repeated rounds, exact destroyed-record gating, preserved
wrong-bank clamp, all256 terrain tile values, every target row and every dead
character's display/active state. Actual heat method and shared original state
execute; the movement-order allocation is test-supplied to isolate rendering.
All62 headless and80 SDL/local suites pass. No external copyrighted assets
added; integrated combat execution and playable executable remain pending.
Next: continue weapon menu/native stack handling and combat execution.

### Original weapon-ordinal lookup and warning popup — 2026-09-18

Converted whole1631:1057..10A1 warning popup and1631:10A2..1121 weapon
lookup from annotated template and every raw ASM instruction. Renamed the
misleading GetAmmo method to Combat_Get_Weapon_Index_For_Ordinal: its result
is raw weapon component-minus-one or00FF, not ammunition. The destruction
bit remains, repeated weapon entries count independently, nonweapons do not
advance ordinals, and zero raw ammunition makes a selected weapon unavailable.
The original has no ten-byte ammo-field guard: ordinal10/11 read walk/jump,
and denser ordinal values can reach subsequent critical bytes. BUG012 is
preserved through raw accesses inside the actual125-byte Mech record.

Popup saves CurrentMenuLayoutIndex before switching to panel4, displays the
caller message, waits for keyboard input, clears the sidebar and restores the
saved layout. No research helper or substituted gameplay algorithm was added.
Tests exercise all524,288 combinations of8 records/256 component bytes/256
ammo bytes, missing ordinals, repeated/destroyed weapons interleaved with
nonweapons, dense loadouts, movement-byte ammo aliases and all nine popup
layouts. UI/input boundaries are test-only; production lookup/popup bodies
execute. All61 headless and79 SDL/local suites pass with strict warnings.

Read the next weapon-menu dependency and its native stack operands. Its dense
twelve-weapon strlen starts atBP-20 and can reach unassignedBP-14 before any
row save; laterBP-14 stores a displayed target's text row. The full method is
still pending an explicit native-stack contract for this bug, not converted
with a fabricated terminator or collector-count replacement. Clarified the
older BUG012 redesign recommendation is for modern C#, not preservation C.
Next: full weapon-menu conversion and remaining combat execution dependencies.

### Original friendly combat planning menu — 2026-09-18

Converted entire183B:14C3..1773 Combat_UI_Menu_Logic from the annotated
template and complete raw ASM. All locals and results are WORDs; native signed
comparisons remain. Read the three EXE-owned menu/status strings directly:
computer status has NO trailing CR, unlike the annotated approximation.

Selects the first active friendly and initially highlights Begin Fight. Each
iteration describes that unit and invokes either its AI planning preview or
manual movement preview. Mech/personnel menus retain their distinct ten/eight
choices. Movement, Clear Moves, Kick, Computer, Weapon and Scan dispatch keep
the native order; personnel Clear Moves fills ALL48 bytes withFF, clears that
unit's AI state and redraws the manual preview. Next Unit skips inactive
friendlies, wraps at12 and keeps Next Unit highlighted in the new menu type.
Flee returns1; Begin Fight returns0 without executing a round. Native signed
out-of-range choices still dispatch as written, rather than being clamped.

DisableComputerControl is the original3092:0090 WORD. Any nonzero value clears
ONLY the initially selected AI state. The Computer option is still displayed
but inert when disabled; subsequently selected AI units still get the automatic
preview call. No all-inactive guard was invented: caller's active-friendly
precondition remains necessary, especially for Next Unit's native loop.

Tests exercise every Mech/personnel action, Begin/Flee, first-active selection,
cross-type Next Unit and wrap, untouched neighboring order rows, nonboolean
AI/disable state, retained other-unit AI state, full-WORD results and native
signed negative-choice handling. These test the real menu body with explicit
test-only callee boundaries, not an integrated combat round. Missing production
AI/weapon-selection bodies remain unresolved library contracts, NOT no-op
stubs. All60 headless and78 SDL/local suites pass. No copyrighted external
assets added. Next: weapon-selection body and AI/combat execution dependencies.

### Original weapon/kick targeting and range — 2026-09-18

Converted whole1543:07CB..0A34 target picker,183B:2231..22BB kick entry,
1631:0BB5..0C62 packed-world distance and1631:0F24..1056 range classification
from annotations and complete raw ASM. Read EXE-owned text, all four native
range pointers and the initial persistent enemy cursor. Restored target/range
and weapon-label06/0F colour controls and exact kick prompt without the
annotation's added trailing space. No external asset data was embedded.

Target cursor persists and skips inactive slots, wrapping23 to12. There is no
all-inactive escape in the binary. Target, Next and Cancel retain their native
effects; an unexpected choice exits without changing the order. OUT is merely
displayed and can still be selected. Both hips must be intact to offer a kick;
other actuator bits are irrelevant here. Kick uses weapon record32 and target
slot11, not immediate damage, and keeps menu option4 highlighted either way.

Distance unpacks128-cell pages and returns floor(larger axis separation/2)
plus the smaller separation. Distant Mech targets receive the original
asymmetric footprint correction, including sequential X comparisons and
packed-boundary masks. All range comparisons are strict; maximum equality is
OUT. Short/medium thresholds pack into one BYTE, scaled threefold when the
record's BYTE12 infantry attack flag is absent, except Kick. This reads the
existing attackCountOrClusterColumn field, NOT skillType; maximum is already
scaled. No tabletop reinterpretation or corrected range gate was introduced.

New tests cover137,216 packed-coordinate pairs, all33 weapon records at100
distances, Mech/personnel footprint differences, packed page crossing,
selection/cancel/next/wrap/inactive skips, unexpected menu choice, OUT acceptance
and all256 low-nibble hip combinations through the real kick/picker chain.
Weapon data, distance and range are production code; rendering/menu/input and
camera boundaries are test-only. All59 headless and77 SDL/local suites pass.
Still no gameplay/emulator comparison or playable executable is claimed.
Next: combat planning menu and remaining weapon-selection/execution workflows.

### Original combat movement-plan selection — 2026-09-18

Converted complete183B:1C1F..2230 Combat_Select_Movement_Plan_For_Turn from
the annotated template and every raw ASM instruction. Read the referenced
EXE-owned strings and three movement-mode text pointers; preserved exact
punctuation, carriage returns and running's extra n. No external assets added.

Friendly AI orders are cleared and previewed BEFORE clearing their action
state. A walk/run/jump mode change prompts with default Yes, retaining orders
on rejection and deleting all48 order bytes on acceptance. Zero movement
reports missing jump jets, exact heat30 shutdown, leg/heat inhibition or
exhausted movement with the native running hint and flag-clear distinctions.

Caller supplies the initial preview endpoint. Eight-direction cursor movement
clamps to the native viewport, but even an attempted boundary move recomputes
the destination. Every cursor key overwrites ONE provisional four-byte
waypoint; matching a prior waypoint clears only that provisional mode byte.
Native signed BYTE comparisons, packed region/local coordinates, restored
camera and user cursor versus reachable path endpoint remain. Space/Return
finish; no Escape cancellation was invented. BUG018 remains: a full48-byte
order row writes the next unit's row through the contiguous original array.

Extended the existing combat-plan test with15 UI scenarios: destination
overwrite/return, prior-waypoint duplicate, cursor beyond affordable travel,
accepted/rejected mode changes including nonboolean Yes, AI-order reset,
missing jets, shutdown, damaged legs, heat penalty, exhausted walking/running
and infantry movement, and full-row spill. Real movement budgets, path builder,
step/direction/occupancy/packed-position methods, keyboard conversion and text
formatting/string methods execute. Queued input, menu/text display and drawing
boundaries remain test-only; these are not emulator gameplay comparisons.
58 headless and76 SDL/local suites pass with strict C17 warnings enabled.
Next: connect the remaining combat turn/menu and execution workflows. The
preservation project remains a static library, not yet a playable executable.

### Original pilot/rider assignment — 2026-09-18

Converted whole1467:0002..08A7 Assign_Pilot_and_rider_to_Mechs from annotated
template and all raw ASM, including final0838 writeback under the original C
startup DS=SS contract. Both compact row maps are local WORD arrays, not global
or file-pointer substitutes. Normal menu/text/input children are bound to their
original contracts; live DS/SS/effective-address comparison remains pending,
not claimed by these tests. Every21 referenced EXE-owned text string was read
from the expanded EXE; restored exact CR/06 colour controls, spacing and absent
punctuation rather than silently reproducing the annotated approximations.

Zero mode synchronizes surviving friendly Mech seats without clearing other
character assignments. Any nonzero mode resets all eight party assignment
bytes and both seats of all four lance records, even dead/destroyed entries.
Native exact FF name sentinel, signed character name/assignment loads, unsigned
seat record IDs, eligible-pilot count and compact menu mappings remain. Summary
names trim trailing spaces; vehicle menu retains the otherwise unused scratch
copy/truncation at byte11 but displays original name. Character detaches from
ALL old seats before vehicle choice, including destroyed Mechs; None dismounts.
Unskilled members ride only. Qualified members take empty pilot seats directly,
otherwise answer the default-Yes prompt. Replaced pilot moves to rider and
existing rider dismounts; replacing a rider dismounts that occupant. Traitor
warning applies to either seat and does not block assignment.

Done rejects unpiloted existing Mechs. Sufficient qualified personnel prompts
assignment; shortage offers default-No abandonment. Nonzero confirmation
changes only name[0] and linked party assignment bytes, retaining seats/record
contents. Menu redraw/another Done is required even after abandonment. Success
clears the four stored-name initials toFF and hidden-name flag to0.

Thirteen scenarios cover synchronization/sparse character and Mech rows,
nonboolean reset, all-party reset, accepted/declined pilot replacement,
unskilled rider replacement, pre-choice detach/None, enough-pilot rejection,
abandonment acceptance/rejection/nonboolean Yes, traitor rider warning, empty
party/lance, full eight-member/four-Mech party and actual Menu_Assign_Pilots
parent integration. Actual string-copy/length/colour/newline methods are used;
queued selection, text display and input/sidebar boundaries are test-only.
58 headless and76 SDL/local suites pass; no external copyrighted assets added.
Next: combat movement-plan selection UI and round execution dependencies.

### Original combat/planning sprite compositor — 2026-09-18

Converted entire0800:0E4B..1731 Draw_Menu_MultiSelect_0E4B from the annotated
template and complete raw ASM. Retains the already selected EGA adapter2 path;
discarded adapter0/1 software/adapter3 paths are not reintroduced. Effects draw
first, then friendly/enemy infantry, then up to eight Mechs depth-sorted by
signed ascending pixelY, then the four Jailbreak parked-Mech overlays. Native
XFFFF sentinel, page-specific narrow projection guards, broad signed guards,
masked local cells, camera parity, terrain overlap and temporary sprite-header
height edits remain. Infantry sprite sums are WORD; mech sums truncate to BYTE.
No active-word gate or Mech-position rewriting was added.

Native compositor children consist of effects->sprite0377 and sprite0377.
Effects does not change DS;0377 saves incoming DS and restores it at shared0568
exit (including the known wrong-port early cleanup). Combined with byte-checked
startup DS=SS, ordinary C local parallel arrays represent the native collection/
sort storage. Exact call-site emulator confirmation remains a workflow check,
not a requirement to invent separate DS-backed global copies of stack locals.

Bound324C/327C pixel anchors,3750 tile bytes,4554 Y-parity bytes and45CE previous
map-row bytes.246C:0795 is explicitly an offset BYTE view into contiguous cache
storage24 bytes before07AD, not an independent WORD table. Both biased native
DS0364/039E WORD lookup views were read directly from the original EXE, including
all128 masked-X indices and surrounding original bytes. Combat visibility24
bytes42F6 and the exploration NPC first16-byte view share ONE allocation; the
NPC view retains its original declared sizeof16. External assets not embedded.

Headless tests cover empty visibility clearing, infantry-before-mech ordering,
all-eight depth sort with parallel-field identity, clipping/header restoration,
cache clipping exemption, inactive-but-positioned actors, pixel anchors,
previous-row alias, Y parity, different WORD/BYTE sprite sums, edge rejection,
neighbour packed-page projection and Jailbreak overlay. SDL test uses actual
original sprite renderer and compares all320x200 viewport pixels with a simple
independent rectangle oracle for depth overlap and clipped/unclipped sprites.
Synthetic sprites are test-only. Effects are an explicit isolated boundary.
57 headless and75 SDL/local suites pass. This is not emulator gameplay validation.
Next: planning UI and full crew assignment under the established data/stack
contract; combat round execution and executable integration remain incomplete.

### Planning renderer prerequisite: original data/stack alias — 2026-09-18

Read every raw ASM instruction in0800:0E4B..1731 and the annotated compositor.
Identified DS-based indexed local-array sort accesses, then checked actual EXE
bytes, its MZ entry/relocations and startup. Startup relocates36DB into the C
data segment (analysis3EDB), installs SS there and explicitly sets DS=SS before
runtime initialization. This maps to captured SS3858 for known load017D.
Ordinary C local arrays are therefore consistent with the compiler's intended
model; unrelated DS246C hardware snapshots are not a disproof. Details and
remaining call-site checks are in docs/investigations/ORIGINAL_DATA_STACK_ALIAS.md.
No routine conversion or new gameplay-validation claim in this checkpoint.
Next: check compositor children restore DS, bind native projection/visibility
aliases and convert complete0800:0E4B, including sort and jail overlay.

### Original combat destination-to-step builder — 2026-09-18

Converted complete183B:193B..1C1E Combat_Calculate_Movement from the annotated
template and every raw ASM instruction. Clears24 plan bytes to sentinel2,
starts working position at camera A44B/A44D, decodes FF-terminated four-byte
orders (mode, packed region, signed local X/Y), and temporarily changes facing
for idle friendly actors or originally unknown facingFF. Calls actual original
Position_0006, appends signed step bytes, computes parity-specific cached tile
cost1/2/3 (jump always1), and restores the original facing. World unit position
tables are not changed. Native signed actor comparison and SUB WORD before SAR
are incorporated, correcting the outstanding annotated audit differences.

Preserved append-before-cost, failed-search zero steps consuming MP, destination
blocked flag, reading remaining orders after budget exhaustion and absence of
a12-step row guard. Flat native storage preserves a13th pair overwriting the
next unit's row when given an oversized budget; no silent clamping introduced.
Terrain-cost ODD-X correction differs deliberately from step-clearance EVEN-X
correction. Four native terrain masks were checked against EXE. Native-valid
orders/cache accesses/full-allocation writes remain the host contract; this
does not emulate arbitrary corrupt accesses outside the allocations.

Integration suite uses actual direction, step selector, occupancy, path builder,
personnel budget and preview. Tests clear paths, camera vs actor origin, facing
restoration, untouched adjacent rows,1/2/3 costs including overspending final
step, jump, empty/terminated orders,12/13 steps, blocked terrain/occupied target,
multiple destinations, all camera parities and quadrant terrain masks, plus
actual budget->plan->preview endpoint/glyph sequence. Drawing calls remain test
boundaries, not a gameplay/emulator certificate.56 headless and73 SDL/local
suites pass. Remaining planning dependency:0800:0E4B sprite composition.

### Original mech movement budget — 2026-09-18

Converted complete183B:22BC..2473 Combat_Mech_Movement from the annotated
template and all raw ASM instructions. Uses mech RECORD ID, unsigned walking/
jumping BYTE values, signed heat BYTE, low-nibble leg actuators only. No primary
actuator on either side forces1 before heat; one surviving primary halves the
walking base rounded up. Other missing actuator bits decrement once per side;
native exact-zero correction occurs after both sides for each bit, NOT a
general minimum1 clamp. Heat loses one point per5 using truncating signed IDIV.
Running adds arithmetic-shift-half of the already adjusted points, plus the
original walking-base parity. Jump bypasses those deductions. Exactly30 heat
zeros movement (31 does not trigger this equality); final negative values
clamp0, without a maximum cap. Shared leg-damage/heat-penalty WORDs reset each
call and retain signed penalty bits. Constants describe modes, masks and heat.

Moved the unchanged shared zero-initialized MechHeatLevel storage from the
inspection object's file to original data, avoiding a spurious dependence on
the inspection UI. Movement-preview tests now use BOTH real original mech and
infantry budget methods; only path generation/rendering remain boundaries.
New instruction-shaped WORD/SAR oracle checks1,835,008 combinations: seven
walking bases including0/255, all256 paired leg actuator configurations,
all256 signed heat bytes, four modes includingFFFF, every mech record and
irrelevant high arm bits. Readable scenarios also cover healthy running, heat,
single/both-primary loss and jumping at29/30/31 heat. This is method-level ASM
behaviour evidence, not emulator gameplay validation.55 headless and72 SDL/
local suites pass. Next: original183B:193B path generation.

### Original combat movement preview — 2026-09-18

Converted complete183B:1774..193A from the annotated method and every raw ASM
instruction. Combat_Render_Movement_Preview optionally recenters on the actor,
rebuilds the cached view/planning sprites, presents it, and draws a white marker
(3x3 cells for mech combatant ranges;1x1 for personnel). Friendly units alone
request their movement budget/path and draw signed dx/dy glyphs; enemies leave
the shared endpoint untouched. Native SS:glyph is a two-byte local string.
The twelve-byte direction-glyph table was checked against the original EXE.
The adapter3-only B782 flag is omitted from the retained EGA pipeline.

Native order/plan allocations are flat24-unit rows of48/24 bytes. Constants
express their purpose, twelve steps, four-column glyph lookup and viewport cell
origin. The sentinel is deliberately read before the twelve-step limit, so a
full plan probes the following row before stopping. BYTE movement mode sign
extension, endpoint WORD arithmetic and empty-plan origin remain unchanged.

New tests cover mech/personnel markers, signed diagonals and modeFF->FFFF,
full twelve-step plans with a nonsentinel next row, empty plans, optional redraw,
real infantry movement budget and every enemy ID12..23. Rendering calls,
recentering and pending mech-budget/path methods are explicit test boundaries;
this is NOT full graphical/path/gameplay validation.54 headless and71 SDL/local
suites pass. No external copyrighted assets were added. Next dependencies:
183B:22BC mech movement budget,193B path generation,0800:0E4B planning sprites.

### Original combat scan target browser — 2026-09-18

Converted complete0DAB:1467..174B Combat_Browse_Scan_Targets_1467 from
annotated template and full raw ASM. Three WORD arguments remain: scanning
actor, side base0/12, include-mechs/detail permission. Disabled details starts
at side+4 and skips that side's four mech slots on each wrap. Eligibility is
active WORD and BOTH packed coordinates !=FFFF;00FF is valid. Friendly actors
call the actual original friendly description; enemies use live mech record
ID-8 or personnel recordID-8, actual health-description method, signed weapon
index and actual compass-direction method. All seven menu/description strings
were verified against EXE, retaining CR/06 controls and absent trailing CR
where native. In particular native Detail ScanDone adjacency is not prettified.

Preserved nonzero browser menu exits; option1 is a detailed stats request only
when offered for a mech, followed by original planning refresh and decrement
cancelling unconditional increment. It redisplays the same target, including
WORD decrement/increment wrap for friendly0. Twelve-actor side cycling and
rental-arena clamp to enemy12 remain. Native unreachable redraw-on-exit local
is permanently zero, so no extra call is invented. Exit clears temporary menu
base-row, restores default6 and selects white text. No empty-target termination
is introduced; the original loops until an eligible actor yields an exit.

Integrated tests run actual scan parent -> browser -> description/health,
actual panel-context and direction methods. Check sparse target filtering,
FFFF versus00FF, enemy live-mech/personnel descriptions, Next cycling/side wrap,
detail return and repeated target including friendly0 WORD wrap, no-mech mode,
friendly personnel, truthyFFFF permission, rental-arena12-only cycling, exact
raw strings/control fields and parent overriding final default6 with infantry4.
Movement preview, stats viewer, planning refresh, text and input boundaries
remain test-only; their mocks are not production methods or full gameplay
validation. Strict C17:53 headless and70 SDL/local suites pass.

### Original scan-menu permissions — 2026-09-18

Converted complete183B:2591..273C Scan_Enemies from annotated template and
full raw ASM. Actor argument remains friendly combatant ID, not a pointer.
Original panel3/4 changes and menu controls offer Friends/Enemies/Cancel.
On-foot enemy browsing omits mechs, optionally warns if ANY enemy mech is
active, and requires active enemy infantry with X!=FFFF. No Y/name/visibility
filter is invented. Mech sensor restriction tests EXACTLY two hits, not>=2;
it clears include/detail flag but does not clear the separate capability
flag. Thus it still calls the browser even with no enemy humans, as native.

Preserved capability-flag addition, negative menu WORD fallback to Friends,
Cancel/unrecognized positive return skipping browser, side base0/12, final
temporary base-row clear and restored Scan Unit defaults mech6/personnel4.
The scan-option count is not restored by this method. Sensor-disabled and
empty-on-foot paths retain their original distinct sidebar/input ordering.
All four strings checked against expanded EXE: menu/warnings have NO trailing
CR, correcting the additional line breaks in the annotated template. Native
269D coordinate sentinel isFFFF, not abbreviated ASM00FF.

Tests use actual panel-context code and explicit test-only browser/render/input
boundaries. Cover every friendly actor, Friends/Cancel/unknown/negative returns,
sensor0/1/2/3, each enemy mech presence, each enemy infantry active/X sentinel,
valid X00FF with YFFFF (parent deliberately permits browser handoff), permission
flags, warning/prompts, final control defaults and sensor-disabled no-human
handoff. Strict C17:52 headless and69 SDL/local suites pass. The target browser,
preview renderer and stats viewer remain real conversion work; no production
stub supplies them and this is not full scan/gameplay validation.

### Original friendly combatant descriptions — 2026-09-18

Converted complete0DAB:18E8..1AFD from annotated template and full raw ASM.
Friendly mech IDs0..3 use unsigned pilot seat -> character record -> signed
name ID (unlike1467 selector's direct pilot-name lookup). Friendly personnel
IDs4..11 subtract the friendly infantry range start. Name-only mode returns
without health/equipment/input; detail mode prints actual original health
description and weapon/armour data unless permitted traitor recognition
replaces that output with the native suspicion warning, wait/key and latch.

Incorporated audit corrections: signed WORD actor-range gate and signed BYTE
name/weapon/armour/traitor loads, signed armour durability comparison, exact
EXE-owned warning punctuation/06 controls, CR-separated Weapon/Armor heading
with06/0F colour reset, and Ruined suffix06/0F. Thus ordinary equipment text
after the heading is white, damaged armour yellow and ruined armour enters
grey then resets white; None does NOT stay green as the old annotation said.
No destroyed-pilot, malformed-index or health zero guard was invented.

Moved the shared original armour durability table unchanged from inspection
object to original data storage: combat description needs that table without
linking the still-unconverted stats-viewer dependency. No gameplay helper or
new table values were introduced.

Tests use the real health-description callee and green-colour helper. They
cover all eight friendly personnel/four friendly mech identities, deliberately
different slot/name IDs, name-only versus detail modes, all armour types with
pristine/damaged/ruined colours, exact raw controls, signed matching durability
bytes, truthy high-WORD mode values, traitor recognition suppressed by either
gate, signedFF traitor nonmatch, wait/key and warning latch. Test-only text
boundary models the limited CR/06 controls used here; this is not a claim
of new end-to-end graphical validation. Strict C17:51 headless and68 SDL/local
suites pass. Scan/combat parents and whole playable executable remain work.

### Original component-status pips — 2026-09-18

Converted complete0DAB:1858..18E7 from annotated template and full ASM.
Corrected unsigned total-loop bound to native signed JL:8000..FFFF totals
draw none. Healthy count marks the first red index; negative/high-bit values
do not accidentally force damage. Pip5 alone subtracts five columns and adds
one row; totals beyond ten do not introduce another wrap. Native WORD shift/
coordinate/column wrapping remains explicit. Original retained EGA colours
are green2/red4 and each pip is the inclusive3x3 rectangle at cell offsets
(2,3)..(4,5). Literal offsets describe artwork layout, not tabletop values.

Call-level tests cover totals0..12, healthy counts0..13/FFFF, negative totals,
largest positive signed total and wrapped coordinates. A second target uses
actual original rectangle/line methods and SDL EGA backing, independently
checks every pixel across the screen's top40 rows for normal combinations
and confirms signed totals draw no marks. No game artwork is embedded.
Strict C17 passes50 headless and67 SDL/local suites.

The parent stats viewer remains incomplete: its uninitialized native stack
palette counter/phase need explicit preservation treatment rather than C
undefined reads or silently selected zero defaults. This verified renderer
does not certify the complete BTSTATS workflow or a playable executable.

### Original compact 'Mech selector — 2026-09-18

Converted complete1467:0B98..0D7D Display_Text_Mech_Names from the annotated
method and full raw ASM. Four nativeD558 mapping bytes are cleared then filled
with eligible friendly record IDs; the return is the row COUNT, not selected
record. SelectedMechId is the signed-BYTE-expanded mapping output. D324 hidden
name staging selects using savedD452 initials and reconstructs a temporary
name in the original dynamic string; live first-name bytes remain untouched.
Original menu23 receives starting text row, compact count and selection0,
then restores baseRow1 after selection. No empty-lance early return is added.

Preserved direct UNSIGNED PilotId -> CharacterNames lookup. This differs from
assignment's Characters[PilotId].name lookup and can visibly differ when name
IDs and roster slots do not match; runtime bug classification remains open.
Absent pilot/name and invalid menu row pointer cases are not sanitized; valid
native table indices remain the flat host array contract. Native1CD3:181E
line-break helper is now original C too: inline DS4FA0 bytes0D,00 are a string,
not a pointer to dereference. Its complete body was checked in ASM.

Tests cover all16 lance-presence patterns in both live and staged modes,
every available compact selection, sparse record IDs, clearing unused mapping
entries, direct pilot-name indexing with deliberately mismatched character
name IDs, temporary hidden-name restoration without live writes, count return,
selected output, menu defaults/base-row restoration and zero-row invocation.
Inspection tests now use the actual selector and line-break method rather than
a selection stub; a single row maps to original record3 before heat reset and
stats-view dispatch. Strict C17:49 headless and65 SDL/local suites pass.

Full1467:0002 assignment remains deferred at its unresolved final0838 DS-vs-SS
row-map binding (existing Astra candidate A-003). Rechecking stack-probe code
207F:2FDC found no DS switch; unrelated capture register snapshots cannot prove
the required effective addresses. Do not substitute local-array write-back
and claim this binding resolved. There remains useful conversion work,
including the stats viewer and remaining original gameplay parents.

### Original pilot-menu entry and shared planning refresh — 2026-09-18

Converted complete0800:4D57..4DC6 Menu_Assign_Pilots and1631:032F..03AA
Menu_Draw_MultiSelect from annotated templates and their full raw ASM.
The pilot-menu entry examines all nine row-major local terrain flags for
bit80 (block dismounting). If safe, it calls the original shared selector
in synchronization mode0, refreshes health/financial status and explicitly
redraws the top graphic again. If blocked, it opens the original message
box, displays the exact EXE-owned populated-area warning through the original
wait/input helper and reads a further key. No proximity heuristic or skip
of noncentral neighbours replaces the native tests.

Shared refresh addresses the party's packed coordinates, rebuilds the cached
framebuffer, calls planning preview for any nonzero WORD or exploration actor
render for zero, then transfers the view. It activates/redraws/borders panel4
followed by panel3, leaving the original panel3 text context selected; no
caller-context restore or elimination of repeated sidebar calls is added.

Tests check each of the nine independent blocking cells, lower bits alone,
all blocked, mode0 assignment, warning/key sequence, both rendering branches,
8000/FFFF truth values, exact refresh order and final original panel geometry.
They use the actual menu-context method; selector, map/render and input
boundaries are explicitly test-only. Full1467:0002 pilot/rider selector and
0800:0E4B planning-preview callee remain conversion work, not production
stubs. These parent methods are not a claim that either complete UI workflow
is playable yet. Strict C17:48 headless and64 SDL/local suites pass.

### Original health-description integration — 2026-09-18

Converted complete1631:02E4..032E Display_Text_Human_Health and checked every
ASM instruction. Both health and Body BYTEs are CBW sign-extended, IDIV
truncates toward zero, and the quotient selects one of eleven native DS2E0C
FAR-pointer descriptions. All strings were extracted from the EXE, including
their leading spaces. This is a textual health band, NOT a health bar or
percentage; earlier inspection-checkpoint references to a health-bar boundary
meant this still-unconverted callee and should not imply a graphical renderer.

Removed inspection's test-only health callee: character inspection now calls
the actual method through the real temporary panel narrowing. Tests check all
eleven bands with nonmultiple health values (no rounding), leading-space text,
signed negative BYTE division yielding a valid band and panel restoration.
There is no invented zero-Body guard, quotient clamp or fallback description.
Nonzero Body and a valid native description-table quotient remain required;
native invalid arithmetic/table pointers are not certified by this test.
47 headless and63 SDL/local suites pass under strict C17.

### Original character and 'Mech inspection — 2026-09-18

Converted complete0800:378D..3BCF Inspect_Characters from the annotated
method and its complete raw ASM. It activates original panel6, counts active
lance slots, optionally selects a 'Mech, clears only that selected heat BYTE,
opens the original stats viewer, refreshes the sidebar and returns. Otherwise
it counts/selects party members through original Citadel dialogue actions,
builds the fixed character sheet, temporarily narrows the panel for the
health display, prints armour/traitor status and seven skill descriptions,
prints equipment ownership and waits/reads input. Original capitalization,
formatting controls, labels and positions were checked against the EXE.

Incorporated ASM audit differences: selected/name/weapon/armour IDs use signed
BYTE loads; armour durability/value are sign-extended before WORD subtraction;
skill clamp is signed JLE4, not an unsigned saturation. No zero clamp is added.
Description table has original "Adequate", not an invented "Average" string.
EXE-owned skill strings, mutable armour durability0/25/40/30/50/50 and zeroed
eight-BYTE transient heat storage are original game data. Shared armour text
table moved unchanged from equipment routine's object into original shared
data so inspection does not accidentally pull unrelated distribution code.

Tests use real text-position/colour methods, numeric formatter and panel
context; test-only boundaries select dialogue choices and record rendering,
stats viewer, health bar and input. Cover single/multiple-member selection,
all armour types pristine/partial/zero, signedFF armour value, negative lost
armour without sanitization, traitor override/nonmatch, all eight equipment
ownership combinations, normal/positive oversized skills, character versus
'Mech paths and selected-only heat reset. Test failure output exits directly
instead of opening a blocking Windows assertion dialogue. Strict C17 passes
47 headless suites and63 SDL/local suites.

Valid table IDs/skill-description indices remain the host contract. Negative
skill bytes in the original address preceding FAR-pointer data; arbitrary
malformed-state pointer resolution is not proven by this valid-game test.
The actual Citadel parent, health renderer and BTSTATS viewer still require
conversion/integration; no production callback or fallback replaces them.

### Original exploration settings — 2026-09-18

Converted complete0800:3BD0..3D3F Menu_Change_Game_Settings from the annotated
template and full ASM. All menu strings and initialized sound WORD1 were
checked against the expanded EXE. Uses the existing caller panel and original
menu-control records; it does not activate a replacement settings window.

Preserved movement defaults1/2/4 ->0/1/2, selected index+1 with three becoming
four, temporary movement-menu base-row flag, persistent three-option count,
WORD overflow, and nativeD34E map-room override to one step. Combat speed
stores the full menu WORD; outtake frequency stores only AL into persistent
D35B. Sound tests the whole enable WORD when constructing the action text,
then toggles only bit0 (native low-BYTE XOR, equivalent WORD XOR1). Quit
stores the complete Yes/No result into ExitMainLoop without boolean narrowing.
Cancel/unrecognized choices have no action; only speed/outtakes refresh the
health/financial sidebar. The unused CGA quit-prompt palette path is omitted
consistently with the retained EGA build.

Tests cover every normal movement default/choice combination, zero/FFFF stored
rates and selection wrapping, nonzero map-room override, all six speeds and
FFFF, sound action text and upper-WORD preservation, all frequency options
and BYTE truncation, full quit responses, cancel and unknown selections.
Pause-menu tests now call the real settings method, including cancel and
sound toggling through the actual parent/callee dispatch; remaining action
and rendering boundaries are explicitly test-only. Strict C17 validation:
46 headless suites and62 SDL/local suites pass. Whole-game playable entry
and the remaining original action workflows are still incomplete.

### Original exploration pause menu — 2026-09-18

Converted complete0800:2C50..2DA7 Game_Pause_Menu from the annotated method,
checking the full raw ASM dispatch and jump table. Uses existing original
menu-panel context and mutable menu-control records, not a replacement host
menu or research helpers. Seven base choices expand to eight only when one
of the four friendly lance records has name byte other thanFF; an empty
name byte0 still counts, and enemy records do not. Original capitalized
"Allocate men in 'Mechs" text is restored.

Preserved persistent window/control mutations, panel activation/sidebar/border/
text ordering, optional pilot allocation, shifted inspect/heal/load/save/map
choices and unconditional underlying viewport redraw after dispatch. Invalid
WORD choices take no action, just as the native comparisons/jump-table range
check do. The next opening resets geometry before reevaluating the lance.
Settings, pilot allocation, inspection, save/load and overhead-map callees
remain separate original conversion work, not production no-op callbacks.

Tests use the real original panel-context method and test-only action/render
boundaries: every choice, zero/out-of-range/FFFF, each friendly slot, enemy-only
presence, empty-but-present slots and expansion followed by reset. Strict C17
validation passes45 headless suites and61 SDL/local suites. This is dispatcher
validation, not certification of the still-unconverted action workflows.

### Original ANM scene playback — 2026-09-18

Converted complete0800:48B7..4AA5 scene dispatch,0800:1AFD..1C11 frame
pipeline and207F:23EC..245B XOR-RLE decoder from the annotated methods,
cross-checking their complete ASM. Original207F:1E37 retained EGA frame
transfer redirects through the separate SDL hardware layer. No external
artwork is embedded or exported; original raw DOS loader reads required ANMs.

Preserved filename/random ordering even for skipped scenes, signed scene and
frequency comparisons, disk selection, close-before-error-test, seven siren
passes, first-frame holds and caller-controlled view restoration. Native
failed handle isFFFF, not00FF: valid handle255 must still load. Packed frame,
converted planar frame and raw-file storage remain adjacent, including the
56 cleared trailing workspace bytes that malformed control0 timing can read.
Signed BYTE timing multiply wraps to WORD before arithmetic right shift;
negative waits are not sanitized. Extended RLE counts are big-endian, XOR
deltas accumulate, zero counts follow native LOOP output exhaustion, and
oversized final runs stop when the3872-byte frame is complete.

Headless method tests exercise decoder grammar, accumulated XOR, filename and
frequency selection, probe failure/valid255, siren reset, menu/sound restore,
signed timing overflow and adjacent-memory timing reads. Opt-in local test
uses real DOS loading, decoder, packed-to-planar conversion and SDL EGA transfer
for all22 shipped ANM files:198 frames match an independent bounded decoder
and all88x88 rendered pixels. Timing, stream advancement, frame count, opaque
zero pixels, outside-frame preservation and hardware masks are also checked.
Only test boundary callbacks suppress interactive menus and real-time waiting.

Validation:44 headless suites and60 SDL/local suites pass under strict C17.
This confirms the animation path, not whole-game playability; executable
entry, remaining original menus/gameplay parents and gameplay comparisons
still need completion. Shipped non-straddling RAM windows remain the decoder
contract; arbitrary malformed FAR-segment input is not an emulated CPU.

### Original movement-step planner — 2026-09-18

Converted complete1631:0006..02E3 Position_0006 from the annotated template,
checking all raw ASM branches and .dis counter/signed constants. Six WORD
arguments remain; working MovementActorPositionX/Y, facing and chosen step are
outputs, while callers publish actual combatant world positions. The cache
subtraction wraps to signed WORD BEFORE SAR1, correcting the systematic audit
mismatch; negative odd halves floor down rather than truncate toward zero.

Preserved first100-call search-bank timer then30-call reload, low-BYTE bank XOR8
versus whole-WORD nonalternate clear, signed BYTE initial facing/adjustments,
one-octant heading correction, eight cumulative retry offsets, packed page
carries, camera parity/cache-origin corrections, signed primary blocking threshold
versus unsigned adjacent footprint threshold, probe80 occupancy bypass without
terrain/footprint bypass, four-argument occupancy with crushingFALSE, temporary
camera restore and sticky destination-blocked latch. At destination/no legal
candidate leaves zero step and original working position/facing. No synthetic
pathfinder, iteration/bounds safeguard or research helper was imported.

Movement311A/312A/313A/314A tables are distinct mutable original storage, not
aliases of equal-valued328A path tables. Direction-search offsets and initial
timer/bank were extracted from expanded DS3EDB:310A..315D. Chosen-step WORDs
and blocked latch map native3092:458E/4590/D57E. Shipped-valid actor/probe IDs
and cache indices remain the original calling contract.

Tests use real direction and occupancy implementations: infantry versus mech
footprint, alternative direction retries, all blocked, inactive-record occupancy,
no planning crush, probes, sticky destination state, search-bank/timer WORD wrap,
one-octant turn, packed X/Y page carry, negative-floor and overflowing screen-X
SUB/SAR addresses, signed threshold. An integrated three-update headless roaming
sequence runs actual NPC -> step -> direction -> occupancy -> frame interpreter,
including collision avoidance and camera restoration. Only unused UI/audio/
tile-upload boundaries are test doubles. The previous NPC adapter suite remains
useful isolated coverage, but is no longer the only movement evidence.

All43 headless and58 SDL/local-asset suites pass. This is not full gameplay or
emulator equivalence proof. Combat movement-order playback/parents, scene
animation, menus and playable executable startup remain unfinished.

### Walk streams, animated tiles and roaming NPCs — 2026-09-18

Converted original0800:1732..17BA stream interpreter,231D..240A friendly
movement animations,240B..24C1 retained-EGA animated map tiles and24C2..2866
roaming NPC update, checking full ASM plus .dis signedFF80 X threshold.
FD consumes/stores facing, FE reads a signed unconsumed relative distance,
FF re-reads itself indefinitely, other negative tokens are skipped. Cursor
WORD wraps without segment carry; AnimationCursor uses a host byte window plus
its native origin/current offset, not an unbounded host pointer increment.
Original streams, direction pointers,25 initial cursors and ten tile IDs are
EXE-owned data extracted from2FE8:0270..02EF/DS3EDB initialized tables.
FD for effect cursor24 writes native3984, the low BYTE of FixedTonePitDivisor,
not a nonexistent25th combatant-direction element. This alias is explicit and
tested with the sound WORD's high BYTE preserved; directions remain24 entries.

Corrected a prior storage mismatch: animation selector and direction both name
native3092:396C, and now alias one array. No independent selector storage remains.
Moved the existing3840-byte animation frame buffer into the original data file
without changing its address contract/contents. No research helper was imported.

NPC update preserves strict packed boundary tests, camera scratch/final restore,
delayed high-waypoint spawning, Rick holdFF-to-FE, signed projected deltas,
six-WORD Position_0006 handoff, per-combatant frames, signed BYTE lookup indices,
waypoint nibble/link mutation and zero-delay arrival disappearance (BUG-021,
probable unintended effect, not yet emulator-confirmed). Valid shipped direction/
waypoint IDs and allocated animation windows are the original-data contract;
arbitrary malformed FAR segment accesses are not emulated by those host arrays.

Converted original207F:0A9F..0B25 animated tile upload into an original entry
redirect and separate SDL hardware body. Upload preserves contiguous32-column
plane schedule, read-before-write latches, inherited rotate/ROP, disabled
set/reset, mode0/bitmaskFF, map-mask15 exit and WORD destination wrapping.
Frame advances before selection (1,2,0), skips other tilesets, wraps WORD counter
and CBW-sign-extends destination tile IDs before multiplication by32.

Tests run all16 walk streams through repeated loops, FD/FE signed/unknown tokens
and offsetFFFF wrap, absence handling/direction changes, tile cycling, camera
boundary/delay/arrival/waypoint rules and packed page crossings. Position_0006
is still a test-only callee adapter; original path-planning implementation is
pending, not stubbed in production. SDL checks every transferred byte for
three destination fixtures, all four ROPs/eight rotations, plus all ten tiles
through three real update/upload cycles. All42 headless and57 SDL/local-asset
suites pass. Scene animation, original path planner, menus/combat parents and
full playable executable/gameplay validation remain unfinished.

### Original exploration main loop — 2026-09-18

Converted complete0800:0000..051A Main_Game_Loop and29F5..2A2A allowance reader
from the annotated template, checking every raw ASM branch. Native input/menu/
movement/animation/world-tick/encounter/finance/redraw/loss/replay ordering remains
in the original method. Corrected audited signed WORD movement-setting comparison
and wrapping step/idle locals, plus signed high-WORD/unsigned low-WORD financial
comparisons. Original prepayment wealth, overflowing DWORD balances, masks,
stock CWD factor extension, BakPhar split, byte timer saturation/borrow and
old-value finance/NPC phase triggers are retained. No synthesized scheduler or
gameplay entrypoint was substituted.

Original Mechs, fog and persistent script state are adjacent at3092:C724..D36F.
WorldMapStateStorage explicitly preserves that exact3148-byte span, with named
array aliases and offset/size assertions; it is not a new game-state API. The
main loop's CAFC/CB1C fog aliases can hit the final16 mech bytes or first16 script
bytes at partial-row world extremes. Tests preserve these writes, rather than
clamp them or invoke undefined out-of-bounds C array arithmetic. Valid native
packed positions remain the data contract; arbitrary malformed segment access
is not emulated by this bounded storage object.

EXE-owned direction strings (including CR), commands, fog masks and stock tables
were recovered from initialized DS3EDB. NpcUpdatePhase beginsFF, exactly as in
the expanded EXE. Added actual1F3D:06C3 EGA viewport presentation, with original
AC00-to-A000 rectangle13,0,27,200; pixel tests confirm sidebar stays unchanged.

Main-loop tests cover idle/input timing, signed negative movement setting,
message interruption, pause, mapper/cache fog, neighbour aliases, training and
recovery countdowns, Starport borrow/restore, encounter gating, allowance/frozen
stocks/signed wealth, three stock outcomes and BakPhar split, allowance byte
order, loss, victory and replay. Callee adapters are test-only; missing original
animation/NPC/menu/combat methods remain unresolved in production, not stubbed.
All41 headless and56 SDL/local-asset suites pass. Full game executable, original
callee conversions and end-to-end gameplay/emulator validation are still pending.

### Exploration sprite compositor — 2026-09-18

Converted the complete retained EGA body0800:051B..0E4A into
src/Original/BTECH_0800_DRAW.c, using annotated code as the template and checking
the raw ASM/.dis branches. Party infantry, grey map NPCs, surviving lance mechs,
friendly position/active rebuilding and the four jailbreak parked humanoid
sprites remain in the original method. Temporary sprite-header clipping remains
inline; the annotation-only terrain helper was NOT imported as a production API.

Preserved drawing order, signed BYTE mech-assignment tests, WORD packed-camera
subtraction, same-page wrap rejection, signedFF8D..00A7/F080..0F98 bounds,
SAR-floor NPC terrain addressing and odd-camera corrections, low-byte-only mech
mask XOR, per-record sprite/overlap indexing versus compacted formation slots,
twelve-WORD friendly active clear, stale unused positions/overlap slots and
anchor restoration after each formation offset. Sprite height is subtracted
and added back, not replaced by an independently saved header.

All formation/display/occlusion tables are original EXE-owned values, checked
against expanded DS3EDB:02AE..0371 and3A16..3A2D. Native3092:006A is named
OnFootPartyMemberCount;3092:32AE is TerrainOverlapRows. Tests exercise compaction,
all camera parities, packed formation carries, signed assignment rejection,
NPC movement-delay/visibility and terrain clipping, raw-word SAR edge behaviour,
cache suppression and jailbreak overlay. Draw/effect boundaries are test doubles;
actual transparent drawing has its separate renderer suite. No gameplay helper
or external copyrighted artwork was added. All40 headless and55 SDL/local-asset
suites pass. Playable main/exploration/combat control flows remain unfinished;
these tests are not emulator/gameplay equivalence certification.

Correction of initial project checkpoint `0bbe09a` per owner instruction:
remove invented game routines, retain original methods, separate SDL redirects.

| Original source/contract | Project source | Treatment |
| --- | --- | --- |
| 1431:000A Heal_Characters | src/Original/BTECH_1431.c | Original method and dialogue-dispatch call retained; accesses map to flat globals |
| 11B8:0002..0809 | src/Original/BTECH_11B8.c | Complete original repair workflow; prices, ordering, wrapping and bugs retained |
| 11B8:1762..1A4E/return | src/Original/BTECH_11B8_AMMO.c | Original ammunition collection, quotes, request capping and per-round purchases |
| 11B8:080A/0925 | src/Original/BTECH_11B8_UPGRADES.c | Original package selection and purchase, all eight mutation paths and BUG-010 |
| 0DAB:04F9..094A | src/Original/BTECH_0DAB_SALVAGE.c | Complete original whole-Mech salvage, native overlapping text placeholders bound to one array |
| 0DAB:094B/0B5E | src/Original/BTECH_0DAB_LOOT.c | Original personnel loot and computer-control prompt; original alias/address quirks retained |
| 0FDC:15E6..17B8 | src/Original/BTECH_0FDC_EQUIPMENT.c | Original weapon distribution, live-party menu mapping and displaced-item chain |
| 0FDC:13DE..15E5 | src/Original/BTECH_0FDC_EQUIPMENT.c | Original armour distribution, suit/remaining-points chain and warning order |
| 0FDC:1A26..1D2F | src/Original/BTECH_0FDC_ARENA.c | Original owned/rental arena staging and restoration, complete record/crew/name handling |
| 0FDC:05F7/19F6/1D30 | src/Original/BTECH_0FDC_BLD.c | Original WORD operand readers and indexed fixed-span BLD decoder; disk/file callees pending |
| 207F:22F8/2368 | src/Original/BTECH_207F_DECODE.c | Original ASM literal/repeat graphics loops, sequential/column-wise output |
| 1F3D:049D maintained EGA path | src/Original/BTECH_1F3D_DECODE.c | Original marker dispatch; all non1 markers use format2 |
| 0800:28CC/2913 maintained EGA path | src/Original/BTECH_0800_DISK.c | Original signed disk/drive selection and complete disk prompt |
| 1F3D:063B/0814 | src/Original/BTECH_1F3D_FILES.c | Original length-prefixed and raw fixed-count loaders, retry/read/close order retained |
| 1F3D:002F/0259/086A | src/Original/BTECH_1F3D_INPUT.c | Original input availability, live/record/replay bridge and fifty-retrace pause |
| 0800:2A2B/2A4F/2A69/2A7E | src/Original/BTECH_0800_INPUT.c | Original key drain, continue prompt, plural suffix and punctuation |
| 207F:3BB6/3BD2 with3C15 tail;3C6C;1F3D:0053 | src/Original/BTECH_207F_NUMBERS.c | Native WORD/DWORD text formatting, signed WORD abs and dynamic-value rendering |
| 0800:2867/28A2 maintained EGA path | src/Original/BTECH_0800_TEXT.c | Original cursor/colour text wrapper and green foreground |
| 183B:2474/24F0/2556 | src/Original/BTECH_183B_SETTINGS.c | Personnel movement budget and original combat-message/effects settings |
| 11B8:0D58..104D | src/Original/BTECH_11B8_RECRUIT.c | Original specialist recruitment, shared RNG reseed, rider assignment and traitor selection |
| 183B:273D | src/Original/BTECH_183B.c | Original critical-slot supporting-structure read and C8 bypass |
| 207F:3B22/3B68/3B9E | src/Original/BTECH_207F_TEXT.c | Original append/copy/length entries; scan and forward word-copy operations inlined, no research helpers |
| 2FE8:02F0..06D7 | src/Original/BTECH_MECHREFS.c | Eight exact original EXE-owned Mech reference records |
| 3EDB:2ED8..3108 | src/Original/BTECH_WEAPONS.c | Original 33-record weapon table, including native text padding and packed statistics |
| 0800:19DD/19F3 | src/Original/BTECH_0800_DICE.c | Original D6/2D6 rolls, rejection and call order retained |
| 0800:1A13 | src/Original/BTECH_0800.c | Original Yes/No selection and redraw loop; native text controls retained |
| 1E56:0D1D | src/Original/BTECH_1E56.c | Original movement-key mappings, incorporating recorded ASM discrepancies |
| 207F:0BC0 | src/Original/BTECH_207F_RANDOM.c | Original three-byte RNG carry chain; no SDL or host RNG substitution |
| EXE-owned data,3092/3EDB storage | src/Original/BTECH_DATA.c | Static original tables and reconstructed declarations only; no reset/load/query helper routines |
| 207F:001C/0030/0051/0067 | src/Original/BTECH_207F.c | Original speaker methods redirect to SDL backend |
| 207F:0B40/0B8A | src/Original/BTECH_207F.c | Original retrace/keyboard methods redirect to SDL backend |
| 1F3D:0006 | src/Original/BTECH_1F3D.c | Original WORD loop retained; underlying retrace method is redirected |
| 0DAB:0D12; original BIOS service | src/Original/BTECH_0DAB.c | Original shutdown call retained; BIOS video operation redirects to SDL |
| Original DOS/C-runtime file imports | src/Original/DOS_IO.c | Existing open/read/write/close/seek contract names redirect to SDL I/O; these are OS imports, not newly discovered game methods |

Removed `Game_Reset`, `Game_LoadOriginalTables`, `Party_QueryInjuries`,
the invented bootstrap main, `Map_Parse`/`Map_Load`, their parser tests and
the old `src/` arrangement. Healing again calls
`Citadel_Building_Dialogs(CitadelDialog_QueryPartyInjuries)`, as in the original.
That complete dispatcher is not yet converted; its headless adapter belongs
only in the test suite. No incomplete dispatcher is presented as original code.

Data storage is declared as `Characters`, `Mechs`, recovery/equipment flags,
cash/text colour and statically initialized original lookup arrays. The invented
`Party` and `GameTables` container objects were removed. Original source names
are unrecoverable; reconstructed names/layouts are identified rather than claimed
to be literal1988 declarations.

Remaining native methods stay in `Btech/` until converted, then go into their
corresponding `src/Original/BTECH_XXXX.c`. Existing files in Original currently
contain only the listed conversions; they are not claimed as complete segments.
Research-helper definitions are excluded. Do not copy calls to them into the
build either: inline their ASM-verified operations into the original caller.

Next: original service methods and their cost/rejection tests, original startup/
dispatcher, native asset loaders, map construction and combat. No replacement
gameplay algorithms or invented entry sequence will be supplied.

Checkpoint:789,734 healing assertions and57 original-name SDL redirect checks
pass. Healing's original dispatcher call is tested with a headless adapter;
that does not certify the full dispatcher or an interactive game sequence.

## Original RNG and dice conversion

Sol: Compared the new bodies directly with clean `BTECH_207F.asm` 0BC0 and
`BTECH_0800.asm` 19DD/19F3. Removed numeric provenance suffixes from local/global
names. Runtime stack probes are omitted; they do not affect game state.
The RNG retains BYTE wrapping, CMC/SBB carry inversion, write order and the
returned low/middle XOR. Native 4FC3 remains outside this method's storage.
The EXE's initial 4FC0..4FC2 bytes are 04/03/02; original startup seed replacement
still needs conversion. No seed/reset routine was invented.

The RNG is in a second original-207F compilation unit solely because its other
converted methods depend on SDL; this keeps native gameplay available headless.
Neither unit represents the complete segment yet.

`rules.dice` checks a fixed 16-step instruction-derived RNG trace, rejected
candidate consumption and 2D6 call order. These expected values are analytical
witnesses, not observed emulator output. Healing retains deterministic dice
adapters for its existing boundary tests; the new dice test links the real bodies.

## Original prompt and key conversion

Sol: Compared 0800:1A13..1AFC and 1E56:0D1D..0E75 against the clean ASM.
Preserved the prompt's initial display, keyboard drain, redraw on every key,
WORD row decrement, default normalization and final colour restoration. The
unused native cursor-copy local and runtime stack probes are omitted.
Recovered exact 3EDB:03EE/03FE text/control sequences from the expanded EXE,
not bare offsets or simplified Yes/No labels. Embedded zero parameters must
be consumed by the original text renderer, which is still unconverted.

Applied the recorded key-table discrepancy: `{` passes unchanged; `|` maps
west and `~` maps north. The final 0E6F jump-table WORD is 0D81, confirmed
directly from the expanded EXE because the printed table truncates there.
Other unmapped WORDs pass unchanged. This corrects the annotation, not an
original game bug. `Btech` itself remains unchanged in this checkpoint.

Dice and dialogue use separate original-0800 compilation units to avoid pulling
unconverted UI dependencies into standalone headless dice tests. No game
routine was invented. `ui.original_prompt` uses test-only keyboard/rendering
adapters to check acceptance, rejection/redraw and cursor/colour effects;
actual graphical highlighting remains pending renderer conversion.

## Complete Mechlub repair conversion

Sol: Converted the existing annotated `Mechlube_Repair_Mech` body, not a new
repair algorithm. Removed FAR/segment notation by binding storage to `Mechs`,
`SelectedMechId`, cash, text cursor and menu globals. Preserved the original
public name, readable EXE-owned strings/address comments, labels and original
callee contracts. Original UI bodies remain unconverted; no production no-op
or replacement dialogue service was introduced.

Retained previously ASM-corrected eleven/eight deficit sums, 35 critical slots,
WORD wrapping, DWORD affordability and record-order partial purchases. Prices:
armour 4 per point, internal structure 9, heat sink 800, weapon selection 300,
actuators 200 flat. Preserve BUG-008 (all matching weapon slots for one selection)
and BUG-009 (continuation scan excludes SRM-6), absent structural prerequisite,
above-maximum wrapped deficits and unrepairable engine/gyro/sensor handling.
Spot-rechecked the clean ASM's structure, sink and weapon purchase/continuation
paths; the full prior audit is documented in `docs/preservation/11B8_MECHLUB_REPAIRS.md`
at repository root. Do not mistake this conversion for new emulator validation.

The method still assumes valid stable selected-Mech/menu indexes as documented
in that audit. Native repeatedly reloads selection; this annotated record cache
does not model a UI callee changing selection halfway through the visit.

Added native 17-byte `Weapon` records and the exact 561-byte EXE-owned table.
The attack/cluster and damage/range fields retain packed values; decoding belongs
to the original consuming methods, not an invented table loader. Native FAR
component-description pointers become ordinary pointers to the original text.
No external game asset was embedded.

`rules.mechlub_repairs` executes the real repair method and the converted original
Yes/No method. Test-only adapters provide unconverted UI calls. Checks cover
full/partial/refused/no-funds purchases, high balance WORD, repair ordering,
heat sinks, actuator maxima/affordability, both weapon bugs, engine rejection,
over-maximum wrapping and selected-record isolation. Menu cursor/wrapping is
only a narrow test contract, not graphical validation. Existing exhaustive
annotation tests remain separate; no duplicate research helper enters the game.

## Original Mechlub ammunition conversion

Sol: Converted the existing `Mechlube_Buy_Ammo` body to flat native-storage
globals and actual `Mech`/`Weapon` declarations. No ammo service/helper algorithm
was substituted. Separate original-11B8 compilation units keep unrelated
unconverted UI dependencies out of callers' static-library links.
The original decimal-input method remains an unconverted contract, not a
production placeholder; the test supplies its deterministic adapter.

Restored the exact seven EXE-owned price BYTEs at3EDB:2060:10/15/25/30/30/60/80
for LRM5/10/15/20 and SRM2/4/6; MG rounds cost2 C-bills. External assets are
not embedded. Prompts, colour controls and provenance comments remain intact.

Retained ten-entry raw weapon collection in critical order, destroyed flags and
laser ordinals; only functional MG/missile entries are shop-supported. Preserve
signed missing-round deficits, signed DWORD interpretation of requested quantity,
low-WORD reload counts, signed BYTE prices with unsigned DWORD affordability,
BYTE ammo increments and the final extra balance display after a nonzero request.
No added over-cap clamp, negative-price guard or autocannon support.

Used the prior complete ASM/DIS audit in repository
`docs/preservation/11B8_MECHLUB_AMMUNITION.md`; spot-rechecked native1762 collection
initialization and1906..1980 request/debit/decrement branches during conversion.
Valid/stable selected-Mech and UI contracts retain the same documented limitation
as repairs: native selection is reloaded, while the annotated method caches it.

`rules.mechlub_ammunition` runs the compiled original body with test-only UI
adapters. Covers all seven missile prices, MG cost, zero/capped/high-bit requests,
insufficient/partial funds, ordinal placement, destroyed/full/unsupported weapons,
ten-entry limit, above-maximum deficit wrapping and sign-extended corrupt price.
These are synthetic ASM-derived expectations, not interactive gameplay validation.

## Original Mechlub upgrade conversion

Sol: Converted `Mechlube_Modify_Mech` and `Mechlube_Upgrade_Mech` in place within
their own original-11B8 compilation unit. Bind native global/record accesses to
ordinary C storage; correct the prior FAR-record defect rather than copying the
segment pseudo-structure. Retain original public methods, prompt strings, call
contracts and eight case bodies. Stage bits, chassis/package counts, armour
counts, weapon ordinals and Locust walking MP now carry explicit rule names.
Critical entries remain neutral physical indices: A-005 anatomy is unresolved;
do not imply that uncertain group labels are certified by the compiling port.

Added exact EXE-owned 125-byte Mech templates (eight records at2FE8:02F0),
the eight original price WORDs at3EDB:1D8C and Commando armour bytes at1E24.
No new table-loading routine or external asset is included. Template oddities
remain original data, not normalized contemporary tabletop stats.

Retain exact stage-one assignment vs stage-two OR, stage-one-only second-stage
selection, completion gating, signed WORD/CWD price and unsigned DWORD balance.
Preserve BUG-010: unsupported selector8 reads adjacent text CR/CR as price0D0Dh
(3341 C-bills), charges BEFORE selector validation and then changes no Mech
bytes. Represent that known aliased WORD explicitly, not a host array overrun.
Stinger stage two copies template CURRENT armour into both destination arrays;
Commando stage two creates six small-laser critical entries and a medium entry,
not the two small lasers suggested by the historical description.

Use the prior 11B8 whole-method audit; spot-rechecked clean ASM's charge/dispatch,
Locust stage-one, Locust stage-two and Commando stage-two writes. Native selected
Mech reloads still rely on the documented stable selection/UI contracts in this
translation. No emulator certification or newly resolved anatomy is claimed.

`rules.mechlub_upgrades` compares the entire 125-byte result for all eight
packages, checks exact costs/record isolation, refusal, insufficient funds,
completed-state gating, unsupported-package charge and signed corrupt price.
The original Yes/No body is linked; remaining UI adapters are test-only.

## Original whole-Mech salvage conversion

Sol: Converted the complete existing `Salvage_Mechs_Dialog` body. Bind characters,
Mechs, WORD casualty flags, saved name initials and sprite-family bytes to their
original global storage declarations. Keep signed Tech/name/assignment loads,
first strict best-technician selection, candidate RNG call order, circular scan,
flag consumption on rejection, catastrophic-damage thresholds and Excellent Tech
override. Original invalid-state infinite scan is NOT guarded: callers must have
at least one wreck before accepting, and infantry flags4..11 cannot be Mech wrecks.

Restore whole records by original byte offsets1..124, then restore the saved
initial, pilot/assignment/sprite family and minimum engine/gyro/structure state.
Preserve full-lance false success: the wreck is consumed and success announced
even when no slot can receive it. Exhaustion/inspection waits remain original
method calls; their timing/UI implementations are not certified by unit tests.

`DynamicString` is one contiguous view of native3092:0012..0063, ending before
the next known stream-cursor WORD at0064. This is a reconstructed memory view,
not a recovered original source allocation size or a new independent buffer
for each alias. Success/failure placeholders are indexes16/27 in the same array,
matching0022/002D. Future compass/other scratch users must alias this storage.
ASM explicitly passes3092:0012 even where inherited prose said DS:0012.

Converted original string entries3B22/3B68/3B9E rather than importing the
annotation's `DOS_Text_ScanRemaining`/`DOS_Text_CopyBytes` research helpers.
Re-read their clean ASM: scans include NUL for copying, length excludes it,
copy proceeds forward with two-byte read-before-write and returns destination.
Native scan ceiling is retained. Host pointer alignment chooses odd-byte prefix;
valid, nonoverlapping, non-segment-straddling strings are the port contract.
Malformed FAR wrapping and overlap-dependent native alignment are not equivalent
to ordinary host arrays; no safety check or contemporary replacement rule added.
These are pure runtime memory operations, not DOS device methods requiring SDL.

Used the prior salvage audit in repository `docs/preservation/0DAB_SALVAGE_REMAINDER.md`
and spot-rechecked clean05EA/070D/07B5 message/record/placeholder operations.
`rules.whole_mech_salvage` links real salvage/text/Yes-No bodies, with test-only
RNG/UI/wait adapters. Checks record copy, name trim and shared aliases, sprite
family, catastrophic rejection/Excellent override, retry, circular range scan,
signed ineligible assignment, refusal and full-lance false success.
This is synthetic behaviour checking, not observed gameplay or rendering.

**Component/material salvage at0DAB:0002 remains unconverted.** Its SRM-6 bucket
inherits BP-14 stack contents. Do not replace it with zero, randomness, a new
persistent gameplay global or undefined host-local reads. Faithful incoming-frame
binding remains required before that complete method enters this build.

## Native frame investigation and personnel loot

Sol: Rechecked 0DAB:0002 prologue and0328..033D: the 2Ah-byte frame is reserved,
but only BP-24h..BP-15h (16 bytes) are cleared for weapon buckets. SRM-6 is the
next byte, BP-14h, with no assignment before use. The caller at183B:1169 passes
no parameter supplying it; subsequent callee stack activity precedes the weapon
block. Static code does not establish one universal incoming value. Component
salvage stays out of the build rather than silently changing this native bug.

Converted complete personnel loot094B..0B5D and computer-control prompt0B5E..0B94.
Native cash is3+(RNG&15) per flagged enemy, minimum2 whenever either qualifying
casualty view is nonzero. Keep cash DWORD wrapping and original prompt order.
Jason exchange uses the converted original Yes/No body; Rex calls the original
`Distribute_Weapon_To_Party`, now converted in the next checkpoint below. No production
inventory placeholder was added. Fallen/current weapon BYTEs now receive CBW
sign extension; malformed weapon-table indexes remain outside valid host-array
contracts, rather than being clamped into a contemporary rule.

Verified native flag aliases from the exact addresses:
`393C+24*2` spans393C..396B; `3954` is its index12 and `395C` index16.
`DeadInfantryFlags` and `LootableInfantryFlags` are pointer macros into that one
array, NOT newly allocated duplicate flags. In loot, IDs4..11 through the3954
view are consequently enemy flags16..23. Friendly flags4..11 can trigger minimum
cash without an enemy cash roll. Material salvage's3954 Mech indices0..3 likewise
map to enemy Mech flags12..15.

Rechecked native0995's C6EB+ID*11h gate and0A7E/0B40 C6A7+slot*11h consumption.
For IDs4..11 the gate is precisely `Mechs` BYTE offsets11,28,45,62,79,96,113,130:
Mech0 name[11], structure[0], ammo[6], critical[11], critical[28], maxArmour[10],
maxAmmo[6], then Mech1 name[5]. This clarifies A-006's static binding, NOT the
author's intent or live gameplay reachability. Preserve that gate; replacing it
with `Characters[8+slot].weapon` would change the binary's behaviour.

Converted original183B:273D supporting-structure lookup, keeping the misleading
public name for traceability. Signed critical-offset thresholds choose structure
ordinals0/1/5/6/2/7/4/3; package-baseC8 returns1. Re-read its full clean ASM,
without inventing disputed anatomical names or changing C8's unexplained purpose.

`rules.personnel_loot_and_support` links these original bodies and Yes/No;
At this checkpoint test-only adapters supplied RNG/UI/Rex's pending distributor. Covers minimum/max
cash, alias effects, all eight exact Mech gate addresses, DWORD wrap, refused/
accepted Jason exchange, Rex's signed WORD handoff, computer-control default,
all35 supporting-slot mappings, signed offset and C8 bypass. This is synthetic
caller/contract checking; it did not certify Rex inventory or gameplay. The next
checkpoint replaces that distributor adapter with the actual original body.

## Original weapon-distribution conversion

Sol: Converted the complete `Distribute_Weapon_To_Party` body0FDC:15E6..17B8;
personnel loot now links it directly. Preserve the living-party WORD menu map,
Cudgel/zero sentinel, compacted rows, held/displaced weapon exchange, Drop it,
and warning AFTER the exchange. No inventory abstraction/helper was invented.
Native305B:00D6 is now declared as `EquipmentDistributionMenuOptionCount`; layout4,
column10 and warning-row20 use explicit UI names.

Native3092 character views bind to `Characters`; name/current/displaced weapon
BYTEs are sign-extended as native CBW/BYTE IMUL requires. Held/displaced equality
and continuation use explicit WORD bit patterns. Original valid weapon/name
indexes are required for host lookup arrays; malformed native FAR-table wrapping
remains outside their contract, not silently clamped or normalized.

Retain the peculiar zero/one-living shortcut: equip Jason even if dead/the sole
living member is someone else, then discard his previous weapon. A held or
displaced Cudgel terminates the chain. Choosing an already-held weapon warns,
but does not end the chain; the next menu can transfer or discard it.
Exact16B4 text is space-to-colon-CR (`" to:\r"`), confirmed from original EXE.
Read clean15E6..17B8 ASM's complete map/menu/shortcut/swap/warning sequence.

Removed the loot suite's fake distributor. Its real loot/inventory linkage now
tests Rex-triggered recipient choice, compacted nonadjacent party slots, chained
displacements, drop, duplicate warning, Cudgel sentinel, zero-living shortcut and
signed high-bit weapon through the direct-Jason branch. UI/menu/RNG remain
test-only adapters; no interactive menu or malformed table-index certification
is implied. Original `Btech` annotations remain unchanged.

## Original armour-distribution conversion

Sol: Converted complete `Distribute_Purchased_Armour`,0FDC:13DE..15E5,
from its annotated template and checked against the full clean ASM body.
Native3092 character accesses bind to flat `Characters`. The local eight-WORD
recipient map compacts living party records; PartySize names that bound.
Both armour type and remaining points exchange with the selected recipient.
Preserve signed CBW for displaced BYTEs and explicit WORD bit patterns for
continuation/equality. A displaced points BYTE128 therefore displays as WORD
FF80 on the next menu, then narrows back to BYTE128 when assigned.

The zero/one-living shortcut equips Jason even when dead and discards his old
armour. Duplicate-type warning occurs AFTER swapping, even when the new armour
has fewer points. It does not end distribution: the displaced suit and points
remain available for another recipient. None/zero ends the chain; Drop it
discards the held suit without changing a character. These quirks are preserved.

Resolved the six original FAR description pointers at3EDB:4E8A to EXE-owned
strings46A2/46A7/46B1/46BB/46C7/46D3. Confirmed exact prompts1661/1667/166D/168B
from unpacked BTECH.EXE SHA256
`F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE`.
No external game asset was copied. Description indexes retain the original
valid-input contract; malformed native FAR-table wrapping is not reproduced.

Armour and weapon menus share native305B:00D6 and layout4/column10/row20;
renamed their shared declarations to EquipmentDistribution names rather than
inventing a second independent global. `rules.armour_distribution` exercises
compacted recipients, chained points, drop, duplicate warning order, signed
points, None, full-party bounds and zero/one-living shortcuts. UI/menu/input
adapters are test-only; these are synthetic witnesses, not gameplay validation.
All9 headless and10 SDL-enabled test suites pass. `Btech` is unchanged.

## Original specialist recruitment conversion

Sol: Converted complete `Recruit_Crescent_Hawk_Agent`,11B8:0D58..104D,
from the annotated method, incorporating the systematic audit discrepancies
after reading the complete clean ASM body. First NameFF party record receives
three2D6 attributes, seven binary skill rolls, forced unskilled Piloting then
Good selected specialty. Piloting specialty reduces health by two points per
Body point. Names cycle2..9; the first occupied friendly Mech with a free
riderFF seat receives the recruit without changing its pilot. Full party still
restores the entered building's saved X/Y return coordinates.

Native CBW/IMUL391/CWD reseed sign-extends the WRAPPED low product WORD,
not the full32-bit product. Its4FC0/4FC2 WORD writes bind to the existing
RandomByteLow/Middle/High and the added4FC3 RandomStateUpperByte. No independent
seed variables were added. Original4FC3 EXE initializer is1;0BC0 leaves it
untouched. Negative/overflow name products are represented correctly by the
seed expression, but corrupted out-of-range name lookup is not certified.
Native signed assignment/name lookups and unsigned pilot ID multiplication
are retained. Skills traverse the whole record's BYTE representation at
offset4..10, with layout assertions rather than a new skill container/helper.

Preserve random advances while no input is available, then drain/read, then the
single still-eligible traitor coin flip. A failed flip leaves eligibility open;
an existing traitor suppresses another flip. Traitor ID stores the party slot,
not the name ID, with probability31. The five exact description fragments
1E2F/1E3C/1E4D/1E56/1E70, plus resolved character names,
were checked in the original unpacked EXE identified above; no external asset
was included. Valid specialty0..6, character/name/assignment and building0..15
indexes are required; native FAR offset wrapping for malformed inputs is not
replaced by host array bounds behavior or silently clamped.

`rules.specialist_recruitment` links original recruitment and2D6, with test-only
RNG/UI/input adapters. Covers all seven specialty offsets (shipped scripts use
Piloting4/Tech5/Medical6), attribute and skill order, exact descriptions, first/
last vacant slots, shared seed bytes, name wrap, passenger selection through
the fourth friendly Mech, no enemy seat selection, unsigned pilot record8,
input wait RNG consumption, successful/refused/existing traitor and full-party
return. Synthetic witnesses, not gameplay/RNG-stream validation. `Btech`
annotations remain unchanged.
All10 headless and11 SDL-enabled test suites pass at this checkpoint.

## Original arena staging and restoration conversion

Sol: Converted complete `Prepare_Party_Mech_For_Arena`1A26..1B40,
`Restore_Party_After_Arena_Combat`1B41..1C9A and
`Prepare_Rental_Locust_For_Arena`1C9B..1D2F from the annotated methods,
checking all three clean ASM bodies. Native FAR3092 record accesses bind
to the byte representation of flat `Mechs`; all copies remain forward
byte-by-byte over MechRecordSize125, including selected-slot-zero self-copy.
The shared3780 backup,430E/3FFA crew arrays,3FE9 name array,D452 initials and
E48E WORD mode retain their address provenance. No new arena algorithm/helper
or duplicated independent state view was introduced.

Owned setup saves four pilot/rider pairs BEFORE its selection UI; SelectedMechId
then selects the record staged in slot zero, whose name initial is recovered
from D452. Jason/no-rider replaces staged crew. Cleanup returns all combat
record bytes to the selected slot, propagates a destroyed name to D452, and
hides that live record with FF. If the selected slot is zero, that same record
stays hidden and retains damage; otherwise the original slot-zero backup is
restored. Four saved pilot/rider pairs are restored in the owned branch.

Rental setup replaces slot zero with the complete original Locust reference
without adding crew/position initialization. Cleanup discards rental changes
and restores the backup, but does NOT restore the owned branch's crew arrays.
Both setups preserve native interleaved current/previous-member assignment
stores, then name save/hide, for members1..7. Literal assignment3 is named
ArenaStagingMechAssignment: temporary Mech slot, not tabletop skill Good.
Cleanup restores names1..7 and dismounts all eight members. Jason's health,
world positions, selector and mode are not reset by these helpers. They neither
start combat nor preserve positions; those responsibilities remain with callers.

Exact selection prompt3EDB:17A5, including trailing CR, was checked against
the unpacked EXE identified above. Only the previously converted EXE-owned
Locust template is embedded; no external copyrighted asset was added.
Valid selected friendly Mech indexes0..3 and stable normal native storage
are required; malformed FAR address wrapping is not certified by flat arrays.

`rules.arena_staging_and_restoration` links all three original bodies with
test-only drawing/selection adapters. Full-record witnesses cover every owned
slot both surviving and destroyed, slot-zero aliasing, hidden-name recovery,
crew saved before UI, exact backup/template bytes, companion death/name state,
interleaved assignment outcomes, preserved Jason damage and positions, and
rental discard without owned crew restoration. These are synthetic state
checks, not arena combat/gameplay validation. All11 headless and12 SDL-enabled
suites pass. `Btech` annotations remain unchanged.

## Native BLD and graphics decoding

Sol: Continued towards the complete preservation game, not an invented bootstrap.
Converted BLD1D30 from its annotated method and full clean ASM, retaining disk1
selection followed by disk2 for IDs below2 or at/above17, the3FF8 WORD file index,
the9000-BYTE shared buffer at00A0 and4594 WORD BTSTATS invalidation AFTER decode.
Addition41 wraps to BYTE before XOR233; the whole fixed span is decoded even
beyond the loaded payload. Native filename SHL/SHL wraps the table byte offset
to WORD, so4000/8000/C000 aliases of valid entries remain valid. Arbitrary reads
beyond the26-entry filename table are outside the host contract. All26 EXE-owned
filenames were extracted from4EC2; no BLD file contents were embedded.

05F7 and19F6 were read in full ASM and converted as the original distinct
little-endian operand readers, not merged into a new research helper. Both
zero-extend the recombined bytes and do not advance the caller's pointer.
`assets.original_bld_decode` exhausts all65536 WORDs, exercises every normal
filename and its three wrapped aliases, disk/loader/cache ordering and every
decoded buffer byte including untouched preexisting tail. Disk selection and
length-prefixed loading are still unconverted test adapters here: this is not
end-to-end asset import confirmation or BLD bytecode execution certification.

The original207F22F8/2368 ASM entries have no maintained pseudo-C bodies.
Reconstructed their complete loops directly from clean ASM rather than importing
the bounded InceptionTools decoder. Positive BYTE counts copy literals; negative
BYTE counts repeat their magnitude; zero commands read a WORD repeat count.
WORD count0 follows native LOOP semantics, executing65536 times unless output
exhaustion stops it first. Both methods stop immediately after32000 output BYTEs,
even midway through a run. Format2 writes200 rows at stride160 before advancing
one BYTE column, retaining the original31999 subtraction. The output represents
320x200 packed4-bit pixels, not a host RGBA framebuffer. B78A/B78E shared FAR
scratch pointer writes bind to GraphicsTransferSource/Destination flat globals.

1F3D049D reads one marker BYTE: exactly1 selects format1 and every other value
selects format2. Maintained EGA-only code omits the already removed adapter0
post-conversion. No source-length validator, short-output guard or bounded
replacement algorithm was added. Callers must supply sufficient storage. Native
source offset arithmetic is WORD-sized; flat-pointer bases do not reproduce
arbitrary segment-boundary aliasing, malformed FAR pointers or CPU register/DF
side effects. These constraints are not claimed as whole-game equivalence.

`assets.original_graphics_decode` uses only generated streams: all256 markers,
extended zero/FFFF counts, positive127/negative128/negative1 runs, transitions
across columns, full literal output, source preservation and destination guards.
This proves synthetic native command/state witnesses, not original art ingestion
or rendering. Next asset work must connect actual file loading and original-file
fixtures. All13 headless and14 SDL-enabled suites pass; `Btech` is unchanged.

## Native disk requests and real asset-file integration

Sol: Converted28CC and2913 from the annotated template and complete clean ASM.
28CC stores014E and, absent hard diskD580, selects driveA before selectingB only
if3FFE is nonzero and the requested SIGNED WORD is greater than1. Incorporated
the audit correction for8000..FFFF. 2913 saves4600 menu layout, uses layout4,
draws, sets maintained EGA colour12 and shows the exact04BA/04CF/04D9/04E0/04E5/
0504/04EF fragments. Confirmed04EF's trailing06/0F controls and0504's mutable
drive letter at+6 from the original EXE. Drain/read precedes redraw/menu restore;
the full WORD key is returned. No extra colour/selector reset was invented.

Converted complete063B and0814 from their original templates/ASM. Both open in
native runtime binary read-only mode8000, recognize only handleFFFF as failure,
prompt/retry, then ignore read/close status. 063B receives its two-BYTE native
SS:BP-2 prefix in a local byte array and explicitly rebuilds little-endian WORD
length before reading the payload; returns1 after completed open path, not a
payload-integrity result. 0814 has no length prefix and reads the requested count.
The prefix local is deliberately not assigned a made-up initial value. Short
headers leave native stack bytes indeterminate; truncated-header host execution
has no supported contract. Valid original headers and short/failed payload reads
retain their original ordering/status handling. Exact arbitrary native stack
residue would require an explicit frame binding, not uninitialized host claims.

`assets.original_disk_and_raw_loader` exhausts all disk WORDs against both
installation flags; checks all four normal disk/drive prompts, saved layout,
WORD key return, failed-open retry, zero-count/read failure, exactlyFFFF gate,
prefix-before-payload, ignored short/error payload and ignored close failure.
The original OS drive method and remaining UI are test adapters here. Production
drive mounting/UI remain pending; they are not silently supplied as no-ops.

Opt-in `assets.local_original_files` links actual native BLD loader, both native
graphics decoders, length-prefixed loader and SDL-backed DOS file reads. Ran
against local ignored `Chinception`: all26 BLDs load/decode with the native9000
span and cache invalidation, and all12 CMP/ICNs load and decode32000 packed bytes
without destination-guard writes. Independently ran the existing InceptionTools
CompressedImageDecoder via its source on the same files; every packed-output
FNV1a32 checksum agrees. Only expected checksums are committed. No copyrighted
file was copied/modified. Tests opt in via Build.ps1 -OriginalAssetDirectory or
CHI_ORIGINAL_ASSET_DIRECTORY; without assets the integration suite is not registered.
Fixture checksums describe this installation, not every possible game revision.

This is real asset ingestion/decoder agreement, not verified rendering or
gameplay. Input/UI and removable-drive selection remain test adapters. Native
ICN/CMP palette/plane presentation, BLD execution, map construction, main loop
and combat still require conversion/integration. All14 headless and16 SDL-enabled
suites (including the local integration suite) pass. `Btech` is unchanged.

## Native input and attract-mode bridge

Sol: Converted original002F,0259..031B and086A plus0800's2A2B/2A4F/2A69/2A7E
from their annotated bodies and clean ASM. Pending_Input probes live input FIRST
even when replay3938 is nonzero, returning WORD0/1. Keyboard_Get_ASCII_Hex_Input
retains full returned WORD bit patterns; its legacy unsigned public declaration
represents native signed keys by explicit casts. Live input returns immediately
unless458C recording is enabled. Recording normalizes lowercase h to H, stores
the low BYTE in the SAME00A0 shared buffer used by BLD/BTSTATS, advances39F8,
and continues fetching when H is recorded. No separate demo buffer was invented.

Replay CBW sign-extends each BYTE; H waits30 retraces and continues. After an
ordinary byte, live input interruption sets3EDB0152 and consumes one real key,
then waits1 retrace; P also sets0152. 2A2B returns immediately during replay,
otherwise probes/consumes until empty. 086A waits50 retraces then invokes that
original drain, not a50Hz rate setter. Continue/suffix/period preserve fixed
050D/051B/051D text. Exit/index/record globals retain native WORD widths.
Replay indexes must stay within the reconstructed9000-BYTE shared buffer;
arbitrary native segment-offset wrap/aliases outside it remain unsupported.

The DOS/runtime207F3BDC availability body is gutted to a separate SDL queue
probe, without carrying forward the obsolete DOS runtime buffer at DS5366.
SDLBackend_InputPending peeks key-down/quit events without consuming them.
Corrected the prior SDL quit poll, which discarded queued gameplay keys and
mistook menu ESC for application shutdown. It now consumes only actual quit
events; Escape remains available to the original game input. This is platform
replacement behavior, not an original game bug fix.

`ui.original_input_bridge` links all seven original methods and uses test-only
keyboard/availability/retrace/text adapters. Covers every live WORD, all non-H
replay BYTEs, H/h recording, signed extended-key recording/replay, repeated H
waits, interruption, P, drain gate/order, fifty-retrace pause and exact text.
Platform suite checks repeated non-consuming availability, quit-poll key
preservation, Escape and quit-event availability/consumption with actual SDL
events. Existing service tests retain their isolated input adapters; these new
checks do not claim every interactive menu or the original DEMOFILE has run.
All15 headless and17 SDL-enabled suites including local original assets pass.
`Btech` annotations remain unchanged; full main/UI/map/combat integration remains.

## Native numeric formatting and text wrappers

Sol: Converted3BB6 and3BD2 wrappers with their complete native3C15 shared
tail after reading clean ASM through3C6A. Since3C15 is NOT an independent
FAR C ABI, inlined its actual operations in both public routines instead of
copying the annotations' research-core helper. Retain decimal-only CWD/sign,
NEG/ADC/NEG across WORD pairs, two-WORD division, BYTE digit wrap/39 adjustment,
zero's single digit, NUL and at-least-once digit reversal. Native CLD maps to
forward C accesses; no emulated CPU FLAGS binding was introduced.

WORD decimal8000..FFFF prints negative, although the old1F3D0053 summary
incorrectly claimed unsigned display. Converted0053 invokes the original
3BB6 on DynamicString0012 then renders. DWORD C-bills remain unsigned stored
values but3BD2 decimal interprets bit31 as sign, including80000000's minimum.
Nondecimal WORD input zero-extends and DWORD input retains all32 bits.
No modern snprintf implementation replaces these original game methods.
Radix0 faults and radix1/nonzero fails to terminate natively; no invented
validation/rejection policy is added. Valid caller buffers are required;
malformed host executions/segment-wrapping text are not certified.

Converted3C6C as Native_Abs_Word rather than colliding with modern libc abs.
Its signed WORD minimum remains8000 after NEG. Converted2867 and maintained
EGA28A2 from their full clean ASM: WORD cursor writes, white colour then text,
and green colour respectively; no extra restore or removed CGA path.

`ui.original_numeric_text` independently compares all65536 WORD decimal/hex
outputs against test-only libc formatting, checks every WORD abs and dynamic
scratch rendering, DWORD boundaries plus1024 deterministic values, base2/36,
native unusual-radix BYTE wrap, zero and positioned text/colour effects.
Existing service suites keep their isolated rendering adapters until the full
text renderer/menu path is integrated. All16 headless and18 SDL-enabled suites
including local original assets pass. Original `Btech` remains unchanged.

## Personnel movement and combat settings

Sol: Converted2474..24EF,24F0..2555 and2556..2590 from the annotated
methods and all three clean ASM bodies. Personnel movement accepts combatant
IDs friendly4..11/enemy16..23, not character-record IDs. Enemy personnel receive
fixed6, ignoring character stats/armour. Friendly movement is floor(3*signed
Dexterity/4), clamped to minimum3 BEFORE armour. Signed armour type>FlakVest
halves that result, and maximum8 applies LAST. A low-stat armoured character
can therefore receive1; high Dex with armour is halved before the cap. Explicit
floor division preserves the native two SARs without relying on host negative
right-shift semantics. No Mech heat/actuator state is reset by this method.

24F0 sets layout3, draws, prints choices and sets native305B:C2/C6/C8 to
2/3/current default, obtains a full WORD choice, clears ONLY C2 and returns.
It does not store the setting; the parent does. 2556 uses layout3 and the
original Yes/No routine with current2E3A default; again the parent stores it.
The clean ASM confirms call/write order and full WORD returns. Resolved the
EXE text3D3A/3D5E directly:3D3A has NO CR after Verbose, contrary to the
annotated literal and its stale verification claim. The new preservation
body uses exact bytes. Original2E38/2E3A initial values are2/1 and are retained
in the converted data. Renamed the effects-settings public method to
SettingsMenu_SeeCombatGraphics, removing the misleading bool suffix while
retaining its native WORD contract and address provenance.

`rules.personnel_movement_and_combat_settings` links all three methods and the
actual original Yes/No body; rendering/menu/key calls are test-only adapters.
Covers every BYTE dexterity/armour pair for every friendly slot, all enemy
personnel IDs, setting choices including FFFF without invented validation,
temporary-line reset/default persistence, both graphics defaults and Y/N,
and colour restoration through Yes/No. It does not certify interactive menus,
collision/path movement or headless combat; those remain to be connected.
All17 headless and19 SDL-enabled suites including local original assets pass.
Original `Btech` annotations remain unchanged.

## Critical damage and component support

Sol: Converted the complete1631:11AB dispatcher and15FA/163E/1B44/1DAB
support bodies against their original ASM. Critical damage takes a Mech record
ID and raw structure offset1C..23. A roll below8 STILL gives one critical;
8/9 give one,10/11 two,12 three. Destroyed sections still roll and notify,
mark every nonempty slot in their exact EXE table group, clear only the limb
actuator nibble and do not newly set the whole-Mech destroyed flag.
Head and central systems retain native retry gates, BYTE counter overflow,
and engine/gyro destruction thresholds. A missing randomly selected actuator
retries without consuming a critical. Empty torso groups end the sequence;
empty head/central component groups can retry. No modern tabletop correction
or defensive random-search limit replaces the original behaviour.

The component helper chooses RNG&3, normalizes before dereferencing and scans
cyclically, rather than selecting RNG modulo group length. Signed negative
counts produce no intact entries and consume no RNG. The low-actuator helper
counts only missing bits4/2/1; bit8 is deliberately excluded. Exact EXE-owned
hit/clear masks and section tables now live in original data globals.
The message is the exact native `\rCritical!`, with no invented trailing CR.
Only message setting None hides it; Brief and Verbose both allow it.

`rules.mech_critical_damage` links the actual dispatch/support and D6/2D6
bodies with scripted RNG and UI-only adapters. Covers all destroyed sections,
all actuator BYTE values, component normalization, sensor/head destruction,
gyro/engine destruction and overflow, absent-system retries, repeatedFF life
support, arm/leg masks and torso/head/central component selection. These
branch witnesses are not full headless combat or gameplay validation.
Original annotations remain unchanged. Both strict C17 builds pass, including
18 headless and20 SDL-enabled suites with the private original-asset check.

## Combat occupancy, crushing and relative packed movement

Sol: Converted the whole1631:16AB..1B43 occupancy/crushing routine,1DCC speed
control and0800:191B relative-position body. Read each complete clean ASM body
and compared the annotated control flow and loads/stores. The occupancy WORD
return is1 blocked,0 allowed. StepX/StepY are never read; callers must already
set the shared proposed packed position. The infantry exact-position scan
uses friendly4..11/enemy16..23 and ignores active flags. Its three-cell Mech
footprint scan also ignores active flags, temporarily moves the shared anchor
through packed left/right cells, and restores it even after finding a block.

Mech occupancy checks active other Mechs on both sides, excluding self, with
centre distance<3. Active same-side infantry within distance<2 block. Opposing
infantry do not block, and are only crushed when the explicit execution flag
allows it. Crushing retains native record translation, menu/drawing/message/
sound/delay/effect order, impact sprite7E, zero sprite-family base, inactive
state, zero health and dead-name byte. Jason/Rex death clears the shared main-
characters-alive WORD. Packed positions and casualty/loot flags are NOT reset.
The Mech move still returns allowed after crushing, and scanning continues.

Converted data adds the original packed/active/frame tables and exact EXE
initial main-character/speed WORDs1/2. The signed speed gate uses setting*12
retrace waits below5, including low-WORD product wrap; otherwise it waits for
a key. The packed offset routine consumes signed Y deltas before X, with
128-cell local carries and distinct X/Y region correction, and does not
rebuild any cache. Constants describe those native encodings and footprints,
not newer tabletop movement rules. The exact message bytes are unchanged.

`rules.combat_occupancy_and_crushing` links real occupancy, packed movement,
WORD abs, verbosity and speed methods. UI/sound/effect/key/retrace calls are
test-only adapters, not production placeholders. Covers all24 actor IDs,
inactive-entry quirks, self exclusion, both sides' footprints, preview versus
execution, all friendly record deaths, Jason/Rex flags, enemy record8 mapping,
position/casualty retention, quiet messages, speed5 skipping crush waits,
all65536 speed WORDs, region crossings and shared-anchor restoration.
This is not a gameplay recording or the promised full headless combat flow.
The persistent-effect producer still needs the audited D557 WORD increment
and adjacent D558 alias incorporated when it is converted, rather than simply
copying the annotated BYTE increment. Original `Btech` remains unchanged.

## Terrain path and compass calculation

Sol: Converted entire1631:1BFE terrain-path check,207F:0971 direction and
207F:1DF8 origin routines after complete ASM-body comparisons. Added exact
EXE-owned direction lookup246C:0240 and five signed-WORD heading tables
3EDB:328A..32D9, with native blocking threshold0150=55h. The routine builds
the local terrain cache then updates its origin before the rental target13
exception; actorId is unused. AX1 means terrain-clear, AX0 blocked, opposite
to the separate occupancy routine. Even a coincident target checks its
starting tile. Tile comparison uses signed WORD threshold and zero-extended
BYTE tile. No occupancy test, actor movement, budget or artificial pathfinding
limit is introduced. Legal callers must provide the required cache coverage.

The path gradually turns one heading toward the desired sector: differences
1..4 clockwise,5..7 counterclockwise. Packed axes use the original heading
correction tables on local bit80 crossings. X and Y parity separately control
when a24-column cache cell is advanced. Cache origin uses only each low BYTE
shifted/masked, plus the original2-cell margin; path begins another6 rows/
6 columns inside it. Parent map/cache construction remains to be converted;
this method references the native1314 producer, not an invented replacement.

Direction stores all four native comparison globals, sign-extends the BYTE
Y difference, then applies region corrections as WORD operations. Do not
re-sign-extend the corrected value as a BYTE or smooth the result into a
modern vector heading. Wrapped WORD negation/doubling and signed comparisons
select overlapping cardinal sectors, then signed lookup FF returns-1 for
coincident/invalid flags. Host shifts use unsigned intermediates to avoid
undefined signed-shift behaviour while preserving the native result.

`rules.combat_terrain_path` runs actual path/direction/origin bodies with only
the unconverted1314 cache producer adapted in the test. Covers an independent
ordinary-coordinate sector oracle, heading scratch stores, native region
correction witnesses, every256x256 low-coordinate origin pair, every BYTE
starting tile, every WORD threshold, cardinal/diagonal half-cell sampling,
all start parities and nearby target deltas, gradual turns, packed east/west
crossings, unchanged actor anchor and rental-call-order bypass. It does not
validate procedural construction, rendered terrain or a full combat round.
All20 headless and22 SDL-enabled suites pass, including private original
assets. Original annotations and copyrighted external files are unchanged.

## Original local terrain-cache producer

Sol: Converted207F:1314..13D8 and its native NEAR13D9..158B helper after
reading both complete ASM bodies. The first expands nine neighbouring8x8
blocks into the24x24 cache and stores nine combined selector/terrain flags.
Local block coordinates use only the low packed-coordinate BYTE shifted4.
The original saved DI is now a local cache-relative WORD index; the NEAR
helper's in/out DI is an explicit index reference, not a new CPU-register or
research-helper global. The native unrolled calls are expressed as three
rows/columns with the same order, BYTE coordinate wrap and output-band strides.

The helper selects parallel adjacency/descriptor cells, handlingFF/8 border
coordinates by redirecting into the proper neighbouring64-byte grid. Terrain
high-bit descriptors bypass reflection/translation;10h selects the dedicated
209Dh block; other nonzero descriptors subtract10h and clamp the BYTE category
base to30h. Reflections transform both template selector and applicable tile
IDs, reversing rows/columns as the ASM specifies. Fixed IDs40h and above are
unchanged. Native CLD is represented by explicitly forward output accesses;
there is no host CPU FLAGS state or research memory accessor in these methods.

Added exact EXE-owned246C:0C1D..217C bytes in `BTECH_MAP_DATA.c`: block templates,
special block, reflected workspace and overlapping horizontal/vertical lookup
views occupy one shared array. These are binary-owned data permitted in source,
NOT imported copyrighted external assets. FNV1a32 initial-span witness80E63AFF;
source expanded-EXE SHA256 is recorded beside the initializer. Adjacency and
descriptor caches are original mutable globals. The procedural corner-builder
and map-file loading/scrolling that fill them are still pending; no invented
generation/bootstrap routine fills them in production.

`maps.original_tile_cache` runs actual producer/helper/path bodies with no
game-method adapters. Tests every native direct-copy block, overlapping lookup
storage, all256 terrain bytes across four reflection phases, fixed IDs, category
underflow/clamping, special/high-bit paths, strided-output canaries, neighbouring
edge/corner selection, nine-block assembly/metadata and generated clear/blocked
combat paths. Fixtures for caches are synthetic; this does not yet certify
procedural maps, scrolling, real-level loading or a rendered gameplay capture.
All21 headless and23 SDL-enabled suites pass, including the private original
asset suite. The original annotated files and external assets remain untouched.

## Native procedural block subdivision

Sol: Converted complete207F:0BFB..0FED: original block builder and horizontal,
vertical and rectangle NEAR subdivision routines. Read all four complete ASM
bodies and used the annotated code as the template, without its extracted
research seed/noise/amplitude helpers or raw memory accessor. Native SI/DI
inputs become explicit descriptor/lattice pointers and a work-stack index
reference. The shared0279 work stack and0272..0278 scratch are original global
storage; the latter now also backs render-phase/reflection aliases rather than
duplicating their overlapping memory. Original09FB seed bytes are EXE-owned
data, not RNG-generated replacement terrain or external assets.

The builder preserves four corners, clears only the first80 of81 lattice
bytes, reseeds each outer edge from its BYTE endpoint sum and processes top,
bottom,left,right. Interior continues the right-edge stream. Horizontal edges
subdivide right-first, vertical bottom-first and rectangles bottom-right-first.
Only anFF midpoint consumes a seed. Centre means are unperturbed WORD sums;
edge perturbations wrap to BYTE before the high-bit-to-zero gate. Vertical
amplitude uses SHR3 and the literal CMP9/DEC correction, NOT division by9.
Seed advancement increments only the low BYTE without carry; parent reseeding
clears the high BYTE. Separate intended buffers and legal construction frames
are required, rather than inventing arbitrary-aliased-memory equivalence.

Preserved native interleaved child-stack stores, DH influence on bottom/right
means, wrapped WORD addition before SHR and the right edge using halfWidth
where a modern algorithm would likely use halfHeight. The output masksF0h
from the upper-left8x8 of the9x9 lattice; final row/column are shared boundaries.
CLD is embodied by explicitly forward C accesses; no research CPU flag state
or new generation helper is added. Parent full-region construction and
adjacency generation still need conversion before map loading is complete.

`maps.original_procedural_block` checks eight full81-byte fixed lattice
witnesses and final seed indices, using the independent recursive/carry-
factored model in `Verify-MapConstructionTranscriptions.ps1` and native seed
bytes. These are synthetic model witnesses, NOT emulator dumps or gameplay
certification. Includes all-zero/high-bit/FF/mixed corners, buffer canaries,
the40-byte actual rectangle-stack peak, already-filled midpoint gates, seed255
wrap and both edge helpers. Actual procedural descriptors then feed actual
tile-cache construction and terrain-path checking without game-method test
adapters; adjacency fixtures remain supplied until that native routine ports.
All22 headless and24 SDL-enabled suites pass, including private original
assets. Original annotations and external files remain unchanged.

## Nine-region construction and adjacency

Sol: Converted full104E,11BB,12BA,12D9,12F2,1303,1886 and18D8 from the
annotated template and complete original ASM bodies. The native104E unrolled
nine blocks become the same row-major schedule with vertex stride16, descriptor
stride64 and origin region-17; native1886 then rebuilds all adjacency selectors.
The corner helper retains four ordered BYTE loads/stores into the9x9 lattice.
Normal descriptors select W8/S4/E2/N1 equality bits; high-bit descriptors encode
direct selector value-80h. Only80h on top/bottom interior columns retains the
original equality exception. Vertical helpers INC their supplied BYTE, not OR1,
including carry/wrap, before the south OR4 operation.

Reconstructed original adjacent246C:02D3..09EC storage as a BYTE-only checked
`MapCacheStorage` with named lattice/adjacency/descriptors/terrainFlags/tiles
members. Public array names remain views of their actual fields. Outer-grid
adjacency reads intentionally reach earlier adjacency or later flag/tile bytes;
separate arrays with negative indexing or invented zero-border tiles would
change native behaviour. The builder processes sequentially so earlier selector
stores affect later outer reads exactly as the original schedule specifies.
Static assertions check physical relative offsets and total span.

Likewise exact EXE-owned09FB..217C storage now groups construction seeds, the
16-byte gap, the0B0B..0C1C vertex view and prior block-template/reflection views.
This permits legal0..255 region construction to retain edge reads into adjoining
seed/template bytes. The vertex view size274 describes the native span, not
proof of the original authors' allocation or a new map dimension. No boundary
clamping, zero padding, research raw-segment helper or external asset is added.
Complete map-file loading/patch application and scrolling remain to be ported.

`maps.original_adjacency` compares288 complete cache images against a separate
neighbour-coordinate oracle, including outside reads and sequential writes,
all256 descriptor values,80h exceptions and mixed caches. Also covers all
BYTE value/flag combinations for both vertical helpers, overlap reachability
and every256 region ID's corner placement and final lattice. The procedural
integration test now calls actual adjacency generation before actual tile-cache
and combat-path routines. These synthetic witnesses do not certify real level
patches, gameplay navigation or a full combat sequence.
All23 headless and25 SDL-enabled suites pass, including private original assets.
MSVC14.51 repeatedly crashed incremental links with LNK1000/IncrBuildImage
after data-layout changes. Preservation test executables now use full links
with Debug information intact; both builds passed after that configuration
change. No original game behaviour or SDL-library build settings changed.

## North/south cache scrolling

Sol: Converted full207F:158C/163B against both complete ASM bodies. Within
a region only the packed Y low BYTE changes; world-edge attempts leave all
state unchanged. Crossing shifts384 retained descriptor bytes with the native
backward/forward overlap order, rebuilds the exposed three blocks from native
vertices, then all adjacency, then writes the three pending slot/region BYTEs.
No extracted research shift/regeneration helpers are included. Queue regions
wrap as BYTEs; vertex addresses use WORD arithmetic. Explicit copy direction
represents native STD/CLD. Tests check retained cache rows, both queues, world
edges and noncrossing cache preservation. All23 headless and25 SDL-enabled
suites pass. Horizontal scrolling and actual queued map-file ingestion remain.

## West/east cache scrolling

Sol: Converted207F:16E3..17C4 and17C5..1885 from their complete ASM bodies
and annotated flow. West copies middle->right before left->middle for each
row; east copies128 bytes per row from middle/right to left/middle, with
explicit forward overlap order. Only region crossings regenerate the newly
exposed three blocks, rebuild all adjacency and replace pending slots/regions.
Normal local movement and world-edge rejection leave cache/queues untouched.
Original INC/DEC AL sign-bit gates are retained rather than inventing equality
checks against local127/0. Region queue arithmetic narrows to BYTE; vertex
offset arithmetic remains WORD, with original native border alias storage.
No extracted research shift or regeneration helper was copied into production.
Tests exercise both column schedules, slots0/3/6 and2/5/8, region increments16,
packed coordinate transitions, blocked world edges and noncrossing preservation.
Queued map-file loading/patching remains the next missing map integration.

## Original streaming view entry points

Sol: Converted0800:17BB and the complete unpromoted1817 raw ASM frame.
Absolute target positioning uses unsigned packed comparisons, completing Y
before X through the four original scrolling routines. Signed relative movement
consumes argument-copy counters, also Y before X, including steps rejected at
world edges. No direct-assignment replacement, research helpers or new bounds/
iteration guard is introduced. Unreachable absolute targets retain native
nontermination. Tests exercise combined X/Y region crossings in both directions,
final pending queues proving axis order, and finite relative world-edge rejection.
All23 headless and25 SDL-enabled suites pass. The completeMTP loader still
requires its tileset, NPC and interactable-table contracts incorporated before
queue consumption can run end-to-end; this checkpoint does not claim it loads
real levels or completes a gameplay navigation flow.

## Headered MTP loading and roaming-NPC initialization

Sol: Converted the complete0800:2DA8..320A against its raw ASM, including
tileset selection, disk selection/retry, the five individual header BYTE reads,
all eight metadata reads,4096-byte tile request, descriptor placement, Starport
patch dispatch and eight NPC initializations. Signed JL/JLE map-number gates
are explicit; high-bit values retain signed decimal filename formatting.
Ignored read lengths preserve native stale working tails. Truncated headers and
invalid slot/waypoint indices remain unsupported native-input contracts rather
than newly invented validation or default values.

Map tile payload246C:101D aliases the existing template storage at+0400;
it is not a separate array. Descriptor grids replace the nine native FAR
pointer views with pointers into the already contiguous descriptor cache.
NPC record stride is26 bytes with the first ten fields understood and the
remaining16 bytes left uninterpreted/untouched. Live NPC positions alias the
existing combatant packed-position arrays, not duplicate world-state arrays.
Overhead loads do not consume RNG or alter NPC state. Maps11+ suppress live
NPC positions while retaining initialized routes, using the original signed gate.

The removed207F:00D1 is CGA-only and makes no writes for the retained EGA path.
Actual tileset drawing, BTTLTECH loader and Starport patch methods are declared
pending conversions; their test-only adapters are not production implementations.
Compressed/decoded storage is native working memory, not new gameplay logic.

Tests cover placement offsets, metadata, short payload tails, NPC records and
parallel tables, overhead gating, signed high-bit inputs, retries and DESTRUCT
loading through the actual original length-prefixed loader/decompressor.
Opt-in local integration loads all14 headered originalMTP files and compares
each metadata field, payload, untouched tail and generated descriptors.
MAP15 is the separate raw32x24 star map, loaded by135D:055A via0814, not this
method. Local MTP test file access uses test-only C17 stdio OS adapters; it is
not an SDL gameplay/rendering comparison. All24 headless and27 SDL-enabled
suites pass. No external assets were copied or committed. Full gameplay remains
incomplete, particularly rendering, map-stream queue consumption and startup.

## Original tileset restoration and Starport tile patch

Sol: Converted0800:4621..46A6 and1543:0C72..0CDD from complete raw ASM.
BTTLTECH restoration preserves compatibility write, disk2 selection, original
length-prefixed loading, decompression, EGA staging dispatch and final tileset ID.
Starport patch binds the native246C:1C1D view inside shared tile/template memory;
it saves every affected byte and applies only nonzero overrides. Any nonzero
restore argument restores all38 saved bytes. Reapplying saves already modified
tiles, so a subsequent restore does NOT invent an original unpatched recovery.
The override initializer is the actual EXE-owned3EDB:2B8E data.

Map-loader tests now call both actual methods, removing their test-only adapters.
They verify save/apply/restore, zero overrides, repeated apply, tileset restore
disk order and the actual file/decompress pipeline. Local integration additionally
loads actualBTTLTECH.ICN via4621 and checks existing verifiedFNV C3537AD1.
Only final EGA hardware staging remains adapted in this test, not certified
rendering. All24 headless and27 SDL-enabled suites remain passing.

Sol: Follow-up storage reconciliation: the final NPC26-byte stride reaches
D450..D45F. Known globals atD450/451 (medical equipment), D452..455 (saved
Mech initials) andD456 (next recruit name) now alias those actual bytes rather
than having independent copies. They are untouched by map NPC initialization,
as tested explicitly. The stride is a native address view, not an assertion
that its trailing bytes belong to an independently allocated NPC record.
Remaining effect-state aliases atD457 onward will be reconciled when converted.

## Exploration movement and map-stream queue consumption

Sol: Converted0800:218F..231C against the complete raw ASM, plus207F:1DA8
as its actual1886 call. Direction commands retain their sign-extended WORD scan
codes. The two queue loops are original intentional sequencing: consume the Y
crossing before X reuses the three entries. Zero map-file lookups still consume
their queue slots. Both loader calls CBW-sign-extend the BYTE map ID; raw80
therefore passesFF80 and formats MAP-128.MTP, not MAP128.MTP.
Blocked interactions skip movement/queue consumption but still expand1314 and
update1DF8. No research queue-drain helper was copied into production.

MapFileByWorldRegion is the actual EXE-owned256-byte2FE8:0030 table,
mutable as in the original scripts. Tests call the actual movement, four scrolling
methods, nine-region construction, MTP loader and adjacency/tile expansion.
A diagonal South-East corner crossing completes six loads: three with old X/new
Y followed by three with new X/new Y. Other cases cover all eight commands,
unsigned-low-BYTE noncommands, zero-file procedural crossings, signed map IDs,
consumed slots and blocked cache refresh. Interaction handling and hardware
staging remain test-only adapters; this does not yet prove live UI movement.
The local asset suite additionally checks all14 headered levels and real
BTTLTECH restoration. All24 headless and27 SDL-enabled suites pass.

## EGA image upload and tile capture hardware redirects

Sol: Converted raw ASM207F:0260..0312 and0313..0376 to original-entry
redirects with separate SDL hardware implementations, following the requested
DOS/hardware boundary. The upload consumes32000 packed bytes and affects8000
columns in four shared64KiB EGA planes. A400/A800 remain offsets within the
same aperture. Pixel nibble/plane ordering and final mode2/zero-mask state are
retained, including inherited map-mask/raster-operation effects. No palette
remapping or invented gameplay draw workflow is added.
Capture's native destination FAR offset/segment become a resolved host pointer;
its source arguments remain byte-column and tile-row WORDs. It reads32 bytes
as eight rows of four planes, preserving read-map-select3 at exit. Legal native
aperture/rectangle inputs and host buffer capacity are the retained contract.

The new graphics suite checks all pixels across all three segment views,
map-mask/XOR retention and tile capture ordering/canaries. Local integration
uploads each of the12 original decoded images through the actual redirect and
compares every staged pixel. The actual BTTLTECH game loader additionally runs
through SDL file access, original decompression and this hardware redirect.
All24 headless and28 SDL-enabled suites pass. Visible presentation, palettes,
sprites and remaining controller operations are pending; no complete-game or
emulator-rendering equivalence is claimed.

## Original EGA palette orchestration and port redirect

Sol: Converted1F3D:0525..05BB retained EGA body055D..059E and hardware
entry207F:022A..025F against complete raw ASM. The original method waits
first, then writes sixteen palette registers. CBW/signedJLE means BYTE128..255
are negative and do not receive the+8 adjustment; BYTE8..127 do. Both native
WORD arguments are narrowed to BL/CL in the hardware redirect. The backend
retains the six-bit palette-register values, not a guessed RGB conversion.

Related width correction:3092:32AC retrace state is WORD storage, not BYTE.
Wait_For_N_Vertical_Retraces and palette setup explicitly narrow that state
because207F:0B40 actually reads BL. Full WORD declaration is now correct.
All256 possible palette BYTE values are tested with wait/write ordering;
SDL integration calls the actual original palette routine and port redirect,
including high-bit inputs and WORD-to-BYTE narrowing. All25 headless and29
SDL-enabled suites pass. Visible colour mapping/presentation remain pending:
the original320x200 monitor/adapter palette interpretation must be confirmed,
not inferred from convenient generic six-bit RGB or four-bit EGA palettes.

## Complete cached-map viewport composition

Sol: Converted207F:18EF..1AA7 and retained NEAR tile wrappers1AA8/1ACE/1AF4,
plus1B71/1BDF/1C83 transfers, against their complete raw ASM bodies. The original
parent remains in Original, with its scratch globals, cache origin, ordering and
edge flags. It composes the right216 pixels (27 byte-columns) atX104 intoAC00,
using thirteen full16-pixel columns and one8-pixel side. Twelve16-row bands
and one8-row edge fill200 lines. Odd map-coordinate parity selects a leading
right/bottom half; even parity leaves a trailing left/top half. Native1959 CLD
is explicit forward child accesses, not copied research FLAGS helpers.

Native row gaps are named expressions (16*40-27 and8*40-27), not unexplained
613/293 loop increments. Full/half source tiles resolve atA400 using original
WORD offset shifts. The separate SDL boundary implements a mode2/mask0
four-plane latch transfer, loading all source planes before destination writes
and preserving the inherited plane-enable mask. No extracted research map or
renderer helper enters production. Hardware header is SDL-type-free; Original
remains independent of SDL3 types. The full renderer is compiled in headless
archives too, with hardware linkage required only when the method is used.

Tests check256 combinations of localX/Y0..15 against independent viewport
geometry, all32000 plane bytes per case, edge halves, untouched sidebar and
postscreen guards, plus final original scratch/controller state. Local integration
now executes each of14 actualMTP files through native loading, adjacency rebuild,
cache expansion and this actual renderer with realSDL IO/plane backing. These
are framebuffer workflow checks, not visible-screen gameplay equivalence;
NPC initialization is intentionally suppressed by overhead mode in this asset
test. All25 headless and30 SDL-enabled suites pass. Sprites, framebuffer
presentation and original game entry/startup still require conversion.

## Original framebuffer rectangle staging and copy

Sol: Converted207F:245C..24D6 and retained24EB..2568/shared2629 exit
against complete raw ASM. Original staged source/destination FAR words become
explicit offset/segment EgaMemoryAddress values. NativeB78A/B78E also serve
RAM decoders: shared GraphicsWorkingAddress unions preserve that reuse instead
of independent scratch copies. Their host pointer alternative is host-sized;
these working unions are not serialized native structs. Decoders keep their
existing RAM-pointer access through named views; framebuffer methods use the
explicit EGA view. All layout-sensitive native address words remain16 bit.

Rectangle parameters are byte-column X and pixel-row Y plus WIDTH/HEIGHT,
not opposite corners. WORD sums wrap before clipping; clipping updates shared
sizes even when origin is subsequently rejected. Mode1 transfer inherits plane
enable and ignores bitmask/raster operation. Original scratch/controller effects
are retained, including mode1 at exit and unchanged mask/raster settings.
Width0 copies nothing; height0 retains native65536-row underflow. Forward
MOVSB order intentionally smears overlapping regions; no memmove correction.
Legal EGA aperture accesses remain the portable host contract: pathological
large copy counts that escape the aperture are not certified host behavior.

Tests verify whole-screen and viewport copies, clipping, rejected origins,
WORD-wrap scratch results, nonzero address offsets, width0/height0 control flow,
inherited plane/raster/mask states and one-byte forward overlap. The local map
workflow now copies each composedAC00 viewport to actualA000 screen memory
through245C and checks all resulting screen-plane bytes. All25 headless and31
SDL-enabled suites pass. Visible presentation/monitor colour interpretation,
sprites and original game startup remain incomplete.

## Original title-image workflow and staged-image wrapper

Sol: Converted0800:46A7..476C and1F3D:0086..00D4 retained EGA branch00B3
against complete raw ASM. Title setup applies the actual EXE-owned2FE8:0000
palette, selects disk2, loadsBTTITLE.CMP, clears compatibility, decompresses
using the original REVERSED work buffers, uploadsA800, copies the full40x200
rectangle through0086/245C, then setsTilesetIdFFFF. CGA-only00D1 is omitted
as a no-op on retained EGA; no new screen-present or startup algorithm is added.
0086 keeps its original unused file-buffer argument: EGA reads stagedA800,
not that host pointer. DefaultEgaPalette is original data, not a standard palette
substitution: its unusual first entries0,0,2,3,4,9,... are retained exactly.

The reusable buffers are now named GraphicsFileWorkspace/GraphicsSceneWorkspace,
not always-compressed/always-decoded arrays. Both working views cover8000h bytes
(246C:244B up toA44B;3092:4614 up toC614). This describes the shared native
address span, not allocator-size proof. Both can contain encoded or decoded
images according to their original caller; no invented new allocation routine.
The scene view previously exposed only32000 output bytes; it now also represents
the remaining768 workspace bytes before the known character-record boundary.

The isolated title unit verifies all six dependency calls and state-write order,
including buffer reversal and lateTilesetIdFFFF. Local integration calls the
actual original title workflow through actual palette, SDL file IO, original
decompression, upload and framebuffer copy; it checks packedFNV5E04241E,
all64000 screen colour indices and all sixteen palette-register values.
All26 headless and32 SDL-enabled suites pass. This verifies title screen-plane
construction, not visible monitor colours, playable startup or game completion.

## Original EGA string loop, font colours and glyph hardware drawing

Sol: Converted1F3D:00D5..01FA retained EGA string loop and207F:2127 colour
setter against complete raw ASM. Font BYTEs are actual EXE-owned246C:A661..AA60,
128 glyphs with eight bitmap rows.207F:2251..22A4 redirects its hardware body
to the separate SDL glyph primitive. Foreground/background WORD arguments
narrow to originalB772/B775 BYTE storage; mode2 ultimately consumes low nibbles.
Every bitmap row gets foreground mask then complemented background mask with
latch-primed masked writes. Inherited plane-enable/raster state is preserved;
the final bitmask is the complement of the last glyph row, not restoredFF/zero.

String coordinates retain column and text-cell-row units (320-byte row stride).
CR resets to the initial column and advances a row WITHOUT incrementing4FBE.
Only automatic signed-column wrap increments that original WORD line counter.
High-bit glyph bytes strip bit7; LF is drawn as glyph10, not a newline.
Empty strings still set colours but do not modify controller draw mode.
Explicit forward host pointer traversal covers valid strings whose original
FAR offsets do not wrap. Legal glyph indices/locations are the hardware contract;
no invented clipping, modern text layout or research pointer helpers are used.

Tests verify all128 glyphs against original bitmap bits in all four screen planes,
WORD-to-BYTE colours, final mask/controller state, CR vs automatic wrap, initial
column retention, high-bit glyphs, literal LF and empty strings. They additionally
verify inherited XOR and disabled-plane retention. All26 headless and33
SDL-enabled suites pass. Full dialogue wrapping/scrolling/menu paths and visible
font/monitor output still need integration before playable startup is complete.

### Dialogue controls, wrapping and panel scrolling — 2026-09-18

Converted original1E56:03F5..07CA and07CB..0A3A against the full raw ASM.
The former buffers words, wraps at the panel width and interprets embedded
background/foreground changes, absolute columns, padding, CR and high-bit
end markers. The latter draws each mutable fragment with the original font,
updates the shared cursor, scrolls A000 by one eight-pixel character row and
consumes the fragment's first byte on every exit. Native signed WORD tests,
BYTE-to-signed-WORD control operands and original call order are retained.

Storage uses named original globals for width3990, height393A, origin39A0/39A4,
background377E and existing cursor3748/374E and foreground37FE. All newly
declared geometry/background WORDs have confirmed EXE initial value0.
03F5 buffers occupy the native relative locations in one98-byte local frame;
host pointers replace explicit SS FAR arguments. Valid strings/panel widths
must fit the native buffers. Corrupted-stack local aliases and segment-offset
wrap are not certified by this representation, nor silently fixed with clipping.
The annotation-only Menu_Text_Read_Byte helper is not part of production code.

Preserved quirks: padding compares against the already drawn cursor rather
than buffered text width; a scrolled fragment updates the cursor using its
length BEFORE truncation/padding; advancing a scrolled row increments the
incoming row even if it is well beyond the panel height. The clear-strip right
endpoint remains (left+width)*8-8, not the usual final pixel.

The isolated dialogue test runs actual03F5 and207F strcat/strcpy with a07CB
test adapter. The SDL integration runs both real dialogue methods, original
font/glyph routines and framebuffer scrolling, checking screen-plane bytes,
cursor updates, padding, truncation and untouched adjacent sidebar memory.
Only unconverted1F3D:01FB clear-strip drawing is a test adapter in that test;
there is no production substitute. Full bottom-row font padding overwrites
the strip, but does not certify that pending rectangle method independently.
All27 headless and35 SDL/local-asset suites pass. This is not emulator capture
or complete playable-game validation. Next: original rectangle/line drawing,
panel setup and menu/startup integration.

### Original rectangle, span and clipped-line methods — 2026-09-18

Converted1F3D:01FB..0258,031C..03EA and03EB..049C against complete raw ASM,
plus207F:05D0/0780 staging and their retained EGA hardware bodies0637..067C
and07D8..080B. Historical Draw_Horizontal_EGA_Line fills an INCLUSIVE
rectangle, with a signed row test and no sorting/clipping.03EB splits each
row into leading pixels, aligned byte groups and inclusive tail pixels.
031C independently sorts/clamps both endpoints, draws only axis-aligned
lines and prioritizes the vertical branch for a single point. Wholly offscreen
endpoints may collapse to a visible border point; this is not corrected.

The EXE-owned EGA table elements remain mutable globals: alignment7 at
3EDB:4FC8, end mask504(01F8) at4FD0 and group shift3 at4FD8.01F8 is the
actual native value, notFFF8. Group arithmetic retains WORD wrapping and
SAR sign extension. Legal EGA table settings/coordinates are the contract;
deleted CGA/Tandy/VGA branches are not reintroduced. Shared primitive scratch
0220/0224/0230/0234/0236 keeps its original WORD values and EXE initial zeroes.

Hardware bodies redirect to SDL methods over the same four physical EGA planes.
Initial Y multiplication uses only its low BYTE; endpoint tests use full signed
WORDs. Vertical writes occur before their endpoint test, so direct reversed
bounds still write once. Aligned group count0 performs65536 writes and wraps
through the complete64KiB aperture, matching the native unguarded LOOP.
Mode2/bitmask state is left as native operations set it. Inherited enabled-plane
and replace/AND/OR/XOR raster settings affect latch-primed writes; no screen
clipping or controller-state reset is invented in the backend.

Removed the clear-strip test adapter from dialogue integration: both original
dialogue methods now execute actual rectangle/span/pixel drawing, font and
framebuffer methods with only hardware redirected. A separate drawing suite
checks9744 start/length combinations independently against bitmap masks,
rectangle inclusivity, reversed rows, BYTE initial-Y truncation, clipping,
diagonal rejection, border-point collapse, zero-count writes across all planes,
and inherited raster/plane-enable state. All27 headless and36 SDL/local-asset
suites pass. Visible monitor/palette output and playable startup remain pending;
next presentation work is original panel descriptors, setup and menu drawing.

### Panel activation and interior clearing — 2026-09-18

Converted original1E56:0281..0387 and0388..03F4 against complete raw ASM.
Menu_Memory_Variables saves outgoing foreground/background/cursor only after
the first activation, then loads the selected panel's eight fields. Selecting
the same panel still saves and reloads its live state. Outgoing geometry is NOT
saved, exactly as native. The first-call gate is the original3EDB:4FA2 WORD,
confirmed initial0, alongside the existing3092:4600 active layout index.

Actual EXE-owned305B:0000..008F supplies nine mutable16-byte records with
left/top/width/height/foreground/background/column/row WORD fields. Compile-time
size/offset checks preserve stride and mutable-context field positions.
This replaces FAR305B references with an ordinary typed global table, not new
UI layouts. The valid original layout IDs0..8 are the flat-host contract;
native out-of-table memory traversal is not implemented as invented rejection.
Annotation-only Menu_Layout_Read_Word/Write_Word helpers are not included.

Draw_Top_Graphic_Sidebar clears the whole active panel interior through actual
inclusive rectangle/span/pixel methods, then zeroes both cursor offsets. Cell
coordinates and extents convert to pixel corners using native WORD wrapping;
it does not draw border tiles. The historical sidebar name remains traceable
to the original entry, with its real behaviour described in source.

Headless tests cover first-call save suppression, switching, same-ID switching,
mutable state restoration across all nine records, and geometry not being saved.
SDL integration activates actual EXE panel7 and checks every visible byte in
all four planes against independently computed clearing bounds, including
untouched outside memory, before running the existing dialogue/font/scroll
sequence. All28 headless and37 SDL/local-asset suites pass. Original border
descriptors, tile drawing, menu selection and playable startup remain pending.

### Border descriptors and repeated edges — 2026-09-18

Converted1E56:0004..01E6 and01E7..0280 against complete raw ASM. Actual
EXE-owned305B:0320/0338/0350 twelve-WORD descriptors contain four corner
IDs followed by four WORD255-terminated edge patterns. The original3EDB:4FA4
FAR style table becomes five typed pointers, preserving the shared0350
descriptor for styles0/1/2 and the0338/0320 descriptors for styles3/4.
Descriptors remain mutable; changing the shared record affects all three styles.

Draw_Menu_Border draws corners in native order then chains top/left/right/bottom
edge sequence starts through DrawCall_Border's returned index. Repeated edges
use signed WORD lengths, wrap the sequence on its terminator, and scan forward
to the next terminator after drawing. Zero/negative lengths still scan and
return the next sequence start. Coordinate increments and tile-ID*32 offsets
retain WORD wrapping. BorderTileset represents the original3092:4066 FAR base
as a normal global pointer; valid allocated tilesets and nonempty terminated
descriptors are the host contract. Native malformed descriptor/segment wrapping
is not certified. Annotation-only descriptor/tile-address helpers are absent.

Headless tests check all five original styles, four-corner/edge call order,
coordinates, repeated multi-tile patterns, partial repeats, zero/negative
length sentinel scans, horizontal coordinate wrapping and shared descriptor
mutation. Only the pending207F:275C tile-drawing method is a test adapter;
there is no production substitute. All29 headless and38 SDL/local-asset suites
pass. Next: actual tile drawing and border asset allocation/capture integration.

### Actual border/menu tile drawing — 2026-09-18

Converted207F:275C retained284D..28A7 EGA body/shared2819 exit against raw
ASM. The original entry redirects hardware to SDLBackend_DrawEgaTile; its
source is32 RAM bytes interleaved as four plane bytes per eight-pixel row.
Column is a screen byte-column and row an eight-pixel cell row. Address
arithmetic wraps as a WORD, without invented clipping. Native B78C source
segment staging is DOS address bookkeeping absorbed by the flat pointer
parameter; the port does not fabricate a host-derived segment or overwrite
the unrelated framebuffer address union with a guessed native value.
Valid stable source tiles with intended DF-clear/non-wrapping traversal remain
the host contract. Deleted other-adapter bodies are not reintroduced.

The separate SDL hardware primitive sets mode0, bitmaskFF and disables
enable-set/reset. It selects each plane in original1/2/4/8 order and restores
all enabled planes on exit, overriding the incoming map mask exactly as native.
Each destination read primes its latch before the source BYTE write. Inherited
data rotation and replace/AND/OR/XOR operations are retained rather than reset;
the read-map selection is unchanged. Rotation/enable-set-reset have explicit
hardware storage, not game-rule globals.

The new integration runs actual border descriptors, corner/edge methods and
tile drawing with no drawing adapter. Synthetic tiles independently check
every visible byte across four planes for all five styles and untouched outside
pixels. Additional cases cover the final screen cell, inherited rotation,
XOR against the previous pixels, overridden incoming disabled-plane state,
enable-set/reset clearing and final mode/mask/plane-enable state. The isolated
headless border test retains its boundary adapter solely to inspect call order.
All29 headless and39 SDL/local-asset suites pass. No external assets are embedded.
Border asset loading/allocation/capture, menu selection and visible startup are
still pending; this checkpoint is not proof of complete playable preservation.

### Original tileset capture and local startup assets — 2026-09-18

Converted1E56:0A3B retained0ACD..0AE4 EGA branch and0AE5..0B5D against
raw ASM. TileSet_Memory_Operation deliberately ignores its display argument:
the EGA helper captures from previously uploadedA800 into plane-interleaved
32-byte RAM tiles. Create_TileSet_Array always calls original05BC allocation,
including count0, with WORD count*32 wrapping followed by CWD signed extension
to DWORD. It uses signed tile-count/column tests, advances after column39 to
the following tile row, and returns the initial allocation rather than the
advanced destination. Relative destination offsets retain WORD arithmetic;
valid original allocations not crossing their FAR-offset boundary are the
flat-host contract. No allocator failure recovery or malformed-count repair
has been invented.

05BC remains an unresolved original dependency, not a production stub. Its
positive-high-WORD oversize path returns unassigned native stack words; a C
uninitialized pointer would be undefined behaviour rather than an honest
model of that residue. This checkpoint does NOT certify that wrapper. Tests
adapt only its allocation boundary while exercising the actual capture loop,
hardware capture, border descriptors and tile drawing. Existing Astra/native
allocator investigation remains relevant before that error path is converted.

Synthetic checks independently reconstruct plane bytes from packed source
pixels for startup counts8/66 and a column39 row crossing, validate allocation
request values256/2112, initial allocation return, guard bytes, final capture
controller state, unconditional zero-count allocation, and negative signed
count's skipped capture with wrapped/CWD requestFFFFFFE0.
The opt-in local-asset suite additionally loads/decodes BTBORDER.CMP and
TINYLAND.CMP through real file/decode methods, uploads atA800, executes actual
capture methods and redraws all8/66 tiles atA000. Both RAM and screen planes
match independently reconstructed decoded pixels. Original files remain local
and are not copied or embedded. All29 headless and40 SDL/local-asset suites
pass. Full startup/menu integration and production allocation remain pending.

### Menu control records, selection loop and XOR highlight — 2026-09-18

Converted1E56:0B5E..0D1C and207F:2B87 retained EGA staging/body/shared exit
against raw ASM. Actual EXE-owned305B:0090..031F becomes41 mutable16-byte
MenuControl records, preserving all eight WORDs including unknown fields.
Existing equipment countD6, combat settingsC2/C6/C8 and personnel menu
202/206/208 names now alias their actual table fields, not independent globals.

Selection draws its initial marker BEFORE draining input, uses the real keyboard
converter, resets invalid/attractor selections and wraps signed selections.
Only ENTER/SPACE confirms; no Escape-to-cancel is added. Sol: subsequent EXE
byte review corrected the comparisons to nativeFFB8/FFB0: opcode83 sign-extends
the immediate BYTE, unlike the generated ASM printout. The unreachable normal
ESC tail remains represented. Marker pixels
remain on return. Unlike the annotation's cached locals, new C rereads native
highlight width/colour and option count during movement handling.

The original highlight stages sharedB792/B79A rectangle scratch and redirects
its eight-scanline hardware loop to SDL. Mode2/full mask/XOR is set explicitly;
enabled planes are inherited. Width0 performs eight65536-byte loops, not no-op.
The final data-rotate register write clears rotation/logical operation to replace.
Another annotation mismatch is corrected: native final OUT leavesAX0003 and
shared exit simply returns, so the retained EGA result is3, NOT BX colour.

Integration uses actual menu selection/converter/highlight methods and SDL
planes, with scripted keyboard/drain test boundaries only. Tests verify movement
wrap, ignored FF directions/ESC, confirmations, initial marker before drain,
disable/recording/invalid-selection resets, shared equipment field alias,
live width/colour/count mutation and its exact resulting XOR pixels, retained
marker/controller state and zero-width full-aperture cancellation. All29
headless and41 SDL/local-asset suites pass. Interactive native menu input
behaviour still needs emulator comparison; startup/allocation/gameplay remain
unfinished. No external copyrighted asset files are included in this checkpoint.

### Original packed-to-planar startup conversion — 2026-09-18

Converted207F:0572..05BE and inlined its internal05BF..05CF register bit
operations against complete raw ASM. Four packed nibble bytes produce four
interleaved plane bytes for eight pixels. Eight shifts replace every incoming
accumulator bit, so local zero-initialized plane accumulators have the same
complete-group result. All four source bytes are consumed before any output
store, preserving in-place MECHSHAP startup conversion and native forward order.
The annotation's fabricated EGA_Animation_Bitshift array-argument helper is NOT
included as a production method.

Count is native WORDs, divided by two for four-byte groups; odd final words are
discarded. Zero/one count executes65536 groups through unguarded LOOP. Relative
WORD source/destination offsets wrap through64KiB; such underflow cases require
full storage with compatible original FAR-boundary semantics. Valid32000-byte
startup buffers do not wrap. Arbitrary nonzero FAR base/offset wrap, hardware
destinations and straddling bus WORD writes are not certified by flat RAM arrays.
DF-clear/stable RAM source is the original intended contract, not a new CLD.

Tests independently reconstruct plane bytes from pixel colour nibbles, checking
256 varied groups, odd count and untouched tail, the full startup-sized image,
in-place equivalence, and zero/one-count underflow over full64KiB source/dest.
Opt-in local integration additionally performs actual in-place0572 conversion
for every one of the12 original artwork files, including MECHSHAP.CMP, and
checks every resulting plane byte against its independently decoded packed
pixels. All30 headless and42 SDL/local-asset suites pass. No local artwork is
copied into source. Original sprite row-copy/snapshot/table construction and
full startup integration are the next missing dependencies; playable-game
behaviour remains incomplete and is not certified by these conversion tests.

### Sprite snapshot allocation/header and strided WORD rows — 2026-09-18

Converted original1F3D:070A..080F retained EGA path and207F:0931..0970
against raw ASM. Capture allocates low-WORD width*height*four-planes plus
four header bytes, publishes the returned pointer even on null, then assigns
only header BYTE1(height-1) andBYTE2(width). Header BYTE0/BYTE3 are not zeroed
or otherwise invented. The native376-entry3092:39FA runtime FAR-pointer table
becomes ordinary host pointers, not on-disk/native-record pointers.

Source is original246C:244B GraphicsFileWorkspace AFTER0572 packed-to-planar
conversion. X/width are eight-pixel cells; Y/height are scanlines. Source offsets,
allocation sizes and row gaps retain WORD wrapping. Deleted other-adapter
second-buffer captures and annotation-only FAR offset helpers are absent.
0931 narrows words-per-row and rows to BYTE, copies WORDs forward with both
source bytes read before each destination write, and adds the source gap after
every row. Zero width copies none; zero height executes256 rows. Overlap
smearing is preserved instead of memcpy/memmove semantics. Native source-
segment bookkeeping is absorbed by the flat parameter. Compatible valid
original FAR boundaries, DF-clear RAM, valid sprite IDs and allocated storage
remain the host contracts; corrupted/overrunning geometry is not repaired.

Headless tests exercise geometry/size, untouched header bytes and guards,
pointer publication on allocation failure, sprite IDs0/375, row/count BYTE
truncation, zero-height iteration, zero width and forward overlapping WORDs.
Only pending05BC allocation is adapted, with no production substitute. Local
integration additionally loads/decodes MECHSHAP.CMP, runs actual in-place0572,
copies the native startup source workspace and captures the first24x24 sprite.
All288 payload plane bytes match independently reconstructed original packed
pixels, alongside native header/guard checks. All31 headless and43 SDL/local-
asset suites pass. Original376-sprite startup schedule, production allocator,
sprite playback/rendering and complete startup/gameplay remain unfinished.

### Original startup and complete sprite capture schedule — 2026-09-18

Converted original0D27:000A string output and0044 Setup_Game against the
authoritative expanded ASM. Retained the owner's EGA-only adapter restriction.
Original disk-choice retries, hard-disk/two-floppy flags and prompts, splash
700-retrace countdown with input cancellation, title loading, border/tinyland
captures and packed MECHSHAP conversion precede the gameplay handoff. EXE-owned
banner/prompt strings are readable source strings; external artwork remains
required and is never embedded. EGA-no-op207F:00D1 translations stay omitted.

The376 captures at040B..0833 remain inline in Setup_Game. The extracted0410
annotation helper was not a native FAR method and is not imported as a new
production API. Geometry uses the literal original atlas coordinates and native
capture order, including swapped source columns for IDs122/123 and the loop
which captures270..289 using counters266..285. TinylandTileset is the original
3092:4588 runtime pointer represented as a host pointer.

The new headless test executes actual startup and in-place0572 conversion,
adapting only dependent methods at test boundaries. It checks all three disk
choices, invalid-choice retry, both splash termination paths, filenames/workspaces,
tile counts/pointers, gameplay/shutdown order and every sprite ID/rectangle with
an independent ID-based oracle. It is not full startup integration: graphics
initialization0C8F, startup DOS output0213, Start_Game and production allocation
remain unfinished dependencies, not production stubs. Actual capture/copy and
real-asset conversion retain their separate integration tests. All32 headless
and44 SDL/local-asset suites pass; no playable-game claim follows from this.

### Title/attract controller — 2026-09-18

Converted native0800:50C8..527A Start_Game against full raw ASM and the .dis
backward-branch/FAR-return tail. The256-byte246C:09FB GameSeeds array remains
global original game data, filled from original PRNG calls. Retained EGA-only
translation no-op, graphics flag stores, animation loading and initial music.
Attract mode loads exactly1023 bytes of required external DEMOFILE into the
shared3092:00A0 workspace at offset2710 (native3092:27B0), sets native seeds
1325/0090, invalidates tileset and activates replay before map/game entry.
On return it restores title/music and tries again. Interactive input instead
drains keys, initializes the map, asks the original first-time/load questions
unless recording mode bypasses them, and exits after normal gameplay returns.
Executable-owned legal/dialogue strings remain readable, not pointer literals.

Test-only dependency adapters validate all five controller paths, seed-table
fill, replay buffer/count/seeds, loop reset, prompt count and load choice.
Real PRNG/input/asset methods have separate tests; adapters do not certify the
unfinished production animation/music/map initialization/save/main-loop chain.
All33 headless and45 SDL/local-asset suites pass. Start_Game itself is no longer
missing, but complete startup integration and playable gameplay remain pending.

### Animated map tiles and intro SIF loading — 2026-09-18

Converted native0800:320B..32B2 against full ASM, correcting the annotation's
missing disk2/disk1 selection and compatibility flag store. ANIMATE.ICN decodes
into3092:4614; pixels begin128 bytes later at4694. Original0572 converts0780
WORDs (3840 bytes) in place. The complete three frames for ten16x16 tiles are
copied to nativeD582 storage before loading BTTLTECH.ICN into the workspace.
Native0A76 REP MOVSW is inline here with forward WORD read-before-write order:
the preceding EGA0572 explicitly clears DF. No arbitrary-DF copy API/research
binding is invented. The retained EGA00D1 no-op is omitted.

Headless tests check header/tail guards, complete plane data, preservation after
base tiles overwrite the workspace, disk order and flag timing. Local original
asset integration executes the actual ANIMATE and base tileset loaders and
independently checks every one of3840 saved plane bytes against packed pixels.

Converted native0800:476D..48B6 retained PC-speaker/EGA music workflow against
full ASM. WWOODBT.SIF remains required external data, loaded headerless through
original DOS wrappers. Seek length narrows to AX; read/close statuses remain
unchecked; four zero terminators follow the payload. Native timer installation,
stop/start selector calls with stream/cadence4, input/finished polling and final
stop/timer restoration remain original interfaces. Dispatcher0306 must bind its
extra arguments when converted; annotation's lost arguments must not be copied.
Only valid original streams fitting the shared workspace have a host contract.

Tests validate missing-file retry, AX length truncation, unchanged bytes after
short read, ignored close error, exactly four terminators with tail guard,
variadic start arguments, both completion/interruption and short-circuit polling.
Timer/playback methods are test-only adapters until native204B conversion; no
production substitutes or audible-playback claim. All35 headless and47 SDL/local
asset suites pass. Full game initialization, main loop, production allocation,
sprite playback and real-time music remain unfinished.

### Native PC-speaker music and SDL timer binding — 2026-09-18

Converted retained native204B PC/idle path:0139 with inline00CF pitch conversion,
0179 stop,0298 consumer,0306 dispatcher,0020/003C interrupt effects,0048 install,
0091 restore and033C completion. Full ASM checked; prior033C .dis/profile tail
supplies its truncated return. Original writable CS state is ordinary globals
and runtime pointers, not a serialized record. Annotation-only explicit-parameter
0233 setup and clock-chain return adapters are NOT production APIs:0233 stores
are inline in the real0306 dispatcher, forwarding its variadic stream/cadence.

Preserved unsigned note table/division, BYTE octave-count wrap and unmasked8086
WORD shifts; low-BYTE cadence (zero means256 ticks); initial note due on first
IRQ; single-zero skip in same tick; double-zero stop without cursor commit;
high-bit notes writing divisor14; cursor commit before pitch output; stop without
frequency reset; gate enable before dispatch. Selectors>=13 retain no-op return.
Game uses only selectors0/1; other signed native selectors execute arbitrary CS
data and have no portable contract, explicitly asserted rather than presented
as thirteen safe entries. Host stream pointers absorb initial FAR offset; valid
non-straddling original stream bounds/DF-clear are required. Deleted Tandy path
is not restored in the EGA-only preservation build.

SDL owns gate/divisor operations independently, mutex-protected for audio-thread
reads. Music does not reset pitch/phase by accidentally calling combined speaker
start. Single-threaded timer servicing at input/retrace boundaries catches up
elapsed IRQs with1193182/4095 rational frequency; blocking keyboard waits service
it every8ms. Native callbacks precede legacy divider tests: first IRQ chains,
then every17 (not16). SDL records chain ticks instead of BIOS vectors/PIC/IRET;
normal host time is SDL's separate clock. Restore removes the binding, shutdown
also removes it. This is cooperative hardware replacement, not an8086 IRQ/PIT
waveform emulator. Long stalls/audio scheduling and audible binary comparisons
remain to validate; nanosecond accumulation assumes service within ordinary
interactive intervals rather than multihour suspension.

Headless tests cover all256 pitch inputs, original table oracle, note/gate/stop
quirks, cadence256-to-BYTE-zero, dispatcher binding,17-IRQ chain and high-divider
BYTE preservation. Local integration traverses actual external WWOODBT.SIF
through real native ticks and independently verifies cursor/termination without
exporting the asset. SDL dummy-device integration waits elapsed time, services
real native callbacks, checks stream completion/mute/chain and timer removal.
All36 headless and49 SDL/local-asset suites pass. Audible playback, complete
startup initialization, main loop and full playable-game validation remain open.

### Destructive native new-game initialization — 2026-09-18

Converted complete0800:4DC7..50C7 Load_Game_Map_Data against raw ASM. This is
the original destructive new-game initializer, NOT the save reconstruction path.
It sets absent mech/name markers without clearing unrelated record bytes;
clears Jason's seven skills and exactly100 persistent BLD-state bytes; clears33
one-use security codes; assigns Jason's original attributes/on-foot/cudgel/full
health; sets20 C-Bills, clears three stock balances and starts the name cycle.
Initial fog reveal ORs1F into six rows, leaving all other visibility untouched.

Native3092:D30C..D36F is now a100-byte union of byte-addressed script state and
named BYTE fields. Healing/injury, upgrade, betrayal and cache/countdown names
previously independent globals now alias their true positions. The odd-address
wealth allowance stays two actual BYTEs, avoiding unaligned host WORD access.
All existing gameplay tests continue to pass through the aliases. Layout/count
assertions protect the span and representative offsets. Remaining unnamed state
bytes retain original storage rather than being discarded on conversion.

Preserved RNG-call order and the even-result no-store mech sprite behaviour;
effect coordinates/page clear while sprite IDs survive; D557 BYTE clear while
adjacent D558 survives in the WORD slot view; both original sidebar redraw calls;
Citadel packed0C45:C019 position/pageCC and centre-slot map/cache/render calls;
both sides' mech/infantry frames and animation reset; story flags only AFTER
combatants draw. No invented reset helper or zeroing entire game state.

Headless tests prefill sentinel state and check all reset fields/unchanged bytes,
fog bitmap, BYTE/named alias bidirectionality, eight RNG calls, original world-
entry ordering and late story reset. Graphics clear, sidebar, map/render and
combatant drawing are test-only boundaries in this initializer test; production
map methods have their separate original-asset tests. All37 headless and50 SDL/
local-asset suites pass. Complete new-game integration, screen clear/shared latch
semantics, character drawing/sidebar and main loop remain unfinished; no playable
game certification is claimed.

### Screen clear, shared latches and party sidebar — 2026-09-18

Converted retained207F:1FBE..200D hardware body to an SDL redirect, checked
against raw ASM. Native1F40 STOSW clears16000 byte offsets, including bytes
beyond the visible8000-byte plane; it is not a whole64KiB clear. Mode2/maskFF
are assigned, map-mask/ROP/rotate remain inherited, and DF-clear is required.
There is NO aperture read, so OR/XOR with CPU zero propagates frozen incoming
EGA latches rather than the current destination bytes. Replace/AND write zero.

Backend now retains four hardware latch bytes across actual reads in tile/glyph/
line/highlight/capture/copy/image paths. ReadEgaPlaneByte remains a non-mutating
inspection accessor, not an emulated aperture read. Optimized0260 upload retains
the last read's plane bits BEFORE bit01 writes; this matters for subsequent
unprimed operations. Existing rendering results/asset comparisons still pass.
Clear tests seed the complete16000-byte range and beyond, check all four ROPs,
disabled planes, unchanged registers, and last-upload latches (FE, notFF).
The first test attempt seeded only the visible region; its failed assertion
identified an incomplete fixture, corrected by filling the second8000 bytes.

Converted original0800:4AA6..4D56 three-method sidebar workflow against raw ASM:
character Body/Dexterity/Charisma row, attribute bar and health/cash parent. Names,
attributes and health retain BYTE sign extension; IDIV10 truncates toward zero;
zero display-health units become1. Inclusive red damage overlay and unbounded
signed attribute display values remain original, not repaired. Twelve-unit bar
scale and original7x15 geometry are explained/named separately from game rules.
Scan eight party records, skip FF names, show at most four at two-row intervals;
menu/border3 and final menu/border4 always happen, only top-panel clearing is gated.

Headless tests execute actual sidebar routines with drawing boundaries, checking
row selection/cap, panel/cash order, header, three bars per character, zero-health
inclusive overlay and signed/full attribute geometry. Actual primitives/font/
panels have separate SDL tests, but the combined visible sidebar is not yet
certified. All38 headless and52 SDL/local-asset suites pass. Full exploration,
character/sprite rendering, production allocation and playable startup remain
unfinished.

### Persistent map-effect compositor — 2026-09-18

Converted native0800:2A93..2C4F retained EGA path against full ASM and .dis.
The latter confirms the signed-immediate FF8D (-115) projection guard which
the listing misleadingly prints8D. The64 saved ring entries reconstruct X from
page low nibble and Y from page high nibble, subtract the WORD camera anchor,
apply26/12-cell viewport origins, reject same-page offscreen/false-wrap values,
then admit bounded neighbouring-page encodings and mask local coordinates7F.
Draw uses actual runtime sprite table and AC00 destination. Only drawn fire
IDs7C/7D toggle by XOR1; other/offscreen effect state survives untouched.

No terrain-clipped annotation helper becomes a production API. Native0377
renderer is declared with flat sprite pointer and explicit aperture address,
but remains unimplemented until its complete hardware body/BUG-019 cleanup
contract is resolved. Test-only draw boundary checks destination, source pointer,
positions, first/second fire frames, visible nonfire, both same-page exclusions
and a negative neighbouring-page projection. Original global storage was moved
from initializer/capture objects into existing BTECH_DATA.c so referencing the
original tables does not accidentally drag unrelated methods into link tests;
no storage layout/value change or new gameplay helper was introduced.

All39 headless and53 SDL/local-asset suites pass. Original exploration compositor
051B (including signed mech-assignment mismatches),0377 transparent rendering
and complete gameplay remain unfinished. The planned friendly-position work is
part of051B, not an independently identified native method to extract into a new
production API; it will remain in that method when converted.

### Transparent planar sprite renderer — 2026-09-18

Converted complete retained207F:0377..0571 hardware routine to original entry
redirect plus separate SDL implementation, checked against every raw ASM path.
All nine original call sites in0800/1631 explicitly load DX=AC00 immediately
before the call. Thus BUG-019 has a known preservation contract, not an invented
incoming-register helper: early cleanup writes0F02 to AC00, or the negative-Y
MUL's high result after source skipping. These are not sequencer3C4, so early
returns MUST leave map-mask unchanged. Successful drawing writes3C4 and restores
all four planes. LastSpriteCleanupPort records that hardware effect; arbitrary
new callers with unrelated incoming CPU registers are not a native certificate.

Preserved BYTE height-plus-one wrap, signed SAR-floor X byte-column, top/left
source skips and WORD arithmetic, bottom/right width/height clipping, unguarded
native zero width/height loops, original row strides, colour0 transparency via
OR-of-four-planes occupancy, high/low shifted plane bytes, and no spill from
rightmost byte-column39. Sprite header is unchanged. Stable nonoverlapping RAM,
non-straddling FAR source boundaries and inherited DF-clear remain contracts;
malformed header/overrunning storage isn't sanitized. Scratch geometry is local
to SDL hardware operation, not new gameplay tables or imported research helpers.

Mode0 read-modify-write performs AND then OR as two distinct aperture reads/
writes, loading all hardware latches and inheriting rotate, logical operation
and set/reset enable/value. It doesn't replace this with ordinary alpha pixels
that would silently reset hardware state. Successful exit leaves read-map3,
write-mode0/maskFF and sequencer mask15, matching original effects.

Tests compare every screen pixel for all eight X shifts, aligned negative-X,
negative-Y, right/bottom clipping, transparency and source-header stability;
test BUG-019 early paths before/after MUL; independently simulate both CPU RMW
writes for all four logical operations and eight rotations. A reversed preserve-
mask-byte fixture was caught by this last check and corrected. Local integration
loads/decodes actual MECHSHAP.CMP, converts/captures first24x24 Locust and renders
it into AC00; every visible pixel matches independent packed artwork. No artwork
is embedded/exported. All39 headless and54 SDL/local-asset suites pass. Visible
emulator comparison, exploration/combat compositors, allocation and playable
startup/gameplay remain unfinished.

### Complete combat effects parent — 2026-09-18

Converted original `1AE8:12C7–1E45`, including attack-stream selection, weapon
sound dispatch, beam/missile drawing, impact playback, casualty removal and
arena respawning. Corrected weapon-table versus component-ID comparisons in
the annotated template and retained original casualty text. Added the native
signed-WORD absolute-value routine, including its minimum-value overflow.

The maintained pipeline is startup-selected EGA adapter 2; removed adapter 1's
unassigned redraw phase is not implemented or certified. Tests use actual
original parent/interpreter/state routines with test-only presentation/audio
boundaries. All 109 headless and 127 SDL/local-asset tests pass. Details and
limitations: `docs/phase4/BTECH_1AE8_EFFECTS_C_CONVERSION.md` in the repository.

Fresh executable-link check still exposes five missing original parents:
`Allocate_Far_Buffer_05BC`, `Combat_Mechanics`, `Salvage_Armour_Dialog`,
`Examine_Screen_BTSTATS_CMP`, and `Combat_Weapon_UI_0004`. No stubs added.
Complete combat, emulator presentation/timing parity and playable startup
remain unfinished.

### Startup buffer allocation and SDL heap — 2026-09-18

Converted `1F3D:05BC`'s normal/null allocation paths and redirected original
runtime `207F:3835` into separate `src/SDL/memory.c`. Low-WORD sizes, signed-negative
high WORDs, non-clearing allocation and original null diagnostic/input/return
are retained. Original sprite/tile callers pass zero or CWD-negative high WORDs.

Positive high-WORD requests still have an unresolved native stack return. The
port warns/waits then explicitly fails, rather than inventing a pointer. This
temporary unsupported-path guard is not native preservation; A-009 remains
open. See repository `docs/phase4/BTECH_1F3D_ALLOCATION_C_CONTRACT.md`.

All110 headless and129 SDL/local-asset tests pass. Real-executable linkage now
exposes four missing methods: `Combat_Mechanics`, `Salvage_Armour_Dialog`,
`Examine_Screen_BTSTATS_CMP`, `Combat_Weapon_UI_0004`. Complete gameplay and
residual-stack paths are still unfinished.

### Complete combat execution parent — 2026-09-18

Converted original `1AE8:000C–12C6` and mapped damage transfer `1631:1122`.
Movement, ammunition, proficiency, hit/damage/armour/critical/ejection handling,
effects handoff and heat updates now run through the original parent. Added
native cursor/saved-tile storage and named range/stride/heat constants.

The unassigned entry attack-result flag remains unsupported when visible effects
would read it before its first assignment; a loud temporary guard prevents a
guessed default. Fatal damage-transfer AX is unobservable under the sole original
caller's destruction/ejection gate, not recovered as zero. Invalid inputs and
corrupt storage still require separate contracts. Repository details:
`docs/phase4/BTECH_1AE8_EXECUTION_C_CONVERSION.md`.

111 headless and130 SDL/local-asset tests pass. Execution tests use actual parents
and damage subordinates with controlled random input and isolated cache/presentation
boundaries. Full-map NPC combat and emulator visual/timing parity remain unverified.
Fresh executable linkage exposes three remaining parents: `Salvage_Armour_Dialog`,
`Examine_Screen_BTSTATS_CMP`, and `Combat_Weapon_UI_0004`. No production stubs added.

### Complete component salvage parent — 2026-09-18

Converted original `0DAB:0002–04F8` with native armour/structure repair priority,
signed deficits, skill gates, heat-sink pool underflow, matching component use and
party-slot scrap payout bug. Recovered the three EXE-owned description strings.
Unknown SRM-6 stack-bucket accesses stop explicitly rather than receiving an
invented zero. This path remains unsupported and prevents full certification.
Repository notes: `docs/phase4/BTECH_0DAB_COMPONENT_SALVAGE_C_CONVERSION.md`.

All112 headless and131 SDL/local-asset tests pass. Parent tests run actual salvage
and supporting-structure logic with isolated UI/random inputs. Fresh executable
linkage exposes only `Examine_Screen_BTSTATS_CMP` and `Combat_Weapon_UI_0004` as
missing parents. Playability, emulator parity and native residual-state paths
still require work. No production stubs or external assets added.

### Complete weapon selection parent — 2026-09-18

Converted original `1543:0004–07CA` including collection, warnings, ammo/target/
range display, popups, overrides, original picker calls and panel restoration.
Corrected native text controls and infantry weapon CBW in the annotated template.
Preserved movement-field ammo aliases, exact heat shutdown, sensor2 early return,
target high-bit/active handling and informational range. Representable biased
personnel target names use the original save-byte view, not negative host arrays.

Full twelve-entry strlen, negative choice stack indexing and unrepresented signed
target memory remain unsupported diagnostic paths, not guessed native behaviour.
Repository detail: `docs/phase4/BTECH_1543_WEAPON_PARENT_C_CONVERSION.md`.

113 headless and132 SDL/local-asset tests pass. Parent tests run real picker,
range, popup, layout and formatter with isolated presentation/user inputs. Fresh
executable linkage now exposes only `Examine_Screen_BTSTATS_CMP` as missing.
Native residual-state paths and full playable/emulator parity remain unfinished.

### Statistics gauges and original tables — 2026-09-18

Converted 0DAB:174C..1857 with signed height tests, signed heat division and
the original one-unit green-segment omission (BUG-006). Added native flashing
state, all three gauge tables and both BYTE/WORD palette families from the EXE.
The actual gauge passes 264737 call-boundary scenarios; all 126 contiguous
table bytes also match the local unpacked EXE independently.

115 headless and135 SDL/local-asset tests pass. This is not full-screen or
gameplay validation: the statistics parent and its native residual palette
locals remain unfinished. Details: `docs/phase4/BTECH_0DAB_STATS_GAUGE_C_CONVERSION.md`.

### Statistics parent and complete executable linkage — 2026-09-18

Converted the original Mech-statistics parent with original storage, text,
equipment/actuator display and signed record/name/heat rules. 224 actual-parent
probes validate the first frame only. The parent remains explicitly unsupported
at the native residual palette-stack read, with a diagnostic guard rather than
guessed initial values or host undefined behaviour.

The real executable links with no missing original methods. It is not yet
certified playable: native residual-state guards, full-screen workflows and
end-to-end platform/gameplay validation remain required. Repository detail:
`docs/phase4/BTECH_0DAB_STATS_PARENT_C_CONVERSION.md`.

116 headless and136 SDL/local-asset tests pass, with successful executable linkage.

### Real startup and normal SDL window close — 2026-09-18

Reproduced a real startup hang: SDL quit became Escape at the original drive
prompt and could not leave its loop. A main-thread lifetime boundary in SDL
now returns control to the launcher on OS close; original menus remain unchanged
and gameplay Escape remains a key. The real-asset integration probe selects hard
disk, loads startup artwork, captures all376 sprites, reaches intro-music setup
and closes at that verified milestone. No production original method is mocked.

The actual executable now builds with ordinary SDL targets.116 headless and138
SDL/local-asset tests pass. Dummy drivers/accelerated retraces do not certify
visible rendering, audible music, original timing or a gameplay playthrough.
See [application lifetime](APPLICATION_LIFETIME.md).

### Shared procedural seeds and actual new-game entry — 2026-09-18

Sol: Corrected a production storage mismatch: startup's 256 seed writes and
procedural subdivision reads must share native 246C:09FB. The former independent
GameSeeds array is now an alias of MapConstructionSeeds; compile-time storage
offset checks and a failing-before/fixed-after title-controller regression verify
the alias. ASM evidence:0800:50C8 writes ES:[BX+09FB] using selector5400;
207F:0D07 reads DS:[09FB+seed index], DS=246C. No random calls or algorithm changed.

The real-asset startup probe now continues through original Citadel map loading,
party/C-bill initialization, first-time confirmation and an actual opening-door
animation frame before normal window close. Initial zero-duration retraces let
the whole animation finish between callbacks; bounded test-only host pacing makes
that milestone observable. No original prompt or animation logic needed changing.

116 headless and139 SDL/local-asset tests pass. This is entry-workflow evidence,
not exploration, NPC/combat, visible/audible fidelity or an interactive playthrough.

### Connected real-asset exploration — 2026-09-18

Sol: Extended the original startup/SDL integration fixture through the complete
opening animation and view restoration into Main_Game_Loop. Actual SDL south
input moves Jason one tile, the native first allowance pays 15 C-bills, and
world updates materialize a delayed roaming NPC. Original movement, interaction,
map-cache, finance, animation, NPC and drawing callees execute without gameplay
test doubles. The fixture only reads game state and queues SDL key/quit events;
its host retrace setting is not a native timing measurement.

116 headless and140 SDL/local-asset tests pass. This is connected exploration
evidence, not a full playthrough, NPC path/collision proof, combat sequence or
visible/audio/emulator parity. No original gameplay change was needed for this
probe; residual native-state guards remain unresolved.

### EGA scanout connected to the SDL window — 2026-09-18

Sol: Found and corrected missing production presentation: EGA draws changed
the four planes but only the initial black image reached the SDL texture.
Hardware-layer scanout now decodes the live screen and six-bit attribute palette
into endian-independent RGBA bytes and presents at input/retrace boundaries.
Completed blocking menus are presented immediately; hot polling is throttled
without rewriting original game drawing or timing logic.

The scanout regression covers all16 plane indices x64 palette codes and the
screen's last pixel, with a real dummy-driver SDL presentation. All141 SDL/local
tests pass, including real-asset exploration. Its input fixture now stops Y
injection at animation entry, preserving native pending-key drains rather than
changing them to hide the test's timing mistake. See [EGA scanout](EGA_SCANOUT.md).
Visual/emulator parity and native residual-state paths remain incomplete.

### Connected headless personnel combat — 2026-09-18

Sol: Added a complete-library encounter regression without game-method doubles:
native procedural cache/view setup, both sides' AI/targeting/path plans, twelve
execution slices, native RNG/dice/damage and casualty cleanup run together.
The synthetic pistol encounter resolves in two rounds, with executed movement,
enemy return fire, a surviving Jason and enemy death/target-link cleanup.

All142 SDL/local-asset tests pass. Dummy SDL and the original no-combat-graphics
setting allow headless execution; no production game code changed. This is
personnel integration evidence, not Mech combat, parent menus, loaded missions,
original-binary trace parity or resolution of unsupported native-state guards.
See [connected combat](CONNECTED_COMBAT.md).

### Connected Mech ammo, damage and shutdown — 2026-09-18

Sol: Added intact EXE-owned Wasp encounters through original AI and complete
execution without method doubles. Both missile pools spend exactly one round,
both armour totals fall and heat20 becomes15 after five firing heat and ten
intact sinks. The threshold30 comparison issues no firing targets, spends no
ammo, leaves armour intact and cools to20. Corrected a fixture sink-count
assumption against the actual template; no production gameplay changed.

All144 SDL/local-asset tests pass. Synthetic fixtures/original graphics-disabled
setting do not establish fatal critical/ejection/salvage paths, mission playthrough,
visible effects or binary trace parity. Native residual-state guards remain.

### Connected destruction, wreck and crew state — 2026-09-18

Sol: Severe-damage Mech fixtures now execute original AI/damage transfers,
fatal-section dispatch, ejection and wreck registration without gameplay doubles.
Enemy crew remains assigned/not materialized; friendly Jason becomes active on
foot. Retired Mech coordinates are cleared after ejection by the effects parent;
native1AE8:1E03 confirmed why an initial fixture expectation was wrong.

Removed a stale attack-count reconstruction comment in both C/pseudo-code:
BP-34's Inferno clear has no further reads in the Mech branch; its actual damage
loop tests BP-7C at0CCA. No original logic changed. All146 SDL/local-asset tests
pass; full critical/salvage, visible/binary parity and residual-state work remain.

Sol: Connected combat now hands an actual destroyed enemy Wasp to the native
component-salvage dialogue. A real SDL key acknowledges the prompt; Average
Tech restores a damaged friendly sink, retains the enemy wreck flag and leaves
C-bills unchanged through the preserved slot-zero technician payout gate.
No production gameplay logic or external assets were added. All147 SDL/local
tests pass. Whole-Mech recovery, other payouts and native residual-stack guards
remain outside this checkpoint's proof scope; see CONNECTED_COMBAT.md.

Sol: Connected actual combat now feeds whole-Mech recovery and rejection.
Excellent Tech recovers the catastrophic Wasp into the first empty Lance slot,
binds Rex and applies the native minimum engine/gyro/structure damage state.
Good Tech rejects it but still consumes its wreck flag. Real SDL key events
drive the original dialogs; no gameplay doubles or external assets are added.
All149 SDL/local tests pass. Native statistics residual-stack reads were
reconfirmed, not guessed away; their guard remains a playability blocker.

Sol: Remaining-path source audit found an unguarded host uninitialized read in
Load_Game when the one-byte marker read fails. Native0800:334F supplies SS:BP-2
and the subsequent BYTE comparison uses the residual stack byte if DOS writes
nothing. The C17 port now diagnoses that unresolved path rather than executing
host undefined behaviour or guessing an invalid marker. This diagnostic is
not original behaviour and does not resolve malformed-save preservation.
Valid/invalid markers that are actually read retain their existing branches,
unchecked later reads and invalid-marker handle leak. All116 headless tests
pass; PLAYABLE_BUILD.md now lists the current unsupported-path categories.

Sol: Asset-backed connected Wasp combat now executes real startup,376 sprite
captures, BTTLTECH.ICN loading, compositor visibility and graphics-enabled
combat/effects before SDL screen presentation. No gameplay/graphics doubles.
Initial heat expectation15/15 was invalid for this path: actual rendering
supplies terrain cooling and animation decisions consume native RNG. The
fixture now checks heat against surviving sinks, engine hits and terrain.
All150 SDL/local tests pass. Visible/audio fidelity, all effects and native
residual-stack branches remain unproven; no external assets were exported.

Sol: Real asset-backed medical and ammo services now connect native dialogue,
SDL keys, RNG/healing, Mech selection, decimal entry and C-bill updates.
Paid healing, insufficient-funds refund and direct-call recovery gating pass;
ammo purchase, below-one-round affordability and capacity capping pass. Added
named SRM-2 component1Eh to the C header, matching Btech.h. No game algorithm
changed. All152 SDL/local tests pass; CONNECTED_SERVICES.md limits the claims.

Sol: Preparing real hospital BLD integration exposed a production menu error.
EXE1E56:0C40/0C46 contain83 7E F8 B8/B0, sign-extending the immediates to
FFB8/FFB0; .dis agrees but printed ASM misleadingly shows positive tokens.
Corrected both code versions, strengthened the existing real-converter menu
test and amended false native-mismatch notes. All152 SDL/local tests pass.
The hospital scene opcode remains untouched; its connected workflow is still
pending. See BTECH_1E56_MENU_SIGN_EXTENSION_CORRECTION.md in docs/phase4.

Sol: Actual hospital BLD now connects indexed loading/decoding, interpreter,
treatment timer reset, facilities charge, party healing, South-key menu exit
and original parent cleanup. The selected branch has professional help denied
and neither kit owned. Its root has seven choices; an initial five-choice
test operator was corrected, without changing script/opcode behaviour. FF Exit
returns normally. No cached-BLD exception suppresses ConditionalScene. All153
SDL/local tests pass; other hospital/scene branches and native-stack guards
still require work. HospitalFacilitiesFee now names the25 charge separately
from its refund alias, consistent with the annotated header.

Sol: Reconciled the annotated keyboard converter's punctuation aliases with
the private expanded EXE and the already-correct preservation implementation:
brace007B passes through, vertical bar007C moves west, tilde007E moves north.
Removed the queued mismatch comment. Both input transcription checks now
reject any discrepancy rather than accepting the three old source errors;
the systematic check compares all65536 WORD inputs and confirms all30 table
entries against EXE bytes. This changes annotated pseudo-code only, not the
compiled game's behaviour. Existing input and SDL menu regressions pass.

Sol: Added connected animation coverage to the existing real-game fixture.
All22 local shipped ANMs execute through the original scene parent, disk/file
calls, frame decoding, EGA transfer and SDL retraces, including native siren
repetition. O0 game-view restoration and the actual BLD ConditionalScene
handoff/disabled-input gate complete. Control bytes are synthetic test input;
external assets remain local and untracked. All154 SDL/local tests pass.
See CONNECTED_ANIMATION_SCENES.md for verification scope and remaining limits.

Sol: Connected the actual new-game/Citadel MAP1.MTP setup to StartTraining,
Locust startup animation/march, native southeast-corner mission movement and
objective handling, late return failure, temporary tile restoration and
LeaveTraining retirement/on-foot return. The test operator uses actual SDL
keys only; its initial south-wall return route was corrected to use the
hangar's east approach without changing collision code. Counted non-movement
inputs guarantee the late-failure case independently of host timing. All155
SDL/local tests pass. See CONNECTED_TRAINING.md for scope and remaining gaps.

Sol: Added connected exploration-loop coverage using the actual0800:0000
parent and original assets: five idle countdown updates, on-foot pause/Return,
subsequent exploration input, three-row fog reveal and SDL application close.
No game-method doubles or production initializers were added. All116 headless
and156 SDL/local tests pass; the new scenario also passed ten repeat runs.
See ../evidence/research/phase4/CONNECTED_EXPLORATION_LOOP.md for explicit scope.

One original-EXE combat-scan statistics entry now supplies a positive residual
stack witness and first-refresh trace. Static arithmetic identifies a candidate
saved-SI source for the frame WORD, but interrupts and the deeper phase WORD
still require provenance. This does not resolve the C statistics guard or
certify the preservation game complete. See the native witness notes under
docs/phase4/NATIVE_STATS_ENTRY_WITNESS.md.

Sol: Connected the real Citadel repair action and Mechlub service to SDL
menus/formatting/rendering for exact-funds, unaffordable and partial armour/
structure repairs, heat-sink then actuator purchases, and rejected engine
repair. Whole125-byte Mech records and C-bill balances match expected native
outcomes; isolated service probes now also check numeric armour/structure
quotes. All116 headless and157 SDL/local tests pass, with ten connected repeat
runs. See ../evidence/research/phase4/CONNECTED_REPAIR_SERVICES.md for scope. This is
additional behavioural validation, not resolution of the residual-stack
guards, complete BLD-service traversal or original-EXE presentation parity.

Sol:2026-09-18 practical-preservation checkpoint: the user clarified that
incidental BIOS replication is excessive. Statistics palette counter/phase now
start deterministically at0, and the first-use effects-only attack flag starts
FALSE. Original subsequent palette operations and assigned-result carry remain;
damage, ammunition and salvage rules were not defaulted. Existing parent probes
cover automatic and interactive statistics return and the first graphics-enabled
skipped attack, while the connected SDL/original-CMP scenario verifies statistics
loading, rendering and automatic dismissal without changing the Mech or C-bills.
All117 headless and159 SDL/local-asset tests pass and the game links. Current
runtime notes and launch instructions now distinguish these resolved presentation
stops from remaining gameplay/exceptional-read guards and manual validation work.
