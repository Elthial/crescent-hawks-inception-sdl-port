/* Sol: Original decode/WORD readers; pending disk/load calls are test-only
 * adapters. Synthetic encoded bytes, never shipped level/script assets. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, diskCalls, loads;
static uint16_t disks[2], expectedId;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"BLD check %u failed\n",checks); exit(1); }
}
void Select_Game_Disk_And_Drive(uint16_t disk)
{
    check(loads == 0 && diskCalls < 2); disks[diskCalls++] = disk;
}
uint16_t Load_File_To_Memory(const uint8_t *filename, uint8_t *memory)
{
    check(memory == BTStatsOrBldMemory && BldFileIndex == expectedId && diskCalls >= 1);
    check(filename == BldFileNameById[expectedId & UINT16_C(0x3FFF)]);
    check(BTStatsAssetLoaded == TRUE);
    /* Simulate a payload shorter than the decoder's fixed span. */
    memory[0] = 0; memory[1] = 255; ++loads; return TRUE;
}
int main(void)
{
    for (uint32_t value = 0; value <= UINT16_MAX; ++value) {
        uint8_t bytes[2] = {(uint8_t)value,(uint8_t)(value >> 8)};
        check(Read_Bld_Target(bytes) == value);
        check(Read_Bld_Immediate_Word(bytes) == value);
        check(bytes[0] == (uint8_t)value && bytes[1] == (uint8_t)(value >> 8));
    }
    const char *names[BldFileCount] = {"TRAINING.BLD","CITADEL.BLD","COMSTAR.BLD","WEAPON.BLD",
        "ARMOR.BLD","REPAIR.BLD","BARRACKS.BLD","LOUNGE.BLD","GARAGE.BLD","HOSPITAL.BLD",
        "ARENA.BLD","PARTY.BLD","CLOTHES.BLD","JAIL.BLD","MAYOR.BLD","WEAPON2.BLD",
        "THEATER.BLD","FROB.BLD","VIEWDISK.BLD","BARRACK2.BLD","ENTRANCE.BLD","HUT.BLD",
        "ENDMECH.BLD","INSTRUCT.BLD","FINDIT.BLD","WINSCENE.BLD"};
    for (unsigned id = 0; id < BldFileCount; ++id) check(!strcmp((char *)BldFileNameById[id],names[id]));
    for (unsigned alias = 0; alias < 4; ++alias) {
        for (unsigned id = 0; id < BldFileCount; ++id) {
            expectedId = (uint16_t)(alias * 0x4000 + id);
            for (unsigned i = 0; i < BldFixedDecodeSpan; ++i) BTStatsOrBldMemory[i] = (uint8_t)i;
            diskCalls = loads = 0; BTStatsAssetLoaded = TRUE;
            Load_And_Decode_Indexed_BLD(expectedId);
            check(loads == 1 && disks[0] == 1);
            unsigned usesDiskTwo = expectedId < 2 || expectedId >= 17;
            check(diskCalls == 1 + usesDiskTwo);
            if (usesDiskTwo) check(disks[1] == 2);
            check(BldFileIndex == expectedId && BTStatsAssetLoaded == FALSE);
            for (unsigned i = 0; i < BldFixedDecodeSpan; ++i) {
                unsigned encoded = i == 0 ? 0 : i == 1 ? 255 : i & 255;
                check(BTStatsOrBldMemory[i] == (((encoded + 41) & 255) ^ 233));
            }
        }
    }
    printf("Original BLD decode/operands: %u checks passed\n",checks);
    return 0;
}
