#include "game.h"


/* Sol: Original1F3D:070A..080F retained EGA branch. Native low-WORD
 * width*height*4 wraps BEFORE header addition. Publish pointer even on null
 * allocation; only header BYTEs1/2 are assigned, leaving0/3 untouched.
 * Artwork must already be converted at original246C:244B FileWorkspace.
 * Column/width are eight-pixel cells, row/height individual scanlines.
 * Valid original sprite IDs/allocated buffers and non-wrapping original
 * FAR offsets are the contract;05BC's unresolved allocation path is not fixed. */
void Capture_Combat_Sprite(uint16_t spriteId,uint16_t column,uint16_t row,uint16_t width,uint16_t height)
{
    uint16_t payload=(uint16_t)((uint32_t)width*height);
    payload=(uint16_t)(payload*SpritePlanesPerCell);
    uint8_t *sprite=Allocate_Far_Buffer((uint16_t)(payload+CombatSpriteHeaderBytes));
    CombatSpritePointers[spriteId]=sprite;
    if (sprite==NULL) return;
    sprite[SpriteHeightMinusOneByte]=(uint8_t)(height-1);
    sprite[SpriteWidthByte]=(uint8_t)width;
    uint16_t sourceColumn=(uint16_t)(column*SpritePlanesPerCell);
    uint16_t sourceRow=(uint16_t)(PackedGraphicsRowBytes*row);
    uint16_t wordsPerRow=(uint16_t)(width*SpriteWordsPerCell);
    uint16_t sourceRowGap=(uint16_t)(PackedGraphicsRowBytes-(uint16_t)(wordsPerRow*2));
    Copy_Strided_Word_Rows(GraphicsFileWorkspace+(uint16_t)(sourceColumn+sourceRow),
        sprite+CombatSpriteHeaderBytes,wordsPerRow,height,sourceRowGap);
}
