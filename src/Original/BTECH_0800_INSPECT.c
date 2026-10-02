#include "game.h"
#include "dos.h"

/* Original EXE-owned DS0194 FAR table and DS4DDB BYTE table. */
uint8_t *SkillLevelDescriptions[SkillLevel_Excellent+1]={
    (uint8_t *)"Unskilled",(uint8_t *)"Amateur",(uint8_t *)"Adequate",
    (uint8_t *)"Good",(uint8_t *)"Excellent"
};

/* Original0800:378D..3BCF, full ASM checked. Original selected party/name/
 * weapon/armour IDs must identify their native tables. Negative skill bytes
 * address preceding FAR-pointer storage in the binary, not a valid skill
 * description; valid host table indices are the calling contract. No clamp
 * to zero or invented error UI is added. Positive oversized skills clamp4. */
void Inspect_Characters(void)
{
    Menu_Memory_Variables(TextPanel_FirstTimePlayer);
    Draw_Top_Graphic_Sidebar();
    Draw_Menu_Border(0);
    Citadel_Building_Dialogs(CitadelDialog_CountActiveMechs);
    if(NumberOfActiveLanceMechs) {
        Display_Text_From_Memory((uint8_t *)"You can inspect a man or a 'Mech.  Do you want to inspect a 'Mech?");
        if(Prompt_Yes_No(TRUE)) {
            Draw_Top_Graphic_Sidebar();
            Display_Text_From_Memory((uint8_t *)"Examine which 'Mech?\r");
            (void)Display_Text_Mech_Names();
            MechHeatLevel[SelectedMechId]=0;
            Examine_Screen_BTSTATS_CMP(SelectedMechId);
            Draw_Health_and_C_Bills_Sidebar(TRUE);
            Draw_Top_Graphic_Sidebar();
            return;
        }
    }
    Citadel_Building_Dialogs(CitadelDialog_CountOtherPartyMembers);
    if(SelectedPartyMemberSlot) {
        Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"Examine which character?\r");
        Citadel_Building_Dialogs(CitadelDialog_SelectPartyMember);
    }
    int16_t characterId=(int8_t)SelectedPartyMemberSlot;
    Character *character=&Characters[characterId];
    uint8_t *name=CharacterNames[(int8_t)character->name];
    Draw_Top_Graphic_Sidebar();
    Set_Text_Colour_Bright_Green();
    Display_Text_From_Memory((uint8_t *)"Name  :\rWeapon:\rArmor :\r\rHealth:\r\r\r\r\r\r\rSkills:\r\r");
    Display_Text_From_Memory((uint8_t *)"Bow & Blade\rPistol\rRifle\rGunnery\rPiloting\rTech\rMedical\r\rMedKit:\t\fMapper:\rField Surgery Kit:");
    Display_Text_At(name,InspectValueColumn,0);
    Display_Text_At(WeaponStats[(int8_t)character->weapon].name,InspectValueColumn,1);
    Display_Text_At(ArmourTextDescription[(int8_t)character->armourType],InspectValueColumn,2);
    TextPanelLeft+=InspectValueColumn; TextPanelWidth-=InspectValueColumn;
    Display_Text_At(name,0,4);
    Display_Text_Human_Health((uint16_t)characterId);
    TextPanelLeft-=InspectValueColumn; TextPanelWidth+=InspectValueColumn;
    if(TraitorInParty && (int8_t)TraitorCharacterId==characterId) {
        TextColour=EGA_BrightYellow; TextColumn=0; TextRow=InspectArmourRow;
        Display_Text_From_Memory((uint8_t *)"You don't like the way he's acting.  He makes you uneasy.");
        TextColour=EGA_BrightWhite;
        Wait_For_50Hz_Then_Check_Input();
        TraitorWarning=TRUE;
    } else if(character->armourType!=ArmourType_None) {
        Display_Text_At((uint8_t *)"Armor ",0,InspectArmourRow);
        int16_t maximumArmour=(int8_t)ArmourTypeDurability[(int8_t)character->armourType];
        int16_t lostArmour=(int16_t)(uint16_t)(maximumArmour-(int8_t)character->armourValue);
        if(!lostArmour) Display_Text_From_Memory((uint8_t *)"has not been injured.");
        else {
            Display_Text_From_Memory((uint8_t *)"has lost ");
            if(!character->armourValue) Display_Text_From_Memory((uint8_t *)"all");
            else {
                Set_Text_Colour_Bright_Green();
                Display_Text_Dynamic_Value((uint16_t)lostArmour);
            }
            TextColour=EGA_BrightWhite;
            Display_Text_From_Memory((uint8_t *)" of its ");
            Display_Text_Dynamic_Value((uint16_t)(int16_t)(int8_t)ArmourTypeDurability[(int8_t)character->armourType]);
            Display_Text_From_Memory((uint8_t *)" protective points.");
        }
    }
    uint8_t *skills=(uint8_t *)character+offsetof(Character,skillBowsAndBlade);
    for(uint16_t skill=0;skill<CharacterSkillCount;++skill) {
        int16_t level=(int8_t)skills[skill];
        if(level>SkillLevel_Excellent) level=SkillLevel_Excellent;
        Display_Text_At(SkillLevelDescriptions[level],InspectSkillColumn,(uint16_t)(InspectFirstSkillRow+skill));
    }
    Display_Text_At((uint8_t *)(PurchasedMedkit?"Yes":"No"),8,21);
    Display_Text_At((uint8_t *)(HasMapper?"Yes":"No"),20,21);
    Display_Text_At((uint8_t *)(PurchasedFieldSurgeryKit?"Yes":"No"),19,22);
    Wait_For_50Hz_Then_Check_Input();
    (void)Keyboard_Get_ASCII_Hex_Input();
}
