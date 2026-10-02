#include "game.h"

/* Original183B:2835..28DA. Native FAR slot pointers0170 address nine
 * sequential64-byte descriptor blocks0564..0764; first descriptor90 means
 * populated, NOT a matching map number. Always rebuild the combined cache.
 * Absent/out-of-world slots are not cleared. Region crossing is linear, not
 * clamped by columns at world edges. Map BYTE is CBW before the loader. */
void Combat_Load_9Grid_Map(void)
{
    int16_t topLeftRegion=(int16_t)((uint16_t)(CrescentHawkMapPositionX|CrescentHawkMapPositionY)>>8);
    topLeftRegion=(int16_t)(topLeftRegion-MapRegionPreviousRowAndColumn);
    for(int16_t row=0;row<MapNeighbourhoodWidth;++row) {
        for(int16_t column=0;column<MapNeighbourhoodWidth;++column) {
            int16_t region=(int16_t)(topLeftRegion+column);
            if(region>=0 && region<WorldRegionCount && MapFileByWorldRegion[region]) {
                uint16_t slot=(uint16_t)(row*MapNeighbourhoodWidth+column);
                if(MapDescriptorCache[slot*MapBlockTileCount]!=MapFileFirstBlockDescriptor)
                    DOS_Load_Map_Files(slot,(uint16_t)(int16_t)(int8_t)MapFileByWorldRegion[region]);
            }
        }
        topLeftRegion=(int16_t)(topLeftRegion+MapRegionRowStride);
    }
    Map_NineGrid_Parent();
}

/* Original183B:28DB..2AA2. First active enemy, NOT nearest/all. Probes
 * at most30 movement attempts using special80 actor (no ordinary collision),
 * beginning at Jason or his assigned Mech. Already at target returns0;
 * only arrival AFTER a step returns1. Restores camera/cache, NOT scratch
 * working position, budget, facing or sticky destination-blocked state. */
uint16_t Combat_Assess_FirstEnemy_Reachability(void)
{
    uint16_t savedMapX=CrescentHawkMapPositionX,savedMapY=CrescentHawkMapPositionY;
    uint16_t startX=CombatantPackedX[Friendly_Infantry_Combatant_Range_First];
    uint16_t startY=CombatantPackedY[Friendly_Infantry_Combatant_Range_First];
    if(Characters[Character_Jason].mechAssignment!=Character_OnFoot) {
        int16_t mechId=(int8_t)Characters[Character_Jason].mechAssignment;
        startX=CombatantPackedX[mechId]; startY=CombatantPackedY[mechId];
    }
    Move_Map_View_To_Packed_Position(startX,startY);
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY); Update_Cached_Map_Origin();
    MovementActorPositionX=CrescentHawkMapPositionX; MovementActorPositionY=CrescentHawkMapPositionY;
    uint16_t foundThenReached=FALSE,targetX=0,targetY=0;
    /* Initialized target locals are never read without a found enemy; unlike
     * native unspecified return bugs, their cold values have no effect. */
    int16_t screenX=CombatPreviewOriginColumn,screenY=CombatPreviewOriginRow;
    for(uint16_t id=Enemy_All_CombatantId_Range_First;id<AllCombatantCount;++id)
        if(!foundThenReached && CombatantActive[id]) {
            targetX=CombatantPackedX[id]; targetY=CombatantPackedY[id]; foundThenReached=TRUE;
        }
    CharacterMovementPointsRemaining=CombatEncounterProbeMaximumSteps;
    if(foundThenReached) {
        foundThenReached=FALSE;
        while((int16_t)CharacterMovementPointsRemaining>0) {
            if(MovementActorPositionX==targetX && MovementActorPositionY==targetY) break;
            Movement_Select_Next_Step(MovementProbeFlag,targetX,targetY,(uint16_t)screenX,(uint16_t)screenY,FALSE);
            screenX=(int16_t)(uint16_t)(screenX+CombatChosenStepX); screenY=(int16_t)(uint16_t)(screenY+CombatChosenStepY);
            --CharacterMovementPointsRemaining;
            if(MovementActorPositionX==targetX && MovementActorPositionY==targetY) {
                CharacterMovementPointsRemaining=0; foundThenReached=TRUE;
            }
        }
    }
    Combat_Character_Pos_Grid(savedMapX,savedMapY);
    return foundThenReached;
}

/* Original183B:2AA3..2ADB, .dis and EXE tail confirm207F:1DF8/return.
 * View/cache only: no framebuffer copy or actor/scratch-state restoration. */
void Combat_Character_Pos_Grid(uint16_t packedX,uint16_t packedY)
{
    Move_Map_View_To_Packed_Position(packedX,packedY);
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY); Update_Cached_Map_Origin();
}
