# `BTECH_1631` — movement, computer orders and combat helpers

## Scope and confidence

Sol: All 24 routines were read against the clean `reko-expanded-2020`
`BTECH_1631.asm`. All24 local routine workflows are now reviewed, including
the remaining AI epilogue, heat/casualty pass and effects wrapper. This is
research pseudo-C, **not a compilable/executable segment**: overlapping DOS
header views and shared207F renderer/formatter helpers still need reconstruction.
The EGA-only source intentionally does not restore deleted alternate adapters;
their original wrapper ABI is documented below. Names describe verified local
operations; anatomical labels and intended AI weapon-range policy retain
A-004/A-005 uncertainties. No expensive model review was run.

| Entry | Operation / evidence |
|---|---|
| `0006` | Try eight movement directions; tile/footprint and occupancy checks |
| `02E4` | Health/Body signed quotient selects description |
| `032F` | Refresh world view and panels 4/3, optionally draw combat menu |
| `03AB` | Computer actor's movement and weapon orders; restore map anchor |
| `0BB5` | Packed-world distance: floor(max/2)+min |
| `0C63` | Clear invalid targets, update eight mech heats, retire dead infantry positions |
| `0F24` | Target-footprint adjustment and weapon range bracket |
| `1057` | Temporary panel-4 message, wait for key, restore previous layout |
| `10A2` | Weapon ordinal -> raw component-minus-one table index or 00FF |
| `1122` | Transfer damage to another raw armour/structure offset |
| `11AB` | Section destruction / repeated critical-hit dispatch |
| `15FA` | Count nonzero critical bytes without destruction bit |
| `163E` | Destroy one eligible critical byte, return 0/1 |
| `16AB` | Occupancy test; optional opposing-infantry crushing |
| `1B44` | Count absent low-three actuator bits |
| `1B8F` | First of eleven hit-location keys -> text plus period |
| `1BFE` | Turning compass-path blocking test through map cache |
| `1DAB` | Gate FAR combat-message pointer by verbosity WORD |
| `1DCC` | Retrace delay or keypress according to combat speed |
| `1DF8` | Packed position displacement -> screen cells anchored at (26,12) |
| `1EA2` | Signed-delta octant selection, not an audio player |
| `1F09` | Forward FAR effects pointer, re-center on Jason, redraw combat view |
| `1F73` | Fetch one FAR sprite pointer, dispatch graphics-mode draw |
| `1FDF` | Decimal C-Bill balance, right-pad to ten characters, restore layout |

## Sol: reconstructed movement search `0006` checkpoint

0006..02E3 body now transcribed against full ASM; this supersedes its former
damaged Reko C;16AB dependency is reconstructed below. Six WORD arguments, WORD
coordinates/cache indices, signed BYTE facing/search-offset loads and WORD
direction/boundary tables. New header views310A..315D preserve overlapping
WORD315C read/clear and BYTE low-bank toggle with a union. Raw EXE tables
confirm X steps0,1,1,1,0,-1,-1,-1; Y steps-1,-1,0,1,1,1,0,-1;
packed boundary correction X is +/-128, Y is +/-3968 (0F80h).

Eight attempts7..0 cumulatively LOAD signed search deltas before testing.
`.dis` confirms01D6 CMP counter,FFFF(-1), despite ASM printingFF. Heading
already at target exits after zeroing output steps. Initial turn1..4 octants
uses+1,5..7 uses-1; rejected directions do not commit facing or working position.

Cache lookup reads BYTE07AD[WORD09ED+cell], not pointer expressions. Initial
tile threshold comparison is signed JG; adjacent footprint comparison uses
unsigned JC. Mech footprint classification masks low actor BYTE in its first
clause but uses unmasked signed actor WORD for enemy12..15 clause. Probe80
therefore uses slot0 facing and a mech footprint. It bypasses occupancy only,
not cache/footprint blocking. No new cache bounds validation was introduced.

Occupancy receives four WORDs(actor,stepX,stepY,0) at a temporarily assigned
candidate global anchor. Nonzero result rejects it and sets D57E only if
candidate equals destination; restore anchor in either case. D57E is not
cleared here. First allowed candidate updates compassE486/E488, facing BYTE
3920+(actor&7F), and WORD steps458E/4590. No actor position-table writes or
budget decrement; all-blocked leaves zero step.16AB signature/body and callers
are now reconstructed below; the entire research source still must not be
taken as a compilable C claim.

## Movement search `0006` assembly audit context

Six WORD arguments: combatant/probe ID, destination packed X/Y, projected
screen X/Y, alternate-search flag. The sixth is not an uninitialized local.
The alternating search decrements `3EDB:315A`, resets to 30 and XORs the BYTE
at `315C` with 8; the non-alternating path clears a WORD at `315C`.

Facing is a BYTE at `3092:3920+(id&7F)`. `E486/E488` hold the working packed
position, and WORDs `458E/4590` receive the chosen step. The heading helper
returns FFFF when destination equals origin. Otherwise turn one octant toward
the desired heading, then try signed loop counter 7 down to 0. Signed BYTE
adjustments are **loaded** at `3EDB:310A+[315C]+counter`; they cumulatively
change facing. They are not the numeric address itself.

WORD tables `311A/312A` supply X/Y steps; `313A/314A` supply packed-boundary
corrections if low-byte bit 80 becomes set. Cache cell index is
`(screenY arithmetic>>1)*24 + ((screenX-13) arithmetic>>1)`, with parity
adjustments against world anchor `246C:A44B/A44D`; add `CachedMapOriginIndex`
before loading the BYTE at `246C:07AD`. A tile passes if below the blocking
threshold. Mechs also check a horizontal adjacent cell.

IDs below 80 additionally test occupancy at the temporarily assigned global
candidate anchor. A blocked candidate equal to destination sets WORD D57E.
The anchor is restored after the query. IDs with the probe high bit skip
occupancy, not tile checks. This helper also serves roaming NPCs.

## Computer control `03AB`

Save global anchor, focus actor, fill its 48-byte plan row at
`3092:32C6+actor*30` with FF. Enemy IDs are temporarily normalized by subtracting
12; their mech-record offset is 4. Friendly actors set action BYTE
`3994+actor` to 1. Call mech (`183B:22BC`) or infantry (`2474`) preparation.

Sol: setup03AB..047D is now reconstructed in C. Both packed-position arrays
use the original combatant index, and row clearing precedes enemy normalization.
BP-36 is a WORD bank offset4; BP-1A is WORD1 only for a mech with heat BYTE
`006E+sideLocalID+bank` exactly zero, otherwise0. Passed as a movement mode,
this means Run1 for an unheated mech, Walk0 otherwise (not an actuator flag).
Infantry preparation uses sideLocalID+bank: friendly4..11 / enemy8..15.
Sol: BP-1A is named `SelectedMovementMode` (formerly `Bool_Loc1C_1627`),
with `MovementMode_Run` / `MovementMode_Walk` rather than TRUE / FALSE.
All five declaration/use sites were updated; later order-write expressions
are now reconstructed in subsequent checkpoints, with no behavior change
from the rename itself.
Sol: target scan047E..05AA and range selection05AB..0687 are reconstructed;
path clearance/withdrawal0688..0796 and order generation0797..08F8 are also
reconstructed. Target assignment/melee08F9..0A9F is also reconstructed;
mech targeting/heat0AA0..0B7A and epilogue0B7B..0BB4 are also reconstructed.
No AI body block remains pending; range-policy intent remains A-004.
Sol: descriptive local names retain the original Reko numeric suffix for
human cross-reference; declaration and downstream uses were renamed together:

| Original local | Descriptive local |
| --- | --- |
| `wLoc24_1066` | `CandidatesRemaining_1066` |
| `TraitorId_1067` | `CandidateCombatantId_1067` |
| `wLoc06_1068` | `TargetPackedY_1068` |
| `wLoc04_1069` | `TargetPackedX_1069` |
| `wLoc0C_1070` | `NearestTargetDistance_1070` |
| `wLoc14_1073` | `SelectedTargetCombatantId_1073` |

All candidate counters/coordinates are WORDs. The0594 test post-decrements
even on exit;0579 compares FFFF, confirmed in `.dis` despite ASM printing FF.
Signed distance comparison preserves the first candidate on ties; accepted
candidates invoke0BB5 twice. Enemy protection uses CBW on D331 before adding4.
No-target retry is not restricted to the initial mech scan: after infantry
failure it resets count8, subtracting12 only at candidate endpoints12/24.
This can continue scanning beyond the24-entry tables. Caller eligibility
preconditions/runtime reachability remain unverified; no new guard was added.

Mechs prefer four opposing mechs, falling back to eight infantry if none is
selected; infantry scan opposing infantry. Only active WORD entries qualify.
Keep the nearest distance, initially 7FFF; target packed coordinates start
FFFF and default target ID is 23. Exclude target 13 in rental mode and, for
enemy actors, the story-protected friendly infantry selected by D331/D333.
The no-eligible-target path needs a runtime/precondition check, not an invented
early return. A downstream BYTE target alias was moved outside the scan loop
to fix Reko's impossible C scope; it represents the selected WORD's low byte.

Infantry use their weapon's short threshold. Mechs inspect intact weapon-valued
critical bytes and derive a movement threshold. **At 064A the raw component
code, not code-minus-one, multiplies the 11-byte weapon stride.** The high
range bits are masked E0 then shifted right by THREE at 0683 (effectively
four times the encoded short range). Preserve as an A-004 discrepancy until
the relationship to intentionally offset entries/movement policy is established.

Sol: range selection05AB..0687 now uses named WORD locals retaining original
suffixes. `WeaponApproachDistance_1106` replaces `RangeBracket_1106`: this is
a distance threshold, not a bracket enum. Mech record selection uses0..3 or
enemy combatant-minus8; read raw critical offsets33..55 inclusive via BYTE
record view, without double-applying7Dh stride or indexing nonexistent
CriticalSlot fields. Ignore empty/destroyed/nonweapon codes. Minimum packed
E0 bits startFF; no qualifying weapon gives0, otherwise nonnegative SAR3.
Infantry record mapping is friendly combatant-minus4 / enemy-minus8. Original
C5DB/C597 reads equal record+0B Weapon; CBW makes the table index signed.
WeaponStats is indexed by record index only, not index times11h again.
Retained raw mech code indexing and fourfold scaling; no inferred repair.
Sol: Reko's `wLoc0C_1124` / descriptive `RemainingApproachDistance_1124`
is now merged into `NearestTargetDistance_1070`: both represent BP-A,
not independent variables. The original1070 declaration/suffix remains.
ASM08A4 decrements this shared WORD and continues when OLD distance exceeds
BP-2C; the corrected post-decrement loop preserves the final failed-test
decrement as well. No invented initialization/copy was added for1124.

Sol:0688..0796 invokes1BFE with four WORDs(actor,target,targetX,targetY),
saving AX as `TargetPathClear_1131`. Signed ID tests select only infantry
actors4..11/16..23 versus mech targets0..3/12..15. Write3992 WORD1 for friendly
actor/0 for enemy; enemy infantry also write374C WORD1, which later parent
logic uses for flight. Corrected header374C from BYTE to WORD, retaining its
legacy name because the full state meaning remains uncertain.

Using unsigned packed-coordinate comparisons, change destination to actor
position plus6 if actor>target or minus6 if actor<target, independently per
axis; equal axes remain unchanged. This moves AWAY, not toward the mech.
After WORD wrapping, local bit7 triggers X+80 / X&0F7F, Y+0F80 / Y&F07F.
Made backward-Y mask explicit: ~0F80 narrowed to WORD equals original F07F;
this is clarification, not a numerical change. Zero BP-32 and BP-2C then force0797's
direct-order branch; neither actor positions nor global anchor are changed
here. No additional path/occupancy validation was added to this destination.

Sol:0797..08F8 direct/approach orders reconstructed. Zero approach range or
blocked path encodes shared target/withdrawal coordinates directly. Otherwise
07EB signed distance<=threshold skips order generation, leaving the earlier
FF-filled row. Larger distance selects fixed signed WORD deltas-1/0/+1 from
unsigned coordinate comparisons and advances each axis together. Boundary
correction matches0688; no distance recomputation or per-axis stop exists.
Loop decrements shared BP-A on each test, including the false test.

Each first order writes BYTE row+0 mode, +1 combined X-region/Y-region,
+2 localX, +3 localY. Removed impossible address-of assignments and fake
pointer/index handoff. Preserved ASM's in-place local masks after forming
region byte, even though this can erase coordinate region bits. No actual
actor position/global anchor movement, budget cap or occupant check occurs
in this block. The following checkpoint covers target/melee logic from08F9.

Sol:08F9..0A9F now reconstructs infantry row+0 target assignment and
protected-party override (D333 nonzero, signed D331+4 equals actor ->FF).
Only that single slot is written; no infantry twelve-slot clear occurs here.
Mech close-contact gate preserves exact signed ID branches: friendly actor<4
with target<16, or enemy actor12..15 with target<4. Under normal opposing-side
selection these are mech pairs; don't broaden this into arbitrary kick targets.

Unpack live canonical actor/target WORD coordinates to region*128+local;
increment both X anchors by1. With BP-32 path-clear nonzero, use signed WORD
abs helper semantics on separation: X<=3 and Y<2 cancel animation396C and
first movement-mode BYTE32C6 toFF, then set facing3920 from four original
packed coordinates. Later destination bytes remain unchanged. No damage,
actuator test or actual kick occurs here. Removed BYTE-truncated differences
and undefined wArg04 actor aliases. Mech target-row/heat handling starts0AA0.

Sol:0AA0..0B7A now clears all12 target BYTES and uses canonical signed heat
BYTE006E+mechRecord. Original enemy path first reads006E+combatant and
overwrites the result from0066+combatant: only the effective latter read is
represented, with no invented extra heat field. Mech record is friendlyID or
enemyID-8. CBW/JGE yields shutdown at signed heat>=30, not unsigned BYTE>=30.

Below shutdown,10A2 is called for ten ordinals0..9; any return other than
WORD00FF assigns selected target. It returns a weapon-table index, not ammo
quantity. Slot10 staysFF; reserved kick slot11 is assigned without new range
or actuator checks. At shutdown, first movement-mode BYTE becomesFF and all
12 already-FF targets are ORed80 (stillFF), retaining redundant original work.
Renamed locals with original suffixes; corrected false sixteen-iteration loop
and wrong kick-row scaling. No gameplay fixes; epilogue starts0B7B.

Blocking/zero range selects direct destination orders; otherwise move toward
an intermediate position when farther than threshold. Packed destination is
encoded into plan bytes +1 region, +2 local X, +3 local Y. Close aligned melee
cases cancel movement and face the target. Infantry set target slot zero;
mechs clear twelve slots, fill up to ten available weapon slots through 10A2,
and set slot eleven to target. Heat >=30 cancels movement/disables targets.
Finally call movement calculation and optional friendly UI update, then restore
the original anchor. These branches are now reconstructed in research C;
intended raw-code/fourfold range policy remains unresolved, not the local flow.

## Heat `0C63`

Argument is a FAR BYTE pointer to movement metrics. First visit 24*12 target
bytes at 3800: clear high bit on non-FF entries, replace inactive targets with
FF. For each friendly slot 0..3 visit mech records slot and slot+4. Enemy
combat IDs are slot+12, not record IDs slot+4.

Movement contribution is signed plan BYTE +1; when raw plan is 2 use signed
movement metric if >3. Add unsigned EngineHits*5, subtract engine heatsinks and
one for every exact critical BYTE 22 (an intact heat sink). Add signed weapon
heat at 0092+recordID to signed heat BYTE 006E+recordID, with byte arithmetic.
Nonzero D576 adds six and decrements its BYTE counter. Overlapping rows at
32AE+combatID and tile below 16 at 3750+combatID subtract four.

Negative heat is clamped at the current record index. Upper clamp mistakenly
uses the friendly slot, omitting enemy offset: see BUG-013. Contribution BYTE
0092 is cleared even for skipped/destroyed mech records. Dead infantry records
map to combat IDs record+4 (friendly), record+8 (enemy). Except Jason, their
packed X/Y are set to **WORD FFFF**, not byte-valued FF. Enemy sprite-family
offset becomes FE; dead Jason gets 96. This routine does not clear active
flags in that final loop.

## Weapon/damage helper corrections

`0BB5`: unpack X as `(packed&0F00)>>1 | packed&7F`, Y as
`(packed&F000)>>5 | packed&7F`. Signed WORD differences become absolute values;
return half the larger plus the smaller. Old unsigned comparisons could never
detect negative differences.

`0F24`: retain WORD target coordinates. For mech targets beyond distance 3,
move X one toward anchor, and subtract TWO from target Y only when anchor Y
is smaller. Boundary tests use the already changed coordinate, not a second
subtraction. This asymmetric footprint adjustment exists in ASM. Weapon
records have 11-byte stride, already implicit in a typed array. Maximum range
is pre-scaled; scale short/medium by three for categories below 80 except
kick index 20. Comparisons are strict: distance equal to maximum is out of
range, equal to short falls into the next bracket.

`10A2`: zero-based weapon ordinal counts masked critical codes 10..20.
Compare the OLD count then increment. Return the unmasked raw code minus one,
preserving destruction flag; return WORD 00FF if missing or current ammo zero.
The legacy name is retained with a rename TODO. There is no ammo-index bound:
ordinals 10/11 read movement fields (BUG-012). This helper does not decrement
ammo or calculate its quantity.

`1122`: 11..18 -> offset+0B; 19->1D, 1A->20, 1B->22,
1C/1E->12, 1D/22->15, 21/23->17. Inputs 1F/20 set E484 destruction flag but
leave the WORD return local unassigned, as do invalid inputs. That is a
caller-contract concern until the damage consumer is reviewed, not automatically
a player-visible bug. The old C erroneously switched on zero-based selectors.

## Critical dispatch `11AB`

Arguments are WORD mech record ID and structure offset. If structure BYTE is
zero, use section start/count tables indexed by structure offset at
`3EDB:316E` / `3176`, mark all nonzero section bytes with bit80, mask appropriate
actuator nibble and return. A damage roll from `0800:19DD` selects a critical
count (initial 1; if roll>=8 use `(roll-8)/2+1`). Verbosity emits the critical
message in panel4 and sets WORD4586.

| Structure offset | Assembly dispatch |
|---|---|
| 1C / 21 | High actuator nibble at 24/25; seven-byte blocks 33/41 |
| 1D / 22 | Seven-byte blocks 3A/48 |
| 1E / 23 | Low actuator nibble at 24/25; two-byte blocks 4F/51 |
| 1F | Head systems: 78 state, 77 sensors, structure 1F, critical 55 |
| 20 | Engine/Gyro counters 75/76; heatsink block 53 count2 |

Literal offsets are intentional: do not silently impose tabletop anatomy or
repair the existing names under A-005. Head rolls 1/6 set nonzero state78 to
FF, 2/5 increment sensors77 up to 2, 3 zero head structure1F and mark destroyed,
4 selects critical55. Center-system rolls choose gyro or engine and destroy
at gyro>=2 or engine>=3, zeroing center structure20; otherwise damage one of
two bytes at53. Actuator selection uses mask tables at 316A/3172 (low nibble)
and 317A/3182 (high). Ineligible rolls retry without consuming critical count;
eligible component hits subtract 163E's return count.

Sol:11AB..15F9 now reconstructed against full ASM. Signature WORD mech
record ID and structure offset; raw BYTE record view avoids double stride and
fictitious structure/component aliases. BP-4 destroyed-section flag, BP-0A
remaining criticals and BP-0C dice roll are separate WORDs. Always rolls2D6
and may display315E Critical message/4586 latch BEFORE destroyed-section
handling. Roll below8 leaves initial critical count1,8/9 ->1,10/11 ->2,12 ->3.

Destroyed section marks every nonzero slot, then clears actuator arm nibble
for1C/21 or leg nibble for1E/23, with no new E484 destruction-flag write.
New header tables rebase original316E/3176+offset accesses to eight BYTES
318A/3192 indexed offset-1C; raw EXE start/count sequences verified.
Actuator rolls3..6 use four-entry rebased mask tables316D/3175/317D/3185;
selected missing bit retries, otherwise BYTE AND consumes one hit. Rolls1/2
or an empty nibble try the critical block; helper0 consumes no hit. No eligible
actuator AND no intact component ends remaining hits for limb sections;
torso-only sections stop when their seven-slot block is empty.

Head has no general empty-section stop test: zero state78 and sensors>=2
retry; stateFF is still nonzero and may be hit again. Preserve this behavior,
probable life-support name and invalid-input retry, not inferred tabletop rules.
Roll3 destroys head/E484, roll4 tries the single55 slot with count1 (not2).
Central first roll1..3 chooses another roll:1..3 increments gyro76 (fatal>=2),
4..6 engine75 (fatal>=3); otherwise first roll4..6 damages53/54 with count2.
Fatal result zeros centre20 and ends hits; counters are BYTE increments.
No new engine/gyro eligibility cap added. No silent original-game repairs.

`15FA` counts intact nonzero bytes in a FAR block. `163E` first counts,
returns zero if empty, otherwise starts at random&3, normalizes to zero if
past count BEFORE dereferencing, scans cyclically, marks ONE byte and returns
one. This is biased first-eligible selection, not uniform random choice.

## Collision and line blocking

`16AB`: four WORD arguments (actor, stepX, stepY, crushOpposingInfantry).
Step X/Y are unused; candidate is global anchor. Infantry reject another
infantry at the exact position, then check both friendly/enemy mech footprints
via temporary anchor and `0800:191B`, restoring it. Mechs reject active other
mechs on the same packed Y with unpacked corrected X distance<3, then same-side
infantry distance<2. If still unblocked and crush flag nonzero, scan opposite
infantry and kill overlapping entries: death tile7E, sprite-family0, active0,
health0, nameFF, sound12; special first two friendly records clear flag014A.
Sol:16AB..1B43 now reconstructed with four WORD arguments and WORD AX result;
this supersedes the former two-BYTE declaration/damaged body. Both callers
verified:0006 pushes(actor,dx,dy,0),1AE8 execution pushes(actor,dx,dy,1),
each ADD SP8. Step WORDs are never read; global candidate anchor owns position.
Planning disables crushing; execution enables it. Updated caller comments and
replaced execution literal1 with TRUE, without changing ordering or behavior.

Infantry exact-position loops check friendly4..11/enemy16..23, excluding self,
but DO NOT test active or unused-position flags. Their mech footprint scan also
has no active guard: test sameY and X centre/left/right using temporary global
anchor and191B(-1,0),then191B(2,0). Restore original candidate before return.
The old annotations' reset-to-false and duplicated X increments were artifacts.

Mech path corrects packed X+1 by+80h when low bit80 set, unpacks regionX*128
plus localX, and compares active other mech sameY absolute X distance<3.
Then decrement unpacked candidate X and check same-side active infantry at
distance<2. Preserve minimum separation semantics, not BYTE differences.
207F:3C6C is signed WORD abs; negative32768 remains negative after16-bit NEG.
Inline short negation in research C preserves that original arithmetic edge.

Only if still unblocked and crush flag nonzero, XOR side base4/16 with14h
to scan opposing eight personnel. Multiple overlaps can die; no early exit.
Friendly combatant IDs4..11 -> records0..7; enemy16..23 -> records8..15.
Name-index load is signed BYTE CBW; enemy text319A and suffix31A6 verified
against expanded EXE. Sound12h, speed<5 delay and an extra friendly-record<8
delay precede tile7E registration, frame7E/family0, active WORD0, health BYTE0,
name BYTEFF; friendly records0/1 clear main-alive WORD014A. No position-table,
393C casualty or395C loot write is present. Blocked BP-6 stays0 after killing,
so execution accepts the move. No original bug or missing behavior was repaired.

Sol:1BFE..1DAA now reconstructed against full ASM with WORD AX result and
four WORD args(actor,target,targetX,targetY). Actor is unused. AI caller0688
lost targetY in Reko C; restored canonical X/Y WORDs and ADD SP8 contract.
Execution caller1AE8:01B5 already passes all four correctly. No surrounding
AI workflow rewrite included. Rebuild
cache origin at the global anchor. Rental mode target13 returns clear early.
Otherwise begin cache cell origin+96, adjust for X/Y parity, reject an initially
blocked tile, and walk toward target. Desired compass changes by at most one
octant each step; WORD tables 328A/329A step world position, 32AA/32BA correct
boundary, 32CA supplies cache Y displacement. Cache advances according to
parity. A blocked tile sets return flag zero and forces working position to
target to exit. This is a turning compass path, not a Bresenham straight ray.

## Presentation and verification

### Sol: remaining-segment completion checkpoint

The owner explicitly authorized larger blocks/all remaining1631 work. Changes
were kept to AI completion, the heat/casualty routine, presentation contracts
and their directly evidenced header views. Existing annotated critical,
movement and collision bodies were preserved rather than rewritten again.

AI0B7B..0BB4 always calls183B:193B movement calculation; only friendly actors
with requested preview call1774(actor,1), then17BB restores saved A44B/A44D.
Caller183B menu uses flag1, automated enemy/friendly control flag0. Named actor
ID/preview arguments now reflect actual use; suffix1044/wArg04/wArg06 retained.
`Target_MechOffset` is now `ActorRecordBankOffset`; `bLoc14_1338` is
`SelectedTargetByte_1338`. This removes actor/target ambiguity, not behavior.

Heat0C63..0F23 is fully transcribed. Normalize all24x12 target BYTES by
clearing bit80 except FF and discarding inactive targets. Iterate four pairs
of mech records0/4,1/5,2/6,3/7, using combatant0..3/12..15 for plan/terrain.
Signed plan BYTE+1 yields no-move0, Walk1, Run2, Jump3; jumping uses signed
successful-step metric when>3. EngineHits*5 is unsigned BYTE multiplication;
subtract engine-mounted sinks and each exact critical22, not destroyedA2.
Add signed weapon heat, update heat with wrapping BYTE arithmetic before
inferno+6/counter decrement and terrain cooling4 (overlap nonzero, tile<16).
Clamp negative heat at actual record; preserve BUG-013's upper clamp at
friendly slot even for enemy. Always clear weapon-heat accumulator, including
destroyed records. Header inferno state is now eight BYTES, not four WORDs.

Dead infantry map to combatant record+4 / record+8. Except Jason, invalidate
X/Y with WORDFFFF; enemy family becomesFE, Jason family96 without coordinate
invalidation. No active flag clear was invented. Canonical packed-position,
active, target, heat and terrain views replace malformed pointer/arithmetic
and double-stride accesses. Original numeric suffixes survive local renames.

Small-helper consistency pass confirms health signed IDIV, panel ordering,
range footprint asymmetry/strict comparisons,10A2 ordinal semantics,1122's
unassigned-return contract,15FA counting/163E cyclic selection,1B44 bit count,
1B8F text sequence and1DAB/1DCC filtering/delay. Changes restore signed
WORD range ID tests, WORD actuator count, full WORD hit-location argument
against CBW keys and signed speed-setting comparison. Shared text scratch
is the BYTE view0012. No original-game bug repair or inferred anatomy rename.

Effects1DF8 retains unsigned packed displacement and screen origin(26,12).
1EA2 now uses WORD octant and explicitly narrowed signed NEG results,
preserving8000 overflow/ties.1F09 forwards the true FAR pointer plus WORD1,
then uses signed assignment BYTE/CBW to focus Jason and redraw world/menu.
Invalid negative assignments are not given an invented safe fallback.

Sprite1F73 now loads explicit Offset/Segment words from39FA+id*4 and calls
EGA0377 with encoded DOS FAR destinationAC00:0000. The header exposes one
four-byte pointer table view, keeping old aliases for0800 callers. Its256
entries describe the BYTE sprite-ID domain, **not a proven allocation length**;
actual loading/extent remains shared-renderer work. The old120-entry claim
was already contradicted by known entry92h(146 decimal) at3C42:3C44.
Original alternate adapter branch4FBA!=2 passed the same source/X/Y to28EB
with destination246C:244B; it remains intentionally omitted in EGA-only source.
0377's WORD signature/clipping body is now reconstructed in the
[207F graphics checkpoint](BTECH_207F_SEGMENT_REVIEW.md), including original
early-cleanup BUG-019. Pointer loading/table extent and other shared rendering
remain separate boundaries, not another1631 workflow block.

C-Bill1FDF padding now has named `BalanceTextLength_101` and signed JL;
retains original width10, termination, colour/layout restoration. DWORD balance
is a genuine16-bit-C unsigned long, not a suspect host pointer. Shared3BD2
formatter's missing radix is now recovered by the207F strings/formatting
review, and this caller passes explicit radix10. The formatter uses signed
decimal interpretation despite unsigned storage; see the shared207F notes.

Verification: combat transcription script3034 assertions and toolkit182 pass;
these use synthetic inputs, not execution of research C/DOS gameplay. Added
heat wrapping/jump/inferno/terrain cases, explicit BUG-013 compatibility case,
destroyed-record reset, target normalization, dead-record mapping, octant ties,
FAR pointer word addresses/encoding and friendly-preview gates. No copyrighted
game files were changed/staged. No Astra invocation needed for resolved local
flow; A-004/A-005 remain optional deeper-intent/anatomy confirmations.

Local1631 review is complete. Next work is shared207F renderer/formatter and
combat-wide naming/contracts, followed by portable subsystem specifications;
runtime compatibility validation remains separate from this annotation pass.

Sol: owner requested preserving named-constant context. Combatant-range
constants remain in all relevant actor/target classification and mappings.
Removed cross-domain uses of Enemy_Infantry_CombatantId_Range_First(16) and
Enemy_All_CombatantId_Range_First(12) are replaced with meaningful domain
constants, not bare numbers: Infantry_Record_Count(all character records),
Combat_TerrainCooling_TileRange_End(tile threshold), Combat_WeaponTarget_RowSize,
Combat_MovementOrder_RowSize, Mech_WeaponOrdinal_Count and Combat_KickTarget_Slot.
This prevents equal numbers from falsely implying a combatant-ID relationship
while retaining human context. Bool_wArg06 is ShowPlanningPreview_wArg06;
source documents the original name and friendly-only preview gate. Values,
evaluation ordering and behavior unchanged; transcription3034/toolkit182 pass.

Health descriptions use signed Health/Body quotient and FAR table at 2E0C.
Actuator helper reads one raw record byte and counts missing bits 4/2/1, 0..3.
Hit-location helper searches eleven signed keys at3242, copies selected FAR
string at324E, appends period, then emits according to verbosity. Text pointers
to 1057/1DAB and effects pointer to1F09 are real 16:16 pointers, not native
32-bit addresses. 1DF8 converts packed displacement to screen cells (26,12).
1EA2 selects signed octants with original tie handling (zero/zero ->6).
1F73 fetches ONE FAR pointer at39FA+spriteID*4: mode4FBA==2 draws toAC00:0000
via207F:0377; other modes use246C:244B via207F:28EB.

1057 and1FDF now save layout4600 BEFORE changing panels and restore it.
C-Bill display formats DWORD balance radix10, measures resulting text, pads
with spaces to ten chars, terminates, emits, sets colour15. Shared formatter's
legacy two-argument signature remains a reconstruction boundary.

Direct EXE checks use MZ header + `(1631-0800)*16+offset`; prologue0006 matches.
At0E6C bytes `8B 5E F8` load [BP-8] alone, `26 80 BF 6E 00 1E` compare heat,
and0E7B `26 C6 87 6E 00 1E` stores30 at the same index. `.dis` confirms signed
comparison. Earlier negative clamp adds [BP-0A] before accessing heat, proving
the enemy-offset omission. No copyrighted files were modified or staged.
Regression results and remaining reconstruction work are recorded with this
commit; toolkit tests cannot validate execution of research pseudo-C.

Verification: InceptionTools suite passed **182 assertions**; five inline
PowerShell distance-transcription sanity cases passed (same point, positive and
negative deltas, X and Y region boundaries). These test mathematical transcription,
not compiled C. `git diff --check` passed. The clean ASM ends partway through
1FDF after pushing saved layout. Raw EXE bytes confirm the restore call at20A1
and `8B E5 5D CB` epilogue at20A6..20A9. The next prologue begins at20AA;
no routine boundary is inferred solely from the truncated ASM listing.
