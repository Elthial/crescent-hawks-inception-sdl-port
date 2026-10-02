#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void assertAtLine(int condition,unsigned line) {
    if(!condition) { fprintf(stderr,"Inspection assertion failed at line%u\n",line); exit(1); }
}
#undef assert
#define assert(condition) assertAtLine((condition),__LINE__)

static uint8_t activeMechs,partyCount,selectedCharacter;
static uint16_t inspectMech;
static unsigned sidebars,healthDraws,healthBars,waits,keys,mechViews,skillDisplays,dialogs;
static uint16_t dialogIds[4];
static char text[2048];
void Draw_Top_Graphic_Sidebar(void) { ++sidebars; }
void Draw_Menu_Border(uint16_t style) { assert(style==0); }
void Citadel_Building_Dialogs(uint16_t action) {
    assert(dialogs<4); dialogIds[dialogs++]=action;
    switch(action) {
    case CitadelDialog_CountActiveMechs: NumberOfActiveLanceMechs=activeMechs; break;
    case CitadelDialog_CountOtherPartyMembers: SelectedPartyMemberSlot=partyCount; break;
    case CitadelDialog_SelectPartyMember: SelectedPartyMemberSlot=selectedCharacter; break;
    default: assert(0);
    }
}
uint16_t Prompt_Yes_No(uint16_t defaultAnswer) { assert(defaultAnswer==TRUE); return inspectMech; }
uint16_t Display_Menu_Choices_And_Check(uint16_t menu) {
    assert(menu==PartyMechSelectionMenu && MenuControls[menu].optionCount==1);
    return 0; /* Single compact row maps to actual friendly mech record3. */
}
void Examine_Screen_BTSTATS_CMP(uint16_t mechId) { assert(mechId==3 && MechHeatLevel[3]==0); ++mechViews; }
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { assert(refresh==TRUE); ++healthDraws; }
void Wait_For_50Hz_Then_Check_Input(void) { ++waits; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return ' '; }
void Display_Text_From_Memory(uint8_t *value) {
    for(unsigned band=0;band<HealthDescriptionCount;++band) if(value==HealthDescriptions[band]) {
        assert(TextPanelLeft==22 && TextPanelWidth==17); /* Original temporary narrowing. */
        ++healthBars;
    }
    if((strcmp((const char *)value,"Yes")==0 || strcmp((const char *)value,"No")==0) &&
        ((TextRow==21 && TextColumn==8) || (TextRow==21 && TextColumn==20) || (TextRow==22 && TextColumn==19))) {
        int owned=TextColumn==8?PurchasedMedkit:TextColumn==20?HasMapper:PurchasedFieldSurgeryKit;
        assert(strcmp((const char *)value,owned?"Yes":"No")==0);
    }
    if(TextColumn==InspectSkillColumn && TextRow>=InspectFirstSkillRow && TextRow<InspectFirstSkillRow+CharacterSkillCount) {
        uint8_t *skills=(uint8_t *)&Characters[SelectedPartyMemberSlot]+offsetof(Character,skillBowsAndBlade);
        unsigned level=skills[TextRow-InspectFirstSkillRow];
        if(level>SkillLevel_Excellent) level=SkillLevel_Excellent;
        assert(strcmp((const char *)value,(const char *)SkillLevelDescriptions[level])==0);
        assert(TextColour==EGA_BrightWhite); ++skillDisplays;
    }
    size_t used=strlen(text),count=strlen((const char *)value)+1;
    assert(used+count<=sizeof text); memcpy(text+used,value,count);
}
static void run(void) {
    sidebars=healthDraws=healthBars=waits=keys=mechViews=skillDisplays=dialogs=0;
    text[0]=0; Inspect_Characters();
    assert(CurrentMenuLayoutIndex==TextPanel_FirstTimePlayer);
    assert(TextPanelLeft==14 && TextPanelWidth==25);
}
static void characterChecks(void) {
    assert(healthBars==1 && !mechViews && skillDisplays==CharacterSkillCount && keys==1);
    assert(dialogIds[0]==CitadelDialog_CountActiveMechs && dialogIds[1]==CitadelDialog_CountOtherPartyMembers);
    assert(strstr(text,"Name  :\rWeapon:")!=NULL);
    assert(strstr(text,(const char *)CharacterNames[Characters[SelectedPartyMemberSlot].name])!=NULL);
}
int main(void) {
    assert(ArmourTypeDurability[1]==25 && ArmourTypeDurability[4]==50);
    assert(strcmp((const char *)SkillLevelDescriptions[2],"Adequate")==0);
    Characters[0].name=0; Characters[0].weapon=7; Characters[0].body=3; Characters[0].health=30;
    Characters[0].armourType=ArmourType_None;
    for(unsigned skill=0;skill<CharacterSkillCount;++skill)
        ((uint8_t *)&Characters[0])[offsetof(Character,skillBowsAndBlade)+skill]=(uint8_t)skill;
    activeMechs=0; partyCount=0; run(); characterChecks();
    assert(dialogs==2 && waits==1 && !healthDraws);
    assert(strstr(text,"Armor has") ==NULL);
    for(unsigned band=0;band<HealthDescriptionCount;++band) {
        Characters[0].health=(uint8_t)(band*3+2); /* IDIV truncation, not rounding. */
        run(); characterChecks();
        assert(strstr(text,(const char *)HealthDescriptions[band])!=NULL);
    }
    Characters[0].body=0xFD; Characters[0].health=0xE0; /* Signed -32/-3 gives10. */
    run(); characterChecks(); assert(strstr(text,(const char *)HealthDescriptions[10])!=NULL);
    Characters[0].body=3; Characters[0].health=30;
    for(unsigned owned=0;owned<8;++owned) {
        PurchasedMedkit=(uint8_t)((owned&1)?0x80:0);
        HasMapper=(uint8_t)((owned&2)?0x80:0);
        PurchasedFieldSurgeryKit=(uint8_t)((owned&4)?0x80:0);
        run(); characterChecks();
    }
    Characters[0].skillMedical=127; run(); characterChecks();
    for(uint8_t armour=1;armour<ArmourTypeCount;++armour) {
        Characters[0].armourType=armour; Characters[0].armourValue=ArmourTypeDurability[armour];
        run(); characterChecks(); assert(strstr(text,"Armor has not been injured.")!=NULL);
        Characters[0].armourValue=0;
        run(); characterChecks(); assert(strstr(text,"Armor has lost all of its ")!=NULL);
        Characters[0].armourValue=(uint8_t)(ArmourTypeDurability[armour]-3);
        run(); characterChecks(); assert(strstr(text,"Armor has lost 3 of its ")!=NULL);
    }
    Characters[0].armourType=1; Characters[0].armourValue=0xFF;
    run(); characterChecks(); assert(strstr(text,"Armor has lost 26 of its 25 protective points.")!=NULL);
    Characters[0].armourValue=30;
    run(); characterChecks(); assert(strstr(text,"Armor has lost -5 of its 25 protective points.")!=NULL);
    TraitorInParty=TRUE; TraitorCharacterId=0; TraitorWarning=FALSE;
    run(); characterChecks(); assert(TraitorWarning && waits==2);
    assert(strstr(text,"You don't like the way he's acting.")!=NULL && strstr(text,"Armor has")==NULL);
    TraitorCharacterId=1; run(); characterChecks(); assert(waits==1 && strstr(text,"Armor has")!=NULL);
    TraitorInParty=FALSE;
    Characters[2]=Characters[0]; Characters[2].name=2;
    partyCount=2; selectedCharacter=2; run(); characterChecks();
    assert(dialogs==3 && SelectedPartyMemberSlot==2 && dialogIds[2]==CitadelDialog_SelectPartyMember);
    activeMechs=1; inspectMech=FALSE; run(); characterChecks();
    assert(strstr(text,"Do you want to inspect a 'Mech?")!=NULL);
    for(unsigned mech=0;mech<LanceSize;++mech) Mechs[mech].name[0]=MECH_Destroyed;
    memcpy(Mechs[3].name,"Commando",9); Mechs[3].pilotId=3;
    inspectMech=TRUE; MechHeatLevel[3]=29; MechHeatLevel[2]=17; run();
    assert(mechViews==1 && MechHeatLevel[3]==0 && MechHeatLevel[2]==17);
    assert(dialogs==1 && !healthBars && !skillDisplays && !waits && !keys && healthDraws==1);
    assert(strstr(text,"Examine which 'Mech?\r")!=NULL && strstr(text,"Name  :")==NULL);
    puts("Original character/mech inspection branches, signed armour and skill descriptions verified.");
    return 0;
}
