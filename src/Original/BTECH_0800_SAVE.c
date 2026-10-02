#include "game.h"
#include "dos.h"

/* Original0800:35D3..378C. Sol: Save marker, native persistent block and
 * packed camera coordinates. Continue all four writes after any failure;
 * only the Cache map room prohibits saving. Original slot/menu/media paths. */
void Save_Game(void)
{
    if(CacheMapRoomLoaded!=FALSE) {
        Draw_Message_Box();
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput(
            (uint8_t *)"You can't save the game inside the map room.");
        Keyboard_Get_ASCII_Hex_Input();
    } else {
        Menu_Memory_Variables(3); Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"Save Game:\rOne\rTwo\rThree\rFour\rFive\rSix\rCancel");
        uint16_t slot=Display_Menu_Choices_And_Check(SaveSlotMenu);
        if(slot!=SaveSlotCancel) {
            SaveGameFileName[4]=(uint8_t)(slot+'1');
            uint16_t failed=FALSE;
            Select_Game_Disk_And_Drive(GameDisk_Save);
            int16_t handle;
            do {
                handle=Get_FileHandle((const uint8_t *)"INFOCOM.CMP",DOSFileMode_ReadBinary);
                if(handle==-1) Request_Game_Disk(RequestedGameDiskNumber);
            } while(handle==-1);
            DOS_close_file((uint16_t)handle);
            handle=Get_FileHandle(SaveGameFileName,DOSFileMode_WriteCreateBinary,DOSFileCreateOwnerReadWrite);
            if(handle==-1) failed=TRUE;
            else {
                uint8_t marker=OriginalSaveFormatMarker;
                if(DOS_write_memory_to_save_file((uint16_t)handle,&marker,1)!=1) failed=TRUE;
                if(DOS_write_memory_to_save_file((uint16_t)handle,OriginalSavedState.bytes,OriginalSavedStateBytes)!=OriginalSavedStateBytes) failed=TRUE;
                if(DOS_write_memory_to_save_file((uint16_t)handle,&CrescentHawkMapPositionX,sizeof(uint16_t))!=sizeof(uint16_t)) failed=TRUE;
                if(DOS_write_memory_to_save_file((uint16_t)handle,&CrescentHawkMapPositionY,sizeof(uint16_t))!=sizeof(uint16_t)) failed=TRUE;
                DOS_close_file((uint16_t)handle);
            }
            if(failed!=FALSE) {
                Draw_Top_Graphic_Sidebar();
                TextColour=GraphicsAdapter==0?EGA_Green:EGA_Red; /*native adapter0 exception*/
                Display_Text_From_Memory((uint8_t *)"Save game failed! Check your disk.\x06\x0F");
                Keyboard_Get_ASCII_Hex_Input();
            }
        }
    }
    Menu_Draw_MultiSelect(FALSE);
    Draw_Health_and_C_Bills_Sidebar(TRUE);
    Select_Game_Disk_And_Drive(GameDisk_First);
}
