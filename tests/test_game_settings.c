#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint16_t mainChoice,submenuChoice,expectedDefault,quitAnswer;
static unsigned menuCalls,sidebars,healthDraws,prompts;
static char text[512];
void Draw_Top_Graphic_Sidebar(void) { ++sidebars; }
void Display_Text_From_Memory(uint8_t *value) {
    size_t used=strlen(text),count=strlen((const char *)value)+1;
    assert(used+count<=sizeof text); memcpy(text+used,value,count);
}
uint16_t Display_Menu_Choices_And_Check(uint16_t menu) {
    if(!menuCalls++) { assert(menu==GameSettingsMenu); return mainChoice; }
    assert(menuCalls==2);
    if(mainChoice==GameSettings_MovementRate) {
        assert(menu==MovementRateMenu);
        assert(MenuControls[menu].baseRow==TRUE);
        assert(MenuControls[menu].optionCount==3);
        assert(MenuControls[menu].selection==expectedDefault);
    } else assert(menu==(mainChoice==GameSettings_CombatSpeed?CombatSpeedMenu:OuttakeFrequencyMenu));
    return submenuChoice;
}
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { assert(refresh==TRUE); ++healthDraws; }
uint16_t Prompt_Yes_No(uint16_t defaultAnswer) { assert(defaultAnswer==FALSE); ++prompts; return quitAnswer; }

static void run(uint16_t choice,uint16_t result)
{
    mainChoice=choice; submenuChoice=result;
    menuCalls=sidebars=healthDraws=prompts=0; text[0]=0;
    uint16_t panel=CurrentMenuLayoutIndex;
    Menu_Change_Game_Settings();
    assert(CurrentMenuLayoutIndex==panel); /* Uses existing caller panel. */
    assert(strstr(text,"Change movement rate\rSet combat speed\rTurn Sound O")!=NULL);
    assert(strstr(text,"\rChange Outtake Frequency\rQuit game\rCancel")!=NULL);
}
int main(void)
{
    assert(SoundEffectsEnabled==TRUE);
    Menu_Memory_Variables(PauseMenuPanel);
    static const uint16_t steps[]={1,2,4};
    for(unsigned initial=0;initial<3;++initial)
        for(unsigned selected=0;selected<3;++selected) {
            ExplorationStepsPerInput=steps[initial]; expectedDefault=(uint16_t)initial;
            CacheMapRoomLoaded=FALSE; run(GameSettings_MovementRate,(uint16_t)selected);
            assert(ExplorationStepsPerInput==steps[selected]);
            assert(MenuControls[MovementRateMenu].baseRow==FALSE);
            assert(menuCalls==2 && sidebars==2 && healthDraws==0);
        }
    ExplorationStepsPerInput=0; expectedDefault=65535;
    run(GameSettings_MovementRate,65535); assert(ExplorationStepsPerInput==0);
    ExplorationStepsPerInput=65535; expectedDefault=65534;
    run(GameSettings_MovementRate,3); assert(ExplorationStepsPerInput==4);
    CacheMapRoomLoaded=0x80; ExplorationStepsPerInput=4; expectedDefault=2;
    run(GameSettings_MovementRate,2); assert(ExplorationStepsPerInput==1);
    CacheMapRoomLoaded=FALSE;
    for(uint16_t speed=0;speed<6;++speed) {
        run(GameSettings_CombatSpeed,speed);
        assert(CombatSpeedSetting==speed && healthDraws==1 && sidebars==2);
    }
    run(GameSettings_CombatSpeed,65535); assert(CombatSpeedSetting==65535);
    SoundEffectsEnabled=0; run(GameSettings_ToggleSound,0);
    assert(SoundEffectsEnabled==1 && strstr(text,"Turn Sound On\r")!=NULL && sidebars==1);
    run(GameSettings_ToggleSound,0);
    assert(SoundEffectsEnabled==0 && strstr(text,"Turn Sound Off\r")!=NULL);
    SoundEffectsEnabled=0x8000; run(GameSettings_ToggleSound,0); assert(SoundEffectsEnabled==0x8001);
    for(uint16_t frequency=0;frequency<3;++frequency) {
        run(GameSettings_OuttakeFrequency,frequency);
        assert(OuttakeFrequency==frequency && healthDraws==1 && sidebars==2);
    }
    run(GameSettings_OuttakeFrequency,0x1234); assert(OuttakeFrequency==0x34);
    quitAnswer=FALSE; ExitMainLoop=TRUE; run(GameSettings_Quit,0);
    assert(!ExitMainLoop && prompts==1 && sidebars==2);
    quitAnswer=0x8001; run(GameSettings_Quit,0); assert(ExitMainLoop==0x8001);
    run(5,0); assert(menuCalls==1 && sidebars==1 && !prompts && !healthDraws);
    run(65535,0); assert(menuCalls==1 && sidebars==1 && ExitMainLoop==0x8001);
    puts("Original settings, WORD/BYTE semantics and cache movement override verified.");
    return 0;
}
