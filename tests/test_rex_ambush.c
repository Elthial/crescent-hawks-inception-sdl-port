#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned rngCalls,diceCalls,combatCalls,choice,rollByte;
static uint16_t savedX[MapCharacterCount],savedY[MapCharacterCount];
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Rex ambush mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
uint8_t Rand_0x00_to_0xFF(void)
{
    unsigned call=rngCalls++;
    if(call==0) {
        for(unsigned npc=0;npc<MapCharacterCount;++npc) {
            check(RoamingMapNpcs[npc].currentPositionX==savedX[npc]);
            check(RoamingMapNpcs[npc].currentPositionY==savedY[npc]);
        }
        for(unsigned actor=0;actor<AllCombatantCount;++actor) {
            check(CombatantPackedX[actor]==UINT16_MAX && CombatantPackedY[actor]==UINT16_MAX);
            check(CombatantCasualtyFlags[actor]==0);
            check(CombatantActive[actor]==(actor==4 || actor==5));
        }
    }
    return (uint8_t)(call==2?choice:rollByte);
}
uint16_t Roll2D6(void) { ++diceCalls; return 12; }
void Combat_Run_Encounter(uint16_t mode) { check(mode==UINT16_MAX && combatCalls++==0); }
int main(void)
{
    const uint8_t skills[7]={1,3,2,4,4,1,0};
    const int xDelta[5]={0,1,2,0,1},yDelta[5]={1,1,1,2,2};
    for(unsigned vacancies=0;vacancies<16;++vacancies)
    for(choice=0;choice<4;++choice)
    for(rollByte=0;rollByte<256;++rollByte) {
        memset(Characters,0x5A,sizeof Characters); memset(Mechs,0x39,sizeof Mechs);
        memset(RoamingMapNpcs,0xA5,sizeof RoamingMapNpcs);
        memset(CombatantActionState,0xA7,sizeof CombatantActionState);
        for(unsigned slot=0;slot<LanceSize;++slot)
            StoredPartyMechNameInitial[slot]=(vacancies&(1u<<slot))?MECH_Destroyed:'L';
        NextRecruitNameId=(uint8_t)rollByte;
        for(unsigned actor=0;actor<AllCombatantCount;++actor) {
            CombatantPackedX[actor]=(uint16_t)(0x2100+actor);
            CombatantPackedY[actor]=(uint16_t)(0x4300+actor);
            CombatantActive[actor]=0xABCD; CombatantCasualtyFlags[actor]=0xAABB;
        }
        for(unsigned npc=0;npc<MapCharacterCount;++npc) {
            savedX[npc]=CombatantPackedX[16+npc]; savedY[npc]=CombatantPackedY[16+npc];
        }
        rngCalls=diceCalls=combatCalls=0;
        Recruit_Rex_And_Start_KuritaParty_Ambush();
        check(combatCalls==1 && diceCalls==choice+2 && rngCalls==3+(choice+2)*7);
        check(CrescentHawkMapPositionX==0x0A69 && CrescentHawkMapPositionY==0x805E);
        Character *rex=&Characters[1];
        check(rex->name==rollByte && rex->body==12 && rex->dexterity==9 && rex->charisma==8);
        check(!memcmp(&rex->skillBowsAndBlade,skills,sizeof skills));
        check(rex->weapon==7 && rex->health==120 && rex->mechAssignment==8);
        check(rex->armourType==0 && rex->armourValue==0 && rex->trainingFlags==0);
        unsigned slot=0;
        while(slot<LanceSize && !(vacancies&(1u<<slot))) ++slot;
        Mech expected=MechRefs[MechRef_Commando]; expected.name[0]=255; expected.pilotId=1;
        check(!memcmp(&Mechs[slot],&expected,sizeof expected));
        check(NextRecruitNameId==(slot==LanceSize?'C':(uint8_t)(rollByte+1)));
        if(slot<LanceSize) check(StoredPartyMechNameInitial[slot]=='C');
        for(unsigned record=8;record<16;++record) {
            Character *enemy=&Characters[record]; unsigned actor=record+8;
            if(record<choice+10) {
                unsigned n=record-8;
                check(enemy->name==0 && enemy->mechAssignment==8);
                for(unsigned skill=0;skill<6;++skill) check(((uint8_t *)enemy)[4+skill]==(rollByte&1));
                check(enemy->skillMedical==0x5A && enemy->body==12 && enemy->health==120 && enemy->weapon==rollByte%8);
                check(enemy->dexterity==0x5A && enemy->armourValue==0x5A);
                check(CombatantPackedX[actor]==0x0A72+(rollByte&1)+xDelta[n]);
                check(CombatantPackedY[actor]==0x805D+(rollByte&1)+yDelta[n]);
                check(CombatantActive[actor]==1 && CombatantMovementDirection[actor]==6 && CombatantAnimationSelector[actor]==6);
                check(CombatantAnimationCursors[actor].offset==0x2E0 && CombatantSpriteFrame[actor]==28 && CombatantSpriteFamilyOffset[actor]==254);
            } else check(enemy->name==255 && CombatantActive[actor]==0);
        }
        for(unsigned actor=0;actor<12;++actor) check(CombatantActionState[actor]==(actor==4 || actor==5?0:0xA7));
        for(unsigned mech=4;mech<8;++mech) check(Mechs[mech].name[0]==255);
    }
    return 0;
}
