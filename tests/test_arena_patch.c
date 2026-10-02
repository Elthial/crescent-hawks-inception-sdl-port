#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned calls,seed;
uint8_t Rand_0x00_to_0xFF(void) { return (uint8_t)(seed+calls++); }
static void check(int ok) { if(!ok) { fputs("Arena terrain mismatch\n",stderr); exit(1); } }
int main(void)
{
    /*Literal native offsets provide an independent write-set/order oracle.*/
    const unsigned offsets[3][12]={{0xC72,0xC73,0xC74,0xC75,0xC76,0xC77,0xCB0},
        {0xA5B,0xA5C,0xA5D,0xA5E,0xA5F,0xA98,0xA99,0xA9A,0xA9B,0xA9C,0xA9D,0xA9E},
        {0xABC,0xC84,0xC8C,0xC94,0xC9C,0xCA4,0xCAC,0xCB4,0xCBC}};
    const unsigned counts[3]={7,12,9};
    uint8_t expected[4096];
    for(seed=0;seed<256;++seed) {
        calls=0; memset(MapFileTiles,0xA5,4096); memset(expected,0xA5,4096);
        Arena_Select_And_Apply_Combat_Map_Patch(); unsigned variant=seed&3;
        check(calls==1 && ArenaCombatMapPatchVariant==variant);
        if(variant) {
            unsigned index=variant-1,count=counts[index];
            for(unsigned cell=0;cell<count;++cell) expected[offsets[index][cell]]=(uint8_t)(cell==0?(variant==3?0x71:0x6F):cell==count-1?0x70:variant==3?0x6B:0x6A);
        }
        check(!memcmp(expected,MapFileTiles,4096));
        calls=0; Arena_Remove_And_Randomize_Combat_Map_Patch();
        if(variant) {
            unsigned index=variant-1,count=counts[index];
            for(unsigned cell=0;cell<count;++cell) expected[offsets[index][cell]]=(uint8_t)(cell==0?(variant==1?0x43:variant==2?0x41:0x40):cell==count-1?(variant==3?0x41:0x42):0x40+((seed+cell-1)&3));
            check(calls==count-2);
        } else check(calls==0);
        check(!memcmp(expected,MapFileTiles,4096) && ArenaCombatMapPatchVariant==variant);
    }
    memset(MapFileTiles,0xA5,4096); memset(expected,0xA5,4096);
    for(unsigned variant=0;variant<65536;++variant) if(variant<1 || variant>3) {
        ArenaCombatMapPatchVariant=(uint16_t)variant; calls=0;
        Arena_Remove_And_Randomize_Combat_Map_Patch(); check(calls==0 && !memcmp(expected,MapFileTiles,4096));
    }
    return 0;
}
