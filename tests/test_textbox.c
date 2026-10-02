#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>

/* All original dialogue, font, rectangle and framebuffer methods execute;
 * only actual hardware writes are redirected to the separate SDL layer. */
static void glyphMatches(uint8_t glyph,uint16_t row,uint16_t column)
{
    for (uint16_t y=0;y<FontGlyphRows;++y)
        for (uint8_t plane=0;plane<4;++plane) {
            uint8_t mask=EmbeddedEgaFont[glyph*FontGlyphRows+y];
            uint8_t expected=(uint8_t)(((9&(1<<plane))?mask:0)|
                ((3&(1<<plane))?(uint8_t)~mask:0));
            assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,
                (uint16_t)(row*TextCellRowByteStride+y*EgaFramebufferRowBytes+column),plane)==expected);
        }
}
int main(void)
{
    /* Activate actual EXE panel7, clear its complete interior, and independently
     * verify all four plane bytes including untouched pixels outside it. */
    Menu_Memory_Variables(7);
    TextBackgroundColour=5; TextColumn=10; TextRow=3;
    Draw_Top_Graphic_Sidebar();
    assert(TextColumn==0 && TextRow==0 && CurrentMenuLayoutIndex==7);
    for (uint16_t row=0;row<EgaScreenHeight;++row)
        for (uint16_t column=0;column<EgaFramebufferRowBytes;++column)
            for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                uint8_t expected=(row>=80 && row<120 && column>=14 && column<39 &&
                    (5&(1<<plane)))?255:0;
                assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,
                    (uint16_t)(row*EgaFramebufferRowBytes+column),plane)==expected);
            }
    Menu_Memory_Variables(4);
    assert(MenuPanelLayouts[7].background==5 && MenuPanelLayouts[7].column==0 &&
        MenuPanelLayouts[7].row==0);
    TextPanelLeft=2; TextPanelTop=1; TextPanelWidth=5; TextPanelHeight=2;
    TextColour=9; TextBackgroundColour=3; TextColumn=1; TextRow=0;
    uint8_t first[]="XY";
    Display_Text_In_TextBox(first,0);
    assert(first[0]==0 && TextColumn==3 && TextRow==0);
    glyphMatches('X',1,3); glyphMatches('Y',1,4);
    TextColumn=0; TextRow=0;
    Display_Text_From_Memory((uint8_t *)"AB\rCD\rE");
    assert(TextColumn==1 && TextRow==1);
    assert(EgaPrimitiveColumn==48 && EgaPrimitiveTop==23 &&
        EgaPrimitiveBottom==23 && EgaPrimitiveColour==0);
    glyphMatches('C',1,2); glyphMatches('D',1,3);
    glyphMatches('E',2,2);
    for (uint16_t column=3;column<7;++column) glyphMatches(' ',2,column);
    assert(FramebufferBoxColumn==2 && FramebufferBoxRow==8 &&
        FramebufferBoxWidth==5 && FramebufferBoxHeight==8);
    assert(GraphicsSourceAddress.ega.offset==TextCellRowByteStride &&
        GraphicsDestinationAddress.ega.offset==0);
    assert(SDLBackend_EgaWriteMode==2);
    /* Advance-line scroll increments incoming row, however far beyond panel. */
    TextRow=7; TextColumn=4;
    uint8_t advanced[40]="Q";
    Display_Text_In_TextBox(advanced,1);
    assert(TextRow==8 && TextColumn==0 && advanced[0]==0);
    glyphMatches('E',1,2); glyphMatches('Q',2,2);
    /* Preserve original length before truncation when updating cursor. */
    TextRow=2; TextColumn=2;
    uint8_t truncated[40]="ABCDEFG";
    Display_Text_In_TextBox(truncated,0);
    assert(TextRow==2 && TextColumn==0);
    assert(truncated[0]==0 && truncated[5]==0 && truncated[6]=='G');
    for (uint16_t column=2;column<7;++column)
        glyphMatches((uint8_t)('A'+column-2),2,column);
    /* The scrolling rectangle never touches the adjacent sidebar byte. */
    for (uint8_t plane=0;plane<4;++plane)
        assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,321,plane)==0);
    puts("Original dialogue, font, cursor and scrolling integration passed");
    return 0;
}
