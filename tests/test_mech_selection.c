#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned rowCount,menuCalls;
static uint16_t selection;
static char displayed[512],expected[512];
static void check(int condition) { if(!condition) { fputs("Original mech selector mismatch\n",stderr); exit(1); } }
static void append(char *destination,const uint8_t *source) {
    size_t used=strlen(destination),length=strlen((const char *)source)+1;
    check(used+length<=512); memcpy(destination+used,source,length);
}
void Display_Text_From_Memory(uint8_t *text) {
    append(displayed,text); if(text[0]=='\r' && !text[1]) ++TextRow;
}
uint16_t Display_Menu_Choices_And_Check(uint16_t menu) {
    check(menu==PartyMechSelectionMenu); ++menuCalls;
    check(MenuControls[menu].baseRow==6 && MenuControls[menu].optionCount==rowCount);
    check(MenuControls[menu].selection==0);
    check(SelectedMechId==(rowCount?MechSlotByMenuRow[rowCount-1]:0));
    return selection;
}
int main(void)
{
    for(unsigned hidden=0;hidden<2;++hidden)
        for(unsigned pattern=0;pattern<16;++pattern) {
            expected[0]=0; rowCount=0; PartyMechNamesHidden=(uint8_t)(hidden?0x80:0);
            for(unsigned slot=0;slot<LanceSize;++slot) {
                char name[MechNameLength];
                (void)snprintf(name,sizeof name,"Mech%u",slot);
                memset(Mechs[slot].name,0,MechNameLength);
                memcpy(Mechs[slot].name,name,strlen(name)+1);
                Mechs[slot].pilotId=(uint8_t)slot;
                Characters[slot].name=(uint8_t)(slot+4); /* Do NOT use character record Name for prefix. */
                StoredPartyMechNameInitial[slot]=(uint8_t)((pattern&(1u<<slot))?'M':MECH_Destroyed);
                if(hidden || !(pattern&(1u<<slot))) Mechs[slot].name[0]=MECH_Destroyed;
                if(pattern&(1u<<slot)) {
                    ++rowCount;
                    append(expected,CharacterNames[slot]); append(expected,(uint8_t *)"'s ");
                    append(expected,(uint8_t *)name); append(expected,(uint8_t *)"\r");
                }
            }
            unsigned choices=rowCount?rowCount:1;
            for(unsigned chosen=0;chosen<choices;++chosen) {
                memset(MechSlotByMenuRow,0xCC,LanceSize); SelectedMechId=65535;
                TextRow=6; selection=(uint16_t)chosen; menuCalls=0; displayed[0]=0;
                check(Display_Text_Mech_Names()==rowCount);
                check(strcmp(displayed,expected)==0 && menuCalls==1);
                check(SelectedMechId==MechSlotByMenuRow[chosen]);
                check(MenuControls[PartyMechSelectionMenu].baseRow==1);
                unsigned compact=0;
                for(unsigned slot=0;slot<LanceSize;++slot)
                    if(pattern&(1u<<slot)) check(MechSlotByMenuRow[compact++]==slot);
                while(compact<LanceSize) check(MechSlotByMenuRow[compact++]==0);
                if(hidden) for(unsigned slot=0;slot<LanceSize;++slot) check(Mechs[slot].name[0]==MECH_Destroyed);
            }
        }
    puts("Original compact mech selection, saved names, pilot-name oddity and zero rows verified.");
    return 0;
}
