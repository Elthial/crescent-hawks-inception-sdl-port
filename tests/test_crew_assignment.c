#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static struct { uint16_t panel,row; } choices[32];
static uint16_t answers[8],defaults[8];
static unsigned choiceCount,choiceIndex,answerCount,answerIndex,keys,waits,messages,iterations;
static unsigned scenario,detachedChecks,healthSidebarCalls;
static uint8_t transcript[16384];
static size_t textBytes;
static void checkAtLine(int condition,unsigned line) {
    if(!condition) { fprintf(stderr,"Crew scenario%u mismatch line%u\n",scenario,line); exit(1); }
}
#define check(condition) checkAtLine((condition),__LINE__)
void Menu_Memory_Variables(uint16_t panel) { check(panel==6 || panel==4 || panel==3); if(panel==6) ++iterations; }
void Draw_Top_Graphic_Sidebar(void) { TextRow=0; }
void Draw_Menu_Border(uint16_t panel) { check(panel==0); }
void Draw_Message_Box(void) { ++messages; TextRow=0; }
void Draw_Health_and_C_Bills_Sidebar(uint16_t update) { check(update==TRUE && choiceIndex==choiceCount); ++healthSidebarCalls; }
void Display_Text_From_Memory(uint8_t *text) {
    size_t length=strlen((char *)text); check(textBytes+length+1<sizeof transcript);
    memcpy(transcript+textBytes,text,length); textBytes+=length; transcript[textBytes]=0;
    for(size_t i=0;i<length;++i)
        if(text[i]=='\r') ++TextRow;
        else if(text[i]==6) { check(i+1<length); TextColour=text[++i]; }
}
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text) { Display_Text_From_Memory(text); ++waits; }
void Wait_For_50Hz_Then_Check_Input(void) { ++waits; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 13; }
uint16_t Prompt_Yes_No(uint16_t defaultChoice) {
    check(answerIndex<answerCount && defaultChoice==defaults[answerIndex]); return answers[answerIndex++];
}
uint16_t Display_Menu_Choices_And_Check(uint16_t panel) {
    check(choiceIndex<choiceCount && choices[choiceIndex].panel==panel);
    uint16_t row=choices[choiceIndex++].row;
    check(row<MenuControls[panel].optionCount);
    if(panel==3 && scenario==6) {
        for(unsigned id=0;id<LanceSize;++id) check(Mechs[id].pilotId!=3 && Mechs[id].riderId!=3);
        ++detachedChecks;
    }
    if(panel==23) check(MenuControls[panel].baseRow==1);
    return row;
}
static void choose(uint16_t panel,uint16_t row) { choices[choiceCount].panel=panel; choices[choiceCount++].row=row; }
static void answer(uint16_t defaultChoice,uint16_t choice) { defaults[answerCount]=defaultChoice; answers[answerCount++]=choice; }
static void prepare(unsigned number) {
    scenario=number; choiceCount=choiceIndex=answerCount=answerIndex=keys=waits=messages=iterations=detachedChecks=healthSidebarCalls=0;
    textBytes=0; transcript[0]=0;
    memset(Characters,0,sizeof Characters); memset(Mechs,0,sizeof Mechs);
    for(unsigned id=0;id<CharacterRecordCount;++id) { Characters[id].name=Character_Dead; Characters[id].mechAssignment=Character_OnFoot; }
    for(unsigned id=0;id<MechRecordCount;++id) { Mechs[id].name[0]=MECH_Destroyed; Mechs[id].pilotId=MECH_NoPilot; Mechs[id].riderId=MECH_NoRider; }
    memset(StoredPartyMechNameInitial,'X',LanceSize); PartyMechNamesHidden=TRUE;
    TraitorInParty=FALSE; TraitorWarning=0; TextRow=99;
}
static void member(unsigned id,uint8_t name,uint8_t piloting) { Characters[id].name=name; Characters[id].skillPiloting=piloting; }
static void mech(unsigned id,uint8_t pilot,uint8_t rider) {
    memcpy(Mechs[id].name,"LOCUST     ",12); Mechs[id].pilotId=pilot; Mechs[id].riderId=rider;
    Mechs[id].tonnage=20;
}
static void run(uint16_t mode) {
    if(scenario==13) Menu_Assign_Pilots();
    else Assign_Pilot_and_rider_to_Mechs(mode);
    check(choiceIndex==choiceCount && answerIndex==answerCount && !PartyMechNamesHidden);
    for(unsigned id=0;id<LanceSize;++id) check(StoredPartyMechNameInitial[id]==MECH_Destroyed);
    check(strstr((char *)transcript,"Assign pilots and passengers:\x06\x0F\r")!=NULL);
}
int main(void) {
    prepare(1); member(1,2,0); member(5,1,1); mech(2,5,MECH_NoRider);
    choose(23,2); run(0);
    check(Characters[5].mechAssignment==2 && Characters[1].mechAssignment==8 && iterations==1);
    check(strstr((char *)transcript," LOCUST pilot\r")!=NULL); /*Trailing spaces trimmed only in summary*/
    check(strstr((char *)transcript,"\r\x06\x0F" "Pilot:\x06\x02 ")!=NULL);
    prepare(2); member(1,1,1); mech(2,1,1);
    for(unsigned id=0;id<PartySize;++id) Characters[id].mechAssignment=2;
    choose(23,0); choose(3,0); choose(23,1); run(0xFFFF);
    check(Mechs[2].pilotId==1 && Mechs[2].riderId==MECH_NoRider && Characters[1].mechAssignment==2);
    for(unsigned id=0;id<PartySize;++id) if(id!=1) check(Characters[id].mechAssignment==8);
    for(unsigned id=0;id<LanceSize;++id) if(id!=2) check(Mechs[id].pilotId==MECH_NoPilot && Mechs[id].riderId==MECH_NoRider);
    for(unsigned kind=3;kind<=5;++kind) {
        prepare(kind); member(1,1,1); member(3,2,(uint8_t)(kind==5?0:1)); member(5,3,0); mech(2,1,5);
        choose(23,1); choose(3,0); choose(23,3);
        if(kind!=5) answer(TRUE,(uint16_t)(kind==3?TRUE:FALSE));
        run(0); check(Characters[5].mechAssignment==8 && Characters[3].mechAssignment==2);
        check(Mechs[2].pilotId==(kind==3?3:1) && Mechs[2].riderId==(kind==3?1:3));
        check(Characters[1].mechAssignment==2);
        if(kind==5) check(strstr((char *)transcript,"not \x06\x0F" "a qualified 'Mech pilot.")!=NULL);
    }
    prepare(6); member(1,1,1); member(3,2,1); member(5,3,0); mech(2,1,3);
    Mechs[0].riderId=3; /*Detach also scans destroyed records*/
    choose(23,1); choose(3,1); choose(23,3); run(0);
    check(detachedChecks==1 && Characters[3].mechAssignment==8 && Mechs[2].pilotId==1);
    prepare(7); member(1,1,1); mech(2,MECH_NoPilot,MECH_NoRider);
    choose(23,1); choose(23,0); choose(3,0); choose(23,1); run(0);
    check(keys==1 && waits==1 && Mechs[2].pilotId==1 && iterations==3);
    check(strstr((char *)transcript,"Assign a pilot to each.")!=NULL);
    prepare(8); member(1,1,0); mech(2,MECH_NoPilot,1);
    choose(23,1); choose(23,1); answer(FALSE,TRUE); run(0);
    check(Mechs[2].name[0]==MECH_Destroyed && Mechs[2].tonnage==20 && Mechs[2].riderId==1);
    check(Characters[1].mechAssignment==8 && iterations==2 && keys==2 && waits==3);
    prepare(9); member(1,1,0); mech(2,MECH_NoPilot,1);
    choose(23,1); choose(23,1); choose(23,1); answer(FALSE,FALSE); answer(FALSE,2); run(0);
    check(iterations==3 && Characters[1].mechAssignment==8 && Mechs[2].name[0]==MECH_Destroyed);
    prepare(10); member(1,1,1); member(3,2,0); mech(2,1,MECH_NoRider);
    TraitorInParty=128; TraitorCharacterId=3;
    choose(23,1); choose(3,0); choose(23,2); run(0);
    check(Mechs[2].riderId==3 && TraitorWarning==TRUE && messages==1 && keys==1);
    check(strstr((char *)transcript," makes you uneasy about putting him in a 'Mech cockpit.")!=NULL);
    prepare(11); choose(23,0); run(0); check(iterations==1 && !keys && !waits);
    prepare(12); for(unsigned id=0;id<PartySize;++id) member(id,(uint8_t)(id%CharacterNameCount),1);
    for(unsigned id=0;id<LanceSize;++id) mech(id,(uint8_t)id,(uint8_t)(id+4));
    choose(23,8); run(0);
    for(unsigned id=0;id<PartySize;++id) check(Characters[id].mechAssignment==id%4);
    prepare(13); member(5,1,1); mech(2,5,MECH_NoRider);
    memset(LocalTerrainFlags,0,sizeof LocalTerrainFlags); choose(23,1); run(0);
    check(healthSidebarCalls==1 && Characters[5].mechAssignment==2);
    puts("Original crew assignment passed"); return 0;
}
