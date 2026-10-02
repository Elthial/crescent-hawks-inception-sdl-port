#include "game.h"

/* Sol: Original0800:186F..191A, complete ASM checked. Arithmetic-only
 * counterpart of17BB: no cache shifts, map loading or graphics calls.
 * Unsigned WORD comparisons; finish Y before X. Do not replace with direct
 * assignments: a noncanonical target may be skipped by normalization, and
 * unreachable targets can loop forever in the native routine. */
void Combat_Move_Position(uint16_t targetX,uint16_t targetY)
{
    while (CrescentHawkMapPositionY>targetY) {
        --CrescentHawkMapPositionY;
        if (CrescentHawkMapPositionY&PackedPositionLocalCarryBit)
            CrescentHawkMapPositionY&=PackedPositionNorthNormalizeMask;
    }
    while (CrescentHawkMapPositionY<targetY) {
        ++CrescentHawkMapPositionY;
        if (CrescentHawkMapPositionY&PackedPositionLocalCarryBit)
            CrescentHawkMapPositionY+=PackedPositionSouthCarry;
    }
    while (CrescentHawkMapPositionX>targetX) {
        --CrescentHawkMapPositionX;
        if (CrescentHawkMapPositionX&PackedPositionLocalCarryBit)
            CrescentHawkMapPositionX&=PackedPositionWestNormalizeMask;
    }
    while (CrescentHawkMapPositionX<targetX) {
        ++CrescentHawkMapPositionX;
        if (CrescentHawkMapPositionX&PackedPositionLocalCarryBit)
            CrescentHawkMapPositionX+=PackedPositionLocalCarryBit;
    }
}
