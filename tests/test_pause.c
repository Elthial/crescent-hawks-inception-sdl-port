#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint16_t selection,settingChoice=5;
static unsigned action,sidebars,borders,redraws;
static char displayed[512];
void Draw_Top_Graphic_Sidebar(void) { ++sidebars; }
void Draw_Menu_Border(uint16_t style) { assert(style==PauseMenuPanel); ++borders; }
void Display_Text_From_Memory(uint8_t *text) {
    size_t used=strlen(displayed),count=strlen((const char *)text)+1;
    assert(used+count<=sizeof displayed); memcpy(displayed+used,text,count);
}
uint16_t Display_Menu_Choices_And_Check(uint16_t menu) {
    if(menu==GameSettingsMenu) { assert(!action); action=1; return settingChoice; }
    assert(menu==PauseMenuPanel); return selection;
}
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { (void)refresh; assert(0); }
uint16_t Prompt_Yes_No(uint16_t defaultAnswer) { (void)defaultAnswer; assert(0); return 0; }
void Menu_Assign_Pilots(void) { assert(!action); action=2; }
void Inspect_Characters(void) { assert(!action); action=3; }
void Heal_Characters(uint16_t service) { assert(!action && service==MedicalService_UsePartyMedicAndEquipment); action=4; }
void Load_Game(void) { assert(!action); action=5; }
void Save_Game(void) { assert(!action); action=6; }
void Show_Overhead_Map(void) { assert(!action); action=7; }
void EGA_DrawBox_Wrapper(void) { ++redraws; }

static void run(uint16_t choice,unsigned expected,int hasMech)
{
    selection=choice; action=sidebars=borders=redraws=0; displayed[0]=0;
    Game_Pause_Menu();
    assert(action==expected && sidebars==(expected==1?2u:1u) && borders==1 && redraws==1);
    assert(MenuControls[PauseMenuPanel].optionCount==(hasMech?8:7));
    assert(MenuPanelLayouts[PauseMenuPanel].top==(hasMech?12:13));
    assert(MenuPanelLayouts[PauseMenuPanel].height==(hasMech?8:7));
    assert(TextPanelTop==(hasMech?12:13) && CurrentMenuLayoutIndex==PauseMenuPanel);
    assert(strstr(displayed,"Return to game\rChange game settings")!=NULL);
    assert((strstr(displayed,"\rAllocate men in 'Mechs")!=NULL)==hasMech);
    assert(strstr(displayed,"\rShow Overhead Map")!=NULL);
}
int main(void)
{
    for(unsigned mech=0;mech<MechRecordCount;++mech) Mechs[mech].name[0]=MECH_Destroyed;
    Mechs[LanceSize].name[0]='E'; /* Enemy presence must not add allocation. */
    run(0,0,0); run(1,1,0);
    for(uint16_t choice=2;choice<=6;++choice) run(choice,choice+1,0);
    run(7,0,0); run(65535,0,0);
    for(unsigned mech=0;mech<LanceSize;++mech) {
        Mechs[mech].name[0]=0; /* Native presence test is !=FF, not nonempty name. */
        run(0,0,1);
        for(uint16_t choice=1;choice<=7;++choice) run(choice,choice,1);
        run(8,0,1); run(65535,0,1);
        Mechs[mech].name[0]=MECH_Destroyed;
    }
    run(0,0,0); /* Geometry is reset on next opening, not left expanded. */
    /* Actual pause->settings->pause return, not a substitute settings action. */
    settingChoice=GameSettings_ToggleSound; SoundEffectsEnabled=FALSE;
    run(PauseMenu_Settings,1,0); assert(SoundEffectsEnabled==TRUE);
    run(PauseMenu_Settings,1,0); assert(SoundEffectsEnabled==FALSE);
    puts("Original pause dispatch verified for both layouts and every friendly slot.");
    return 0;
}
