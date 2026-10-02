#include "game.h"


/* Original1467:0B98..0D7D, complete ASM checked. Return ROW COUNT while
 * SelectedMechId returns the actual record ID. Saved initials make staged
 * hidden names selectable without restoring the live name byte. The direct
 * unsigned PilotId -> CharacterNames lookup is original, NOT Infantry.Name.
 * No absent-pilot or zero-row guard is invented. Native valid pilot/name and
 * menu row indices remain the host array contract. */
uint16_t Display_Text_Mech_Names(void)
{
    for(uint16_t slot=0;slot<LanceSize;++slot) MechSlotByMenuRow[slot]=0;
    SelectedMechId=0;
    uint16_t rowCount=0;
    for(uint16_t slot=0;slot<LanceSize;++slot) {
        uint8_t initial=PartyMechNamesHidden?StoredPartyMechNameInitial[slot]:Mechs[slot].name[0];
        if(initial!=MECH_Destroyed) MechSlotByMenuRow[rowCount++]=(uint8_t)slot;
    }
    SelectedMechId=(uint16_t)(int16_t)(int8_t)MechSlotByMenuRow[0];
    MenuControls[PartyMechSelectionMenu].baseRow=TextRow;
    for(uint16_t row=0;row<rowCount;++row) {
        SelectedMechId=(uint16_t)(int16_t)(int8_t)MechSlotByMenuRow[row];
        Mech *mech=&Mechs[SelectedMechId];
        Display_Text_From_Memory(CharacterNames[mech->pilotId]);
        Display_Text_From_Memory((uint8_t *)"'s "); /*3EDB:2900*/
        if(!PartyMechNamesHidden) Display_Text_From_Memory(mech->name);
        else {
            DynamicString[0]=StoredPartyMechNameInitial[SelectedMechId];
            (void)Append_Large_Text_To_Memory(DynamicString+1,mech->name+1);
            Display_Text_From_Memory(DynamicString);
        }
        Display_Text_4FA0_Value();
    }
    MenuControls[PartyMechSelectionMenu].optionCount=rowCount;
    MenuControls[PartyMechSelectionMenu].selection=0;
    uint16_t selectedRow=Display_Menu_Choices_And_Check(PartyMechSelectionMenu);
    SelectedMechId=(uint16_t)(int16_t)(int8_t)MechSlotByMenuRow[selectedRow];
    MenuControls[PartyMechSelectionMenu].baseRow=1;
    return rowCount;
}
