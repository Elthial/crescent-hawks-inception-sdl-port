#include "game.h"

/* 11B8:152F..16B1. Sol: Hide companions and friendly Mechs during the prison
 * escape, restore their name bytes afterwards, then award one reference
 * Stinger in the first empty slot if Jason survived. No pilot assignment,
 * boarding, or full-roster replacement occurs in the original method. */
void Run_Jailbreak_Mission_And_Award_Stinger(void)
{
    for (uint16_t partyMember = 1; partyMember < PartySize; ++partyMember) {
        SavedPartyNameId[partyMember] = Characters[partyMember].name;
        Characters[partyMember].name = Character_Dead;
    }
    for (uint16_t mechSlot = 0; mechSlot < LanceSize; ++mechSlot) {
        StoredPartyMechNameInitial[mechSlot] = Mechs[mechSlot].name[0];
        Mechs[mechSlot].name[0] = MECH_Destroyed;
    }
    CrescentHawkMapPositionX = JailEntranceX;
    CrescentHawkMapPositionY = JailEntranceY;
    Characters[Character_Jason].mechAssignment = Character_OnFoot;
    Mech_Mission(Mission_Jailbreak);
    CrescentHawkMapPositionX = JailEscapeX;
    CrescentHawkMapPositionY = JailEscapeY;
    for (uint16_t partyMember = 1; partyMember < PartySize; ++partyMember)
        Characters[partyMember].name = SavedPartyNameId[partyMember];
    for (uint16_t mechSlot = 0; mechSlot < LanceSize; ++mechSlot) {
        Mechs[mechSlot].name[0] = StoredPartyMechNameInitial[mechSlot];
        StoredPartyMechNameInitial[mechSlot] = MECH_Destroyed;
    }
    if (MainCharactersAlive != 0) {
        for (uint16_t mechSlot = 0; mechSlot < LanceSize; ++mechSlot) {
            if (Mechs[mechSlot].name[0] == MECH_Destroyed) {
                uint8_t *destination = (uint8_t *)&Mechs[mechSlot];
                const uint8_t *reference = (const uint8_t *)&MechRefs[MechRef_Stinger];
                for (uint16_t recordByte = 0; recordByte < MechRecordSize; ++recordByte)
                    destination[recordByte] = reference[recordByte];
                break;
            }
        }
    }
}
