# BTECH_0DAB systematic C-to-ASM audit

Sol: Checked 2026-09-17. Initial baseline `343a18f`; final baseline `3db44f5`.
**All14 definitions checked** — four locally matched, nine with mismatches,
one with unresolved runtime binding. File checking is complete, not functional
correction or gameplay validation. Historical review notes are not certificates.

Sol: Preservation update: [0002–0258 material salvage cleanup](../preservation/0DAB_SALVAGE_MATERIALS.md)
incorporates signed Tech/name loads, FAR-record bindings and signed armour/structure
transfer caps in that prefix. The complete method remains mismatched/unfinished;
later critical pointers and the original uninitialised SRM-6 native-stack BYTE
need separate work. Compiled prefix tests are not a whole-method certificate.

Sol: Follow-up [remaining salvage cleanup](../preservation/0DAB_SALVAGE_REMAINDER.md)
corrects the critical FAR pointers, names/counts, signed payout addition and
whole-Mech signed BYTE/text/trim discrepancies in place. Heat-sink/payout sections
and the complete whole-Mech method compile in isolated tests. The inherited
SRM-6 frame byte and shared scratch-memory binding still prevent an unconditional
preservation-C certificate. Historical discrepancy lists below describe the
pre-cleanup baseline; current audit scopes name the corrections and limitations.

Matching private listing: `BTech-Reko-expanded/BTECH.reko/BTECH_0DAB.asm`.
SHA256: `3B161FE51B4EA962FEE61FB373E52331906AAE52F407FAB401208FCDCDDACA70`.
Supplementary `BTECH_0DAB.dis` SHA256:
`D6CADEFC960AC5DE6DDEB19A02A4A458071E0236A2AE741E05A15ADFEC626D49`.
The intermediate listing resolves signed immediates and the truncated final
BTSTATS epilogue; it is not independent runtime evidence.

## Whole-body comparisons

| Method | Fresh result | Complete comparison scope |
| --- | --- | --- |
| `Salvage_Armour_Dialog` | Checked - mismatch | 0DAB:0002..04F8; entire technician/prompt, wreck armour pooling and reverse repair, structure,35 critical-slot heat-sink and weapon paths,16-byte bucket init, payout and return; BYTE CBW/signed deficit and FAR-temporary discrepancies |
| `Return_Bool_Allow_Computer_Control_Dialog` | Fully matched (native WORD/FAR contract) | 0DAB:0B5E..0B94; menu3, top draw, exact DS1115 prompt, WORD computer-control default and AX yes/no return; stack probe abstracted |
| `Calibrate_CPU_And_Video_Timing_0B95` | Fully matched (platform binding documented) | 0DAB:0B95..0C8E; BIOS DWORD snapshots with CLI/STI, initial change wait, unsigned four-tick target, WORD spin count, signed32 product/division,10000 retrace samples and signed majority; no cycle/timing certification |
| `Initialize_Graphics_Runtime_0C8F` | Fully matched (platform binding documented) | 0DAB:0C8F..0D11; FAR file buffer, adapter/video table/mode, black clear, calibration, low-WORD subtraction then signed divide6/min1, graphics config and FAR handler install; callee implementations not confirmed |
| `Restore_BIOS_Text_Mode_0D12` | Fully matched (platform replacement documented) | 0DAB:0D12..0D25; pass WORD2 to BIOS mode helper and FAR return; port replaces operation rather than replicating DOS/BIOS |
| `DOS_Critical_Error_Handler_0D26` | Checked - unresolved binding | 0DAB:0D26..0D3C raw bytes; prologue/stack probe, single zero WORD to207F:3CD8, stack cleanup and FAR return accounted; exact nonlocal/runtime return contract unbound; replace DOS critical error handling in port |
| `Draw_Vertical_Mech_Status_Gauge_174C` | Checked - mismatch | 0DAB:174C..1857; red then green rectangles, cached top edge, heat-only division/blink/reset, colour and bottom updates; EGA path accounted, signed heights/IDIV differ from unsigned C |
| `Draw_Component_Status_Pips_1858` | Checked - mismatch | 0DAB:1858..18E7; EGA green/red transitions, pip5 column/row wrap, WORD coordinate shifts and rectangle calls; native signed total loop differs from unsigned C |

| `Salvage_Mechs_Dialog` | Checked - mismatch | 0DAB:04F9..094A; entire technician search, qualified pilot scan, two-RNG candidate/16-flag wrap and consumption, damage acceptance/rejection, messages/trim,124-byte copy, name/pilot/sprite/minimum damage install, success/exhaustion/pause and return; signed BYTE/FAR/text/undeclared trim-view discrepancies |
| `Loot_Enemy_Soldiers_Dialog` | Checked - mismatch | 0DAB:094B..0B5D; eight biased flag slots, rawC6EB eligibility read, cash RNG/minimum/colour/CWD add, message/input exits, eight weapon transfers via Rex or Jason prompt and return; near raw read and weapon CBW discrepancies |
| `Generate_Random_Encounter_Enemies` | Checked - mismatch | 0DAB:0D3D..1466; map anchoring, four random origin rolls,12 enemy resets, eight infantry RNG/records, four conditional mech template copies, complete infantry/mech scan/projection/discard/spawn/spacing and return; near infantry pointers, signed threshold and low-WORD-before-SAR discrepancies |
| `Combat_Browse_Scan_Targets_1467` | Checked - mismatch | 0DAB:1467..174B; three-WORD interface, live/FFFF filters, friendly/enemy/compass descriptions, focus, menu/details, same-target decrement,12-ID wrap, Arena clamp, dead redraw flag, menu/colour exit; .dis provesFFFF vsC00FF plus signed indices/gates and missing CR/control/menu text |
| `Display_Friendly_Combatant_Description_18E8` | Checked - mismatch | 0DAB:18E8..1AFD; white colour, pilot/name/mech branch, infantry name/detail gate, traitor warning, health/cursor/weapon/armour states and all returns; CBW/IMUL/signed gate and literal renderer-control discrepancies |
| `Examine_Screen_BTSTATS_CMP` | Checked - mismatch | 0DAB:1AFE through .dis final FAR-return tail; complete asset cache/disk/decompress/EGA draw, chassis/crew, component pips and weapon/location rows, four actuator cases, eleven gauges/heat clamp, palette counters/patches, input/600 redraw timeout/palette restore/key; near casts, name/gate signedness and undefined C palette locals |

All listed meaningful local reads/writes, calls, branch conditions and loops
were compared through their returns. Stack probes, prologues, register saves
and FAR-return scaffolding are documented abstractions. Callees are contracts,
not newly certified implementations. Native WORD/DWORD wrap and FAR16 address
views are prerequisites; no host-C compilation or gameplay validation is claimed.

## Salvage_Armour_Dialog: full local flow, discrepancies remain

| Native block | Accounted-for behaviour |
| --- | --- |
| 0002–004F | Eight records, live Name BYTE check, strictly best technician, earlier tie retained |
| 0050–00CA | FAR character-name lookup, biased Tech-skill phrase lookup, exact DS strings and wait |
| 00CB–0114 | Four enemy combatants12–15, wreck WORD flags,11 BYTE armour locations and wreck count |
| 0115–018C | Descending armour locations10..0, friendly slots0..3, max/current deficit, capped transfer |
| 018D–0258 | Excellent Tech gate, eight structure locations, descending7..0 repair order |
| 0259–031E | Average Tech gate,35 slots+33..55, intact heat-sink count, destroyed-sink repair with273D gate |
| 031F–0429 | Good Tech gate,16 initialized weapon buckets, inclusive10..20 collection, destroyed-slot repair and bucket decrement |
| 042A–04F2 | Wreck/member gates, one RNG per wreck, mask7F plus90, member-ID multiplier, text/plural, signed payout addition |
| 04F3–04F8 | Local cleanup and FAR return |

### Mismatches, not approved fixes

1. Native0031 uses CBW on the Tech BYTE and signed best-skill comparison.
   Current unsigned C can select raw80–FF, which native treats as negative.
   The subsequent name BYTE also undergoes CBW before FAR-table indexing.
   Valid Name IDs and skills0..4 agree; invalid-state reachability is unknown.
2. At0155 and0221 native deficit WORDs are capped with signed JLE, not unsigned
   comparison. For maximum5, current10 and pool3, native deficit-5 is retained:
   current becomes5 and pool8. C unsigned deficit65531 is capped to3: current
   becomes13 and pool0. This matters for over-maximum records; ordinary
   max>=current inputs agree. Do not conflate a C mismatch with a shipped bug.
3. Mech and contiguous-critical temporary pointers are near, despite native
   ES-segment3092 access. They need FAR bindings or an explicitly defined flat
   replacement memory view, not accidental DS3EDB addressing.
4. The C explanation of valid salvage-description indices2..4 is an input-domain
   assumption, not a native upper-bound check. ASM only tests skill>1.

### Preserved native quirks / limits

The heat-sink pool is decremented without being checked before each repair.
The weapon bucket initialization covers10..1F, while collection/repair accepts20;
the last bucket is uninitialized in both representations. Scrap uses party member
index, not Tech skill, and skips member0. These operations were independently
read in this fresh pass, but their gameplay effects were not exercised. Existing
original-bug notes retain their separate evidence/reachability qualifications.
The first critical block's left/right anatomical naming remains the A-005
hypothesis; this audit confirms byte offsets+33..55, not the disputed anatomy.

Literal DS text0EFB/0EFE/0F28/0F40/0F65/0F9E/0FA0 was checked against3EDB.asm,
including06 0F renderer controls. BYTE pools and bounded valid payout fit signed
WORDs; DWORD cash addition still requires exactly32 bits.

## Presentation helper discrepancies

- `174C`:177C/1829 use signed JLE1 for red/green heights, whereas C uses unsigned
  >1.1793 performs signed IDIV6 for heat. Raw8000 yields quotient-5461 native
  versus5461 unsigned C; native delay10-quotient=5471, versus-5451 after C WORD
  assignment. Shipped small heights agree. Native decrement tests the resulting
  sign bit; the signed-short countdown model needs explicit wrap in a port.
  EGA red4/green2 and low-BYTE XOR0A correspond; removed CGA colour paths are
  intentional. Cached top edge for green height1 after red is preserved.
- `1858`:18DA uses signed JL for the total. Total8000 draws zero native pips,
  while unsigned C attempts32768. Known totals2/3/10 agree. First red index,
  pip5 row wrap and all rectangle arguments match under WORD arithmetic.

## Platform methods and unresolved DOS contract

The yes/no wrapper locally matches; this does not fix or confirm0800's callee.
CPU calibration matches functional counting/data flow with a32-bit volatile
BIOS tick view. A compiled C volatile loop cannot reproduce native instruction
cycles; timing/retrace hardware services will be replaced on the new platform.
Initialization and shutdown match their local calls and stores under those
platform contracts, including subtraction truncated before signed division6.

The0D26 entry appears as raw bytes, not a named proc. Decoding accounts for
PUSH BP/MOV BP, compiler zero-frame probe, zero WORD argument, FAR call3CD8,
ADD SP2/POP BP/RETF. The nonlocal DOS runtime response contract is unresolved
and is already documented in the hardware/runtime notes. No further DOS
implementation or Astra spend is needed for this checkpoint.

## Final six comparisons

### Whole-mech salvage04F9–094A

The reordered structured C matches the native branch traversal: best technician,
initial jump to the eight-pilot loop, then accepted-prompt inspection delay and
two RNG calls (within-lance index first). Candidate scan checks the seed before
advancing, wraps16 to0, consumes its wreck flag and recounts16 flags. Enemy
combatants12..15 map to records4..7. Engine3, gyro2 or structure+1F zero requires
Excellent Tech. Rejection rebuilds the saved initial/name; success trims its
message, sets state1 and seeks four free friendly slots. It copies bytes1..124,
restores the initial, binds pilot/assignment, selects Locust versus humanoid
art, forces engine/gyro hits1 and raises zero structure+1F/+20 to1. Exhaustion
sets state/party index9, with both native pauses preserved. The full-lance
fallthrough still announces success/consumes a candidate without installation.

Mismatches: Tech and names use native CBW; qualification0875 uses signed BYTE
JL against8. C unsigned assignment80..FF wrongly qualifies. Near record/copy
pointers drop3092. The trim expression references undeclared
`DynamicStringVariable_w0012`; native0685/0694 use BYTE3092:0012+index, not a
WORD array. Declare an explicit BYTE buffer view before treating it as bound.
Native0FFF is ` is inspecting the wrecks...\r`; C omits the periods/CR.
The valid prompt0FAE, success101D, rejection102F and empty-field104C text match.
Changing a private trim counter to a C loop break is harmless because it does
not escape. No candidate-count guard was added before the native circular scan;
the caller's nonempty-wreck precondition still needs separate validation.

### Personnel loot094B–0B5D

Compared both biased casualty flags for combatant-style indices4..11, the
asymmetric C6EB BYTE gate, eight-or-fewer RNG cash rolls3..18, minimum2, cash
CWD addition, exact pocket/cash/confiscation text and both early exits. The
eight395C flags then choose fallen weapon C6A7+slot*11: Rex present dispatches
distribution, otherwise Jason gets current/fallen names and default-No exchange
prompt. No native armour-transfer branch exists here despite the historical
broad human outline; actual loot is cash and weapons.

Mismatches: raw near cast loses3092 segment; both weapon IDs use CBW natively,
not zero extension. Valid IDs agree. Native post-decrement cash count endsFFFF;
C leaves0, an irrelevant difference in an unused private counter. The A-006
eligibility/consumption address asymmetry is original code and remains an
unresolved gameplay interpretation, not a new transcription correction.

### Encounter generation0D3D–1466

| Native block | Complete local comparison |
| --- | --- |
| 0D3D–0DF0 | Packed-position neighbourhood/origin calls; four RNG rolls for signed10..17 offsets; twelve enemy active/FFFF clears |
| 0DF1–0F0A | Eight name resets and random-presence gates; seven two-bit weapon rolls; body/health/dexterity dice; seven skills; armour type and two damage dice |
| 0F0B–0FBE | Four enemy name resets, random gate then friendly-name guard, signed remainder3 on nonnegative RNG,125-byte FAR template copy and sprite family |
| 0FBF–11F4 | Seventeen-column infantry scan, parity corrections, pre-check tile read, passability/negative checks, cache bounds, discard or packed spawn/frame10, post-record scan advance |
| 11F5–1461 | Unconditional transition advance without wrap; two-cell mech footprint, both pre-check reads, full parity/index/projection/cache tests, discard or frame0 spawn, three-unit spacing and nine-column post-placement wrap |
| 1462–1466 | Cleanup and FAR return |

RNG call order and live-record conditional calls correspond. Infantry/skill
temporaries are near despite native3092 storage. Native threshold comparisons
are signed; unsigned C disagrees for a raw high-bit threshold, though current
55/21 thresholds agree. Failed search loops have no row limit: subtraction must
wrap to signed WORD before SAR, unlike a32-bit promoted C subtraction. Example
scan X=-32768: native low WORD of X-13 is32755, SAR gives16377; promoted C gives
-16391. Placement counter comparisons are signed natively, not unsigned C.
Pre-check negative/out-of-range reads and lack of an upper bound remain native
quirks; this fresh audit does not claim their gameplay outcome was tested.

### Scan browser1467–174B

All three WORD arguments, capability-dependent starting ID, active/coordinate
filters, friendly delegate, enemy mech and human descriptions, health/weapon
cursor writes, scanner-to-target compass call, focus, detail offer, menu choices,
same-target detail return, twelve-ID wrap and Arena clamp were included.
Final layout/default/colour writes and unreachable saved-redraw branch match.

Both native coordinate checks14B6/14C5 are **FFFF**, explicitly confirmed in
.dis; C00FF is wrong. Earlier browser/source/caller notes were corrected, but
operative tests remain unchanged. Several signed target gates and signed BYTE
weapon IMUL differ for invalid high-bit values. Text discrepancies are more
immediately visible:1148 is `Enemy\r`;115B is `Weapon:\r06 0F`;1166 begins
CR and ends06 0F;1174 is `06 0F Scan...\rNext Unit\r` (no space after the
control pair in the actual bytes). C omits these line breaks/control bytes and
the Next Unit row. The browser returns void, not the selected ID; its human
outline was corrected.

### Friendly description18E8–1AFD

Compared mech pilot's unsigned BYTE MUL17, signed name-table index, possessive
label and mech name return; infantry mapping/name and detail gate; traitor
recognition/flag/key path; health and cursor writes; signed weapon-table IMUL;
armour none/ruined/durability-colour/name paths and all common returns.

C zero-extends names/traitor/armour values, while native CBW signs them.
Native weapon IMUL and initial signed combatant gate likewise need explicit
native-width handling. Known party/equipment IDs agree. DS1222 includes
leading space,06 06, a period and06 0F; DS1245 is
`Weapon:\r\rArmor:06 0F`, not the C space-padded label; DS1257 appends06 0F
after Ruined. These controls are lost in current C, not intentional EGA omissions.

### BTSTATS1AFE through final return

| Native block | Complete local comparison |
| --- | --- |
| 1AFE–1D8A | Destruction colour/pointers; asset-cache gate and disk/load; compatibility clear; decompression; EGA A800-to-A000 draw; exact heading; eight-character type; tonnage; friendly pilot/rider or unknown crew |
| 1D8B–1F85 | Engine/gyro/sensor pips; engine plus intact critical heat sinks; all35 weapon-component checks, colours, table-minus-one names, seven location thresholds and display-row increments |
| 1F86–21BE | Actuator labels/colour; both fixed0F leg tests and both max-nibble arm tests, each OK/Gone/Hit case; input flag0 and timeout600 |
| 21BF–22E9 | Eleven armour/structure gauges; signed heat BYTE clamp0..30; heat gauge/retrace; frame increment/test/mask; phase increment/mask; EGA BYTE and MCGA WORD palette writes; input or timeout decision |
| 22EA through return | Default EGA palette restore, final keyboard call, .dis register/frame cleanup and FAR return |

All retained EGA operations and explicit FAR framebuffer arguments correspond.
C near raw record-byte casts discard3092 for sink/weapon/structure reads.
Name BYTE table indices are CBW-signed; record classification gates are signed
WORD comparisons. The header's signed `Mech_HeatLevel` does correctly preserve
native negative-to-zero clamping, so no heat-clamp mismatch is claimed here.
MCGA palette-entry4 remains a WORD write and its phase table is WORD-indexed,
even though only the EGA palette application branch is retained.

Both palette WORD locals are genuinely uninitialized in native code. C also
reads uninitialized locals, but undefined C behaviour is not a faithful port
mechanism: capture starting values or model explicit unknown/seeded phases.
Native first frame increment can miss16 until the mask; then all indices are
bounded. No initialization was silently introduced. The .asm ends after the
keyboard call; .dis supplies the final epilogue. No timing/rendering trial was run.

## Completion / correction boundary

Every retained method has a source-body hash and explicit fresh status.
All nine mismatch methods remain functionally unchanged. These discrepancies
are not nine original-game bugs: several are FAR/signedness memory contracts
or renderer-control losses in research C. The DOS critical-error runtime binding
remains unresolved and is slated for replacement, not DOS reimplementation.

## Verification

- C changes are `Sol:` comments only; functional fixes remain pending review.
- `Verify-MethodInventory.ps1` checks312 definitions, body/ASM record hashes and
  generated-index consistency.
- `Verify-0DABSystematicAudit.ps1` exercises discrepancy boundary models and
  fourteen-record status totals. Models do not run x86 or validate gameplay.
- Checkpoint run passed340 inventory checks and12 new0DAB model/count checks;
  existing0800 checks (23+16) and187656 startup/music assertions also passed.
- Completed-file run passed340 inventory checks and23 expanded0DAB model/count
  checks; existing0800 checks (23+16) and187656 startup/music assertions passed.
  No emulator run or compiled-C gameplay validation was performed.
