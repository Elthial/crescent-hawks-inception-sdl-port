#include "game.h"

/* Original0800:2C50..2DA7, complete ASM checked. Name byteFF denotes an
 * absent/destroyed friendly slot; zero is still present. Native geometry and
 * option-count mutations persist after returning, unlike a temporary host UI. */
void Game_Pause_Menu(void)
{
    MenuPanelLayouts[PauseMenuPanel].top=PauseMenuTopRow;
    MenuPanelLayouts[PauseMenuPanel].height=PauseMenuBaseChoices;
    MenuControls[PauseMenuPanel].optionCount=PauseMenuBaseChoices;
    uint16_t partyHasMech=FALSE;
    for(uint16_t mechId=0;mechId<LanceSize;++mechId)
        if(Mechs[mechId].name[0]!=MECH_Destroyed) partyHasMech=TRUE;
    if(partyHasMech) {
        ++MenuControls[PauseMenuPanel].optionCount;
        --MenuPanelLayouts[PauseMenuPanel].top;
        ++MenuPanelLayouts[PauseMenuPanel].height;
    }
    Menu_Memory_Variables(PauseMenuPanel);
    Draw_Top_Graphic_Sidebar();
    Draw_Menu_Border(PauseMenuPanel);
    Display_Text_From_Memory((uint8_t *)"Return to game\rChange game settings"); /*3EDB:051F*/
    if(partyHasMech)
        Display_Text_From_Memory((uint8_t *)"\rAllocate men in 'Mechs"); /*0543*/
    Display_Text_From_Memory((uint8_t *)"\rInspect Character\rHeal Characters\rLoad Game\rSave Game"); /*055B*/
    Display_Text_From_Memory((uint8_t *)"\rShow Overhead Map"); /*0592*/
    uint16_t choice=Display_Menu_Choices_And_Check(PauseMenuPanel);
    if(partyHasMech) {
        switch(choice) {
        case PauseMenu_Settings: Menu_Change_Game_Settings(); break;
        case PauseMenu_Allocate: Menu_Assign_Pilots(); break;
        case PauseMenu_WithMechs_Inspect: Inspect_Characters(); break;
        case PauseMenu_WithMechs_Heal: Heal_Characters(MedicalService_UsePartyMedicAndEquipment); break;
        case PauseMenu_WithMechs_Load: Load_Game(); break;
        case PauseMenu_WithMechs_Save: Save_Game(); break;
        case PauseMenu_WithMechs_Map: Show_Overhead_Map(); break;
        }
    } else {
        switch(choice) {
        case PauseMenu_Settings: Menu_Change_Game_Settings(); break;
        case PauseMenu_NoMechs_Inspect: Inspect_Characters(); break;
        case PauseMenu_NoMechs_Heal: Heal_Characters(MedicalService_UsePartyMedicAndEquipment); break;
        case PauseMenu_NoMechs_Load: Load_Game(); break;
        case PauseMenu_NoMechs_Save: Save_Game(); break;
        case PauseMenu_NoMechs_Map: Show_Overhead_Map(); break;
        }
    }
    EGA_DrawBox_Wrapper();
}
