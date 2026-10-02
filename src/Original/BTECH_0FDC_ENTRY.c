#include "game.h"

/* EXE-owned3EDB:141A..1433. Scene BYTE is CBW at the call site. */
uint8_t EnterBuildingAnimations[BldFileCount]={0,17,0,21,21,18,0,0,18,0,20,19,0,0,0,21,0,0,0,0,0,0,0,0,0,0};

/* Original0FDC:0008..01BF. Requested building identity is saved before
 * resolving its map-specific script. Native lookup is signed; callers must
 * supply an address represented by the original alternate-building view.
 * The binary unconditionally skips its dormant copyright quiz. */
void Interact_with_BLD(uint16_t requestedBuilding)
{
    uint16_t script=requestedBuilding;
    BldInteractionFlag=TRUE; EnteredBuildingId=requestedBuilding;
    if(script!=Bld_Viewdisk && (int16_t)script<Bld_EndMech)
        script=(uint16_t)(int16_t)(int8_t)AlternativeBldByBuildingId[(int16_t)script];
    Select_Game_Disk_And_Drive(GameDisk_First);
    Load_And_Decode_Indexed_BLD(script);
    if(EnterBuildingAnimations[script]!=0)
        Display_Animation_Scene((uint16_t)(int16_t)(int8_t)EnterBuildingAnimations[script],BuildingEntryAnimationCallerRedraw);
    Execute_Bld_Bytecode(BTStatsOrBldMemory);
    Citadel_Building_Dialogs(CitadelDialog_CountOtherPartyMembers);
    if(HasViewedHolodisk==FALSE && script==Bld_Garage && HasHoloviewer!=FALSE && SelectedPartyMemberSlot!=0) {
        Menu_Memory_Variables(HolodiskPromptLayout); Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"When you get outside, you decide to use your new ");
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"holoviewer to watch your father's holodisk.");
        (void)Keyboard_Get_ASCII_Hex_Input();
        Interact_with_BLD(Bld_Viewdisk); ViewedHolodisk=HolodiskViewedMapMarker;
    }
    if(script==Bld_BarracksReturn && SelectedPartyMemberSlot!=0) {
        Interact_with_BLD(Bld_Viewdisk); ViewedHolodisk=HolodiskViewedMapMarker;
    }
    Menu_Memory_Variables(BuildingExitSidebarLayout); Draw_Top_Graphic_Sidebar();
    Menu_Memory_Variables(BuildingHealthSidebarLayout); Draw_Health_and_C_Bills_Sidebar(TRUE);
    if(script==Bld_CacheEntrance && PersistentState.fields.entranceBldState!=FALSE) {
        for(uint16_t row=0;row<CacheEntranceFogRowCount;++row)
            MapFogOfWar[(CacheEntranceFogFirstRow+row)*MapFogOfWarRowBytes+CacheEntranceFogColumn]=0;
        Draw_STARLEAG_ICN_AND_Game_Logic();
    }
}
