#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void verify(int ok,unsigned line) {
    if (!ok) { fprintf(stderr,"Combat packed restore mismatch line%u\n",line); exit(1); }
}
#define check(x) verify(!!(x),__LINE__)
static uint16_t packed(unsigned index,unsigned axisY) {
    return (uint16_t)(((index/128)<<(axisY?12:8))+(index%128));
}
int main(void) {
    /* Original combat direction order N,NE,E,SE,S,SW,W,NW. The stationary
     * entry is unused by the movement branch; native table contains0. */
    const uint8_t directionGrid[9]={7,0,1,6,0,2,5,4,3};
    for(int deltaY=-1;deltaY<=1;++deltaY)for(int deltaX=-1;deltaX<=1;++deltaX) {
        int tableIndex=deltaY*MovementDeltaDirectionRowStride+deltaX+MovementDeltaDirectionBias;
        check(MovementStepDirectionByDelta[tableIndex]==directionGrid[(deltaY+1)*3+deltaX+1]);
    }
    MapCacheStorage originalCache;
    memset(&MapCache,0xA5,sizeof MapCache); originalCache=MapCache;
    CachedMapOriginIndex=0x1234; CachedMapOriginColumn=0x4321; CachedMapOriginRowOffset=0x5678;
    for (unsigned start=0;start<2048;++start) for (unsigned region=0;region<16;++region) {
        unsigned target=region*128+((start+region*7)%128);
        CrescentHawkMapPositionX=packed(start,0); CrescentHawkMapPositionY=packed(2047-start,1);
        Combat_Move_Position(packed(target,0),packed(2047-target,1));
        check(CrescentHawkMapPositionX==packed(target,0) && CrescentHawkMapPositionY==packed(2047-target,1));
    }
    for (unsigned region=0;region<15;++region) for (unsigned local=128;local<256;++local) {
        uint16_t targetX=(uint16_t)((region<<8)+local);
        uint16_t targetY=(uint16_t)((region<<12)+local);
        CrescentHawkMapPositionX=0; CrescentHawkMapPositionY=0;
        Combat_Move_Position(targetX,targetY);
        check(CrescentHawkMapPositionX==(region+1)*256 && CrescentHawkMapPositionY==(region+1)*4096);
        /* Descend below the noncanonical target first, then ascend and skip it. */
        CrescentHawkMapPositionX=(uint16_t)((region+1)*256+1);
        CrescentHawkMapPositionY=(uint16_t)((region+1)*4096+1);
        Combat_Move_Position(targetX,targetY);
        check(CrescentHawkMapPositionX==(region+1)*256 && CrescentHawkMapPositionY==(region+1)*4096);
    }
    check(!memcmp(&MapCache,&originalCache,sizeof MapCache));
    check(CachedMapOriginIndex==0x1234 && CachedMapOriginColumn==0x4321 && CachedMapOriginRowOffset==0x5678);
    puts("Original arithmetic combat restore: canonical coordinates, skipped targets and untouched cache passed.");
    return 0;
}
