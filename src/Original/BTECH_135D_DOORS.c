#include "game.h"
#include "dos.h"

/* EXE-owned tables246E..24B5, expanded EXE
 * F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE.
 * Requirements store ONE-based terminal numbers; native CBW then DEC.
 * Entrance's zero requirements are never used to index the consumed table. */
uint8_t CacheDoorAnimationTiles[CacheDoorFrameCount*CacheDoorFrameTileCount]={122,123,124,125,118,119,120,121,116,0,1,117};
int8_t CacheDoorPositionX[CacheDoorCount]={48,80,92,38,34,38,88,58,112,114,98,4};
int8_t CacheDoorPositionY[CacheDoorCount]={122,122,112,84,76,68,72,64,38,20,38,124};
int8_t CacheDoorRequiredCode[CacheCodeColourCount][CacheDoorCount]={
    {15,1,2,17,29,20,13,30,25,8,28,0},
    {14,3,7,19,12,27,31,23,33,9,24,0},
    {11,5,18,26,6,22,4,32,10,21,16,0}
};

/* Original135D:0AB6..0D48 complete .dis/EXE tail checked. Any negative
 * argument looks up coordinates; only EXACT -1 animates/plays sound/waits.
 * Nonnegative replays its ID without validating current colour selections.
 * Replay still consumes requirements. Caller supplies native-valid replay ID.
 * Door11 writes D35A, although cache setup replays only stored doors0..10. */
void StarLeague_Key_Codes(uint16_t worldX,uint16_t worldY,int16_t replayDoorId)
{
    uint16_t localX=(uint16_t)(worldX+1)&PackedPositionLocalMask;
    uint16_t localY=worldY&CacheLocalEvenCoordinateMask;
    uint16_t canOpen=FALSE;
    int16_t doorId=0,requiredCode[CacheCodeColourCount];
    if(replayDoorId>=0) {
        doorId=replayDoorId;
        localX=(uint16_t)(int16_t)CacheDoorPositionX[doorId];
        localY=(uint16_t)(int16_t)CacheDoorPositionY[doorId];
        canOpen=TRUE;
    } else {
        for(uint16_t candidate=0;candidate<CacheDoorCount;++candidate) {
            if((int16_t)localY!=CacheDoorPositionY[candidate] ||
                (localX&CacheLocalEvenCoordinateMask)!=(uint8_t)CacheDoorPositionX[candidate]) continue;
            doorId=(int16_t)candidate; canOpen=TRUE;
            for(uint16_t colour=0;colour<CacheCodeColourCount;++colour)
                requiredCode[colour]=(int16_t)(CacheDoorRequiredCode[colour][doorId]-1);
            if(doorId!=CacheDoorEntrance &&
                ((int8_t)SelectedCacheCodeByColour[0]!=requiredCode[0] ||
                 (int8_t)SelectedCacheCodeByColour[1]!=requiredCode[1] ||
                 (int8_t)SelectedCacheCodeByColour[2]!=requiredCode[2])) {
                Draw_Message_Box();
                if((int8_t)SelectedCacheCodeByColour[0]!=requiredCode[0]) Display_Text_From_Memory((uint8_t *)"Incorrect RED code.\r");
                if((int8_t)SelectedCacheCodeByColour[1]!=requiredCode[1]) Display_Text_From_Memory((uint8_t *)"Incorrect BLUE code.\r");
                if((int8_t)SelectedCacheCodeByColour[2]!=requiredCode[2]) Display_Text_From_Memory((uint8_t *)"Incorrect YELLOW code.");
                canOpen=FALSE; Wait_For_50Hz_Then_Check_Input(); (void)Keyboard_Get_ASCII_Hex_Input(); MessageBoxOpen=TRUE;
            }
            break; /* First coordinate match only, even when rejected. */
        }
    }
    if(!canOpen) return;
    if(replayDoorId>=0)
        for(uint16_t colour=0;colour<CacheCodeColourCount;++colour)
            requiredCode[colour]=(int16_t)(CacheDoorRequiredCode[colour][doorId]-1);
    if(doorId!=CacheDoorEntrance) {
        for(uint16_t colour=0;colour<CacheCodeColourCount;++colour) CacheSecurityCodeUsed[requiredCode[colour]]=TRUE;
    } else {
        for(uint16_t colour=0;colour<CacheCodeColourCount;++colour) SelectedCacheCodeByColour[colour]=0xFF;
    }
    CacheDoorOpened[doorId]=TRUE;
    if(replayDoorId==CacheDoorLookupByPosition) Play_Sound_If_Enabled(Sound_CacheDoor);
    uint16_t startColumn=(localX>>1)&MapCacheLocalBlockMask;
    uint16_t tileOffset=(uint16_t)((localY&CacheDoorCoarseCellMask)*CacheDoorCoarseYByteStride+
        (localY&CacheDoorEvenCellWithinBlockMask)*CacheDoorEvenCellByteStride+
        (localX&CacheDoorCoarseCellMask)*CacheDoorEvenCellByteStride+startColumn);
    /* Packed Y uses32-byte region stride here, not star-map0913's16.
     * Four horizontal tile writes cross8-column blocks with extra56 gap. */
    for(uint16_t frame=0;frame<CacheDoorFrameCount;++frame) {
        uint16_t column=startColumn,writeOffset=0;
        if(replayDoorId!=CacheDoorLookupByPosition) frame=CacheDoorFrameCount-1;
        for(uint16_t tile=0;tile<CacheDoorFrameTileCount;++tile) {
            MapFileTiles[tileOffset+writeOffset]=CacheDoorAnimationTiles[frame*CacheDoorFrameTileCount+tile];
            ++column; ++writeOffset;
            if(column==MapBlockWidth) writeOffset=(uint16_t)(writeOffset+CacheDoorNextBlockGap);
        }
        PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
        Copy_Data_To_GraphicsMemory(); Draw_Infantry_And_Mechs(); EGA_DrawBox_Wrapper();
        if(replayDoorId==CacheDoorLookupByPosition) Wait_For_N_Vertical_Retraces(CacheDoorFrameRetraces);
    }
}
