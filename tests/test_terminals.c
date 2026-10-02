#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned boxes,keys,prompts,scripts,doorFrames;
static uint16_t promptResult,lastScript,lastNumber;
static int reloadColour;
static char text[2048];
static void verify(int condition,unsigned line) { if(!condition) { fprintf(stderr,"Cache terminal mismatch line%u\n",line); exit(1); } }
#define check(condition) verify(!!(condition),__LINE__)
/* Whole five original methods and original persistent flags/tables. UI and
 * pending BLD parent are test-only boundaries, not production placeholders. */
void Draw_Message_Box(void) { ++boxes; }
void Display_Text_From_Memory(uint8_t *message) {
    size_t length=strlen(text),added=strlen((char *)message); check(length+added<sizeof text);
    memcpy(text+length,message,added+1);
}
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *message) { Display_Text_From_Memory(message); }
void Display_Text_Dynamic_Value(uint16_t number) { lastNumber=number; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
uint16_t Prompt_Yes_No(uint16_t choice) {
    check(choice==TRUE); ++prompts;
    if(reloadColour>=0) SecurityTerminalColour[0]=(int8_t)reloadColour;
    return promptResult;
}
void Interact_with_BLD(uint16_t script) {
    check(MessageBoxOpen==0x7777 && TransmittedCacheFound==0x8888);
    ++scripts; lastScript=script;
}
/* Door flow uses actual original selection/consumption/tile changes; drawing
 * and sound remain boundaries in this terminal-to-door integration case. */
void Play_Sound_If_Enabled(uint16_t sound) { check(sound==Sound_CacheDoor); }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(x==CrescentHawkMapPositionX && y==CrescentHawkMapPositionY); }
void Copy_Data_To_GraphicsMemory(void) {}
void Draw_Infantry_And_Mechs(void) {}
void EGA_DrawBox_Wrapper(void) { ++doorFrames; }
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(count==CacheDoorFrameRetraces); }
void Wait_For_50Hz_Then_Check_Input(void) { check(0); }
static void prepare(void) {
    boxes=keys=prompts=scripts=0; text[0]=0; reloadColour=-1;
    promptResult=TRUE; lastNumber=lastScript=0xFFFF;
    MessageBoxOpen=0x7777; TransmittedCacheFound=0x8888;
}
int main(void) {
    for(unsigned flag=0;flag<256;++flag) {
        prepare(); PhoenixHawkFound=(uint8_t)flag; StarLeague_Cache_PhoenixHawk();
        check(scripts==(unsigned)(flag==0) && !boxes && !keys && PhoenixHawkFound==flag);
        if(!flag) check(lastScript==Bld_EndMech);
    }
    prepare(); Display_StarLeague_Cache_Dialog_Window();
    check(boxes==1 && keys==1 && MessageBoxOpen==TRUE);
    check(!strcmp(text,"These boxes are full of 'Mech gyros!  The cache must be around here!"));
    /* Exhaustive local coordinates and varied upper packed page bits. */
    for(unsigned x=0;x<256;++x) for(unsigned y=0;y<256;++y) {
        prepare(); HPGTerminalPowerOn=0xFE;
        StarLeague_HyperPulse_Power_Dialog_Window((uint16_t)(0xF000+x),(uint16_t)(0x0F00+y));
        unsigned maskedX=((x+1)&126),maskedY=y&126;
        unsigned matches=maskedX==78 && maskedY>=12 && maskedY<=17;
        check(boxes==matches && keys==matches && HPGTerminalPowerOn==(matches?1:0xFE));
        check(MessageBoxOpen==(matches?1:0x7777));
    }
    memset(CacheSecurityCodeUsed,0,sizeof CacheSecurityCodeUsed);
    for(unsigned code=0;code<CacheSecurityCodeCount;++code) {
        for(int dx=-1;dx<=3;++dx) {
            prepare(); memset(SelectedCacheCodeByColour,0xFF,CacheCodeColourCount);
            StarLeague_Security_Terminal((uint16_t)(0x0200+SecurityTerminalPosX[code]+dx-1),(uint16_t)(0x3000+SecurityTerminalPosY[code]));
            unsigned matches=(unsigned)(dx>=0 && dx<3);
            check(boxes==matches && prompts==matches && !keys);
            check(MessageBoxOpen==(matches?1:0x7777));
            if(matches) {
                check(lastNumber==code+1 && SelectedCacheCodeByColour[SecurityTerminalColour[code]]==code);
                check(strstr(text,"Security Terminal.\rDo you want to imprint ") && strstr(text," on your code key?\x06\x0F"));
            }
            check(CacheSecurityCodeUsed[code]==0);
        }
        prepare(); promptResult=0; memset(SelectedCacheCodeByColour,0x55,CacheCodeColourCount);
        StarLeague_Security_Terminal((uint16_t)(SecurityTerminalPosX[code]-1),(uint16_t)SecurityTerminalPosY[code]);
        check(boxes==1 && prompts==1 && SelectedCacheCodeByColour[SecurityTerminalColour[code]]==0x55);
        prepare(); CacheSecurityCodeUsed[code]=2;
        StarLeague_Security_Terminal((uint16_t)(SecurityTerminalPosX[code]-1),(uint16_t)SecurityTerminalPosY[code]);
        check(boxes==1 && keys==1 && !prompts && !strcmp(text,"You've already used this terminal's code."));
        check(CacheSecurityCodeUsed[code]==2); CacheSecurityCodeUsed[code]=0;
    }
    prepare(); reloadColour=2;
    StarLeague_Security_Terminal((uint16_t)(SecurityTerminalPosX[0]-1),(uint16_t)SecurityTerminalPosY[0]);
    check(SelectedCacheCodeByColour[2]==0 && strstr(text,"RED")); SecurityTerminalColour[0]=0;
    /* Duplicate native table fixture demonstrates no early match break. */
    int8_t savedX=SecurityTerminalPosX[1],savedY=SecurityTerminalPosY[1];
    SecurityTerminalPosX[1]=SecurityTerminalPosX[0]; SecurityTerminalPosY[1]=SecurityTerminalPosY[0];
    prepare(); StarLeague_Security_Terminal((uint16_t)(SecurityTerminalPosX[0]-1),(uint16_t)SecurityTerminalPosY[0]);
    check(boxes==2 && prompts==2 && SelectedCacheCodeByColour[0]==1);
    SecurityTerminalPosX[1]=savedX; SecurityTerminalPosY[1]=savedY;
    const unsigned flagValues[]={0,1,2,127,128,255};
    for(unsigned power=0;power<6;++power) for(unsigned white=0;white<256;++white)
        for(unsigned parts=0;parts<6;++parts) for(unsigned location=0;location<2;++location) {
            prepare(); HPGTerminalPowerOn=(uint8_t)flagValues[power]; WhiteCacheCodeCorrect=(uint8_t)white;
            MechPartsCacheFound=(uint8_t)flagValues[parts];
            HPGTransmitter(location?1:3,location?14:4);
            unsigned success=flagValues[power]!=0 && white==1 && flagValues[parts]!=0;
            check(MessageBoxOpen==TRUE && scripts==success && boxes==!success && keys==!success);
            check(TransmittedCacheFound==(success?TRUE:0x8888));
            if(success) check(lastScript==Bld_WinScene);
            else if(!flagValues[power]) check(strstr(text,"power to these terminals"));
            else if(white!=1) check(!strcmp(text,"Incorrect WHITE code."));
            else check(strstr(text,"haven't found the cache"));
        }
    for(unsigned x=0;x<256;++x) for(unsigned y=0;y<256;++y) {
        prepare(); HPGTerminalPowerOn=1; WhiteCacheCodeCorrect=1; MechPartsCacheFound=1;
        HPGTransmitter((uint16_t)(0xFF00+x),(uint16_t)(0xF000+y));
        unsigned localX=(x+1)&126,localY=y&126;
        unsigned matches=(localX==4 && localY==4)||(localX==2 && localY==14);
        check(scripts==matches && !boxes && !keys && MessageBoxOpen==(matches?1:0x7777));
    }
    /* Imprint the three required codes, open their actual door, then revisit
     * a consumed terminal. No scripted replacement for either original body. */
    memset(CacheSecurityCodeUsed,0,sizeof CacheSecurityCodeUsed); memset(CacheDoorOpened,0,CacheDoorCount);
    for(unsigned colour=0;colour<CacheCodeColourCount;++colour) {
        unsigned code=(unsigned)(CacheDoorRequiredCode[colour][0]-1);
        prepare(); StarLeague_Security_Terminal((uint16_t)(SecurityTerminalPosX[code]-1),(uint16_t)SecurityTerminalPosY[code]);
        check(SelectedCacheCodeByColour[colour]==code && !CacheSecurityCodeUsed[code]);
    }
    prepare(); doorFrames=0;
    StarLeague_Key_Codes((uint16_t)(CacheDoorPositionX[0]-1),(uint16_t)CacheDoorPositionY[0],CacheDoorLookupByPosition);
    check(doorFrames==CacheDoorFrameCount && CacheDoorOpened[0]==TRUE);
    for(unsigned colour=0;colour<CacheCodeColourCount;++colour) {
        unsigned code=(unsigned)(CacheDoorRequiredCode[colour][0]-1);
        check(CacheSecurityCodeUsed[code]==TRUE && SelectedCacheCodeByColour[colour]==code);
        prepare(); StarLeague_Security_Terminal((uint16_t)(SecurityTerminalPosX[code]-1),(uint16_t)SecurityTerminalPosY[code]);
        check(!prompts && keys==1 && !strcmp(text,"You've already used this terminal's code."));
    }
    puts("Original Star League cache terminal checks passed"); return 0;
}
