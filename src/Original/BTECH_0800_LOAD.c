#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>

/* Original0800:32B3..35D2. Sol: Restore native saved bytes, rebuild the map
 * and unsaved friendly animation state. Preserve unchecked read counts and
 * invalid-marker handle leak. Marker read must supply a BYTE: failed zero-
 * byte marker reads depend on original residual stack, not certified here. */
void Load_Game(void)
{
    Menu_Memory_Variables(3); Draw_Top_Graphic_Sidebar();
    Display_Text_From_Memory((uint8_t *)"Load Game:\rOne\rTwo\rThree\rFour\rFive\rSix\rCancel");
    uint16_t slot=Display_Menu_Choices_And_Check(SaveSlotMenu);
    if(slot!=SaveSlotCancel) {
        SaveGameFileName[4]=(uint8_t)(slot+'1');
        Select_Game_Disk_And_Drive(GameDisk_Save);
        int16_t handle;
        do {
            handle=Get_FileHandle((const uint8_t *)"INFOCOM.CMP",DOSFileMode_ReadBinary);
            if(handle==-1) Request_Game_Disk(RequestedGameDiskNumber);
        } while(handle==-1);
        DOS_close_file((uint16_t)handle);
        handle=Get_FileHandle(SaveGameFileName,DOSFileMode_ReadBinary);
        if(handle==-1) {
            Draw_Top_Graphic_Sidebar();
            Display_Text_From_Memory((uint8_t *)"Load game failed! File ");
            TextColour=EGA_Green; Display_Text_From_Memory(SaveGameFileName);
            Display_Text_From_Memory((uint8_t *)"\x06\x0F not found. Press a key.");
            Keyboard_Get_ASCII_Hex_Input();
        } else {
            uint8_t marker;
            /* Sol:334F supplies SS:BP-2; after the read it compares that BYTE, not AX.
             * A failed read leaves native stack residue. Do not turn that
             * into an invalid-marker default or a host undefined read. */
            if(DOS_read_file_handler((uint16_t)handle,&marker,1)!=1) {
                fputs("Unresolved original0800:32B3 unread save-marker stack byte\n",stderr);
                abort(); /* Explicit unsupported path, NOT original behaviour. */
            }
            if(marker!=OriginalSaveFormatMarker) {
                Draw_Top_Graphic_Sidebar();
                Display_Text_From_Memory((uint8_t *)"Game saved is invalid. Use only games saved from this version.");
                Keyboard_Get_ASCII_Hex_Input(); /*no close: original bug*/
            } else {
                CurrentMap=Map_Citadel; ViewedHolodisk=FALSE;
                DOS_read_file_handler((uint16_t)handle,OriginalSavedState.bytes,OriginalSavedStateBytes);
                DOS_read_file_handler((uint16_t)handle,&CrescentHawkMapPositionX,sizeof(uint16_t));
                DOS_read_file_handler((uint16_t)handle,&CrescentHawkMapPositionY,sizeof(uint16_t));
                DOS_close_file((uint16_t)handle);
                for(uint16_t code=0;code<CacheSecurityCodeCount;++code) CacheSecurityCodeUsed[code]=FALSE;
                MenuControls[OuttakeFrequencyMenu].selection=(uint16_t)(int16_t)(int8_t)OuttakeFrequency;
                if(KuritaDestroyedCitadel!=FALSE) CurrentMap=Map_DestroyedCitadel;
                if(HasViewedHolodisk!=FALSE) ViewedHolodisk=ViewedHolodiskCacheMarker;
                if(InsideStarLeagueCache!=FALSE) Draw_STARLEAG_ICN_AND_Game_Logic();
                else {
                    uint16_t centre=(uint16_t)((CrescentHawkMapPositionX|CrescentHawkMapPositionY)>>8);
                    Map_Construct_Nine_Regions(centre);
                    int16_t region=(int16_t)(centre-MapRegionPreviousRowAndColumn);
                    for(uint16_t row=0;row<MapNeighbourhoodWidth;++row) {
                        for(uint16_t column=0;column<MapNeighbourhoodWidth;++column) {
                            int16_t neighbour=(int16_t)(uint16_t)(region+column);
                            if(neighbour>=0 && neighbour<WorldRegionCount && MapFileByWorldRegion[neighbour]!=0)
                                DOS_Load_Map_Files(row*MapNeighbourhoodWidth+column,(uint16_t)(int16_t)(int8_t)MapFileByWorldRegion[neighbour]);
                        }
                        region=(int16_t)(uint16_t)(region+MapRegionRowStride);
                    }
                    Map_NineGrid_Parent();
                }
            }
            /* Invalid markers also reach this reset, despite not loading data. */
            for(uint16_t mech=0;mech<LanceSize;++mech) {
                CombatantSpriteFamilyOffset[mech]=Mechs[mech].name[0]=='L'?MECH_Sprite_LOCUST:MECH_Sprite_COMMANDO;
                CombatantSpriteFrame[mech]=0; CombatantMovementDirection[mech]=0; CombatantAnimationSelector[mech]=0;
                CombatantAnimationCursors[mech]=(AnimationCursor){WalkAnimationStreams,WalkAnimationStreamNativeOffset,WalkAnimationStreamNativeOffset};
            }
            TraitorWarning=FALSE;
            if(InsideStarLeagueCache==FALSE && TilesetId==Tileset_StarLeagueCache) Load_And_Draw_BTTLTECH_ICN();
        }
    }
    Menu_Draw_MultiSelect(FALSE); Draw_Health_and_C_Bills_Sidebar(TRUE);
    Select_Game_Disk_And_Drive(GameDisk_First);
}
