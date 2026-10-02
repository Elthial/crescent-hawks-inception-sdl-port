#include "game.h"

/* Sol: Complete original0800:1C12..218E, expanded ASM checked.
 * Arguments are signed attempted-step deltas, NOT absolute coordinates.
 * Calls may change shared map/threshold state; retain original call ordering.
 * Projection-table indices/native map addresses must be represented/valid.
 * No cache clamping, footprint-row guard or safe substitute interaction. */
enum {
    InteractionLocalSubcellMask=15,InteractionBoundaryNorth=8,
    InteractionBoundarySouth=4,InteractionBoundaryWest=2,InteractionBoundaryEast=1,
    InteractionDeepWater=15,InteractionProbeScreenX=26,InteractionProbeScreenY=12,
    ExteriorBuildingEntryCount=12,InteractionTerrainOffset=0x07AD,
    CacheLadderTile=0x94,CacheStarFirstTile=0x97,CacheStarLastTile=0xF0,
    CacheSubmitLeftTile=0x8C,CacheSubmitRightTile=0x8D,
    CacheKeyPanelLeftTile=0x7E,CacheKeyPanelMiddleTile=0x7F,CacheKeyPanelRightTile=0x80,
    CachePassageTile=0xF5,CachePassageFirstX=0x0C01,CachePassageLastX=0x0C04,
    CachePassageY=0xC054,CacheTerminalLeftTile=0xB6,CacheTerminalRightTile=0xB7,
    CachePhoenixHawkFirstTile=0xF6,CacheEquipmentTile=0x83,
    CacheEquipmentFirstTile=0xA5,CacheEquipmentLastTile=0xA7,
    CacheTransmitterTile=0x4D,CachePowerFirstTile=0x3A,CachePowerSecondTile=0x3D,
    CacheMapRoomEntryTile=0x28
};
/* Original EXE-owned3EDB:045C..04AF, word probes/rows then BYTE neighbours. */
static const int16_t mechProbeX[LanceSize]={0,0,3,-3};
static const int16_t infantryProbeX[PartySize]={0,0,0,1,-1,1,-1,1};
static const int16_t mechProbeY[LanceSize]={-2,4,1,1};
static const int16_t infantryProbeY[PartySize]={0,-1,1,0,0,-1,1,1};
static const uint16_t cacheRowOffsets[10]={0,24,48,72,96,120,144,168,192,216};
static const uint8_t neighbourByBoundary[16]={0,5,3,0,7,8,6,0,1,2,0,0,0,0,0,0};

uint16_t Map_Interactables_Building_Or_Items(int16_t deltaX,int16_t deltaY)
{
    uint16_t handled=FALSE;
    uint16_t localX=CrescentHawkMapPositionX&InteractionLocalSubcellMask;
    uint16_t localY=CrescentHawkMapPositionY&InteractionLocalSubcellMask;
    uint16_t boundary=0;
    if (deltaY<0 && !localY) boundary=InteractionBoundaryNorth;
    if (deltaY>0 && localY==InteractionLocalSubcellMask) boundary|=InteractionBoundarySouth;
    if (deltaX<0 && !localX) boundary|=InteractionBoundaryWest;
    if (deltaX>0 && localX==InteractionLocalSubcellMask) boundary|=InteractionBoundaryEast;
    if (boundary && LocalTerrainFlags[neighbourByBoundary[boundary]]==InteractionDeepWater) {
        MessageBoxOpen=TRUE;
        Draw_Message_Box();
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"The water is too deep that way.");
        handled=TRUE;
    }
    if (!handled) {
        for (uint16_t slot=0;slot<PartySize;++slot) {
            uint16_t actor=slot+Friendly_Infantry_Combatant_Range_First;
            if (!CombatantActive[actor]) continue;
            uint16_t projectedX=(uint16_t)(infantryProbeX[slot]+deltaX+InteractionProbeScreenX);
            uint16_t projectedY=(uint16_t)(infantryProbeY[slot]+deltaY+InteractionProbeScreenY);
            int16_t relativeX=(int16_t)(uint16_t)(projectedX-MapViewportLeftByte);
            int16_t halfX=(int16_t)(relativeX>=0?relativeX/2:-((-(int32_t)relativeX+1)/2));
            uint16_t offset=(uint16_t)(cacheRowOffsets[(projectedY&0xFFFE)/2]+halfX);
            if (!(projectedX&1) && (CrescentHawkMapPositionX&1)) ++offset;
            if ((projectedY&1) && (CrescentHawkMapPositionY&1)) offset+=MapCacheWidth;
            uint16_t address=(uint16_t)(InteractionTerrainOffset+CachedMapOriginIndex+offset);
            uint16_t tile=MapRuntime.bytes[address-MapRuntimeFirstOffset];
            if (!InsideStarLeagueCache && !MessageBoxOpen) {
                for (uint16_t building=0;building<ExteriorBuildingEntryCount;++building) {
                    if ((uint16_t)(CombatantPackedX[actor]+deltaX)!=MapInteractablePositionX[building] ||
                        (uint16_t)(CombatantPackedY[actor]+deltaY)!=MapInteractablePositionY[building]) continue;
                    MessageBoxOpen=TRUE; Draw_Message_Box();
                    Display_Text_From_Memory((uint8_t *)"Will you enter the\r");
                    Display_Text_From_Memory(MapBuildingNames[building]);
                    Display_Text_From_Memory((uint8_t *)"?");
                    if (Prompt_Yes_No(TRUE)) Interact_with_BLD(building);
                    break;
                }
            }
            if ((int16_t)tile<(int16_t)BlockingTileCodeThreshold) continue;
            if (InsideStarLeagueCache) {
                uint16_t worldX=(uint16_t)(CrescentHawkMapPositionX+deltaX+infantryProbeX[slot]);
                uint16_t worldY=(uint16_t)(CrescentHawkMapPositionY+deltaY+infantryProbeY[slot]);
                if (CacheMapRoomLoaded) {
                    if (tile==CacheLadderTile) Draw_STARLEAG_ICN_Scene();
                    if (tile>=CacheStarFirstTile && tile<=CacheStarLastTile)
                        Map_Interactable_Play_Sound(worldX,worldY);
                    if (tile==CacheSubmitLeftTile || tile==CacheSubmitRightTile) Cache_StarMap_CorrectPassword();
                } else {
                    if (tile==CacheKeyPanelLeftTile) StarLeague_Key_Codes(worldX,worldY,CacheDoorLookupByPosition);
                    if (tile==CacheKeyPanelMiddleTile) StarLeague_Key_Codes((uint16_t)(worldX-2),worldY,CacheDoorLookupByPosition);
                    if (tile==CacheKeyPanelRightTile) StarLeague_Key_Codes((uint16_t)(worldX-4),worldY,CacheDoorLookupByPosition);
                    if (tile==CachePassageTile && worldX>=CachePassageFirstX &&
                        worldX<=CachePassageLastX && worldY==CachePassageY) StarLeague_Secret_Passageway_Discovered();
                    if (tile==CacheTerminalLeftTile || tile==CacheTerminalRightTile) StarLeague_Security_Terminal(worldX,worldY);
                    if (tile>=CachePhoenixHawkFirstTile) StarLeague_Cache_PhoenixHawk();
                    if (tile==CacheEquipmentTile || (tile>=CacheEquipmentFirstTile && tile<=CacheEquipmentLastTile))
                        Display_StarLeague_Cache_Dialog_Window();
                    if (tile==CacheTransmitterTile) HPGTransmitter(worldX,worldY);
                    if (tile==CachePowerFirstTile || tile==CachePowerSecondTile)
                        StarLeague_HyperPulse_Power_Dialog_Window(worldX,worldY);
                    if (tile==CacheMapRoomEntryTile) StarLeague_Map_Room(worldX,worldY);
                }
            }
            handled=TRUE; break;
        }
        if (!handled) {
            for (uint16_t slot=0;slot<LanceSize;++slot) {
                if (!CombatantActive[slot]) continue;
                uint16_t projectedX=(uint16_t)(mechProbeX[slot]+deltaX+InteractionProbeScreenX);
                uint16_t projectedY=(uint16_t)(mechProbeY[slot]+deltaY+InteractionProbeScreenY);
                int16_t relativeX=(int16_t)(uint16_t)(projectedX-MapViewportLeftByte);
                int16_t halfX=(int16_t)(relativeX>=0?relativeX/2:-((-(int32_t)relativeX+1)/2));
                uint16_t offset=(uint16_t)(cacheRowOffsets[(projectedY&0xFFFE)/2]+halfX);
                if (!(projectedX&1) && (CrescentHawkMapPositionX&1)) ++offset;
                if ((projectedY&1) && (CrescentHawkMapPositionY&1)) offset+=MapCacheWidth;
                uint16_t address=(uint16_t)(InteractionTerrainOffset+CachedMapOriginIndex+offset);
                uint16_t tile=MapRuntime.bytes[address-MapRuntimeFirstOffset];
                if ((int16_t)tile>=(int16_t)BlockingTileCodeThreshold) handled=TRUE;
                else {
                    offset=(uint16_t)(offset+(((projectedX^CrescentHawkMapPositionX)&1)?-1:1));
                    address=(uint16_t)(InteractionTerrainOffset+CachedMapOriginIndex+offset);
                    tile=MapRuntime.bytes[address-MapRuntimeFirstOffset];
                    /* Native1FE7 uses unsigned JC, unlike both primary tests. */
                    if (tile>=BlockingTileCodeThreshold) handled=TRUE;
                }
                /* Prompt still runs when blocked, and has no Cache-interior gate. */
                if (!MessageBoxOpen) {
                    for (uint16_t building=0;building<ExteriorBuildingEntryCount;++building) {
                        uint16_t y=(uint16_t)(CombatantPackedY[slot]+deltaY);
                        if (y!=MapInteractablePositionY[building]) continue;
                        uint16_t x=(uint16_t)(CombatantPackedX[slot]+deltaX);
                        uint16_t doorway=MapInteractablePositionX[building];
                        if (doorway!=x && doorway!=(uint16_t)(x-1) && doorway!=(uint16_t)(x+1)) continue;
                        MessageBoxOpen=TRUE; Draw_Message_Box();
                        Display_Text_From_Memory((uint8_t *)"Will you enter the\r");
                        Display_Text_From_Memory(MapBuildingNames[building]);
                        Display_Text_From_Memory((uint8_t *)"?");
                        if (Prompt_Yes_No(TRUE)) Interact_with_BLD(building);
                        break;
                    }
                }
                if (handled) break;
            }
        }
    }
    return handled;
}
