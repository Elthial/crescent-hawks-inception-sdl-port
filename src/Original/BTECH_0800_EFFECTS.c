#include "game.h"
enum { EffectFireFramePairMask=0x7E,EffectFireFirstFrame=0x7C };

/* Sol: Native0800:2A93..2C4F retained EGA, full ASM/.dis checked. Packed page
 * nibbles are different world axes. Broad signed bounds admit neighbour-page
 * carry encodings; same-page tests reject false low-byte wrap projections.
 * Fire frames advance ONLY when drawn. Empty/offscreen records are not erased. */
void Draw_Persistent_Map_Effects(void)
{
    uint16_t cameraX=CrescentHawkMapPositionX,cameraY=CrescentHawkMapPositionY;
    uint8_t cameraPage=(uint8_t)((cameraX|cameraY)>>8);
    for (uint16_t slot=0;slot<PersistentMapEffectSlotCount;++slot) {
        uint8_t page=MapEffectPackedPage[slot];
        uint16_t worldX=(uint16_t)(MapEffectPositionXLow[slot]|((uint16_t)(page&0x0F)<<8));
        uint16_t worldY=(uint16_t)(MapEffectPositionYLow[slot]|((uint16_t)(page&0xF0)<<8));
        int16_t x=(int16_t)(uint16_t)(worldX-cameraX+MapCameraCentreCellX);
        int16_t y=(int16_t)(uint16_t)(worldY-cameraY+MapCameraCentreCellY);
        int outsideX=(cameraPage&0x0F)==(page&0x0F) && (x<MapViewportLeftByte || x>MapViewportLastCellX);
        int outsideY=(cameraPage&0xF0)==(page&0xF0) && (y<0 || y>MapViewportLastCellY);
        if (x<MapProjectionMinimumX || x>MapProjectionMaximumX ||
            y<MapProjectionMinimumY || y>MapProjectionMaximumY || outsideX || outsideY) continue;
        uint8_t spriteId=MapEffectSpriteIndex[slot];
        EgaMemoryAddress destination={0,MapViewportSegment};
        DrawCall_EGA_CharacterPos(destination,CombatSpritePointers[spriteId],
            (int16_t)(((uint16_t)x&PackedPositionLocalMask)*8),
            (int16_t)(((uint16_t)y&PackedPositionLocalMask)*8));
        if ((MapEffectSpriteIndex[slot]&EffectFireFramePairMask)==EffectFireFirstFrame)
            MapEffectSpriteIndex[slot]^=1;
    }
}
