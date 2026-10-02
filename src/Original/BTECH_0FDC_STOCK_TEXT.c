#include "game.h"

/* Original0FDC:19E1..19F5. Leading space and final period complete
 * the caller's "You invest" or "You sell" prefix. EXE-owned3EDB:1791. */
void Display_No_Stock_Transaction_Text(void)
{
    Display_Text_From_Memory((uint8_t *)" nothing this time.");
}
