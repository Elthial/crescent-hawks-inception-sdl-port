#include "game.h"

/* Sol: Actual EXE-owned WORD descriptors305B:0320/0338/0350. The first
 * three FAR table entries alias the SAME0350 descriptor; preserve that alias. */
uint16_t BorderDescriptors[3][BorderDescriptorWords]={
    {0,1,2,3,4,255,5,255,5,255,4,255},
    {2,3,6,7,4,255,5,255,5,255,4,255},
    {0,1,6,7,4,255,5,255,5,255,4,255}
};
uint16_t *BorderStyles[BorderStyleCount]={
    BorderDescriptors[2],BorderDescriptors[2],BorderDescriptors[2],
    BorderDescriptors[1],BorderDescriptors[0]
};
uint8_t *BorderTileset;

/* Sol: Original1E56:01E7..0280. Repeat one terminated sequence, then scan
 * its remaining entries and return the following sequence index. Signed WORD
 * negative/zero lengths skip drawing but STILL scan the WORD255 terminator.
 * Valid nonempty terminated descriptors/tilesets are the host contract. */
uint16_t DrawCall_Border(uint16_t *descriptor,uint16_t sequenceStart,
    uint16_t column,uint16_t row,uint16_t length,uint16_t horizontal)
{
    uint16_t sequenceIndex=sequenceStart,count=0;
    while ((int16_t)count < (int16_t)length) {
        uint16_t tileOffset=(uint16_t)(descriptor[sequenceIndex]*BorderTileBytes);
        DrawCall_SingleTile(BorderTileset+tileOffset,column,row);
        if (horizontal != 0) ++column; else ++row;
        ++sequenceIndex;
        if (descriptor[sequenceIndex]==BorderPatternEnd) sequenceIndex=sequenceStart;
        ++count;
    }
    while (descriptor[sequenceIndex]!=BorderPatternEnd) ++sequenceIndex;
    return (uint16_t)(sequenceIndex+1);
}

/* Sol: Original1E56:0004..01E6. Four corners, then top/left/right/bottom
 * edge sequences, with return AX chaining their starts. Cell coordinates and
 * tile-ID*32 arithmetic retain WORD wrapping. No annotation-only FAR lookup
 * or tile-address helper is included; ordinary typed global views suffice for
 * original in-range data whose native base+offset does not wrap a segment. */
void Draw_Menu_Border(uint16_t style)
{
    uint16_t *descriptor=BorderStyles[style];
    uint16_t left=TextPanelLeft,top=TextPanelTop;
    DrawCall_SingleTile(BorderTileset+(uint16_t)(descriptor[0]*BorderTileBytes),
        (uint16_t)(left-1),(uint16_t)(top-1));
    DrawCall_SingleTile(BorderTileset+(uint16_t)(descriptor[1]*BorderTileBytes),
        (uint16_t)(left+TextPanelWidth),(uint16_t)(top-1));
    DrawCall_SingleTile(BorderTileset+(uint16_t)(descriptor[2]*BorderTileBytes),
        (uint16_t)(left-1),(uint16_t)(top+TextPanelHeight));
    DrawCall_SingleTile(BorderTileset+(uint16_t)(descriptor[3]*BorderTileBytes),
        (uint16_t)(left+TextPanelWidth),(uint16_t)(top+TextPanelHeight));
    uint16_t sequenceStart=BorderCornerCount;
    sequenceStart=DrawCall_Border(descriptor,sequenceStart,left,(uint16_t)(top-1),TextPanelWidth,1);
    sequenceStart=DrawCall_Border(descriptor,sequenceStart,(uint16_t)(TextPanelLeft-1),top,TextPanelHeight,0);
    sequenceStart=DrawCall_Border(descriptor,sequenceStart,(uint16_t)(TextPanelLeft+TextPanelWidth),top,TextPanelHeight,0);
    (void)DrawCall_Border(descriptor,sequenceStart,left,(uint16_t)(top+TextPanelHeight),TextPanelWidth,1);
}
