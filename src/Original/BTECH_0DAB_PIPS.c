#include "game.h"

/* Original0DAB:1858..18E7, full ASM checked. Retained EGA colours.
 * Healthy count is the FIRST red pip, not a number of damaged components.
 * Only index5 wraps the five-column run: larger totals do not wrap again.
 * Native JL compares signed WORDs; totals8000..FFFF draw nothing. */
void Draw_Component_Status_Pips(uint16_t column,uint16_t row,uint16_t totalPips,uint16_t healthyPips)
{
    uint16_t colour=EGA_Green;
    for(uint16_t pip=0;(int16_t)pip<(int16_t)totalPips;++pip) {
        if(pip==ComponentPipsFirstRowCount) { column-=ComponentPipsFirstRowCount; ++row; }
        if(pip==healthyPips) colour=EGA_Red;
        uint16_t x=(uint16_t)(column*ComponentPipCellWidth);
        uint16_t y=(uint16_t)(row*FontGlyphRows);
        Draw_Horizontal_EGA_Line((uint16_t)(x+2),(uint16_t)(y+3),
            (uint16_t)(x+4),(uint16_t)(y+5),colour);
        ++column;
    }
}
