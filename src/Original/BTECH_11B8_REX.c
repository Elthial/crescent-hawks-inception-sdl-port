#include "game.h"

/* EXE-owned3EDB:1E7A..1E91: Rex skills and overlapping spawn offsets. */
uint8_t RexAmbushData[RexAmbushDataBytes]={1,3,2,4,4,1,0,0,0,1,2,0,1,2,0,1,1,1,1,2,2,2,3,3};

/* Original11B8:104E..137E. Sol: Recruit Rex, install a hidden Commando,
 * save roaming NPC positions, replace encounter actors with two friends and
 * two to five soldiers, then enter prearranged combat. Preserve the missing
 * full-Lance guard: slot4 overwrites enemy Mech4 and D456 recruit-name BYTE. */
void Recruit_Rex_And_Start_KuritaParty_Ambush(void)
{
    CrescentHawkMapPositionX=KuritaPartyAlleyX;
    CrescentHawkMapPositionY=KuritaPartyAlleyY;
    Character *rex=&Characters[Character_Rex];
    uint8_t name=NextRecruitNameId;
    ++NextRecruitNameId;
    rex->name=name; rex->weapon=Infantry_Pistol;
    rex->body=RexBody; rex->dexterity=RexDexterity; rex->charisma=RexCharisma;
    rex->health=RexBody*CharacterHealthPerBodyPoint;
    rex->trainingFlags=0; rex->armourValue=0; rex->armourType=ArmourType_None;
    CombatantSpriteFamilyOffset[Friendly_Infantry_Combatant_Range_First+Character_Rex]=0;
    rex->mechAssignment=Character_OnFoot;
    for(uint16_t skill=0;skill<CharacterSkillCount;++skill)
        ((uint8_t *)rex)[offsetof(Character,skillBowsAndBlade)+skill]=RexAmbushData[skill];

    uint16_t commandoSlot=0;
    while(commandoSlot<LanceSize && StoredPartyMechNameInitial[commandoSlot]!=MECH_Destroyed) ++commandoSlot;
    for(uint16_t byte=0;byte<MechRecordSize;++byte)
        ((uint8_t *)&Mechs[commandoSlot])[byte]=((uint8_t *)&MechRefs[MechRef_Commando])[byte];
    /* D452+4 is D456. Access the encompassing NPC record representation,
     * rather than indexing element4 of the four-BYTE saved-name view. */
    ((uint8_t *)&RoamingMapNpcs[MapCharacterCount-1])[
        offsetof(RoamingMapNpc,remainingNativeRecord)+2+commandoSlot]='C';
    Mechs[commandoSlot].name[0]=MECH_Destroyed;
    Mechs[commandoSlot].pilotId=Character_Rex;
    CombatantSpriteFamilyOffset[commandoSlot]=MECH_Sprite_COMMANDO;
    for(uint16_t npc=0;npc<MapCharacterCount;++npc) {
        RoamingMapNpcs[npc].currentPositionX=CombatantPackedX[Enemy_Infantry_CombatantId_Range_First+npc];
        RoamingMapNpcs[npc].currentPositionY=CombatantPackedY[Enemy_Infantry_CombatantId_Range_First+npc];
    }
    for(uint16_t mech=Enemy_Mech_Record_First;mech<MechRecordCount;++mech) Mechs[mech].name[0]=MECH_Destroyed;
    for(uint16_t record=Enemy_Infantry_Record_First;record<CharacterRecordCount;++record) Characters[record].name=Character_Dead;
    for(uint16_t actor=0;actor<AllCombatantCount;++actor) {
        CombatantCasualtyFlags[actor]=FALSE; CombatantActive[actor]=FALSE;
        CombatantPackedY[actor]=CombatantPosition_Unused; CombatantPackedX[actor]=CombatantPosition_Unused;
    }
    CombatantActive[Friendly_Infantry_Combatant_Range_First+Character_Rex]=TRUE;
    CombatantActive[Friendly_Infantry_Combatant_Range_First+Character_Jason]=TRUE;
    CombatantActionState[Friendly_Infantry_Combatant_Range_First+Character_Rex]=0;
    CombatantActionState[Friendly_Infantry_Combatant_Range_First+Character_Jason]=0;
    uint16_t originX=(Rand_0x00_to_0xFF()&1)+RexAmbushOriginX;
    uint16_t originY=(Rand_0x00_to_0xFF()&1)+RexAmbushOriginY;
    uint16_t recordEnd=(Rand_0x00_to_0xFF()&RexAmbushCountMask)+RexAmbushMinimumRecordEnd;
    for(uint16_t record=Enemy_Infantry_Record_First;record<recordEnd;++record) {
        Character *enemy=&Characters[record];
        uint16_t actor=record+Enemy_Infantry_Record_First;
        enemy->name=0; enemy->mechAssignment=Character_OnFoot;
        CombatantSpriteFamilyOffset[actor]=CharacterSpriteFamily_EnemyInfantry;
        CombatantSpriteFrame[actor]=EnemyInitialInfantryFrame;
        CombatantAnimationCursors[actor]=(AnimationCursor){WalkAnimationStreams,WalkAnimationStreamNativeOffset,
            WalkAnimationStreamNativeOffset+(CompassDirectionCount+EnemyInitialDirection)*WalkAnimationStreamBytes};
        CombatantMovementDirection[actor]=EnemyInitialDirection;
        CombatantAnimationSelector[actor]=EnemyInitialDirection;
        CombatantActive[actor]=TRUE;
        CombatantPackedX[actor]=(uint16_t)(originX+(int8_t)RexAmbushData[record]);
        CombatantPackedY[actor]=(uint16_t)(originY+(int8_t)RexAmbushData[RexAmbushYOffset+record]);
        for(uint16_t skill=0;skill<EnemyRandomizedSkillCount;++skill)
            ((uint8_t *)enemy)[offsetof(Character,skillBowsAndBlade)+skill]=Rand_0x00_to_0xFF()&1;
        enemy->body=(uint8_t)Roll2D6();
        enemy->health=(uint8_t)((int8_t)enemy->body*CharacterHealthPerBodyPoint);
        enemy->weapon=Rand_0x00_to_0xFF()%RexAmbushWeaponCount;
    }
    Combat_Run_Encounter(UINT16_MAX);
}
