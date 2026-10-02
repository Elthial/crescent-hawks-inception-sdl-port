#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static Character expectedCharacters[CharacterRecordCount];
static Mech expectedMechs[MechRecordCount];
static uint8_t savedNames[PartySize];
static unsigned missionCalls;
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Jailbreak mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
/* Only Mission9 is a boundary here. Its complete controller has a separate
 * suite; this test verifies the original enclosing save/restore/award method. */
void Mech_Mission(uint16_t mission)
{
    check(mission==Mission_Jailbreak && missionCalls++==0);
    check(CrescentHawkMapPositionX==JailEntranceX && CrescentHawkMapPositionY==JailEntranceY);
    check(Characters[0].mechAssignment==Character_OnFoot);
    for(unsigned member=1;member<PartySize;++member) {
        check(Characters[member].name==Character_Dead);
        check(SavedPartyNameId[member]==savedNames[member]);
    }
    for(unsigned slot=0;slot<LanceSize;++slot) {
        check(Mechs[slot].name[0]==MECH_Destroyed);
        check(StoredPartyMechNameInitial[slot]==expectedMechs[slot].name[0]);
    }
    /* Changes made by Mission9 are NOT rolled back with the name bytes. */
    Characters[0].health=37;
    Characters[3].skillMedical=6;
    Mechs[2].currentAmmo[0]=19;
    expectedCharacters[0].health=37;
    expectedCharacters[3].skillMedical=6;
    expectedMechs[2].currentAmmo[0]=19;
}
int main(void)
{
    for(unsigned vacancies=0;vacancies<16;++vacancies)
    for(unsigned alive=0;alive<3;++alive)
    for(unsigned name=0;name<256;++name) {
        memset(Characters,0x5A,sizeof Characters);
        memset(Mechs,0x39,sizeof Mechs);
        memset(SavedPartyNameId,0xA6,sizeof SavedPartyNameId);
        for(unsigned member=0;member<PartySize;++member)
            savedNames[member]=Characters[member].name=(uint8_t)(name+member);
        for(unsigned slot=0;slot<LanceSize;++slot) {
            Mechs[slot].name[0]=(vacancies&(1u<<slot))?MECH_Destroyed:(uint8_t)('A'+slot);
            StoredPartyMechNameInitial[slot]=0xE1;
        }
        memcpy(expectedCharacters,Characters,sizeof Characters);
        memcpy(expectedMechs,Mechs,sizeof Mechs);
        expectedCharacters[0].mechAssignment=Character_OnFoot;
        MainCharactersAlive=(uint16_t)(alive==2?0x8000:alive);
        missionCalls=0;
        Run_Jailbreak_Mission_And_Award_Stinger();
        if(alive!=0) for(unsigned slot=0;slot<LanceSize;++slot)
            if(vacancies&(1u<<slot)) { expectedMechs[slot]=MechRefs[MechRef_Stinger]; break; }
        check(missionCalls==1);
        check(CrescentHawkMapPositionX==JailEscapeX && CrescentHawkMapPositionY==JailEscapeY);
        check(!memcmp(Characters,expectedCharacters,sizeof Characters));
        check(!memcmp(Mechs,expectedMechs,sizeof Mechs));
        check(SavedPartyNameId[0]==0xA6);
        for(unsigned member=1;member<PartySize;++member) check(SavedPartyNameId[member]==savedNames[member]);
        for(unsigned slot=0;slot<LanceSize;++slot) check(StoredPartyMechNameInitial[slot]==MECH_Destroyed);
    }
    return 0;
}
