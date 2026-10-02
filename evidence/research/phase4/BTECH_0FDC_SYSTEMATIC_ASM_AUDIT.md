# BTECH_0FDC systematic C-to-ASM audit

Sol: Checked 2026-09-17 against baseline `01e7212`. **All 15 retained
definitions checked: five locally matched, ten with mismatches.** Checking
is complete; functional corrections remain pending review. Only comments and
human descriptions changed in this pass. This is not gameplay validation.

Private matching evidence (not newly distributed):

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, SHA256
  `673F7768D4E0889DCBCFC69476C5E8947DCEF16B90C588665A1F9F262599EF94`.
- Supplementary `BTECH_0FDC.dis`, SHA256
  `D3EC2C1EA61EF4D07EAFC478F42C0D1A40090C4815938625F10855A86625CE00`.
  This supplies the truncated loader epilogue through its FAR return.
- Static data `BTECH_3EDB.asm`, SHA256
  `BF0095488D585D4CD18EFF48B52FEC7FDB9C8E115B037D2325A2D2BA200EFFDD`.
  Used to resolve segment-selector cells and exact native text bytes.

## Complete comparison scope

| Method | Fresh result | Whole-body scope |
| --- | --- | --- |
| `Interact_with_BLD` | Checked - mismatch | 0008..01BF; original/resolved IDs, alternate lookup, disk/load, bypassed copyright block, entry scene, interpreter/action calls, both recursive holodisk paths, menu restore and eight fog writes |
| `Execute_Bld_Bytecode_01C0` | Checked - mismatch | 01C0..05F6; WORD cursor/exit, byte dispatch and all 28 E4..FF paths, operands, calls, state, money, conditions, branch tables and exit |
| `Read_Bld_Target_05F7` | Fully matched (native WORD/FAR contract) | 05F7..0628; both FAR byte reads, zero extension, high-byte shift, ADD versus equivalent OR, AX result |
| `Mech_Mission_0629` | Checked - mismatch | 0629..0D48; all ten setup cases, world/input loop, three exploration objectives, combat calls, pass/skill/Kurita outcomes, tile restoration, jail flag and shared-buffer reload |
| `Mission_GenerateEnemies` | Checked - mismatch | 0D49..134A; NPC position staging, all resets, signed deployment test, RNG/template selection, rental/Kurita overrides, mech copies/damage/critical effects, sprites/positions/pilots and infantry generation |
| `Restore_Mission_Map_View_After_Combat` | Fully matched (native WORD/FAR contract) | 134B..13DD; assignment8 gate, slot-zero mech versus infantry coordinates, Y+2 and bit7 correction, four redraw calls |
| `Distribute_Purchased_Armour` | Checked - mismatch | 13DE..15E5; eight initialized WORD menu IDs, live compaction, held-item loop, direct-Jason/menu/drop paths, recipient exchange, warning and displaced-item continuation |
| `Distribute_Weapon_To_Party` | Checked - mismatch | 15E6..17B8; eight initialized WORD menu IDs, live compaction, Cudgel/zero loop sentinel, direct-Jason/menu/drop paths, exchange-before-warning and displaced-item continuation |
| `Talk_To_Building_Occupants` | Checked - mismatch | 17B9..19E0; exterior lookup, eight waiting NPC flags, compact menu, Rick gate, destroyed-Citadel replies, activity/destination text, DEC/JNS selection and all returns |
| `Display_No_Stock_Transaction_Text` | Fully matched (text contract documented) | 19E1..19F5; exact DS:1791 suffix, renderer call and return |
| `Read_Bld_Immediate_Word_19F6` | Fully matched (native WORD/FAR contract) | 19F6..1A25; CBW temporaries followed by low-byte recombination, little-endian AX result |
| `Prepare_Party_Mech_For_Arena` | Checked - mismatch | 1A26..1B40; four crew saves, prompt/selection, 125-byte backup/staging, stored-name restore, overlapping assignment stores, companion hide, Jason/no-rider and rental-clear |
| `Restore_Party_After_Arena_Combat` | Checked - mismatch | 1B41..1C9A; rental discard versus owned-mech damage return, destruction/name propagation, selected-slot-zero alias, conditional backup restore, four crew restores and common dismount/name restoration |
| `Prepare_Rental_Locust_For_Arena` | Checked - mismatch | 1C9B..1D2F; 125-byte backup/Locust copy, overlapping assignment stores, companion hide and rental-set |
| `Load_And_Decode_Indexed_BLD_1D30` | Fully matched (native WORD/FAR contract) | 1D30 through .dis return; equivalent disk selection, remembered ID, FAR filename/destination, fixed 9000-byte add/XOR decode and BTSTATS-cache clear |

Ranges are segment 0FDC. Every meaningful local read/write, call, branch and
loop was compared through return. Compiler stack probes, frame allocation,
register saves and FAR-return scaffolding are abstractions. Callees are checked
as contracts here, not certified implementations. Memory scratchpad declarations
are address views, not an ordinary sequential host struct.

## Contracts and matching behaviour

Native arithmetic uses BYTE/WORD/DWORD widths. FAR address arithmetic wraps
the offset at 16 bits without carrying into the segment. The two matched WORD
readers require that memory view, including offset FFFF followed by offset0000
in the same segment; they do not certify ordinary host pointer arithmetic.

BLD's cursor is relative to the decoded payload at 3092:00A0, not an encoded
file-header offset. Both target readers return unsigned little-endian WORDs.
`19F6`'s intermediate CBW operations do not make its final value signed.

`134B` adds two to the mech Y WORD, then adds0F80 if bit7 is set, to normalize
crossing local Y7F into the next packed-map row. On-foot coordinates are copied
without that correction. The fixed stock suffix includes its leading space
and final period and matches the native bytes exactly.

## Building entry discrepancies

At002D the ID comparison is signed JGE22, not unsigned `<22`. A WORD8000
therefore attempts the alternate-table read natively but skips it in C.
At00A6 the scene BYTE undergoes CBW before the WORD argument; C zero-extends
it. Normal positive IDs agree. The alternate view is only16 declared bytes,
yet the native comparison admits10h/11h and13h..15h too (12h is special-cased).
Out-of-view reachability needs caller/data constraints, not a fabricated bound.

The unconditional jump at0056 really bypasses0058..0084; omission of those
dormant copyright-check bytes is intentional, not a failed C reconstruction.
The two garage text fragments match their native data. Fog clearing is eight
bytes D118+16*n, equivalent to rows96..103, byte-column12 of the fog view.

## Interpreter: all opcode paths

| Opcode | Native operation and comparison |
| --- | --- |
| E4 | Sound BYTE CBW then0800:19BF; missing sign extension in C |
| E5 | Signed immediate WORD CWD, DWORD add, balance display; matches under32-bit wrap |
| E6 | Two immediate WORD coordinate writes, cursor+4; matches under FAR offset wrap |
| E7 | Immediate X equality, absolute target or cursor+2; matches |
| E8 | RNG then CBW mask fetch, target or skip; C fetches first, but mask truth agrees for RNG0..255 |
| E9 | Recruit BYTE CBW then11B8:0D58; missing sign extension |
| EA | Scene BYTE CBW; only read second CBW operand when input enabled, advance it either way; C zero-extends and reads second operand unconditionally |
| EB/EC | Surgery-kit/medkit byte nonzero gates, target or skip; matches |
| ED | Signed skill index/minimum and signed skill BYTE comparison over eight live records; unsigned C and undeclared `Skill[]` do not match |
| EE/EF | Sign-extended immediate compared as unsigned DWORD, saturating subtraction or branch; current unsigned cast is correct under32-bit contract |
| F0 | Two CBW bytes to layout WORDs; explicit C signed-char casts match |
| F1 | CBW state index, low-BYTE wrapping addition; index conversion missing |
| F2 | Shared timed wait/input check; matches |
| F3 | CBW state index AND state value, doubled WORD table displacement; both conversions missing |
| F4 | CBW state index then raw BYTE assignment; index conversion missing |
| F5 | Action BYTE CBW then1CD3:0004; missing sign extension |
| F6 | Yes/no WORD1 then target/skip; matches |
| F7 | CBW state index then byte nonzero test; index conversion missing |
| F8 | Absolute target; matches |
| F9 | Menu BYTE CBW, returned WORD doubled into target-table offset; menu conversion missing |
| FA | Border BYTE CBW; missing sign extension |
| FB | Blocking key read; matches |
| FC | FAR inline text display, strlen AX+1 added to cursor; matches under native width/wrap |
| FD | Sidebar redraw; matches |
| FE | Layout BYTE CBW; missing sign extension |
| FF | Exit WORD set1; matches |

The fetch CBW/subtractFFE4/unsigned<=1B dispatch selects exactlyE4..FF for
all256 possible byte values. C's different-looking zero-extended switch has
the same dispatch set. There is no missing low-byte opcode.

ED accesses native C618+17*partyRecord+signedSkillIndex. The header declares
seven named skill bytes, not a `Skill[]` member. Skill80 and minimum0 fail
the native signed test but would pass an unsigned one. Similarly state index80
means D28C natively, not D38C; F3 state valueFF means a table displacement-2,
not+510. These are reconstructed-C mismatches, not evidence those values occur
in shipped scripts. Persistent state has100 declared bytes, not a guarantee
that arbitrary operands are safe.

E8 read/call ordering and EA's extra read are explicitly accounted for. With
immutable, valid RAM script data and the RNG0..255 contract, E8's observable
branch agrees even for high-bit masks; do not label its mask signedness alone
a functional discrepancy. EA's conditional omission matters to an exact memory
trace or a future bounded reader, apart from its definite signed arguments.
EE/EF deliberately use unsigned DWORD comparisons after CWD; this differs from
other signed money comparisons elsewhere and must not be "corrected" to signed.

## Training and jailbreak

All ten setup cases and shared loop/outcome paths were compared. Important
confirmed details: mission4 falls through; mission6 coin flip/mission7 forced
Kurita attack; ten ammo bytes+27..30 cleared in mission2; mission8 skips the
world loop; first idle decrement triggers an immediate redraw; the NPC phase
is a WORD masked3 after the phase1 update; southeast time includes signed
terrain-byte SAR3; rubble interaction accepts Xtarget-1 or Xtarget one row south.

Jail combat occurs on the81st processed update because the old timer is tested
against80 before its incremented value is used. Four WORD attempt flags prevent
retesting one parked mech from advancing the count. The third distinct attempt
starts; both previous attempts fail. Mission8/9 leave the pass byte clear.
Mission0 Locust bonus subtracts50 with WORD wrap before the signed `<215` test.
Missions2..7 consume the Kurita flag and record survival, not destruction, inD311.

Remaining mismatches:

- `Mech.Name` is an array; scalar comparisons need its first byte, not a pointer.
  Skill outcome accesses use nonexistent `Skill.Gunnery/Piloting` rather than
  the declared bytes at+07/+08 (C61B/C61C). Do not count this source as runnable C.
- Exact native text differs: DS1434 ends in a period;146B has lowercase
  "come";14A1 has two spaces after the first sentence;14EF uses "realize"
  and a final period;152A has a final period;155D contains `'Mech, `;
 159A has no leading space and ends in a period;15D4 begins with two CR bytes;
 1611 has a single space between "refuses" and "to". These matter to display
  layout even when the broad mission workflow is right.

## Enemy generation

Native NPC staging uses record stride1A and live combatants16..23. The reset
loops clear four enemy-mech name initials, eight enemy infantry names and24
position/active/wreck slots. Jason's deployment uses a signed WORD `<128` test,
not merely bit7; current C's explicit signed cast preserves that distinction.
Both training random rolls are consumed before jail/mission2 overrides.
World spawning retains left-X roll, coin flip, optional right-X roll, then Y.
Rental mode consumes2D6 even when replacing its result with a uniform3..6.

Full template size125, armour offsets11..1B inclusive (eleven bytes), five
critical attempts34..53, engine/sensor/gyro RNG order, signed severity clamp,
all sprite/stream/active/position and spectator overrides agree in their value
calculations. X/Y offset tables are already declared signed char, preserving
CBW. Infantry only randomizes six skills+04..09; Medical is untouched. Body
is2D6, health is low-BYTE body*10, weapon is RNG%14, matching signed IDIV in
the RNG0..255 domain. Requested mech counts have no native four-record cap;
do not silently add one in a faithful preservation implementation.

Discrepancies: near NPC/position/record/byte pointers lose segment3092. Native
11E1..1205 initializes pilot Name/assignment and increments its index BEFORE
jumping back to the template-copy loop. C does it AFTER copying/damage RNG.
The previous annotation claiming native "last" was false and is corrected.
For disjoint normal records and the documented RNG contract, final values can
agree, but aliasing/invalid-count or instrumentation-sensitive ordering does not.

## Armour, weapons and conversations

Distribution's eight FFFF WORD menu entries and living-member compaction match.
Zero/one living member equips Jason even if Jason is not the survivor, discarding
old equipment. The menu Drop index equals the living count. Exchange happens
before the duplicate-item warning, then the displaced item becomes the held
item. Zero armour type/Cudgel weapon ID stops the respective loop. These
quirks are present in ASM, not fixes to apply during confirmation.

Both routines have near recipient views despite native FAR3092 reads/writes.
Character-name lookups use CBW, armour-description lookups use CBW, and the
equipped-weapon description uses signed BYTE IMUL17. Current unsigned indices
differ for80..FF. Their displaced-item temporaries already explicitly sign-extend
and match. Both native " to:" fragments have a leading space missing in C.

Conversations preserve the last waiting NPC's waypoint, even if that NPC is
not in this building, for the Rick/Atlas special gate. The flag/first-NPC delay
writes and subsequent return match. Other paths preserve compact-choice DEC/JNS
selection, destroyed-Citadel RNG replies, CBW activity indexing and destination
lookup. Remaining differences are near NPC pointers and the native greeting's
two spaces after '?'. The existing Astra candidate remains a gameplay-reachability
question, not an unresolved local ASM instruction.

## Arena and loader

All three arena routines lose FAR3092 through near byte pointers. Apart from
that binding, backup/copy extents, crew BYTE arrays, hide/name/destruction writes,
owned/rental gates and selected-slot-zero alias paths agree. Native paired
C620+17*i and C60F+17*i stores for i1..7 overlap to cover all eight assignment
bytes. Grouped C loops preserve final state for disjoint declared views; no
intervening calls need to see the original per-iteration store order. Neither
setup helper stages world positions or starts combat; summaries now say so.

Loader signed JL comparisons select Disk2 for negative WORD IDs. Current
unsigned tests also select Disk2 for8000..FFFF via `>=17`, so the **disk result
actually agrees across the entire WORD domain**, despite different tests.
However filename indexing shifts the ID twice in native WORD arithmetic:
ID4000 aliases ID0000's FAR-pointer-table entry; C host-wide indexing does not.
Under an explicit native16-bit index/address binding this is equivalent. The
loader is locally matched under the same native WORD/FAR address-view contract
as the other matched helpers, not certified for unmodified host-wide pointer
arithmetic. Normal filename IDs0..25 agree. The decoder's byte addition before
XOR, exact9000 iterations, shared-buffer destination and cache-clear match.

## Follow-up and verification

Correct only reviewed blocks in a later implementation pass. Do not change
original-game quirks merely to make the reconstructed source look cleaner.
`Verify-0FDCSystematicAudit.ps1` exercises dispatch, little-endian recombination,
signed operand witnesses, disk-gate equivalence, table-offset wrap, jail timer/
attempt counts and registry coverage. These are static models, not x86 execution
or gameplay/rendering certification.
