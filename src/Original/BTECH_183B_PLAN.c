#include "game.h"

static const uint8_t movementTerrainMasks[4]={9,3,12,6}; /*3EDB:3B12*/

/* Original183B:193B..1C1E, entire raw ASM checked. Destination orders
 * produce signed BYTE step pairs; working position starts at the CAMERA,
 * not CombatantPackedX/Y. Persistent actor positions are not published.
 * Native has no per-row order/step bounds guards: valid upstream budgets
 * and FF-terminated orders are the contract, not new truncation rules. */
void Combat_Calculate_Movement(uint16_t combatantId)
{
    uint16_t orderStart=(uint16_t)(combatantId*CombatMovementOrderBytes);
    uint16_t planStart=(uint16_t)(combatantId*CombatMovementPlanBytesPerUnit);
    for(uint16_t byte=0;byte<CombatMovementPlanBytesPerUnit;++byte)
        CombatMovementPlanBytes[planStart+byte]=CombatMovementPlanEnd;
    MovementActorPositionX=CrescentHawkMapPositionX;
    MovementActorPositionY=CrescentHawkMapPositionY;
    int16_t savedFacing=(int8_t)CombatantMovementDirection[combatantId];
    uint16_t screenX=CombatPreviewOriginColumn,screenY=CombatPreviewOriginRow;
    uint16_t orderByte=0,stepByte=0;
    for(;;) {
        int16_t mode=(int8_t)CombatMovementOrders[orderStart+orderByte++];
        if(mode==-1) break;
        uint16_t region=(uint16_t)(CombatMovementOrders[orderStart+orderByte++]<<8);
        uint16_t targetX=(uint16_t)((uint16_t)(int16_t)(int8_t)
            CombatMovementOrders[orderStart+orderByte++]|(region&CombatOrderXRegionMask));
        uint16_t targetY=(uint16_t)((uint16_t)(int16_t)(int8_t)
            CombatMovementOrders[orderStart+orderByte++]|(region&CombatOrderYRegionMask));
        if(((int16_t)combatantId<Enemy_All_CombatantId_Range_First &&
            !CombatantActionState[combatantId]) || savedFacing==-1)
            CombatantMovementDirection[combatantId]=(uint8_t)Get_Target_Compass_Direction(
                MovementActorPositionX,MovementActorPositionY,targetX,targetY);
        CombatDestinationBlocked=0;
        while((int16_t)CharacterMovementPointsRemaining>0) {
            if((MovementActorPositionX==targetX && MovementActorPositionY==targetY) || CombatDestinationBlocked) break;
            Movement_Select_Next_Step(combatantId,targetX,targetY,screenX,screenY,FALSE);
            screenX=(uint16_t)(screenX+CombatChosenStepX);
            screenY=(uint16_t)(screenY+CombatChosenStepY);
            CombatMovementPlanBytes[planStart+stepByte++]=(uint8_t)CombatChosenStepX;
            CombatMovementPlanBytes[planStart+stepByte++]=(uint8_t)CombatChosenStepY;
            int16_t relativeX=(int16_t)(uint16_t)(screenX-MapViewportLeftByte);
            int16_t signedY=(int16_t)screenY;
            int16_t halfX=(int16_t)(relativeX>=0?relativeX/2:-((-(int32_t)relativeX+1)/2));
            int16_t halfY=(int16_t)(signedY>=0?signedY/2:-((-(int32_t)signedY+1)/2));
            uint16_t cell=(uint16_t)(halfY*MapCacheWidth+halfX);
            uint16_t mask=movementTerrainMasks[(screenX&1)+(screenY&1)*2];
            /* This routine tests ODD X; Movement_Select_Next_Step tests EVEN X.
             * Retain the distinction rather than sharing an invented helper. */
            if((screenX&1) && (CrescentHawkMapPositionX&1)) { ++cell; mask^=CombatTerrainCameraOddXMaskToggle; }
            if((screenY&1) && (CrescentHawkMapPositionY&1)) { cell+=MapCacheWidth; mask^=CombatTerrainCameraOddYMaskToggle; }
            uint16_t tile=CombatMap[(uint16_t)(CachedMapOriginIndex+cell)];
            uint16_t cost=CombatTerrainNormalStepCost;
            if(mode!=MovementMode_Jump && tile<CombatMovementTerrainLimit && (tile&mask))
                cost=(tile&MapConstructionTerrainMask)==TerrainOcclusionTallCategory?
                    CombatTerrainTallStepCost:CombatTerrainObstructedStepCost;
            /* Step is appended BEFORE cost. Last terrain step can exceed
             * remaining MP. Native SUB WORD then JNS, not widened clamp. */
            CharacterMovementPointsRemaining=(uint16_t)(CharacterMovementPointsRemaining-cost);
            if((int16_t)CharacterMovementPointsRemaining<0) CharacterMovementPointsRemaining=0;
        }
    }
    CombatantMovementDirection[combatantId]=(uint8_t)savedFacing;
}
