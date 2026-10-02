#include "game.h"

/* Original1631:1F09..1F72. Restore tiles, focus Jason, refresh animated
 * tiles, prepare framebuffer, draw world. Signed BYTE assignment test and
 * CBW retained; native-valid assignment is a caller contract. */
void Combat_Restore_Map_View_And_Draw_World(uint8_t *savedMap)
{
    Combat_Copy_Map_Cache(savedMap,TRUE);
    int16_t focusId=Friendly_Infantry_Combatant_Range_First;
    if((int8_t)Characters[Character_Jason].mechAssignment<Character_OnFoot)
        focusId=(int8_t)Characters[Character_Jason].mechAssignment;
    Move_Map_View_To_Packed_Position(CombatantPackedX[focusId],CombatantPackedY[focusId]);
    Update_Animated_Map_Tiles();
    Copy_Data_To_GraphicsMemory();
    Draw_Menu_MultiSelect();
}

/* Original1631:1F73..1FDE retained EGA branch, matching the annotated
 * game with non-EGA pipelines removed. ONE native FAR sprite entry becomes
 * ONE host pointer; pixel WORDs retain signed clipping at renderer0377. */
void Draw_Combat_Sprites(uint16_t spriteId,uint16_t pixelX,uint16_t pixelY)
{
    EgaMemoryAddress destination={0,MapViewportSegment};
    DrawCall_EGA_CharacterPos(destination,CombatSpritePointers[spriteId],(int16_t)pixelX,(int16_t)pixelY);
}
