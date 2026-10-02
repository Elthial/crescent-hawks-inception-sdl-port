#include "game.h"

/* Sol: Original1E56:0A3B retained0ACD..0AE4 EGA path. It ignores the
 * supplied display pointer and captures the uploaded image at nativeA800.
 * Deleted other-adapter bodies and FAR address helpers are not included. */
void TileSet_Memory_Operation(uint8_t *display,uint8_t *destination,uint16_t column,uint16_t row)
{
    (void)display;
    DrawCall_Read_EGAMemory(destination,column,row);
}

/* Sol: Original1E56:0AE5..0B5D. Always allocate, even count0, with SHL5
 * WORD wrap BEFORE CWD sign extension to DWORD. Signed count and column tests
 * are retained. Return the initial allocation, not the advanced destination.
 * Valid allocated buffers not crossing the original FAR offset boundary are
 * the host contract; allocator05BC owns the unresolved oversize/failure path. */
uint8_t *Create_TileSet_Array(uint8_t *display,uint16_t column,uint16_t row,uint16_t count)
{
    uint16_t allocationBytes=(uint16_t)(count*BorderTileBytes);
    uint32_t requestedBytes=(uint32_t)(int32_t)(int16_t)allocationBytes;
    uint8_t *allocation=Allocate_Far_Buffer(requestedBytes);
    uint16_t destinationOffset=0;
    for (uint16_t captured=0;(int16_t)captured<(int16_t)count;++captured) {
        TileSet_Memory_Operation(display,allocation+destinationOffset,column,row);
        ++column;
        if ((int16_t)column > EgaFramebufferRowBytes-1) {
            column=0;
            ++row;
        }
        destinationOffset=(uint16_t)(destinationOffset+BorderTileBytes);
    }
    return allocation;
}
