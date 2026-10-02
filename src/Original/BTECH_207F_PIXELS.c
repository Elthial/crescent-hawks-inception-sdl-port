#include "game.h"
#include "../SDL/backend.h"

/* Sol: Original207F:05D0, staging preserved.0637..067C hardware body is
 * replaced in SDL, including native BYTE initial-Y multiply and first-write
 * before the signed endpoint test. Other adapter bodies remain absent. */
void EGA_Draw_Vertical_Pixel_Run(uint16_t column,uint16_t top,uint16_t bottom,uint16_t colour)
{
    EgaPrimitiveColumn=column;
    EgaPrimitiveTop=top;
    EgaPrimitiveBottom=bottom;
    EgaPrimitiveColour=colour;
    SDLBackend_DrawEgaVerticalRun(column,top,bottom,(uint8_t)colour);
}

/* Sol: Original207F:0780 staging and07D8..080B EGA hardware redirect.
 * Zero group count is an unguarded LOOP:65536 byte writes, not no-op. */
void EGA_Draw_Aligned_Span(uint16_t column,uint16_t row,uint16_t groupCount,uint16_t colour)
{
    EgaPrimitiveColumn=column;
    EgaPrimitiveTop=row;
    EgaPrimitiveGroupCount=groupCount;
    EgaPrimitiveColour=colour;
    SDLBackend_DrawEgaAlignedSpan(column,row,groupCount,(uint8_t)colour);
}
