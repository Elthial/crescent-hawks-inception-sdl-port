#include "game.h"
#include "dos.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Sol: real parent, layout, copy/formatter, tables and gauge helper.
 * The layout matrix stops at the FIRST retrace. Additional continuation
 * scenarios check portable palette timing, auto-dismiss and input return.
 * It is not a decoded full-screen or SDL pixel test.
 * Asset decoding/upload, text display and hardware rectangles are isolated. */
static jmp_buf firstRetrace;
static unsigned loaded,decoded,uploaded,blitted,rectangles,texts;
static unsigned recordId;
static unsigned continuation,retraces,paletteUpdates,paletteRestores,keyReads,pendingReads;
static uint8_t startingPaletteColour;
static struct { char text[80]; uint16_t column,row,colour; } output[64];
static void verify(int condition,unsigned line)
{
    if(!condition) { fprintf(stderr,"Mech statistics parent mismatch line%u\n",line); exit(1); }
}
#define check(x) verify(!!(x),__LINE__)
void Draw_Top_Graphic_Sidebar(void) {}
void Select_Game_Disk_And_Drive(uint16_t disk) { check(disk==1); }
uint16_t Load_File_To_Memory(const uint8_t *filename,uint8_t *memory)
{
    check(strcmp((const char *)filename,"BTSTATS.CMP")==0 && memory==BTStatsOrBldMemory);
    check(BTStatsAssetLoaded==TRUE); ++loaded; return 0;
}
void Decompress_File_Into_Memory(uint8_t *source,uint8_t *destination)
{
    check(source==BTStatsOrBldMemory && destination==GraphicsFileWorkspace);
    check(GraphicsCompatibilityFlag==FALSE); ++decoded;
}
void DrawCall_Image_To_VGA_Memory(uint8_t *image,uint16_t destination)
{
    check(image==GraphicsFileWorkspace && destination==EgaSceneStagingSegment);
    check(decoded==1); ++uploaded;
}
void EGA_DrawBox_Operation(EgaMemoryAddress source,EgaMemoryAddress destination,
    uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
    check(source.segment==EgaSceneStagingSegment && destination.segment==EgaScreenSegment);
    check(source.offset==0 && destination.offset==0 && x==0 && y==0);
    check(width==40 && height==200 && uploaded==1); ++blitted;
}
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t column,uint16_t row,
    uint16_t foreground,uint16_t background)
{
    size_t length=strlen((char *)text);
    check(texts<64 && length<sizeof output[0].text && background==EGA_Black);
    memcpy(output[texts].text,text,length+1);
    output[texts].column=column; output[texts].row=row; output[texts++].colour=foreground;
}
void Draw_Horizontal_EGA_Line(uint16_t left,uint16_t top,uint16_t right,uint16_t bottom,uint16_t colour)
{
    check(left<=right && (right-left==2 || right-left==5));
    check(colour==2 || colour==4 || colour==14);
    /* Reversed bounds are permitted: BUG-006 reaches the primitive unchanged. */
    (void)top; (void)bottom; ++rectangles;
}
void Wait_For_N_Vertical_Retraces(uint16_t count)
{
    check(count==1 && decoded==1 && uploaded==1 && blitted==1 && rectangles>0);
    ++retraces;
    if(!continuation) longjmp(firstRetrace,1);
}
void Set_Palette_registers(const uint8_t *palette)
{
    check(continuation);
    if(palette==DefaultEgaPalette) { ++paletteRestores; return; }
    check(palette==BTStatsEgaPalette && retraces%BTStatsPaletteRefreshRedraws==0);
    /* Native applies the old table BEFORE patching the next phase. */
    check(palette[BTStatsCyclingPaletteEntry]==(paletteUpdates?
        BTStatsEgaRedCycle[paletteUpdates%BTStatsPaletteCycleCount]:startingPaletteColour));
    ++paletteUpdates;
}
uint16_t Pending_Input(void) { check(continuation==2); ++pendingReads; return retraces>=3; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { check(continuation); ++keyReads; return 13; }
static unsigned find(uint16_t column,uint16_t row,const char *text)
{
    for(unsigned i=0;i<texts;++i)
        if(output[i].column==column && output[i].row==row && strcmp(output[i].text,text)==0) return i;
    check(FALSE); return 0;
}
static void run(unsigned id,int heat,unsigned cached,unsigned hasRider)
{
    uint16_t combatantId=(uint16_t)(id<LanceSize?id:id+8);
    Mech *mech=&Mechs[id];
    memset(mech,0,sizeof *mech);
    memcpy(mech->name,"CommandoLong",13);
    mech->tonnage=25; mech->pilotId=2; mech->riderId=(uint8_t)(hasRider?3:MECH_NoRider);
    if(id>=LanceSize) { mech->pilotId=255; mech->riderId=254; } /*Unknown branch must not dereference crew*/
    mech->engineHeatSinks=8;
    memset(mech->currentArmour,10,sizeof mech->currentArmour);
    memset(mech->currentStructure,5,sizeof mech->currentStructure);
    mech->currentActuators[0]=0xF0; mech->currentActuators[1]=0x06;
    mech->maxActuators[0]=0xF0; mech->maxActuators[1]=0;
    /* Each native display-label boundary, including destroyed weapon colour. */
    const unsigned offsets[]={0x33,0x3A,0x41,0x48,0x4F,0x51,0x53,0x55};
    const char *labels[]={"LA","LT","RA","RT","LL","RL","CT","H"};
    for(unsigned i=0;i<8;++i) ((uint8_t *)mech)[offsets[i]]=(uint8_t)(Mech_Small_Laser|(i==3?0x80:0));
    mech->criticalSlots[1]=Heat_Sink; mech->criticalSlots[2]=Destroyed_Heat_Sink;
    Characters[2].name=0;
    Characters[3].name=1;
    MechHeatLevel[id]=(int8_t)heat;
    BTStatsAssetLoaded=(uint16_t)cached; GraphicsCompatibilityFlag=TRUE;
    MechHeatGaugeFlashCountdown=10; MechHeatGaugeFlashColour=0;
    loaded=decoded=uploaded=blitted=rectangles=texts=0;
    recordId=id;
    if(setjmp(firstRetrace)==0) Examine_Screen_BTSTATS_CMP(combatantId);
    /* Values used after longjmp are globals or unchanged pre-setjmp locals. */
    check(recordId==id && loaded==(cached?0u:1u) && BTStatsAssetLoaded==TRUE);
    check(CurrentMenuLayoutIndex==4 && TextColour==EGA_BrightYellow);
    check(MechHeatLevel[id]==(heat<0?0:heat>30?30:heat));
    find(6,0,"Commando"); find(6,1,"25");
    if(id<LanceSize) {
        find(6,2,(char *)CharacterNames[0]);
        find(6,3,hasRider?(char *)CharacterNames[1]:"None");
    }
    else find(6,2,"Unknown\rUnknown");
    for(unsigned i=0;i<8;++i) {
        unsigned line=find(11,(uint16_t)(6+i),labels[i]);
        check(output[line].colour==(i==3?EGA_DarkGrey:EGA_BrightYellow));
        find(0,(uint16_t)(6+i),(char *)WeaponStats[Mech_Small_Laser-1].name);
    }
    find(10,20,"Gone"); find(10,21,"Hit"); find(10,22," OK"); find(10,23," OK");
    /* Cold heat skips the flash entirely; all other clamped values update it. */
    check(MechHeatGaugeFlashCountdown==(heat<=0?10:9));
    check(rectangles==(heat<=0 || heat>=30?37u:38u));
}
int main(void)
{
    static const int heats[]={-128,-1,0,10,30,31,127};
    for(unsigned id=0;id<MechRecordCount;++id)
        for(unsigned h=0;h<sizeof heats/sizeof heats[0];++h)
            for(unsigned cached=0;cached<=1;++cached)
                for(unsigned hasRider=0;hasRider<=1;++hasRider) run(id,heats[h],cached,hasRider);
    /* Continue the actual parent beyond the former guard: automatic600-retrace
     * dismissal and interactive input restore the palette and consume one key.
     * Hardware and asset boundaries stay isolated; this is not pixel validation. */
    for(unsigned mode=1;mode<=2;++mode) {
        continuation=mode; DisableInput=(uint16_t)(mode==1);
        loaded=decoded=uploaded=blitted=rectangles=texts=0;
        retraces=paletteUpdates=paletteRestores=keyReads=pendingReads=0;
        startingPaletteColour=BTStatsEgaPalette[BTStatsCyclingPaletteEntry];
        Examine_Screen_BTSTATS_CMP(0);
        check(retraces==(mode==1?(unsigned)BTStatsAutoDismissRedraws:3u));
        check(paletteUpdates==(mode==1?(unsigned)(BTStatsAutoDismissRedraws/BTStatsPaletteRefreshRedraws):0u));
        check(paletteRestores==1 && keyReads==1 && pendingReads==(mode==1?0u:3u));
    }
    puts("Original stats parent: 224 first-frame probes and portable palette/input continuation passed.");
    return 0;
}
