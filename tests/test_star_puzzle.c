#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned boxes,keys,sounds;
static uint16_t lastSound;
static char text[128];
static void verify(int condition,unsigned line) { if(!condition) { fprintf(stderr,"Star puzzle mismatch line%u\n",line); exit(1); } }
#define check(condition) verify(!!(condition),__LINE__)
void Draw_Message_Box(void) { ++boxes; }
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *message) { check(strlen((char *)message)<sizeof text); memcpy(text,message,strlen((char *)message)+1); }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
void Play_Sound_If_Enabled(uint16_t sound) { lastSound=sound; ++sounds; }
static void prepare(void) {
    memset(MapFileTiles,0x97,MapFileMaximumTileBytes);
    boxes=keys=sounds=0; text[0]=0; WhiteCacheCodeCorrect=2; MessageBoxOpen=0;
}
static void solve(unsigned success) {
    Cache_StarMap_CorrectPassword();
    check(boxes==1 && keys==1 && sounds==1 && MessageBoxOpen==TRUE);
    check(lastSound==(success?Sound_PasswordAccepted:Sound_PasswordIncorrect));
    check(WhiteCacheCodeCorrect==(success?1:2));
    check(!strcmp(text,success?"Password accepted.\rWHITE code installed.\rYou can trigger the HyperPulse Generator now.":"Incorrect password."));
}
int main(void) {
    /* Actual original toggle and puzzle logic; sound/text/input boundaries.
     * Required-set permutations are irrelevant: all128 selection subsets. */
    for(unsigned mask=0;mask<128;++mask) {
        prepare();
        for(unsigned target=0;target<7;++target) if(mask&(1u<<target)) MapFileTiles[CacheStarPuzzleTargetOffsets[target]]=0x98;
        solve(mask==127);
        for(unsigned target=0;target<7;++target) check(MapFileTiles[CacheStarPuzzleTargetOffsets[target]]==(mask==127?0x98:0x97));
    }
    for(unsigned type=0;type<256;++type) {
        prepare(); for(unsigned target=0;target<7;++target) MapFileTiles[CacheStarPuzzleTargetOffsets[target]]=0x98;
        MapFileTiles[0]=(uint8_t)type; unsigned selected=type>=0x97 && type<=0xF0 && !(type&1);
        solve(!selected); check(MapFileTiles[0]==(selected?type-1:type));
    }
    prepare(); for(unsigned target=0;target<7;++target) MapFileTiles[CacheStarPuzzleTargetOffsets[target]]=0;
    MapFileTiles[768]=0x98; solve(TRUE); check(MapFileTiles[768]==0x98); /* Parity-only required checks / scan extent. */
    for(unsigned extra=0;extra<CacheMapRoomTileBytes;++extra) {
        prepare(); unsigned required=FALSE;
        for(unsigned target=0;target<7;++target) {
            MapFileTiles[CacheStarPuzzleTargetOffsets[target]]=0x98;
            if(extra==CacheStarPuzzleTargetOffsets[target]) required=TRUE;
        }
        MapFileTiles[extra]=0x98; solve(required);
    }
    prepare(); WhiteCacheCodeCorrect=TRUE; Cache_StarMap_CorrectPassword();
    check(WhiteCacheCodeCorrect==TRUE && lastSound==Sound_PasswordIncorrect);
    for(unsigned type=0;type<256;++type) {
        prepare(); MapFileTiles[0]=(uint8_t)type; Map_Interactable_Play_Sound(0xFFFF,0xF000);
        check(MapFileTiles[0]==(uint8_t)(type+(type&1?1:-1)) && sounds==1 && lastSound==Sound_MapInteraction);
    }
    for(unsigned x=0;x<128;++x) for(unsigned y=0;y<128;++y) {
        prepare(); unsigned localX=(x+1)%128;
        unsigned offset=(y/16)*256+((y%16)/2)*8+(localX/16)*64+(localX%16)/2;
        Map_Interactable_Play_Sound((uint16_t)(0x0700+x),(uint16_t)(0x9000+y));
        check(MapFileTiles[offset]==0x98 && lastSound==Sound_MapInteraction);
        if(offset) check(MapFileTiles[offset-1]==0x97);
        check(MapFileTiles[offset+1]==0x97);
    }
    puts("Original star toggle, required-set checks and WHITE latch passed"); return 0;
}
