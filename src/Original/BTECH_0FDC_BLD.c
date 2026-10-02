#include "game.h"

/* Sol: Original EXE-owned FAR filename table3EDB:4EC2 resolved to flat
 * pointers. File CONTENTS remain required external copyrighted assets.
 * Source: expanded EXE SHA256
 * F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE. */
uint8_t *BldFileNameById[BldFileCount] = {
    (uint8_t *)"TRAINING.BLD", (uint8_t *)"CITADEL.BLD", (uint8_t *)"COMSTAR.BLD",
    (uint8_t *)"WEAPON.BLD", (uint8_t *)"ARMOR.BLD", (uint8_t *)"REPAIR.BLD",
    (uint8_t *)"BARRACKS.BLD", (uint8_t *)"LOUNGE.BLD", (uint8_t *)"GARAGE.BLD",
    (uint8_t *)"HOSPITAL.BLD", (uint8_t *)"ARENA.BLD", (uint8_t *)"PARTY.BLD",
    (uint8_t *)"CLOTHES.BLD", (uint8_t *)"JAIL.BLD", (uint8_t *)"MAYOR.BLD",
    (uint8_t *)"WEAPON2.BLD", (uint8_t *)"THEATER.BLD", (uint8_t *)"FROB.BLD",
    (uint8_t *)"VIEWDISK.BLD", (uint8_t *)"BARRACK2.BLD", (uint8_t *)"ENTRANCE.BLD",
    (uint8_t *)"HUT.BLD", (uint8_t *)"ENDMECH.BLD", (uint8_t *)"INSTRUCT.BLD",
    (uint8_t *)"FINDIT.BLD", (uint8_t *)"WINSCENE.BLD"
};

/* 0FDC:05F7. Sol: Read an absolute payload branch target without advancing
 * the source pointer. Native BYTEs are zero-extended into a little-endian WORD. */
uint16_t Read_Bld_Target(uint8_t *targetBytes)
{
    uint16_t lowByte = targetBytes[0];
    uint16_t highByte = targetBytes[1];
    return (uint16_t)(lowByte | (highByte << NativeByteBits));
}

/* 0FDC:19F6. Sol: Read an immediate WORD, without advancing the source.
 * Native temporary CBW is undone when its low BYTEs are recombined. */
uint16_t Read_Bld_Immediate_Word(uint8_t *wordBytes)
{
    uint16_t lowByte = wordBytes[0];
    uint16_t highByte = wordBytes[1];
    return (uint16_t)(lowByte | (highByte << NativeByteBits));
}

/* 0FDC:1D30. Sol: Select the asset disk, load the indexed original BLD,
 * decode the WHOLE shared buffer, then invalidate its alternate BTSTATS use.
 * No short-payload guard, cache substitute or new format algorithm. */
void Load_And_Decode_Indexed_BLD(uint16_t bldFileId)
{
    Select_Game_Disk_And_Drive(1);
    if (bldFileId < BldFirstDiskOneId || bldFileId >= BldFirstLateDiskTwoId)
        Select_Game_Disk_And_Drive(2);
    BldFileIndex = bldFileId;
    /* Sol: Original SHL/SHL wraps the FAR table byte offset to a WORD.
     * Thus IDs4000/8000/C000 alias entry0. Require a valid resulting entry;
     * arbitrary reads beyond the original table are not host-safe contracts. */
    uint16_t filenameTableByteOffset = (uint16_t)(bldFileId * NativeFarPointerBytes);
    Load_File_To_Memory(BldFileNameById[filenameTableByteOffset / NativeFarPointerBytes],
        BTStatsOrBldMemory);
    for (uint16_t bufferOffset = 0; bufferOffset < BldFixedDecodeSpan; ++bufferOffset)
        BTStatsOrBldMemory[bufferOffset] = (uint8_t)(
            (uint8_t)(BTStatsOrBldMemory[bufferOffset] + BldDecodeAddition) ^ BldDecodeXor);
    BTStatsAssetLoaded = FALSE;
}
