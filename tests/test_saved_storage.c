#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Saved storage mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
int main(void)
{
    uint8_t native[OriginalSavedStateBytes];
    uint8_t *bytes=OriginalSavedState.bytes;
    check((uint8_t *)Characters==bytes);
    check((uint8_t *)&WorldMapState==bytes+0x110);
    check((uint8_t *)&CBills==bytes+0xD5C && (uint8_t *)StockBalances==bytes+0xD60);
    check((uint8_t *)RoamingMapNpcs==bytes+0xD7C);
    check(MapEffectSpriteIndex==bytes+0xE43 && MapEffectPackedPage==bytes+0xE83);
    check(MapEffectPositionXLow==bytes+0xEC3 && MapEffectPositionYLow==bytes+0xF03);
    check(&NextMapEffectSlot==bytes+0xF43 && MechSlotByMenuRow==bytes+0xF44);
    check(WeaponProficiencyUseCount==bytes+(0xD35C - 0xC614));
    check(LastWeaponProficiencyCategory==bytes+(0xD364 - 0xC614));
    /* Model the original one raw read: not a new serialization format. */
    for(unsigned seed=0;seed<256;++seed) {
        for(unsigned index=0;index<OriginalSavedStateBytes;++index) native[index]=(uint8_t)(index*37+seed);
        memset(MechSlotByMenuRow,0xBE,LanceSize);
        memcpy(bytes,native,sizeof native);
        check(!memcmp(Characters,native,sizeof Characters));
        check(!memcmp(WorldMapState.bytes,native+0x110,sizeof WorldMapState));
        check(!memcmp(RoamingMapNpcs,native+0xD7C,sizeof RoamingMapNpcs));
        check(!memcmp(MapEffectSpriteIndex,native+0xE43,64));
        check(!memcmp(MapEffectPackedPage,native+0xE83,64));
        check(!memcmp(MapEffectPositionXLow,native+0xEC3,64));
        check(!memcmp(MapEffectPositionYLow,native+0xF03,64));
        check(NextMapEffectSlot==native[0xF43]);
        check(!memcmp(WeaponProficiencyUseCount,native+(0xD35C - 0xC614),PartySize));
        check(!memcmp(LastWeaponProficiencyCategory,native+(0xD364 - 0xC614),PartySize));
        for(unsigned row=0;row<LanceSize;++row) check(MechSlotByMenuRow[row]==0xBE);
        uint32_t expectedCash=(uint32_t)native[0xD5C]|((uint32_t)native[0xD5D]<<8)|
            ((uint32_t)native[0xD5E]<<16)|((uint32_t)native[0xD5F]<<24);
        check(CBills==expectedCash); /* current preservation target is little endian */
        for(unsigned byte=0;byte<9;++byte) {
            MapEffectSpriteIndex[byte]=(uint8_t)(seed+byte);
            check(RoamingMapNpcs[7].remainingNativeRecord[7+byte]==(uint8_t)(seed+byte));
            RoamingMapNpcs[7].remainingNativeRecord[7+byte]^=0xFF;
            check(MapEffectSpriteIndex[byte]==(uint8_t)((seed+byte)^0xFF));
        }
        NextMapEffectSlot=0; check(MechSlotByMenuRow[0]==0xBE);
        MechSlotByMenuRow[0]=0xA7; check(NextMapEffectSlot==0);
    }
    /* Typed gameplay writes are immediately present in the native raw block. */
    memset(bytes,0,sizeof OriginalSavedState.bytes);
    Characters[0].health=80; Mechs[0].currentAmmo[0]=17;
    CBills=UINT32_C(0x12345678); StockBalances[2]=UINT32_C(0xABCDEF01);
    PurchasedMedkit=1; StoredPartyMechNameInitial[3]='W';
    check(bytes[15]==80 && bytes[0x110+offsetof(Mech,currentAmmo)]==17);
    check(bytes[0xD5C]==0x78 && bytes[0xD5F]==0x12 && bytes[0xD68]==1 && bytes[0xD6B]==0xAB);
    check(bytes[0xE3C]==1 && bytes[0xE41]=='W');
    return 0;
}
