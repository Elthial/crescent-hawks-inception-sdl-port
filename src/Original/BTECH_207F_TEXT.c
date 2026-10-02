#include "game.h"

/* Sol: Original 207F:3B9E. REPNE scan count excludes the final terminator.
 * Native scan ceiling remains UINT16_MAX; malformed FAR-segment wrapping is
 * outside flat host arrays' contract. Valid strings must fit their storage. */
uint16_t Loop_Until_TextPtr_Null(uint8_t *text)
{
    uint16_t remaining = UINT16_MAX;
    uint16_t offset = 0;
    while (remaining != 0) {
        uint8_t character = text[offset++];
        --remaining;
        if (character == 0) break;
    }
    return (uint16_t)((uint16_t)~remaining - 1);
}

/* Sol: Original 207F:3B68, FAR strcpy despite the legacy Append name.
 * Inline the native scan and forward word-copy operations, not the annotation's
 * research helpers. Ordinary nonoverlapping RAM strings are the host contract;
 * native FAR-offset wrapping/overlap alignment is not certified here. */
uint8_t *Append_Large_Text_To_Memory(uint8_t *destination, uint8_t *source)
{
    uint16_t remaining = UINT16_MAX;
    uint16_t count = 0;
    uint16_t offset = 0;
    while (remaining != 0) {
        uint8_t character = source[count++];
        --remaining;
        if (character == 0) break;
    }
    if (((uintptr_t)destination & 1) != 0) {
        destination[offset] = source[offset];
        ++offset;
        --count;
    }
    uint16_t wordsRemaining = (uint16_t)(count >> 1);
    while (wordsRemaining != 0) {
        uint8_t low = source[offset];
        uint8_t high = source[(uint16_t)(offset + 1)];
        destination[offset++] = low;
        destination[offset++] = high;
        --wordsRemaining;
    }
    if ((count & 1) != 0) destination[offset] = source[offset];
    return destination;
}

/* Sol: Original 207F:3B22, FAR strcat. Destination terminator is overwritten,
 * source terminator copied, and the original destination pointer returned. */
uint8_t *Append_Text_To_Memory(uint8_t *destination, uint8_t *source)
{
    uint16_t remaining = UINT16_MAX;
    uint16_t destinationOffset = 0;
    while (remaining != 0) {
        uint8_t character = destination[destinationOffset++];
        --remaining;
        if (character == 0) break;
    }
    --destinationOffset;
    remaining = UINT16_MAX;
    uint16_t count = 0;
    uint16_t sourceOffset = 0;
    while (remaining != 0) {
        uint8_t character = source[count++];
        --remaining;
        if (character == 0) break;
    }
    if (((uintptr_t)source & 1) != 0) {
        destination[destinationOffset++] = source[sourceOffset++];
        --count;
    }
    uint16_t wordsRemaining = (uint16_t)(count >> 1);
    while (wordsRemaining != 0) {
        uint8_t low = source[sourceOffset++];
        uint8_t high = source[sourceOffset++];
        destination[destinationOffset++] = low;
        destination[destinationOffset++] = high;
        --wordsRemaining;
    }
    if ((count & 1) != 0) destination[destinationOffset] = source[sourceOffset];
    return destination;
}
