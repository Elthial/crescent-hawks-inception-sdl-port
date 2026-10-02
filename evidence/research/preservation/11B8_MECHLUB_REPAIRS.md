# Sol: Mechlub repairs — in-place preservation cleanup

Baseline `47968b2`. Clean only the existing `Mechlube_Repair_Mech` at11B8:0002–0809
and its shared prototype/constants. Retain its public name and original address;
modify/upgrade/ammunition routines are unchanged. No new module or duplicate game
implementation. Mechlub services continue next with ammunition, before salvage,
upgrades and combat as separately reviewable blocks.

## BattleTech names and meaningful numbers

Lower-camel locals now name the selected Mech, armour points, internal-structure
points, destroyed critical components and weapon-type repair menu. This routine
does not use a Tech skill roll: it buys shop work. Do not invent a tabletop skill
check. Eleven armour locations are eight primary locations plus rear armour on
the three torso locations; eight internal-structure locations omit rear sections.
The two packed actuator bytes each contain arm/leg state on one side.

| Named constant | Native meaning |
| --- | --- |
| MechArmourLocationCount / MechInternalStructureLocationCount | 11 armour /8 internal-structure bytes |
| MechActuatorSide_Left / Right | Packed side indexes0/1, not four limb records |
| MechRepairArmourPointCostCBills | 4 per armour point |
| MechRepairStructurePointCostCBills | 9 per internal-structure point |
| MechRepairHeatSinkCostCBills | 800 per exact destroyed heat sink |
| MechRepairWeaponSelectionCostCBills | 300 per weapon-type menu selection |
| MechRepairActuatorsCostCBills | 200 flat fee for one or both sides |
| MechRepairWeaponScratchCount | Native20-WORD arrays, not20 weapon types |
| MechRepairWeaponTypeCount | 17 component types, Small Laser10h through SRM-620h inclusive |
| MechRepairWeaponContinuationTypeCount | Original16-index continuation scan; deliberately excludes SRM-6 |
| MechComponentToWeaponRecordBias | Component ID minus1 gives weapon-table record index |

Generic menu row positions and the balance-display dialogue action are named
as UI/system values, not gameplay counts. Exact visible strings and their native
address comments are retained. Native labels0800/0558 become
`WaitForRepairAcknowledgement` / `DisplayRepairResultAndWait`, without reordering
the routine. The latter intentionally remains the shared cross-block result path.

## Incorporated ASM corrections

Re-read the complete0002–0809 ASM after the annotation audit. Selected-Mech and
raw critical-byte pointers now retain FAR3092, rather than silently using DS3EDB.
The native125-byte Mech structure is copied verbatim into generated test inputs;
compile-time witnesses confirm its size and armour/internal-damage offsets.

Use uint16_t locals and explicit unsigned WORD narrowing for deficit accumulation,
the combined damage zero-test, armour/structure quotes and cancel-row subtraction.
Host integer promotion must not change native65535+1 into nonzero65536 or render
a wrapped quote as a larger host number. Deficits are BYTE maxima minus BYTE
current values, not signed BYTE stats. Their result is a wrapped WORD.

Balances and debits remain native unsigned DWORD operations. Host test adapter
uses uint32_t; the existing shared unsigned-long balance is32-bit in the native
and Windows environments. A full cross-platform memory declaration pass is
still required; this is not certification of the complete scratchpad header.

## Original behaviour and faults preserved

- Order: armour, internal structure, heat sinks, weapons, actuators. Partial work
  stays bought when funds run out; records are repaired in physical byte order.
- Engine/gyro/sensor damage is checked only when the combined WORD work total
  is zero, and only the first such damage is reported as unrepairable.
- Dialogue says structure is needed to mount equipment, but no extra structural
  prerequisite gates heat-sink/weapon purchases. Do not add one.
- BUG-008: one weapon selection repairs every matching destroyed critical slot,
  but decrements its local menu count only once. A later redundant payment can
  therefore repair no additional physical component. Do not correct the counts.
- BUG-009: continuation omits SRM-6; other weapon repair can stop the menu while
  SRM-6 remains destroyed. Preserve the unequal display/continuation bounds.
- Restore right then left actuator bytes from chassis maxima, after the key,
  then debit the flat fee. Do not substitute universalFF or debit earlier.
- Above-maximum armour/structure remains a wrapped deficit and can buy further
  BYTE increments, not a clamped deficit. Wrapped aggregate cancellation can
  choose the original no-work path despite physical damage.

Equivalence requires valid selected-Mech and menu indexes, native BYTE/WORD/FAR
views, and shared UI/text/balance helpers that do not change Mech_Selected during
the visit. Native repeatedly reloads that selector; the existing selected-record
cache assumes it stays fixed. Malformed wrapped pointer/table addresses are not
made safe or fully emulated by ordinary host arrays. Platform stack probes and
frame mechanics remain abstractions. Shared callee mismatches are not resolved
by this caller cleanup; in particular Prompt_Yes_No's FAR-text issue remains.

## Verification

`scripts/Verify-PreservationRepairs.ps1` compiles the ACTUAL source body as C17
with `/W4 /WX`, generated constants/Mech view from the real shared header and
a narrow UI/state adapter. 850,042 checks pass (including instrumentation,
not that many distinct gameplay scenarios). Covers all65,536 maximum/current
BYTE pairs independently for armour and internal structure, wrapped quotes,
partial/refused/no-funds repairs, high balance WORD, selected record isolation,
heat sinks, actuator affordability/maxima, cancel WORD wrap, both original weapon
bugs and combined damage wrap. The synthetic models do not execute the EXE or
validate gameplay/UI integration. Whole-game compilation remains pending.
