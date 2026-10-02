#include "game.h"

/* Sol: Original1E56:0388..03F4. Clear the active panel interior, not just
 * the top of the sidebar. Origin and extent are eight-pixel text cells;
 * inclusive last pixel and intermediate arithmetic wrap as native WORDs.
 * Border drawing is a separate original method. */
void Draw_Top_Graphic_Sidebar(void)
{
    uint16_t bottom=(uint16_t)(TextPanelTop+TextPanelHeight);
    uint16_t right=(uint16_t)(TextPanelLeft+TextPanelWidth);
    Draw_Horizontal_EGA_Line((uint16_t)(TextPanelLeft*FontGlyphRows),
        (uint16_t)(TextPanelTop*FontGlyphRows),
        (uint16_t)(right*FontGlyphRows-1),
        (uint16_t)(bottom*FontGlyphRows-1),TextBackgroundColour);
    TextRow=0;
    TextColumn=0;
}
