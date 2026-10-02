#include "game.h"

/* Sol: Original1631:1BFE..1DAA. AX1 clear/0 terrain-blocked, opposite
 * polarity to the separate occupancy routine. Native cache/caller contract;
 * no added bounds check, pathfinder or iteration limit. */
uint16_t Combat_Check_Terrain_Path(uint16_t actorId,uint16_t targetId,
    uint16_t targetX,uint16_t targetY)
{
    (void)actorId; /* Native first argument is never read. */
    uint16_t pathX=CrescentHawkMapPositionX, pathY=CrescentHawkMapPositionY;
    PosXY_OffsetGrid(pathX,pathY);
    Update_Cached_Map_Origin();
    if (ArenaRentalMechMode!=FALSE && targetId==ArenaRentalPathTargetId) return TRUE;
    uint16_t cacheIndex=(uint16_t)(CachedMapOriginIndex+CombatPathCentreOffset);
    int16_t xParity=1, yParity=pathY&1;
    if (pathX&1) { ++cacheIndex; xParity=0; }
    uint16_t heading=(uint16_t)Get_Target_Compass_Direction(pathX,pathY,targetX,targetY);
    uint16_t clearPath=TRUE, tile=CombatMap[cacheIndex];
    if ((int16_t)BlockingTileCodeThreshold<=(int16_t)tile) return FALSE;
    while (pathX!=targetX || pathY!=targetY) {
        uint16_t desiredHeading=(uint16_t)Get_Target_Compass_Direction(pathX,pathY,targetX,targetY);
        int16_t difference=(int16_t)(desiredHeading-heading);
        if (difference!=0) {
            difference&=CompassDirectionMask;
            heading=(uint16_t)((heading+(difference<=CompassClockwiseTurnLastDifference?1:-1))&CompassDirectionMask);
        }
        int16_t stepX=CombatPathStepX[heading], stepY=CombatPathStepY[heading];
        pathX=(uint16_t)(pathX+stepX);
        if (pathX&PackedPositionLocalCarryBit) pathX=(uint16_t)(pathX+CombatPathBoundaryX[heading]);
        pathY=(uint16_t)(pathY+stepY);
        if (pathY&PackedPositionLocalCarryBit) pathY=(uint16_t)(pathY+CombatPathBoundaryY[heading]);
        if (stepX!=0) {
            xParity=(int16_t)((xParity+stepX)&1);
            if (xParity==0) cacheIndex=(uint16_t)(cacheIndex+stepX);
        }
        if (stepY!=0) {
            yParity=(int16_t)((yParity+stepY)&1);
            if (yParity==0) cacheIndex=(uint16_t)(cacheIndex+CombatPathCacheRowDelta[heading]);
        }
        tile=CombatMap[cacheIndex];
        if ((int16_t)BlockingTileCodeThreshold<=(int16_t)tile) {
            pathX=targetX; pathY=targetY; clearPath=FALSE;
        }
    }
    return clearPath;
}
