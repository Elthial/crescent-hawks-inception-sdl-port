#include "game.h"

/* EXE-owned seven WORD offsets3EDB:241E, expanded EXE
 * F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE. */
uint16_t CacheStarPuzzleTargetOffsets[CacheStarPuzzleTargetCount]={140,186,325,341,375,439,443};

/* Original135D:0913..097F. Coordinate indexing deliberately uses Y stride16,
 * unlike doors' stride32. INC/XOR1/DEC BYTE toggles odd->next even and
 * even->previous odd with wrap (FF->00). No star/category gate here. */
void Map_Interactable_Play_Sound(uint16_t worldX,uint16_t worldY)
{
    uint16_t localX=(uint16_t)(worldX+1)&PackedPositionLocalMask;
    uint16_t tileOffset=(uint16_t)((worldY&CacheDoorCoarseCellMask)*CacheStarMapCoarseYByteStride+
        (worldY&CacheDoorEvenCellWithinBlockMask)*CacheDoorEvenCellByteStride+
        (localX&CacheDoorCoarseCellMask)*CacheDoorEvenCellByteStride+((localX>>1)&MapCacheLocalBlockMask));
    uint8_t tile=MapFileTiles[tileOffset];
    tile=(uint8_t)(tile+1); tile^=CacheStarSelectedParityMask; --tile;
    MapFileTiles[tileOffset]=tile;
    Play_Sound_If_Enabled(Sound_MapInteraction);
}

/* Original135D:0980..0AB5. Membership set, NOT a click sequence. Required
 * offsets are checked only for even parity, not star-tile category. Additional
 * selected stars anywhere in first768 tiles reject. Failure deselects all
 * selected star-category tiles without clearing a previously latched WHITE. */
void Cache_StarMap_CorrectPassword(void)
{
    uint16_t correct=TRUE;
    for(uint16_t target=0;target<CacheStarPuzzleTargetCount;++target)
        if(MapFileTiles[CacheStarPuzzleTargetOffsets[target]]&CacheStarSelectedParityMask) correct=FALSE;
    if(correct)
        for(uint16_t tile=0;tile<CacheMapRoomTileBytes;++tile) {
            uint8_t type=MapFileTiles[tile];
            if(type>=CacheStarTileRangeFirst && type<=CacheStarTileRangeLast && !(type&CacheStarSelectedParityMask)) {
                uint16_t required=FALSE;
                for(uint16_t target=0;target<CacheStarPuzzleTargetCount;++target)
                    if(CacheStarPuzzleTargetOffsets[target]==tile) { required=TRUE; break; }
                if(!required) correct=FALSE;
            }
        }
    Draw_Message_Box();
    if(correct) {
        Play_Sound_If_Enabled(Sound_PasswordAccepted);
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Password accepted.\rWHITE code installed.\rYou can trigger the HyperPulse Generator now.");
        WhiteCacheCodeCorrect=TRUE;
    } else {
        Play_Sound_If_Enabled(Sound_PasswordIncorrect);
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Incorrect password.");
        for(uint16_t tile=0;tile<CacheMapRoomTileBytes;++tile) {
            uint8_t type=MapFileTiles[tile];
            if(type>=CacheStarTileRangeFirst && type<=CacheStarTileRangeLast && !(type&CacheStarSelectedParityMask)) --MapFileTiles[tile];
        }
    }
    (void)Keyboard_Get_ASCII_Hex_Input(); MessageBoxOpen=TRUE;
}
