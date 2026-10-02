#include "game.h"

/* Sol: Original1E56:07CB..0A3A, retained EGA branch. Text is mutable:
 * every exit consumes its first byte. Scroll saves the original string length
 * BEFORE padding/truncation; advancing a scrolled line increments the incoming
 * row rather than clamping it. Neither quirk is corrected for preservation. */
void Display_Text_In_TextBox(uint8_t *text,uint16_t advanceLine)
{
    uint16_t textLength;
    if ((int16_t)TextRow < (int16_t)TextPanelHeight) {
        Draw_EGA_Text_To_Screen(text,(uint16_t)(TextColumn+TextPanelLeft),
            (uint16_t)(TextRow+TextPanelTop),TextColour,TextBackgroundColour);
        if (!advanceLine) {
            textLength=Loop_Until_TextPtr_Null(text);
            TextColumn=(uint16_t)(TextColumn+textLength);
        }
    } else {
        textLength=Loop_Until_TextPtr_Null(text);
        uint16_t paddingIndex=textLength;
        while ((int16_t)TextPanelWidth > (int16_t)paddingIndex)
            text[paddingIndex++]=' ';
        text[TextPanelWidth]=0;
        EgaMemoryAddress scrollSource={TextCellRowByteStride,EgaScreenSegment};
        EgaMemoryAddress screen={0,EgaScreenSegment};
        EGA_DrawBox_Operation(scrollSource,screen,TextPanelLeft,
            (uint16_t)(TextPanelTop*FontGlyphRows),TextPanelWidth,
            (uint16_t)(TextPanelHeight*FontGlyphRows-FontGlyphRows));
        uint16_t interiorBottom=(uint16_t)(TextPanelTop+TextPanelHeight);
        uint16_t interiorRight=(uint16_t)(TextPanelLeft+TextPanelWidth);
        /* Native right endpoint is (left+width)*8-8, not last pixel-1. */
        Draw_Horizontal_EGA_Line((uint16_t)(TextPanelLeft*FontGlyphRows),
            (uint16_t)(interiorBottom*FontGlyphRows-FontGlyphRows),
            (uint16_t)(interiorRight*FontGlyphRows-FontGlyphRows),
            (uint16_t)(interiorBottom*FontGlyphRows-1),0);
        Draw_EGA_Text_To_Screen(text,TextPanelLeft,(uint16_t)(interiorBottom-1),
            TextColour,TextBackgroundColour);
        if (!advanceLine) {
            TextColumn=(uint16_t)(TextColumn+textLength);
            TextRow=(uint16_t)(TextPanelHeight-1);
        }
    }
    if (advanceLine || (int16_t)TextColumn >= (int16_t)TextPanelWidth) {
        TextColumn=0;
        ++TextRow;
    }
    text[0]=0;
}
