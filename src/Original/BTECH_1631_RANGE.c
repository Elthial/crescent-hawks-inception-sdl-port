#include "game.h"

/* Original1631:0BB5..0C62, complete ASM checked. Packed page nibbles
 * represent128 cells each. This is NOT Euclidean or tabletop hex distance:
 * half the larger axis separation (rounded down), plus the smaller. */
uint16_t Combat_Packed_Distance_From_Map_Position(uint16_t packedX,uint16_t packedY)
{
    int16_t deltaX=(int16_t)(((CrescentHawkMapPositionX&PackedPositionXRegionMask)>>1 |
        (CrescentHawkMapPositionX&PackedPositionLocalMask))-
        ((packedX&PackedPositionXRegionMask)>>1 | (packedX&PackedPositionLocalMask)));
    int16_t deltaY=(int16_t)(((CrescentHawkMapPositionY&PackedPositionYRegionMask)>>5 |
        (CrescentHawkMapPositionY&PackedPositionLocalMask))-
        ((packedY&PackedPositionYRegionMask)>>5 | (packedY&PackedPositionLocalMask)));
    if(deltaX<0) deltaX=(int16_t)-deltaX;
    if(deltaY<0) deltaY=(int16_t)-deltaY;
    return (uint16_t)(deltaX>deltaY?deltaX/2+deltaY:deltaY/2+deltaX);
}

/* Original1631:0F24..1056. Far Mechs occupy several personnel cells;
 * adjust their target edge before measuring. X comparisons are sequential,
 * Y adjustment is only toward a target south of the camera. Strict thresholds
 * remain: equality with maximum range is OUT, not Long. */
uint16_t Combat_Calculate_RangeBracket(uint16_t targetId,uint16_t weaponId)
{
    uint16_t targetX=CombatantPackedX[targetId],targetY=CombatantPackedY[targetId];
    if(((int16_t)targetId<Friendly_Infantry_Combatant_Range_First ||
        ((int16_t)targetId>=Enemy_All_CombatantId_Range_First &&
         (int16_t)targetId<Enemy_Infantry_CombatantId_Range_First)) &&
        (int16_t)Combat_Packed_Distance_From_Map_Position(targetX,targetY)>MechFootprintRangeCorrectionThreshold) {
        if(CrescentHawkMapPositionX<targetX) {
            --targetX;
            if(targetX&PackedPositionLocalCarryBit) targetX&=PackedPositionWestNormalizeMask;
        }
        if(CrescentHawkMapPositionX>targetX) {
            ++targetX;
            if(targetX&PackedPositionLocalCarryBit) targetX=(uint16_t)(targetX+PackedPositionLocalCarryBit);
        }
        if(CrescentHawkMapPositionY<targetY) {
            targetY=(uint16_t)(targetY-2); /*Mech footprint extends two personnel rows north*/
            if(targetY&PackedPositionLocalCarryBit) targetY&=PackedPositionNorthNormalizeMask;
        }
    }
    uint16_t distance=Combat_Packed_Distance_From_Map_Position(targetX,targetY);
    Weapon *weapon=&WeaponStats[weaponId];
    uint16_t bracket=RangeBracket_OutOfRange;
    if(weapon->maximumRange>distance) bracket=RangeBracket_Long;
    uint16_t mediumRange=weapon->rangeBracket&WeaponMediumRangeMask;
    uint16_t shortRange=weapon->rangeBracket>>WeaponShortRangeShift;
    /* Native2EE4 is record BYTE12, the infantry flag in the packed attack
     * count/cluster field, NOT skillType. Kick is explicitly unscaled. */
    if(weapon->attackCountOrClusterColumn<WeaponInfantryAttackFlag && weaponId!=WeaponIndex_Kick) {
        mediumRange*=MechToPersonnelRangeScale; shortRange*=MechToPersonnelRangeScale;
    }
    if(distance<mediumRange) bracket=RangeBracket_Medium;
    if(distance<shortRange) bracket=RangeBracket_Short;
    return bracket;
}
