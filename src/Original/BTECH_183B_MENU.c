#include "game.h"


/* Original183B:14C3..1773, complete raw ASM checked. Planning returns
 *0 Begin Fight or1 Flee; it does not execute the combat round. Caller must
 *supply an active friendly: native Next Unit has no all-inactive escape. */
uint16_t Combat_UI_Menu_Logic(void)
{
    uint16_t initializeDefaultChoice=TRUE,selectedCombatantId=0;
    while((int16_t)selectedCombatantId<Enemy_All_CombatantId_Range_First && !CombatantActive[selectedCombatantId])
        ++selectedCombatantId;
    uint16_t fleeRequested=FALSE,finishPlanning=FALSE;
    if(DisableComputerControl) CombatantActionState[selectedCombatantId]=0;
    while(!finishPlanning) {
        Menu_Memory_Variables(4); Draw_Top_Graphic_Sidebar();
        Display_Friendly_Combatant_Description(selectedCombatantId,FALSE,FALSE);
        TextColour=15;
        if(CombatantActionState[selectedCombatantId]) {
            Display_Text_From_Memory((uint8_t *)"\rThe computer is currently moving this unit.");
            Combat_Computer_Control(selectedCombatantId,TRUE);
        } else Combat_Render_Movement_Preview(selectedCombatantId,TRUE);
        Menu_Memory_Variables(3); Draw_Top_Graphic_Sidebar(); TextColour=15;
        if((int16_t)selectedCombatantId<Friendly_Infantry_Combatant_Range_First) {
            Display_Text_From_Memory((uint8_t *)"Walk\rRun\rJump\rUse Weapons\rKick\rComputer\rScan Unit\rNext Unit\rFlee\rBegin Fight\r");
            CombatMessageMenuOptionCount=CombatMechPlanningChoiceCount;
        } else {
            Display_Text_From_Memory((uint8_t *)"Move\rClear Moves\rUse Weapon\rComputer\rScan Unit\rNext Unit\rFlee\rBegin Fight\r");
            CombatMessageMenuOptionCount=CombatPersonnelPlanningChoiceCount;
        }
        if(initializeDefaultChoice) {
            CombatMessageMenuDefaultOption=(uint16_t)(CombatMessageMenuOptionCount-CombatPlanningBeginOffset);
            initializeDefaultChoice=FALSE;
        }
        Drain_Pending_Keyboard_Input();
        uint16_t choice=Display_Menu_Choices_And_Check(3);
        uint16_t fleeChoice=(uint16_t)(CombatMessageMenuOptionCount-CombatPlanningFleeOffset);
        if((int16_t)choice>=(int16_t)fleeChoice) {
            ++finishPlanning;
            if(choice==fleeChoice) fleeRequested=TRUE;
        }
        if(((int16_t)selectedCombatantId<Friendly_Infantry_Combatant_Range_First && (int16_t)choice<MovementMode_Jump+1) ||
           ((int16_t)selectedCombatantId>=Friendly_Infantry_Combatant_Range_First && choice==CombatPersonnelChoice_Move))
            Combat_Select_Movement_Plan_For_Turn(selectedCombatantId,choice);
        if((int16_t)selectedCombatantId>=Friendly_Infantry_Combatant_Range_First && choice==CombatPersonnelChoice_ClearMoves) {
            for(uint16_t byte=0;byte<CombatMovementOrderBytes;++byte)
                CombatMovementOrders[selectedCombatantId*CombatMovementOrderBytes+byte]=UINT8_MAX;
            CombatantActionState[selectedCombatantId]=0;
            Combat_Render_Movement_Preview(selectedCombatantId,FALSE);
        }
        if((int16_t)selectedCombatantId<Friendly_Infantry_Combatant_Range_First && choice==CombatMechMenuKickOption)
            Combat_Kick_Target(selectedCombatantId);
        if((((int16_t)selectedCombatantId<Friendly_Infantry_Combatant_Range_First && choice==CombatMechChoice_Computer) ||
            ((int16_t)selectedCombatantId>=Friendly_Infantry_Combatant_Range_First && choice==CombatPersonnelChoice_Computer)) && !DisableComputerControl)
            Combat_Computer_Control(selectedCombatantId,TRUE);
        if(((int16_t)selectedCombatantId<Friendly_Infantry_Combatant_Range_First && choice==CombatMechChoice_Weapons) ||
           ((int16_t)selectedCombatantId>=Friendly_Infantry_Combatant_Range_First && choice==CombatPersonnelChoice_Weapon))
            Combat_Weapon_UI(selectedCombatantId);
        if(((int16_t)selectedCombatantId<Friendly_Infantry_Combatant_Range_First && choice==CombatMechChoice_Scan) ||
           ((int16_t)selectedCombatantId>=Friendly_Infantry_Combatant_Range_First && choice==CombatPersonnelChoice_Scan))
            Scan_Enemies(selectedCombatantId);
        if(choice==(uint16_t)(CombatMessageMenuOptionCount-CombatPlanningNextUnitOffset)) {
            do {
                ++selectedCombatantId;
                if((int16_t)selectedCombatantId>=Enemy_All_CombatantId_Range_First) selectedCombatantId=0;
            } while(!CombatantActive[selectedCombatantId]);
            CombatMessageMenuDefaultOption=(uint16_t)(((int16_t)selectedCombatantId<Friendly_Infantry_Combatant_Range_First?
                CombatMechPlanningChoiceCount:CombatPersonnelPlanningChoiceCount)-CombatPlanningNextUnitOffset);
        }
    }
    return fleeRequested;
}
