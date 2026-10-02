# Sol: systematic BTECH_1631 C-to-ASM comparison

Checked2026-09-17 against baseline `155c8ee`: all24 retained methods.
20 locally matched,3 mismatched,1 unresolved native unassigned-return contract.
Executable C is unchanged; new Sol comments, corrected human summaries and
registry entries document this checking-only pass. Earlier segment reviews
remain research evidence, not the fresh certificate.

## Evidence and boundaries

Private `BTech-Reko-expanded/BTECH.reko/BTECH_1631.asm` SHA256
`A6D72B67CA981630FAF00D78C2747E077C69BB028029F8F8ADA329E18CC0DD3D`.
Corresponding `.dis` SHA256
`F79AFB5C13D43155978EC7D98F0043C67AA8B6851D6AC0954E2F88CB8C07ECBA`.
Read the entire annotated file and matching ASM; `.dis` confirms abbreviated
FFFF comparisons and completes the cash-display tail, whose split instructions
also appear at the next183B segment start. Read3EDB initialized text, critical/
movement/weapon tables and selector cells. Correction2026-09-18:55C8 resolves3092, not3EDB.

Compared every meaningful local argument/temporary, read/write, call, branch,
loop and exit. Compiler probes, frames and saved registers are abstracted.
Fully matched means local semantic agreement under native BYTE/WORD and16:16
FAR memory-view contracts, including offset wrap. It does not certify compilation,
host struct layout, shared callees, renderer timing or gameplay. Current named
views are research bindings, not independently reconstructed state ownership.
EGA adapter2 is the retained rendering target:1F73's removed non-EGA drawing
branch is deliberate, not a mismatch. Platform text bindings may replace literal
storage, but must retain native control bytes and display behavior.

## Whole-method ledger

| Method / range | Result | Local workflow checked |
| --- | --- | --- |
| Position_0006 /0006–02E3 | Mismatch | Search bank/timer, facing, eight cumulative candidates, packed corrections, screen/cache addressing, two-tile mech footprint, temporary occupancy anchor, accepted step and blocked-destination latch |
| Display_Text_Human_Health /02E4–032E | Matched | Signed health/Body division and FAR condition-text pointer lookup |
| Menu_Draw_MultiSelect /032F–03AA | Matched | Map projection/copy, optional combat menu versus units, presentation and panels4/3 |
| Combat_Computer_Control /03AB–0BB4 | Matched | Full target scan, approach/path planning, withdrawal, order encoding, target assignment, close-contact override, heat gate, movement/preview and anchor restore |
| Combat_Operation_0BB5 /0BB5–0C62 | Matched | Unpack both axes, signed difference/absolute values, half larger plus smaller distance |
| Combat_Mech_HeatLevels /0C63–0F23 | Matched |24-by12 target cleanup, eight heat records, movement/engine/sink/weapon/inferno/terrain terms, original bank-less upper clamp and dead-personnel presentation |
| Combat_Calculate_RangeBracket /0F24–1056 | Matched | Mech footprint corrections, distance calls, strict range tests, ordinary mech scaling and kick exception |
| Combat_Weapon_Display_Text_And_Menu_Options /1057–10A1 | Matched | Save layout, temporary panel/text/key, redraw and restore |
| Combat_GetAmmoForSelectedWeapon_10A2 /10A2–1121 | Matched |35 critical BYTES, masked weapon classification, ordinal and raw component-minus-one, zero-ammo rejection |
| Combat_StructureHit_1122 /1122–11AA | Unresolved | Raw-offset transfer dispatch and destroyed flag agree; terminal/invalid cases return unassigned native stack WORD |
| Combat_Critical_Mech_Damage_11AB /11AB–15F9 | Mismatch | Full roll/count/notification, destroyed-section marking, head/engine/gyro/torso/arm/leg dispatch, actuator retries and component-helper calls |
| MechId_Random_Count_15FA /15FA–163D | Matched | Signed count loop; count nonzero, undestroyed bytes |
| Combat_fn1631_163E /163E–16AA | Matched | Count gate, random0..3 start, normalization/cyclic scan, one destruction-bit write and return0/1 |
| Return_bool_fn1631_16AB /16AB–1B43 | Matched | Infantry exact/three-cell collisions, anchor restore, mech footprints, same-side infantry blocking, optional opposing crushing and casualty writes |
| Combat_Check_Mech_Actuator_Status /1B44–1B8E | Matched | Raw record-offset BYTE; count missing low-three bits |
| Combat_DisplayText_ArmourHitLocation /1B8F–1BFD | Matched | Eleven signed keys, first-match FAR text copy, period append and verbosity wrapper |
| Combat_Return_bool_fn1631_1BFE /1BFE–1DAA | Matched | Cache update, rental shortcut, initial tile, turning/path steps and parity cache deltas, blocking and local loop termination |
| Combat_CombatMessageVerbosityFilter /1DAB–1DCB | Matched | Nonzero verbosity gate and FAR text dispatch |
| GameSpeed_RateControl /1DCC–1DF7 | Matched | Signed speed<5 selects low-WORD speed*12 retraces, otherwise key wait |
| Combat_CompassPos_1DF8 /1DF8–1EA1 | Matched | Anchor26/12; unsigned packed-axis comparisons and boundary-corrected displacement into shared screen coordinates |
| Combat_AudioVisual_1EA2 /1EA2–1F08 | Matched | Signed-delta octant selection, WORD NEG overflow and native tie rules |
| Combat_AudioVisual_MechPositioning_And_GraphicsMemory_1F09 /1F09–1F72 | Matched | FAR buffer restoration, signed Jason assignment/default4 focus, view move, animation/copy/menu |
| Draw_Combat_Sprites_1F73 /1F73–1FDE | Matched, EGA-only | FAR sprite pointer stride4, AC00:0000 destination and WORD X/Y renderer dispatch |
| Display_Text_CBill_Balance /1FDF–complete split tail | Mismatch | Saved layout, panel/coordinates, heading, green colour, DWORD decimal formatting, signed minimum-width padding, native final write and layout restore |

## Mismatches queued for a later correcting pass

### Position addressing: narrow before arithmetic shift

At0237 native CX subtracts13 as a WORD before SAR1. Current
`((CandidateScreenX - 13) >> 1)` shifts a promoted subtraction without that
intermediate WORD narrowing. At CandidateScreenX=-32768, native subtraction
wraps to32755 and shifts to16377; promoted subtraction is-32781 and shifts
to-16391. The resulting cache-cell difference is32768, not erased by final WORD
assignment. Ordinary screen coordinates agree, but the whole WORD contract does
not. Narrow the subtraction before shifting in a later fixing pass. The surrounding
WORD additions/multiply and final cell assignment otherwise preserve low bits.

### Critical notification text

Native DS:315E is CR followed by `Critical!` and NUL; it has no trailing CR.
Current literal adds one. Remaining critical dispatch/write/loop behavior agrees.
Do not turn this formatting discrepancy into an original-game fault.

### Cash heading and final colour-write segment

Native DS:32DA starts06/0F, prints `C-Bills:` and endsCR. Current spaces omit
both colour controls and the line break. At the end native loads ES from55C8
and writes WORD37FE=15. Correction2026-09-18: the expanded EXE stores2892 at
3EDB:55C8, which relocates to analysis3092. The previous3EDB claim was incorrect.
The original `seg3092->EGATextColour` target is correct; the preservation C
restores that colour, and the annotated label now retains native control bytes.
See CrescentHawksInception/docs/CASH_DISPLAY.md for direct-byte evidence/tests.

## Unresolved return contract

1122 assigns return BP-2 for armour11..18 and most structure19..23 mappings.
For1F/20 it sets E484=1 but does not assign BP-2; invalid inputs also leave it
untouched. C's uninitialized `DamageLocation` documents that observation but
does not reproduce a specific native residual stack WORD. Defined transfer cases
and writes match; whole-method confirmation requires caller consumption/stack
evidence or an explicitly approved replacement policy. No invented default was
added. Existing anatomy labels remain provisional under A-005.

## Larger workflow details confirmed

AI03AB–047D saves/moves the anchor, clears48 order bytes before bank-normalizing
enemy IDs and selects run only for a zero-heat mech.047E–05AA scans4 mech or8
personnel candidates, filters rental13 and protected traitor, keeps first distance
ties and calls distance twice on acceptance. Old-count decrement and failed-scan
fallback are preserved, including possible repeated/out-of-range scans when no
target qualifies. FFFF in0579 is confirmed by `.dis`, not read as00FF.

05AB–0687 preserves raw weapon-code indexing and mech packed-short-range>>3
versus infantry>>5; policy intent remains A-004, not a new transcription fix.
0688–0796 performs the four-WORD path call and six-step infantry withdrawal
with unsigned comparisons and packed-axis correction.0797–08F8 either encodes
the direct destination or uses fixed signed step deltas and **old-distance**
post-decrement approach loop, including the last failed test.08F9–0A9F assigns
personnel targets/protected-traitor suppression or applies the close-contact
box and facing override.0AA0–0BB4 clears12 mech targets, assigns ten weapons and
kick slot11 below signed heat30, otherwise cancels movement; always calculates
movement, optionally previews friendlies and restores the anchor.

0C63 clears target high bits before active lookup, so CBW indexes remain0..127.
Each existing mech receives signed movement/weapon heat, unsigned engine hits*5
and engine sinks, intact critical sinks, inferno6 and terrain cooling4. Enemy
movement/terrain values overwrite preliminary friendly reads without side effects.
BYTE heat wraps before signed lower clamp. Original upper clamp still addresses
friendly slot rather than bank-adjusted record (existing BUG-013); weapon heat
is cleared even for skipped records. Dead personnel map to combatants4..11 or
16..23; Jason coordinates survive. These native quirks were not silently fixed.

Critical11AB rolls2D6 even for a destroyed section; remaining hits start1 and
increase at8/10/12. Destroyed sections mark their nonempty component block and
clear the appropriate actuator nibble without newly setting E484. Live head
systems retry unavailable selections; life-support +78 remains a probable label.
Engine/gyro BYTE increments retain wrap and unsigned destruction thresholds.
Arm/leg components versus actuator rolls, signed masks, clear masks, retry counts
and no-eligible-component termination agree. Component helper163E returns success,
not a raw index; its biased random start is preserved, not replaced by uniform
selection. Corrected human summaries now describe those actual operations.

16AB never reads passed dx/dy: candidate coordinates are global. Infantry
collision checks omit active filters in both exact-personnel and mech-footprint
branches; temporary anchor is restored. Mechs use corrected/unpacked X distance,
same packed Y and active filters. Same-side infantry block; opposing infantry
are killed only when crush is enabled and unblocked. Sound, delays, persistent
effect, sprite/active/Health/Name and main-character-alive writes agree. Crushing
does not change blocked result or invalidate positions/casualty-loot flags here.
1BFE tests terrain, not occupancy, and updates cache even before rental shortcut.

Distance operands unpack to0..2047, so their signed absolute differences cannot
hit8000. Octant helper does accept arbitrary signed WORDs and explicitly narrows
NEG before comparison, preserving8000 and(0,0)->6.1DF8 updates screen coordinates,
not a compass marker;1EA2 selects an octant, not visibility. Those summaries were
corrected without renaming methods. Rendering integration remains unvalidated.

Verification uses static witnesses and existing combat models, not native x86
execution/gameplay. Fresh registry records include checked source-body hashes;
the report and method inventory distinguish matching, mismatch and unresolved.
