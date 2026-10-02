# Sol: systematic BTECH_183B C-to-ASM comparison

Checked2026-09-17 against baseline `861f553`: all17 retained methods.
12 locally matched,5 mismatched. Executable C is unchanged; Sol TODO comments,
human-summary corrections and fresh registry entries document this checking pass.

## Evidence and contracts

Private `BTech-Reko-expanded/BTECH.reko/BTECH_183B.asm` SHA256:
`474F23D1CE62B0B643C85106A1862E01955642771E92C9B0BA12FDFF988DBBDE`.
Corresponding `.dis` SHA256:
`72F20FC13361274D0A54A731F319F71F02ABE5563CC515F181BA02C8997A6336`.

Read the complete annotated C and exported ASM. The2AA3 ASM export stops after
the1314 call; `.dis` supplies1DF8, POP BP and return. Read initialized3EDB text
and selector cells55CA..5628. Native55EC=3092 correctly binds this file's text
colour writes. Correction2026-09-18:1631 cash helper55C8 likewise resolves3092;
the old claim of a3EDB selector discrepancy was wrong (see CASH_DISPLAY.md).
`.dis` confirms FFFF facing/position comparisons despite abbreviated ASM FF.
The salvage technician comparison really is00FF after CBW, retaining BUG-017.

Compared meaningful arguments, reads/writes, calls, branches, loops and exits;
compiler frames/probes/saved registers are abstracted. Matching assumes native
BYTE/WORD arithmetic and16:16 FAR views, including offset wrap, and documented
shared-call contracts. Automatic glyph storage must retain the native SS pointer
when ported; this is not certification of a modern compiler's near-to-far cast.
Named research views are not a compilable memory-layout struct. Shared callees,
gameplay, rendering timing and platform implementation are not certified here.

## Whole-method ledger

| Method / native range | Result | Local workflow checked |
| --- | --- | --- |
| Combat_Parent_000A /000A–1481 | Mismatch | Encounter setup/assessment, consent/evasion, settings and rounds, friendly/enemy planning, mechanics, all betrayal branches, flight/end checks, Kurita messages, salvage/healing, arena/training outcomes and common NPC exit |
| Combat_Computer_Control_1482 /1482–14C2 | Matched | Twelve relative IDs, WORD active filter, planner(id,0) |
| Combat_UI_Menu_Logic /14C3–1773 | Matched | Initial active scan/default, action-state preview, mech/personnel menus, signed returned choice, planning/clear/kick/computer/weapon/scan dispatch, next-unit wrap, flee return |
| Combat_MovementTarget_unknown_1774 /1774–193A | Matched | Optional anchor/cache/render, mech/personnel marker, friendly budget/path calls, sentinel-before-bound probe, signed deltas, direction glyph FAR SS buffer and endpoint writes |
| Combat_Calculate_Movement /193B–1C1E | Mismatch | Step-row initialization, saved facing, four-byte orders, direction override, blocked flag, append-before-cost, parity/cache masks, terrain costs/budget clamp and facing restoration |
| Combat_Select_Movement_Plan_For_Turn /1C1F–2230 | Mismatch | AI takeover, mode-change consent/deletion, destination scan, budget/status messages, eleven-entry movement-key table, cursor bounds, provisional destination replacement, anchor restore/redraw and Space/Return exit |
| Combat_Kick_Target /2231–22BB | Mismatch | Both hip bits required, target UI(id,32,11) versus message/key wait, restore Kick default4 |
| Combat_Mech_Movement /22BC–2473 | Matched | Unsigned walk/jump BYTE load, hip/actuator reductions, signed heat/5, run SAR increment, exact heat30 shutdown and final signed clamp |
| Combat_Infantry_Movement /2474–24EF | Matched | Enemy fixed6; friendly signed Dexterity*3/SAR2, minimum3 before signed armour penalty, final maximum8 |
| SettingsMenu_CombatMessages /24F0–2555 | Matched | Text/layout/default, WORD menu return, clear vertical layout; parent stores setting |
| SettingsMenu_SeeCombatGraphics_bool /2556–2590 | Matched | Prompt/current WORD setting, preserve prompt WORD return; parent stores setting |
| Scan_Enemies /2591–273C | Matched | Menu choice, enemy-mech restrictions/messages, sensor equality2, active/non-FFFF enemy-human scan, availability sum, negative-choice fallback, browser arguments and defaults |
| Check_If_CriticalSlot_Destroyed /273D–27C8 | Matched | Signed raw-offset thresholds, structure BYTE result, packageC8 return1 override |
| Register_Persistent_Map_Effect_27C9 /27C9–2834 | Mismatch | Raw low-BYTE slot, four effect arrays, packed page/low coordinates, WORD counter increment and low-BYTE limit/reset |
| Combat_Load_9Grid_Map_2835 /2835–28DA | Matched | Combined high BYTE minus11, three rows/columns, signed region bounds, nonzero map ID, FAR descriptor90 skip, signed map BYTE call and cache rebuild |
| Combat_Assess_FirstEnemy_Reachability_28DB /28DB–2AA2 | Matched | Jason foot/mech start, first active enemy,30-step special80 probe, pre-step arrival returns0, post-step arrival returns1, unconditional viewport restore |
| Combat_Character_Pos_Grid /2AA3–complete split tail | Matched | Move packed anchor, rebuild offset grid and cached origin; no actor-position or scratch-state restoration |

## Discrepancies for the correcting pass

### Parent: display bytes and signed name indices

Native3402 is `Attacking force:` followed by CR, whereas C appends a space.
Native341B is ` and` followed by CR, whereas C uses a trailing space.
Native34B8 ends `combat.`; C omits its final period. Remaining inspected parent
dialogue literals agree. Betrayal name-table indexing uses CBW of each name BYTE;
C's unsigned TraitorNameId and pilot/rider name accesses differ for128..255.
Valid ordinary names agree, but the full raw-byte address contract does not.
Preserve signed indexing or explicitly approve sanitization during porting.

The betrayal wreck-family expression is correct: native unsigned family<1
selects80, otherwise81; `MECH_Sprite_LOCUST` is0, so the C equality matches all
BYTE values. Do not replace it based on an assumed Locust=1 convention.

### Movement builder: WORD subtraction before SAR, and signed ID test

Native subtracts13 from CX as a WORD before SAR1. C shifts a promoted
`ScreenX-13` without intermediate narrowing. At ScreenX=-32768, native yields
16377 versus promoted C -16391; final cell assignment does not erase the32768
difference. This is the same pattern recorded in1631 Position_0006.
Native1A30 tests signed combatant<12, while C uses unsigned CombatantId<12.
Ordinary IDs0..23 agree;8000..FFFF differ. No extra order/step-row bounds were
invented: native lacks them, and a managed port needs an explicit policy.

### Movement-plan dialogue

Native3B38 is `ing.` rather than `ing`.3B3D (`Do you wish to ...`),3B8C
(`Press a key ...`) and3C5B (`You might move farther ...`) each start with CR;
current literals omit it.3C0D `leg damage.` ends CR, also omitted. Other inspected
movement-dialogue strings agree. Control bytes matter to wrapping/layout.

### Kick refusal text

Native3CFA begins `This 'Mech`; current C begins `This mech`. The remainder,
hip tests, calls and default selection agree.

### Persistent-effect counter width

Native uses `INC WORD PTR ES:[D557]`, but reads the slot and tests/resets only
the low BYTE. `.dis` independently shows the WORD read/write. C's BYTE field
increment cannot carry into D558. Valid ring slots0..63 agree, including63->0;
a raw FF counter increments D558 natively and leaves low BYTE0, unlike C.
The adjacent byte's ownership/intended counter width needs investigation before
changing the header or calling this an original-game bug. No runtime fault is
claimed and no correction was made.

## Important native behavior retained

Planning menus assume an active friendly: initial scan can reach12 and Next Unit
can loop indefinitely otherwise. Movement preview probes the next row's first
BYTE when a full24-byte row reaches its guard. Builder steps are recorded before
terrain costs; the last step may exceed remaining points. Direction updates are
temporary: saved facing is restored. Repeated destination detection uses signed
CBW of region/coordinate bytes, including its high-region comparison behavior.

Mech walk/jump bytes are zero-extended, not CBW; heat is signed and IDIV5
truncates toward zero. Damage minimum adjustment occurs during each4/2/1 mask
iteration, run adds SAR(points,1) plus original walk parity, jump bypasses damage/
heat penalties but not exact30 shutdown. Infantry minimum precedes armour
halving, so it can become1. These are native rules, not transcription fixes.

Salvage retains the dead-technician00FF/FFFF mismatch and the legacy friendly
salvage-array gating; imported labels alone do not justify new logic. Kurita
message progression, ruined-Citadel loading, common exit animation writes and
NPC-record stride26 agree. The reachability probe returns0 if already at its
target, and leaves compass/budget scratch changed despite restoring the viewport.

Human summaries were corrected for step building versus budgeting, preview
endpoint writes, settings returned to the parent, supporting-structure lookup
and unconditional effect-ring overwrite. Verification is static, not gameplay.
