#include "game.h"

/* Sol: Original183B:1482..14C2, every instruction/branch checked against ASM.
 * Plan twelve actors on one side, in table order. Active is a WORD, not a
 * Boolean BYTE; any nonzero value qualifies. Read each entry when reached,
 * not a snapshot before calls, because planning can change shared state.
 * Original callers supply0 (friendly Lance plus infantry) or12 (enemy side).
 * No original clamp for an invalid starting ID is invented here. */
void Combat_Plan_Computer_Side(uint16_t firstCombatantId)
{
    for (uint16_t sideSlot=0;sideSlot<Enemy_All_CombatantId_Range_First;++sideSlot) {
        uint16_t combatantId=(uint16_t)(firstCombatantId+sideSlot);
        if (CombatantActive[combatantId]!=FALSE)
            Combat_Computer_Control(combatantId,FALSE);
    }
}
