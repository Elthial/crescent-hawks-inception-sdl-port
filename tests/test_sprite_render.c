#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t background[PackedGraphicsOutputBytes],sprite[4+3*2*4];
static uint8_t pixel(unsigned x,unsigned y) { return (uint8_t)((x+3*y)%16); }
static void reset(void)
{
    SDLBackend_EgaRasterOperation=EgaRaster_Replace; SDLBackend_EgaMapMask=15;
    SDLBackend_EgaRotateCount=SDLBackend_EgaEnableSetReset=SDLBackend_EgaSetReset=0;
    memset(background,0x99,sizeof background);
    DrawCall_Image_To_VGA_Memory(background,0xAC00);
}
static uint8_t screenPixel(unsigned x,unsigned y)
{
    uint8_t colour=0;
    for (uint8_t plane=0;plane<4;++plane)
        if (SDLBackend_ReadEgaPlaneByte(0xAC00,(uint16_t)(y*40+x/8),plane)&(0x80>>(x%8))) colour|=(uint8_t)(1<<plane);
    return colour;
}
static uint8_t writeOracle(uint8_t old,uint8_t cpu,unsigned rotate,unsigned operation)
{
    if (rotate) cpu=(uint8_t)((cpu>>rotate)|(cpu<<(8-rotate)));
    if (operation==EgaRaster_And) return (uint8_t)(cpu&old);
    if (operation==EgaRaster_Or) return (uint8_t)(cpu|old);
    if (operation==EgaRaster_Xor) return (uint8_t)(cpu^old);
    return cpu;
}
int main(void)
{
    memset(sprite,0,sizeof sprite); sprite[1]=2; sprite[2]=2;
    for (unsigned y=0;y<3;++y)
        for (unsigned x=0;x<16;++x)
            for (unsigned plane=0;plane<4;++plane)
                if (pixel(x,y)&(1<<plane)) sprite[4+y*8+(x/8)*4+plane]|=(uint8_t)(0x80>>(x%8));
    EgaMemoryAddress destination={0,0xAC00};
    const int16_t positions[][2]={{0,0},{1,8},{2,8},{3,8},{4,8},{5,8},{6,8},{7,8},{-8,8},{8,-1},{319,199},{312,8}};
    for (unsigned test=0;test<sizeof positions/sizeof positions[0];++test) {
        int x=positions[test][0],y=positions[test][1]; reset();
        DrawCall_EGA_CharacterPos(destination,sprite,(int16_t)x,(int16_t)y);
        assert(SDLBackend_EgaMapMask==15 && SDLBackend_EgaReadMapSelect==3);
        assert(SDLBackend_EgaWriteMode==0 && SDLBackend_EgaBitMask==255 && SDLBackend_LastSpriteCleanupPort==0x3C4);
        for (unsigned sy=0;sy<200;++sy)
            for (unsigned sx=0;sx<320;++sx) {
                int localX=(int)sx-x,localY=(int)sy-y;
                uint8_t expected=9;
                if (localX>=0 && localX<16 && localY>=0 && localY<3) {
                    uint8_t colour=pixel((unsigned)localX,(unsigned)localY);
                    if (colour) expected=colour;
                }
                assert(screenPixel(sx,sy)==expected);
            }
    }
    /* BUG-019: no unconditional map-mask restore on offscreen exits. */
    const int16_t rejected[][2]={{320,0},{-16,0},{0,200},{0,-3}};
    for (unsigned i=0;i<4;++i) {
        reset(); SDLBackend_EgaMapMask=2; SDLBackend_EgaWriteMode=2;
        DrawCall_EGA_CharacterPos(destination,sprite,rejected[i][0],rejected[i][1]);
        assert(SDLBackend_EgaMapMask==2 && SDLBackend_EgaWriteMode==2 && SDLBackend_LastSpriteCleanupPort==0xAC00);
        assert(screenPixel(0,0)==9);
    }
    reset(); SDLBackend_EgaMapMask=2;
    DrawCall_EGA_CharacterPos(destination,sprite,320,-1);
    assert(SDLBackend_LastSpriteCleanupPort==0 && SDLBackend_EgaMapMask==2); /* MUL-high overwroteDX */
    assert(sprite[1]==2 && sprite[2]==2); /* renderer never changes header */
    uint8_t small[]={0,0,1,0,0xAA,0xCC,0xF0,0x0F};
    for (unsigned operation=0;operation<4;++operation)
        for (unsigned rotate=0;rotate<8;++rotate) {
            reset(); SDLBackend_EgaRasterOperation=(uint8_t)operation;
            SDLBackend_EgaRotateCount=(uint8_t)rotate;
            DrawCall_EGA_CharacterPos(destination,small,3,0);
            for (uint8_t plane=0;plane<4;++plane)
                for (unsigned byte=0;byte<2;++byte) {
                    uint16_t shifted=(uint16_t)((uint16_t)(small[4+plane]<<8)>>3);
                    uint8_t mask=byte?0x1F:0xE0,bits=byte?(uint8_t)shifted:(uint8_t)(shifted>>8);
                    uint8_t old=(9&(1<<plane))?255:0;
                    uint8_t first=writeOracle(old,(uint8_t)(old&mask),rotate,operation);
                    uint8_t expected=writeOracle(first,(uint8_t)(first|bits),rotate,operation);
                    assert(SDLBackend_ReadEgaPlaneByte(0xAC00,(uint16_t)byte,plane)==expected);
                }
        }
    puts("Original transparent sprites: all shifts, clipping/spill/zero-colour, header stability and BUG-019 cleanup passed");
    return 0;
}
