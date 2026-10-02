#include "game.h"
#include "dos.h"

/* Sol: Original1F3D:0525..05BB retained EGA body055D..059E.
 * CBW before signed JLE7 is meaningful: high-bit BYTEs are negative and do
 * not receive the+8 adjustment.022A later consumes the low colour BYTE. */
void Set_Palette_registers(const uint8_t *palette)
{
    Wait_For_Retrace((uint8_t)VideoStatusInactiveLevel);
    for (uint16_t index=0;index<EgaPaletteRegisterCount;++index) {
        int16_t colour=(int8_t)palette[index];
        if (colour>=EgaPaletteFirstHighColour) colour+=EgaPaletteHighColourAdjustment;
        EGA_Set_Palette_Registers_in_out(index,(uint16_t)colour);
    }
}
