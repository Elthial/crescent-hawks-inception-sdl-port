#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
static void glyphMatches(uint8_t glyph,uint16_t row,uint16_t column)
{
    for (uint16_t y=0;y<8;++y)
        for (uint8_t plane=0;plane<4;++plane) {
            uint8_t mask=EmbeddedEgaFont[glyph*8+y];
            uint8_t expected=(uint8_t)(((9&(1<<plane))?mask:0)|((3&(1<<plane))?(uint8_t)~mask:0));
            assert(SDLBackend_ReadEgaPlaneByte(0xA000,(uint16_t)(row*320+y*40+column),plane)==expected);
        }
}
int main(void)
{
    Set_Text_Colours(0xAA19,0xBB23);
    assert(EgaFontForeground==0x19 && EgaFontBackground==0x23);
    for (uint16_t glyph=0;glyph<128;++glyph) {
        EGA_Draw_Glyph((uint16_t)(glyph*8),0,0);
        glyphMatches((uint8_t)glyph,0,0);
        assert(SDLBackend_EgaWriteMode==2 && SDLBackend_EgaBitMask==(uint8_t)~EmbeddedEgaFont[glyph*8+7]);
    }
    uint8_t text[]="AB\rC";
    Draw_EGA_Text_To_Screen(text,39,0,9,3);
    assert(TextAutoWrapLineCount==1);
    glyphMatches('A',0,39); glyphMatches('B',1,39); glyphMatches('C',2,39);
    uint8_t highText[]={0xC1,10,0};
    Draw_EGA_Text_To_Screen(highText,0,3,9,3);
    glyphMatches('A',3,0); glyphMatches(10,3,1);
    assert(TextAutoWrapLineCount==1);
    uint8_t empty[]={0};
    SDLBackend_EgaWriteMode=1;
    Draw_EGA_Text_To_Screen(empty,0,0,0xABCD,0x0123);
    assert(EgaFontForeground==0xCD && EgaFontBackground==0x23 && SDLBackend_EgaWriteMode==1);
    Set_Text_Colours(9,3);
    EGA_Draw_Glyph('A'*8,0,0);
    SDLBackend_EgaMapMask=0x0D; SDLBackend_EgaRasterOperation=EgaRaster_Xor;
    EGA_Draw_Glyph('A'*8,0,0);
    for (uint16_t y=0;y<8;++y) {
        for (uint8_t plane=0;plane<4;++plane) {
            uint8_t expected=plane==1?(uint8_t)~EmbeddedEgaFont['A'*8+y]:0;
            assert(SDLBackend_ReadEgaPlaneByte(0xA000,(uint16_t)(y*40),plane)==expected);
        }
    }
    assert(SDLBackend_EgaMapMask==0x0D && SDLBackend_EgaRasterOperation==EgaRaster_Xor);
    puts("Original text/font:128 embedded glyphs, wrap/CR, high-bit/LF and empty-string state passed.");
    return 0;
}
