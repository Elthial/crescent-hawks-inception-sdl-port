#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>

static void spanMatches(uint16_t row,uint16_t left,uint16_t right,uint8_t colour)
{
    for (uint16_t byteColumn=0;byteColumn<EgaFramebufferRowBytes;++byteColumn) {
        uint8_t bits=0;
        for (uint16_t bit=0;bit<EgaPixelsPerByte;++bit) {
            uint16_t x=(uint16_t)(byteColumn*EgaPixelsPerByte+bit);
            if (x>=left && x<=right) bits|=(uint8_t)(0x80>>bit);
        }
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
            assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,
                (uint16_t)(row*EgaFramebufferRowBytes+byteColumn),plane)==
                ((colour&(1<<plane))?bits:0));
    }
}
int main(void)
{
    assert(EgaSpanAlignmentMask==7 && EgaSpanEndAlignmentMask==504 && EgaSpanGroupShift==3);
    /* Every start alignment and all lengths1..32, including right screen edge. */
    unsigned spans=0;
    for (uint16_t left=0;left<EgaScreenWidth;++left)
        for (uint16_t length=1;length<=32 && left+length<=EgaScreenWidth;++length) {
            uint16_t right=(uint16_t)(left+length-1);
            Draw_EGA_Horizontal_Span(50,left,right,9);
            spanMatches(50,left,right,9);
            Draw_EGA_Horizontal_Span(50,left,right,0);
            spanMatches(50,left,right,0);
            ++spans;
        }
    Draw_Horizontal_EGA_Line(3,10,19,12,15);
    for (uint16_t row=10;row<=12;++row) spanMatches(row,3,19,15);
    /* Reversed rectangle Y emits nothing, unlike directly called vertical run. */
    Draw_Horizontal_EGA_Line(0,13,319,12,15);
    spanMatches(13,0,319,0);
    EGA_Draw_Vertical_Pixel_Run(23,13,12,9);
    spanMatches(13,23,23,9);
    assert(EgaPrimitiveTop==13 && EgaPrimitiveBottom==12 && EgaPrimitiveColumn==23);
    /* Initial MUL uses low Y BYTE, full WORD still controls iteration. */
    EGA_Draw_Vertical_Pixel_Run(31,257,258,9);
    spanMatches(1,31,31,9); spanMatches(2,31,31,9);
    assert(EgaPrimitiveTop==257 && EgaPrimitiveBottom==258);
    Draw_Clipped_Axis_Aligned_EGA_Line(19,20,3,20,9);
    spanMatches(20,3,19,9);
    Draw_Clipped_Axis_Aligned_EGA_Line(0,21,10,22,15);
    spanMatches(21,0,319,0); spanMatches(22,0,319,0);
    /* Both negative endpoints clamp independently to border point0,0. */
    Draw_Clipped_Axis_Aligned_EGA_Line(65532,65529,65533,65534,9);
    spanMatches(0,0,0,9);
    Draw_Clipped_Axis_Aligned_EGA_Line(500,300,600,400,9);
    spanMatches(199,319,319,9);
    assert(EgaPrimitiveColumn==319 && EgaPrimitiveTop==199 && EgaPrimitiveBottom==199);
    /* Full unchecked LOOP zero-count visits every physical byte exactly once. */
    EGA_Draw_Aligned_Span(0,0,0,15);
    for (uint32_t address=0;address<EgaPlaneBytes;++address)
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
            assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,(uint16_t)address,plane)==255);
    assert(EgaPrimitiveGroupCount==0 && SDLBackend_EgaBitMask==255);
    SDLBackend_EgaMapMask=13; SDLBackend_EgaRasterOperation=EgaRaster_Xor;
    EGA_Draw_Vertical_Pixel_Run(3,30,30,9);
    for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
        assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,1200,plane)==
            ((plane==0 || plane==3)?239:255));
    assert(SDLBackend_EgaMapMask==13 && SDLBackend_EgaRasterOperation==EgaRaster_Xor &&
        SDLBackend_EgaBitMask==16 && SDLBackend_EgaWriteMode==2);
    SDLBackend_EgaRasterOperation=EgaRaster_And;
    EGA_Draw_Aligned_Span(0,30,1,0);
    for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
        assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,1200,plane)==(plane==1?255:0));
    SDLBackend_EgaRasterOperation=EgaRaster_Or;
    EGA_Draw_Aligned_Span(0,30,1,9);
    for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
        assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,1200,plane)==(plane==2?0:255));
    printf("Original rectangles/lines: %u span cases and hardware edge cases passed\n",spans);
    return 0;
}
