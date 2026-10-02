#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
static uint8_t packed[PackedGraphicsOutputBytes],before[4][EgaScreenPlaneBytes];
int main(void)
{
    static const uint16_t rectangles[][4]={
        {0,0,40,200},{13,0,27,200},{35,195,20,20},{39,199,1,1},
        {40,0,1,1},{0,200,1,1},{1,1,0,0},{0xFFFF,200,2,2}
    };
    EgaMemoryAddress source={0,0xA800},destination={0,0xA000};
    for (unsigned i=0;i<sizeof(packed);++i) packed[i]=(uint8_t)(i*13+(i>>4));
    DrawCall_Image_To_VGA_Memory(packed,source.segment);
    for (unsigned test=0;test<sizeof(rectangles)/sizeof(rectangles[0]);++test) {
        for (unsigned i=0;i<sizeof(packed);++i) packed[i]=(uint8_t)(i*7+test*17);
        SDLBackend_EgaMapMask=15; SDLBackend_EgaRasterOperation=0;
        DrawCall_Image_To_VGA_Memory(packed,destination.segment);
        for (uint8_t plane=0;plane<4;++plane)
            for (uint16_t i=0;i<EgaScreenPlaneBytes;++i)
                before[plane][i]=SDLBackend_ReadEgaPlaneByte(destination.segment,i,plane);
        /* Mode1 must ignore these inherited colour-write settings. */
        SDLBackend_EgaBitMask=0x23; SDLBackend_EgaRasterOperation=EgaRaster_Xor;
        SDLBackend_EgaMapMask=0x0B;
        uint16_t x=rectangles[test][0],y=rectangles[test][1];
        uint16_t width=rectangles[test][2],height=rectangles[test][3];
        if ((uint16_t)(x+width)>=41) width=(uint16_t)(40-x);
        if ((uint16_t)(y+height)>=201) height=(uint16_t)(200-y);
        SDLBackend_EgaWriteMode=2;
        EGA_DrawBox_Operation(source,destination,x,y,rectangles[test][2],rectangles[test][3]);
        assert(FramebufferBoxWidth==width && FramebufferBoxHeight==height);
        assert(GraphicsSourceAddress.ega.segment==source.segment && GraphicsDestinationAddress.ega.offset==0);
        assert(SDLBackend_EgaBitMask==0x23 && SDLBackend_EgaRasterOperation==EgaRaster_Xor);
        assert(SDLBackend_EgaWriteMode==((x<40 && y<200)?1:2));
        for (uint8_t plane=0;plane<4;++plane)
            for (uint16_t row=0;row<200;++row)
                for (uint16_t column=0;column<40;++column) {
                    uint16_t index=(uint16_t)(row*40+column);
                    uint8_t expected=before[plane][index];
                    if (x<40 && y<200 && width!=0 && column>=x && column<x+width &&
                        row>=y && row<y+height && (0x0B&(1<<plane)))
                        expected=SDLBackend_ReadEgaPlaneByte(source.segment,index,plane);
                    assert(SDLBackend_ReadEgaPlaneByte(destination.segment,index,plane)==expected);
                }
    }
    SDLBackend_EgaMapMask=15; SDLBackend_EgaRasterOperation=0;
    EgaMemoryAddress sourceOffset={16,0xA800},destinationOffset={17,0xA000};
    EGA_DrawBox_Operation(sourceOffset,destinationOffset,1,2,3,4);
    for (uint8_t plane=0;plane<4;++plane)
        for (uint16_t row=2;row<6;++row)
            for (uint16_t column=1;column<4;++column)
                assert(SDLBackend_ReadEgaPlaneByte(0xA000,(uint16_t)(17+row*40+column),plane)==
                    SDLBackend_ReadEgaPlaneByte(0xA800,(uint16_t)(16+row*40+column),plane));
    /* One-byte overlapping shift smears forward within each native row. */
    SDLBackend_EgaMapMask=15; SDLBackend_EgaRasterOperation=0;
    for (unsigned i=0;i<sizeof(packed);++i) packed[i]=(uint8_t)(i*19);
    DrawCall_Image_To_VGA_Memory(packed,0xA000);
    uint8_t first[4];
    for (uint8_t plane=0;plane<4;++plane) first[plane]=SDLBackend_ReadEgaPlaneByte(0xA000,0,plane);
    EgaMemoryAddress shifted={1,0xA000};
    EGA_DrawBox_Operation(destination,shifted,0,0,10,1);
    for (uint8_t plane=0;plane<4;++plane)
        for (uint16_t i=1;i<=10;++i) assert(SDLBackend_ReadEgaPlaneByte(0xA000,i,plane)==first[plane]);
    /* Native1F3D:06C3 presents only the working viewport, leaving sidebar intact. */
    SDLBackend_EgaMapMask=15; SDLBackend_EgaRasterOperation=0;
    for (unsigned i=0;i<sizeof(packed);++i) packed[i]=(uint8_t)(i*23+(i>>5));
    DrawCall_Image_To_VGA_Memory(packed,MapViewportSegment);
    for (uint8_t plane=0;plane<4;++plane)
        for (uint16_t i=0;i<EgaScreenPlaneBytes;++i)
            before[plane][i]=SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,i,plane);
    EGA_DrawBox_Wrapper();
    assert(FramebufferBoxColumn==MapViewportLeftByte && FramebufferBoxRow==0);
    assert(FramebufferBoxWidth==MapViewportByteWidth && FramebufferBoxHeight==EgaScreenHeight);
    for (uint8_t plane=0;plane<4;++plane)
        for (uint16_t row=0;row<EgaScreenHeight;++row)
            for (uint16_t column=0;column<EgaFramebufferRowBytes;++column) {
                uint16_t i=(uint16_t)(row*EgaFramebufferRowBytes+column);
                uint8_t expected=column<MapViewportLeftByte ? before[plane][i]
                    : SDLBackend_ReadEgaPlaneByte(MapViewportSegment,i,plane);
                assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,i,plane)==expected);
            }
    puts("Original framebuffer copy and viewport presentation passed.");
    return 0;
}
