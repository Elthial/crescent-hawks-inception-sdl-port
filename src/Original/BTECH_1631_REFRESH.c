#include "game.h"

/* Original1631:032F..03AA, complete ASM checked. Rebuild current cached
 * view, draw requested planning preview OR exploration actors, transfer
 * framebuffer, then activate/redraw/border panels4 and3 in that order.
 * The last selected panel is3; the original does not restore caller context. */
void Menu_Draw_MultiSelect(uint16_t drawPlanningPreview)
{
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    Copy_Data_To_GraphicsMemory();
    if(drawPlanningPreview) Draw_Menu_MultiSelect();
    else Draw_Infantry_And_Mechs();
    EGA_DrawBox_Wrapper();
    Menu_Memory_Variables(RefreshLowerPanel);
    Draw_Top_Graphic_Sidebar();
    Draw_Menu_Border(RefreshLowerPanel);
    Menu_Memory_Variables(RefreshPartyPanel);
    Draw_Top_Graphic_Sidebar();
    Draw_Menu_Border(RefreshPartyPanel);
}
