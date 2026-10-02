#include "game.h"
#include <string.h>

/* Sol: Original0DAB:0D3D..1466, complete random encounter generation.
 * ASM checked: four origin rolls, eight infantry rolls, four Mech rolls,
 * then the original two scans. BYTE/WORD truncation and signed JG preserved.
 * BUG-005: adjacent Mech cell may cross a row. Search has no native row cap. */
enum {
    EncounterDistanceMask=7,EncounterMinimumDistance=10,
    EncounterPartyScreenX=26,EncounterPartyScreenY=12,
    EncounterWeaponRollCount=7,EncounterSmallRollMask=3,
    EncounterTemplateCount=3,EncounterHealthPerBody=10,
    EncounterSearchLastColumn=16,EncounterMechLastColumn=8,
    EncounterMechSpacing=3,EncounterInfantryFrame=16,
    EncounterFirstGroundTileExclusive=15,EncounterTerrainOffset=0x07AD,
    EncounterPackedEastCarry=0x0080,EncounterPackedSouthCarry=0x0F80
};
/* Original EXE-owned3EDB:2CF4 and FAR template table3EDB:2DF8.
 * Native27E8 template segment relocates to analysis2FE8. */
static const uint8_t enemyWeaponByRollSum[22]={
    10,12,9,7,0,8,1,3,5,7,7,8,6,4,3,2,0,0,8,9,13,11
};
void Generate_Random_Encounter_Enemies(void)
{
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    Update_Cached_Map_Origin();
    int16_t offsetX=(int16_t)((Rand_0x00_to_0xFF()&EncounterDistanceMask)+EncounterMinimumDistance);
    if (Rand_0x00_to_0xFF()&1) offsetX=-offsetX;
    int16_t offsetY=(int16_t)((Rand_0x00_to_0xFF()&EncounterDistanceMask)+EncounterMinimumDistance);
    if (Rand_0x00_to_0xFF()&1) offsetY=-offsetY;
    uint16_t scanX=(uint16_t)(offsetX+EncounterPartyScreenX),rowStartX=scanX;
    uint16_t scanY=(uint16_t)(offsetY+EncounterPartyScreenY);
    for (uint16_t actor=Enemy_All_CombatantId_Range_First;actor<AllCombatantCount;++actor) {
        CombatantActive[actor]=FALSE;
        CombatantPackedY[actor]=CombatantPosition_Unused;
        CombatantPackedX[actor]=CombatantPosition_Unused;
    }
    for (uint16_t record=PartySize;record<CharacterRecordCount;++record) {
        Character *person=&Characters[record];
        person->name=Character_Dead;
        if (!(Rand_0x00_to_0xFF()&1)) continue;
        person->name=1; /* Original generated-enemy name selector; label unresolved. */
        person->mechAssignment=Character_OnFoot;
        uint16_t weaponRoll=0;
        for (uint16_t roll=0;roll<EncounterWeaponRollCount;++roll)
            weaponRoll+=(Rand_0x00_to_0xFF()&EncounterSmallRollMask);
        person->weapon=enemyWeaponByRollSum[weaponRoll];
        person->body=(uint8_t)Roll2D6();
        person->health=(uint8_t)(person->body*EncounterHealthPerBody);
        person->dexterity=(uint8_t)Roll2D6();
        /* Character bytes4..10 are seven skills. Use whole-object byte view,
         * not an out-of-bounds array starting at the first scalar member. */
        for (uint16_t skill=0;skill<EncounterWeaponRollCount;++skill)
            ((uint8_t *)person)[4+skill]=Rand_0x00_to_0xFF()&EncounterSmallRollMask;
        person->armourType=Rand_0x00_to_0xFF()&EncounterSmallRollMask;
        uint8_t firstArmourRoll=(uint8_t)Roll2D6();
        uint8_t secondArmourRoll=(uint8_t)Roll2D6();
        person->armourValue=(uint8_t)(firstArmourRoll+secondArmourRoll);
    }
    for (uint16_t record=LanceSize;record<MechRecordCount;++record) {
        Mechs[record].name[0]=MECH_Destroyed;
        uint8_t chance=Rand_0x00_to_0xFF();
        if ((chance&1) && Mechs[record-LanceSize].name[0]!=MECH_Destroyed) {
            uint16_t templateId=Rand_0x00_to_0xFF()%EncounterTemplateCount;
            memcpy(&Mechs[record],&MechRefs[templateId],sizeof(Mech));
            CombatantSpriteFamilyOffset[record+PartySize]=
                templateId==0?MECH_Sprite_LOCUST:MECH_Sprite_COMMANDO;
        }
    }
    uint16_t scanColumn=0,placedInfantry=0;

    for (uint16_t record=PartySize;record<CharacterRecordCount;++record) {
        if (Characters[record].name==Character_Dead) continue;
        int16_t halfX,halfY;
        for (;;) {
            int16_t relativeX=(int16_t)(uint16_t)(scanX-MapViewportLeftByte);
            halfX=(int16_t)(relativeX>=0?relativeX/2:-((-(int32_t)relativeX+1)/2));
            int16_t signedY=(int16_t)scanY;
            halfY=(int16_t)(signedY>=0?signedY/2:-((-(int32_t)signedY+1)/2));
            uint16_t candidate=(uint16_t)(halfY*MapCacheWidth+halfX+CachedMapOriginIndex);
            if (!(scanX&1) && (CrescentHawkMapPositionX&1)) ++candidate;
            if ((scanY&1) && (CrescentHawkMapPositionY&1)) candidate+=MapCacheWidth;
            /* Native reads precede signed rejection. Address wraps as a WORD.
             * Contract: the address lies in the reconstructed native map storage.
             * No new scan cap or tile-array clamp is imposed. */
            uint16_t address=(uint16_t)(EncounterTerrainOffset+candidate);
            uint16_t tile=MapRuntime.bytes[address-MapRuntimeFirstOffset];
            
            if (tile>EncounterFirstGroundTileExclusive &&
                (int16_t)BlockingTileCodeThreshold>(int16_t)tile && (int16_t)candidate>=0) break;
            ++scanX; ++scanColumn;
            if ((int16_t)scanColumn>EncounterSearchLastColumn) {
                scanColumn=0; scanX=rowStartX; ++scanY;
            }
        }
        uint16_t column=(uint16_t)(halfX+CachedMapOriginColumn);
        if (!(scanX&1) && (CrescentHawkMapPositionX&1)) ++column;
        uint16_t row=(uint16_t)(halfY*MapCacheWidth+CachedMapOriginRowOffset);
        if ((scanY&1) && (CrescentHawkMapPositionY&1)) row+=MapCacheWidth;
        uint16_t actor=record+PartySize;
        if ((int16_t)column<0 || (int16_t)column>=MapCacheWidth ||
            (int16_t)row<0 || (int16_t)row>=MapCacheTileCount) {
            CombatantPackedY[actor]=CombatantPosition_Unused;
            CombatantPackedX[actor]=CombatantPosition_Unused;
            Characters[record].name=Character_Dead;
            CombatantActive[actor]=FALSE;
        } else {
            uint16_t x=(uint16_t)(CrescentHawkMapPositionX+scanX-EncounterPartyScreenX);
            if (x&PackedPositionLocalCarryBit) {
                if ((int16_t)scanX<EncounterPartyScreenX) x&=PackedPositionWestNormalizeMask;
                else x+=EncounterPackedEastCarry;
            }
            uint16_t y=(uint16_t)(CrescentHawkMapPositionY+scanY-EncounterPartyScreenY);
            if (y&PackedPositionLocalCarryBit) {
                if ((int16_t)scanY<EncounterPartyScreenY) y&=PackedPositionNorthNormalizeMask;
                else y+=EncounterPackedSouthCarry;
            }
            CombatantPackedX[actor]=x; CombatantPackedY[actor]=y;
            CombatantSpriteFrame[actor]=EncounterInfantryFrame;
            CombatantActive[actor]=TRUE;
            ++placedInfantry;
        }
        scanColumn+=1; scanX+=1;
        if ((int16_t)scanColumn>EncounterSearchLastColumn) {
            scanColumn=0; scanX=rowStartX; ++scanY;
        }
    }

    /* Native11F5: one-unit separation, deliberately no wrap check. */
    ++scanX; ++scanColumn;

    for (uint16_t record=LanceSize;record<MechRecordCount;++record) {
        if (Mechs[record].name[0]==MECH_Destroyed) continue;
        int16_t halfX,halfY;
        for (;;) {
            int16_t relativeX=(int16_t)(uint16_t)(scanX-MapViewportLeftByte);
            halfX=(int16_t)(relativeX>=0?relativeX/2:-((-(int32_t)relativeX+1)/2));
            int16_t signedY=(int16_t)scanY;
            halfY=(int16_t)(signedY>=0?signedY/2:-((-(int32_t)signedY+1)/2));
            uint16_t candidate=(uint16_t)(halfY*MapCacheWidth+halfX+CachedMapOriginIndex);
            if (!(scanX&1) && (CrescentHawkMapPositionX&1)) ++candidate;
            if ((scanY&1) && (CrescentHawkMapPositionY&1)) candidate+=MapCacheWidth;
            /* Native reads precede signed rejection. Address wraps as a WORD.
             * Contract: the address lies in the reconstructed native map storage.
             * No new scan cap or tile-array clamp is imposed. */
            uint16_t address=(uint16_t)(EncounterTerrainOffset+candidate);
            uint16_t tile=MapRuntime.bytes[address-MapRuntimeFirstOffset];
            uint16_t adjacent=(uint16_t)(candidate+(((scanX^CrescentHawkMapPositionX)&1)?-1:1));
            address=(uint16_t)(EncounterTerrainOffset+adjacent);
            uint16_t adjacentTile=MapRuntime.bytes[address-MapRuntimeFirstOffset];
            if (tile>EncounterFirstGroundTileExclusive &&
                (int16_t)BlockingTileCodeThreshold>(int16_t)tile && (int16_t)candidate>=0 &&
                adjacentTile>EncounterFirstGroundTileExclusive &&
                (int16_t)BlockingTileCodeThreshold>(int16_t)adjacentTile && (int16_t)adjacent>=0) break;
            ++scanX; ++scanColumn;
            if ((int16_t)scanColumn>EncounterSearchLastColumn) {
                scanColumn=0; scanX=rowStartX; ++scanY;
            }
        }
        uint16_t column=(uint16_t)(halfX+CachedMapOriginColumn);
        if (!(scanX&1) && (CrescentHawkMapPositionX&1)) ++column;
        uint16_t row=(uint16_t)(halfY*MapCacheWidth+CachedMapOriginRowOffset);
        if ((scanY&1) && (CrescentHawkMapPositionY&1)) row+=MapCacheWidth;
        uint16_t actor=record+PartySize;
        if ((int16_t)column<0 || (int16_t)column>=MapCacheWidth ||
            (int16_t)row<0 || (int16_t)row>=MapCacheTileCount) {
            CombatantPackedY[actor]=CombatantPosition_Unused;
            CombatantPackedX[actor]=CombatantPosition_Unused;
            Mechs[record].name[0]=MECH_Destroyed;
            CombatantActive[actor]=FALSE;
        } else {
            uint16_t x=(uint16_t)(CrescentHawkMapPositionX+scanX-EncounterPartyScreenX);
            if (x&PackedPositionLocalCarryBit) {
                if ((int16_t)scanX<EncounterPartyScreenX) x&=PackedPositionWestNormalizeMask;
                else x+=EncounterPackedEastCarry;
            }
            uint16_t y=(uint16_t)(CrescentHawkMapPositionY+scanY-EncounterPartyScreenY);
            if (y&PackedPositionLocalCarryBit) {
                if ((int16_t)scanY<EncounterPartyScreenY) y&=PackedPositionNorthNormalizeMask;
                else y+=EncounterPackedSouthCarry;
            }
            CombatantPackedX[actor]=x; CombatantPackedY[actor]=y;
            CombatantSpriteFrame[actor]=0;
            CombatantActive[actor]=TRUE;
            
        }
        scanColumn+=EncounterMechSpacing; scanX+=EncounterMechSpacing;
        if ((int16_t)scanColumn>EncounterMechLastColumn) {
            scanColumn=0; scanX=rowStartX; ++scanY;
        }
    }

    (void)placedInfantry; /* Original unused count; does not gate placement. */
}

