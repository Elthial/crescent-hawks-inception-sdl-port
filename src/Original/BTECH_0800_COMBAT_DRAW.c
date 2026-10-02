#include "game.h"


/* Original0800:0E4B..1731, retained EGA adapter2 paths. Every raw ASM
 * block checked. Native C startup DS=SS and child0377 saves/restores DS,
 * so indexed local-array sort accesses refer to these same C locals.
 * Effects first, infantry next, ascending-Y mechs, jail overlay last.
 * No active-word filter: native eligibility starts with X!=FFFF only. */
void Draw_Menu_MultiSelect(void)
{
    Draw_Persistent_Map_Effects();
    uint16_t cameraX=CrescentHawkMapPositionX,cameraY=CrescentHawkMapPositionY;
    EgaMemoryAddress destination={0,MapViewportSegment};
    const uint16_t occlusionMasks[4]={9,3,12,6}; /*DS0372 WORDs*/
    for(uint16_t id=0;id<AllCombatantCount;++id) CombatantVisibleOnScreen[id]=FALSE;
    for(uint16_t side=0;side<=Enemy_All_CombatantId_Range_First;side+=Enemy_All_CombatantId_Range_First)
        for(uint16_t slot=0;slot<PartySize;++slot) {
            uint16_t id=(uint16_t)(side+Friendly_Infantry_Combatant_Range_First+slot);
            uint16_t worldX=CombatantPackedX[id],worldY=CombatantPackedY[id];
            if(worldX==CombatantPosition_Unused) continue;
            int16_t x=(int16_t)(uint16_t)(worldX-cameraX+MapCameraCentreCellX);
            int16_t y=(int16_t)(uint16_t)(worldY-cameraY+MapCameraCentreCellY);
            unsigned outsideX=(worldX&0xFF00)==(cameraX&0xFF00) && (x<MapViewportLeftByte || x>MapViewportLastCellX);
            unsigned outsideY=(worldY&0xFF00)==(cameraY&0xFF00) && (y<0 || y>MapViewportLastCellY);
            if(x<MapProjectionMinimumX || x>MapProjectionMaximumX || y<MapProjectionMinimumY ||
                y>MapProjectionMaximumY || outsideX || outsideY) continue;
            CombatantVisibleOnScreen[id]=TRUE;
            x=(int16_t)((uint16_t)x&PackedPositionLocalMask);
            y=(int16_t)((uint16_t)y&PackedPositionLocalMask);
            int16_t localX=(int16_t)(x-MapViewportLeftByte);
            int16_t halfX=(int16_t)(localX>=0?localX/2:-((-(int32_t)localX+1)/2));
            uint16_t mapIndex=(uint16_t)((y/2)*MapCacheWidth+halfX+CachedMapOriginIndex);
            if((localX&1) && (cameraX&1)) ++mapIndex;
            if((y&1) && (cameraY&1)) mapIndex+=MapCacheWidth;
            unsigned parity=(((y^cameraY)&1)*2)+((localX^cameraX)&1);
            uint16_t mask=occlusionMasks[parity];
            uint8_t tile=CombatMap[mapIndex],overlap=0;
            MapTileUnderCombatant[id]=tile;
            if(!InsideStarLeagueCache && tile<TerrainOcclusionSpecialTileFirst && (tile&mask) &&
                (tile&TerrainOcclusionCategoryMask)<TerrainOcclusionCategoryLimit)
                overlap=(tile&TerrainOcclusionCategoryMask)==TerrainOcclusionTallCategory?
                    InfantryTallTerrainOverlapRows:InfantryTerrainOverlapRows;
            TerrainOverlapRows[id]=overlap;
            uint16_t pixelX=(uint16_t)(x*PixelsPerMapCell),pixelY=(uint16_t)(y*PixelsPerMapCell);
            CombatantScreenPixelX[id]=pixelX; CombatantScreenPixelY[id]=pixelY;
            uint16_t spriteId=(uint16_t)(CombatantSpriteFrame[id]+CombatantSpriteFamilyOffset[id]);
            uint8_t *sprite=CombatSpritePointers[spriteId];
            /* Original inline temporary header mutation, not a new helper. */
            sprite[SpriteHeightMinusOneByte]-=overlap;
            DrawCall_EGA_CharacterPos(destination,sprite,(int16_t)pixelX,(int16_t)pixelY);
            sprite[SpriteHeightMinusOneByte]+=overlap;
        }
    int16_t lastVisible=-1;
    uint16_t visibleX[VisibleMechMaximum],visibleY[VisibleMechMaximum];
    uint8_t visibleOverlap[VisibleMechMaximum],visibleSprite[VisibleMechMaximum];
    for(uint16_t side=0;side<=Enemy_All_CombatantId_Range_First;side+=Enemy_All_CombatantId_Range_First)
        for(uint16_t slot=0;slot<LanceSize;++slot) {
            uint16_t id=(uint16_t)(side+slot),worldX=CombatantPackedX[id],worldY=CombatantPackedY[id];
            if(worldX==CombatantPosition_Unused) continue;
            int16_t x=(int16_t)(uint16_t)(worldX-cameraX+MapCameraCentreCellX);
            int16_t y=(int16_t)(uint16_t)(worldY-cameraY+MapCameraCentreCellY);
            unsigned outsideX=(worldX&0xFF00)==(cameraX&0xFF00) && (x<CombatMechViewportFirstCellX || x>MapViewportLastCellX);
            unsigned outsideY=(worldY&0xFF00)==(cameraY&0xFF00) && (y<0 || y>CombatMechViewportLastCellY);
            CombatMechYParityAdjustment[id]=0; CombatMechPreviousMapRowTile[id]=0;
            if(x<CombatMechProjectionMinimumX || x>MapProjectionMaximumX || y<MapProjectionMinimumY ||
                y>CombatMechProjectionMaximumY || outsideX || outsideY) continue;
            CombatantVisibleOnScreen[id]=TRUE;
            x=(int16_t)((uint16_t)x&PackedPositionLocalMask);
            y=(int16_t)((uint16_t)y&PackedPositionLocalMask);
            uint16_t mapOffset=(uint16_t)((y/2)*MapCacheWidth);
            if((y&1) && (cameraY&1)) mapOffset+=MapCacheWidth;
            uint16_t mask=1;
            if((y^cameraY)&1) { mask=4; CombatMechYParityAdjustment[id]=1; }
            mapOffset+=CombatMechColumnOffset[x];
            if(cameraX&1) mapOffset+=CombatMechOddCameraColumnOffset[x];
            uint16_t mapIndex=(uint16_t)(CachedMapOriginIndex+mapOffset);
            /* Native246C:0795 BYTE view is24 bytes BEFORE CombatMap, not
             * a separate WORD array. Keep alias inside contiguous MapCache. */
            CombatMechPreviousMapRowTile[id]=((uint8_t *)&MapCache)[
                offsetof(MapCacheStorage,tiles)-MapCacheWidth+mapIndex];
            uint8_t tile=CombatMap[mapIndex],overlap=0;
            MapTileUnderCombatant[id]=tile;
            if(!InsideStarLeagueCache && tile<TerrainOcclusionSpecialTileFirst && (tile&mask) &&
                (tile&TerrainOcclusionCategoryMask)<TerrainOcclusionCategoryLimit) {
                overlap=MechTerrainOverlapRows;
                if(((tile&TerrainOcclusionCategoryMask)==TerrainOcclusionTallCategory ||
                    !(tile&TerrainOcclusionCategoryMask)) &&
                    (tile&TerrainOcclusionAllQuadrantsMask)==TerrainOcclusionAllQuadrantsMask)
                    overlap=MechTallTerrainOverlapRows;
            }
            TerrainOverlapRows[id]=overlap;
            uint16_t pixelX=(uint16_t)(x*PixelsPerMapCell),pixelY=(uint16_t)(y*PixelsPerMapCell);
            CombatantScreenPixelX[id]=pixelX; CombatantScreenPixelY[id]=pixelY;
            ++lastVisible;
            visibleX[lastVisible]=pixelX; visibleY[lastVisible]=pixelY; visibleOverlap[lastVisible]=overlap;
            /* Native ADD AL, truncates sprite sum before storing local BYTE. */
            visibleSprite[lastVisible]=(uint8_t)(CombatantSpriteFrame[id]+CombatantSpriteFamilyOffset[id]);
        }
    if(lastVisible!=-1) {
        for(int16_t front=0;front<lastVisible;++front)
            for(int16_t candidate=(int16_t)(front+1);candidate<=lastVisible;++candidate)
                if((int16_t)visibleY[candidate]<(int16_t)visibleY[front]) {
                    uint16_t word=visibleX[candidate]; visibleX[candidate]=visibleX[front]; visibleX[front]=word;
                    word=visibleY[candidate]; visibleY[candidate]=visibleY[front]; visibleY[front]=word;
                    uint8_t byte=visibleSprite[candidate]; visibleSprite[candidate]=visibleSprite[front]; visibleSprite[front]=byte;
                    byte=visibleOverlap[candidate]; visibleOverlap[candidate]=visibleOverlap[front]; visibleOverlap[front]=byte;
                }
        for(int16_t index=0;index<=lastVisible;++index) {
            uint8_t *sprite=CombatSpritePointers[visibleSprite[index]];
            uint8_t overlap=visibleOverlap[index];
            sprite[SpriteHeightMinusOneByte]-=overlap;
            DrawCall_EGA_CharacterPos(destination,sprite,(int16_t)(uint16_t)(visibleX[index]-PixelsPerMapCell),
                (int16_t)(uint16_t)(visibleY[index]-2*PixelsPerMapCell));
            sprite[SpriteHeightMinusOneByte]+=overlap;
        }
    }
    if(DrawJailMissionParkedMechs)
        for(uint16_t slot=0;slot<LanceSize;++slot) {
            int16_t x=(int16_t)(uint16_t)(JailParkedMechFirstX+slot*JailParkedMechSpacing-cameraX+MapCameraCentreCellX);
            int16_t y=(int16_t)(uint16_t)(JailParkedMechY-cameraY+MapCameraCentreCellY);
            /* Jail overlay ALWAYS applies narrow bounds, no page test. */
            if(x<MapProjectionMinimumX || x>MapProjectionMaximumX || y<MapProjectionMinimumY || y>MapProjectionMaximumY ||
                x<MapViewportLeftByte || x>MapViewportLastCellX || y<0 || y>MapViewportLastCellY) continue;
            DrawCall_EGA_CharacterPos(destination,CombatSpritePointers[MECH_Sprite_COMMANDO],
                (int16_t)(((uint16_t)x&PackedPositionLocalMask)*PixelsPerMapCell),
                (int16_t)(((uint16_t)y&PackedPositionLocalMask)*PixelsPerMapCell));
        }
}
