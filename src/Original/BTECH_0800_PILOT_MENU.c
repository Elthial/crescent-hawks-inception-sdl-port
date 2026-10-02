#include "game.h"
#include "dos.h"

/* Original0800:4D57..4DC6, complete ASM checked. The row-major3x3 terrain
 * neighbourhood is already populated by map-cache addressing. Every entry
 * is tested; no centre-only shortcut. Shared assignment mode0 synchronizes
 * the existing pilot/rider slots before entering its original UI. */
void Menu_Assign_Pilots(void)
{
    uint16_t dismountAllowed=TRUE;
    for(uint16_t neighbour=0;neighbour<MapNeighbourhoodCount;++neighbour)
        if(LocalTerrainFlags[neighbour]&MapTile_BlockDismounting) dismountAllowed=FALSE;
    if(dismountAllowed) {
        Assign_Pilot_and_rider_to_Mechs(AssignmentMode_SynchronizeExisting);
        Draw_Health_and_C_Bills_Sidebar(TRUE);
        Draw_Top_Graphic_Sidebar(); /* Extra original redraw, not a duplicate to eliminate. */
    } else {
        Draw_Message_Box();
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput(
            (uint8_t *)"It would be dangerous to dismount so close to a populated area."); /*3EDB:0ACE*/
        (void)Keyboard_Get_ASCII_Hex_Input();
    }
}
