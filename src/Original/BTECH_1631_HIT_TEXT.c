#include "game.h"

/* Original1631:1B8F..1BFD. Signed BYTE keys match the full WORD offset.
 * First match copies the location name, appends a period and passes through
 * the native verbosity filter. Unmatched offsets do not touch scratch text. */
void Combat_DisplayText_ArmourHitLocation(uint16_t armourOffset)
{
    for(uint16_t location=0;location<CombatArmourLocationCount;++location)
        if((uint16_t)(int16_t)CombatArmourLocationOffsets[location]==armourOffset) {
            (void)Append_Large_Text_To_Memory(DynamicString,ArmourLocationText[location]);
            (void)Append_Text_To_Memory(DynamicString,(uint8_t *)".");
            Combat_CombatMessageVerbosityFilter(DynamicString);
            break;
        }
}
