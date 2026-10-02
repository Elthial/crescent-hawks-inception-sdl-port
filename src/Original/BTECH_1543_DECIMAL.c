#include "game.h"

/* Original1543:0CDE..0EE5. Sol: Edit the shared decimal scratch string, clearing
 * the remainder of its display row after each redraw. Escape resets rather
 * than cancels, Backspace permits empty input, Enter accepts. */
uint32_t Prompt_For_Unsigned_Decimal(void)
{
    uint8_t *input=DynamicString;
    input[0]='0'; input[1]=0;
    uint16_t savedColumn=TextColumn,savedRow=TextRow;
    TextColour=GraphicsAdapter==0?EGA_Blue:EGA_Green;
    uint16_t accepted=FALSE;
    do {
        TextColumn=savedColumn; TextRow=savedRow;
        Display_Text_From_Memory(input);
        uint16_t clearTop=(uint16_t)((uint16_t)(TextPanelTop+TextRow)*NumericEntryCellPixels);
        uint16_t clearRight=(uint16_t)((uint16_t)(TextPanelLeft+TextPanelWidth)*NumericEntryCellPixels-1);
        uint16_t clearLeft=(uint16_t)((uint16_t)(TextPanelLeft+TextColumn)*NumericEntryCellPixels);
        Draw_Horizontal_EGA_Line(clearLeft,clearTop,clearRight,
            (uint16_t)(clearTop+NumericEntryCellPixels-1),0);
        int16_t key=(int16_t)Keyboard_Get_ASCII_Hex_Input();
        if(key==NumericEntryEnter) accepted=TRUE;
        else {
            uint16_t length=Loop_Until_TextPtr_Null(input);
            if(key>='0' && key<='9') {
                if((int16_t)length<NumericEntryMaximumDigits) {
                    while(input[0]=='0') Append_Large_Text_To_Memory(input,input+1);
                    length=Loop_Until_TextPtr_Null(input);
                    input[length]=(uint8_t)key; input[length+1]=0;
                }
            } else {
                if(key==NumericEntryEscape) { input[0]='0'; input[1]=0; }
                if(key==NumericEntryBackspace && length!=0) input[length-1]=0;
            }
        }
    } while(accepted==FALSE);
    uint32_t value=0;
    for(uint16_t digit=0;input[digit]!=0;++digit)
        value=value*NumericRadix_Decimal+(uint32_t)(int32_t)(int8_t)input[digit]-'0';
    TextColour=EGA_BrightWhite;
    return value;
}
