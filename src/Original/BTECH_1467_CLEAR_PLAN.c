#include "game.h"

/* Original1467:0D7E..0DC3, whole ASM and .dis tail checked. Clears576
 * generated step BYTES, NOT1152 destination-order bytes. Zero is not the02
 * end sentinel; native execution rebuilds plans separately. */
void Combat_Clear_Movement_Plans(void)
{
    for(uint16_t combatant=0;combatant<AllCombatantCount;++combatant)
        for(uint16_t byte=0;byte<CombatMovementPlanBytesPerUnit;++byte)
            CombatMovementPlanBytes[combatant*CombatMovementPlanBytesPerUnit+byte]=0;
}
