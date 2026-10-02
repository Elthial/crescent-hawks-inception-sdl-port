#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned sidebars,keys,menus;
static uint16_t choice;
static uint8_t randomValue;
static char transcript[2048];
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Occupants mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
/* Actual original conversation body; text, menu, input and RNG boundaries. */
void Display_Text_From_Memory(uint8_t *text) {
    size_t used=strlen(transcript),length=strlen((char *)text); check(used+length<sizeof transcript);
    memcpy(transcript+used,text,length+1);
}
void Display_Text_4FA0_Value(void) { Display_Text_From_Memory((uint8_t *)"\r"); }
void Draw_Top_Graphic_Sidebar(void) { ++sidebars; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
uint16_t Display_Menu_Choices_And_Check(uint16_t control) { check(control==20); ++menus; return choice; }
uint8_t Rand_0x00_to_0xFF(void) { return randomValue; }
static void prepare(void) { transcript[0]=0; sidebars=keys=menus=0; }
int main(void) {
    for(unsigned i=0;i<8;++i) {
        memset(MapCharacterNames[i],0,MapNameBytes); MapCharacterNames[i][0]=(uint8_t)('A'+i);
        memset(MapBuildingNames[i],0,MapNameBytes); MapBuildingNames[i][0]=(uint8_t)('a'+i);
        AlternativeBldByBuildingId[i]=(uint8_t)i;
    }
    NpcActivityBuildingId=3; HoldRickAtlasForConversation=0; KuritaDestroyedCitadel=0;
    for(unsigned mask=0;mask<256;++mask) {
        unsigned count=0;
        for(unsigned i=0;i<8;++i) {
            RoamingMapNpcs[i].waypointPair=(uint8_t)(0x30+i);
            RoamingMapNpcs[i].movementDelay=(uint8_t)((mask>>i)&1); count+=(mask>>i)&1;
        }
        for(unsigned selected=0;selected<=count;++selected) {
            choice=(uint16_t)selected; prepare(); RickAtlasConversationTriggered=77;
            Talk_To_Building_Occupants();
            if(count==0) { check(menus==0 && keys==1 && sidebars==1 && RickAtlasConversationTriggered==77); continue; }
            check(menus==1 && BuildingOccupantMenuOptionCount==count+1);
            if(selected==count) { check(keys==0 && sidebars==1 && RickAtlasConversationTriggered==77); continue; }
            check(keys==1 && sidebars==2 && RickAtlasConversationTriggered==0);
            unsigned slot=0,rank=0;
            for(;slot<8;++slot) if(mask&(1u<<slot)) { if(rank==selected) break; ++rank; }
            size_t length=strlen(transcript); check(length>=2 && transcript[length-2]=='a'+(int)slot && transcript[length-1]=='.');
            check(strstr(transcript,"Hi, Jason, how are you?  I just stopped in here to buy a new pistol.")!=NULL);
        }
    }
    /* First NPC waits in weapon shop; an unrelated LAST waiting NPC's
     * lounge waypoint still triggers Rick. Reverse that last route to reject. */
    memset(RoamingMapNpcs,0,sizeof RoamingMapNpcs); RoamingMapNpcs[0].movementDelay=255;
    RoamingMapNpcs[0].waypointPair=0x30; RoamingMapNpcs[7].movementDelay=1;
    choice=0; HoldRickAtlasForConversation=255;
    for(unsigned last=0;last<8;++last) {
        RoamingMapNpcs[0].movementDelay=255; RoamingMapNpcs[7].waypointPair=(uint8_t)(last<<4);
        prepare(); Talk_To_Building_Occupants();
        check(RickAtlasConversationTriggered==(last==7));
        check(RoamingMapNpcs[0].movementDelay==(last==7?0:255));
        check(keys==(last==7?0u:1u) && sidebars==(last==7?1u:2u));
    }
    HoldRickAtlasForConversation=0; KuritaDestroyedCitadel=255;
    RoamingMapNpcs[0].movementDelay=1; RoamingMapNpcs[7].movementDelay=0;
    for(unsigned random=0;random<256;++random) {
        randomValue=(uint8_t)random; prepare(); Talk_To_Building_Occupants();
        check(strstr(transcript,(char *)DestroyedCitadelNpcReplyText[random&7])!=NULL && keys==1 && sidebars==2);
    }
    /* No exterior slot resolves this activity: no occupant matches slot8. */
    NpcActivityBuildingId=10; prepare(); RickAtlasConversationTriggered=77; Talk_To_Building_Occupants();
    check(!strcmp(transcript,"Nobody here seems interested in talking to you.") && menus==0 && RickAtlasConversationTriggered==77);
    prepare(); Display_No_Stock_Transaction_Text(); check(!strcmp(transcript," nothing this time.") && keys==0 && sidebars==0);
    return 0;
}
