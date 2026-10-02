/* Sol: Actual movement/settings and Yes/No bodies; drawing/menu/key adapters. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks, menuSetups, draws, options, keys, prompts;
static uint16_t returnedOption, answer;
static int graphicsPrompt;
static void check(int condition)
{
    ++checks; if (!condition) { fprintf(stderr,"Combat settings check %u failed\n",checks); exit(1); }
}
void Menu_Memory_Variables(uint16_t layout) { check(layout == 3); ++menuSetups; }
void Draw_Top_Graphic_Sidebar(void) { ++draws; }
void Display_Text_From_Memory(uint8_t *text)
{
    if (!prompts++) check(!strcmp((char *)text,graphicsPrompt ? "See combat graphics:" :
        "Combat messages:\rNone\rBrief\rVerbose"));
}
uint16_t Display_Menu_Choices_And_Check(uint16_t layout)
{
    check(layout == 3 && draws == 1 && prompts == 1);
    check(CombatMessageMenuVerticalLines == 2 && CombatMessageMenuOptionCount == 3);
    check(CombatMessageMenuDefaultOption == CombatMessageVerbosity);
    ++options; return returnedOption;
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return answer; }
void Drain_Pending_Keyboard_Input(void) { }
int main(void)
{
    memset(Characters,0,sizeof Characters);
    for (unsigned slot = 0; slot < PartySize; ++slot)
        for (unsigned dexterity = 0; dexterity < 256; ++dexterity)
            for (unsigned armour = 0; armour < 256; ++armour) {
                Characters[slot].dexterity = (uint8_t)dexterity;
                Characters[slot].armourType = (uint8_t)armour;
                Combat_Infantry_Movement((uint16_t)(4 + slot));
                int dex = dexterity < 128 ? (int)dexterity : (int)dexterity-256;
                int points = dex * 3;
                points = points >= 0 ? points/4 : -((-points+3)/4);
                if (points < 3) points = 3;
                int armourType = armour < 128 ? (int)armour : (int)armour-256;
                if (armourType > 1) points /= 2;
                if (points > 8) points = 8;
                check(CharacterMovementPointsRemaining == points);
            }
    for (uint16_t id = 16; id < AllCombatantCount; ++id) {
        Combat_Infantry_Movement(id); check(CharacterMovementPointsRemaining == 6);
    }
    for (unsigned selected = 0; selected < 4; ++selected) {
        returnedOption = selected == 3 ? UINT16_MAX : (uint16_t)selected;
        menuSetups = draws = prompts = options = 0;
        CombatMessageVerbosity = 2; CombatDisplayGraphics = 7;
        check(SettingsMenu_CombatMessages() == returnedOption);
        check(CombatMessageVerbosity == 2 && CombatDisplayGraphics == 7);
        check(CombatMessageMenuVerticalLines == 0 && CombatMessageMenuOptionCount == 3 && options == 1);
    }
    graphicsPrompt = 1;
    for (unsigned enabled = 0; enabled < 2; ++enabled) {
        for (unsigned accept = 0; accept < 2; ++accept) {
            menuSetups = draws = prompts = options = keys = 0;
            CombatDisplayGraphics = (uint16_t)enabled; TextColour = 12;
            answer = accept ? 'Y' : 'N';
            check(SettingsMenu_SeeCombatGraphics() == accept);
            check(CombatDisplayGraphics == enabled && keys == 1 && menuSetups == 1 && draws == 1);
            check(TextColour == 12);
        }
    }
    printf("Personnel movement/combat settings: %u checks passed\n",checks); return 0;
}
