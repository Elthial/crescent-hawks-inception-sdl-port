/* Sol: Synthetic witnesses for original0FDC:13DE, not gameplay validation.
 * Real inventory body; only unconverted drawing/menu/input are adapters. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, menus, warnings, keys;
static uint16_t choices[4], displayedPoints[4];
static uint8_t expectedWarningPoints;
static int suffixSeen, pointsPromptSeen;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Armour check %u failed\n",checks); exit(1); }
}
void Draw_Top_Graphic_Sidebar(void) { TextRow = TextColumn = 0; }
void Display_Text_From_Memory(uint8_t *text)
{
    if (!strcmp((char *)text," to:\r")) suffixSeen = 1;
    if (!strcmp((char *)text,"Drop it\r\r\rArmor points left: ")) pointsPromptSeen = 1;
}
void Display_Text_4FA0_Value(void) { ++TextRow; }
void Display_Text_Dynamic_Value(uint16_t value)
{
    check(menus < 4); displayedPoints[menus] = value;
}
uint16_t Display_Menu_Choices_And_Check(uint16_t layout)
{
    check(layout == EquipmentDistributionMenuLayout && menus < 4);
    return choices[menus++];
}
void Display_Text_At(uint8_t *text, uint16_t column, uint16_t row)
{
    check(column == 0 && row == EquipmentDistributionWarningRow);
    check(!strcmp((char *)text,"He already has that type of armor."));
    check(Characters[0].armourValue == expectedWarningPoints);
    ++warnings;
}
void Wait_For_50Hz_Then_Check_Input(void) { }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
static void reset(void)
{
    memset(Characters,0,sizeof Characters);
    for (unsigned i = 0; i < PartySize; ++i) Characters[i].name = Character_Dead;
    Characters[0].name = 0; Characters[5].name = 5;
    memset(choices,0,sizeof choices); memset(displayedPoints,0,sizeof displayedPoints);
    menus = warnings = keys = 0; suffixSeen = pointsPromptSeen = 0;
    EquipmentDistributionMenuOptionCount = 0;
}
int main(void)
{
    check(!strcmp((char *)ArmourTextDescription[ArmourType_None],"None"));
    check(!strcmp((char *)ArmourTextDescription[ArmourType_FlakVest],"Flak Vest"));
    check(!strcmp((char *)ArmourTextDescription[ArmourType_FlakSuit],"Flak Suit"));
    check(!strcmp((char *)ArmourTextDescription[ArmourType_LightEnv],"Lt Env Suit"));
    check(!strcmp((char *)ArmourTextDescription[ArmourType_HeavyEnv],"Hv Env Suit"));
    check(!strcmp((char *)ArmourTextDescription[ArmourType_Ablative],"Ablative"));
    reset(); Distribute_Purchased_Armour(ArmourType_None,35);
    check(menus == 0 && Characters[0].armourType == ArmourType_None);

    reset(); choices[0] = 1;
    Distribute_Purchased_Armour(ArmourType_FlakSuit,35);
    check(Characters[5].armourType == ArmourType_FlakSuit && Characters[5].armourValue == 35);
    check(Characters[0].armourType == ArmourType_None && menus == 1);
    check(suffixSeen && pointsPromptSeen && displayedPoints[0] == 35);
    check(EquipmentDistributionMenuOptionCount == 3);

    reset(); Characters[0].armourType = ArmourType_FlakVest; Characters[0].armourValue = 12;
    choices[1] = 1; Distribute_Purchased_Armour(ArmourType_FlakSuit,35);
    check(Characters[0].armourValue == 35 && Characters[0].armourType == ArmourType_FlakSuit);
    check(Characters[5].armourValue == 12 && Characters[5].armourType == ArmourType_FlakVest);
    check(menus == 2 && displayedPoints[1] == 12 && warnings == 0);

    reset(); choices[0] = 2; Distribute_Purchased_Armour(ArmourType_Ablative,50);
    check(Characters[0].armourType == ArmourType_None && Characters[5].armourType == ArmourType_None);
    check(menus == 1);

    reset(); Characters[0].armourType = ArmourType_FlakSuit; Characters[0].armourValue = 50;
    expectedWarningPoints = 20; choices[1] = 1;
    Distribute_Purchased_Armour(ArmourType_FlakSuit,20);
    check(warnings == 1 && keys == 1 && menus == 2);
    check(Characters[0].armourValue == 20 && Characters[5].armourValue == 50);

    reset(); Characters[0].armourType = ArmourType_FlakVest; Characters[0].armourValue = 128;
    choices[1] = 1; Distribute_Purchased_Armour(ArmourType_FlakSuit,35);
    check(displayedPoints[1] == UINT16_C(0xFF80) && Characters[5].armourValue == 128);

    reset(); Characters[0].name = Character_Dead;
    Characters[0].armourType = ArmourType_Ablative; Characters[0].armourValue = 50;
    Distribute_Purchased_Armour(ArmourType_FlakVest,12);
    check(menus == 0 && Characters[0].armourType == ArmourType_FlakVest && Characters[0].armourValue == 12);
    check(Characters[5].armourType == ArmourType_None);
    reset(); Characters[0].name = Characters[5].name = Character_Dead;
    Distribute_Purchased_Armour(ArmourType_FlakSuit,35);
    check(menus == 0 && Characters[0].armourValue == 35);

    reset(); for (unsigned i = 0; i < PartySize; ++i) Characters[i].name = 0;
    choices[0] = PartySize - 1; Distribute_Purchased_Armour(ArmourType_Ablative,50);
    check(Characters[PartySize-1].armourType == ArmourType_Ablative);
    check(EquipmentDistributionMenuOptionCount == PartySize + 1);
    printf("Armour distribution: %u checks passed\n",checks);
    return 0;
}
