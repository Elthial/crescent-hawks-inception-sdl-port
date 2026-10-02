#include "game.h"
#include "../SDL/backend.h"

/* Original207F:2127 retained EGA branch: foreground/background BYTEs. */
void Set_Text_Colours(uint16_t foreground,uint16_t background)
{
    EgaFontForeground=(uint8_t)foreground;
    EgaFontBackground=(uint8_t)background;
}
/* Original207F:2251..22A4, full ASM checked. Hardware body redirects. */
void EGA_Draw_Glyph(uint16_t glyphOffset,uint16_t rowByteOffset,uint16_t column)
{
    SDLBackend_DrawEgaGlyph(EmbeddedEgaFont+glyphOffset,rowByteOffset,column,
        EgaFontForeground,EgaFontBackground);
}
/* Original1F3D:00D5..01FA retained EGA loop, full raw ASM checked.
 * CR resets to INITIAL column, not zero. Only automatic wrap increments4FBE.
 * High-bit glyph bytes select their low7 bits; LF is an ordinary glyph.
 * Valid strings/coordinates and intended FAR-offset-not-wrapping contract. */
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t column,uint16_t row,
    uint16_t foreground,uint16_t background)
{
    Set_Text_Colours(foreground,background);
    uint16_t initialColumn=column;
    uint16_t rowByteOffset=(uint16_t)(TextCellRowByteStride*row);
    while (*text!=0) {
        if (*text=='\r') {
            column=initialColumn;
            rowByteOffset+=TextCellRowByteStride;
            ++text;
        } else {
            uint16_t glyphOffset=(uint16_t)((*text++&FontGlyphIndexMask)*FontGlyphRows);
            if ((int16_t)column>(int16_t)(uint16_t)(TextScreenColumnCount-1)) {
                column=initialColumn;
                ++TextAutoWrapLineCount;
                rowByteOffset+=TextCellRowByteStride;
            }
            EGA_Draw_Glyph(glyphOffset,rowByteOffset,column++);
        }
    }
}
