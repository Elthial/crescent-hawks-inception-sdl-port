#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned step,randomCalls;
uint8_t Rand_0x00_to_0xFF(void) { return (uint8_t)(randomCalls++&1); }
void Graphics_Set_Screen_To_Black(void) { assert(++step==1); }
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { assert(++step==2 && refresh==TRUE); }
void Draw_Top_Graphic_Sidebar(void) { assert(++step==3); }
void Map_Construct_Nine_Regions(uint16_t page)
{
    assert(++step==4 && page==CitadelStartingPackedPage);
    assert(CrescentHawkMapPositionX==CitadelStartingPositionX && CrescentHawkMapPositionY==CitadelStartingPositionY);
}
void DOS_Load_Map_Files(uint16_t slot,uint16_t map) { assert(++step==5 && slot==4 && map==Map_Citadel); }
void Map_NineGrid_Parent(void) { assert(++step==6); }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { assert(++step==7 && x==0x0C45 && y==0xC019); }
void Copy_Data_To_GraphicsMemory(void) { assert(++step==8); }
void Draw_Infantry_And_Mechs(void)
{
    assert(++step==9 && ViewedHolodisk==0xAA && TransmittedCacheFound==0xBEEF);
    for (unsigned id=0;id<AllCombatantCount;++id) {
        assert(CombatantAnimationSelector[id]==255);
        assert(CombatantSpriteFrame[id]==(id%12<4?0:16));
    }
}
int main(void)
{
    memset(Characters,0xAA,sizeof Characters); memset(Mechs,0xBB,sizeof Mechs);
    memset(PersistentState.bytes,0xCC,sizeof PersistentState.bytes);
    memset(MapFogOfWar,0x80,sizeof MapFogOfWar); memset(CacheSecurityCodeUsed,0xFF,sizeof CacheSecurityCodeUsed);
    memset(CombatantSpriteFamilyOffset,0x33,sizeof CombatantSpriteFamilyOffset);
    memset(MapEffectSpriteIndex,0x88,sizeof MapEffectSpriteIndex);
    memset(MapEffectPackedPage,0x99,sizeof MapEffectPackedPage);
    memset(MapEffectPositionXLow,0x99,sizeof MapEffectPositionXLow); memset(MapEffectPositionYLow,0x99,sizeof MapEffectPositionYLow);
    NextMapEffectSlot=0xEF; MechSlotByMenuRow[0]=0xBE; ViewedHolodisk=0xAA; TransmittedCacheFound=0xBEEF;
    PurchasedMedkit=PurchasedFieldSurgeryKit=1;
    for (unsigned i=0;i<StockCompanyCount;++i) StockBalances[i]=0x12345678;
    Load_Game_Map_Data();
    assert(step==9 && randomCalls==8 && CBills==20 && NextRecruitNameId==1);
    for (unsigned i=0;i<LanceSize;++i) assert(StoredPartyMechNameInitial[i]==255);
    for (unsigned i=0;i<MechRecordCount;++i) assert(Mechs[i].name[0]==255 && Mechs[i].name[1]==0xBB && Mechs[i].tonnage==0xBB);
    for (unsigned i=1;i<PartySize;++i) assert(Characters[i].name==255 && Characters[i].body==0xAA);
    assert(Characters[PartySize].name==0xAA); /* enemy records are not cleared */
    const Character expected={0,8,9,7,0,0,0,0,0,0,0,Infantry_Cudgel,Character_OnFoot,0,0,80,0};
    assert(!memcmp(&Characters[0],&expected,sizeof expected));
    for (unsigned i=0;i<PersistentBldStateBytes;++i) assert(PersistentState.bytes[i]==(i==51?50:0));
    assert(PartyHealthRecoveryTimer==0 && PartyHasInjuredMember==0 && TraitorBattleProbability==0);
    /* Both named and byte-addressed accesses are the SAME original state. */
    PartyHealthRecoveryTimer=63; assert(PersistentState.bytes[41]==63);
    PersistentState.bytes[18]=1; assert(MechModificationWorkflowEnabled==1);
    for (unsigned i=0;i<CacheSecurityCodeCount;++i) assert(CacheSecurityCodeUsed[i]==0);
    for (unsigned i=0;i<MapFogOfWarBytes;++i) {
        unsigned relative=i-CitadelInitialRevealIndex;
        int reveal=i>=CitadelInitialRevealIndex && relative/MapFogOfWarRowBytes<6 && relative%MapFogOfWarRowBytes==0;
        assert(MapFogOfWar[i]==(reveal?0x9F:0x80));
    }
    for (unsigned i=0;i<StockCompanyCount;++i) assert(StockBalances[i]==0);
    for (unsigned i=0;i<LanceSize;++i) {
        assert(CombatantSpriteFamilyOffset[i]==0x33); /* even RNG leaves old family */
        assert(CombatantSpriteFamilyOffset[i+12]==MECH_Sprite_COMMANDO);
    }
    assert(CombatantSpriteFamilyOffset[4]==CharacterSpriteFamily_Jason && CombatantSpriteFamilyOffset[5]==0x33);
    for (unsigned i=16;i<AllCombatantCount;++i) assert(CombatantSpriteFamilyOffset[i]==CharacterSpriteFamily_EnemyInfantry);
    assert(NextMapEffectSlot==0 && MechSlotByMenuRow[0]==0xBE && PurchasedMedkit==0 && PurchasedFieldSurgeryKit==0);
    for (unsigned i=0;i<PersistentMapEffectSlotCount;++i)
        assert(MapEffectSpriteIndex[i]==0x88 && MapEffectPackedPage[i]==0 && MapEffectPositionXLow[i]==0 && MapEffectPositionYLow[i]==0);
    assert(ViewedHolodisk==0 && TransmittedCacheFound==0 && TraitorWarning==0 && DrawJailMissionParkedMechs==0);
    assert(CurrentMap==Map_Citadel && MainCharactersAlive==TRUE);
    puts("Original destructive new-game reset, overlapping state, Jason/economy/fog, stale-byte preservation and world-entry order passed");
    return 0;
}
