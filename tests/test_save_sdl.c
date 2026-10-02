/* Test-only UI/map boundaries. Save/load and DOS-to-SDL file calls are real.
 * Run exclusively in the synthetic fixture directory, never beside game saves. */
#define _CRT_SECURE_NO_WARNINGS
#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint16_t slot;
static unsigned reconstructed;
static void verify(int ok,unsigned line)
{ if(!ok) { fprintf(stderr,"SDL save roundtrip failed at line %u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
void Menu_Memory_Variables(uint16_t layout) { check(layout==3); }
void Draw_Top_Graphic_Sidebar(void) { }
void Display_Text_From_Memory(uint8_t *text) { check(text!=NULL); }
uint16_t Display_Menu_Choices_And_Check(uint16_t menu) { check(menu==SaveSlotMenu); return slot; }
void Select_Game_Disk_And_Drive(uint16_t disk)
{ check(disk==GameDisk_Save || disk==GameDisk_First); }
uint16_t Request_Game_Disk(uint16_t disk) { (void)disk; check(0); return FALSE; }
void Draw_Message_Box(void) { check(0); }
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text)
{ (void)text; check(0); }
void Map_Construct_Nine_Regions(uint16_t centre) { check(centre==0xCC); ++reconstructed; }
void DOS_Load_Map_Files(uint16_t cell,uint16_t map) { (void)cell; (void)map; check(0); }
void Map_NineGrid_Parent(void) { check(reconstructed==1); }
void Draw_STARLEAG_ICN_AND_Game_Logic(void) { check(0); }
void Load_And_Draw_BTTLTECH_ICN(void) { check(0); }
void Menu_Draw_MultiSelect(uint16_t redraw) { check(redraw==FALSE); }
void Draw_Health_and_C_Bills_Sidebar(uint16_t redraw) { check(redraw==TRUE); }
static void requireAbsent(const char *path)
{
    FILE *existing=fopen(path,"rb");
    if(existing) { fclose(existing); fprintf(stderr,"Refusing to overwrite %s\n",path); check(0); }
}
int main(void)
{
    uint8_t expected[OriginalSavedStateBytes],disk[OriginalSavedStateBytes+5];
    FILE *file;
    requireAbsent("INFOCOM.CMP");
    for(slot=0;slot<6;++slot) {
        SaveGameFileName[4]=(uint8_t)('1'+slot);
        requireAbsent((const char *)SaveGameFileName);
    }
    /* Only the original disk-presence check opens this synthetic sentinel. */
    file=fopen("INFOCOM.CMP","wb"); check(file!=NULL);
    check(fputc(0,file)==0); check(fclose(file)==0);
    for(slot=0;slot<6;++slot) {
        for(unsigned index=0;index<sizeof expected;++index)
            expected[index]=(uint8_t)(index*37+slot*19);
        memcpy(OriginalSavedState.bytes,expected,sizeof expected);
        CacheMapRoomLoaded=FALSE; InsideStarLeagueCache=FALSE;
        memcpy(expected,OriginalSavedState.bytes,sizeof expected);
        CrescentHawkMapPositionX=0x0C35; CrescentHawkMapPositionY=0xC047;
        Save_Game();
        /* Native O_CREAT lacks O_TRUNC. A second save must retain an old tail. */
        file=fopen((const char *)SaveGameFileName,"ab"); check(file!=NULL);
        check(fwrite("TAIL",1,4,file)==4); check(fclose(file)==0);
        Save_Game();
        file=fopen((const char *)SaveGameFileName,"rb"); check(file!=NULL);
        check(fread(disk,1,sizeof disk,file)==sizeof disk);
        for(unsigned index=0;index<4;++index) check(fgetc(file)=="TAIL"[index]);
        check(fgetc(file)==EOF); check(fclose(file)==0);
        check(disk[0]==OriginalSaveFormatMarker);
        check(!memcmp(disk+1,expected,sizeof expected));
        check(disk[sizeof expected+1]==0x35 && disk[sizeof expected+2]==0x0C);
        check(disk[sizeof expected+3]==0x47 && disk[sizeof expected+4]==0xC0);
        memset(OriginalSavedState.bytes,0xCD,sizeof OriginalSavedState.bytes);
        memset(MapFileByWorldRegion,0,sizeof MapFileByWorldRegion);
        CrescentHawkMapPositionX=CrescentHawkMapPositionY=0xFFFF;
        TilesetId=0; reconstructed=0;
        Load_Game();
        check(reconstructed==1);
        check(!memcmp(OriginalSavedState.bytes,expected,sizeof expected));
        check(CrescentHawkMapPositionX==0x0C35 && CrescentHawkMapPositionY==0xC047);
        /* A supplied valid marker is distinct from an unread marker. Native
         * payload reads ignore counts: EOF retains existing state/camera tails.
         * This deliberately preserves that original failure behaviour. */
        file=fopen((const char *)SaveGameFileName,"wb"); check(file!=NULL);
        check(fwrite(disk,1,65,file)==65); check(fclose(file)==0);
        memset(OriginalSavedState.bytes,0,sizeof OriginalSavedState.bytes);
        memset(expected,0,sizeof expected); memcpy(expected,disk+1,64);
        CrescentHawkMapPositionX=0x0C99; CrescentHawkMapPositionY=0xC088;
        reconstructed=0;
        Load_Game();
        check(reconstructed==1 && !memcmp(OriginalSavedState.bytes,expected,sizeof expected));
        check(CrescentHawkMapPositionX==0x0C99 && CrescentHawkMapPositionY==0xC088);
        check(remove((const char *)SaveGameFileName)==0);
    }
    check(remove("INFOCOM.CMP")==0);
    puts("Six original save/load slots roundtrip through real SDL file redirects.");
    return 0;
}
