#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned menus,delays;
static char text[128];
static void checkLine(int value,unsigned line) { if(!value) { fprintf(stderr,"Ejection mismatch line%u\n",line); exit(1); } }
#define check(value) checkLine(!!(value),__LINE__)
/* Original ejection, message filter and Starport patch methods execute.
 * Only display/timing boundaries are replaced; RNG must not be called. */
void Menu_Memory_Variables(uint16_t panel) { check(panel==4); ++menus; }
void Set_Text_Colour_Bright_Green(void) { TextColour=10; }
void Display_Text_From_Memory(uint8_t *message) {
    size_t used=strlen(text),length=strlen((char *)message);
    check(TextColour==10 && used+length<sizeof text); memcpy(text+used,message,length+1);
}
void GameSpeed_RateControl(void) { ++delays; }
uint8_t Rand_0x00_to_0xFF(void) { check(0); return 0; }
static void prepare(unsigned record) {
    memset(Mechs,0,sizeof Mechs); memset(Characters,0,sizeof Characters);
    memset(CombatantCasualtyFlags,0,sizeof CombatantCasualtyFlags);
    memset(CombatWeaponTarget,255,sizeof CombatWeaponTarget);
    memset(DestroyedMechNameInitial,0,sizeof DestroyedMechNameInitial);
    for(unsigned id=0;id<AllCombatantCount;++id) { CombatantActive[id]=7; CombatantPackedX[id]=0x0220; CombatantPackedY[id]=0x3050; }
    Mechs[record].name[0]='L'; Mechs[record].pilotId=Mechs[record].riderId=255;
    CombatNotificationLatch=3; ShowArmShotOffAnimation=0x0100;
    ArenaRentalMechMode=0; CombatMessageVerbosity=1; CombatSpeedSetting=4;
    text[0]=0; menus=delays=0;
}
int main(void) {
    const uint16_t combatants[8]={0,1,2,3,12,13,14,15};
    for(unsigned record=0;record<8;++record) {
        prepare(record); uint16_t id=combatants[record];
        for(unsigned slot=0;slot<AllCombatantCount*CombatWeaponTargetSlots;++slot)
            CombatWeaponTarget[slot]=(uint8_t)(slot%3==0?id:slot%3==1?id|128:255);
        Combat_Mech_Eject(id);
        check(!CombatantActive[id] && CombatantCasualtyFlags[id]==1 && Mechs[record].name[0]==255 && DestroyedMechNameInitial[record]=='L');
        check(!CombatNotificationLatch && !ShowArmShotOffAnimation && TextColour==15 && menus==1 && delays==(unsigned)(record<4));
        check(CombatantPackedX[id]==0x0220 && CombatantPackedY[id]==0x3050);
        for(unsigned slot=0;slot<AllCombatantCount*CombatWeaponTargetSlots;++slot)
            check(CombatWeaponTarget[slot]==(slot%3==1?(id|128):255));
        check(strstr(text,"\rMech is destroyed!"));
        check((strstr(text,"\rMen eject!")!=NULL)==(record<4));
    }
    for(unsigned pilot=0;pilot<=8;++pilot) for(unsigned rider=0;rider<=8;++rider) {
        prepare(2); Mechs[2].pilotId=(uint8_t)(pilot==8?255:pilot); Mechs[2].riderId=(uint8_t)(rider==8?255:rider);
        CombatantPackedX[2]=0x017F; CombatantPackedY[2]=0x4066;
        Combat_Mech_Eject(2);
        if(pilot<8) {
            check(Characters[pilot].mechAssignment==8 && CombatantActive[pilot+4]==1 && CombatantPackedY[pilot+4]==0x4066);
            check(CombatantPackedX[pilot+4]==(pilot==rider?0x0200:0x017F)); /*Rider overwrites shared occupant*/
        }
        if(rider<8) check(Characters[rider].mechAssignment==8 && CombatantActive[rider+4]==1 && CombatantPackedX[rider+4]==0x0200 && CombatantPackedY[rider+4]==0x4066);
        check(Mechs[2].pilotId==(pilot==8?255:pilot) && Mechs[2].riderId==(rider==8?255:rider));
    }
    for(unsigned x=0;x<65536;++x) {
        prepare(0); Mechs[0].riderId=1; CombatantPackedX[0]=(uint16_t)x;
        Combat_Mech_Eject(0);
        unsigned expected=(x+1)%65536; if(expected&128) expected=(expected+128)%65536;
        check(CombatantPackedX[5]==expected);
    }
    prepare(4); ArenaRentalMechMode=2; Mechs[5].name[0]='C'; Combat_Mech_Eject(12);
    check(Mechs[5].name[0]==255 && !CombatantActive[13] && !CombatantCasualtyFlags[13] && !DestroyedMechNameInitial[5]);
    prepare(5); ArenaRentalMechMode=1;
    for(unsigned byte=0;byte<StarportPatchBytes;++byte) MapFileTiles[StarportPatchTileOffset+byte]=(uint8_t)(byte+1);
    Combat_Mech_Eject(13);
    check(!text[0] && !delays);
    for(unsigned byte=0;byte<StarportPatchBytes;++byte) {
        check(StarportSavedMapBytes[byte]==byte+1);
        check(MapFileTiles[StarportPatchTileOffset+byte]==(StarportMapPatchOverrides[byte]?StarportMapPatchOverrides[byte]:byte+1));
    }
    prepare(0); CombatMessageVerbosity=0; CombatSpeedSetting=5; Combat_Mech_Eject(0);
    check(!strcmp(text,"\rMen eject!") && !delays);
    prepare(0); CombatSpeedSetting=UINT16_MAX; Combat_Mech_Eject(0); check(delays==1); /*Signed speed gate*/
    puts("Original Mech destruction/ejection and arena patch checks passed"); return 0;
}
