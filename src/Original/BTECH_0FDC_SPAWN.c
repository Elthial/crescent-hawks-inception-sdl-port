#include "game.h"

/* Original EXE-owned FAR template table and adjacent BYTE views. */
Mech *EnemySpawnTemplates[EnemySpawnTemplateCount]={
    &MechRefs[MechRef_Jenner],&MechRefs[MechRef_UrbanMech],&MechRefs[MechRef_Commando],
    &MechRefs[MECH_REF_Locust],&MechRefs[MechRef_Wasp],&MechRefs[MECH_REF_Locust],
    &MechRefs[MechRef_Stinger],&MechRefs[MechRef_Wasp],&MechRefs[MechRef_Commando],
    &MechRefs[MechRef_Jenner],&MechRefs[MechRef_UrbanMech]};
uint8_t EnemySpawnSpriteFamilies[EnemySpawnTemplateCount]={MECH_Sprite_LOCUST,MECH_Sprite_LOCUST,MECH_Sprite_COMMANDO,
    MECH_Sprite_LOCUST,MECH_Sprite_COMMANDO,MECH_Sprite_LOCUST,MECH_Sprite_COMMANDO,MECH_Sprite_COMMANDO,
    MECH_Sprite_COMMANDO,MECH_Sprite_LOCUST,MECH_Sprite_LOCUST};
uint8_t EnemySpawnSharedData[EnemySpawnSharedDataBytes]={0,4,0,4,0,1,2,0,1,2,0,1,0,0,4,4,1,1,1,2,2,2,3,3,0,1,3,5,10,14,20};
uint8_t EnemySpectatorAnimationStream[3]={16,254,2};

/* Original0FDC:0D49..134A. Caller supplies native-valid Mech count<=4 and
 * damage level0..6 (positive high levels are clamped natively). High-bit
 * corrupt damage indexes require the wider original data window, not a clamp.
 * Native pilots share character storage with generated infantry. */
void Mission_GenerateEnemies(uint16_t mechRequest,uint16_t infantryRequest)
{
    uint16_t family=MECH_Sprite_LOCUST,spawnX,spawnY;
    Mech *selectedTemplate=&MechRefs[MECH_REF_Locust];
    CombatantActionState[Character_Jason]=0;
    for(uint16_t npc=0;npc<MapCharacterCount;++npc) {
        RoamingMapNpcs[npc].currentPositionX=CombatantPackedX[Enemy_Infantry_CombatantId_Range_First+npc];
        RoamingMapNpcs[npc].currentPositionY=CombatantPackedY[Enemy_Infantry_CombatantId_Range_First+npc];
    }
    for(uint16_t mech=Enemy_Mech_Record_First;mech<MechRecordCount;++mech) Mechs[mech].name[0]=MECH_Destroyed;
    for(uint16_t character=Enemy_Infantry_Record_First;character<CharacterRecordCount;++character) Characters[character].name=Character_Dead;
    for(uint16_t actor=0;actor<AllCombatantCount;++actor) {
        CombatantCasualtyFlags[actor]=FALSE; CombatantActive[actor]=FALSE;
        CombatantPackedY[actor]=CombatantPosition_Unused; CombatantPackedX[actor]=CombatantPosition_Unused;
    }
    CombatantActive[(int16_t)infantryRequest<EnemyOnFootDeploymentThreshold?Character_Jason:Friendly_Infantry_Combatant_Range_First]=TRUE;
    if((mechRequest&PackedPositionLocalCarryBit)!=0) {
        spawnX=(Rand_0x00_to_0xFF()&EnemyArenaSpawnRandomMask)+EnemyArenaLeftSpawnX;
        if((Rand_0x00_to_0xFF()&1)!=0) spawnX=(Rand_0x00_to_0xFF()&EnemyArenaSpawnRandomMask)+EnemyArenaRightSpawnX;
        spawnY=(Rand_0x00_to_0xFF()&EnemyArenaSpawnRandomMask)+EnemyArenaSpawnY;
        if(ArenaRentalMechMode!=FALSE)
            for(uint16_t mech=0;mech<LanceSize;++mech)
                if(StoredPartyMechNameInitial[mech]==MECH_Destroyed) mechRequest=2;
        uint16_t choice=Roll2D6()-EnemyChassisDiceMinimum;
        if(ArenaRentalMechMode!=FALSE) choice=(Rand_0x00_to_0xFF()&EnemyArenaSpawnRandomMask)+EnemyRentalChassisFirstIndex;
        selectedTemplate=EnemySpawnTemplates[choice]; family=(uint16_t)(int16_t)(int8_t)EnemySpawnSpriteFamilies[choice];
    } else {
        spawnX=(Rand_0x00_to_0xFF()&EnemyTrainingSpawnRandomMask)+EnemyTrainingSpawnX;
        spawnY=(Rand_0x00_to_0xFF()&EnemyTrainingSpawnRandomMask)+EnemyTrainingSpawnY;
        if((infantryRequest&PackedPositionLocalCarryBit)!=0) { spawnX=EnemyJailSpawnX; spawnY=EnemyJailSpawnY; }
        if(PersistentState.bytes[0]==Mission_DisabledLocust) spawnX=EnemyDisabledLocustSpawnX;
    }
    uint16_t nextPilot=Enemy_Infantry_Record_First,mechCount=mechRequest&PackedPositionLocalMask;
    if(KuritaAttackFlag!=FALSE) mechCount=LanceSize;
    for(uint16_t slot=0;slot<mechCount;++slot) {
        uint16_t actor=Enemy_All_CombatantId_Range_First+slot;
        Characters[nextPilot].name=0; Characters[nextPilot].mechAssignment=(uint8_t)(Enemy_Mech_Record_First+slot); ++nextPilot;
        Mech *enemy=&Mechs[Enemy_Mech_Record_First+slot]; uint8_t *enemyBytes=(uint8_t *)enemy;
        for(uint16_t byte=0;byte<MechRecordSize;++byte) {
            enemyBytes[byte]=((uint8_t *)(KuritaAttackFlag!=FALSE?&MechRefs[MechRef_Jenner]:selectedTemplate))[byte];
            if(ArenaRentalMechMode!=FALSE && slot!=0) ((uint8_t *)&Mechs[Enemy_Mech_Record_First+1])[byte]=((uint8_t *)&MechRefs[MechRef_Spectator])[byte];
        }
        if(KuritaAttackFlag!=FALSE) EnemyMechDamageLevel=0;
        if((int8_t)EnemyMechDamageLevel>EnemyDamageLevelMaximum) EnemyMechDamageLevel=EnemyDamageLevelMaximum;
        for(uint16_t armour=0;armour<MechArmourLocationCount;++armour) {
            uint8_t damage=EnemySpawnSharedData[EnemyArmourDamageOffset+(int8_t)EnemyMechDamageLevel];
            enemy->currentArmour[armour]=enemy->currentArmour[armour]>damage?(uint8_t)(enemy->currentArmour[armour]-damage):0;
        }
        if((int8_t)EnemyMechDamageLevel>EnemyCriticalDamageThreshold) {
            for(uint16_t attempt=0;attempt<EnemyCriticalDamageAttempts;++attempt) {
                uint16_t offset=(Rand_0x00_to_0xFF()&EnemyCriticalDamageRandomMask)+EnemyCriticalDamageFirstOffset;
                if(enemyBytes[offset]!=0) enemyBytes[offset]|=Component_Destroyed;
            }
            if((int8_t)EnemyMechDamageLevel>EnemySystemDamageThreshold) {
                enemy->engineHits=Rand_0x00_to_0xFF()&1; enemy->sensorHits=Rand_0x00_to_0xFF()&1; enemy->gyroHits=Rand_0x00_to_0xFF()&1;
            }
        }
        CombatantSpriteFamilyOffset[actor]=(uint8_t)family; CombatantSpriteFrame[actor]=EnemyInitialMechFrame;
        CombatantAnimationCursors[actor]=(AnimationCursor){WalkAnimationStreams,WalkAnimationStreamNativeOffset,
            WalkAnimationStreamNativeOffset+EnemyInitialDirection*WalkAnimationStreamBytes}; /*native FAR2FE8:02A0, not a mutable friendly table lookup*/
        CombatantMovementDirection[actor]=EnemyInitialDirection; CombatantAnimationSelector[actor]=EnemyInitialDirection; CombatantActive[actor]=TRUE;
        CombatantPackedX[actor]=(uint16_t)(spawnX+(int8_t)EnemySpawnSharedData[slot]);
        CombatantPackedY[actor]=(uint16_t)(spawnY+(int8_t)EnemySpawnSharedData[EnemySpawnYOffset+slot]);
        if(ArenaRentalMechMode!=FALSE && slot!=0) {
            uint16_t spectator=Enemy_All_CombatantId_Range_First+1;
            CombatantSpriteFrame[spectator]=EnemySpectatorFrame; CombatantSpriteFamilyOffset[spectator]=MECH_Sprite_LOCUST;
            CombatantAnimationCursors[spectator]=(AnimationCursor){EnemySpectatorAnimationStream,EnemySpectatorStreamOffset,EnemySpectatorStreamOffset};
            CombatantMovementDirection[spectator]=EnemySpectatorDirection; CombatantAnimationSelector[spectator]=EnemySpectatorDirection;
            CombatantPackedX[spectator]=EnemySpectatorX; CombatantPackedY[spectator]=EnemySpectatorY;
        }
    }
    if(infantryRequest!=0) {
        uint16_t limit=(uint16_t)(nextPilot+(infantryRequest&PackedPositionLocalMask));
        if((int16_t)limit>CharacterRecordCount) limit=CharacterRecordCount;
        for(uint16_t record=nextPilot;(int16_t)record<(int16_t)limit;++record) {
            uint16_t actor=record+Enemy_Infantry_Record_First; Character *infantry=&Characters[record];
            infantry->name=0; infantry->mechAssignment=Character_OnFoot;
            CombatantSpriteFamilyOffset[actor]=CharacterSpriteFamily_EnemyInfantry; CombatantSpriteFrame[actor]=EnemyInitialInfantryFrame;
            CombatantAnimationCursors[actor]=(AnimationCursor){WalkAnimationStreams,WalkAnimationStreamNativeOffset,
                WalkAnimationStreamNativeOffset+(CompassDirectionCount+EnemyInitialDirection)*WalkAnimationStreamBytes}; /*native FAR2FE8:02E0*/
            CombatantMovementDirection[actor]=EnemyInitialDirection; CombatantAnimationSelector[actor]=EnemyInitialDirection; CombatantActive[actor]=TRUE;
            CombatantPackedX[actor]=(uint16_t)(spawnX+(int8_t)EnemySpawnSharedData[record]);
            CombatantPackedY[actor]=(uint16_t)(spawnY+(int8_t)EnemySpawnSharedData[EnemySpawnYOffset+record]);
            for(uint16_t skill=0;skill<EnemyRandomizedSkillCount;++skill)
                ((uint8_t *)infantry)[offsetof(Character,skillBowsAndBlade)+skill]=Rand_0x00_to_0xFF()&1;
            infantry->body=(uint8_t)Roll2D6(); infantry->health=(uint8_t)((int8_t)infantry->body*CharacterHealthPerBodyPoint);
            infantry->weapon=Rand_0x00_to_0xFF()%EnemyInfantryWeaponTypeCount;
        }
    }
}
