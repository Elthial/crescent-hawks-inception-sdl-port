#include "game.h"
#include "../SDL/backend.h"

/* Sol: Original207F:2B87 retained EGA staging2BB0..2BD6 and2CB0..2CDE,
 * shared2C7B exit. Highlight uses byte-column/width and eight-pixel cell row.
 * Hardware body is redirected, retaining shared rectangle scratch. */
uint8_t DrawCall_Combat_Menu(uint16_t column,uint16_t row,uint16_t width,uint16_t colour)
{
    FramebufferBoxColumn=column;
    FramebufferBoxWidth=width;
    SDLBackend_ToggleEgaHighlight(column,row,width,(uint8_t)colour);
    /* Native final OUT leaves AX0003; shared exit does not return BX colour. */
    return 3;
}
