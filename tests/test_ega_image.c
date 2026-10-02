#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>

static uint8_t packed[PackedGraphicsOutputBytes];
int main(void)
{
    static const uint16_t segments[]={0xA000,0xA400,0xA800};
    for (unsigned pass=0;pass<3;++pass) {
        for (unsigned i=0;i<sizeof(packed);++i) packed[i]=(uint8_t)(i+pass*37);
        DrawCall_Image_To_VGA_Memory(packed,segments[pass]);
        assert(SDLBackend_EgaWriteMode==2 && SDLBackend_EgaBitMask==0);
        for (uint16_t column=0;column<EgaScreenPlaneBytes;++column) {
            for (unsigned pixel=0;pixel<8;++pixel) {
                uint8_t colour=0;
                for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
                    if (SDLBackend_ReadEgaPlaneByte(segments[pass],column,plane)&(0x80>>pixel))
                        colour|=(uint8_t)(1<<plane);
                uint8_t expected=packed[column*4+pixel/2];
                expected=(uint8_t)((pixel&1)?expected&15:expected>>4);
                assert(colour==expected);
            }
        }
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
            assert(SDLBackend_ReadEgaPlaneByte(segments[pass],EgaScreenPlaneBytes,plane)==0);
            uint16_t apertureOffset=(uint16_t)((segments[pass]-EgaApertureSegment)*16);
            assert(SDLBackend_ReadEgaPlaneByte(EgaApertureSegment,apertureOffset,plane)==
                SDLBackend_ReadEgaPlaneByte(segments[pass],0,plane));
        }
    }
    /*0260 does not reset inherited map-mask or raster-operation registers. */
    SDLBackend_EgaMapMask=0x0A;
    SDLBackend_EgaRasterOperation=EgaRaster_Xor;
    DrawCall_Image_To_VGA_Memory(packed,0xA800);
    for (uint16_t column=0;column<EgaScreenPlaneBytes;++column) {
        assert(SDLBackend_ReadEgaPlaneByte(0xA800,column,1)==0);
        assert(SDLBackend_ReadEgaPlaneByte(0xA800,column,3)==0);
        for (uint8_t plane=0;plane<EgaPlaneCount;plane+=2) {
            uint8_t expected=0;
            for (unsigned pixel=0;pixel<8;++pixel) {
                uint8_t colour=packed[column*4+pixel/2];
                colour=(uint8_t)((pixel&1)?colour&15:colour>>4);
                if (colour&(1<<plane)) expected|=(uint8_t)(0x80>>pixel);
            }
            assert(SDLBackend_ReadEgaPlaneByte(0xA800,column,plane)==expected);
        }
    }
    assert(SDLBackend_EgaMapMask==0x0A && SDLBackend_EgaRasterOperation==EgaRaster_Xor);
    uint8_t captured[34]; captured[0]=captured[33]=0xCC;
    DrawCall_Read_EGAMemory(captured+1,39,24);
    for (uint16_t row=0;row<8;++row)
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
            assert(captured[1+row*4+plane]==
                SDLBackend_ReadEgaPlaneByte(0xA800,(uint16_t)(24*320+39+row*40),plane));
    assert(captured[0]==0xCC && captured[33]==0xCC);
    assert(SDLBackend_EgaWriteMode==2 && SDLBackend_EgaBitMask==0 && SDLBackend_EgaReadMapSelect==3);
    puts("Original EGA transfer: all pixels, all planes and shared segment views passed.");
    return 0;
}
