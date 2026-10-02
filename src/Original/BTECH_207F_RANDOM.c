#include "game.h"

/* Sol: Original 207F:0BC0. Gameplay, not a DOS method: do not redirect to SDL.
 * Update the three native state bytes through the SHR/RCL/CMC/SBB/RCR chain,
 * returning the XOR of the updated low and middle bytes. 4FC3 is untouched.
 * Separate compilation keeps hardware redirects out of headless game builds. */
uint8_t Rand_0x00_to_0xFF(void)
{
    uint8_t low = RandomByteLow;
    uint8_t middle = RandomByteMiddle;
    uint8_t high = RandomByteHigh;
    uint8_t carryFromLow = (uint8_t)((low >> 1) & 1);
    uint8_t carryFromHigh = (uint8_t)(high >> 7);
    uint8_t carryFromMiddle = (uint8_t)(middle >> 7);
    uint8_t newHigh = (uint8_t)((high << 1) | carryFromLow);
    uint8_t newMiddle = (uint8_t)((middle << 1) | carryFromHigh);
    uint8_t subtraction = (uint8_t)((low >> 2) - low - (1 - carryFromMiddle));
    uint8_t newLow = (uint8_t)((low >> 1) | ((subtraction & 1) << 7));

    RandomByteHigh = newHigh;
    RandomByteMiddle = newMiddle;
    RandomByteLow = newLow;
    return (uint8_t)(newLow ^ newMiddle);
}
