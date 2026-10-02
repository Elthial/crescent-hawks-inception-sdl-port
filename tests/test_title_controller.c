#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static unsigned mode, probes, maps, playbacks, interactive, music, titles, prompts, texts, saves;
static unsigned randomCalls;
uint8_t Rand_0x00_to_0xFF(void) { return (uint8_t)randomCalls++; }
void Load_And_Draw_ANIMATE_ICN(void)
{
    assert(randomCalls==GameSeedTableSize && GraphicsCompatibilityFlag==TRUE);
    if(GameSeeds!=MapConstructionSeeds ||
        GameSeeds!=MapRuntime.bytes+(0x09FB-MapRuntimeFirstOffset)) {
        fputs("Startup/procedural seeds do not share native246C:09FB storage\n",stderr);
        exit(1);
    }
    for (unsigned i=0;i<GameSeedTableSize;++i) assert(GameSeeds[i]==(uint8_t)i);
}
void Load_And_Play_Intro_Music(void) { ++music; }
uint16_t Pending_Input(void)
{
    assert(DisableInput==FALSE); ++probes;
    return (uint16_t)(mode!=0 || probes==2);
}
void Select_Game_Disk_And_Drive(uint16_t disk) { assert(disk==GameDisk_Second); }
void DOS_Load_File_to_memory(const uint8_t *name,uint8_t *buffer,uint16_t bytes)
{
    assert(!strcmp((const char *)name,"DEMOFILE"));
    assert(buffer==BTStatsOrBldMemory+AttractDemoBufferOffset && bytes==1023);
}
void Load_Game_Map_Data(void) { ++maps; }
void Main_Game_Loop(uint16_t attract)
{
    if (attract) {
        assert(DisableInput==TRUE && AttractModeReplayIndex==AttractDemoBufferOffset);
        assert(RandomByteLow==0x25 && RandomByteMiddle==0x13 && RandomByteHigh==0x90 && RandomStateUpperByte==0);
        assert(TilesetId==Tileset_None); ++playbacks;
    } else { assert(DisableInput==FALSE); ++interactive; }
}
void Drain_Pending_Keyboard_Input(void) { assert(interactive==0); }
void Menu_Memory_Variables(uint16_t layout) { assert(layout==TextPanel_FirstTimePlayer); }
void Draw_Top_Graphic_Sidebar(void) { }
void Draw_Menu_Border(uint16_t style) { assert(style==0); }
void Display_Text_From_Memory(uint8_t *text) { assert(text && *text); ++texts; }
uint16_t Prompt_Yes_No(uint16_t defaultYes)
{
    assert(defaultYes==TRUE); ++prompts;
    if (prompts==1) return (uint16_t)(mode==1); /* first-time yes */
    return (uint16_t)(mode==3); /* load saved game yes */
}
void Load_Game(void) { ++saves; }
void Load_And_Draw_BTTITLE_CMP(void) { assert(playbacks==1); ++titles; }
int main(void)
{
    /* attract->interactive, first-time, decline save, load save, recording bypass */
    for (mode=0;mode<5;++mode) {
        probes=maps=playbacks=interactive=music=titles=prompts=texts=saves=randomCalls=0;
        AttractModeRecordingActive=(uint16_t)(mode==4);
        Start_Game();
        assert(interactive==1 && AttractModeRecordingActive==FALSE);
        assert(playbacks==(mode==0?1u:0u) && maps==(mode==0?2u:1u));
        assert(music==(mode==0?2u:1u) && titles==(mode==0?1u:0u));
        assert(prompts==(mode==4?0u:mode==1?1u:2u));
        assert(texts==(mode==4?0u:mode==1?5u:6u));
        assert(saves==(mode==3?1u:0u));
    }
    puts("Original title controller seed table, attract return, first-time/load choices and recording bypass passed");
    return 0;
}
