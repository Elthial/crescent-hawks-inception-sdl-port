#include "game.h"
#include "../SDL/backend.h"

/* Sol: Original207F:245C..24D6 retained EGA staging. X/width units are
 * byte-columns, Y/height pixel rows: the last two are sizes, not end points.
 * Preserve original shared address/rectangle scratch for the child entry. */
void EGA_DrawBox_Operation(EgaMemoryAddress source,EgaMemoryAddress destination,
    uint16_t column,uint16_t row,uint16_t width,uint16_t height)
{
    GraphicsSourceAddress.ega=source;
    GraphicsDestinationAddress.ega=destination;
    FramebufferBoxColumn=column;
    FramebufferBoxRow=row;
    FramebufferBoxWidth=width;
    FramebufferBoxHeight=height;
    DrawCall_EGA_DrawBox();
}

/* Sol: Original24D7 retained24EB..2568/shared2629 exit, full raw ASM checked.
 * WORD addition wraps BEFORE clipping. Clipping writes scratch even if origin
 * is later rejected. REP width0 copies nothing; height0 executes65536 rows.
 * Mode1 ignores inherited raster/bitmask but respects enabled planes.
 * Explicit forward order represents the intended inherited DF-clear contract;
 * overlapping regions retain native MOVSB smear, not memmove semantics. */
void DrawCall_EGA_DrawBox(void)
{
    if ((uint16_t)(FramebufferBoxColumn+FramebufferBoxWidth)>=EgaFramebufferRowBytes+1)
        FramebufferBoxWidth=(uint16_t)(EgaFramebufferRowBytes-FramebufferBoxColumn);
    if ((uint16_t)(FramebufferBoxRow+FramebufferBoxHeight)>=EgaScreenHeight+1)
        FramebufferBoxHeight=(uint16_t)(EgaScreenHeight-FramebufferBoxRow);
    if (FramebufferBoxColumn>=EgaFramebufferRowBytes || FramebufferBoxRow>=EgaScreenHeight)
        return;
    uint16_t rectangleOffset=(uint16_t)(FramebufferBoxColumn+FramebufferBoxRow*EgaFramebufferRowBytes);
    uint16_t source=(uint16_t)(GraphicsSourceAddress.ega.offset+rectangleOffset);
    uint16_t destination=(uint16_t)(GraphicsDestinationAddress.ega.offset+rectangleOffset);
    uint16_t width=FramebufferBoxWidth,rows=FramebufferBoxHeight;
    SDLBackend_EgaWriteMode=1;
    do {
        for (uint16_t remaining=width;remaining!=0;--remaining)
            SDLBackend_CopyEgaLatchByte(GraphicsSourceAddress.ega.segment,source++,
                GraphicsDestinationAddress.ega.segment,destination++);
        uint16_t rowGap=(uint16_t)(EgaFramebufferRowBytes-width);
        source=(uint16_t)(source+rowGap);
        destination=(uint16_t)(destination+rowGap);
    } while (--rows!=0);
}
