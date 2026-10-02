#include "game.h"

/* Sol: Original0800:218F..231C, complete ASM checked. WORD commands are
 * sign-extended scan codes, not ASCII or unsigned low BYTE values.
 * Drain Y crossings BEFORE X can replace the shared three-entry queue.
 * Final cache expansion/origin update run even when an interaction blocks. */
void Character_Movement_On_Map(uint16_t movementCommand)
{
    int16_t deltaX=0,deltaY=0;
    if (movementCommand==Command_MoveNorth || movementCommand==Command_MoveNorthEast ||
        movementCommand==Command_MoveNorthWest) deltaY=-1;
    if (movementCommand==Command_MoveSouth || movementCommand==Command_MoveSouthEast ||
        movementCommand==Command_MoveSouthWest) deltaY=1;
    if (movementCommand==Command_MoveEast || movementCommand==Command_MoveNorthEast ||
        movementCommand==Command_MoveSouthEast) deltaX=1;
    if (movementCommand==Command_MoveWest || movementCommand==Command_MoveNorthWest ||
        movementCommand==Command_MoveSouthWest) deltaX=-1;
    if (Map_Interactables_Building_Or_Items(deltaX,deltaY)==FALSE) {
        if (deltaY==-1) Map_Move_North();
        if (deltaY==1) Map_Move_South();
        for (uint16_t entry=0;entry<MapNeighbourhoodWidth;++entry) {
            uint8_t slot=PendingMapGridSlot[entry];
            if (slot!=PendingMapSlot_Unused) {
                uint8_t mapFile=MapFileByWorldRegion[PendingMapRegionIndex[entry]];
                if (mapFile!=0) {
                    DOS_Load_Map_Files(slot,(uint16_t)(int16_t)(int8_t)mapFile);
                    Map_NineGrid_Parent();
                }
                PendingMapGridSlot[entry]=PendingMapSlot_Unused;
            }
        }
        if (deltaX==1) Map_Move_East();
        if (deltaX==-1) Map_Move_West();
        for (uint16_t entry=0;entry<MapNeighbourhoodWidth;++entry) {
            uint8_t slot=PendingMapGridSlot[entry];
            if (slot!=PendingMapSlot_Unused) {
                uint8_t mapFile=MapFileByWorldRegion[PendingMapRegionIndex[entry]];
                if (mapFile!=0) {
                    DOS_Load_Map_Files(slot,(uint16_t)(int16_t)(int8_t)mapFile);
                    Map_NineGrid_Parent();
                }
                PendingMapGridSlot[entry]=PendingMapSlot_Unused;
            }
        }
    }
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    Update_Cached_Map_Origin();
}
