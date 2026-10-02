#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t allocation[4098],source[2048],destination[2048];
static uint32_t requested;
static int fail;
uint8_t *Allocate_Far_Buffer(uint32_t bytes)
{
    requested=bytes; memset(allocation,0xCC,sizeof allocation);
    return fail?NULL:allocation+1;
}
int main(void)
{
    for (unsigned i=0;i<sizeof GraphicsFileWorkspace;++i) GraphicsFileWorkspace[i]=(uint8_t)(i*17+i/157);
    Capture_Combat_Sprite(0,3,24,3,24);
    uint8_t *sprite=CombatSpritePointers[0];
    assert(sprite==allocation+1 && requested==292);
    assert(sprite[0]==0xCC && sprite[1]==23 && sprite[2]==3 && sprite[3]==0xCC);
    for (unsigned row=0;row<24;++row)
        assert(!memcmp(sprite+4+row*12,GraphicsFileWorkspace+(24+row)*160+12,12));
    assert(allocation[0]==0xCC && sprite[292]==0xCC);
    fail=1; Capture_Combat_Sprite(0,0,0,3,24); assert(CombatSpritePointers[0]==NULL);
    fail=0; Capture_Combat_Sprite(375,8,160,2,14);
    assert(CombatSpritePointers[375]==allocation+1 && requested==116);
    sprite=CombatSpritePointers[375];
    for (unsigned row=0;row<14;++row)
        assert(!memcmp(sprite+4+row*8,GraphicsFileWorkspace+(160+row)*160+32,8));
    for (unsigned i=0;i<sizeof source;++i) source[i]=(uint8_t)i;
    memset(destination,0xCC,sizeof destination);
    Copy_Strided_Word_Rows(source,destination,257,258,4);
    assert(destination[0]==0 && destination[1]==1 && destination[2]==6 && destination[3]==7 && destination[4]==0xCC);
    Copy_Strided_Word_Rows(source,destination,1,0,4);
    for (unsigned row=0;row<256;++row) assert(destination[row*2]==(uint8_t)(row*6) && destination[row*2+1]==(uint8_t)(row*6+1));
    memset(destination,0xCC,sizeof destination);
    Copy_Strided_Word_Rows(source,destination,256,0,1);
    assert(destination[0]==0xCC && destination[511]==0xCC);
    uint8_t overlap[]={1,2,3,4,5,6,7,8};
    Copy_Strided_Word_Rows(overlap,overlap+2,3,1,0);
    const uint8_t expected[]={1,2,1,2,1,2,1,2}; assert(!memcmp(overlap,expected,8));
    puts("Original sprite allocation/header/pointer publication and BYTE-count row copying passed");
    return 0;
}
