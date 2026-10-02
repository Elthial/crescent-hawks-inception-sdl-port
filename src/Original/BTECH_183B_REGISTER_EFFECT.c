#include "game.h"

/* Sol: complete original183B:27C9..2834, expanded ASM checked.
 * Access whole native saved storage so slot64..68 retain adjacent aliases.
 * Addresses must remain in represented3092:C614..D55B; arbitrary corrupted
 * slot values reaching later unrepresented globals are not certified.
 * Native code has no entry bounds check; none is silently added here. */
void Register_Persistent_Map_Effect(uint16_t spriteId,uint16_t packedX,uint16_t packedY)
{
    enum { SpriteBase=0xD457 - 0xC614,PageBase=0xD497 - 0xC614,
        XBase=0xD4D7 - 0xC614,YBase=0xD517 - 0xC614,
        CounterBase=0xD557 - 0xC614,LocalPositionMask=0x7F,PackedMapPageShift=8 };
    uint8_t *memory=OriginalSavedState.bytes;
    memory[SpriteBase+NextMapEffectSlot]=(uint8_t)spriteId;
    memory[PageBase+NextMapEffectSlot]=(uint8_t)((packedX|packedY)>>PackedMapPageShift);
    memory[XBase+NextMapEffectSlot]=(uint8_t)(packedX&LocalPositionMask);
    uint16_t slot=NextMapEffectSlot;
    uint16_t counter=(uint16_t)(memory[CounterBase]|((uint16_t)memory[CounterBase+1]<<8));
    ++counter;
    memory[CounterBase]=(uint8_t)counter;memory[CounterBase+1]=(uint8_t)(counter>>8);
    memory[YBase+slot]=(uint8_t)(packedY&LocalPositionMask);
    if(NextMapEffectSlot>PersistentMapEffectSlotCount-1)NextMapEffectSlot=0;
}
