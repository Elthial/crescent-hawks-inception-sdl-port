/* Sol: Fixed instruction-trace witnesses, not gameplay/emulator certification. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

static void check(int condition)
{
    if (!condition) {
        fputs("Original RNG/dice witness failed\n", stderr);
        exit(1);
    }
}

int main(void)
{
    static const uint8_t trace[][4] = {
        {181,146,39,32}, {135,201,78,65}, {248,100,156,130},
        {139,178,57,4}, {171,217,114,9}, {136,108,228,18},
        {126,182,200,36}, {75,219,144,73}, {205,237,32,147},
        {183,246,65,38}, {249,123,130,77}, {185,189,4,155},
        {215,222,9,54}, {125,111,18,109}, {147,183,36,219},
        {146,219,73,183}
    };
    check(RandomByteLow == 4 && RandomByteMiddle == 3 && RandomByteHigh == 2);
    RandomByteLow = 0x25;
    RandomByteMiddle = 0x13;
    RandomByteHigh = 0x90;
    for (size_t i = 0; i < sizeof trace / sizeof trace[0]; ++i) {
        check(Rand_0x00_to_0xFF() == trace[i][0]);
        check(RandomByteLow == trace[i][1]);
        check(RandomByteMiddle == trace[i][2]);
        check(RandomByteHigh == trace[i][3]);
    }
    RandomByteLow = 0x25;
    RandomByteMiddle = 0x13;
    RandomByteHigh = 0x90;
    check(RollD6() == 6); /* First candidate 5: one RNG read. */
    check(RollD6() == 1); /* Candidate 7 rejected; next candidate 0 accepted. */
    check(RandomByteLow == 100 && RandomByteMiddle == 156 && RandomByteHigh == 130);
    check(Roll2D6() == 8); /* Next two candidates 3,3: 4 + 4. */
    check(RandomByteLow == 217 && RandomByteMiddle == 114 && RandomByteHigh == 9);
    puts("Original RNG/dice witnesses passed");
    return 0;
}
