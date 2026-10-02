#include "game.h"

/* Sol: Complete original1CD3:17C6..17E9, ASM checked. No input wait. */
void Draw_Message_Box(void)
{
    enum { StandardMessagePanel=7,PlainBorder=0 };
    Menu_Memory_Variables(StandardMessagePanel);
    Draw_Top_Graphic_Sidebar();
    Draw_Menu_Border(PlainBorder);
}

/* Sol: Complete original1CD3:17EA..1808, ASM checked. Timed input bridge
 * is distinct from the blocking ASCII read performed by callers. */
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text)
{
    Display_Text_From_Memory(text);
    Wait_For_50Hz_Then_Check_Input();
}

/* Sol: Complete original1CD3:1809..181D. Restore the final period from
 * EXE-owned3EDB:4F7E; no extra presentation wait or input call. */
void Display_Text_Shop_Cannot_Afford_Text(void)
{
    Display_Text_From_Memory((uint8_t *)"Come back when you can afford it.");
}
