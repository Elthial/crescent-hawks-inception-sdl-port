# Sol: variable naming cleanup —2026-09-18

Follow-up: remaining memory-variable address suffixes have also been removed
from the annotated source and its live verification scripts. The complete
old-to-new mapping and original addresses are in [MEMORY_VARIABLE_NAMES.md](MEMORY_VARIABLE_NAMES.md).
The preservation C variables were checked again and already had clean names.
Native method-address suffixes are retained; the earlier address-variable policy
below records the first cleanup, not the current variable naming policy.

Follow-up validation: all 26 annotated files match exactly the planned identifier
substitutions, apart from removed trailing whitespace. The 113 headless and 132
SDL/local-asset C tests pass. Live annotation checks were updated to match the
new variable names, including previously stale repetition/noise local-name checks.
There are no arithmetic, control-flow, table-layout or original-bug changes.

Removed decimal Reko suffixes from local variables and parameters across the
annotated Btech code:278 scoped names in68 methods, across13 source files.
The compiled Reimplementation/src/Original code was also inspected; its
variables and parameters already had no such suffixes.

Names that would collide receive distinct role names rather than merging locals:
RoundSlice, MovingCombatantId, WeaponSlot; ArmourPurchaseCost,
WeaponPurchaseCost, MedicalTreatmentCost; SelectedRangeBracket,
PackedWeaponRange; PlacementScanX, PlacementScanY, PlacementRowStartX.
Combat effects use ShooterId, TargetCombatantId, TargetRecordId,
TargetMechDestroyed, TargetPersonnelDead, AttackDirection and SavedCombatMap.

This is identifier cleanup only: no arithmetic, branch conditions, types,
initialization, call order or original bug fixes are changed. The annotated
code is still pseudo-C, not a compilable target. Existing C tests cannot prove
these pseudo-C methods correct.

Native method-address suffixes and named memory-address views remain distinct
from discarded Reko uniqueness numbers. Their addresses identify original
methods/storage and are not arbitrary local variable IDs. Historical audit
documents retain old names as evidence; git history supplies the exact mapping.

Validation: an identifier-normalized token comparison against abfed0b passes
for all13 changed source files, including unchanged literals and operators.
All108 headless C tests pass. No new executable C behaviour was introduced.
