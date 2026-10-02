#include "game.h"

/* Sol: Original207F:0BFB..0D06. Explicit native SI/DI buffer contracts:
 * separate64-byte descriptor and81-byte lattice; preserve four corners.
 * No extracted research noise/seed helpers enter the preservation code. */
void Map_Build_Procedural_Block(uint8_t *descriptorOutput,uint8_t *lattice)
{
    uint8_t corners[4]={lattice[0],lattice[MapCornerTopRight],
        lattice[MapCornerBottomLeft],lattice[MapCornerBottomRight]};
    for (uint16_t index=0;index<MapCornerBottomRight;++index) lattice[index]=MapUnfilledVertex;
    lattice[0]=corners[0]; lattice[MapCornerTopRight]=corners[1];
    lattice[MapCornerBottomLeft]=corners[2]; lattice[MapCornerBottomRight]=corners[3];
    for (uint16_t edge=0;edge<4;++edge) {
        uint8_t first=edge==0?0:edge==1?MapCornerBottomLeft:edge==2?0:MapCornerTopRight;
        uint8_t last=edge==0?MapCornerTopRight:edge==1?MapCornerBottomRight:
            edge==2?MapCornerBottomLeft:MapCornerBottomRight;
        MapConstructionSeedIndex=(uint8_t)(lattice[first]+lattice[last]);
        MapSubdivisionStack[0]=first; MapSubdivisionStack[1]=last;
        uint16_t stackTop=2;
        if (edge<2) Map_Subdivide_Horizontal_Edge(lattice,&stackTop);
        else Map_Subdivide_Vertical_Edge(lattice,&stackTop);
    }
    /* Interior continues the right-edge seed stream, without reseeding. */
    MapSubdivisionStack[0]=0; MapSubdivisionStack[1]=MapCornerTopRight;
    MapSubdivisionStack[2]=MapCornerBottomLeft; MapSubdivisionStack[3]=MapCornerBottomRight;
    uint16_t stackTop=4;
    Map_Subdivide_Rectangle(lattice,&stackTop);
    for (uint16_t row=0;row<MapBlockHeight;++row)
        for (uint16_t column=0;column<MapBlockWidth;++column)
            descriptorOutput[row*MapBlockWidth+column]=lattice[row*MapLatticeWidth+column]&MapConstructionTerrainMask;
}

/* Sol: OriginalNEAR207F:0D07..0D78. Push left then right; LIFO visits
 * right first. Only anFF midpoint consumes a seed. Increment follows store. */
void Map_Subdivide_Horizontal_Edge(uint8_t *lattice,uint16_t *stackTop)
{
    while (*stackTop!=0) {
        *stackTop-=2;
        uint16_t top=*stackTop;
        uint8_t first=MapSubdivisionStack[top],last=MapSubdivisionStack[top+1];
        uint8_t halfSpan=(uint8_t)(last-first);
        if (halfSpan==1) continue;
        uint8_t firstValue=lattice[first],lastValue=lattice[last];
        halfSpan>>=1;
        uint8_t midpoint=(uint8_t)(first+halfSpan);
        MapSubdivisionStack[top]=first; MapSubdivisionStack[top+1]=midpoint;
        MapSubdivisionStack[top+2]=midpoint; MapSubdivisionStack[top+3]=last;
        *stackTop+=4;
        if (lattice[midpoint]==MapUnfilledVertex) {
            uint16_t mean=(uint16_t)((firstValue+lastValue)>>1);
            uint8_t seed=MapConstructionSeeds[MapConstructionSeedIndex];
            uint8_t amplitude=(uint8_t)(halfSpan<<1);
            uint8_t mask=(uint8_t)((amplitude<<1)-1);
            uint8_t noise=(uint8_t)((seed&mask)-amplitude);
            uint8_t value=(uint8_t)(mean+noise);
            lattice[midpoint]=value>=PackedPositionLocalCarryBit?0:value;
            MapConstructionSeedIndex=(uint16_t)((MapConstructionSeedIndex&0xFF00)|
                (uint8_t)(MapConstructionSeedIndex+1));
        }
    }
}

/* Sol: OriginalNEAR207F:0D79..0DF7. Vertical leaf distance9 is one lattice
 * row. Amplitude uses SHR3, literal CMP9/DEC, then SHL1, NOT division by9. */
void Map_Subdivide_Vertical_Edge(uint8_t *lattice,uint16_t *stackTop)
{
    while (*stackTop!=0) {
        *stackTop-=2;
        uint16_t top=*stackTop;
        uint8_t first=MapSubdivisionStack[top],last=MapSubdivisionStack[top+1];
        uint8_t halfSpan=(uint8_t)(last-first);
        if (halfSpan==MapLatticeWidth) continue;
        uint8_t firstValue=lattice[first],lastValue=lattice[last];
        halfSpan>>=1;
        uint8_t midpoint=(uint8_t)(first+halfSpan);
        MapSubdivisionStack[top]=first; MapSubdivisionStack[top+1]=midpoint;
        MapSubdivisionStack[top+2]=midpoint; MapSubdivisionStack[top+3]=last;
        *stackTop+=4;
        if (lattice[midpoint]==MapUnfilledVertex) {
            uint16_t mean=(uint16_t)((firstValue+lastValue)>>1);
            uint8_t seed=MapConstructionSeeds[MapConstructionSeedIndex];
            uint8_t amplitude=halfSpan>>3;
            if (amplitude==9) --amplitude; /* Literal native correction. */
            amplitude=(uint8_t)(amplitude<<1);
            uint8_t mask=(uint8_t)((amplitude<<1)-1);
            uint8_t noise=(uint8_t)((seed&mask)-amplitude);
            uint8_t value=(uint8_t)(mean+noise);
            lattice[midpoint]=value>=PackedPositionLocalCarryBit?0:value;
            MapConstructionSeedIndex=(uint16_t)((MapConstructionSeedIndex&0xFF00)|
                (uint8_t)(MapConstructionSeedIndex+1));
        }
    }
}

/* Sol: OriginalNEAR207F:0DF8..0FED. Preserve interleaved child-stack writes,
 * top/left/bottom/right seed order, retained DH influence on means and the
 * right-edge use of halfWidth instead of halfHeight (original behaviour). */
void Map_Subdivide_Rectangle(uint8_t *lattice,uint16_t *stackTop)
{
    while (*stackTop!=0) {
        *stackTop-=4;
        uint16_t top=*stackTop;
        uint8_t tl=MapSubdivisionStack[top],tr=MapSubdivisionStack[top+1];
        uint8_t bl=MapSubdivisionStack[top+2],br=MapSubdivisionStack[top+3];
        if ((uint8_t)(tr-tl)==1) continue;
        uint8_t centre=(uint8_t)(tl+((uint8_t)(br-tl)>>1));
        if (lattice[centre]==MapUnfilledVertex)
            lattice[centre]=(uint8_t)((lattice[tl]+lattice[tr]+lattice[bl]+lattice[br])>>2);
        MapConstructionScratch[0]=tl; MapConstructionScratch[1]=tr;
        MapConstructionScratch[2]=bl; MapConstructionScratch[3]=br;
        MapConstructionScratch[4]=centre;
        MapSubdivisionStack[top]=tl; MapSubdivisionStack[top+3]=centre;
        MapSubdivisionStack[top+6]=centre; MapSubdivisionStack[top+9]=centre;
        MapSubdivisionStack[top+12]=centre;
        uint8_t halfWidth=(uint8_t)(tr-tl)>>1;
        MapConstructionScratch[5]=halfWidth;
        uint16_t edgeMean=(uint16_t)((lattice[tl]+lattice[tr])>>1);
        uint8_t edge=(uint8_t)(tl+halfWidth);
        MapSubdivisionStack[top+5]=tr;
        MapSubdivisionStack[top+1]=edge; MapSubdivisionStack[top+4]=edge;
        if (lattice[edge]==MapUnfilledVertex) {
            uint8_t seed=MapConstructionSeeds[MapConstructionSeedIndex];
            MapConstructionSeedIndex=(uint16_t)((MapConstructionSeedIndex&0xFF00)|(uint8_t)(MapConstructionSeedIndex+1));
            uint8_t amplitude=(uint8_t)(halfWidth<<1);
            uint8_t noise=(uint8_t)((seed&(uint8_t)((amplitude<<1)-1))-amplitude);
            uint8_t value=(uint8_t)(edgeMean+noise);
            lattice[edge]=value>=PackedPositionLocalCarryBit?0:value;
        }
        uint8_t halfHeight=(uint8_t)(bl-tl)>>1;
        MapConstructionScratch[6]=halfHeight;
        edgeMean=(uint16_t)((lattice[tl]+lattice[bl])>>1);
        edge=(uint8_t)(tl+halfHeight);
        MapSubdivisionStack[top+2]=edge; MapSubdivisionStack[top+8]=edge;
        if (lattice[edge]==MapUnfilledVertex) {
            uint8_t seed=MapConstructionSeeds[MapConstructionSeedIndex];
            MapConstructionSeedIndex=(uint16_t)((MapConstructionSeedIndex&0xFF00)|(uint8_t)(MapConstructionSeedIndex+1));
            uint8_t amplitude=halfHeight>>3;
            if (amplitude==9) --amplitude;
            amplitude=(uint8_t)(amplitude<<1);
            uint8_t noise=(uint8_t)((seed&(uint8_t)((amplitude<<1)-1))-amplitude);
            uint8_t value=(uint8_t)(edgeMean+noise);
            if (value>=PackedPositionLocalCarryBit) value=0;
            lattice[edge]=value;
            edgeMean=(uint16_t)(((uint16_t)noise<<8)|value); /* Native retained DH. */
        }
        MapSubdivisionStack[top+10]=bl; MapSubdivisionStack[top+15]=br;
        edgeMean=(uint16_t)(((edgeMean&0xFF00)|lattice[bl])+lattice[br]);
        edgeMean>>=1;
        edge=(uint8_t)(bl+halfWidth);
        MapSubdivisionStack[top+11]=edge; MapSubdivisionStack[top+14]=edge;
        if (lattice[edge]==MapUnfilledVertex) {
            uint8_t seed=MapConstructionSeeds[MapConstructionSeedIndex];
            MapConstructionSeedIndex=(uint16_t)((MapConstructionSeedIndex&0xFF00)|(uint8_t)(MapConstructionSeedIndex+1));
            uint8_t amplitude=(uint8_t)(halfWidth<<1);
            uint8_t noise=(uint8_t)((seed&(uint8_t)((amplitude<<1)-1))-amplitude);
            uint8_t value=(uint8_t)(edgeMean+noise);
            if (value>=PackedPositionLocalCarryBit) value=0;
            lattice[edge]=value;
            edgeMean=(uint16_t)((edgeMean&0xFF00)|value);
        }
        edgeMean=(uint16_t)(((edgeMean&0xFF00)|lattice[tr])+lattice[br]);
        edgeMean>>=1;
        edge=(uint8_t)(tr+halfHeight);
        MapSubdivisionStack[top+7]=edge; MapSubdivisionStack[top+13]=edge;
        if (lattice[edge]==MapUnfilledVertex) {
            uint8_t seed=MapConstructionSeeds[MapConstructionSeedIndex];
            MapConstructionSeedIndex=(uint16_t)((MapConstructionSeedIndex&0xFF00)|(uint8_t)(MapConstructionSeedIndex+1));
            uint8_t amplitude=halfWidth>>3; /* Native AL: NOT halfHeight. */
            if (amplitude==9) --amplitude;
            amplitude=(uint8_t)(amplitude<<1);
            uint8_t noise=(uint8_t)((seed&(uint8_t)((amplitude<<1)-1))-amplitude);
            uint8_t value=(uint8_t)(edgeMean+noise);
            lattice[edge]=value>=PackedPositionLocalCarryBit?0:value;
        }
        *stackTop+=16; /* Children TL/TR/BL/BR; LIFO visits BR first. */
    }
}
