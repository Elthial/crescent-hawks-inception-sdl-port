#include "game.h"

/* Original1631:0C63..0F23, complete raw ASM checked. Movement-step BYTEs
 * come from the execution parent's24-byte array. Addition wraps BYTE heat
 * BEFORE signed clamping. BUG013 clamps the FRIENDLY slot even during the
 * enemy bank iteration: don't replace that index with mechRecordId. */
void Combat_Mech_HeatLevels(uint8_t *successfulMovementSteps)
{
    for(uint16_t combatantId=0;combatantId<AllCombatantCount;++combatantId)
        for(uint16_t slot=0;slot<CombatWeaponTargetSlots;++slot) {
            uint8_t *target=&CombatWeaponTarget[combatantId*CombatWeaponTargetSlots+slot];
            if(*target!=UINT8_MAX) {
                *target&=CombatTargetIdMask; /* Sol: clear fired bit; retain planned combatant ID. */
                if(!CombatantActive[*target]) *target=UINT8_MAX;
            }
        }
    for(uint16_t friendlySlot=0;friendlySlot<LanceSize;++friendlySlot)
        for(uint16_t recordBank=0;recordBank<=Enemy_Mech_Record_First;recordBank+=Enemy_Mech_Record_First) {
            uint16_t mechRecordId=(uint16_t)(friendlySlot+recordBank);
            uint16_t combatantId=(uint16_t)(friendlySlot+(recordBank?Enemy_All_CombatantId_Range_First:0));
            Mech *mech=&Mechs[mechRecordId];
            if(mech->name[0]!=MECH_Destroyed) {
                int16_t heatChange=(int16_t)((int8_t)CombatMovementOrders[combatantId*CombatMovementOrderBytes]+1);
                if(heatChange==MechJumpMinimumHeat && (int8_t)successfulMovementSteps[combatantId]>MechJumpMinimumHeat)
                    heatChange=(int8_t)successfulMovementSteps[combatantId];
                heatChange=(int16_t)(heatChange+mech->engineHits*MechEngineHitHeatPerRound-mech->engineHeatSinks);
                for(uint16_t offset=MECH_ComponentBlock_Start;offset<=MECH_ComponentBlock_End;++offset)
                    if(((uint8_t *)mech)[offset]==Heat_Sink) --heatChange;
                heatChange=(int16_t)(heatChange+CombatWeaponHeat[mechRecordId]);
                MechHeatLevel[mechRecordId]=(int8_t)(uint8_t)(MechHeatLevel[mechRecordId]+heatChange);
                if(MechInfernoRoundsRemaining[mechRecordId]) {
                    MechHeatLevel[mechRecordId]=(int8_t)(uint8_t)(MechHeatLevel[mechRecordId]+MechInfernoHeatPerRound);
                    --MechInfernoRoundsRemaining[mechRecordId];
                }
                if(TerrainOverlapRows[combatantId] && MapTileUnderCombatant[combatantId]<CombatCoolingTerrainTileEnd)
                    MechHeatLevel[mechRecordId]=(int8_t)(uint8_t)(MechHeatLevel[mechRecordId]-MechTerrainCoolingHeatPerRound);
                if(MechHeatLevel[mechRecordId]<0) MechHeatLevel[mechRecordId]=0;
                if(MechHeatLevel[friendlySlot]>MechHeatShutdownLevel) MechHeatLevel[friendlySlot]=MechHeatShutdownLevel;
            }
            CombatWeaponHeat[mechRecordId]=0; /*Even absent/destroyed Mechs*/
        }
    for(uint16_t characterId=0;characterId<CharacterRecordCount;++characterId)
        if(Characters[characterId].name==Character_Dead) {
            uint16_t combatantId=(uint16_t)(characterId+Friendly_Infantry_Combatant_Range_First);
            if(characterId>=Enemy_Infantry_Record_First) combatantId+=Enemy_Infantry_Record_First-Friendly_Infantry_Combatant_Range_First;
            if(characterId!=Character_Jason) CombatantPackedX[combatantId]=CombatantPackedY[combatantId]=CombatantPosition_Unused;
            if(combatantId>=Enemy_Infantry_CombatantId_Range_First) CombatantSpriteFamilyOffset[combatantId]=CharacterSpriteFamily_EnemyInfantry;
            if(characterId==Character_Jason) CombatantSpriteFamilyOffset[combatantId]=CharacterSpriteFamily_Jason;
            /*Active flags are not cleared here.*/
        }
}
