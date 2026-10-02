#include "game.h"

/* Sol: Original1631:0006..02E3, all raw ASM branches/.dis checked.
 * Actor/probe ID80 skips occupancy, not terrain or mech footprint. The routine
 * changes working actor position/facing and chosen step, not world tables.
 * Caller must provide native-valid cache indices; no new bounds guard. */
void Movement_Select_Next_Step(uint16_t actorOrProbeId,uint16_t targetX,uint16_t targetY,
    uint16_t screenX,uint16_t screenY,uint16_t alternateDirectionSearch)
{
    if (alternateDirectionSearch) {
        --MovementSearchBankTimer;
        if (!MovementSearchBankTimer) {
            MovementSearchBankTimer=MovementSearchBankTogglePeriod;
            MovementSearchBank^=CompassDirectionCount; /* Native low BYTE XOR8. */
        }
    } else MovementSearchBank=0; /* Native WORD clear, not just low BYTE. */
    uint16_t facingSlot=actorOrProbeId&MovementProbeActorMask;
    int16_t candidateFacing=(int8_t)CombatantMovementDirection[facingSlot];
    CombatChosenStepX=0; CombatChosenStepY=0;
    uint16_t needsMechFootprint=((uint8_t)actorOrProbeId&MovementProbeActorMask)<LanceSize
        || ((int16_t)actorOrProbeId>=Enemy_All_CombatantId_Range_First
            && (int16_t)actorOrProbeId<Enemy_Infantry_CombatantId_Range_First);
    uint16_t desiredFacing=(uint16_t)Get_Target_Compass_Direction(
        MovementActorPositionX,MovementActorPositionY,targetX,targetY);
    if (desiredFacing==MovementHeading_AtDestination) return;
    int16_t headingDifference=(int16_t)(uint16_t)(desiredFacing-candidateFacing);
    if (headingDifference) {
        headingDifference=(int16_t)((uint16_t)headingDifference&CompassDirectionMask);
        candidateFacing+=(headingDifference<=CompassClockwiseTurnLastDifference)?1:-1;
        candidateFacing=(int16_t)((uint16_t)candidateFacing&CompassDirectionMask);
    }
    for (int16_t attempt=CompassDirectionCount-1;attempt>=0;--attempt) {
        int16_t adjustment=MovementDirectionSearchOffsets[MovementSearchBank+attempt];
        candidateFacing=(int16_t)((uint16_t)(candidateFacing+adjustment)&CompassDirectionMask);
        int16_t stepX=MovementDirectionStepX[candidateFacing],stepY=MovementDirectionStepY[candidateFacing];
        uint16_t candidateX=(uint16_t)(MovementActorPositionX+stepX);
        uint16_t candidateY=(uint16_t)(MovementActorPositionY+stepY);
        if (candidateX&PackedPositionLocalCarryBit) candidateX+=MovementPackedBoundaryX[candidateFacing];
        if (candidateY&PackedPositionLocalCarryBit) candidateY+=MovementPackedBoundaryY[candidateFacing];
        int16_t candidateScreenX=(int16_t)(uint16_t)(screenX+stepX);
        int16_t candidateScreenY=(int16_t)(uint16_t)(screenY+stepY);
        /* Native SUB CX,13 wraps BEFORE SAR1. Negative odd SAR floors down. */
        int16_t relativeX=(int16_t)(uint16_t)(candidateScreenX-MapViewportLeftByte);
        int16_t halfX=(int16_t)(relativeX>=0?relativeX/2:-((-(int32_t)relativeX+1)/2));
        int16_t halfY=(int16_t)(candidateScreenY>=0?candidateScreenY/2:
            -((-(int32_t)candidateScreenY+1)/2));
        uint16_t cell=(uint16_t)(halfY*MapCacheWidth+halfX);
        if (!(candidateScreenX&1) && (CrescentHawkMapPositionX&1)) ++cell;
        if ((candidateScreenY&1) && (CrescentHawkMapPositionY&1)) cell+=MapCacheWidth;
        uint16_t cacheIndex=(uint16_t)(CachedMapOriginIndex+cell);
        uint16_t tile=CombatMap[cacheIndex];
        if ((int16_t)BlockingTileCodeThreshold<=(int16_t)tile) continue;
        uint16_t allowed=TRUE;
        if (needsMechFootprint) {
            cell=(uint16_t)(cell+(((candidateScreenX^CrescentHawkMapPositionX)&1)?-1:1));
            cacheIndex=(uint16_t)(CachedMapOriginIndex+cell);
            tile=CombatMap[cacheIndex];
            /* Second comparison is unsigned JC, unlike the first signed JG. */
            if (tile>=BlockingTileCodeThreshold) allowed=FALSE;
        }
        if (allowed && (int16_t)actorOrProbeId<MovementProbeFlag) {
            uint16_t savedMapX=CrescentHawkMapPositionX,savedMapY=CrescentHawkMapPositionY;
            CrescentHawkMapPositionX=candidateX; CrescentHawkMapPositionY=candidateY;
            if (Combat_Check_Occupancy_And_Crush(actorOrProbeId,stepX,stepY,FALSE)) {
                allowed=FALSE;
                if (candidateX==targetX && candidateY==targetY) CombatDestinationBlocked=TRUE;
            }
            CrescentHawkMapPositionX=savedMapX; CrescentHawkMapPositionY=savedMapY;
        }
        if (allowed) {
            MovementActorPositionX=candidateX; MovementActorPositionY=candidateY;
            CombatantMovementDirection[facingSlot]=(uint8_t)candidateFacing;
            CombatChosenStepX=stepX; CombatChosenStepY=stepY;
            return;
        }
    }
}
