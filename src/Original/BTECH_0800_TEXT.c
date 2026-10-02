#include "game.h"

/* 0800:2867. Sol: Set native cursor WORDs and bright-white foreground,
 * then invoke the original text renderer. No colour/cursor restore exists. */
void Display_Text_At(uint8_t *text, uint16_t column, uint16_t row)
{
    TextColumn = column;
    TextRow = row;
    TextColour = EGA_BrightWhite;
    Display_Text_From_Memory(text);
}
/* 0800:28A2 maintained EGA path. Sol: Select bright-green foreground.
 * The deleted CGA palette substitution is not reintroduced. */
void Set_Text_Colour_Bright_Green(void)
{
    TextColour = EGA_BrightGreen;
}
