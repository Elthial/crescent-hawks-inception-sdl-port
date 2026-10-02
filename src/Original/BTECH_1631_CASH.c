#include "game.h"

/* Sol: original1631:1FDF. Expanded EXE3EDB:55C8 stores2892, relocated
 * to3092 in the analysis image. Contrary to the old audit note, the final
 * WORD write restores TextColour, not a byte pair in3EDB dialogue data. */
void Display_Text_CBill_Balance(void)
{
    enum { CashMenuLayout=3,CashDisplayRow=9,CashDisplayMinimumWidth=10 };
    uint16_t previousLayout=CurrentMenuLayoutIndex;
    Menu_Memory_Variables(CashMenuLayout);
    TextColumn=0;TextRow=CashDisplayRow;
    Display_Text_From_Memory((uint8_t *)"\006\017C-Bills:\r"); /*3EDB:32DA*/
    Set_Text_Colour_Bright_Green();
    CBill_Text_Formatting(CBills,DynamicString,NumericRadix_Decimal);
    uint16_t length=Loop_Until_TextPtr_Null(DynamicString);
    while((int16_t)length<CashDisplayMinimumWidth) DynamicString[length++]=' ';
    DynamicString[length]=0;
    Display_Text_From_Memory(DynamicString);
    TextColour=EGA_BrightWhite;
    Menu_Memory_Variables(previousLayout);
}
