#include "game.h"
enum { PlanarGroupBytes=4, PlanarPlaneCount=4, PackedPixelsPerByte=2 };

/* Sol: Original207F:0572..05BE with internal05BF..05CF bit operations
 * inlined, not an annotation-only helper with a fabricated C ABI. Each group
 * reads four packed bytes before its two native WORD stores, supporting
 * in-place conversion under the original DF-clear RAM contract. Eight pixels
 * replace every bit of the four accumulators regardless of incoming BX/DX.
 * Native count0/1 LOOP underflow and odd-word discard remain intact. Relative
 * WORD offsets wrap through64KiB; source/destination must provide that storage
 * for underflow cases. Ordinary startup32000-byte buffers never wrap; arbitrary
 * nonzero original FAR-offset boundaries/bus writes are not host contracts. */
void VGA_Inline_ASM_Loop(uint8_t *source,uint8_t *destination,uint16_t wordsToConvert)
{
    uint16_t sourceOffset=0,destinationOffset=0;
    uint16_t groupsRemaining=(uint16_t)(wordsToConvert>>1);
    do {
        uint8_t planes[PlanarPlaneCount]={0,0,0,0};
        for (uint16_t packedByte=0;packedByte<PlanarGroupBytes;++packedByte) {
            uint8_t pixels=source[sourceOffset++];
            for (uint16_t pixel=0;pixel<PackedPixelsPerByte;++pixel) {
                planes[3]=(uint8_t)((planes[3]<<1)|((pixels>>7)&1));
                planes[2]=(uint8_t)((planes[2]<<1)|((pixels>>6)&1));
                planes[1]=(uint8_t)((planes[1]<<1)|((pixels>>5)&1));
                planes[0]=(uint8_t)((planes[0]<<1)|((pixels>>4)&1));
                pixels=(uint8_t)(pixels<<4);
            }
        }
        destination[destinationOffset++]=planes[0];
        destination[destinationOffset++]=planes[1];
        destination[destinationOffset++]=planes[2];
        destination[destinationOffset++]=planes[3];
    } while (--groupsRemaining != 0);
}
