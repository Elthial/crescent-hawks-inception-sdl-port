#include "game.h"

/* Sol: Actual EXE-owned305B:0000..008F. Nine eight-WORD records, not new
 * host UI layouts. Foreground/background/cursor are mutable per-panel state. */
MenuPanelLayout MenuPanelLayouts[MenuPanelLayoutCount]={
    {0,0,40,25,15,0,0,0},
    {14,15,25,9,15,0,0,0},
    {1,1,38,23,15,0,0,0},
    {1,13,11,11,15,0,0,0},
    {1,1,11,11,15,0,0,0},
    {10,1,29,16,15,0,0,0},
    {14,1,25,23,15,0,0,0},
    {14,10,25,5,15,0,0,0},
    {0,0,16,25,15,0,0,0}
};
uint16_t MenuPanelContextInitialized; /* actual initial WORD0 */

/* Sol: Original1E56:0281..0387, complete raw ASM checked. First call skips
 * outgoing save; selecting the SAME layout still saves then restores it.
 * Original geometry is never saved here. Valid native layout IDs0..8 are the
 * host contract; no invented bounds rejection or FAR-memory helper is used. */
void Menu_Memory_Variables(uint16_t index)
{
    if (MenuPanelContextInitialized != 0) {
        MenuPanelLayout *previous=&MenuPanelLayouts[CurrentMenuLayoutIndex];
        previous->foreground=TextColour;
        previous->background=TextBackgroundColour;
        previous->column=TextColumn;
        previous->row=TextRow;
    } else MenuPanelContextInitialized=1;
    CurrentMenuLayoutIndex=index;
    MenuPanelLayout *panel=&MenuPanelLayouts[index];
    TextPanelLeft=panel->left;
    TextPanelTop=panel->top;
    TextPanelWidth=panel->width;
    TextPanelHeight=panel->height;
    TextColour=panel->foreground;
    TextBackgroundColour=panel->background;
    TextColumn=panel->column;
    TextRow=panel->row;
}
