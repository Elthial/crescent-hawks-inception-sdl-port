/* Sol: Real terrain path, direction and origin routines. The cache producer
 * is a test-only adapter: this suite does not certify map construction. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks, cacheCalls;
static uint16_t requestedX,requestedY;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Terrain path check %u failed\n",checks); exit(1); }
}
void PosXY_OffsetGrid(uint16_t x,uint16_t y)
{
    check(x==requestedX && y==requestedY); ++cacheCalls;
}
static void prepare(uint16_t x,uint16_t y)
{
    requestedX=CrescentHawkMapPositionX=x; requestedY=CrescentHawkMapPositionY=y;
    memset(CombatMap,0,sizeof CombatMap);
    BlockingTileCodeThreshold=0x55; ArenaRentalMechMode=0; cacheCalls=0;
}
static uint16_t path(uint16_t x,uint16_t y)
{
    uint16_t result=Combat_Check_Terrain_Path(0xFEDC,12,x,y);
    check(cacheCalls==1 && CrescentHawkMapPositionX==requestedX && CrescentHawkMapPositionY==requestedY);
    return result;
}
int main(void)
{
    check(BlockingTileCodeThreshold==0x55);
    /* Independent ordinary-coordinate sector oracle, using signed arithmetic
     * without native BYTE/WORD overflow (the chosen local window fits). */
    for (int dx=-40;dx<=40;++dx) for (int dy=-40;dy<=40;++dy) {
        unsigned flags=0;
        int ax=dx<0?-dx:dx, ay=dy<0?-dy:dy;
        if (-dy*2>=ax) flags|=8;
        if (dy*2>=ax) flags|=4;
        if (dx*2>=ay) flags|=2;
        if (-dx*2>=ay) flags|=1;
        const int16_t directions[16]={-1,6,2,-1,4,5,3,-1,0,7,1,-1,-1,-1,-1,-1};
        int16_t result=Get_Target_Compass_Direction(0x0240,0x3040,
            (uint16_t)(0x0240+dx),(uint16_t)(0x3040+dy));
        check(result==directions[flags]);
        check(CompassCompareX0==0x0240 && CompassCompareY0==0x3040 &&
            CompassCompareX1==(uint16_t)(0x0240+dx) && CompassCompareY1==(uint16_t)(0x3040+dy));
    }
    check(Get_Target_Compass_Direction(0x027F,0x300A,0x0300,0x300A)==2);
    check(Get_Target_Compass_Direction(0x0300,0x300A,0x027F,0x300A)==6);
    check(Get_Target_Compass_Direction(0x020A,0x3000,0x020A,0x207F)==0);
    /* Preserve the native WORD OR80: this is not re-sign-extended as a BYTE. */
    check(Get_Target_Compass_Direction(0x020A,0x300A,0x020A,0x400A)==0);
    for (unsigned x=0;x<256;++x) for (unsigned y=0;y<256;++y) {
        CrescentHawkMapPositionX=(uint16_t)(0x0A00+x);
        CrescentHawkMapPositionY=(uint16_t)(0xB000+y);
        Update_Cached_Map_Origin();
        unsigned row=(y/2)%8+2, column=(x/2)%8+2;
        check(CachedMapOriginColumn==column && CachedMapOriginRowOffset==row*24 &&
            CachedMapOriginIndex==row*24+column);
    }
    for (unsigned tile=0;tile<256;++tile) {
        prepare(0x0208,0x3008); CombatMap[300]=(uint8_t)tile;
        check(path(0x0208,0x3008)==(tile<0x55)); /* Even coincident target checks start. */
    }
    for (unsigned threshold=0;threshold<=UINT16_MAX;++threshold) {
        prepare(0x0208,0x3008); BlockingTileCodeThreshold=(uint16_t)threshold;
        CombatMap[300]=85;
        check(path(0x0208,0x3008)==((int16_t)threshold>85));
    }
    /* Native half-cell sampling: east visits301 on both first and second
     * steps,302 only on the third; north visits300 then276. */
    prepare(0x0208,0x3008); CombatMap[302]=85;
    check(path(0x020A,0x3008)==1);
    cacheCalls=0; check(path(0x020B,0x3008)==0);
    prepare(0x0208,0x3008); CombatMap[276]=85;
    check(path(0x0208,0x3007)==1);
    cacheCalls=0; check(path(0x0208,0x3006)==0);
    prepare(0x0208,0x3008); CombatMap[324]=85;
    check(path(0x0208,0x3009)==1);
    cacheCalls=0; check(path(0x0208,0x300A)==0);
    prepare(0x0208,0x3008); CombatMap[299]=85;
    check(path(0x0207,0x3008)==0);
    prepare(0x0208,0x3008); CombatMap[277]=85;
    check(path(0x0209,0x3007)==1);
    cacheCalls=0; check(path(0x020A,0x3006)==0);
    /* All parity combinations and short target headings on clear terrain. */
    for (unsigned px=0;px<2;++px) for (unsigned py=0;py<2;++py)
        for (int dx=-4;dx<=4;++dx) for (int dy=-4;dy<=4;++dy) {
            uint16_t x=(uint16_t)(0x0208+px), y=(uint16_t)(0x3008+py);
            prepare(x,y); check(path((uint16_t)(x+dx),(uint16_t)(y+dy))==1);
        }
    prepare(0x027E,0x307E); check(path(0x0301,0x307E)==1);
    prepare(0x0301,0x3008); check(path(0x027E,0x3008)==1);
    /* Cache preparation happens BEFORE the rental target13 shortcut. */
    prepare(0x0208,0x3008); memset(CombatMap,255,sizeof CombatMap); ArenaRentalMechMode=1;
    CompassCompareX0=0x1234;
    check(Combat_Check_Terrain_Path(13,13,0x0208,0x3008)==1 && cacheCalls==1);
    check(CachedMapOriginIndex==150 && CompassCompareX0==0x1234);
    cacheCalls=0; check(path(0x0208,0x3008)==0); /* Target12 does not qualify. */
    printf("Terrain path/direction: %u checks passed\n",checks); return 0;
}
