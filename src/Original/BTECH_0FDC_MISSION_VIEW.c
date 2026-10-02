#include "game.h"

/* Original0FDC:134B..13DD. Any assignment other than BYTE8 uses MECH0,
 * not the assigned Mech record. Do not "correct" this training convention.
 * Mech view shifts two cells south and normalizes the packed local-Y carry;
 * on-foot view copies Jason's infantry coordinates without that adjustment. */
void Restore_Mission_Map_View_After_Combat(void)
{
    if(Characters[Character_Jason].mechAssignment!=Character_OnFoot) {
        CrescentHawkMapPositionX=CombatantPackedX[Character_Jason];
        CrescentHawkMapPositionY=(uint16_t)(CombatantPackedY[Character_Jason]+MissionMechCameraYOffset);
        if((CrescentHawkMapPositionY&PackedPositionLocalCarryBit)!=0)
            CrescentHawkMapPositionY=(uint16_t)(CrescentHawkMapPositionY+PackedPositionSouthCarry);
    } else {
        CrescentHawkMapPositionX=CombatantPackedX[Friendly_Infantry_Combatant_Range_First+Character_Jason];
        CrescentHawkMapPositionY=CombatantPackedY[Friendly_Infantry_Combatant_Range_First+Character_Jason];
    }
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    Copy_Data_To_GraphicsMemory(); Draw_Infantry_And_Mechs(); EGA_DrawBox_Wrapper();
}
