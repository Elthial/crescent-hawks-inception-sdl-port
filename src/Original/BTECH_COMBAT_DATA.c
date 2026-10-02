#include "game.h"
uint16_t DisableComputerControl; /* Original3092:0090, shared mission/menu WORD. */
uint8_t CombatSavedFacing[AllCombatantCount]; /*original3092:45B6..45CD*/
uint8_t CombatMovementStepCursor[AllCombatantCount]; /*original3092:0078..008F*/
uint8_t CombatSavedTileCache[MapCacheTileCount]; /*original3092:4314..4553*/
uint8_t *SalvageDescriptionByTechSkill[3]={
    (uint8_t *)" and heat sinks",(uint8_t *)", heat sinks and weapons",
    (uint8_t *)", heat sinks, weapons and structure"}; /*EXE-owned3EDB:0FA2 FAR table*/
uint8_t CombatMechYParityAdjustment[AllCombatantCount],CombatMechPreviousMapRowTile[AllCombatantCount]; /*4554/45CE*/
uint16_t CombatantScreenPixelX[AllCombatantCount],CombatantScreenPixelY[AllCombatantCount];
uint16_t KuritaAttackFlag,MissionNpcUpdatePhase; /*3092:3772,3EDB:5802*/
uint8_t CombatantActionState[Enemy_All_CombatantId_Range_First]; /*3092:3994*/

/* Original global storage, not additional game algorithms. Split from menu
 * routines so the round rules can access shared tables without UI coupling. */
uint16_t EnemyTargetId=Enemy_All_CombatantId_Range_First; /*3EDB:2B20*/
uint8_t CombatWeaponTarget[AllCombatantCount*CombatWeaponTargetSlots]; /*3092:3800*/
uint8_t *WeaponRangeText[RangeBracket_OutOfRange+1]={
    (uint8_t *)"Short",(uint8_t *)"Med",(uint8_t *)"Long",(uint8_t *)"OUT"}; /*3EDB:2EBC*/
int8_t CombatWeaponHeat[MechRecordCount]; /*3092:0092*/
uint8_t MechInfernoRoundsRemaining[MechRecordCount]; /*3092:D576*/
uint8_t MapTileUnderCombatant[AllCombatantCount]; /*3092:3750*/
uint16_t ShowArmShotOffAnimation; /*3092:3986*/
uint16_t FriendlyPersonnelWithdrawal,EnemyPersonnelFlightPossible; /*3092:3992/374C*/
uint8_t CombatMovementOrders[AllCombatantCount*CombatMovementOrderBytes]; /*3092:32C6*/
uint8_t CombatMovementPlanBytes[AllCombatantCount*CombatMovementPlanBytesPerUnit]; /*3092:40B4*/
int8_t CombatArmourLocationOffsets[CombatArmourLocationCount]={18,19,17,25,23,24,22,27,21,20,26};
uint8_t *ArmourLocationText[CombatArmourLocationCount]={
    (uint8_t *)"Left Torso",(uint8_t *)"Left Leg",(uint8_t *)"Left Arm",(uint8_t *)"Left Rear Torso",
    (uint8_t *)"Right Torso",(uint8_t *)"Right Leg",(uint8_t *)"Right Arm",(uint8_t *)"Right Rear Torso",
    (uint8_t *)"Center Torso",(uint8_t *)"Head",(uint8_t *)"Rear Center Torso"};
