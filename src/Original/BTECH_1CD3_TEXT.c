#include "game.h"

/* Original1CD3:181E..1833. DS4FA0 is inline0D,00, not a FAR pointer slot. */
void Display_Text_4FA0_Value(void)
{
    Display_Text_From_Memory((uint8_t *)"\r");
}
