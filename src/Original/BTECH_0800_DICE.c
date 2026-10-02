#include "game.h"

enum { D6FaceCount = 6, D6CandidateMask = 7, D6FirstFace = 1 };

/* Sol: Original 0800:19DD. Roll the first die before the second, preserving
 * native SI/AX call order. The runtime stack-probe call is not gameplay. */
uint16_t Roll2D6(void)
{
    uint16_t firstDie = RollD6();
    uint16_t secondDie = RollD6();
    return (uint16_t)(firstDie + secondDie);
}

/* Sol: Original 0800:19F3. Three random bits yield candidates 0..7; reject
 * 6/7 and consume another RNG value. Do not replace with modulo-six. */
uint16_t RollD6(void)
{
    uint16_t candidate;
    do {
        candidate = (uint16_t)(Rand_0x00_to_0xFF() & D6CandidateMask);
    } while (candidate >= D6FaceCount);
    return (uint16_t)(candidate + D6FirstFace);
}
