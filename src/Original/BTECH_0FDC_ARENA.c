#include "game.h"

/* 0FDC:1A26..1B40. Sol: Save crew and slot zero, stage the selected owned
 * Mech in slot zero, hide companions and choose owned-Mech restoration.
 * This method neither saves world positions nor starts combat. */
void Prepare_Party_Mech_For_Arena(void)
{
    for (uint16_t mechSlot = 0; mechSlot < LanceSize; ++mechSlot)
    {
        SavedArenaMechPilotId[mechSlot] = Mechs[mechSlot].pilotId;
        SavedArenaMechRiderId[mechSlot] = Mechs[mechSlot].riderId;
    }
    Draw_Top_Graphic_Sidebar();
    Display_Text_From_Memory((uint8_t *)"Which 'Mech will you take into combat?\r"); /* 3EDB:17A5 */
    Display_Text_Mech_Names();
    uint8_t *arenaMech = (uint8_t *)&Mechs[0];
    for (uint16_t recordOffset = 0; recordOffset < MechRecordSize; ++recordOffset)
    {
        ArenaMechRecordBackup[recordOffset] = arenaMech[recordOffset];
        arenaMech[recordOffset] = ((uint8_t *)&Mechs[SelectedMechId])[recordOffset];
    }
    Mechs[0].name[0] = StoredPartyMechNameInitial[SelectedMechId];
    for (uint16_t partyMemberSlot = 1; partyMemberSlot < PartySize; ++partyMemberSlot)
    {
        /* Sol: Paired native C620+17*i/C60F+17*i stores address the same
         * record field in adjacent members. Retain their interleaved order. */
        Characters[partyMemberSlot].mechAssignment = ArenaStagingMechAssignment;
        Characters[partyMemberSlot-1].mechAssignment = ArenaStagingMechAssignment;
        SavedPartyNameId[partyMemberSlot] = Characters[partyMemberSlot].name;
        Characters[partyMemberSlot].name = Character_Dead;
    }
    Mechs[0].pilotId = Character_Jason;
    Mechs[0].riderId = MECH_NoRider;
    ArenaRentalMechMode = FALSE;
}

/* 0FDC:1B41..1C9A. Sol: Return owned-Mech combat damage to its roster slot
 * or discard the rental; restore records/crew as appropriate, then dismount
 * everyone and restore companion names. Slot-zero self-copy is intentional. */
void Restore_Party_After_Arena_Combat(void)
{
    uint8_t *arenaMech = (uint8_t *)&Mechs[0];
    if (ArenaRentalMechMode != FALSE)
    {
        for (uint16_t recordOffset = 0; recordOffset < MechRecordSize; ++recordOffset)
            arenaMech[recordOffset] = ArenaMechRecordBackup[recordOffset];
    }
    else
    {
        for (uint16_t recordOffset = 0; recordOffset < MechRecordSize; ++recordOffset)
            ((uint8_t *)&Mechs[SelectedMechId])[recordOffset] = arenaMech[recordOffset];
        if (arenaMech[0] == MECH_Destroyed)
            StoredPartyMechNameInitial[SelectedMechId] = MECH_Destroyed;
        ((uint8_t *)&Mechs[SelectedMechId])[0] = MECH_Destroyed;
        if (SelectedMechId != 0)
        {
            for (uint16_t recordOffset = 0; recordOffset < MechRecordSize; ++recordOffset)
                arenaMech[recordOffset] = ArenaMechRecordBackup[recordOffset];
        }
        for (uint16_t mechSlot = 0; mechSlot < LanceSize; ++mechSlot)
        {
            Mechs[mechSlot].pilotId = SavedArenaMechPilotId[mechSlot];
            Mechs[mechSlot].riderId = SavedArenaMechRiderId[mechSlot];
        }
    }
    for (uint16_t partyMemberSlot = 1; partyMemberSlot < PartySize; ++partyMemberSlot)
    {
        Characters[partyMemberSlot].mechAssignment = Character_OnFoot;
        Characters[partyMemberSlot-1].mechAssignment = Character_OnFoot;
        Characters[partyMemberSlot].name = SavedPartyNameId[partyMemberSlot];
    }
}

/* 0FDC:1C9B..1D2F. Sol: Back up slot zero and install the complete reference
 * Locust, hide companions and choose rental restoration. Pilot and positions
 * are left to the caller; do not invent initialization here. */
void Prepare_Rental_Locust_For_Arena(void)
{
    uint8_t *arenaMech = (uint8_t *)&Mechs[0];
    uint8_t *referenceLocust = (uint8_t *)&MechRefs[MECH_REF_Locust];
    for (uint16_t recordOffset = 0; recordOffset < MechRecordSize; ++recordOffset)
    {
        ArenaMechRecordBackup[recordOffset] = arenaMech[recordOffset];
        arenaMech[recordOffset] = referenceLocust[recordOffset];
    }
    for (uint16_t partyMemberSlot = 1; partyMemberSlot < PartySize; ++partyMemberSlot)
    {
        Characters[partyMemberSlot].mechAssignment = ArenaStagingMechAssignment;
        Characters[partyMemberSlot-1].mechAssignment = ArenaStagingMechAssignment;
        SavedPartyNameId[partyMemberSlot] = Characters[partyMemberSlot].name;
        Characters[partyMemberSlot].name = Character_Dead;
    }
    ArenaRentalMechMode = TRUE;
}
