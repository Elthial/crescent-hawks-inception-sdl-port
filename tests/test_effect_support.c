#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned stage,draws;
static uint16_t expectedFocus,expectedSprite,expectedX,expectedY;
static void verify(int condition,unsigned line) { if(!condition) { fprintf(stderr,"Effect support mismatch line%u\n",line); exit(1); } }
#define check(condition) verify(!!(condition),__LINE__)
/* Actual original map copy/math/wrappers; draw/cache-update adapters isolated.
 * Their ordering and arguments are checked, not substituted game logic. */
void Move_Map_View_To_Packed_Position(uint16_t x,uint16_t y) {
    check(stage++==0 && x==CombatantPackedX[expectedFocus] && y==CombatantPackedY[expectedFocus]);
    for(unsigned i=0;i<MapCacheTileCount;++i) check(CombatMap[i]==(uint8_t)(i*7));
}
void Update_Animated_Map_Tiles(void) { check(stage++==1); }
void Copy_Data_To_GraphicsMemory(void) { check(stage++==2); }
void Draw_Menu_MultiSelect(void) { check(stage++==3); }
void DrawCall_EGA_CharacterPos(EgaMemoryAddress destination,uint8_t *sprite,int16_t x,int16_t y) {
    check(destination.offset==0 && destination.segment==MapViewportSegment);
    check(sprite==CombatSpritePointers[expectedSprite] && x==(int16_t)expectedX && y==(int16_t)expectedY); ++draws;
}
static uint16_t packX(unsigned cell) { return (uint16_t)((cell/128)*256+cell%128); }
static uint16_t packY(unsigned cell) { return (uint16_t)((cell/128)*4096+cell%128); }
static unsigned expectedOctant(int y,int x) {
    /* Independent signed-8086 magnitude comparison, including NEG8000. */
    int negateY=y==-32768?-32768:-y,negateX=x==-32768?-32768:-x;
    if(y<0 && x>0) return negateY<=x?2:3;
    if(y<0) return negateX<=negateY?4:5;
    if(x<=0) return negateX>=y?6:7;
    return x<=y?0:1;
}
int main(void) {
    const int samples[]={-32768,-32767,-257,-128,-1,0,1,127,256,32766,32767};
    for(unsigned bits=0;bits<65536;++bits) for(unsigned sample=0;sample<sizeof samples/sizeof samples[0];++sample) {
        int16_t delta=(int16_t)(uint16_t)bits,other=(int16_t)samples[sample];
        check(Combat_Projectile_Octant(delta,other)==expectedOctant(delta,other));
        check(Combat_Projectile_Octant(other,delta)==expectedOctant(other,delta));
    }
    const unsigned positions[]={0,1,126,127,128,129,255,256,1023,1024,2046,2047};
    for(unsigned a=0;a<sizeof positions/sizeof positions[0];++a)
        for(unsigned b=0;b<sizeof positions/sizeof positions[0];++b) {
            Combat_CompassPos(packX(positions[a]),packY(positions[b]),packX(positions[b]),packY(positions[a]));
            check(MovementActorPositionX==(uint16_t)(26+(int)positions[b]-(int)positions[a]));
            check(MovementActorPositionY==(uint16_t)(12+(int)positions[a]-(int)positions[b]));
        }
    uint8_t external[MapCacheTileCount+2];
    for(unsigned i=0;i<MapCacheTileCount;++i) CombatMap[i]=(uint8_t)(i*7);
    memset(external,0xCC,sizeof external); Combat_Copy_Map_Cache(external+1,FALSE);
    check(external[0]==0xCC && external[sizeof external-1]==0xCC);
    check(!memcmp(external+1,CombatMap,MapCacheTileCount));
    for(unsigned flag=1;flag<65536;flag*=2) {
        memset(CombatMap,0,MapCacheTileCount); Combat_Copy_Map_Cache(external+1,(uint16_t)flag);
        check(!memcmp(external+1,CombatMap,MapCacheTileCount));
    }
    /* Overlap test lives within the COMPLETE cache object. Source/destination
     * tiles-2 shares its preceding terrain byte/descriptor state; per-word
     * forward copying must differ from memmove on the restore direction. */
    uint8_t expected[sizeof MapCache],*cacheBytes=(uint8_t *)&MapCache;
    size_t tileOffset=offsetof(MapCacheStorage,tiles);
    for(unsigned restore=0;restore<2;++restore) {
        for(unsigned i=0;i<sizeof MapCache;++i) cacheBytes[i]=(uint8_t)(i*11);
        memcpy(expected,cacheBytes,sizeof expected);
        size_t source=restore?tileOffset-2:tileOffset,destination=restore?tileOffset:tileOffset-2;
        for(unsigned i=0;i<MapCacheTileCount;i+=2) {
            uint8_t low=expected[source+i],high=expected[source+i+1];
            expected[destination+i]=low; expected[destination+i+1]=high;
        }
        Combat_Copy_Map_Cache(cacheBytes+tileOffset-2,(uint16_t)restore);
        check(!memcmp(expected,cacheBytes,sizeof expected));
    }
    for(unsigned assignment=0;assignment<=8;++assignment) {
        Characters[0].mechAssignment=(uint8_t)assignment; expectedFocus=(uint16_t)(assignment<8?assignment:4);
        CombatantPackedX[expectedFocus]=(uint16_t)(0x0200+assignment); CombatantPackedY[expectedFocus]=(uint16_t)(0x3000+assignment);
        stage=0; Combat_Restore_Map_View_And_Draw_World(external+1); check(stage==4);
    }
    for(unsigned sprite=0;sprite<CombatSpriteCount;++sprite) {
        expectedSprite=(uint16_t)sprite; CombatSpritePointers[sprite]=external+(sprite%sizeof external);
        expectedX=(uint16_t)(sprite*257); expectedY=(uint16_t)(0xFFFF-sprite);
        Draw_Combat_Sprites(expectedSprite,expectedX,expectedY);
    }
    check(draws==CombatSpriteCount); puts("Original combat effect support checks passed"); return 0;
}
