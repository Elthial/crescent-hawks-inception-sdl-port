#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){
    uint8_t expected[OriginalSavedStorageBytes];
    const uint16_t positions[]={0,0x7F,0x80,0x1234,0xABCD,0xFFFF};
    for(unsigned slot=0;slot<=68;++slot)for(unsigned high=0;high<256;++high)
    for(unsigned entry=0;entry<6;++entry){
        memset(OriginalSavedState.bytes,0xA5,sizeof OriginalSavedState.bytes);
        NextMapEffectSlot=(uint8_t)slot;MechSlotByMenuRow[0]=(uint8_t)high;
        memcpy(expected,OriginalSavedState.bytes,sizeof expected);
        uint16_t x=positions[entry],y=positions[5-entry];
        /* Independent sequential byte-store oracle. D557 overlaps y[64];
         * D558 overlaps y[65] and the counter's high byte. */
        unsigned sprite=0xD457-0xC614,page=0xD497-0xC614;
        unsigned xBase=0xD4D7-0xC614,yBase=0xD517-0xC614,cursor=0xD557-0xC614;
        expected[sprite+slot]=0xFE;expected[page+slot]=(uint8_t)((x/256)|(y/256));
        expected[xBase+slot]=(uint8_t)(x%128);
        expected[cursor]=(uint8_t)(slot+1);
        expected[yBase+slot]=(uint8_t)(y%128);
        if(expected[cursor]>=64)expected[cursor]=0;
        Register_Persistent_Map_Effect(0xABFE,x,y);
        if(memcmp(expected,OriginalSavedState.bytes,sizeof expected)){
            fprintf(stderr,"Map effect mismatch slot%u high%u entry%u\n",slot,high,entry);return EXIT_FAILURE;
        }
    }
    puts("Original effect registration preserves saved-state and menu aliases.");return 0;
}
