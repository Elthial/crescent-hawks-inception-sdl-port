#include "game.h"

/* Original0DAB:1467..174B, full ASM checked. Side base0/12 and actor IDs
 * must name actual native arrays. No eligible-target escape is added: original
 * cycles indefinitely until an eligible target supplies a nonzero menu choice.
 * Native unused coordinates are FFFF, not the abbreviated00FF ASM display. */
void Combat_Browse_Scan_Targets(uint16_t scanningCombatantId,uint16_t sideBase,uint16_t includeMechDetails)
{
    uint16_t target=sideBase;
    if(!includeMechDetails) target+=LanceSize;
    uint16_t exitChoice=0;
    do {
        if(CombatantActive[target] && CombatantPackedX[target]!=CombatantPosition_Unused &&
            CombatantPackedY[target]!=CombatantPosition_Unused) {
            Menu_Memory_Variables(RefreshLowerPanel);
            Draw_Top_Graphic_Sidebar();
            TextColour=EGA_BrightWhite;
            if((int16_t)target<Enemy_All_CombatantId_Range_First)
                Display_Friendly_Combatant_Description(target,TRUE,TRUE);
            else {
                uint8_t *description;
                if((int16_t)target<Enemy_Infantry_CombatantId_Range_First) {
                    Display_Text_From_Memory((uint8_t *)"Enemy\r"); /*DS1148*/
                    description=Mechs[target-EnemyCombatantToRecordOffset].name;
                } else {
                    uint16_t record=(uint16_t)(target-EnemyCombatantToRecordOffset);
                    Display_Text_From_Memory((uint8_t *)"Enemy human"); /*114F*/
                    Display_Text_Human_Health(record);
                    TextColumn=0; TextRow=CombatDescriptionHeadingRow;
                    Set_Text_Colour_Bright_Green();
                    Display_Text_From_Memory((uint8_t *)"Weapon:\r\x06\x0F"); /*115B*/
                    TextColumn=0; TextRow=CombatDescriptionWeaponRow;
                    description=WeaponStats[(int8_t)Characters[record].weapon].name;
                }
                Display_Text_From_Memory(description);
                Set_Text_Colour_Bright_Green();
                Display_Text_From_Memory((uint8_t *)"\rDirection:\x06\x0F"); /*1166*/
                int16_t direction=Get_Target_Compass_Direction(CombatantPackedX[scanningCombatantId],
                    CombatantPackedY[scanningCombatantId],CombatantPackedX[target],CombatantPackedY[target]);
                Display_Text_From_Memory(CompassDirectionText[direction]);
            }
            Combat_Render_Movement_Preview(target,TRUE);
            Menu_Memory_Variables(CombatSettingsMenuLayout);
            Draw_Top_Graphic_Sidebar();
            MenuControls[CombatSettingsMenuLayout].optionCount=ScanBrowserBaseChoiceCount;
            Display_Text_From_Memory((uint8_t *)"\x06\x0FScan...\rNext Unit\r"); /*1174*/
            uint16_t detailOffered=FALSE;
            if(includeMechDetails && ((int16_t)target<Friendly_Infantry_Combatant_Range_First ||
                ((int16_t)target>=Enemy_All_CombatantId_Range_First && (int16_t)target<Enemy_Infantry_CombatantId_Range_First))) {
                Display_Text_From_Memory((uint8_t *)"Detail Scan"); /*1189: no trailing CR in original.*/
                MenuControls[CombatSettingsMenuLayout].optionCount=ScanBrowserDetailChoiceCount;
                detailOffered=TRUE;
            }
            Display_Text_From_Memory((uint8_t *)"Done"); /*1195*/
            MenuControls[CombatSettingsMenuLayout].baseRow=ScanMenuFirstChoiceRow;
            MenuControls[CombatSettingsMenuLayout].selection=0;
            exitChoice=Display_Menu_Choices_And_Check(CombatSettingsMenuLayout);
            if(exitChoice==ScanBrowser_Detail && detailOffered) {
                exitChoice=0;
                Examine_Screen_BTSTATS_CMP(target);
                Menu_Draw_MultiSelect(TRUE);
                --target; /* Cancels unconditional increment: redisplay the SAME actor. */
            }
        }
        ++target;
        if((int16_t)target>(int16_t)(uint16_t)(sideBase+CombatantsPerSide-1)) {
            target=sideBase;
            if(!includeMechDetails) target+=LanceSize;
        }
        if(ArenaRentalMechMode && (int16_t)target>=Enemy_All_CombatantId_Range_First)
            target=Enemy_All_CombatantId_Range_First;
    } while(!exitChoice);
    MenuControls[CombatSettingsMenuLayout].baseRow=0;
    MenuControls[CombatSettingsMenuLayout].selection=CombatMenu_MechScanChoice;
    /* Native RedrawPreviousMenuOnExit local is zero and never written.
     * Its dead conditional redraw therefore has no host call. */
    TextColour=EGA_BrightWhite;
}
