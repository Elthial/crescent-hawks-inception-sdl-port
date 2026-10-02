# Combat movement and critical-slot/ammo linkage

Sol: Current checkpoint (2026-09-17): later effects sections in this document
resolve the misleading boolean names and apparent shooter-frame/projectile
alias. Numeric Reko suffixes remain intentionally retained, not native BP
positions. Earlier pending/TODO prose records the review sequence.

Sol: Reviewed checkpoint, not a wholesale C# port. Evidence is the clean
`BTECH_183B.asm` at000A..017B,06F1..089F,1774..1C1E,22BC..24EF,273D..27C8,
and `BTECH_1AE8.asm` at000C..12C6. Raw EXE prologues at183B:193B/22BC/273D
match the MZ-header + `(segment-0800)*16+offset` mapping; raw ammo-consumption
and kick argument bytes were also checked. No game assets were staged.

## What changed, and what did not

Reconstructed complete movement builder193B, mech movement-budget helper22BC,
and supporting-structure helper273D. In1AE8, reconstructed weapon resolution,
ammo consumption, friendly completed-order removal and mech damage application.
Corrected movement execution's BYTE pair cursor, signed deltas, WORD position
indexing and named stack movement counts. Header aliases now name both order
and step storage; WeaponStats includes all33 records, including Kick index32.
Shared1631 step-output/facing/blocked/weapon-heat references use the same named
views, and0C63's signature now explicitly accepts the FAR movement-count pointer.
All new interpretations/comments use `Sol:`.

Parent/menu expressions and much of1AE8's personnel attacks, skill/accuracy
calculation, hit-location selection and effects12C7 remain damaged Reko C.
Do not treat these files as executable C. Only the named reconstructed blocks
have had control flow replaced; alias substitutions elsewhere rename proven
storage without claiming the surrounding expression is repaired. Header fields
are overlapping research views, NOT additional sequential DOS allocations.

## Storage and aliases

### Reko `fp` audit

Sol: `fp` here is the decompiler's frame-pointer placeholder, **not a file
pointer**. All affected ASM functions establish `push bp; mov bp,sp` and use
SS-relative BP addresses for these local arrays. The old expressions also had
two-byte offset errors; the ASM is authoritative:

| Old expression | Actual local | Evidence |
|---|---|---|
| `(fp-122)[i]` in1AE8 | `SuccessfulMovementSteps[24]`, SS:BP-78 (decimal120) | 0034 initialization;113C movement increment;02E0..0311 target-count load;12B4 passes SS:BP-78 to heat helper |
| `(ss:fp-30)[i]` / `(fp-30)+i` in183B parent | `SavedCombatTerrainFlags[9]`, SS:BP-1C (decimal28) | 003D copies terrain bytes;1232 tests saved BYTE bit80 before salvage |
| `fp-16` in183B preview | `MovementPreviewGlyph[2]`, SS:BP-0E (decimal14) | 187C..1899 initializes NUL byteBP-0D;18A0..1902 loads glyph intoBP-0E, forms pointer and pushes SS |

The last commit had already replaced the1AE8 local; this follow-up removes the
three remaining `fp` expressions in the reviewed files. Preview reconstruction
also restores signed step consumption, actual X/Y arguments and the two-byte
NUL-terminated glyph buffer. It is stack text, not file content. Other untouched
Reko files can still contain `fp` and require their own instruction-level audit.

| Address | Storage | Alias resolution |
|---|---|---|
| 3092:32C6..3745 | 24 rows *48 BYTEs | `CombatMovementOrders_32C6`; destination orders |
| 3092:3506 | Order row12 | Enemy orders, not a second array |
| 3092:32CA | Order row0+4 | Next order, not a separate field |
| 3092:32DE /351E | Half-row offset+24 | Initialization writes second halves of friendly/enemy rows |
| 3092:40B4..42F3 | 24 rows *24 BYTEs | `CombatMovementPlanBytes_40B4`; twelve signed step pairs |
| 3092:40B5 | Step X base+1 | Y of same pair, not another array |
| 3092:41D4 | Step row12 | Enemy half of same step table |
| 3092:0078..008F | 24 BYTEs | `CombatMovementStepCursor_0078`; pair cursor reset each turn |
| SS:BP-78..BP-61 | 24 local BYTEs | `SuccessfulMovementSteps`; counts nonzero successful moves |
| 3092:0092..0099 | Eight BYTEs | `CombatWeaponHeat_0092`; indexed by mech record, not combatant |
| 3092:008A+enemyID | Biased heat address | Same as0092+(enemyID-8) |
| 3092:0066+enemyID | Biased heat-level address | Same as006E+(enemyID-8) |
| 3092:3920 | 24 facing BYTEs | Separate from sprite selector396C |
| 3092:458E /4590 | Signed WORD step outputs | Scalars from Position_0006, not pointers/arrays |
| 3092:3800..391F | 24 rows *12 BYTEs | Planned targets; FF none, highbit already fired/disabled |

Native stack arguments and indices are WORDs; stored modes, steps, ammo and
critical codes remain BYTEs. Segmented pointers are16:16, not native32-bit
addresses. Mech combat IDs0..3 map directly to records0..3; enemy12..15 map
to records4..7 by subtracting8. Infantry4..11 -> records0..7;16..23 ->8..15.
The previous `/4` position accesses were Reko artifacts: ASM uses id*2 into
the WORD position tables, not id/4 into a C array.

## Movement workflow

`183B:000A` clears both halves of order, step and target tables. In the
friendly planning path06F1..07CE, active actors with action state3994==0 get
paths generated if necessary; other actors use computer control. Enemy
computer planning starts with1482(12). At0889 call1AE8:000C with combat-rule
flag. This is the relevant workflow, not a claim to have reconstructed every
entry/escape/story/cleanup branch in the parent.

Each four-byte order is `[mode, region, localX, localY]`. ModeFF terminates;
other modes decode X=`localX|(region<<8 &0F00)`,
Y=`localY|(region<<8 &F000)`. Modes0/1/2 mean walk/run/jump in budget handling.
`193B` starts the working packed position at global anchor, projected cells
at(26,12), fills the generated row with02, then calls Position_0006 until
destination, destination-blocked flag or budget exhaustion. It appends low
BYTE X/Y outputs before subtracting terrain cost. Jump costs1; ordinary
terrain costs1/2/3 according to tile value and parity mask at3EDB:3B12.
The final step can cost more than remaining points, followed by a zero clamp.
Builder changes working coordinates/facing but restores original facing;
stored actor positions change only in the turn runner.

No builder output-index<24 or input-index<48 guard exists in this ASM. Valid
budgets/terminators are preconditions; edited saves can violate them. A future
managed implementation should bound both tables explicitly, without mistaking
that safety policy for original behavior. Position_0006 remains a reconstruction
boundary; its audited behavior, rather than its broken old C, defines the call.

`22BC` reads WalkMove or JumpMove. Walking with neither low-nibble bit8 sets
one point; one missing bit8 gives ceil(walk/2). Missing bits4/2/1 each subtract
points, with the original exact-zero floor check inside the loop. Walking/run
subtract signed heat/5; run adds arithmetic half plus original WalkMove parity.
Jump bypasses these penalties. Heat==30 forces zero for all modes; final
negative points clamp tozero. The original ==30 test is not silently >=30.

`1AE8:000C` interleaves movement and firing over twelve slices. A generated
pair is at`actor*24 + cursor[actor]*2`. CBW expands FF to-1. Either02 stops
that actor, and(0,0) is skipped without cursor advancement. Candidate movement
uses global anchor; collision check16AB(actor,dx,dy,1) can crush infantry.
Only successful moves increment cursor, update stored WORD positions and
increment the local movement count. A blocked pair retries in later slices.
Local counts feed target accuracy penalties and the final heat call; global
cursor is not the movement-heat metric.

After firing,0F03..0FF5 checks first destination of FRIENDLY actors0..11.
If reached, copy44 bytes from order row+4 onto row (forward overlap), set
BYTE44 FF and leave the final three bytes unchanged. This retires one waypoint,
not a step pair, and is not a memmove of twelve unrelated records.

## Weapon linkage — resolved executable behavior

1. Scan raw critical bytes33..55, accepting masked codes10..20. Each accepted
   byte occupies ONE ordinal, including destroyed/highbit entries.
2. For requested ordinal,1631:10A2 returns **unmasked code-minus-one**. Thus
   component10 -> weapon index0F,20 ->1F; destroyed90 ->8F, then firing rejects
   its highbit. No sorting, weapon-type ammo search or duplicate-code grouping.
3. Availability reads BYTE`record+27+ordinal`; zero returns00FF. Destroyed
   entries keep their ordinal, so subsequent ammo fields do not shift.
4. Range and blocking tests run before ammo consumption. Then0239..0296
   decrement that same ordinal's BYTE unless FF (unlimited), BEFORE the hit
   roll. Misses therefore consume finite ammo too.
5. Slot11 is kick, index20, not an ammo-bearing component. Both low actuator
   bit8 values must exist. Infantry use their record's Weapon byte directly.
6. On a processed attack,0D76 marks target highbit and exits this actor's
   slot scan for that slice. Later slices advance to other unfired slots.

Consequently repeated critical codes are separate selectable/executable weapon
entries in this game, not deduplicated physical occupancy. This settles the
ordinary ammo/critical linkage. Whether an upgrade author *intended* multiple
identical weapons is separate from what this executable does. The six10-byte
Commando upgrade writes therefore expose six small-laser entries if their
ordinals are reachable; do not collapse them to "two lasers" in a compatible
combat model. Slots beyond the ten real ammo fields remain a separate defect.

**BUG-012 consequence confirmed:** slot10 passes the `<11` consumption check,
so reads/decrements WalkMove at31 as if ammo. Slot11 instead takes kick path.
Enemy baseC363+enemyID*7D equals C724+(enemyID-8)*7D+27: this is correct bias,
not an enemy-only ammo corruption. The computer planner's raw-code range lookup
at1631:064A still differs from firing's code-minus-one; A-004 retains that issue.

Weapon field2EE4 is attack-count/cluster-column, not a simple missile boolean.
For ordinary mech attacks with value>1, damage multiplies by the BYTE cluster
table entry at2E5E+TwoD6Roll*7+column. The old C multiplied by that numeric
address rather than loading its byte. Named77-byte table begins2E6E.

## Critical and damage linkage

`183B:273D` returns supporting structure value, **not** a destruction boolean.
Its threshold mapping agrees exactly with critical dispatch1631:11AB:

| Critical range | Structure offset |
|---|---|
| 33..39 | 1C |
| 3A..40 | 1D |
| 41..47 | 21 |
| 48..4E | 22 |
| 4F..50 | 1E |
| 51..52 | 23 |
| 53..54 | 20 |
| 55 | 1F |

This resolves numerical grouping, not all anatomical names (A-005). Special
record7B==C8 returns1 without checking supporting structure; purpose unknown.
It is not evidence for a200-rated engine.

Mech damage0B43..0CD2 indexes a BYTE at the raw hit-location offset. Damage
greater than that byte zeroes it, subtracts its old value, calls critical
handling for offsets1C..23, then calls1122 for overflow transfer. Exact/lesser
damage subtracts directly, consumes the damage, and also calls critical
handling for internal structure. Arm-loss flag is gated by raw offsets1C/21;
head/center1F/20 reaching zero ejects the mech. Inferno index0B instead sets
three-turn D576 heat state and clears ordinary damage. The surrounding attack
count/personnel path is still marked for reconstruction.

On fatal overflow1F/20,1122's unassigned return does not become another record
access: destruction flag causes ejection/NameFF, then the damage loop checks
NameFF before dereferencing the next offset. Invalid hit-location inputs still
need rejection in a managed port. This narrows the former return-contract concern.

**Further original discrepancy:** enemy kick calculates normalized record ID
in localBP-7A, but at02BC/02D5 passes unnormalized combatant ID to actuator
helper1B44. Its mech stride then reads beyond the eight-record table. Retain
as a probable original bug pending live enemy-kick confirmation; do not fix
the call while presenting the change as mere decompiler cleanup.

## Sol: accuracy reconstruction (1AE8:0297..0428, 071B/076A/091D)

BP-30 is a signed WORD target number: a 2D6 roll meeting or exceeding it
hits. Ordinary base is `rangeBracket*2+4`; kick base is 3 plus actuator
penalties. Add signed order-mode BYTE plus one (FF therefore adds zero),
then the signed movement-penalty table BYTE indexed by successful target
steps at SS:BP-78. Table contents/policy are not newly inferred here.

Infantry attackers map to character records by subtracting 4 (friendly) or
8 (enemy). Mech attackers read unsigned PilotId at record+79; nonzero
SensorHits at +77 adds 2. Current signed heat adds one at each threshold
8, 13, 17, 24. Weapon heat's low nibble accumulates separately in BYTE0092.
Enemy biased addresses normalize to mech records4..7 for all these reads.

071B subtracts the signed character skill BYTE at record+04+weapon.SkillType.
076A adds signed cover arithmetic-shifted by one for personnel targets;
091D shifts it by three for mech targets. Reko incorrectly shifted the
entire mech target-number expression and used unsigned result locals.

Changes are limited to this arithmetic and character-record naming; damage,
hit-location selection and message construction remain separate review work.
The raw skill index and PilotId have no original bounds validation; a managed
port must establish validation policy rather than silently invent one here.
BUG-014 enemy kick helper arguments remain unchanged intentionally.

Sol: readability correction following user review: retain the existing
`MECH_Heat_Level1..4` and combatant-range constants in the accuracy code.
End-of-line comments explain their thresholds, record mappings and residual
numeric penalties/strides. Future ASM cleanup must preserve annotated meaning;
use existing meaningful constants, or explain literals where no suitable name
exists. This correction changes presentation only, not arithmetic or behavior.

## Sol: personnel attacks (1AE8:076A..0915)

High bit of WeaponType selects personnel damage encoding: low Damage nibble
is a bonus, high nibble counts D6 rolls. Roll damage once, in a WORD local;
non-personnel weapons instead start at127. Zero initial damage becomes1.
Low seven WeaponType bits count attack repeats. One signed 2D6 comparison
governs the whole batch; a miss cancels all repeats. Both ASM loops test the
old count before decrement, not equality with an effects-category constant.

C622 armour and C623 health are loaded as signed BYTES (CBW). Nonzero armour
at least floor(damage/2) changes the damage local to that half and subtracts
the same half from armour; otherwise subtract remaining armour from damage
and clear armour. Compare signed health against WORD damage, clear on lethal
damage or subtract the low damage BYTE. Crucially, damage is NOT reset between
repeats, and the loop does NOT stop at zero health. Thus repeated armoured hits
can progressively halve damage, and damage1 can become0 despite the earlier
minimum. Record these as observed mechanics, not confirmed bugs/author intent.

After the batch, zero health sets BP-2C death flag. Downstream effects12C7
receive it; actual removal, NameFF and story consequences are not newly audited
in this block. Miss-triggered fire uses WORD FFFF, not BYTE FF. Its tile interval
10..3F is retained with an uncertainty comment rather than invented tile names.
Removed Reko pointer/value confusions from armour/health accesses. Research C
remains non-compilable elsewhere; synthetic checks do not run DOS gameplay.

## Sol: mech hit-location selection (1AE8:093F..09F1)

Read signed animation/facing BYTE396C; FF selects signed saved BYTE45B6,
not a WORD table. Subtract the attacker-to-target compass heading (BP-60,
copied separately to BP-58 for effects). No modulo-eight normalization occurs.
For normal facing/heading0..7, difference -7..7 indexes bytes2D0A..2D18 via
biased base2D11. Verified expanded-EXE values in order:
`1,1,0,0,0,3,3,2,1,1,0,0,0,3,3`. Category names are deliberately not guessed.

095E loads signed category and independently rolls2D6; unsigned hit BYTE is
at2E40+category*11+roll. Named research view starts2E42 (first valid roll2),
four eleven-outcome rows through2E6D. This is a raw mech-record damage offset,
not a critical-slot index. Reko omitted the category stride and used an
unrelated undefined heading local. Nonstandard/invalid facing values still
need a managed validation policy; the fifteen-byte view describes normal inputs.

Kick still consumes that ordinary location roll before overriding it. It
reads unsigned tonnage (enemy C34C normalizes to record+10), divides by5,
and writes shared WeaponStats[kick].Damage at3EDB:3103. Random bit3 chooses
BYTE2E43 or2E4B, verified as raw13h or18h. These are table aliases, not an
independent WORD kick table. Selection precedes the later hit roll, even on
a miss. No claim of front/side/rear naming or anatomical mapping is made here.

Verification reads were local to the ignored expanded EXE; no binary or
external asset was staged. Source changes remain restricted to selection.

## Sol: effects argument contract and personnel death (1AE8:12C7/1B0C..1BE2)

Caller0CD3 pushes nine WORD arguments and FAR address3092:4314 (22 bytes).
Actual BP-relative arguments are shooter+06, target+08, weapon+0A, compass+0C,
attack/effect state+0E, personnel-death flag+10, mech-destroyed flag+12,
character/mech record+14, hit/applied flag+16, buffer offset+18/segment+1A.
Reko BYTE parameters and native int pointer are incorrect extrapolations.
Legacy parameter suffixes and two boolean names remain explicitly TODO until
animation-path consumers are audited; they do not describe true stack offsets.

1B15 marks target death/wreck WORD393C, clears active WORD406A, writes character
NameFF, sets sprite BYTE7E and base-family BYTE0. It registers the persistent
impact using the target's current WORD packed coordinates before clearing them.
All24 rows of12 target BYTES are scanned; compare low7 bits against target ID
and replace matches withFF, including fired/high-bit entries. Original Reko
used the high-bit mask instead, plus a malformed int-pointer expression.
Character records0/1 (Jason/Rex) clear main-characters-alive WORD014A.
Coordinates become WORDFFFF, not BYTEFF. Effects state cleanup also runs with
graphics disabled or main characters already dead. Mech destruction and arena
spawning remain outside this reviewed branch.

## Sol: wreck placement and arena reinforcements (1AE8:1BFC..1E28)

For a destroyed mech, ordinarily select wreck80 for family0, wreck81 otherwise.
Subtract one from both packed WORD coordinates; if low-byte bit80 is set,
mask X with0F7F or Y withF07F, exactly as ASM. This is not a byte coordinate
or a call to general packed-map normalization. Register wreck then play sound8.
Rental-arena mode with destroyed target combatant13 bypasses that ordinary
wreck and instead displays the arena destruction/reinforcement messages,
sound7 and screen refresh centered explicitly on combat mech slot0.

1D32 copies125 template BYTES2FE8:065B into3092:CA12 andCA8F: records6 and7
at C724+record*7D. Template index is7, not the former header08 (array has8).
These are corrected transcription/annotation mistakes, not original bugs.
Reinforcement combatants14/15 receive frame0, family0, FAR stream2FE8:02A0,
active WORD1, X097D; Y8030/8070 respectively. Selector bytes397A/397B are4/0;
legacy SpawnedCombatant bytes392E/392F are also4/0 at relative indices6/7,
not7/8. That legacy table's broader meaning remains uncertain.

Clear rental-arena WORD E48E, set story BYTE D32F to1 and restore layout4.
Finally clear only destroyed target's active WORD and write WORDFFFF to its
coordinates; newly spawned14/15 survive. Loop consolidation has no calls
observing intermediate initialization state. Existing standalone UrbanMech
aliases have inconsistent numbering; this block now uses canonical ID tables.
Animation paths before this branch remain a separate review boundary.

Boundary detail: WORD decrement occurs before masking, so X0900 becomes087F,
Y8000 becomes707F. Do not substitute a preferred map-boundary policy when
transcribing these instructions; reachability/gameplay impact is not established.

## Sol: attack animation setup/playback (1AE8:12F6..1521)

Jason's signed assignment BYTE selects a local camera combatant; values>=8
fall back to on-foot ID4. No save-state assignment write occurs. Save shooter
facing BYTE45B6 when396C!=FF. Mechs additionally save their current frame in
WORD BP-0C. Choose eight-direction FAR fire/kick stream tables2D58/2D78 or
2D98/2DB8 according to family0/nonzero. These match the sprite-sheet research.

Personnel read DS3EDB's FAR table3FF8+weapon*32+direction*4, not ES3092's
scalar BLD index3FF8. Local expanded-EXE read confirms pointer entries here;
stored segment36DB is pre-relocation, corresponding to logical3EDB. Named
view covers15 personnel weapon indices times8 directions (120 FAR entries).

Missile indices0A/0B and19..1F clear the concurrent target-impact flag;
0A/0B against a mech also select cutscene1. Nonzero flag saves target facing
and installs FAR impact stream3EDB:2E3C. Its first four bytes20..23 are frames,
not four-byte pointer data; following control tail overlaps other research views.

150B tests the next BYTE at shooter FAR cursor againstFF, not the pointer.
Each iteration stores the returned AL frame from1732 for shooter and optional
target. Visible shooter causes render/box/five-retrace delay; hidden shooter
still advances both streams. No independent target-end condition is invented.
Rename the formerly misleading Bool_NonMissile parameter PlayTargetImpactStream
and frame local SavedShooterFrame. Later projectile/laser/impact paths are still
pending; Bool_OnFoot's misleading hit/applied name remains flagged there.

1AFD's mech-shooter frame restoration now names the saved WORD correctly.
The later Reko projectile block still appears to alias it with a coordinate;
that assignment is explicitly TODO pending its own ASM reconstruction.

## Sol: sounds, travel drawing and impact (1AE8:1521..1B0C)

Weapon comparisons are zero-based TABLE indices. Existing named sound/weapon
constants are preserved. Non-travel effects0 skip straight to impact handling;
missiles1 play launch sound1 before visibility testing. Personnel lasers2
use sound9; mech lasers/PPC3 use sound2 (legacy KickUnknown label retained).
Adapter0 palette fallbacks3/2 and adapter1 redraw throttling are retained as
observed legacy branches, not evidence that removed graphics pipelines remain.

Travel coordinates are signed WORD pixels: visible units use324C/327C,
hidden units use1DF8 relative to the local Jason camera anchor then multiply8.
Mech target Y is shifted up8. Beams add target inset3/3 and signed direction
muzzle offsets2D28..2D57; missiles skip these insets. Install direction FAR
stream2DD8 into effect cursor slot24 at0256. Skip travel if both absolute axis
gaps<=8. Major/minor distances and signed error accumulator implement an
octant-table Bresenham-style walk; primary/secondary BYTES41DC..41FB sign
extend, and missile steps multiply4. Original exact-equality exit is retained;
managed bounds/overflow policy and unusual endpoint reachability are not invented.

Missile frame from1732 plus68h selects sprite; viewport is X104..319/Y0..199.
Adapter1 draws only each fourth visible tick. BP-4 counter is uninitialized
in this routine (probable original defect, see BUG-015); preserve explicitly
unassigned research local rather than claiming an initial phase. Beams plot
one pixel per eligible step via a degenerate line. Neither path overwrites
SavedShooterFrame: the previous apparent coordinate/frame alias was Reko noise.

Rename final flag AttackApplied: false just redraws; true plays impact sound4.
Only weapon indices>=7 and visible targets run impact stream41D8 in slot24.
Verified bytes7C,7D,7C,FF produce sprites frame+FAh (374,375,374), not fixed112h.
Render/box per frame; wait5 retraces once after sequence; nonzero selected
cutscene then plays. Restore mech-shooter saved frame and enter death/wreck
cleanup regardless. Detailed1631:1DF8/1F73 helper bodies remain separate audits.

## Sol: combat parent entry/reset/formation (183B:000A..02DB)

Save nine local terrain BYTES and map anchor WORDs. Rules0 (random encounter)
copies prior enemy-infantry WORD positions4024/4056 into the eight roaming
NPC slotsD390/D392 with26-byte stride; these are not contiguous NPC X/Y arrays.
Nonzero rules skip this capture and retain externally supplied combat state.

Original split loops00A1..017C fill576 plan BYTES40B4..42F3 with02,288 target
BYTES3800..391F withFF,1152 order BYTES32C6..3745 withFF. Direct canonical
matrix loops cover exactly the same addresses, without calls observing write
order. Rules0 additionally clears24 death/wreck and active WORDs, cover/facing
BYTES and sets position WORDsFFFF. Nonzero rules preserve these tables here.
0181 clears overlapping eight-BYTE views3994 and3998 (union twelve BYTES),
plus eight heat and timed/inferno-state BYTES. Reko WORD aliases were wrong.

01DC uses living on-foot character records0..7, compact formation slot only
advancing for included members, and combatant ID=record+4. Formation offsets
are signed BYTES3A16/3A1E; store current packed WORDs and INCREMENT active WORD.
025E similarly handles friendly mech records0..3 using compact3A26/3A2A offsets,
without applying typed-array mech stride twice. Restore the shared map anchor
after each placement. Active state increments rather than sets1 even in
prearranged mode; preserve this numerical behavior rather than normalize it.

This pass stops before02DB encounter assessment and the turn/menu loop. Their
escape, betrayal and salvage locals/expressions remain pending. Local BP-0A
is a reused WORD scratch value; its old unwieldy name is retained with a note
rather than inventing one semantic meaning across unaudited later blocks.

## Sol: encounter assessment and turn/menu dispatch (183B:02DB..0826)

Random mode generates enemies and gates assessment on active enemy IDs12..23
with both packed WORD positions!=FFFF, then helper28DB. Clean Reko intermediate
confirms these CMP imm8FF values sign-expand toFFFF despite ASM pretty-printFF.
Count enemy mech records4..7 by first Name BYTE only, but enemy character
records8..15 require Name!=FF and combatant(record+8) active/valid coordinates.
If gate fails, consent local is not consumed. Prearranged mode sets consent/gate1.
Declined consent still forces engagement if random low2 bits==0 (1/4 assuming
uniform bits); force path restores consent1. Corrected duplicate Kurita prompt
using original EXE text3452. Rules2 refreshes map; then computer-control,
verbosity and graphics dialogs precede the round loop.

WORD BP-12 is named RoundExitStatus (formerly inconsistent duplicate local).
Manual planning resets active friendly step rows and deactivates WORDFFFF
positions before opening UI14C3. Kurita boundary forces exit result2 only when
flag3772 is nonzero; preserve unsigned Y versus signed X comparisons and group
all four boundary alternatives under the flag. Interior bounds are
X>=0B7F and<0D00, Y>=B07F and<D000. Result2 skips ordinary escape random roll;
ordinary nonzero request becomes0 for low bits00,1 otherwise (3/4), not6/7.
Signed rules>=2 then prohibits escape. Kurita blocks ordinary exit but accepts
result2 when rules permit. Copy final result into round-exit status.

Continue result0 builds friendly plans only if active, action-state0, first
step sentinel2 and first order!=FF. Other action-state invokes1631:03AB.
Mech movement mode sign-extends; personnel uses2474. Full-AI mode bypasses
the manual UI:1482(0), and3992/nonexistent-friendly-mechs can set raw random
exit0..3 in both WORD locals. Do not normalize this AI-specific result as bool.
This pass stops at0826 before enemy planning, mechanics and betrayal/end checks.
UI14C3 and helper28DB bodies remain independently unaudited even though their
parent-side contracts are now described.

## Sol: mechanics, betrayal and round-end checks (183B:0826..1037;1482)

Clear374C, run1482(12) enemy AI, restore round anchor and execute mechanics with
signed rules>0 converted to WORD bool. Wrapper1482 applies03AB to twelve active
WORD entries (start0 friendly/start12 enemy), not a mech-only loop.

Betrayal requires main-characters-alive WORD014A, traitor BYTE D333 and warning
WORD374A. Read signed traitor recordD331 and assignmentC620, not pointers.
On-foot assignment8: Rex interrogation/corpse, clear on-foot WORD coordinates.
Assignment's mech PilotId!=traitor: remove rider, corpse at mech anchor, keep pilot.
Traitor pilots: unsigned RiderId BYTE is WORD00FF sentinel (dis confirms),
notFFFF. Skilled Infantry[rider].Skill_Piloting!=0 promotes rider to PilotId,
clears RiderId, keeps mech; absent/unskilled rider destroys first mech Name BYTE.
Unskilled rider is placed on foot at mech anchor before mech removal. Betrayal
wreck uses family0?80:81 at exact anchor, unlike normal wreck's -1/-1 offset.
Finish all paths with traitor NameFF, on-foot active WORD0, D333=0,D330=7F.
Original narrative fragments were verified locally against EXE strings and
retained inline; no external assets or ignored binaries were staged.

D32F plus active mech0 and signed X strictly between0900/0A07 latches local
escape flag BP-0C. Clear invalid enemy WORDFFFF positions from active state;
any active12..23 keeps round open. Random-mode flight handling only runs with
374C nonzero. If any enemy mech12..15 remains, clear374C and individually flee
active personnel16..23 when signed distance metric>25, restoring shared map
position afterward. Target byte CBW is unmasked/unvalidated (BUG-016 probable).
No mech active leaves global374C flight coin flip bit0, ending round if set.
Special rules1/2 gate loss on mech0 active and D32F/latch; main-character death
or latched escape also ends round.

Kurita WORD BP-8 is a persistent message index initialized0 by initial flag
setup. Ongoing round consumes1 message; final round consumes5-index messages,
testing old count before decrement. Helmet scene precedes message1 but does
not advance index. Display FAR table3A2E[index], then increment; index3 now means
message2 just announced ruined Citadel, so load map0B into grid slot4. No extra
index increment skips message3. Table five zero-based strings confirmed locally.
1009 cancels computer control on pending input only when DisableInput WORD0;
102E loops while round-exit WORD0. Post-combat salvage/mode outcomes1037 onward
remain the next parent boundary.

## Sol: post-combat salvage and mode outcomes (183B:1037..1481)

Completed the parent tail against the original expanded ASM. BP-0A has no
damage-sprite/movement-budget assignments in this parent: renamed it
`EncounterAssessmentResult`, retaining helper28DB's unresolved policy.
Zero calls2AA3 with the original outer WORD packed map anchor, then common
cleanup. Corrected2AA3's two argument widths only; its body remains pending.

1060 salvage branch applies to signed rules<1 or rules3, and requires consent
BP-6 nonzero, exit status BP-38 zero and main characters alive. Recovered
the missing consent test. Any casualty among enemy combatants12..23 prompts
the Kuritan victory text. Armour dialog0DAB:0002 needs a friendly mech with
first name BYTE notFF, an enemy mech wreck12..15 and a nonzero Tech skill.
It runs once. Corrected double-applied mech stride and malformed skill views.
The technician's name check is an original probable bug (BUG-017): CBW turns
FF intoFFFF but CMP uses00FF, so a dead record with retained Tech can qualify.
Expanded EXE bytes at183B:115B are98-3D-FF-00, confirming the distinction.

Mech dialog0DAB:04F9 independently requires a friendly393C/3954 flag in
slots0..3, living unassigned/on-foot skilled pilot, free friendly mech slot,
and no bit80 in any of nine saved terrain BYTES. Assignment comparison is
signed BYTE>=8. The legacy3954 label is not promoted to a proven semantic
name: these four WORDs are used for friendly mechs here. There is no separate
enemy-wreck gate on this branch. Pure eligibility scans are presented together
for readability; dialogs and all writes retain original sequencing.

1251 always calls infantry loot on this victory path, clears D335 BYTE recovery
timer, and sums10*Body-Health for living characters. Both inputs are signed
BYTES, total wraps as WORD, and only a nonzero total calls healing with WORD0
(MedEquip_None). This is an integer sum, not a boolean or pointer expression.
Survivors return to the OUTER pre-combat anchor through17BB/2835 and redraw;
escape/non-consent instead displays elusion text before the same return.

136D mode outcomes skip messages if main characters died. Kurita flag takes
precedence and only exit status2 displays the scripted escape message. Rules1
requests return to base. Other non-salvage modes use arena outcome unless the
escape latch is set: D32D is1 if mech slot0's first BYTE is notFF, otherwise0.
Arena strings were corrected from human transcription against expanded EXE.

141A cleanup is unconditional across all exits. Eight roaming NPC records
D390/D392 have stride1A and WORD coordinates; restore canonical combatant
position slots16..23 (4024/4056 + slot*2), clear animation selectors16..23
and0..7 toFF, and set NPC sprite frames16..23 to10. It does NOT clear all
friendly selectors0..11. Moved the misplaced common label to the parent tail,
replacing the undefined141A goto and wrong BYTE/contiguous-array writes.
No original game bugs were repaired; these are research-C transcription and
annotation changes, not a runnable port.

## Sol: friendly turn/planning menu (183B:14C3..1773)

Reconstructed the complete menu against expanded ASM, preserving helper
ordering. Six stack locals are WORDs: BP-2 flee return, BP-4 initial-default
latch, BP-6 finish-planning latch, BP-8 clear-row counter, BP-0A selected
combatant ID, BP-0C selected menu choice. Reko's alleged text pointer merged
unrelated register addresses with BP-6; removed those assignments and the
bogus selected-ID assignments that made planning terminate after one choice.

Initial scan uses WORD active flags406A for friendly IDs0..11, not a BYTE
visible/on-map alias. DisableComputer0090 clears only the initially selected
action BYTE3994. Each iteration displays unit description; nonzero action
state previews AI1631:03AB(id,1), otherwise calls1774(id,1). These helper
bodies retain their own audit boundaries; no round execution happens here.

Original EXE menus use carriage-return separators, not literal dots. Verified
and restored text at3EDB:3A42/3A6F/3ABC. Menu section count00C6 is10 for
mechs0..3 and8 for personnel4..11;00C8 is the existing Menu_DefaultOptions
view (replaced undefined w00C8 alias). Initial default is last choice Begin
Fight. Menu AX is WORD, and the last-two comparison is signed.

| Action | Mech index | Personnel index |
| --- | --- | --- |
| Walk / Run / Jump | 0 / 1 / 2 | Move:0 |
| Clear Moves | — | 1 |
| Use Weapon(s) | 3 | 2 |
| Kick | 4 | — |
| Computer | 5 | 3 |
| Scan Unit | 6 | 4 |
| Next Unit | 7 | 5 |
| Flee | 8 | 6 |
| Begin Fight | 9 | 7 |

Clear Moves writesFF to all48 BYTES of the personnel movement-order row,
clears action state and calls1774(id,0). Computer is displayed even when
disabled, but its explicit choice does nothing then. Movement/weapon/scan
actions do not finish planning. Next Unit cycles active friendly IDs, wraps
at12, then keeps Next Unit highlighted with00C8=7/5 for the new type.
Only Flee/Begin Fight increments BP-6; return AX is1 for Flee,0 for Begin
Fight, consumed by parent0605. Preserve signed movement-choice comparison
as well: no new validation of unexpected menu return values is inferred.

Original precondition remains visible: initial scan can reach ID12 when no
friendlies are active, and Next Unit has no empty-set termination guard.
Do not classify a confirmed gameplay bug without tracing caller reachability.
Changes are Sol-tagged transcription corrections and comments, not a C# pass.

## Sol: selection marker and friendly movement preview (183B:1774..193A)

Audited the complete helper against expanded ASM, with bounded cross-check of
207F:2B87 and1F3D:06C3. Legacy method name retained; both arguments are WORDs.
Second argument requests viewport recenter/redraw, not movement mode. Initial
BP-6/-0A hold distinct canonical packed position WORDs4004/4036; corrected
the Y,Y viewport call to X,Y. Flag0 skips viewport copy/multiselect redraw but
still performs background preparation, marker drawing and friendly preview.

17D4 reuses BP-6 as marker size and BP-2/-4 as signed offsets. Friendly
mechs0..3 and enemy mechs12..15 use size3, offsets(-1,-2); infantry uses
size1, offsets(0,0). Restored BP-8 row increment and207F:2B87 argument order
(x,y,width,color): mech rows draw at(25,10),(25,11),(25,12), width3;
infantry draws at(26,12), width1. Each helper call spans8 pixel scanlines.
Removed invented15/16 assignments that had destroyed the loop. Literal
26/12 denotes the preview character-cell anchor, not packed map coordinates.

Added missing B782 WORD scratch-pad view, retaining provisional name.
1774 sets it1 after1F3D:06C3. Only adapter3 branch207F:2C1E reads it and
shifts the low color nibble left4; adapter2 EGA path ignores it. EGA drawing
2CB0..2CDB configures write-mode2/XOR and performs latch read/write across
eight scanlines. No unsupported global renderer rewrite or flag meaning
was inferred from that bounded helper inspection.

1844 skips the entire path/budget/glyph branch for enemies12..23. Friendlies
call mech budget22BC with signed BYTE order mode CBW->WORD, or personnel
budget2474, then193B path builder. Those functions have separate audit status.
Preserved existing glyph reconstruction: FAR SS:BP-0E two-BYTE NUL-terminated
buffer, signed dx/dy pairs40B4+id*24, 2 stop sentinel, FF=-1. Cursor starts
(26,12), advances before drawing glyph3B06[(dx+1)+4*(dy+1)] with colors15/0.
Final WORD cursor3778/377A is written only for friendlies, even for empty path.

Made the original guard order explicit:1903 reads the step sentinel before
189A tests byte count24. A full row reads the next row's first BYTE but draws
at most12 pairs. This is bounded DOS-memory lookahead, not a claim of a
confirmed player-visible bug; a future managed port should guard its array
before reading without inventing an additional drawn step. Signed combatant
comparisons preserved; legal IDs remain caller responsibility.

## Sol: movement-plan destination entry (183B:1C1F..2230)

Reconstructed the full UI against expanded ASM; WORD arguments and locals,
BYTE order writes. Friendly manual takeover clears48 order BYTES if action
state3994 is nonzero, calls1774(id,1) BEFORE clearing action state, then clears
it. Do not move that preview call after the state write.

Existing mode is signed BYTE Orders[0], FF=-1. Mech mode changes prompt only
with existing orders, different requested mode, and available jets if requesting
Jump. BP-0C starts1, decrements before prompt, increments only on Yes. Declining
returns; accepting deletes all orders, previews and waits for a key. Corrected
invented12/13 pointer-derived values. Mode strings at3CD4 form three FAR
pointers (stride4), not a single WORD; corrected the header scratch-pad view.
Run+n+ing formatting is retained.

DestinationSlot BP-0A finds the first FF mode BYTE at offsets0,4,...,44,
or48 when full. Budget helper and193B path builder precede display. BP-16/-1A
receive3778/377A but are never read; omitted dead stores with an explicit note.
Zero budget message precedence: no jets, heat exactly MECH_Heat_Max30,
then leg/heat inhibition, otherwise cannot move farther and suggest Run for
non-running mechs. Friendly infantry clears stale mech penalties first.
Ordinary status paths clear penalties; no-jets/shutdown bypass that cleanup.
Retained meaningful mode/heat/combatant constants and explained numeric bounds.

Cursor entry uses WORD FFxx move commands. Replaced broken doubled C case
values with eight named compass commands from eleven-slot jump table2049.
Signed WORD deltas preserve left/up=-1. Inclusive panel columns13..39 and
rows0..24 reject out-of-bounds coordinate changes independently. The requested
delta still triggers preview processing even if a boundary rejected movement.
Space20/Enter0D finish entry, with no Escape cancellation or rollback path.

Every requested movement temporarily offsets packed map position by cursor
minus anchor26/12. Order layout is mode, combined region BYTE, localX&7F,
localY&7F. Region combines highY nibble and lowX nibble of packed position.
Scan earlier destinations before DestinationSlot: matching destination clears
the provisional slot toFF, NOT the matched earlier waypoint. Original CBW
comparisons are retained; region>=80 sign-extends and cannot equal the newly
assembled unsigned region WORD. High-region duplicate filtering deserves a
later compatibility/reachability check; no invented unsigned comparison fix.

DestinationSlot never advances within this interaction. Each cursor move
overwrites one provisional waypoint, not append-per-key. Restore original map
WORDS, save cursor WORDs, call1774(id,0), restore user's cursor instead of its
reachable endpoint, then redraw single-cell color14 marker. Removed malformed
aliases, unknown locals and pointer expressions. Palette14 is yellow cursor;
3778/377A remain WORD character-cell coordinates.

The full-row scan can yield48 and217B writes four BYTES without a capacity
check (BUG-018 probable). Original behavior remains in research C; no bounds
fix or C# rewrite included. Expanded EXE status text at3BAD/3BCA/3C1A/3C93
was locally cross-checked. Synthetic tests do not execute this pseudo-C.

## Sol: kick selection and personnel movement budget (183B:2231;2474)

2231..22BB takes a WORD friendly mech ID0..3. Reads actuator BYTES at
C748/C749 + id*7D (+24/+25), and requires bit08 intact on BOTH legs.
Removed erroneous address-of operators; other actuator bits are not gates.
Calls1543:07CB(id,20h,0Bh): zero-based kick weapon index Mech_Kick-1,
reserved last target slot11 of12. This selects/clears a kick order, not damage.
Damaged-hip branch displays a message and reads a key. Both outcomes restore
00C8 WORD default choice4, keeping Kick highlighted. Existing target picker
audit confirms slot handling; no new eligibility rules added.

2474..24EF takes WORD combatant ID, not record ID. Default WORD3770=6 is
used unchanged for enemy infantry16..23, with no enemy stats/armour lookup.
Friendly combatants4..11 map to records0..7 by subtract4. Original signed
BYTE IMUL3*Dexterity(C616) produces WORD AX, then two SARs give floor(3*Dex/4).
Removed BYTE product/result truncation. Minimum3 is applied before armour.
Signed ArmourType BYTE C621>ArmourType_FlakVest1 halves points by SAR:
Flak Suit, environmental suits and ablative armour are affected; None/Vest
are not. Final maximum8 is applied AFTER armour, so minimum3 may become1
and an uncapped base12 becomes6 in heavy armour, not4. This ordering is
original behavior, not a fault to silently repair. High-bit BYTE values retain
signed interpretation for research fidelity, though legal stats are positive.

Neither helper validates every input ID; menu/caller ranges are the current
precondition. Personnel helper does not clear mech heat/leg penalty globals;
the zero-budget UI has its own stale-penalty cleanup. Changes restricted to
these two helpers; previously reviewed22BC mech budget remains untouched.

## Sol: combat settings and scan-side menu (183B:24F0..273C)

24F0 message menu and2556 graphics prompt return AX WORDs. Corrected BYTE
signatures/selected-local truncation. Parent0543/0554 calls these helpers and
stores results to2E38/2E3A; helpers do not persist settings themselves. Message
menu uses00C2=2,00C6=3,00C8=current verbosity; None/Brief/Verbose choices
0/1/2. It resets00C2 to0 before returning. Graphics prompt passes the current
WORD graphics setting as Yes/No default and returns raw AX without invented
normalization. Restored EXE carriage-return message separators at3D3A.

2591 scan menu takes WORD friendly combatant ID, not pointer. EXE3D73 starts
06/0F color controls, then CR-separated Friends/Enemies/Cancel. Layout00C2=1,
00C6=3,00C8=0. All six local fields are WORDs, with signed ScanOption BP-8.
BP-4 initially1 means include mechs AND detailed scanning; browser0DAB:1467
starts at side+4 when flag0, skipping mechs. BP-2 initially1 is a browser
availability/permission flag, not an enemy-human population counter.

Only Enemies1 imposes restrictions. Friendly infantry4..11 scans canonical
enemy mech active WORDs12..15 (4082 alias); warns about human-only descriptions
if any mech is active, then clears BP-2/BP-4. A friendly mech with SensorHits
exactly2 (C79B + record*7D, offset77) warns and clears BP-4 BEFORE key wait;
BP-2 remains1. Corrected missing Sensor field view to existing SensorHits.
No >=2 inference or additional range/visibility test added.

All choices then scan enemy personnel IDs16..23: active WORD406A nonzero and
packed X WORD4004 notFFFF sets BP-2=1. Reko .dis269D confirms sign-extended
imm8 FF meansFFFF. The fresh0DAB audit likewise confirms browser1467's native
coordinate checks useFFFF; its earlier00FF explanation was a listing misread.
Only X is tested here. Add BP-4 to BP-2; sum0..2 is tested for zero, not count.
No-humans message appears only for Enemies and zero sum (ordinary infantry
without eligible human targets). Sensor-destroyed mech retains permission1
and may enter human-only browser even if no humans; browser filtering remains
its own behavior, not silently repaired in this parent.

Negative menu result falls back to Friends0; choice2 Cancel skips browsing.
Nonzero permission and choice<2 calls1467 with three WORDs(selectedID,
side0/12,include/detail). Friends browsing is not sensor-restricted. Common
cleanup resets00C2=0 and restores Scan Unit default00C8=6 for mechs,4 for
personnel. Replaced malformed loop syntax, unused aliases and32-bit locals.
Cross-checked EXE text3D3A/3D5E/3D73/3D94/3DCD locally; no external assets
included. No original gameplay fixes or unverified scan rules introduced.

## Sol: cache loading, encounter probe and viewport restore (183B:2835..2ADB)

2835 computes ((packedX|packedY)>>8)-11h, then visits three rows/columns.
Fixed missing X shift/subtraction precedence. Index is a WORD signed bound
check0..255 in the16-column region map-number table2FE8:0030, BYTE0 absent.
Slot=row*3+column loads only if descriptor[0]!=90. LES reads16:16 FAR pointer
from3EDB:0170+slot*4; removed near-pointer cast.90 is first generated block
descriptor, not proof of map identity. See earlier0800 descriptor audit.
Skipped invalid/absent slots are not cleared here; coarse row advances10h,
without a new explicit column-wrap guard. Map-number BYTE is CBW-sign-extended
for DOS_Load_Map_Files's WORD argument. Always finishes207F:1DA8 combined-map
rebuild. No new interpretation of missing-map or populated-slot behavior.

28DB now named Combat_Assess_FirstEnemy_Reachability_28DB; updated its sole
parent caller. All locals/result are WORDs. Saves outer map anchor, selects
Jason's on-foot combatant4 at400C/403E if assignment BYTE is exactly8,
otherwise uses CBW-signed assigned mech ID to canonical position WORD tables.
No sentinel/high-bit normalization was added. Centres viewport, calculates
pixel/grid origin, refreshes cache origin and seeds working compassE486/E488.

Scans12..23, latching ONLY the first active enemy's X/Y, not nearest or every
enemy and not a visibility/position sentinel filter. Restored omitted X store.
3770=30 is an iteration limit for this path probe, unrelated to actual unit
allowance or heat despite equal numerical value. With an enemy found, reset
BP-2 and iterate while signed budget>0; exit early if already at target.
Position0006 receives WORD actor80h, targetX/Y, screen anchor26/12 and flag0.
80h was incorrectly aliased into targetY by Reko. Working screen coordinates
advance by chosen-step WORDs458E/4590, then decrement probe budget. Only
post-step arrival sets result1 and budget0, including arrival on step30.
Already-at-target returns0; no-active-enemy also returns0 but leaves budget30.
Preserve these original edge semantics, not hypothetical clean boolean logic.

Position0006's80h special policy deserves its existing deeper path/collision
audit; this bounded review confirms the call contract, not exact full gameplay
reachability. No blocked-path early exit is invented: a zero step may exhaust
the30-iteration budget. Probe always calls2AA3(saved map WORDs) before returning
AX result. Compass/remaining budget scratch is NOT restored. Source comments
and synthetic checks distinguish probe success from ordinary movement queries.

2AA3 restores only viewport/cache through0800:17BB,207F:1314,207F:1DF8.
No actor-position table writes, framebuffer copy or compass/budget restoration.
The ASM export ends after the1314 call, so verified final ADD SP4/FAR1DF8/
POP BP/RETF via .dis and expanded EXE. Raw FAR selector187F relocates to207F.
This completes the final183B helpers, not a claim that every deeper dependency
or all original gameplay bugs are resolved.

## Sol: AI actor setup (1631:03AB..047D)

Verified against primary ASM: save global WORD anchor A44B/A44D; focus actor
using WORD X4004/Y4036 with identical indices; clear all48 order bytes at
32C6+actor*30h before normalizing enemies by12. Friendly actors set action
BYTE3994+actor=1; enemy bank offset is WORD4. Restored the missing heat-array
view in the mode test: only side-local mech IDs0..3 at exactly zero heat
retain mode1(Run); all others select0(Walk). Infantry skip the heat lookup.
Mech preparation receives record0..7; infantry receives record4..15.

Corrected transcription errors only; no gameplay repair. BP-1A is now named
SelectedMovementMode with Run/Walk constants; other legacy shared locals remain
for later blocks. Nearest-target selection is reviewed in the next checkpoint;
later weapon planning remains unreviewed C. Synthetic checks
cover all24 actor mappings, original row indices and zero/nonzero heat modes.

## Sol: nearest opposing target (1631:047E..05AA)

Restored WORD candidate count/IDs/packed coordinates, canonical active406A
and X4004/Y4036 views, signed nearest-distance comparison and post-decrement
loop. Four opposing mechs are preferred regardless of infantry proximity;
only if none qualifies does the scan proceed to eight infantry. Infantry
actors begin directly with opposing infantry. Rental combatant13 is excluded;
enemy actors also exclude D331's signed BYTE+4 when D333 is nonzero. Equal
distances retain the earliest candidate; accepted candidates call0BB5 twice.

Confirmed0579's sentinel as FFFF from primary `.dis`, not literal00FF from
the ASM pretty-print. Failed infantry scans also refill the count: there is
no complete no-target exit or table-bound guard. This is an observed original
control-flow risk, not yet a proven reachable gameplay bug. Preserve it in
research; validate legal encounter preconditions before managed-port policy.
05A7 restores enemy actor IDs. Downstream BYTE target alias is now in valid
scope; the following checkpoint now covers05AB range selection. Movement
planning from0688 is now covered by subsequent completion checkpoints.

## Sol: AI movement range selection (1631:05AB..0687)

Reconstructed both branches from ASM. Infantry load Weapon BYTE+0B with
CBW, index17-byte records, and decode top3 range bits with SHR5. Rebasing
C5DB+friendlyID*11h / C597+enemyID*11h gives infantry records ID-4 / ID-8.
Mechs normalize record IDs0..7, scan every critical BYTE offset33..55,
exclude zero/high80/outside10..20, and select minimum E0 range bits through
RAW component-code table index. FF sentinel becomes0; otherwise SAR3 yields
four times encoded short range. Signed comparisons operate on zero-extended
nonnegative bytes. No inferred component-minus-one correction was made.

Removed double-scaled struct-array indices, bogus address-of weapon loads,
nonexistent CriticalSlot array access and undefined minimum comparison local.
Named WORD distance threshold WeaponApproachDistance_1106 and renamed
related locals retaining numeric suffixes. Later uses were mechanically
renamed only at this checkpoint;0688 flow is now covered below. A-004 remains about
intended AI raw-code range/scaling policy, not unresolved instruction decoding.
Synthetic checks cover all256 packed range bytes and all16 infantry address
identities; these are transcription checks, not DOS gameplay validation.

## Sol: AI path clearance and infantry withdrawal (1631:0688..0796)

Four WORD1BFE arguments confirmed; AX saved as TargetPathClear_1131, with
original suffix retained. Reconstructed signed ID classification and unsigned
packed-coordinate comparisons. Infantry actors against mech targets set
3992 according to actor side; enemy infantry additionally set374C WORD1.
Header374C was incorrectly BYTE despite the explicit original WORD store.
Do not interpret3992's legacy name as proof of a distance/range check here.

Destination moves six steps away from the mech on each unequal coordinate,
then applies original packed-boundary arithmetic after16-bit wrapping.
Zeroing path-clear result and weapon-approach distance forces direct-order
generation at0797. This is withdrawal planning, not actual actor movement.
No original behavior was repaired. Next bounded review0797 will reunify the
split BP-A distance local and reconstruct approach-loop tests/order stores.
Synthetic checks cover all24x24 actor/target classifications and six packed
boundary/equal-axis examples. Pending requested distance-variable renames
and their notes are included with this checkpoint.

## Sol: direct and approach destination orders (1631:0797..08F8)

Reconstructed four-BYTE first order at32C6+actor*30h: mode, combined region
byte, localX, localY. Zero weapon approach distance or blocked path chooses
direct target/withdrawal destination; otherwise signed actual-distance<=range
leaves the cleared row untouched. Corrected Reko's target-ID comparison.
Larger distances step fixed signed WORD X/Y deltas together while the OLD
distance exceeds threshold. BP-A decrements even on the final false test;
1124's split alias is merged into the original1070 local with both names
documented. No invented duplicate distance state.

Preserved WORD wrapping, packed-boundary corrections, and in-place local
masking after region encoding. Replaced fake pointers and invalid assignment
syntax with canonical BYTE row accesses. This constructs a destination only,
not a collision-safe path or movement execution; no extra gameplay guard was
added. Tests cover distance/threshold loop step counts, final shared distance,
and combined-region encoding. Next bounded review starts08F9.

## Sol: infantry targeting and mech close-contact override (1631:08F9..0A9F)

Infantry write selected target low BYTE to3800+actor*12; protected party
member's slot becomesFF when D333 is set and CBW(D331)+4 matches actor.
Mechs preserve exact signed ID gate, unpack live actor/target coordinates,
centre both X anchors with+1 and use signed WORD separation (not truncated
BYTE abs). Path-clear must be nonzero and |dx|<=3 / |dy|<2. Then animation
selector396C and first movement-order mode32C6 becomeFF; facing3920 turns
toward target using original packed coordinates. Destination bytes remain
untouched. This is a close-contact movement/facing override, not kick damage.

Canonical arrays replace stale aliases/undefined actor names. Descriptive
local names retain original numeric suffixes. No gameplay repair or extra
actuator/occupancy guard. Next boundary0AA0: mech weapon targets and heat.
Synthetic checks cover contact bounds/path gate and signed protected ID mapping.

## Sol: mech weapon target row and heat shutdown (1631:0AA0..0B7A)

Clear12 target slots, normalize enemy combatant12..15 to record4..7, load
effective signed heat006E+record (original enemy0066+combatant). Below signed
30, call10A2 for exactly ten ordinals0..9 and assign selected target whenever
return is not WORD00FF. Corrected Reko's loop bound16. Slot10 remainsFF;
slot11 is the unconditionally assigned reserved kick target. No range/actuator
eligibility is inferred here. At signed heat>=30 cancel first movement mode
and retain original all12-target OR80 loop, redundant after FF initialization.

Canonical arrays replace stale aliases and the incorrectly scaled kick slot.
Named WORD locals retain numeric suffixes. Effective heat load omits only
the overwritten preliminary enemy read, not any gameplay state change.
Synthetic checks cover all256 heat encodings, eight mech-address identities
and twelve target slots. Next boundary0B7B: calculation/preview/anchor restore.

## Sol: remaining1631 local workflow completion

Completed AI epilogue0B7B..0BB4, heat/casualty0C63..0F23, effects sprite
wrapper and small-helper WORD/signedness consistency pass. Detailed original
addresses, renamed-local mapping, formulas, call contracts and exact changes
are in the [1631 completion checkpoint](BTECH_1631_SEGMENT_REVIEW.md).
Inferno stateD576 is eight BYTES; sprite pointer39FA is one16:16 table view,
not separate offset/segment arrays. Retained BUG-013's enemy upper-clamp
omission, BYTE heat wrapping, Jason corpse-coordinate exception and deleted
alternate adapters. No extra gameplay guard or original bug fix was added.

All24 local1631 entries are reviewed; pseudo-C is still non-compilable because
DOS header views overlap and shared207F renderer/formatter bodies/signatures
need their own pass. Pointer-table extent is not yet proven;256-entry view
represents BYTE sprite-ID domain only. Next work is these shared dependencies
and combat-wide contracts/specification, not more unreviewed1631 branches.

## Verification and next review boundaries

`scripts/Verify-CombatTranscriptions.ps1`:3034 synthetic assertions cover budgets,
signed/stop pairs, alias/address identities, order shifting, duplicate/destroyed
weapon ordinals, all35 critical offsets, accuracy boundaries, personnel damage/repeats
hit-location address rebasing/kick damage, casualty target cleanup, wreck coordinates
reinforcement address identities, animation FAR-table strides/local anchors,
synthetic travel endpoints, viewport bounds, frame-to-sprite bases and parent
clear coverage/formation address identities, escape odds, Kurita boundary grouping,
message sequencing, betrayal rider outcomes, signed flight-target conversion,
post-combat gates, NPC cleanup aliases, signed health/name conversions,
planning-menu action/exit mappings, Next Unit cycling, movement-row clearing,
selection-marker footprints, glyph lookup indices, full-row preview lookahead,
destination slot scanning/encoding, signed cursor bounds, mode-change latch,
both-hip kick eligibility, signed personnel min/armour/max budget ordering,
scan permission/detail dispatch, active aliases, WORD sentinel boundaries,
coarse cache indexing, first-enemy bounded probe/arrival side effects,
direction-bank candidate sequences, octant turning, probe-footprint policy,
occupancy footprint thresholds, crushing side/record mapping, packed X correction,
turning-path parity transitions, rental bypass, starting-tile threshold,
critical-count rolls, section-table rebasing and actuator clear masks.
These test transcriptions, NOT
execution of the annotated C. InceptionTools regression suite:182 assertions.
No DOS runtime behavior is claimed from these tests.

## Sol: final183B consistency checkpoint

Reviewed all17 source routine entries against the accumulated ASM checkpoints,
cross-checked settings/probe WORD returns and their parent callers, canonical
active/packed-position/facing views and named menu choices. Removed stale
1774 pending-body comment; restored MovementMode_Jump/Run and MECH_Heat_Max
names in22BC without numerical or ordering changes. No original BUG-012..018
behavior was silently fixed. This is consistency review, not another complete
instruction-by-instruction replay or proof of runnable C: header views overlap
and dependencies still contain damaged decompiler bodies.

Position0006 and16AB occupancy with both callers are reconstructed in the
[1631 checkpoint](BTECH_1631_SEGMENT_REVIEW.md).
## Sol: turning cached-tile path clearance (1631:1BFE..1DAA)

Completed body against ASM; four WORDs(actor,target,targetX,targetY), actor
unused, return AX1 clear/0 blocked. AI caller1631:0688 needed missing targetY
restored; execution caller1AE8:01B5 already has all four. Uses global anchor
as origin, rebuilds pixel/cache origin even on rental shortcut. Arena rental
WORD E48E nonzero and SECOND argument target13 returns1 without tile tests.

Seed BYTE-cache index09ED+96h (150=6*24+6), increment if originX odd.
X parity starts1 for even originX,0 for odd; Y parity=originY&1. Initial tile
must be below blocking threshold using signed WORD comparison. Same-position
target returns1 only if that initial tile passes (unlike28DB post-step rule).
FFFF heading sentinel is never used as a table index in this case.

Path coordinates and heading are WORDs; desired octant changes heading by
at most1, modulo8, then step328A/329A and packed-boundary32AA/32BA updates.
New header views are eight WORDs each; raw EXE confirms X/Y deltas matching
0006, corrections +/-80h and +/-0F80h, and row delta32CA +/-24/0. Numeric
cache index advances by stepX when updated X parity becomes0, by32CA when
updated Y parity becomes0. Removed invented pointer arithmetic/division.
Test each resulting tile; blocked sets local coordinates to target and clear
flag0 to terminate, with NO global actor/anchor movement. Path loop is not
Bresenham and does not test occupants, select alternate directions or consume
movement budget. No iteration/cache bounds guard exists in original ASM.
Future managed port should validate/cache-bound its traversal explicitly.

No new original-game bug claim or gameplay repair. Detailed caller-dependent
range policy remains in A-004 rather than requiring expensive confirmation of
these resolved arithmetic/call contracts. Critical dispatch1631:11AB has now
been reconstructed in the1631 checkpoint, preserving offset-based anatomy and
separate dice/remaining-hit state. Broader AI03AB is now reconstructed across
the later checkpoints; all remaining local1631 workflows have been reviewed.
A-004 now concerns AI range policy rather than ordinary ammo ordinal mapping;
A-005 concerns anatomical labels/C8 exception rather than numerical grouping.
