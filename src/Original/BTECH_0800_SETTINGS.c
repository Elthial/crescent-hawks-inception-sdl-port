#include "game.h"

/* Original0800:3BD0..3D3F, complete ASM checked. Retained EGA path.
 * Submenus operate within the caller's panel; no new panel activation.
 * BYTE outtake assignment truncates the menu WORD; sound XOR changes bit0
 * only. Movement arithmetic remains WORD wrapping, including invalid inputs. */
void Menu_Change_Game_Settings(void)
{
    Draw_Top_Graphic_Sidebar();
    Display_Text_From_Memory((uint8_t *)"Change movement rate\rSet combat speed\rTurn Sound O"); /*0896*/
    Display_Text_From_Memory((uint8_t *)(SoundEffectsEnabled?"ff":"n")); /*08C9/08CC*/
    Display_Text_From_Memory((uint8_t *)"\rChange Outtake Frequency\rQuit game\rCancel"); /*08CE*/
    uint16_t choice=Display_Menu_Choices_And_Check(GameSettingsMenu);
    switch(choice) {
    case GameSettings_MovementRate: {
        Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"Set movement rate to...\rOne step\rTwo steps\rFour steps"); /*08F9*/
        MenuControls[MovementRateMenu].optionCount=MovementRateChoiceCount;
        MenuControls[MovementRateMenu].baseRow=TRUE;
        MenuControls[MovementRateMenu].selection=(uint16_t)(ExplorationStepsPerInput-1);
        if(ExplorationStepsPerInput==ExplorationSteps_Four)
            --MenuControls[MovementRateMenu].selection;
        uint16_t selectedRate=Display_Menu_Choices_And_Check(MovementRateMenu);
        ExplorationStepsPerInput=(uint16_t)(selectedRate+1);
        if(ExplorationStepsPerInput==ExplorationSteps_Three)
            ++ExplorationStepsPerInput; /* Third choice denotes four, not three steps. */
        MenuControls[MovementRateMenu].baseRow=FALSE;
        if(CacheMapRoomLoaded) ExplorationStepsPerInput=ExplorationSteps_One;
        return;
    }
    case GameSettings_CombatSpeed:
        Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"Fastest\rFaster\rNormal\rSlower\rSlowest\rKeypress"); /*092F*/
        CombatSpeedSetting=Display_Menu_Choices_And_Check(CombatSpeedMenu);
        Draw_Health_and_C_Bills_Sidebar(TRUE);
        return;
    case GameSettings_ToggleSound:
        SoundEffectsEnabled^=TRUE;
        return;
    case GameSettings_OuttakeFrequency:
        Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"Show Outtakes\rVery Frequently\rFrequently\rInfrequently"); /*095D*/
        OuttakeFrequency=(uint8_t)Display_Menu_Choices_And_Check(OuttakeFrequencyMenu);
        Draw_Health_and_C_Bills_Sidebar(TRUE);
        return;
    case GameSettings_Quit:
        Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"Quitting will exit the game and return you to the system.  "); /*0993*/
        Display_Text_From_Memory((uint8_t *)"Are you sure you want to quit?"); /*09CF*/
        ExitMainLoop=Prompt_Yes_No(FALSE);
        return;
    default:
        return;
    }
}
