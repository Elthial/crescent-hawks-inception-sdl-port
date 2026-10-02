#include "game.h"
/* Sol: exact EXE-owned direction BYTEs3EDB:2ECC..2ED6. Native2ED1 is
 * a prebiased base; use deltaY*4+deltaX+5 for represented signed steps. */
const uint8_t MovementStepDirectionByDelta[MovementDeltaDirectionEntries]={7,0,1,0,6,0,2,0,5,4,3};
/* Sol: original EXE-owned BYTE tables, recovered directly from expanded EXE.
 * Facing lookup is difference+7, NOT difference modulo eight. Location rows
 * contain raw Mech record armour offsets, with 2D6 outcomes2..12.
 * Cluster native base2E5E is sixteen bytes before this first valid cell;
 * index is roll*7+weapon.attackCountOrClusterColumn-16 (columns2..8).
 * Movement penalties index successful nonzero moves across twelve slices.
 * These are original game data, not modern tabletop replacements. */
const uint8_t CombatHitCategoryByFacingDifference[CombatHitFacingEntries]={1,1,0,0,0,3,3,2,1,1,0,0,0,3,3};
const uint8_t CombatTargetMovementPenalty[CombatMovementPenaltyEntries]={0,0,0,1,1,2,2,3,3,3,4,4,4};
const uint8_t CombatMechHitLocationOffsets[CombatHitLocationEntries]={18,19,17,17,19,18,21,23,22,24,20,21,22,22,24,23,21,18,19,17,17,20,26,22,22,24,27,26,25,19,17,17,20,23,24,22,22,24,23,21,18,17,19,20};
const uint8_t CombatMissileClusterTable[CombatMissileClusterEntries]={1,1,1,2,3,5,6,1,2,2,2,3,5,6,1,2,2,3,4,6,9,1,2,3,3,6,9,12,1,2,3,4,6,9,12,1,3,3,4,6,9,12,2,3,3,4,6,9,12,2,3,4,5,8,12,16,2,3,4,5,8,12,16,2,4,5,6,10,15,20,2,4,5,6,10,15,20};
