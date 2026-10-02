#include "game.h"
#include "dos.h"

/* Original183B:2591..273C, full ASM checked. Actor ID, not a pointer.
 * Include/detail flag also determines whether the browser skips mech slots.
 * Native capability arithmetic is retained, not replaced by a target count. */
void Scan_Enemies(uint16_t scanningCombatantId)
{
    uint16_t includeMechDetails=TRUE;
    Menu_Memory_Variables(CombatSettingsMenuLayout);
    Draw_Top_Graphic_Sidebar();
    Display_Text_From_Memory((uint8_t *)"\x06\x0FScan...\rFriends\rEnemies\rCancel"); /*DS3D73*/
    MenuControls[CombatSettingsMenuLayout].baseRow=ScanMenuFirstChoiceRow;
    MenuControls[CombatSettingsMenuLayout].optionCount=ScanSideChoiceCount;
    MenuControls[CombatSettingsMenuLayout].selection=ScanSide_Friends;
    int16_t side=(int16_t)Display_Menu_Choices_And_Check(CombatSettingsMenuLayout);
    uint16_t canBrowse=TRUE;
    if(side==ScanSide_Enemies) {
        Menu_Memory_Variables(RefreshLowerPanel);
        Draw_Top_Graphic_Sidebar();
        if((int16_t)scanningCombatantId>=Friendly_Infantry_Combatant_Range_First) {
            uint16_t enemyMechPresent=FALSE;
            for(uint16_t id=Enemy_All_CombatantId_Range_First;id<Enemy_Infantry_CombatantId_Range_First;++id)
                if(CombatantActive[id]) enemyMechPresent=TRUE;
            if(enemyMechPresent) {
                Display_Text_From_Memory((uint8_t *)"You can only see and describe enemy humans.\rPress a key."); /*3D94*/
                (void)Keyboard_Get_ASCII_Hex_Input();
            }
            canBrowse=FALSE; includeMechDetails=FALSE;
        } else if(Mechs[scanningCombatantId].sensorHits==MechSensorMaximumHits) {
            /* Native test is EXACTLY two sensor hits, not >=two. */
            Display_Text_From_Memory((uint8_t *)"Because your sensors are destroyed you can't scan enemy mechs.\rPress a key."); /*3DCD*/
            includeMechDetails=FALSE;
            (void)Keyboard_Get_ASCII_Hex_Input();
            Draw_Top_Graphic_Sidebar();
        }
    }
    for(uint16_t id=Enemy_Infantry_CombatantId_Range_First;id<AllCombatantCount;++id)
        if(CombatantActive[id] && CombatantPackedX[id]!=CombatantPosition_Unused) canBrowse=TRUE;
    canBrowse+=includeMechDetails;
    if(!canBrowse && side==ScanSide_Enemies) {
        Menu_Memory_Variables(RefreshLowerPanel);
        Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory((uint8_t *)"There are no enemy humans in this battle."); /*3E19*/
        Prompt_And_Wait_For_Key();
    }
    Draw_Top_Graphic_Sidebar();
    if(side<0) side=ScanSide_Friends;
    if(side<ScanSide_Cancel && canBrowse)
        Combat_Browse_Scan_Targets(scanningCombatantId,
            (uint16_t)(Enemy_All_CombatantId_Range_First*side),includeMechDetails);
    MenuControls[CombatSettingsMenuLayout].baseRow=0;
    MenuControls[CombatSettingsMenuLayout].selection=
        (int16_t)scanningCombatantId<Friendly_Infantry_Combatant_Range_First?
        CombatMenu_MechScanChoice:CombatMenu_InfantryScanChoice;
}
