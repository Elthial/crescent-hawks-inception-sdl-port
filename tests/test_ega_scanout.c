#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t pixels[ScreenWidth*ScreenHeight];
static void check(int condition)
{
    if(!condition) { fputs("EGA scanout mismatch\n",stderr); exit(1); }
}
int main(void)
{
    check(SDLBackend_Open());
    SDLBackend_ClearEgaScreen();
    SDLBackend_EgaRasterOperation=EgaRaster_Replace;
    SDLBackend_EgaRotateCount=0;
    /* Exercise all four bitplanes and every bit within a screen byte. */
    for(uint16_t x=0;x<16;++x)
        SDLBackend_DrawEgaVerticalRun(x,0,0,(uint8_t)x);
    SDLBackend_DrawEgaVerticalRun(ScreenWidth-1,ScreenHeight-1,ScreenHeight-1,15);
    for(unsigned colour=0;colour<64;++colour) {
        for(uint8_t index=0;index<16;++index)
            SDLBackend_SetEgaPaletteRegister(index,(uint8_t)((colour+index)&63));
        uint8_t mode=SDLBackend_EgaWriteMode,mask=SDLBackend_EgaMapMask;
        uint8_t readMap=SDLBackend_EgaReadMapSelect,bitMask=SDLBackend_EgaBitMask;
        SDLBackend_DecodeEgaScreen(pixels);
        check(SDLBackend_EgaWriteMode==mode && SDLBackend_EgaMapMask==mask);
        check(SDLBackend_EgaReadMapSelect==readMap && SDLBackend_EgaBitMask==bitMask);
        for(unsigned x=0;x<16;++x) {
            unsigned code=(colour+x)&63;
            const uint8_t *pixel=(const uint8_t *)&pixels[x];
            check(pixel[0]==((code&4)?170:0)+((code&16)?85:0));
            check(pixel[1]==((code&0x17)==6?85:((code&2)?170:0)+((code&16)?85:0)));
            check(pixel[2]==((code&1)?170:0)+((code&16)?85:0));
            check(pixel[3]==255);
        }
        check(pixels[ScreenWidth*ScreenHeight-1]==pixels[15]);
        check(pixels[16]==pixels[ScreenWidth*ScreenHeight-2]);
        check(SDLBackend_GetEgaPaletteRegister(15)==((colour+15)&63));
    }
    check(SDLBackend_PresentEgaScreen()); /* real texture/renderer, dummy driver */
    SDLBackend_Close();
    puts("EGA scanout: 16 plane indices x64 palette codes, RGBA byte order, last pixel and real SDL presentation passed.");
    return 0;
}
