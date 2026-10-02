#include "game.h"
#include "dos.h"

uint8_t *SelectedMovementModeText[MovementMode_Jump+1]={(uint8_t *)"walk",(uint8_t *)"run",(uint8_t *)"jump"}; /*3EDB:3CD4*/

/* Original183B:1C1F..2230, all raw ASM checked. Cursor moves overwrite
 * ONE provisional waypoint, not append per key. Space/Return finish; no
 * invented Escape rollback. Native caller has already rendered the endpoint. */
void Combat_Select_Movement_Plan_For_Turn(uint16_t combatantId,uint16_t selectedMovementMode)
{
    uint16_t orderStart=(uint16_t)(combatantId*CombatMovementOrderBytes);
    if((int16_t)combatantId<Enemy_All_CombatantId_Range_First) {
        if(CombatantActionState[combatantId]) {
            for(uint16_t byte=0;byte<CombatMovementOrderBytes;++byte) CombatMovementOrders[orderStart+byte]=UINT8_MAX;
            Combat_Render_Movement_Preview(combatantId,TRUE); /*Before clearing action state*/
        }
        CombatantActionState[combatantId]=0;
    }
    uint16_t acceptMode=TRUE;
    int16_t existingMode=(int8_t)CombatMovementOrders[orderStart];
    if((int16_t)combatantId<Friendly_Infantry_Combatant_Range_First && existingMode!=-1 &&
        (selectedMovementMode!=MovementMode_Jump || Mechs[combatantId].jumpMove!=JumpJets_Removed) &&
        (uint16_t)existingMode!=selectedMovementMode) {
        --acceptMode;
        Menu_Memory_Variables(RefreshLowerPanel); Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"This mech is now "); /*3B24*/
        (void)Append_Large_Text_To_Memory(DynamicString,SelectedMovementModeText[existingMode]);
        if(existingMode==MovementMode_Run) (void)Append_Text_To_Memory(DynamicString,(uint8_t *)"n");
        (void)Append_Text_To_Memory(DynamicString,(uint8_t *)"ing."); /*3B38 punctuation*/
        Display_Text_From_Memory(DynamicString);
        Display_Text_From_Memory((uint8_t *)"\rDo you wish to "); /*3B3D*/
        Display_Text_From_Memory(SelectedMovementModeText[selectedMovementMode]);
        Display_Text_From_Memory((uint8_t *)" instead?");
        if(Prompt_Yes_No(TRUE)) {
            ++acceptMode; Draw_Top_Graphic_Sidebar();
            Display_Text_From_Memory((uint8_t *)"All your current movement orders have been deleted.");
            Display_Text_From_Memory((uint8_t *)"\rPress a key to enter new moves."); /*3B8C*/
            for(uint16_t byte=0;byte<CombatMovementOrderBytes;++byte) CombatMovementOrders[orderStart+byte]=UINT8_MAX;
            Combat_Render_Movement_Preview(combatantId,FALSE);
            (void)Keyboard_Get_ASCII_Hex_Input();
        }
    }
    if(!acceptMode) goto PlanningDone;
    uint16_t destinationByte=0;
    while(destinationByte<CombatMovementOrderBytes && CombatMovementOrders[orderStart+destinationByte]!=UINT8_MAX)
        destinationByte+=CombatMovementDestinationBytes;
    if((int16_t)combatantId<Friendly_Infantry_Combatant_Range_First) Combat_Mech_Movement(combatantId,selectedMovementMode);
    else Combat_Infantry_Movement(combatantId);
    Combat_Calculate_Movement(combatantId);
    /* Native unused endpoint copies to BP-16/-1A intentionally omitted. */
    Menu_Memory_Variables(RefreshLowerPanel); Draw_Top_Graphic_Sidebar();
    if(!CharacterMovementPointsRemaining) {
        if(selectedMovementMode==MovementMode_Jump && Mechs[combatantId].jumpMove==JumpJets_Removed) {
            Display_Text_From_Memory((uint8_t *)"This 'Mech has no jump jets."); goto WaitForMovementMessage;
        }
        if((int16_t)combatantId<Friendly_Infantry_Combatant_Range_First && MechHeatLevel[combatantId]==MechHeatShutdownLevel) {
            Display_Text_From_Memory((uint8_t *)"This 'Mech has overheated and shut down."); goto WaitForMovementMessage;
        }
        if((int16_t)combatantId>=Friendly_Infantry_Combatant_Range_First && (int16_t)combatantId<Enemy_All_CombatantId_Range_First) {
            CurrentMechHeatMovementPenalty=0; CurrentMechLegDamage=0;
        }
        if(CurrentMechLegDamage || CurrentMechHeatMovementPenalty) {
            Display_Text_From_Memory((uint8_t *)"Movement is inhibited by ");
            if(CurrentMechLegDamage) Display_Text_From_Memory((uint8_t *)"leg damage.\r"); /*3C0D*/
            else if(CurrentMechHeatMovementPenalty) Display_Text_From_Memory((uint8_t *)"this 'Mech's heat level.");
        } else {
            Display_Text_From_Memory((uint8_t *)"You can't ");
            Display_Text_From_Memory((int16_t)combatantId<Friendly_Infantry_Combatant_Range_First?
                SelectedMovementModeText[selectedMovementMode]:(uint8_t *)"move");
            Display_Text_From_Memory((uint8_t *)" any further this turn.");
            if((int16_t)combatantId<Friendly_Infantry_Combatant_Range_First && selectedMovementMode!=MovementMode_Run)
                Display_Text_From_Memory((uint8_t *)"\rYou might move farther if you tried running.");
        }
        CurrentMechHeatMovementPenalty=0; CurrentMechLegDamage=0;
WaitForMovementMessage:
        Prompt_And_Wait_For_Key(); goto PlanningDone;
    }
    Display_Text_From_Memory((uint8_t *)"You have ");
    Display_Text_Dynamic_Value(CharacterMovementPointsRemaining);
    Display_Text_From_Memory((uint8_t *)" movement points left.  Move the cursor to where you want to go.");
    (void)DrawCall_Combat_Menu(MovementPreviewEndpointColumn,MovementPreviewEndpointRow,1,CombatDestinationCursorYellow);
    uint16_t moveCommand;
    do {
        moveCommand=Keyboard_Convert_To_MoveCommands(Keyboard_Get_ASCII_Hex_Input());
        int16_t deltaX=0,deltaY=0;
        switch(moveCommand) {
        case Command_MoveSouthEast: deltaX=1; deltaY=1; break;
        case Command_MoveSouth: deltaY=1; break;
        case Command_MoveSouthWest: deltaX=-1; deltaY=1; break;
        case Command_MoveEast: deltaX=1; break;
        case Command_MoveWest: deltaX=-1; break;
        case Command_MoveNorthEast: deltaX=1; deltaY=-1; break;
        case Command_MoveNorth: deltaY=-1; break;
        case Command_MoveNorthWest: deltaX=-1; deltaY=-1; break;
        }
        MovementPreviewEndpointColumn=(uint16_t)(MovementPreviewEndpointColumn+deltaX);
        if((int16_t)MovementPreviewEndpointColumn<MapViewportLeftByte || (int16_t)MovementPreviewEndpointColumn>MapViewportLastCellX)
            MovementPreviewEndpointColumn=(uint16_t)(MovementPreviewEndpointColumn-deltaX);
        MovementPreviewEndpointRow=(uint16_t)(MovementPreviewEndpointRow+deltaY);
        if((int16_t)MovementPreviewEndpointRow<0 || (int16_t)MovementPreviewEndpointRow>MapViewportLastCellY)
            MovementPreviewEndpointRow=(uint16_t)(MovementPreviewEndpointRow-deltaY);
        if(deltaX || deltaY) {
            uint16_t savedMapX=CrescentHawkMapPositionX,savedMapY=CrescentHawkMapPositionY;
            Offset_Packed_Position((int16_t)(uint16_t)(MovementPreviewEndpointColumn-CombatPreviewOriginColumn),
                (int16_t)(uint16_t)(MovementPreviewEndpointRow-CombatPreviewOriginRow));
            uint16_t destinationX=CrescentHawkMapPositionX&PackedPositionLocalMask;
            uint16_t destinationY=CrescentHawkMapPositionY&PackedPositionLocalMask;
            uint16_t region=(uint16_t)(((CrescentHawkMapPositionY&PackedPositionYRegionMask)|
                (CrescentHawkMapPositionX&PackedPositionXRegionMask))>>8);
            uint16_t writeDestination=TRUE;
            for(uint16_t byte=0;byte<destinationByte;byte+=CombatMovementDestinationBytes)
                if((uint16_t)(int16_t)(int8_t)CombatMovementOrders[orderStart+byte+1]==region &&
                    (uint16_t)(int16_t)(int8_t)CombatMovementOrders[orderStart+byte+2]==destinationX &&
                    (uint16_t)(int16_t)(int8_t)CombatMovementOrders[orderStart+byte+3]==destinationY) {
                    writeDestination=FALSE; CombatMovementOrders[orderStart+destinationByte]=UINT8_MAX;
                }
            if(writeDestination) {
                /* Native BUG-018: no destinationByte<48 guard here. Flat
                 * native allocation preserves a full-row next-unit overwrite. */
                CombatMovementOrders[orderStart+destinationByte]=(uint8_t)selectedMovementMode;
                CombatMovementOrders[orderStart+destinationByte+1]=(uint8_t)region;
                CombatMovementOrders[orderStart+destinationByte+2]=(uint8_t)destinationX;
                CombatMovementOrders[orderStart+destinationByte+3]=(uint8_t)destinationY;
            }
            CrescentHawkMapPositionX=savedMapX; CrescentHawkMapPositionY=savedMapY;
            uint16_t savedCursorX=MovementPreviewEndpointColumn,savedCursorY=MovementPreviewEndpointRow;
            Combat_Render_Movement_Preview(combatantId,FALSE);
            MovementPreviewEndpointColumn=savedCursorX; MovementPreviewEndpointRow=savedCursorY;
            (void)DrawCall_Combat_Menu(savedCursorX,savedCursorY,1,CombatDestinationCursorYellow);
        }
    } while(moveCommand!=MovementPlanningFinish_Space && moveCommand!=MovementPlanningFinish_Return);
PlanningDone:
    Draw_Top_Graphic_Sidebar();
}
