#include "game.h"

/* Sol: Original207F:0971..0A25. Region corrections operate on WORDs,
 * even when a BYTE subtraction supplied the initial Y displacement.
 * Explicit unsigned intermediate shifts/negation avoid host signed-shift UB. */
int16_t Get_Target_Compass_Direction(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{
    CompassCompareX0=x0; CompassCompareY0=y0;
    CompassCompareX1=x1; CompassCompareY1=y1;
    uint8_t yLowDifference=(uint8_t)((uint8_t)y0-(uint8_t)y1);
    uint16_t northwardDifference=(uint16_t)(int16_t)(int8_t)yLowDifference;
    uint8_t y0Region=(uint8_t)(y0>>8)&0xF0;
    uint8_t y1Region=(uint8_t)(y1>>8)&0xF0;
    if (y1Region>y0Region) northwardDifference|=PackedPositionLocalCarryBit;
    else if (y1Region<y0Region) northwardDifference&=PackedPositionLocalMask;
    uint16_t eastwardDifference=(uint16_t)(x1-x0);
    if ((uint8_t)(x1>>8)>(uint8_t)(x0>>8)) eastwardDifference&=PackedPositionLocalMask;
    else if ((uint8_t)(x1>>8)<(uint8_t)(x0>>8)) eastwardDifference|=PackedPositionLocalCarryBit;
    uint16_t absoluteX=(int16_t)eastwardDifference<0
        ? (uint16_t)(0-eastwardDifference) : eastwardDifference;
    uint16_t absoluteY=(int16_t)northwardDifference<0
        ? (uint16_t)(0-northwardDifference) : northwardDifference;
    uint16_t flags=0;
    /* Native doubling defines overlapping cardinal sectors for diagonals:
     * an axis contributes when twice its signed displacement reaches the
     * magnitude of the other axis. Preserve the post-SHL signed WORD test. */
    if ((int16_t)(uint16_t)(northwardDifference<<1)>=(int16_t)absoluteX) flags|=Compass_North;
    if ((int16_t)(uint16_t)((uint16_t)(0-northwardDifference)<<1)>=(int16_t)absoluteX) flags|=Compass_South;
    if ((int16_t)(uint16_t)(eastwardDifference<<1)>=(int16_t)absoluteY) flags|=Compass_East;
    if ((int16_t)(uint16_t)((uint16_t)(0-eastwardDifference)<<1)>=(int16_t)absoluteY) flags|=Compass_West;
    return (int16_t)(int8_t)CompassDirectionLookup[flags]; /* CBW includes FF=>-1. */
}

/* Sol: Original207F:1DF8..1E36. Mask after shifting the LOW BYTE only.
 * Origin describes the viewport's local position inside a 24-column cache. */
void Update_Cached_Map_Origin(void)
{
    uint16_t row=(((uint8_t)CrescentHawkMapPositionY>>1)&MapCacheLocalBlockMask)+MapCacheOriginMargin;
    CachedMapOriginRowOffset=(uint16_t)(row*MapCacheWidth);
    uint16_t column=(((uint8_t)CrescentHawkMapPositionX>>1)&MapCacheLocalBlockMask)+MapCacheOriginMargin;
    CachedMapOriginColumn=column;
    CachedMapOriginIndex=(uint16_t)(CachedMapOriginRowOffset+column);
}
