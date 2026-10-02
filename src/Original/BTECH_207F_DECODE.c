#include "game.h"

/* 207F:22F8. Sol: Original ASM had no maintained pseudo-C body. Reconstruct
 * its literal/repeat command loop directly, stopping at32000 output BYTEs.
 * Positive command: literal count. Negative BYTE: repeat its magnitude.
 * Zero command: little-endian WORD repeat count. Count zero executes LOOP
 * as65536, NOT an empty run. Output exhaustion takes precedence over LOOP.
 * Caller supplies sufficient source/destination; no invented safe parser. */
void Format01_Decode(uint8_t *encodedPayload, uint8_t *graphics)
{
    uint16_t sourceIndex = 0, destinationIndex = 0;
    uint16_t outputRemaining = PackedGraphicsOutputBytes, runRemaining;
    uint8_t value, repeating;
    GraphicsTransferSource = encodedPayload;
    GraphicsTransferDestination = graphics;

decodeCommand:
    repeating = FALSE;
    runRemaining = encodedPayload[sourceIndex];
    if (runRemaining == 0)
    {
        ++sourceIndex;
        runRemaining = (uint16_t)(encodedPayload[sourceIndex] |
            ((uint16_t)encodedPayload[(uint16_t)(sourceIndex+1)] << NativeByteBits));
        ++sourceIndex;
        repeating = TRUE;
    }
    else if (runRemaining & UINT16_C(0x80)) /* sign bit of the command BYTE */
    {
        runRemaining = (uint8_t)(0 - runRemaining);
        repeating = TRUE;
    }
readRunByte:
    ++sourceIndex;
    value = encodedPayload[sourceIndex];
writeRunByte:
    graphics[destinationIndex++] = value;
    if (--outputRemaining == 0) return;
    --runRemaining;
    if (runRemaining != 0)
    {
        if (repeating) goto writeRunByte;
        goto readRunByte;
    }
    ++sourceIndex;
    goto decodeCommand;
}

/* 207F:2368. Sol: Same native commands, but output descends200 rows at
 * stride160 before moving one BYTE column right. Keep the native row-wrap
 * subtraction and run termination order rather than a second-pass transpose. */
void Format02_Decode(uint8_t *encodedPayload, uint8_t *graphics)
{
    uint16_t sourceIndex = 0, destinationIndex = 0;
    uint16_t outputRemaining = PackedGraphicsOutputBytes, runRemaining;
    uint8_t rowsRemaining = EgaScreenHeight, value, repeating;
    GraphicsTransferSource = encodedPayload;
    GraphicsTransferDestination = graphics;

decodeCommand:
    repeating = FALSE;
    runRemaining = encodedPayload[sourceIndex];
    if (runRemaining == 0)
    {
        ++sourceIndex;
        runRemaining = (uint16_t)(encodedPayload[sourceIndex] |
            ((uint16_t)encodedPayload[(uint16_t)(sourceIndex+1)] << NativeByteBits));
        ++sourceIndex;
        repeating = TRUE;
    }
    else if (runRemaining & UINT16_C(0x80)) /* sign bit of the command BYTE */
    {
        runRemaining = (uint8_t)(0 - runRemaining);
        repeating = TRUE;
    }
readRunByte:
    ++sourceIndex;
    value = encodedPayload[sourceIndex];
writeRunByte:
    graphics[destinationIndex] = value;
    destinationIndex = (uint16_t)(destinationIndex + PackedGraphicsRowBytes);
    if (--rowsRemaining == 0)
    {
        rowsRemaining = EgaScreenHeight;
        destinationIndex = (uint16_t)(destinationIndex - (PackedGraphicsOutputBytes-1));
    }
    if (--outputRemaining == 0) return;
    --runRemaining;
    if (runRemaining != 0)
    {
        if (repeating) goto writeRunByte;
        goto readRunByte;
    }
    ++sourceIndex;
    goto decodeCommand;
}
