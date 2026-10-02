#include "game.h"

/* Original11B8:137F..1440. Sol: Roll and remember one of four terrain
 * variants. Zero writes nothing. Patch indices are relative to native101D;
 * remembered WORD is246C:0010 (the annotated3092 binding was wrong). */
void Arena_Select_And_Apply_Combat_Map_Patch(void)
{
    ArenaCombatMapPatchVariant=Rand_0x00_to_0xFF()&ArenaPatchVariantMask;
    if(ArenaCombatMapPatchVariant==1) {
        MapFileTiles[ArenaPatchOneStart]=ArenaPatchStartTile;
        for(uint16_t tile=0;tile<ArenaPatchOneLength;++tile) MapFileTiles[ArenaPatchOneInterior+tile]=ArenaPatchHorizontalTile;
        MapFileTiles[ArenaPatchOneEnd]=ArenaPatchEndTile;
    } else if(ArenaCombatMapPatchVariant==2) {
        MapFileTiles[ArenaPatchTwoStart]=ArenaPatchStartTile;
        for(uint16_t tile=0;tile<ArenaPatchTwoFirstLength;++tile) MapFileTiles[ArenaPatchTwoFirstInterior+tile]=ArenaPatchHorizontalTile;
        for(uint16_t tile=0;tile<ArenaPatchTwoSecondLength;++tile) MapFileTiles[ArenaPatchTwoSecondInterior+tile]=ArenaPatchHorizontalTile;
        MapFileTiles[ArenaPatchTwoEnd]=ArenaPatchEndTile;
    } else if(ArenaCombatMapPatchVariant==3) {
        MapFileTiles[ArenaPatchThreeStart]=ArenaPatchSparseStartTile;
        for(uint16_t tile=ArenaPatchThreeFirstInterior;tile<ArenaPatchThreeEnd;tile+=ArenaPatchThreeStride) MapFileTiles[tile]=ArenaPatchVerticalTile;
        MapFileTiles[ArenaPatchThreeEnd]=ArenaPatchEndTile;
    }
}

/* Original11B8:1441..152E. Sol: Replace endpoints and roll interior ground
 * variants; no old tile backup/restore, cache refresh or selector reset. */
void Arena_Remove_And_Randomize_Combat_Map_Patch(void)
{
    if(ArenaCombatMapPatchVariant==1) {
        MapFileTiles[ArenaPatchOneStart]=ArenaGroundFirstTile+3;
        for(uint16_t tile=0;tile<ArenaPatchOneLength;++tile) MapFileTiles[ArenaPatchOneInterior+tile]=(uint8_t)((Rand_0x00_to_0xFF()&ArenaGroundVariantMask)+ArenaGroundFirstTile);
        MapFileTiles[ArenaPatchOneEnd]=ArenaGroundFirstTile+2;
    } else if(ArenaCombatMapPatchVariant==2) {
        MapFileTiles[ArenaPatchTwoStart]=ArenaGroundFirstTile+1;
        for(uint16_t tile=0;tile<ArenaPatchTwoFirstLength;++tile) MapFileTiles[ArenaPatchTwoFirstInterior+tile]=(uint8_t)((Rand_0x00_to_0xFF()&ArenaGroundVariantMask)+ArenaGroundFirstTile);
        for(uint16_t tile=0;tile<ArenaPatchTwoSecondLength;++tile) MapFileTiles[ArenaPatchTwoSecondInterior+tile]=(uint8_t)((Rand_0x00_to_0xFF()&ArenaGroundVariantMask)+ArenaGroundFirstTile);
        MapFileTiles[ArenaPatchTwoEnd]=ArenaGroundFirstTile+2;
    } else if(ArenaCombatMapPatchVariant==3) {
        MapFileTiles[ArenaPatchThreeStart]=ArenaGroundFirstTile;
        for(uint16_t tile=ArenaPatchThreeFirstInterior;tile<ArenaPatchThreeEnd;tile+=ArenaPatchThreeStride) MapFileTiles[tile]=(uint8_t)((Rand_0x00_to_0xFF()&ArenaGroundVariantMask)+ArenaGroundFirstTile);
        MapFileTiles[ArenaPatchThreeEnd]=ArenaGroundFirstTile+1;
    }
}
