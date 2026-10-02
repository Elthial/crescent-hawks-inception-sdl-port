#include "game.h"

/* Sol: Original 183B:273D. Legacy name retained: returns supporting structure
 * BYTE, NOT a destroyed-component Boolean. The signed comparisons partition
 * critical record offsets33..55; their anatomical interpretation remains A-005.
 * Native structure offsets1C/1D/21/22/1E/23/20/1F become array ordinals below.
 * The special C8 upgrade-base bypass returns1 exactly as in the binary. */
uint16_t Check_If_CriticalSlot_Destroyed(uint16_t mechId, uint16_t componentOffset)
{
    int32_t signedOffset = componentOffset <= INT16_MAX
        ? (int32_t)componentOffset : (int32_t)componentOffset - ((int32_t)UINT16_MAX + 1);
    uint16_t structureIndex = 0;
    if (signedOffset >= 0x3A) structureIndex = 1;
    if (signedOffset >= 0x41) structureIndex = 5;
    if (signedOffset >= 0x48) structureIndex = 6;
    if (signedOffset >= 0x4F) structureIndex = 2;
    if (signedOffset >= 0x51) structureIndex = 7;
    if (signedOffset >= 0x53) structureIndex = 4;
    if (signedOffset >= 0x55) structureIndex = 3;
    if (Mechs[mechId].upgradePackageBase == 0xC8) return 1;
    return Mechs[mechId].currentStructure[structureIndex];
}
