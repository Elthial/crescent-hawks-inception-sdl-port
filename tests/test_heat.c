#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Actual heat method and original shared state/data execute without UI. */
static uint8_t steps[AllCombatantCount];
static void checkLine(int value,unsigned line) { if(!value) { fprintf(stderr,"Heat mismatch line%u\n",line); exit(1); } }
#define check(value) checkLine(!!(value),__LINE__)
static void prepare(void) {
    memset(Mechs,0,sizeof Mechs); memset(Characters,0,sizeof Characters);
    memset(MechHeatLevel,0,sizeof MechHeatLevel); memset(CombatWeaponHeat,0,sizeof CombatWeaponHeat);
    memset(MechInfernoRoundsRemaining,0,sizeof MechInfernoRoundsRemaining);
    memset(TerrainOverlapRows,0,sizeof TerrainOverlapRows); memset(MapTileUnderCombatant,0,sizeof MapTileUnderCombatant);
    memset(CombatWeaponTarget,255,sizeof CombatWeaponTarget); memset(CombatMovementOrders,255,sizeof CombatMovementOrders);
    memset(CombatantActive,0,sizeof CombatantActive); memset(steps,0,sizeof steps);
    for(unsigned id=0;id<MechRecordCount;++id) Mechs[id].name[0]=MECH_Destroyed;
}
int main(void) {
    /* BYTE heat and signed weapon heat combine modulo256 before clamping.
     * All65536 pairs tested in each bank; enemy has no upper clamp. */
    for(unsigned bank=0;bank<2;++bank) for(unsigned rawHeat=0;rawHeat<256;++rawHeat) for(unsigned rawWeaponHeat=0;rawWeaponHeat<256;++rawWeaponHeat) {
        prepare(); unsigned record=bank*4;
        Mechs[record].name[0]='M'; MechHeatLevel[record]=(int8_t)(uint8_t)rawHeat;
        CombatWeaponHeat[record]=(int8_t)(uint8_t)rawWeaponHeat;
        unsigned sum=(rawHeat+rawWeaponHeat)%256;
        unsigned expected=sum>=128?0:sum;
        if(!bank && expected>30) expected=30;
        Combat_Mech_HeatLevels(steps);
        check(MechHeatLevel[record]==(int)expected && CombatWeaponHeat[record]==0);
    }
    for(unsigned bank=0;bank<2;++bank) for(unsigned rawSteps=0;rawSteps<256;++rawSteps) {
        prepare(); unsigned record=bank*4,combatant=bank*12;
        Mechs[record].name[0]='M'; CombatMovementOrders[combatant*CombatMovementOrderBytes]=MovementMode_Jump;
        steps[combatant]=(uint8_t)rawSteps;
        unsigned expected=rawSteps>3 && rawSteps<128?rawSteps:3;
        if(!bank && expected>30) expected=30;
        Combat_Mech_HeatLevels(steps); check(MechHeatLevel[record]==(int)expected);
    }
    for(unsigned bank=0;bank<2;++bank) for(unsigned mode=0;mode<3;++mode) {
        prepare(); unsigned record=bank*4,combatant=bank*12;
        Mechs[record].name[0]='M'; CombatMovementOrders[combatant*CombatMovementOrderBytes]=(uint8_t)mode;
        Combat_Mech_HeatLevels(steps); check(MechHeatLevel[record]==(int)mode+1);
    }
    prepare(); Mechs[4].name[0]='M'; Mechs[4].engineHits=2; Mechs[4].engineHeatSinks=3;
    Mechs[4].criticalSlots[0]=Heat_Sink; Mechs[4].criticalSlots[1]=Destroyed_Heat_Sink;
    CombatWeaponHeat[4]=4; MechInfernoRoundsRemaining[4]=2; TerrainOverlapRows[12]=8; MapTileUnderCombatant[12]=15;
    Combat_Mech_HeatLevels(steps);
    check(MechHeatLevel[4]==12 && MechInfernoRoundsRemaining[4]==1 && !CombatWeaponHeat[4]);
    Combat_Mech_HeatLevels(steps); check(MechHeatLevel[4]==20 && !MechInfernoRoundsRemaining[4]);
    prepare(); Mechs[4].name[0]='M'; MechHeatLevel[4]=40; MechHeatLevel[0]=50;
    Combat_Mech_HeatLevels(steps); check(MechHeatLevel[4]==40 && MechHeatLevel[0]==30); /*BUG013*/
    prepare(); MechHeatLevel[0]=50; CombatWeaponHeat[0]=17; MechInfernoRoundsRemaining[0]=2;
    Combat_Mech_HeatLevels(steps); check(MechHeatLevel[0]==50 && !CombatWeaponHeat[0] && MechInfernoRoundsRemaining[0]==2);
    for(unsigned tile=0;tile<256;++tile) {
        prepare(); Mechs[4].name[0]='M'; MechHeatLevel[4]=10; TerrainOverlapRows[12]=128; MapTileUnderCombatant[12]=(uint8_t)tile;
        Combat_Mech_HeatLevels(steps); check(MechHeatLevel[4]==(tile<16?6:10));
    }
    prepare(); CombatantActive[12]=2;
    for(unsigned slot=0;slot<AllCombatantCount*CombatWeaponTargetSlots;++slot)
        CombatWeaponTarget[slot]=(uint8_t)(slot%3==0?0x8C:slot%3==1?13:255);
    Combat_Mech_HeatLevels(steps);
    for(unsigned slot=0;slot<AllCombatantCount*CombatWeaponTargetSlots;++slot)
        check(CombatWeaponTarget[slot]==(slot%3==0?12:255));
    prepare();
    for(unsigned character=0;character<CharacterRecordCount;++character) {
        unsigned combatant=character<8?character+4:character+8;
        Characters[character].name=Character_Dead; CombatantActive[combatant]=1;
        CombatantPackedX[combatant]=0x1234; CombatantPackedY[combatant]=0x5678; CombatantSpriteFamilyOffset[combatant]=7;
    }
    Combat_Mech_HeatLevels(steps);
    for(unsigned character=0;character<CharacterRecordCount;++character) {
        unsigned combatant=character<8?character+4:character+8;
        check(CombatantActive[combatant]==1);
        if(!character) check(CombatantPackedX[combatant]==0x1234 && CombatantPackedY[combatant]==0x5678 && CombatantSpriteFamilyOffset[combatant]==0x96);
        else check(CombatantPackedX[combatant]==65535 && CombatantPackedY[combatant]==65535 && CombatantSpriteFamilyOffset[combatant]==(character>=8?0xFE:7));
    }
    puts("Original round heat, target reset and casualty state checks passed"); return 0;
}
