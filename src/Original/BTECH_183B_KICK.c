#include "game.h"
#include "dos.h"

/* Original183B:2231..22BB, complete ASM checked. Both intact hips are
 * required; other actuators do not gate the picker. This stores a kick order,
 * not immediate damage. Kick remains highlighted after either outcome. */
void Combat_Kick_Target(uint16_t mechId)
{
    Menu_Memory_Variables(4); Draw_Top_Graphic_Sidebar();
    if((Mechs[mechId].currentActuators[MechActuatorSide_Left]&MechLegPrimaryActuatorMask) &&
       (Mechs[mechId].currentActuators[MechActuatorSide_Right]&MechLegPrimaryActuatorMask)) {
        Display_Text_From_Memory((uint8_t *)"Choose the enemy to kick:");
        Combat_Select_Weapon_Target_UI(mechId,WeaponIndex_Kick,CombatKickTargetSlot);
    } else {
        Display_Text_From_Memory((uint8_t *)"This 'Mech can't kick because it has damaged hips. Press a key.");
        (void)Keyboard_Get_ASCII_Hex_Input();
    }
    CombatMessageMenuDefaultOption=CombatMechMenuKickOption;
}
