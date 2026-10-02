#include "game.h"


/* Sol: Original0800:4DC7..50C7 complete ASM. DESTRUCTIVE new-game reset, not
 * save loading. Only native assigned bytes/fields change: fog is OR-revealed,
 * absent records retain other fields, even RNG leaves mech families untouched,
 * effect sprite IDs and the adjacent mech-menu BYTE D558 survive. */
void Load_Game_Map_Data(void)
{
    for (uint16_t slot=0;slot<LanceSize;++slot) StoredPartyMechNameInitial[slot]=MECH_Destroyed;
    for (uint16_t slot=0;slot<MechRecordCount;++slot) Mechs[slot].name[0]=MECH_Destroyed;
    for (uint16_t slot=1;slot<PartySize;++slot) Characters[slot].name=Character_Dead;
    /* unsigned char access to object representation is defined C; avoids
     * stepping beyond a scalar member as though it were a seven-element array. */
    uint8_t *jasonBytes=(uint8_t *)&Characters[Character_Jason];
    for (uint16_t skill=0;skill<CharacterSkillCount;++skill)
        jasonBytes[offsetof(Character,skillBowsAndBlade)+skill]=SkillLevel_Unskilled;
    for (uint16_t index=0;index<PersistentBldStateBytes;++index) PersistentState.bytes[index]=0;
    for (uint16_t index=0;index<CacheSecurityCodeCount;++index) CacheSecurityCodeUsed[index]=FALSE;
    Characters[Character_Jason].name=Character_Jason;
    Characters[Character_Jason].mechAssignment=Character_OnFoot;
    CBills=NewGameStartingCBills;
    for (uint16_t stock=0;stock<StockCompanyCount;++stock) StockBalances[stock]=0;
    NextRecruitNameId=1;
    for (uint16_t row=0;row<CitadelInitialRevealRows;++row)
        MapFogOfWar[CitadelInitialRevealIndex+row*MapFogOfWarRowBytes]|=CitadelInitialRevealMask;
    Characters[Character_Jason].body=JasonStartingBody;
    Characters[Character_Jason].health=JasonStartingBody*CharacterHealthPerBodyPoint;
    Characters[Character_Jason].dexterity=JasonStartingDexterity;
    Characters[Character_Jason].charisma=JasonStartingCharisma;
    Characters[Character_Jason].weapon=Infantry_Cudgel;
    Characters[Character_Jason].trainingFlags=0;
    Characters[Character_Jason].armourValue=0;
    Characters[Character_Jason].armourType=0;
    CombatantSpriteFamilyOffset[Friendly_Infantry_Combatant_Range_First+Character_Jason]=CharacterSpriteFamily_Jason;
    for (uint16_t id=Enemy_Infantry_CombatantId_Range_First;id<AllCombatantCount;++id)
        CombatantSpriteFamilyOffset[id]=CharacterSpriteFamily_EnemyInfantry;
    for (uint16_t slot=0;slot<LanceSize;++slot)
        for (uint16_t side=0;side<=Enemy_All_CombatantId_Range_First;side+=Enemy_All_CombatantId_Range_First)
            if ((Rand_0x00_to_0xFF()&1)!=0) CombatantSpriteFamilyOffset[side+slot]=MECH_Sprite_COMMANDO;
    PersistentState.fields.allowanceLow=NewGameAllowanceWealthLimit;
    PersistentState.fields.allowanceHigh=0;
    PurchasedFieldSurgeryKit=FALSE; PurchasedMedkit=FALSE;
    NextMapEffectSlot=0; /* native BYTE store D557: adjacent D558 survives */
    for (uint16_t slot=0;slot<PersistentMapEffectSlotCount;++slot) {
        MapEffectPositionYLow[slot]=0; MapEffectPositionXLow[slot]=0; MapEffectPackedPage[slot]=0;
    }
    Graphics_Set_Screen_To_Black();
    Draw_Health_and_C_Bills_Sidebar(TRUE);
    Draw_Top_Graphic_Sidebar(); /* duplicate clear/redraw is original */
    CrescentHawkMapPositionX=CitadelStartingPositionX;
    CrescentHawkMapPositionY=CitadelStartingPositionY;
    Map_Construct_Nine_Regions(CitadelStartingPackedPage);
    DOS_Load_Map_Files(MapCacheSlot_Centre,Map_Citadel);
    Map_NineGrid_Parent();
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    Copy_Data_To_GraphicsMemory();
    for (uint16_t id=0;id<Enemy_All_CombatantId_Range_First;++id) {
        uint16_t enemyId=id+Enemy_All_CombatantId_Range_First;
        CombatantSpriteFrame[id]=CombatantSpriteFrame[enemyId]=(uint8_t)(id<LanceSize?0:RoamingNpcInitialFrame);
        CombatantAnimationSelector[id]=CombatantAnimationSelector[enemyId]=AnimationDirection_Refresh;
    }
    Draw_Infantry_And_Mechs();
    ViewedHolodisk=FALSE; TransmittedCacheFound=FALSE;
    DrawJailMissionParkedMechs=FALSE; TraitorWarning=FALSE;
    CurrentMap=Map_Citadel; MainCharactersAlive=TRUE;
}
