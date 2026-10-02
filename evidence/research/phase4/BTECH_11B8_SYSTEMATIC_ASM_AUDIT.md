# BTECH_11B8 systematic C-to-ASM audit

Sol: Checked 2026-09-17, baseline `2dc8bd9`. **All ten definitions checked;
ten have discrepancies, none receives a whole-method match certificate.**
Much of the game logic matches; a pointer-binding defect alone prevents a
whole-method match. Only comments and human descriptions changed. Corrections
are queued, not silently implemented. No gameplay validation is claimed.

Sol: Preservation update2026-09-17: [repair0002–0809 cleanup](../preservation/11B8_MECHLUB_REPAIRS.md)
incorporates its FAR-record and WORD-promotion corrections. Rechecked locally
under valid/stable selection and native memory/shared-call contracts; repair now
has a local match record. The subsequent [ammo cleanup](../preservation/11B8_MECHLUB_AMMUNITION.md)
rechecks1762 through its complete DIS return after FAR/signedness/text corrections;
two methods now locally match and eight retain their mismatches.
The original ten-method table below records the earlier checking checkpoint,
not the current correction status. C17 adapter tests are not gameplay validation.

Private evidence:

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, SHA256
  `F7F84B8A74996FEF5D51835B80CFD6A6F2827339476610B31AFFB8928B657DD0`.
- Supplementary `BTECH_11B8.dis`, SHA256
  `F23483F95783D122C028DA54025A93F1B1E2C3D07C500F9521E3500E2916E035`.
  Includes the ammo routine's truncated register/frame/FAR-return tail.
- Segment-selector and text evidence: `BTECH_3EDB.asm`, SHA256
  `BF0095488D585D4CD18EFF48B52FEC7FDB9C8E115B037D2325A2D2BA200EFFDD`.

## Whole-method coverage

| Method | Result | Complete scope (segment11B8) |
| --- | --- | --- |
| `Mechlube_Repair_Mech` | Checked - mismatch | 0002..0809; selection, eleven/eight deficit totals,35 critical entries, two actuator flags, no-repair/internal-damage path, zero funds, armour/structure/sink/weapon/actuator prompts and purchases, shared messages and all returns |
| `Mechlube_Modify_Mech` | Checked - mismatch | 080A..0924; selected mech/level gate, workflow flag and money display, base/stage package selection, unsupported selector8, stored price and return |
| `Mechlube_Upgrade_Mech` | Checked - mismatch | 0925..0D57; quote/prompt, signed WORD cost with unsigned DWORD affordability, insufficient/charge paths, all eight mutations, selector rejection, completion/key and return |
| `Recruit_Crescent_Hawk_Agent` | Checked - mismatch | 0D58..104D; first vacant party slot, PRNG reseed/name cycling, stats/seven skills/specialty/health, rider assignment, full description/input/traitor paths and saved-building coordinates |
| `Recruit_Rex_And_Start_KuritaParty_Ambush` | Checked - mismatch | 104E..137E; Rex/Commando installation, NPC staging, encounter resets, Jason/Rex deployment, three origin/count rolls, six-skill enemy generation and FFFF combat call |
| `Arena_Select_And_Apply_Combat_Map_Patch_137F` | Checked - mismatch | 137F..1440; RNG selector0..3, remembered WORD, no-patch case and every endpoint/interior write |
| `Arena_Remove_And_Randomize_Combat_Map_Patch_1441` | Checked - mismatch | 1441..152E; selector load, no-patch case, three endpoint patterns, all random interior writes and return |
| `Run_Jailbreak_Mission_And_Award_Stinger_152F` | Checked - mismatch | 152F..16B1; hide/save names, jail position and mission9, escape position, restore/clear names, survival gate, first-empty search and125-byte Stinger award |
| `Play_Failed_Mech_Startup_Scene_16B2` | Checked - mismatch | 16B2..1761; disk/file load,7800-byte scratch clear, frame/stream initialization, do-first decode/control gate, failure sound,60 retraces, menu/sidebar and return |
| `Mechlube_Buy_Ammo` | Checked - mismatch | 1762 through .dis return; ten-entry raw-critical collection, functional/damaged gates, per-slot display/deficit/price/request cap, round-by-round unsigned affordability/debit, messages and both final paths |

All meaningful local reads/writes, calls, conditions and loops were compared.
Stack probes, prologues, register saves, unused returned-selection locals and
FAR returns are documented abstractions. `Alloca(0E)` in the Rex transcription
represents native frame reservation, not an extra game allocation. Callees are
contracts, not certified implementations. Original BYTE/WORD/DWORD wrap and
FAR offset-only wrap are required; these scratchpad structures are address views,
not a compilable sequential memory layout or certified modern-host program.

## Shared pointer-binding discrepancy

At the original audit checkpoint, repair, package selection/purchase, recruitment, Rex ambush and ammo routines
use near `Mech*`, `Infantry*`, NPC, record-byte, skill-byte and/or WORD-table
temporaries where native reads/writes use segment3092. Native DS is3EDB; dropping
the segment is not harmless in a16-bit implementation. Jail award also casts
the2FE8 template to a near pointer. Bind FAR pointers or explicitly named flat
memory views in the later correction pass. This does not mean every statement
inside those routines is wrong.

## Repairs: matched workflow and original quirks

Armour deficits use eleven bytes+11..1B and structure deficits eight+1C..23,
each accumulated into WORDs. Native maxima are record+45 beyond the matching
current fields; C's MaxArmour/MaxStructure mapping is correct. Critical33..55
is35 bytes, masked weapon range10..20 with20 included, sink22, destroyed80.
Both20-WORD local arrays start zero. Actuator flags are side0/side1, not four
limbs. When the WORD-wrapped combined total is zero, native examines75..77
(engine/gyro/sensors), displays only the first damaged component, otherwise
the no-work message. The biased native pointer-table address resolves to the
declared three-entry table at1D12.

The order is armour, structure, heat sinks, weapons, actuators. Armour costs4
per point, structure9, exactA2 sinks800 each, weapons300 per selection and
actuators200 total. Every debit covers the DWORD balance. Partial repairs remain
when funds run out. No additional structure-prerequisite test gates sink or
weapon repair despite the dialogue wording. Actuators restore right then left
from maximum bytes, not universalFF, after the acknowledgment key.

Native weapon purchase clears EVERY matching destroyed component but decrements
its local count once (existing BUG-008). Its continuation scan excludes type
index10/SRM-6 (BUG-009). Both are preserved in current C. Jumping to0558 from
the weapon-result paths eventually reaches the actuator path; C's unusual
cross-block goto has that same effect, not an early function return.

The repair cleanup now incorporates the near selected-mech/byte binding fix. A host
port must also keep the combined total, quotes and menu decrement WORD-wrapped:
deficitFFFF plus weapon latch1 wraps0 natively, unlike promoted32-bit addition.
Over-maximum armour/structure causes a negative wrapped deficit to be decremented
towards zero while still buying increments; WORD locals and explicit narrowing preserve it,
not an approved new repair rule. Existing fixed repair text matches native bytes.

## Modification selection and all eight upgrades

080A only selects and quotes; it does not charge or apply a Locust upgrade.
The exact level3 gate clearsD31E and returns without waiting. Other levels set
D31E. Zero-extended base0..3 plus4 only for exact level1 selects packages0..7;
all unsupported bases map8. The native8 price read overlaps the following
CR CR text and returns0D0D, deliberately represented by the existing pseudo-cost
constant. This is original BUG-010, not an out-of-range read to reintroduce in C.

0925 treats price0076 as signed WORD (header already declares signed short),
then compares its sign-extended value as unsigned DWORD. Funds are charged
before validating the package. All unsupported BYTE selectors skip mutations
but still get the completion message after payment. CBW versus unsigned C
`<=7` accepts the same selector set0..7 for all256 bytes.

All mutations were compared to raw record offsets:

- Locust1: arm41/33 medium lasers, current/max ammo0/1 FF, exact level1.
- Wasp1: leg4F medium laser, ammo1 FF, eleven Locust current/max armour pairs.
- Stinger1: arm43/42/34/33 small lasers, ammo0..4 FF, exact level1.
- Commando1: torso4A/arm41 medium lasers, copy ammo2 to3 before replacing1/2,
  eleven DS1E24 armour bytes to both profiles, exact level1.
- Locust2: walk7, engine-hit byte75 clear, torso49/3B small lasers,
  ammo3/4 FF, OR level2.
- Wasp2: jump0, torso53/arm33/head55 medium lasers, ammo2..4 FF, OR level2.
- Stinger2: jump0, both armour profiles from Locust CURRENT armour,
  torso54/53 medium lasers, ammo5/6 FF, OR level2.
- Commando2: six bytes4F..54 small laser10, head55 medium laser11,
  all ten ammo slotsFF, OR level2.

Near mech pointer remains the discrepancy. The anatomical naming of critical
groups remains the existing Astra review candidate, not an unexamined mutation.
Historical prose about two small lasers cannot override the six native writes.

## Recruitment: important signed-low-WORD PRNG finding

At0E4F native Name BYTE CBW, IMUL0187, then **CWD replaces the product's DX**
with the sign of its wrapped AX. The resulting seed is signed16(Name*391)
sign-extended to32, NOT the full signed32 product in current C. Name80 yields
full product-50048, low AX3C80 (15488), native DX0000. Name54 yields full32844
but native signed AX-32692. Ordinary Name IDs0..9 agree. The previous comment
implying the full product was confirmed has been replaced with a TODO. Unknown
reachability of high name IDs is not evidence of a shipped gameplay defect.

First vacant NameFF slot is filled; full party still restores the entered
building's saved coordinates. Native name increment and signed BYTE>=10 wrap
to2 match C's explicit signed cast. Training/armour/weapon zeroing, three2D6
stats, low-BYTE body*10 health, seven binary skill rolls, forced Piloting0 then
selected skill3, and specialty4 health reduction all match. First occupied mech
with riderFF gets the recruit; PilotId is unchanged. Input wait repeatedly
advances RNG only while no input, then drains/reads. First still-eligible agent
has one coin flip to set traitor slot and three flags/probability1F.

Other discrepancies: near record/skill pointers, CBW character-name lookups,
and signed BYTE IMUL125 assignment before mech lookup. PilotId multiplication
uses unsigned BYTE MUL17, so do not blanket-sign-extend that crew ID. Normal
assignment0..3 and name0..9 follow the same dialogue path. Building coordinate
table offsets must wrap as native WORD addresses in the eventual port.

## Rex ambush and jailbreak award

Rex's record1 receives the oldD456 name without the general recruit's cycling,
Body12/Dexterity9/Charisma8, Pistol7, Health120, zero training/armour, on-foot8
and seven exact skills from1E7A. First saved-nameFF mech slot gets125 bytes of
Commando, saved initialC, live initialFF, pilot1 and sprite family92. Native
has no guard when four slots are occupied: slot4 is used. Current C preserves
the missing guard; final flat-array/address behavior still needs explicit views.

Eight NPC positions are saved with stride1A, four enemy mech/eight infantry names
cleared,24 combat positions setFFFF and active/wreck WORDs cleared. Jason/Rex
common IDs4/5 activate. Three rolls choose X0A72..73,Y805D..5E and exclusive
record end10..13, yielding2..5 soldiers. Six skills randomized, Body2D6,
health low-BYTE*10, weapon RNG%8, correct CBW signed origin-delta tables and
FFFF prearranged-combat argument. All value/RNG flow matches apart from near
temporary bindings.

Jail helper preserves seven companion and four mech name bytes, calls mission9
at0D10:7024 with Jason on foot, then restores names at0D00:7014 and clears
stored initials. A surviving party gets one125-byte Stinger in the first empty
slot, none with a full roster. It does not assign a new pilot or embark Jason.
The only found discrepancy is both destination3092/source2FE8 near byte views.

## Arena patch selector: confirmed wrong segment

Native both helpers load ES through **DS:54EA**, whose data bytes are6C24:
the selector is WORD **246C:0010**, not3092:0010. Current field access and prior
comments use the wrong segment. Both source comments now state the confirmed
address with TODOs; executable access remains untouched. Reconcile the header,
memory reference and historical arena notes during a reviewed binding fix.

Every actual tile write and counted loop matches. Selector0 means no patch.
Variant1 writes seven cells (five interiors); variant2 twelve (four+six interiors);
variant3 nine (seven sparse interiors stepping8). Native addresses minus MapTile
base101D give the current indexes. Setup has one RNG call; teardown has0/5/10/7
RNG calls for variants0/1/2/3 and writes40..43 interior tiles plus fixed endpoints.
It does not save original bytes, refresh caches, clear the selector or restore
an exact map backup. Human outlines corrected accordingly.

## Failure scene and ammunition

Failure scene disk1/O0 filename,3E80 load to246C:42C3, clear1E78 bytes at244B,
frame0 and stream246C:42F6, do-first decode, sound14,60 retraces, layout4 and
sidebar all match. Native control test is signed BYTE JL49; unsigned C exits
on80..FF where native continues. Normal shipped controls41..52 agree. This
helper restores layout/sidebar, not a full world redraw.

Ammo scanner records at most ten weapon-valued critical bytes from33..55,
retaining destroyed80. Signed raw-byte comparisons make only functional MG18
and missiles1A..20 eligible; autocannons14..17 are excluded. C's explicit
functional/range predicates agree for all possible collected weapon bytes.
Each ammo slot keeps its raw critical-list order, even damaged/non-ammo entries.
No-stock/damaged paths and final pauses match.

Remaining discrepancies besides near views:

Sol: The following ammunition discrepancies were incorporated during the
preservation cleanup. This list documents the prior findings, not outstanding
ammo-body defects; shared helper/header/gameplay contracts remain separate.

- Missing rounds is a native signed WORD in the requested-amount cap. At1906
  CWD and signed high-WORD comparison treat requested DX:AX as signed32.
  C uses unsigned deficit/request. Above-max ammo or requested80000000..FFFFFFFF
  can therefore choose different warning/cap paths; ordinary deficits0..255
  and requests0..7FFFFFFF agree.
- Missile price BYTE CBW before DWORD unsigned affordability and subtraction;
  C zero-extends it. Shipped positive prices agree. Preserve low-WORD count
  wrap, per-round ammo BYTE increments and balance display ordering: native
  decrements remaining BEFORE the display call; C does so AFTER, with equal
  observable game state under the display contract.
- Native text differs at1E99 (CR, not space),1ED1 (two spaces after sentence),
  1EFF (06 0F colour control plus CR CR),1F19 (06 0F, capital C-bills and a
  quantity question),1F49 (leading CR and two spaces),1F94 (CR CR/final period),
  2008 (three CR/final period). These are display/prompt losses, not mere prose.

`Verify-11B8SystematicAudit.ps1` supplies static signedness/address/loop witnesses
and ten-method registry coverage. Together with inventory checks it confirms
audit bookkeeping and comment-only C edits, not execution, gameplay or rendering.
