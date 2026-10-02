#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char text[512];
static unsigned waits,keys,healthCalls,armourColour;
static void check(int condition) { if(!condition) { fputs("Combatant description mismatch\n",stderr); exit(1); } }
void Wait_For_50Hz_Then_Check_Input(void) { ++waits; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return ' '; }
void Display_Text_From_Memory(uint8_t *value) {
    for(unsigned band=0;band<HealthDescriptionCount;++band) if(value==HealthDescriptions[band]) ++healthCalls;
    if(TextRow==CombatDescriptionArmourRow) armourColour=TextColour;
    size_t used=strlen(text),length=strlen((const char *)value)+1;
    check(used+length<=sizeof text); memcpy(text+used,value,length);
    /* Test boundary models only controls present in these strings. Production
     * renderer remains independently tested; this is not a replacement API. */
    for(unsigned i=0;value[i];++i) {
        if(value[i]==6) TextColour=value[++i];
        else if(value[i]=='\r') { TextColumn=0; ++TextRow; }
        else ++TextColumn;
    }
}
static void run(uint16_t id,uint16_t recognition,uint16_t details) {
    text[0]=0; waits=keys=healthCalls=armourColour=0;
    TextColour=0; TextRow=0; TextColumn=0;
    Display_Friendly_Combatant_Description(id,recognition,details);
}
int main(void) {
    for(unsigned party=0;party<PartySize;++party) {
        Characters[party].name=(uint8_t)(party+2);
        Characters[party].body=4; Characters[party].health=39;
        Characters[party].weapon=7; Characters[party].armourType=ArmourType_None;
        run((uint16_t)(party+Friendly_Infantry_Combatant_Range_First),TRUE,FALSE);
        check(strcmp(text,(const char *)CharacterNames[party+2])==0 && !healthCalls && !waits && TextColour==15);
        run((uint16_t)(party+Friendly_Infantry_Combatant_Range_First),TRUE,TRUE);
        check(healthCalls==1 && strstr(text,(const char *)HealthDescriptions[9])!=NULL);
        check(strstr(text,"Weapon:\r\rArmor:\x06\x0F")!=NULL && strstr(text,"PistolNone")!=NULL);
        check(TextColumn==4 && TextRow==10 && TextColour==15);
    }
    for(unsigned mech=0;mech<LanceSize;++mech) {
        memcpy(Mechs[mech].name,"Commando",9); Mechs[mech].pilotId=(uint8_t)(mech+1);
        run((uint16_t)mech,TRUE,TRUE);
        check(strstr(text,(const char *)CharacterNames[mech+3])==text);
        check(strstr(text,"'s\rCommando")!=NULL && !healthCalls && !waits);
    }
    for(uint8_t armour=1;armour<ArmourTypeCount;++armour) {
        Characters[0].armourType=armour;
        Characters[0].armourValue=ArmourTypeDurability[armour]; run(4,FALSE,TRUE);
        check(armourColour==15 && TextColour==15 && strstr(text,(const char *)ArmourTextDescription[armour])!=NULL);
        Characters[0].armourValue=1; run(4,FALSE,TRUE);
        check(armourColour==14 && TextColour==14);
        Characters[0].armourValue=0; run(4,FALSE,TRUE);
        check(armourColour==8 && TextColour==15 && strstr(text,"Ruined\x06\x0F")!=NULL);
    }
    ArmourTypeDurability[1]=0xFF; Characters[0].armourType=1; Characters[0].armourValue=0xFF;
    run(4,FALSE,TRUE); check(armourColour==15); /* Both BYTEs CBW to-1. */
    ArmourTypeDurability[1]=25;
    TraitorInParty=0x80; TraitorCharacterId=0; TraitorWarning=FALSE;
    run(4,0x8000,0xFFFF);
    check(strstr(text," \x06\x06looks very suspicious to you.\x06\x0F")!=NULL);
    check(TraitorWarning && waits==1 && keys==1 && !healthCalls && TextColour==15);
    TraitorWarning=FALSE; run(4,FALSE,TRUE); check(!waits && healthCalls==1 && !TraitorWarning);
    run(4,TRUE,FALSE); check(!waits && !healthCalls && !TraitorWarning);
    TraitorCharacterId=0xFF; run(4,TRUE,TRUE); check(!waits && healthCalls==1);
    puts("Original friendly combatant descriptions, health handoff and renderer controls verified.");
    return 0;
}
