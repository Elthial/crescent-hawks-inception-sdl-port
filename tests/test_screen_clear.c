#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t packed[PackedGraphicsOutputBytes];
int main(void)
{
    for (unsigned operation=0;operation<4;++operation) {
        SDLBackend_EgaMapMask=15; SDLBackend_EgaRasterOperation=EgaRaster_Replace;
        memset(packed,0xFF,sizeof packed);
        DrawCall_Image_To_VGA_Memory(packed,0xA000);
        DrawCall_Image_To_VGA_Memory(packed,0xA1F4); /* next8000 byte range */
        DrawCall_Image_To_VGA_Memory(packed,0xA3E8); /* range beginning at clear end */
        memset(packed,0x55,sizeof packed);
        DrawCall_Image_To_VGA_Memory(packed,0xA800);
        uint8_t captured[32]; DrawCall_Read_EGAMemory(captured,0,0); /* hardware read loads all latches */
        SDLBackend_EgaRasterOperation=(uint8_t)operation; SDLBackend_EgaMapMask=0x0B;
        SDLBackend_EgaRotateCount=5;
        Graphics_Set_Screen_To_Black();
        assert(SDLBackend_EgaWriteMode==2 && SDLBackend_EgaBitMask==255);
        assert(SDLBackend_EgaRasterOperation==operation && SDLBackend_EgaMapMask==0x0B && SDLBackend_EgaRotateCount==5);
        for (unsigned offset=0;offset<16000;++offset)
            for (uint8_t plane=0;plane<4;++plane) {
                uint8_t expected=plane==2?255:0; /* disabled plane remains incoming FF */
                if ((operation==EgaRaster_Or || operation==EgaRaster_Xor) && plane==0) expected=255;
                assert(SDLBackend_ReadEgaPlaneByte(0xA000,(uint16_t)offset,plane)==expected);
            }
        for (uint8_t plane=0;plane<4;++plane)
            assert(SDLBackend_ReadEgaPlaneByte(0xA000,0x4000,plane)==255);
    }
    /* Upload leaves latches at its last READ, before final bit01 write. */
    SDLBackend_EgaMapMask=15; SDLBackend_EgaRasterOperation=EgaRaster_Replace;
    memset(packed,0,sizeof packed); DrawCall_Image_To_VGA_Memory(packed,0xA800);
    memset(packed,0xFF,sizeof packed); DrawCall_Image_To_VGA_Memory(packed,0xA800);
    SDLBackend_EgaRasterOperation=EgaRaster_Or;
    Graphics_Set_Screen_To_Black();
    for (uint8_t plane=0;plane<4;++plane) assert(SDLBackend_ReadEgaPlaneByte(0xA000,0,plane)==0xFE);
    puts("Original8000-WORD screen clear preserves map mask/ROP and frozen EGA latches");
    return 0;
}
