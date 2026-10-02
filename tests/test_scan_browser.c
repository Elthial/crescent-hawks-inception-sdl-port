#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t choices[8],previews[8],expectedTargets[8];
static unsigned choiceCount,choiceIndex,previewCount,detailViews,refreshes,healthCalls,sideMenuPending;
static char text[2048];
static void check(int condition) { if(!condition) { fputs("Original scan browser mismatch\n",stderr); exit(1); } }
void Draw_Top_Graphic_Sidebar(void) { TextColumn=0; TextRow=0; }
void Display_Text_From_Memory(uint8_t *value) {
    for(unsigned band=0;band<HealthDescriptionCount;++band) if(value==HealthDescriptions[band]) ++healthCalls;
    size_t used=strlen(text),length=strlen((const char *)value)+1;
    check(used+length<=sizeof text); memcpy(text+used,value,length);
    for(unsigned i=0;value[i];++i) {
        if(value[i]==6) TextColour=value[++i];
        else if(value[i]=='\r') { TextColumn=0; ++TextRow; }
        else ++TextColumn;
    }
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { return ' '; }
void Wait_For_50Hz_Then_Check_Input(void) { check(0); }
void Prompt_And_Wait_For_Key(void) { check(0); }
void Combat_Render_Movement_Preview(uint16_t id,uint16_t mode) {
    check(mode==TRUE && previewCount<choiceCount && id==expectedTargets[previewCount]);
    previews[previewCount++]=id;
}
uint16_t Display_Menu_Choices_And_Check(uint16_t menu) {
    check(menu==CombatSettingsMenuLayout && MenuControls[menu].baseRow==1 && !MenuControls[menu].selection);
    if(sideMenuPending) { sideMenuPending=0; return ScanSide_Enemies; }
    check(choiceIndex<choiceCount && previewCount==choiceIndex+1);
    uint16_t target=previews[choiceIndex];
    check(MenuControls[menu].optionCount==(target<4 || (target>=12 && target<16)?3:2));
    return choices[choiceIndex++];
}
void Examine_Screen_BTSTATS_CMP(uint16_t id) { check(id==previews[previewCount-1]); ++detailViews; }
void Menu_Draw_MultiSelect(uint16_t mode) { check(mode==TRUE); ++refreshes; }
static void prepare(void) {
    memset(CombatantActive,0,sizeof(uint16_t)*AllCombatantCount);
    for(unsigned id=0;id<AllCombatantCount;++id) {
        CombatantPackedX[id]=(uint16_t)(32+id*2); CombatantPackedY[id]=16;
    }
    for(unsigned record=0;record<CharacterRecordCount;++record) {
        Characters[record].name=2; Characters[record].body=4; Characters[record].health=39;
        Characters[record].weapon=7; Characters[record].armourType=0;
    }
    for(unsigned record=0;record<MechRecordCount;++record) {
        memcpy(Mechs[record].name,"Commando",9); Mechs[record].pilotId=1;
    }
    choiceCount=choiceIndex=previewCount=detailViews=refreshes=healthCalls=sideMenuPending=0;
    text[0]=0; ArenaRentalMechMode=FALSE; TraitorInParty=FALSE;
}
static void offer(uint16_t target,uint16_t choice) { check(choiceCount<8); expectedTargets[choiceCount]=target; choices[choiceCount++]=choice; }
static void finished(void) {
    check(choiceCount==choiceIndex && previewCount==choiceCount);
    check(MenuControls[3].baseRow==0 && MenuControls[3].selection==6 && TextColour==15);
}
int main(void) {
    prepare(); CombatantActive[12]=CombatantActive[13]=CombatantActive[14]=CombatantActive[15]=CombatantActive[16]=1;
    CombatantPackedX[13]=0xFFFF; CombatantPackedY[14]=0xFFFF; CombatantPackedX[16]=0x00FF;
    offer(12,0); offer(15,0); offer(16,1);
    Combat_Browse_Scan_Targets(0,12,TRUE); finished();
    check(!detailViews && healthCalls==1 && strstr(text,"Enemy\rCommando")!=NULL);
    check(strstr(text,"Weapon:\r\x06\x0F")!=NULL && strstr(text,"\rDirection:\x06\x0F")!=NULL);
    check(strstr(text,"\x06\x0FScan...\rNext Unit\rDetail ScanDone")!=NULL);
    prepare(); CombatantActive[12]=1; offer(12,1); offer(12,2);
    Combat_Browse_Scan_Targets(0,12,TRUE); finished(); check(detailViews==1 && refreshes==1);
    prepare(); CombatantActive[0]=1; offer(0,1); offer(0,2);
    Combat_Browse_Scan_Targets(0,0,0xFFFF); finished(); check(detailViews==1 && refreshes==1); /* WORD--0 then++FFFF. */
    prepare(); CombatantActive[12]=CombatantActive[16]=1; offer(16,1);
    Combat_Browse_Scan_Targets(4,12,FALSE); finished(); check(!detailViews && healthCalls==1);
    prepare(); CombatantActive[12]=CombatantActive[23]=1; offer(12,0); offer(23,0); offer(12,2);
    Combat_Browse_Scan_Targets(0,12,TRUE); finished();
    prepare(); CombatantActive[0]=CombatantActive[4]=1; offer(4,1);
    Combat_Browse_Scan_Targets(0,0,FALSE); finished(); check(healthCalls==1);
    check(strstr(text,(const char *)CharacterNames[2])!=NULL);
    prepare(); CombatantActive[12]=CombatantActive[13]=1; ArenaRentalMechMode=TRUE;
    offer(12,0); offer(12,2); Combat_Browse_Scan_Targets(0,12,TRUE); finished();
    prepare(); CombatantActive[16]=1; offer(16,1); sideMenuPending=TRUE;
    Scan_Enemies(4); check(choiceIndex==1 && previewCount==1 && healthCalls==1);
    check(MenuControls[3].baseRow==0 && MenuControls[3].selection==4); /* Parent overrides browser's6. */
    puts("Original scan parent/browser/description handoff, filtering, detail return and arena cycling verified.");
    return 0;
}
