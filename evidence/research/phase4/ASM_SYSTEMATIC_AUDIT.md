# Systematic C-to-ASM audit

Sol: Fresh confirmation baseline reset on 2026-09-17 at source revision
`62ab3d0`. Earlier reviews remain research evidence, not whole-method certificates.
Work one source file at a time; do not bulk-promote methods from mentions in MD.

## Procedure

1. Read the current annotated file and matching original ASM, including all
   branch targets and any truncated tail in `.dis` or the full listing.
2. Account for arguments, return values, meaningful stack/register temporaries,
   reads/writes, calls, branch conditions, loops and side effects.
3. Check native WORD/BYTE signedness, overflow, FAR offset wrap and units.
4. Record deliberate platform bindings, EGA-only omissions and other deviations.
   Check a callee only as far as needed to resolve the caller's contract; this
   does not confirm that other file.
5. List mismatches and unresolved operations. A mismatch is not a game bug.
   Do not silently fix substantive code during a checking-only pass.
6. Record an explicit status, source-body hash, source revision, matching ASM
   hash, date, checked blocks and evidence report. No inferred confirmation.

Fully matched is a local semantic comparison under documented contracts, not
compiled C, gameplay, rendering or music validation. Removing compiler stack
probes/prologues is a documented abstraction, not an excuse to omit a game write.

## File queue

| File | Fresh audit | Result / next action |
| --- | --- | --- |
| `BTECH_0D27.c` | [Startup comparison](BTECH_0D27_SYSTEMATIC_ASM_AUDIT.md) | All3 implementations locally matched; EGA/platform abstractions documented |
| `BTECH_0800.c` | [Exploration comparison](BTECH_0800_SYSTEMATIC_ASM_AUDIT.md) | All49 checked:28 matched,20 mismatches,1 unresolved binding; corrections pending review |
| `BTECH_0DAB.c` | [Salvage/UI/platform comparison](BTECH_0DAB_SYSTEMATIC_ASM_AUDIT.md) | All14 checked:4 matched,9 mismatches,1 unresolved runtime binding; corrections pending review |
| `BTECH_0FDC.c` | [BLD/mission/arena comparison](BTECH_0FDC_SYSTEMATIC_ASM_AUDIT.md) | All15 checked:5 matched,10 mismatches; corrections pending review |
| `BTECH_11B8.c` | [Repairs/upgrades/recruitment comparison](BTECH_11B8_SYSTEMATIC_ASM_AUDIT.md) | All10 checked: repair/ammo corrected/rechecked locally;2 matched,8 mismatches, including confirmed arena-selector segment |
| `BTECH_135D.c` | [Star League cache/puzzle comparison](BTECH_135D_SYSTEMATIC_ASM_AUDIT.md) | All13 checked:7 matched,6 mismatches; native door playback is FFFF, not00FF; corrections pending review |
| `BTECH_1431.c` | [Healing comparison](BTECH_1431_SYSTEMATIC_ASM_AUDIT.md) | All1 checked:1 locally matched under native memory/shared-call contracts |
| `BTECH_1467.c` | [Crew/quiz/selection comparison](BTECH_1467_SYSTEMATIC_ASM_AUDIT.md) | All4 checked:2 matched,2 mismatches; crew row-map DS/SS binding also unresolved |
| `BTECH_1631.c` | [Combat planning/damage/collision comparison](BTECH_1631_SYSTEMATIC_ASM_AUDIT.md) | All24 checked:20 matched,3 mismatches,1 unresolved native unassigned-return contract |
| `BTECH_183B.c` | [Combat parent/menu/movement comparison](BTECH_183B_SYSTEMATIC_ASM_AUDIT.md) | All17 checked:12 matched,5 mismatches; correction pass pending review |
| `BTECH_1AE8.c` | [Combat mechanics/effects comparison](BTECH_1AE8_SYSTEMATIC_ASM_AUDIT.md) | All3 checked:1 matched,2 mismatches; component/table-index mix-up and remaining Reko expressions queued |
| `BTECH_1543.c` | [Weapon/target/ejection/input comparison](BTECH_1543_SYSTEMATIC_ASM_AUDIT.md) | All5 checked:5 mismatches; explicit near-pointer bindings, menu controls and signed gates queued; local subflows documented |
| `BTECH_1CD3.c` | [Building dispatcher/helper comparison](BTECH_1CD3_SYSTEMATIC_ASM_AUDIT.md) | All5 checked:3 matched,2 mismatches; all47 actions covered, reversed fire cleanup, array-as-value accesses and dialogue/layout queued |
| `BTECH_1E56.c` | [Panel/text/menu/input comparison](BTECH_1E56_SYSTEMATIC_ASM_AUDIT.md) | All15 checked (10 native/5 research helpers):13 matched,2 mismatches; text stack FAR binding and three keyboard aliases queued |
| `BTECH_1F3D.c` | [EGA/input/load/allocation comparison](BTECH_1F3D_SYSTEMATIC_ASM_AUDIT.md) | All18 checked (17 native/one research helper):16 matched,1 mismatch,1 unresolved allocator return; explicit loader SS binding queued |
| `BTECH_1FC5.c` | [Sound dispatcher/generator comparison](BTECH_1FC5_SYSTEMATIC_ASM_AUDIT.md) | All15 checked:15 locally matched under valid-stream/native WORD and register-handoff contracts; no executable corrections |
| `BTECH_207F.c` | [Hardware/EGA/input/map/rendering/runtime comparison](BTECH_207F_SYSTEMATIC_ASM_AUDIT.md) | All82 retained implementations checked:76 locally matched under explicit contracts,6 missing represented DF-clear effects; executable corrections queued |
| `BTECH_204B.c` | [Music/timer comparison](BTECH_204B_SYSTEMATIC_ASM_AUDIT.md) | All19 checked:17 local contract matches,2 dispatcher parent-frame/variadic argument mismatches; corrections queued |

Data-only files and omitted ASM entries need classification separately. The
inventory's replacement column does not count as an ASM audit.

All312 retained implementations now have fresh explicit audit records:
225 locally matched under documented contracts,83 mismatches and4 unresolved
bindings/return contracts. Crew assignment additionally retains an unresolved
DS/SS issue inside a mismatched method. This completes the checking pass, not
the executable correction pass or gameplay validation.

Preservation-C cleanup has started in place: [dice methods0800:19DD/19F3](../preservation/0800_DICE_CLEANUP.md).
Caller-only renames are reverse-substitution/hash checked; retained mismatch
statuses are not promoted by cleanup. Original game bugs remain preserved.

Next in-place block completed: [healing1431:000A](../preservation/1431_HEALING_CLEANUP.md).
Rechecked after cleanup; medical/party names replace Reko suffixes and literals,
native post-dice Health reload restored. Actual-source C17 adapter tests pass;
full-game compilation and gameplay comparison remain pending. Follow with
Mechlub services, salvage, upgrades, then combat.

Mechlub repair0002–0809 now cleaned in place and rechecked: [repair correction](../preservation/11B8_MECHLUB_REPAIRS.md).
Only that method's FAR/WORD discrepancies are resolved; the rest of11B8 and its
shared callees are not promoted. Original repair bugs and readable text remain.

Mechlub ammunition1762 through DIS return now corrected/rechecked:
[ammo correction](../preservation/11B8_MECHLUB_AMMUNITION.md). Shared callee
discrepancies remain; original unsigned/signed edge effects and shop restrictions
are preserved. Next: salvage.

Salvage0002–0258 prefix corrected in place: [material salvage](../preservation/0DAB_SALVAGE_MATERIALS.md).
Whole-method status remains mismatched. Weapon salvage's inherited native stack
BYTE cannot be certified by leaving a host C local uninitialised; explicit native
frame-state representation is queued, not replaced by a bug fix.
