#include "game.h"

/* Sol: GameSeeds aliases the procedural seed bytes in original246C:09FB.
 * Do not introduce independent storage:207F subdivision reads these writes. */

/* Sol: Original0800:50C8..527A, ASM plus .dis return tail. Attract playback
 * returns to title/music; only interactive play clears the loop flag. EGA00D1
 * is a no-op; compiler frame allocation/unused stack WORD have no host effect. */
void Start_Game(void)
{
    for (uint16_t index=0;index<GameSeedTableSize;++index)
        GameSeeds[index]=Rand_0x00_to_0xFF();
    GraphicsCompatibilityFlag=FALSE;
    GraphicsCompatibilityFlag=TRUE;
    Load_And_Draw_ANIMATE_ICN();
    uint16_t continueStartup=TRUE;
    Load_And_Play_Intro_Music();
    while (continueStartup!=FALSE) {
        DisableInput=FALSE;
        if (Pending_Input()==FALSE) {
            Select_Game_Disk_And_Drive(GameDisk_Second);
            DOS_Load_File_to_memory((const uint8_t *)"DEMOFILE",
                BTStatsOrBldMemory+AttractDemoBufferOffset,AttractDemoFileBytes);
            AttractModeReplayIndex=AttractDemoBufferOffset;
            /* Native WORD seeds1325/0090 split into original byte state. */
            RandomByteLow=0x25; RandomByteMiddle=0x13;
            RandomByteHigh=0x90; RandomStateUpperByte=0;
            TilesetId=Tileset_None;
            DisableInput=TRUE;
            Load_Game_Map_Data();
            Main_Game_Loop(TRUE);
        } else {
            Drain_Pending_Keyboard_Input();
            Load_Game_Map_Data();
            if (AttractModeRecordingActive==FALSE) {
                Menu_Memory_Variables(TextPanel_FirstTimePlayer);
                Draw_Top_Graphic_Sidebar();
                Draw_Menu_Border(0); /* original plain border style */
                Display_Text_From_Memory((uint8_t *)"\"BattleTech\" is a registered trademark of FASA corporation,  ");
                Display_Text_From_Memory((uint8_t *)"(C) 1988 and is used under exclusive license.\r\rBoard game, design, ");
                Display_Text_From_Memory((uint8_t *)"characters and universe (C) 1988 FASA.\r\rComputer program (C) 1988 Infocom, Inc.\r\r");
                Display_Text_From_Memory((uint8_t *)"Computer program by Westwood Associates.\r\r");
                Display_Text_From_Memory((uint8_t *)"Is this your first time playing BattleTech?");
                if (Prompt_Yes_No(TRUE)==FALSE) {
                    Display_Text_From_Memory((uint8_t *)"\r\rYou can load a previously saved game, or start a new game.  Do you want to load an old game?");
                    if (Prompt_Yes_No(TRUE)!=FALSE) Load_Game();
                }
            }
            Main_Game_Loop(FALSE);
            AttractModeRecordingActive=FALSE;
            continueStartup=FALSE;
        }
        if (continueStartup!=FALSE) {
            Load_And_Draw_BTTITLE_CMP();
            Load_And_Play_Intro_Music();
        }
    }
}
