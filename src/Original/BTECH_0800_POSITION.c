#include "game.h"

/* Sol: Original0800:17BB..1816. Unsigned packed comparisons, Y before X.
 * Invalid/unreachable targets retain native nontermination; no new guard. */
void Move_Map_View_To_Packed_Position(uint16_t targetX,uint16_t targetY)
{
    while (targetY<CrescentHawkMapPositionY) Map_Move_North();
    while (targetY>CrescentHawkMapPositionY) Map_Move_South();
    while (targetX<CrescentHawkMapPositionX) Map_Move_West();
    while (targetX>CrescentHawkMapPositionX) Map_Move_East();
}

/* Sol: Original0800:1817..186E, complete unpromoted raw ASM frame.
 * Consume signed argument copies even when a world edge prevents movement. */
void Move_Map_View_By_Signed_Delta(int16_t deltaX,int16_t deltaY)
{
    while (deltaY<0) { Map_Move_North(); ++deltaY; }
    while (deltaY>0) { Map_Move_South(); --deltaY; }
    while (deltaX<0) { Map_Move_West(); ++deltaX; }
    while (deltaX>0) { Map_Move_East(); --deltaX; }
}

/* Sol: Original0800:191B..19BE. Consume signed deltas Y then X, moving
 * the shared packed anchor but not rebuilding the cached map. Each local
 * axis spans128 cells; X regions occupy bits8..11, Y regions bits12..15. */
void Offset_Packed_Position(int16_t deltaX, int16_t deltaY)
{
    while (deltaY < 0) {
        --CrescentHawkMapPositionY;
        if (CrescentHawkMapPositionY & PackedPositionLocalCarryBit)
            CrescentHawkMapPositionY &= PackedPositionNorthNormalizeMask;
        ++deltaY;
    }
    while (deltaY > 0) {
        ++CrescentHawkMapPositionY;
        if (CrescentHawkMapPositionY & PackedPositionLocalCarryBit)
            CrescentHawkMapPositionY += PackedPositionSouthCarry;
        --deltaY;
    }
    while (deltaX < 0) {
        --CrescentHawkMapPositionX;
        if (CrescentHawkMapPositionX & PackedPositionLocalCarryBit)
            CrescentHawkMapPositionX &= PackedPositionWestNormalizeMask;
        ++deltaX;
    }
    while (deltaX > 0) {
        ++CrescentHawkMapPositionX;
        if (CrescentHawkMapPositionX & PackedPositionLocalCarryBit)
            CrescentHawkMapPositionX += PackedPositionLocalCarryBit;
        --deltaX;
    }
}
