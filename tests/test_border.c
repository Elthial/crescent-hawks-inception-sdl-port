#include "game.h"
#include <assert.h>
#include <stdio.h>
typedef struct TileCall { uint16_t tile,column,row; } TileCall;
static TileCall calls[100];
static unsigned count;
static uint8_t tiles[8*BorderTileBytes];
/* Test adapter for pending original207F:275C hardware routine only. */
void DrawCall_SingleTile(uint8_t *tile,uint16_t column,uint16_t row)
{
    assert(count<100 && tile>=tiles && tile<tiles+sizeof tiles);
    assert((tile-tiles)%BorderTileBytes==0);
    calls[count].tile=(uint16_t)((tile-tiles)/BorderTileBytes);
    calls[count].column=column; calls[count].row=row; ++count;
}
static void expect(unsigned index,uint16_t tile,uint16_t column,uint16_t row)
{
    assert(index<count && calls[index].tile==tile && calls[index].column==column && calls[index].row==row);
}
int main(void)
{
    BorderTileset=tiles;
    assert(BorderStyles[0]==BorderStyles[1] && BorderStyles[1]==BorderStyles[2]);
    TextPanelLeft=2; TextPanelTop=3; TextPanelWidth=5; TextPanelHeight=2;
    for (uint16_t style=0;style<BorderStyleCount;++style) {
        count=0; Draw_Menu_Border(style); assert(count==18);
        expect(0,style==3?2:0,1,2); expect(1,style==3?3:1,7,2);
        expect(2,style==4?2:6,1,5); expect(3,style==4?3:7,7,5);
        for (uint16_t i=0;i<5;++i) expect(4+i,4,(uint16_t)(2+i),2);
        expect(9,5,1,3); expect(10,5,1,4); expect(11,5,7,3); expect(12,5,7,4);
        for (uint16_t i=0;i<5;++i) expect(13+i,4,(uint16_t)(2+i),5);
    }
    uint16_t pattern[]={1,2,3,255,4,255};
    count=0; assert(DrawCall_Border(pattern,0,10,11,5,1)==4 && count==5);
    for (uint16_t i=0;i<5;++i) expect(i,(uint16_t)(1+i%3),(uint16_t)(10+i),11);
    count=0; assert(DrawCall_Border(pattern,0,10,11,0,0)==4 && count==0);
    assert(DrawCall_Border(pattern,0,10,11,65535,0)==4 && count==0);
    assert(DrawCall_Border(pattern,4,10,11,2,0)==6 && count==2);
    expect(0,4,10,11); expect(1,4,10,12);
    count=0; assert(DrawCall_Border(pattern,0,65535,11,2,1)==4);
    expect(0,1,65535,11); expect(1,2,0,11);
    BorderDescriptors[2][0]=7;
    assert(BorderStyles[0][0]==7 && BorderStyles[1][0]==7 && BorderStyles[2][0]==7);
    puts("Original border styles, corner/edge order, repeats and sentinel scans passed");
    return 0;
}
