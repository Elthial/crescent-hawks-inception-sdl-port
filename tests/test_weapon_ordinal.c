#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned calls,sidebars,waits;
static uint16_t panels[2];
static uint8_t message[]="Destroyed weapon";
static void checkLine(int value,unsigned line) { if(!value) { fprintf(stderr,"Weapon ordinal mismatch line%u\n",line); exit(1); } }
#define check(value) checkLine(!!(value),__LINE__)
/* Only popup display/input boundary is replaced. Original ordinal lookup
 * and layout-saving popup bodies run; no production substitute is supplied. */
void Menu_Memory_Variables(uint16_t panel) { check(calls<2); panels[calls++]=panel; CurrentMenuLayoutIndex=panel; }
void Draw_Top_Graphic_Sidebar(void) { check(CurrentMenuLayoutIndex==4); ++sidebars; }
void Display_Text_From_Memory(uint8_t *text) { check(text==message && CurrentMenuLayoutIndex==4); }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++waits; return 0x4800; }
int main(void) {
    for(unsigned record=0;record<MechRecordCount;++record) for(unsigned raw=0;raw<256;++raw) for(unsigned ammo=0;ammo<256;++ammo) {
        memset(&Mechs[record],0,sizeof Mechs[record]);
        Mechs[record].criticalSlots[0]=(uint8_t)raw; Mechs[record].currentAmmo[0]=(uint8_t)ammo;
        unsigned masked=raw&127;
        unsigned expected=(masked>=16 && masked<=32 && ammo)?raw-1:255;
        check(Combat_Get_Weapon_Index_For_Ordinal((uint16_t)record,0)==expected);
        check(Combat_Get_Weapon_Index_For_Ordinal((uint16_t)record,1)==255);
    }
    memset(&Mechs[0],0,sizeof Mechs[0]);
    /* Nonweapons don't advance the ordinal, destroyed weapons do. */
    for(unsigned slot=0;slot<MechCriticalSlotCount;++slot)
        Mechs[0].criticalSlots[slot]=(uint8_t)(slot%2?0x91:0x22);
    memset(Mechs[0].currentAmmo,1,sizeof Mechs[0].currentAmmo);
    Mechs[0].walkMove=Mechs[0].jumpMove=1;
    for(unsigned ordinal=0;ordinal<17;++ordinal) {
        check(Combat_Get_Weapon_Index_For_Ordinal(0,(uint16_t)ordinal)==0x90);
    }
    check(Combat_Get_Weapon_Index_For_Ordinal(0,17)==255);
    check(Combat_Get_Weapon_Index_For_Ordinal(0,UINT16_MAX)==255);
    memset(&Mechs[0],0,sizeof Mechs[0]);
    for(unsigned ordinal=0;ordinal<12;++ordinal) Mechs[0].criticalSlots[ordinal]=(uint8_t)(16+ordinal);
    Mechs[0].walkMove=3; Mechs[0].jumpMove=0;
    check(Combat_Get_Weapon_Index_For_Ordinal(0,10)==25);
    check(Combat_Get_Weapon_Index_For_Ordinal(0,11)==255);
    Mechs[0].jumpMove=255;
    check(Combat_Get_Weapon_Index_For_Ordinal(0,11)==26); /*FF ammo means available*/
    for(unsigned layout=0;layout<9;++layout) {
        calls=sidebars=waits=0; CurrentMenuLayoutIndex=(uint16_t)layout;
        Combat_Weapon_Display_Text_And_Menu_Options(message);
        check(calls==2 && panels[0]==4 && panels[1]==layout && sidebars==2 && waits==1 && CurrentMenuLayoutIndex==layout);
    }
    puts("Original weapon ordinal/ammo aliases and popup checks passed"); return 0;
}
