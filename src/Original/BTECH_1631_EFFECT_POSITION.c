#include "game.h"

/* Original1631:1DF8..1EA1. Unsigned packed-axis stepping; these shared
 * coordinates alias movement scratch E486/E488. Not a compass heading or
 * subtraction of packed words: a region boundary skips the unused low half. */
void Combat_CompassPos(uint16_t cameraX,uint16_t cameraY,uint16_t targetX,uint16_t targetY)
{
    MovementActorPositionX=CombatPreviewOriginColumn;
    MovementActorPositionY=CombatPreviewOriginRow;
    while(cameraY>targetY) {
        --cameraY;
        if(cameraY&PackedPositionLocalCarryBit) cameraY&=PackedPositionNorthNormalizeMask;
        --MovementActorPositionY;
    }
    while(cameraY<targetY) {
        ++cameraY;
        if(cameraY&PackedPositionLocalCarryBit) cameraY=(uint16_t)(cameraY+PackedPositionSouthCarry);
        ++MovementActorPositionY;
    }
    while(cameraX>targetX) {
        --cameraX;
        if(cameraX&PackedPositionLocalCarryBit) cameraX&=PackedPositionWestNormalizeMask;
        --MovementActorPositionX;
    }
    while(cameraX<targetX) {
        ++cameraX;
        if(cameraX&PackedPositionLocalCarryBit) cameraX=(uint16_t)(cameraX+PackedPositionLocalCarryBit);
        ++MovementActorPositionX;
    }
}

/* Original1631:1EA2..1F08. Signed WORD NEG before signed comparison:
 * -32768 remains -32768. Ties and (0,0)->6 are original, not atan2. */
uint16_t Combat_Projectile_Octant(int16_t deltaY,int16_t deltaX)
{
    uint16_t octant;
    if(deltaY<0) {
        if(deltaX>0) {
            octant=2;
            if((int16_t)(uint16_t)-deltaY<=deltaX) return octant;
        } else {
            octant=4;
            if((int16_t)(uint16_t)-deltaX<=(int16_t)(uint16_t)-deltaY) return octant;
        }
    } else if(deltaX<=0) {
        octant=6;
        if((int16_t)(uint16_t)-deltaX>=deltaY) return octant;
    } else {
        octant=0;
        if(deltaX<=deltaY) return octant;
    }
    return (uint16_t)(octant+1);
}
