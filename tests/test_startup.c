#include "game.h"
#include "dos.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test-only dependency boundaries. Setup_Game and the packed-plane conversion
 * are real original code; this is sequencing validation, not a playable game. */
static unsigned keys, loads, decodes, uploads, tilesets, captures;
static unsigned retraces, drains, titles, starts, restores, characters;
static int cancelSplash;
static uint8_t border[1], tinyland[1];
static const char *const filenames[]={"INFOCOM.CMP","BTBORDER.CMP","TINYLAND.CMP","MECHSHAP.CMP"};
void DrawCall_SingleTile(uint8_t *tile,uint16_t x,uint16_t y)
{
    (void)tile; (void)x; (void)y;
    assert(!"Startup should not draw menu borders");
}
void Platform_Write_Startup_Character(int16_t character)
{
    assert(character != 0);
    ++characters;
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void)
{
    ++keys;
    assert(!"Installed SDL startup must not request a DOS drive count");
    return 0;
}
void Initialize_Graphics_Runtime(void)
{
    assert(GraphicsAdapter==GraphicsAdapter_Ega);
    assert(keys==0 && HasHardDisk==TRUE && SecondFloppyDriveAvailable==FALSE);
}
void Select_Game_Disk_And_Drive(uint16_t disk)
{
    assert(disk==GameDisk_Second && loads==0);
}
uint16_t Load_File_To_Memory(const uint8_t *name,uint8_t *memory)
{
    assert(loads<4 && !strcmp((const char *)name,filenames[loads]));
    assert(memory==GraphicsSceneWorkspace && decodes==loads);
    if (loads!=0) assert(titles==1);
    ++loads;
    return 0;
}
void Decompress_File_Into_Memory(uint8_t *compressed,uint8_t *graphics)
{
    assert(compressed==GraphicsSceneWorkspace && graphics==GraphicsFileWorkspace);
    assert(loads==decodes+1);
    ++decodes;
    /* Eight packed pixels 1,2,3,4,5,6,7,8: independent expected planes below. */
    for (unsigned i=0;i<PackedGraphicsOutputBytes;i+=4) {
        graphics[i]=0x12; graphics[i+1]=0x34;
        graphics[i+2]=0x56; graphics[i+3]=0x78;
    }
}
void DrawCall_Image_To_VGA_Memory(uint8_t *image,uint16_t segment)
{
    assert(image==GraphicsFileWorkspace && segment==EgaSceneStagingSegment);
    assert(decodes==uploads+1 && uploads<3);
    ++uploads;
}
void Set_Palette_registers(const uint8_t *palette)
{
    assert(palette==DefaultEgaPalette && uploads==1);
}
void Draw_GraphicsFile_In_Memory(uint8_t *image,uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
    assert(image==GraphicsFileWorkspace && x==0 && y==0 && width==40 && height==200);
}
void Drain_Pending_Keyboard_Input(void) { ++drains; }
void Wait_For_N_Vertical_Retraces(uint16_t count)
{
    assert(count==1 && drains==1); ++retraces;
}
uint16_t Pending_Input(void) { return (uint16_t)(cancelSplash && retraces==3); }
void Load_And_Draw_BTTITLE_CMP(void)
{
    assert(drains==2 && retraces==(cancelSplash?3u:700u)); ++titles;
}
uint8_t *Create_TileSet_Array(uint8_t *display,uint16_t x,uint16_t y,uint16_t count)
{
    assert(display==GraphicsFileWorkspace && x==0 && y==0);
    assert(uploads==tilesets+2);
    assert(count==(tilesets==0?8:66));
    return tilesets++==0?border:tinyland;
}
void Capture_Combat_Sprite(uint16_t id,uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
    unsigned expectedX,expectedY,expectedWidth=1,expectedHeight=8;
    assert(id==captures && id<376 && decodes==4 && tilesets==2);
    assert(GraphicsFileWorkspace[0]==0xAA && GraphicsFileWorkspace[1]==0x66);
    assert(GraphicsFileWorkspace[2]==0x1E && GraphicsFileWorkspace[3]==0x01);
    /* ID-based oracle, independent of the startup loop variables. */
    if (id<16) {
        expectedX=3*(id%12); expectedY=id<12?0:24; expectedWidth=3; expectedHeight=24;
    } else if (id<36) { expectedX=id-4; expectedY=24;
    } else if (id<120) { expectedX=12+(id-36)%12; expectedY=72+8*((id-36)/12);
    } else if (id<130) {
        static const uint16_t geometry[10][4]={
            {0,96,3,24},{3,96,3,24},{9,120,3,24},{6,120,3,24},
            {8,160,2,14},{10,160,2,14},{0,160,1,8},{1,160,1,8},
            {8,144,2,16},{10,144,2,16}};
        const uint16_t *g=geometry[id-120];
        expectedX=g[0]; expectedY=g[1]; expectedWidth=g[2]; expectedHeight=g[3];
    } else if (id<146) { expectedX=4+(id-130)%4; expectedY=144+8*((id-130)/4);
    } else if (id<162) {
        expectedX=3*((id-146)%12); expectedY=id<158?48:72; expectedWidth=3; expectedHeight=24;
    } else if (id<166) {
        static const uint16_t geometry[4][2]={{6,96},{9,96},{0,120},{3,120}};
        expectedX=geometry[id-162][0]; expectedY=geometry[id-162][1]; expectedWidth=3; expectedHeight=24;
    } else if (id<186) { expectedX=id-154; expectedY=32;
    } else if (id<270) { expectedX=24+(id-186)%12; expectedY=72+8*((id-186)/12);
    } else if (id<290) { expectedX=id-258; expectedY=40;
    } else if (id<374) { expectedX=12+(id-290)%12; expectedY=128+8*((id-290)/12);
    } else { expectedX=2*(id-374); expectedY=144; expectedWidth=2; expectedHeight=11; }
    assert(x==expectedX && y==expectedY && width==expectedWidth && height==expectedHeight);
    ++captures;
}
void Start_Game(void)
{
    assert(captures==376 && BorderTileset==border && TinylandTileset==tinyland);
    ++starts;
}
void Restore_BIOS_Text_Mode(void) { assert(starts==1); ++restores; }
int main(void)
{
    keys=loads=decodes=uploads=tilesets=captures=0;
    retraces=drains=titles=starts=restores=characters=0;
    cancelSplash=0;
    Setup_Game();
    assert(starts==1 && restores==1 && characters>50);
    puts("Installed hard-disk startup, splash timing, asset order and all 376 sprite rectangles passed");
    return 0;
}
