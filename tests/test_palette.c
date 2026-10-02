#include "game.h"
#include "dos.h"
#include <assert.h>
#include <stdio.h>
uint16_t VideoStatusInactiveLevel;
static uint16_t values[EgaPaletteRegisterCount],calls,waits;
void Wait_For_Retrace(uint8_t phase)
{ assert(phase==0x23 && calls==0); ++waits; }
void EGA_Set_Palette_Registers_in_out(uint16_t index,uint16_t colour)
{ assert(waits==1 && index==calls); values[calls++]=colour; }
int main(void)
{
    uint8_t palette[EgaPaletteRegisterCount];
    VideoStatusInactiveLevel=0x8123; /* Native0B40 uses lowBYTE despite WORD storage. */
    for (unsigned first=0;first<256;++first) {
        calls=waits=0;
        for (unsigned i=0;i<EgaPaletteRegisterCount;++i) palette[i]=(uint8_t)(first+i);
        Set_Palette_registers(palette);
        assert(calls==EgaPaletteRegisterCount && waits==1);
        for (unsigned i=0;i<EgaPaletteRegisterCount;++i) {
            int16_t expected=(int8_t)palette[i];
            if (expected>7) expected+=8;
            assert(values[i]==(uint16_t)expected);
        }
    }
    puts("Original palette: all BYTE values, signed adjustment and call ordering passed.");
    return 0;
}
