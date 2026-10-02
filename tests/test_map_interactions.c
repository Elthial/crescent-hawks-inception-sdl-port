#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned calls,kind,prompts,buildings,texts;
static uint16_t seenX,seenY;
static void verify(int ok,unsigned line) { if(!ok){fprintf(stderr,"Interaction mismatch line%u\n",line);exit(1);} }
#define check(x) verify(!!(x),__LINE__)
static void record(unsigned action,uint16_t x,uint16_t y){++calls;kind=action;seenX=x;seenY=y;}
void Draw_STARLEAG_ICN_Scene(void){record(1,0,0);}
void Map_Interactable_Play_Sound(uint16_t x,uint16_t y){record(2,x,y);}
void Cache_StarMap_CorrectPassword(void){record(3,0,0);}
void StarLeague_Key_Codes(uint16_t x,uint16_t y,int16_t door){check(door==CacheDoorLookupByPosition);record(4,x,y);}
void StarLeague_Secret_Passageway_Discovered(void){record(5,0,0);}
void StarLeague_Security_Terminal(uint16_t x,uint16_t y){record(6,x,y);}
void StarLeague_Cache_PhoenixHawk(void){record(7,0,0);}
void Display_StarLeague_Cache_Dialog_Window(void){record(8,0,0);}
void HPGTransmitter(uint16_t x,uint16_t y){record(9,x,y);}
void StarLeague_HyperPulse_Power_Dialog_Window(uint16_t x,uint16_t y){record(10,x,y);}
void StarLeague_Map_Room(uint16_t x,uint16_t y){record(11,x,y);}
void Draw_Message_Box(void){check(MessageBoxOpen==TRUE);++prompts;}
void Display_Text_From_Memory(uint8_t *text){check(text!=NULL);++texts;}
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text){check(!strcmp((char *)text,"The water is too deep that way."));++texts;}
uint16_t Prompt_Yes_No(uint16_t yes){check(yes==TRUE);return TRUE;}
void Interact_with_BLD(uint16_t building){check(building==0);++buildings;}
static void reset(void){
    memset(&MapRuntime,0,sizeof MapRuntime);
    memset(CombatantActive,0,sizeof CombatantActive);
    memset(MapInteractablePositionX,0xFF,sizeof MapInteractablePositionX);
    memset(MapInteractablePositionY,0xFF,sizeof MapInteractablePositionY);
    calls=kind=prompts=buildings=texts=0;seenX=seenY=0;
    CachedMapOriginIndex=52;InsideStarLeagueCache=TRUE;CacheMapRoomLoaded=FALSE;
    MessageBoxOpen=FALSE;BlockingTileCodeThreshold=0x28;
}
int main(void){
    for(unsigned room=0;room<2;++room) for(unsigned tile=0;tile<256;++tile){
        reset();CacheMapRoomLoaded=(uint8_t)room;CombatantActive[4]=0x8000;
        MapCache.tiles[202]=(uint8_t)tile;
        unsigned expected=0;
        if(tile>=0x28){
            if(room){if(tile==0x94)expected=1;else if(tile>=0x97 && tile<=0xF0)expected=2;else if(tile==0x8C || tile==0x8D)expected=3;}
            else {if(tile>=0x7E && tile<=0x80)expected=4;else if(tile==0xB6 || tile==0xB7)expected=6;
                else if(tile>=0xF6)expected=7;else if(tile==0x83 || (tile>=0xA5 && tile<=0xA7))expected=8;
                else if(tile==0x4D)expected=9;else if(tile==0x3A || tile==0x3D)expected=10;else if(tile==0x28)expected=11;}
        }
        check(Map_Interactables_Building_Or_Items(0,0)==(uint16_t)(tile>=0x28));
        check(calls==(unsigned)(expected!=0) && kind==expected && !prompts);
        if(expected==4)check(seenX==(uint16_t)(0-(tile-0x7E)*2) && seenY==0);
        if(expected==2 || expected==6 || expected==9 || expected==10 || expected==11)check(seenX==0 && seenY==0);
    }
    reset();CombatantActive[0]=0x0100;MapCache.tiles[179]=0x28;
    check(Map_Interactables_Building_Or_Items(0,0)==TRUE && !calls);
    /* Even with a blocked Mech footprint, native code still offers a doorway. */
    reset();CombatantActive[0]=TRUE;MapCache.tiles[178]=0x28;
    CombatantPackedX[0]=0;CombatantPackedY[0]=7;MapInteractablePositionX[0]=0xFFFF;MapInteractablePositionY[0]=7;
    check(Map_Interactables_Building_Or_Items(0,0)==TRUE && buildings==1 && prompts==1 && texts==3);
    reset();InsideStarLeagueCache=FALSE;CombatantActive[4]=TRUE;
    CombatantPackedX[4]=9;CombatantPackedY[4]=7;MapInteractablePositionX[0]=9;MapInteractablePositionY[0]=7;
    check(Map_Interactables_Building_Or_Items(0,0)==FALSE && buildings==1 && prompts==1);
    /* Deep-water boundary refusal happens before any actor scan. */
    const int16_t dx[8]={0,0,-1,1,-1,1,-1,1},dy[8]={-1,1,0,0,-1,-1,1,1};
    const unsigned neighbours[8]={1,7,3,5,0,2,6,8};
    for(unsigned direction=0;direction<8;++direction){
        reset();CrescentHawkMapPositionX=(uint16_t)(dx[direction]>0?15:0);CrescentHawkMapPositionY=(uint16_t)(dy[direction]>0?15:0);
        LocalTerrainFlags[neighbours[direction]]=15;
        check(Map_Interactables_Building_Or_Items(dx[direction],dy[direction])==TRUE && prompts==1 && texts==1 && !calls);
    }
    puts("Original walking/building/cache interaction tests passed.");return 0;
}
