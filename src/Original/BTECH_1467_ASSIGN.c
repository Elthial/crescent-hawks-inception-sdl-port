#include "game.h"
#include "dos.h"

/* Original1467:0002..08A7, complete raw ASM checked. Nonzero resets all
 * party assignments; zero synchronizes surviving Mech seats without first
 * clearing characters. Native C startup DS=SS explains final0838 row reads.
 * Valid character/name/Mech/menu indices remain the native caller contract. */
void Assign_Pilot_and_rider_to_Mechs(uint16_t resetAssignments)
{
    if(resetAssignments) {
        for(uint16_t slot=0;slot<PartySize;++slot) Characters[slot].mechAssignment=Character_OnFoot;
        for(uint16_t slot=0;slot<LanceSize;++slot) {
            Mechs[slot].pilotId=MECH_NoPilot; Mechs[slot].riderId=MECH_NoRider;
        }
    } else {
        for(uint16_t slot=0;slot<LanceSize;++slot) {
            Mech *mech=&Mechs[slot];
            if(mech->name[0]==MECH_Destroyed) continue;
            if(mech->pilotId!=MECH_NoPilot) Characters[mech->pilotId].mechAssignment=(uint8_t)slot;
            if(mech->riderId!=MECH_NoRider) Characters[mech->riderId].mechAssignment=(uint8_t)slot;
        }
    }
    uint16_t characterSlotByRow[PartySize];
    for(uint16_t row=0;row<PartySize;++row) characterSlotByRow[row]=UINT16_MAX;
    uint16_t assignmentComplete=FALSE;
    do {
        Menu_Memory_Variables(CrewAssignmentPanel);
        Draw_Top_Graphic_Sidebar();
        Draw_Menu_Border(0);
        Set_Text_Colour_Bright_Green();
        Display_Text_From_Memory((uint8_t *)"Assign pilots and passengers:\x06\x0F\r"); /*2622*/
        MenuControls[PartyMechSelectionMenu].baseRow=TextRow;
        uint16_t livingCount=0;
        for(uint16_t slot=0;slot<PartySize;++slot) {
            Character *character=&Characters[slot];
            if(character->name==Character_Dead) continue;
            characterSlotByRow[livingCount++]=slot;
            Display_Text_From_Memory(CharacterNames[(int8_t)character->name]);
            int16_t mechId=(int8_t)character->mechAssignment;
            if(mechId==Character_OnFoot) Display_Text_From_Memory((uint8_t *)" on foot\r"); /*2643*/
            else {
                Mech *mech=&Mechs[mechId];
                DynamicString[0]=' ';
                (void)Append_Large_Text_To_Memory(DynamicString+1,mech->name);
                uint16_t last=(uint16_t)(Loop_Until_TextPtr_Null(DynamicString)-1);
                while(last && DynamicString[last]==' ') DynamicString[last--]=0;
                Display_Text_From_Memory(DynamicString);
                Display_Text_From_Memory((uint8_t *)(mech->pilotId==slot?" pilot\r":" rider\r")); /*264D/2655*/
            }
        }
        Display_Text_From_Memory((uint8_t *)"Done\r"); /*265D*/
        MenuControls[PartyMechSelectionMenu].optionCount=(uint16_t)(livingCount+1);
        TextRow=CrewAssignmentMechSummaryRow;
        for(uint16_t slot=0;slot<LanceSize;++slot) {
            Mech *mech=&Mechs[slot];
            if(mech->name[0]==MECH_Destroyed) continue;
            Display_Text_From_Memory(mech->name);
            Display_Text_From_Memory((uint8_t *)"\r\x06\x0F" "Pilot:\x06\x02 "); /*2663*/
            Display_Text_From_Memory(mech->pilotId==MECH_NoPilot?(uint8_t *)"None":CharacterNames[(int8_t)Characters[mech->pilotId].name]);
            Display_Text_From_Memory((uint8_t *)"\r\x06\x0F" "Rider:\x06\x02 "); /*2675*/
            Display_Text_From_Memory(mech->riderId==MECH_NoRider?(uint8_t *)"None":CharacterNames[(int8_t)Characters[mech->riderId].name]);
            TextColour=EGA_BrightWhite;
            Display_Text_4FA0_Value();
        }
        uint16_t characterRow=Display_Menu_Choices_And_Check(PartyMechSelectionMenu);
        if(characterRow==livingCount) {
            assignmentComplete=TRUE;
            for(uint16_t slot=0;slot<LanceSize;++slot)
                if(Mechs[slot].name[0]!=MECH_Destroyed && Mechs[slot].pilotId==MECH_NoPilot) assignmentComplete=FALSE;
            if(assignmentComplete) break;
            Menu_Memory_Variables(RefreshLowerPanel);
            Draw_Top_Graphic_Sidebar();
            Display_Text_From_Memory((uint8_t *)"\x06\x0FNot all of your 'Mechs have pilots.  "); /*2687*/
            uint16_t qualifiedCount=0,mechCount=0;
            for(uint16_t slot=0;slot<PartySize;++slot)
                if(Characters[slot].name!=Character_Dead && Characters[slot].skillPiloting!=SkillLevel_Unskilled) ++qualifiedCount;
            for(uint16_t slot=0;slot<LanceSize;++slot) if(Mechs[slot].name[0]!=MECH_Destroyed) ++mechCount;
            if((int16_t)qualifiedCount>=(int16_t)mechCount) {
                Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Assign a pilot to each."); /*271E*/
                (void)Keyboard_Get_ASCII_Hex_Input();
            } else {
                Wait_For_50Hz_Then_Check_Input();
                (void)Keyboard_Get_ASCII_Hex_Input();
                Draw_Top_Graphic_Sidebar();
                Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"\x06\x0FYou'll have to abandon any 'Mech that doesn't have a pilot."); /*26AF*/
                (void)Keyboard_Get_ASCII_Hex_Input();
                Draw_Top_Graphic_Sidebar();
                Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Are you ready to abandon your nonpiloted 'Mechs?"); /*26ED*/
                if(Prompt_Yes_No(FALSE))
                    for(uint16_t slot=0;slot<LanceSize;++slot) {
                        Mech *mech=&Mechs[slot];
                        if(mech->name[0]==MECH_Destroyed || mech->pilotId!=MECH_NoPilot) continue;
                        mech->name[0]=MECH_Destroyed;
                        for(uint16_t member=0;member<PartySize;++member)
                            if((int16_t)(int8_t)Characters[member].mechAssignment==(int16_t)slot)
                                Characters[member].mechAssignment=Character_OnFoot;
                    }
            }
            /* Even confirmed abandonment returns to the menu; Done again. */
            goto RedrawAssignmentPanel;
        }
        uint16_t partySlot=characterSlotByRow[characterRow];
        Character *character=&Characters[partySlot];
        Menu_Memory_Variables(RefreshLowerPanel);
        Draw_Top_Graphic_Sidebar();
        TextColour=EGA_BrightWhite;
        Display_Text_From_Memory(CharacterNames[(int8_t)character->name]);
        Display_Text_From_Memory((uint8_t *)" is "); /*2736*/
        if(character->skillPiloting==SkillLevel_Unskilled) {
            Set_Text_Colour_Bright_Green();
            Display_Text_From_Memory((uint8_t *)"not "); /*273B*/
        }
        Display_Text_From_Memory((uint8_t *)"\x06\x0F" "a qualified 'Mech pilot."); /*2740*/
        Menu_Memory_Variables(RefreshPartyPanel);
        Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"Assign to which vehicle?\r"); /*275B*/
        MenuControls[RefreshPartyPanel].baseRow=TextRow;
        uint16_t mechSlotByRow[LanceSize],vehicleCount=0;
        for(uint16_t slot=0;slot<LanceSize;++slot) mechSlotByRow[slot]=UINT16_MAX;
        for(uint16_t slot=0;slot<LanceSize;++slot) {
            if(Mechs[slot].name[0]==MECH_Destroyed) continue;
            (void)Append_Large_Text_To_Memory(DynamicString,Mechs[slot].name);
            DynamicString[CrewVehicleNameTemporaryTerminator]=0;
            /* Native displays ORIGINAL name, not its truncated scratch copy. */
            Display_Text_From_Memory(Mechs[slot].name);
            mechSlotByRow[vehicleCount++]=slot;
        }
        Display_Text_From_Memory((uint8_t *)"None"); /*2775*/
        MenuControls[RefreshPartyPanel].optionCount=(uint16_t)(vehicleCount+1);
        /* Detach before vehicle choice: native has no cancellation rollback. */
        for(uint16_t slot=0;slot<LanceSize;++slot) {
            if(Mechs[slot].pilotId==partySlot) Mechs[slot].pilotId=MECH_NoPilot;
            if(Mechs[slot].riderId==partySlot) Mechs[slot].riderId=MECH_NoRider;
        }
        uint16_t vehicleRow=Display_Menu_Choices_And_Check(RefreshPartyPanel);
        if(vehicleRow==vehicleCount) { character->mechAssignment=Character_OnFoot; goto RedrawAssignmentPanel; }
        uint16_t mechId=mechSlotByRow[vehicleRow];
        Mech *mech=&Mechs[mechId];
        uint16_t assignAsPilot=FALSE;
        if(character->skillPiloting!=SkillLevel_Unskilled) {
            if(mech->pilotId==MECH_NoPilot) assignAsPilot=TRUE;
            else {
                Draw_Top_Graphic_Sidebar();
                Display_Text_From_Memory((uint8_t *)"Assign to this 'Mech as the pilot?"); /*277A*/
                assignAsPilot=Prompt_Yes_No(TRUE);
            }
        }
        uint16_t previous=assignAsPilot?mech->pilotId:mech->riderId;
        if(previous!=(assignAsPilot?MECH_NoPilot:MECH_NoRider)) {
            if(assignAsPilot) {
                if(mech->riderId!=MECH_NoRider) Characters[mech->riderId].mechAssignment=Character_OnFoot;
                mech->riderId=(uint8_t)previous;
            } else Characters[previous].mechAssignment=Character_OnFoot;
        }
        if(TraitorInParty && (int16_t)partySlot==(int16_t)(int8_t)TraitorCharacterId) {
            Draw_Message_Box();
            Display_Text_From_Memory((uint8_t *)"Something about "); /*279D*/
            Display_Text_From_Memory(CharacterNames[(int8_t)character->name]);
            Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)" makes you uneasy about putting him in a 'Mech cockpit."); /*27AE*/
            TraitorWarning=TRUE;
            (void)Keyboard_Get_ASCII_Hex_Input();
        }
        if(assignAsPilot) mech->pilotId=(uint8_t)partySlot;
        else mech->riderId=(uint8_t)partySlot;
        character->mechAssignment=(uint8_t)mechId;
RedrawAssignmentPanel:
        Draw_Top_Graphic_Sidebar();
    } while(!assignmentComplete);
    for(uint16_t slot=0;slot<LanceSize;++slot) StoredPartyMechNameInitial[slot]=MECH_Destroyed;
    PartyMechNamesHidden=FALSE;
}
