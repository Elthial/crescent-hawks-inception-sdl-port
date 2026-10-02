#include "game.h"

/* Sol: Original207F:0931..0970. DL narrows words/row, DH narrows rows;
 * zero rows execute256 iterations, zero width copies none. Destination stays
 * contiguous; source gap is added after EVERY row. Forward WORD read-before-
 * write order preserves overlapping copy smear, unlike memcpy/memmove.
 * Native025A DOS source-segment bookkeeping is absorbed by the flat parameter.
 * Relative WORD offsets wrap; valid non-straddling RAM buffers and compatible
 * original FAR boundaries/DF-clear are the host contract. */
void Copy_Strided_Word_Rows(uint8_t *source,uint8_t *destination,
    uint16_t wordsPerRow,uint16_t rows,uint16_t sourceRowGap)
{
    uint16_t sourceOffset=0,destinationOffset=0;
    uint8_t remainingRows=(uint8_t)rows;
    do {
        for (uint16_t word=0;word<(uint8_t)wordsPerRow;++word) {
            uint8_t low=source[sourceOffset];
            uint8_t high=source[(uint16_t)(sourceOffset+1)];
            destination[destinationOffset]=low;
            destination[(uint16_t)(destinationOffset+1)]=high;
            sourceOffset=(uint16_t)(sourceOffset+2);
            destinationOffset=(uint16_t)(destinationOffset+2);
        }
        sourceOffset=(uint16_t)(sourceOffset+sourceRowGap);
    } while (--remainingRows != 0);
}
