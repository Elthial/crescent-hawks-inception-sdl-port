/* Sol: Original arena bodies; only selection/drawing use test adapters.
 * Synthetic state witnesses, not combat/gameplay validation. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, draws, selections, prompts;
static uint16_t selectedSlot;
static Mech originalMechs[MechRecordCount];
static Character originalCharacters[CharacterRecordCount];
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Arena check %u failed\n",checks); exit(1); }
}
void Draw_Top_Graphic_Sidebar(void) { ++draws; }
void Display_Text_From_Memory(uint8_t *text)
{
    check(!strcmp((char *)text,"Which 'Mech will you take into combat?\r")); ++prompts;
}
uint16_t Display_Text_Mech_Names(void)
{
    check(draws == 1 && prompts == 1);
    for (unsigned i = 0; i < LanceSize; ++i) {
        check(SavedArenaMechPilotId[i] == originalMechs[i].pilotId);
        check(SavedArenaMechRiderId[i] == originalMechs[i].riderId);
    }
    ++selections; SelectedMechId = selectedSlot; return selectedSlot;
}
static void reset(void)
{
    for (unsigned i = 0; i < MechRecordCount; ++i) {
        uint8_t *record = (uint8_t *)&Mechs[i];
        for (unsigned byte = 0; byte < MechRecordSize; ++byte) record[byte] = (uint8_t)(i * 17 + byte);
        Mechs[i].name[0] = (uint8_t)('A' + i);
        Mechs[i].pilotId = (uint8_t)i; Mechs[i].riderId = (uint8_t)(i + LanceSize);
    }
    for (unsigned i = 0; i < CharacterRecordCount; ++i) {
        memset(&Characters[i],(int)i,sizeof Characters[i]);
        Characters[i].name = (uint8_t)(i % CharacterNameCount);
        Characters[i].mechAssignment = (uint8_t)(i % LanceSize);
    }
    Characters[6].name = Character_Dead;
    for (unsigned i = 0; i < LanceSize; ++i) StoredPartyMechNameInitial[i] = Mechs[i].name[0];
    memset(SavedArenaMechPilotId,0xAA,sizeof SavedArenaMechPilotId);
    memset(SavedArenaMechRiderId,0xBB,sizeof SavedArenaMechRiderId);
    memset(SavedPartyNameId,0xCC,sizeof SavedPartyNameId);
    memset(ArenaMechRecordBackup,0xDD,sizeof ArenaMechRecordBackup);
    memcpy(originalMechs,Mechs,sizeof Mechs); memcpy(originalCharacters,Characters,sizeof Characters);
    CrescentHawkMapPositionX = 123; CrescentHawkMapPositionY = 456;
    selectedSlot = 0; SelectedMechId = 3; ArenaRentalMechMode = UINT16_MAX;
    draws = selections = prompts = 0;
}
static void checkHiddenParty(void)
{
    check(Characters[0].name == originalCharacters[0].name && SavedPartyNameId[0] == 0xCC);
    for (unsigned i = 0; i < PartySize; ++i) {
        check(Characters[i].mechAssignment == ArenaStagingMechAssignment);
        if (i) check(Characters[i].name == Character_Dead && SavedPartyNameId[i] == originalCharacters[i].name);
    }
    check(!memcmp(&Characters[PartySize],&originalCharacters[PartySize],
        (CharacterRecordCount - PartySize) * sizeof(Character)));
    check(CrescentHawkMapPositionX == 123 && CrescentHawkMapPositionY == 456);
}
static void checkRestoredParty(void)
{
    for (unsigned i = 0; i < PartySize; ++i) {
        Character expected = originalCharacters[i]; expected.mechAssignment = Character_OnFoot;
        if (i == 0) expected.health = 17; /* Caller/combat damage is not undone. */
        check(!memcmp(&Characters[i],&expected,sizeof expected));
    }
    check(CrescentHawkMapPositionX == 123 && CrescentHawkMapPositionY == 456);
}
int main(void)
{
    for (uint16_t slot = 0; slot < LanceSize; ++slot) {
        for (unsigned destroyed = 0; destroyed < 2; ++destroyed) {
            reset(); selectedSlot = slot;
            /* The displayed initial can differ from a hidden live record. */
            Mechs[slot].name[0] = MECH_Destroyed;
            memcpy(originalMechs,Mechs,sizeof Mechs);
            Prepare_Party_Mech_For_Arena();
            Mech expectedArena = originalMechs[slot];
            expectedArena.name[0] = StoredPartyMechNameInitial[slot];
            expectedArena.pilotId = Character_Jason; expectedArena.riderId = MECH_NoRider;
            check(!memcmp(&Mechs[0],&expectedArena,sizeof expectedArena));
            check(!memcmp(ArenaMechRecordBackup,&originalMechs[0],MechRecordSize));
            check(draws == 1 && prompts == 1 && selections == 1 && ArenaRentalMechMode == FALSE);
            checkHiddenParty();
            Mechs[0].currentArmour[0] = 1; Mechs[0].currentAmmo[0] = 2; Mechs[0].engineHits = 2;
            if (destroyed) Mechs[0].name[0] = MECH_Destroyed;
            Characters[0].health = 17;
            Mech combatResult = Mechs[0];
            Restore_Party_After_Arena_Combat();
            for (unsigned i = 0; i < MechRecordCount; ++i) {
                Mech expected = originalMechs[i];
                if (i == slot) { expected = combatResult; expected.name[0] = MECH_Destroyed; }
                if (i < LanceSize) { expected.pilotId = originalMechs[i].pilotId; expected.riderId = originalMechs[i].riderId; }
                check(!memcmp(&Mechs[i],&expected,sizeof expected));
            }
            check(StoredPartyMechNameInitial[slot] == (destroyed ? MECH_Destroyed : 'A' + slot));
            check(ArenaRentalMechMode == FALSE && SelectedMechId == slot);
            checkRestoredParty();
        }
    }
    reset(); Prepare_Rental_Locust_For_Arena();
    check(!memcmp(&Mechs[0],&MechRefs[MECH_REF_Locust],MechRecordSize));
    check(!memcmp(ArenaMechRecordBackup,&originalMechs[0],MechRecordSize));
    check(ArenaRentalMechMode == TRUE && draws == 0 && selections == 0 && prompts == 0);
    for (unsigned i = 0; i < LanceSize; ++i)
        check(SavedArenaMechPilotId[i] == 0xAA && SavedArenaMechRiderId[i] == 0xBB);
    checkHiddenParty();
    Mechs[0].name[0] = MECH_Destroyed; Mechs[0].engineHits = 3; Mechs[0].pilotId = 0;
    Characters[0].health = 17; Mechs[1].pilotId = 7;
    Restore_Party_After_Arena_Combat();
    check(!memcmp(&Mechs[0],&originalMechs[0],MechRecordSize));
    check(Mechs[1].pilotId == 7); /* Rental path does not restore saved crew arrays. */
    check(ArenaRentalMechMode == TRUE && SelectedMechId == 3);
    for (unsigned i = 0; i < LanceSize; ++i) check(StoredPartyMechNameInitial[i] == 'A' + i);
    checkRestoredParty();
    printf("Arena staging/restoration: %u checks passed\n",checks);
    return 0;
}
