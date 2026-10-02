#include "game.h"

/* Original1AE8:1E46..1EB3. ASM export truncates at POP SI; clean .dis and
 * EXE5E 8B E5 5D CB confirm the return. A2D6 roll outside5..9 registers
 * fire at actor position plus WORD offsets. Native masks on bit7 are not
 * ordinary normalized movement: preserve their truncation exactly. */
void Combat_Random_CreateFire(uint16_t entityId,uint16_t offsetX,uint16_t offsetY)
{
    uint16_t roll=Roll2D6();
    if((int16_t)roll<CombatMissFireRollLow || (int16_t)roll>CombatMissFireRollHigh) {
        uint16_t fireX=(uint16_t)(CombatantPackedX[entityId]+offsetX);
        uint16_t fireY=(uint16_t)(CombatantPackedY[entityId]+offsetY);
        if(fireX&PackedPositionLocalCarryBit) fireX&=PackedPositionWestNormalizeMask;
        if(fireY&PackedPositionLocalCarryBit) fireY&=PackedPositionNorthNormalizeMask;
        Register_Persistent_Map_Effect(CombatMissFireSprite,fireX,fireY);
    }
}
