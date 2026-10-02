#include "game.h"
#include "dos.h"

/* Original1631:1057..10A1, complete ASM checked. Save active layout BEFORE
 * switching to the lower message panel; restore it after acknowledgement. */
void Combat_Weapon_Display_Text_And_Menu_Options(uint8_t *message)
{
    uint16_t savedLayout=CurrentMenuLayoutIndex;
    Menu_Memory_Variables(4); Draw_Top_Graphic_Sidebar();
    Display_Text_From_Memory(message); (void)Keyboard_Get_ASCII_Hex_Input();
    Draw_Top_Graphic_Sidebar(); Menu_Memory_Variables(savedLayout);
}

/* Original1631:10A2..1121. Former GetAmmo name was misleading: returns
 * raw component-minus-one for a weapon ordinal, NOT an ammo count. It keeps
 * the destruction bit; zero ammunition gives00FF. Critical-slot order counts
 * repeated weapons independently. BUG012: ordinals10/11 read walk/jump bytes,
 * with NO ten-ammo-field guard. Both AI and execution share this lookup. */
uint16_t Combat_Get_Weapon_Index_For_Ordinal(uint16_t mechId,uint16_t weaponOrdinal)
{
    uint16_t currentOrdinal=0,weaponIndex=CombatWeaponOrdinalUnavailable;
    uint8_t *mechBytes=(uint8_t *)&Mechs[mechId];
    for(uint16_t criticalOffset=MECH_ComponentBlock_Start;criticalOffset<=MECH_ComponentBlock_End;++criticalOffset) {
        uint16_t component=mechBytes[criticalOffset];
        uint16_t componentId=component&MechComponentIdMask;
        if(componentId>=Mech_Small_Laser && componentId<=Mech_SRMissile6) {
            if(currentOrdinal++==weaponOrdinal) {
                weaponIndex=(uint16_t)(component-MechComponentToWeaponRecordBias);
                if(!mechBytes[MechCurrentAmmoFirstOffset+weaponOrdinal]) weaponIndex=CombatWeaponOrdinalUnavailable;
                break;
            }
        }
    }
    return weaponIndex;
}
