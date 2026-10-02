#include "game.h"
#include "dos.h"

/* Original0DAB:18E8..1AFD, complete ASM checked. Friendly combatant IDs
 * only: mech0..3, infantry4..11. Name/weapon/armour/traitor BYTEs use native
 * signed extension; pilot seat itself is unsigned. Valid table indices and
 * nonzero Body for the health callee are the original calling contract. */
void Display_Friendly_Combatant_Description(uint16_t combatantId,uint16_t allowTraitorRecognition,uint16_t showInfantryDetails)
{
    TextColour=EGA_BrightWhite;
    if((int16_t)combatantId<Friendly_Infantry_Combatant_Range_First) {
        Mech *mech=&Mechs[combatantId];
        Display_Text_From_Memory(CharacterNames[(int8_t)Characters[mech->pilotId].name]);
        Display_Text_From_Memory((uint8_t *)"'s\r"); /*DS121E*/
        Display_Text_From_Memory(mech->name);
        return;
    }
    uint16_t partyMember=(uint16_t)(combatantId-Friendly_Infantry_Combatant_Range_First);
    Character *character=&Characters[partyMember];
    Display_Text_From_Memory(CharacterNames[(int8_t)character->name]);
    if(!showInfantryDetails) return;
    if(TraitorInParty && (int8_t)TraitorCharacterId==(int16_t)partyMember && allowTraitorRecognition) {
        Display_Text_From_Memory((uint8_t *)" \x06\x06looks very suspicious to you.\x06\x0F"); /*DS1222*/
        Wait_For_50Hz_Then_Check_Input();
        (void)Keyboard_Get_ASCII_Hex_Input();
        TraitorWarning=TRUE;
        return;
    }
    Display_Text_Human_Health(partyMember);
    TextColumn=0; TextRow=CombatDescriptionHeadingRow;
    Set_Text_Colour_Bright_Green();
    Display_Text_From_Memory((uint8_t *)"Weapon:\r\rArmor:\x06\x0F"); /*DS1245*/
    TextColumn=0; TextRow=CombatDescriptionWeaponRow;
    Display_Text_From_Memory(WeaponStats[(int8_t)character->weapon].name);
    TextColumn=0; TextRow=CombatDescriptionArmourRow;
    int16_t armourType=(int8_t)character->armourType;
    if(armourType==ArmourType_None) {
        Display_Text_From_Memory((uint8_t *)"None"); /*DS1260*/
        return;
    }
    int16_t armourRemaining=(int8_t)character->armourValue;
    if(!armourRemaining) {
        TextColour=EGA_DarkGrey;
        Display_Text_From_Memory((uint8_t *)"Ruined\x06\x0F"); /*DS1257*/
        return;
    }
    if((int8_t)ArmourTypeDurability[armourType]!=armourRemaining) TextColour=EGA_BrightYellow;
    Display_Text_From_Memory(ArmourTextDescription[armourType]);
}
