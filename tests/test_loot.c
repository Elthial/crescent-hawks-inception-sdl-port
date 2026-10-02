/* Sol: Original loot/prompt/support/inventory bodies; only unconverted UI and
 * deterministic RNG calls are test adapters. Not an emulator/gameplay capture. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, rngReads, keys, menus, warnings;
static uint8_t randomByte;
static uint16_t answer, displayedCash, choices[8];
static int giveSuffixSeen;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Loot/support check %u failed\n",checks); exit(1); }
}
uint8_t Rand_0x00_to_0xFF(void) { ++rngReads; return randomByte; }
void Menu_Memory_Variables(uint16_t layout) { check(layout == 1 || layout == 3); }
void Draw_Top_Graphic_Sidebar(void) { TextRow = TextColumn = 0; }
void Draw_Menu_Border(uint16_t border) { check(border == 0); }
void Display_Text_From_Memory(uint8_t *text)
{
    if (!strcmp((char *)text," to:\r")) giveSuffixSeen = 1;
    ++TextRow;
}
void Set_Text_Colour_Bright_Green(void) { TextColour = 10; }
void Display_Text_Dynamic_Value(uint16_t value) { displayedCash = value; }
void Display_Sentence_Period(void) { }
void Wait_For_50Hz_Then_Check_Input(void) { }
void Drain_Pending_Keyboard_Input(void) { }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return answer; }
void Display_Text_4FA0_Value(void) { ++TextRow; }
uint16_t Display_Menu_Choices_And_Check(uint16_t layout)
{
    check(layout == EquipmentDistributionMenuLayout && menus < 8);
    return choices[menus++];
}
void Display_Text_At(uint8_t *text, uint16_t column, uint16_t row)
{
    check(column == 0 && row == EquipmentDistributionWarningRow);
    check(!strcmp((char *)text,"He already has that weapon."));
    ++warnings;
}
static void reset(void)
{
    memset(Characters,0,sizeof Characters);
    memset(Mechs,0,sizeof Mechs);
    memset(CombatantCasualtyFlags,0,sizeof CombatantCasualtyFlags);
    Characters[Character_Rex].name = Character_Dead;
    CBills = 100;
    rngReads = keys = menus = warnings = 0;
    memset(choices,0,sizeof choices);
    giveSuffixSeen = 0;
    randomByte = 0;
    answer = 'Y';
    displayedCash = 0;
}
int main(void)
{
    reset(); Loot_Enemy_Soldiers_Dialog(); check(CBills == 100 && rngReads == 0 && keys == 0);
    reset(); CombatantCasualtyFlags[4] = 1;
    Loot_Enemy_Soldiers_Dialog(); check(CBills == 102 && displayedCash == 2 && rngReads == 0);
    reset(); CombatantCasualtyFlags[16] = 1; randomByte = 15;
    Loot_Enemy_Soldiers_Dialog(); check(CBills == 118 && displayedCash == 18 && rngReads == 1);
    check(DeadInfantryFlags[4] == 1 && LootableInfantryFlags[0] == 1);
    reset(); for (unsigned i = 16; i < AllCombatantCount; ++i) CombatantCasualtyFlags[i] = 1;
    randomByte = 15;
    Loot_Enemy_Soldiers_Dialog(); check(CBills == 244 && displayedCash == 144 && rngReads == 8);
    reset(); CombatantCasualtyFlags[16] = 1; CBills = UINT32_MAX;
    Loot_Enemy_Soldiers_Dialog(); check(CBills == 2); /* Native DWORD wrap. */

    /* Weapon-presence gate reads Mech storage; do not substitute enemy weapon. */
    reset(); CombatantCasualtyFlags[16] = 1; Characters[8].weapon = 7;
    Loot_Enemy_Soldiers_Dialog(); check(Characters[0].weapon == 0 && keys == 1);
    for (unsigned slot = 0; slot < Enemy_Infantry_Record_Count; ++slot) {
        reset(); CombatantCasualtyFlags[16 + slot] = 1;
        ((uint8_t *)Mechs)[11 + slot * sizeof(Character)] = 1;
        Characters[0].weapon = 1; Characters[8 + slot].weapon = 7;
        Loot_Enemy_Soldiers_Dialog();
        check(Characters[0].weapon == 7 && keys == 2 && menus == 0);
    }
    reset(); CombatantCasualtyFlags[16] = 1; Mechs[0].name[11] = 1;
    Characters[0].weapon = 1; Characters[8].weapon = 7; answer = 'n';
    Loot_Enemy_Soldiers_Dialog(); check(Characters[0].weapon == 1);
    reset(); CombatantCasualtyFlags[16] = 1; Mechs[0].name[11] = 1;
    Characters[Character_Rex].name = 1; Characters[8].weapon = 7; choices[0] = 1;
    Loot_Enemy_Soldiers_Dialog(); check(menus == 1 && Characters[1].weapon == 7 && keys == 1);
    check(giveSuffixSeen && EquipmentDistributionMenuOptionCount == PartySize + 1);

    /* Zero/one-living shortcut equips Jason even if the sole survivor is Rex. */
    reset(); CombatantCasualtyFlags[16] = 1; Mechs[0].name[11] = 1;
    for (unsigned i = 0; i < PartySize; ++i) Characters[i].name = Character_Dead;
    Characters[Character_Rex].name = 1; Characters[8].weapon = 128;
    Loot_Enemy_Soldiers_Dialog(); check(menus == 0 && Characters[0].weapon == 128 && keys == 1);

    reset();
    for (unsigned i = 0; i < PartySize; ++i) Characters[i].name = Character_Dead;
    Characters[0].name = 0; Characters[5].name = 5; choices[0] = 1;
    Distribute_Weapon_To_Party(7);
    check(Characters[5].weapon == 7 && Characters[0].weapon == 0 && menus == 1);
    check(EquipmentDistributionMenuOptionCount == 3);

    reset();
    for (unsigned i = 2; i < PartySize; ++i) Characters[i].name = Character_Dead;
    Characters[1].name = 1; Characters[0].weapon = 1;
    choices[0] = 0; choices[1] = 1;
    Distribute_Weapon_To_Party(7);
    check(Characters[0].weapon == 7 && Characters[1].weapon == 1 && menus == 2);

    reset();
    for (unsigned i = 2; i < PartySize; ++i) Characters[i].name = Character_Dead;
    Characters[1].name = 1; choices[0] = 2;
    Distribute_Weapon_To_Party(7);
    check(Characters[0].weapon == 0 && Characters[1].weapon == 0 && menus == 1);

    reset();
    for (unsigned i = 2; i < PartySize; ++i) Characters[i].name = Character_Dead;
    Characters[1].name = 1; Characters[0].weapon = 7;
    choices[0] = 0; choices[1] = 2;
    Distribute_Weapon_To_Party(7);
    check(warnings == 1 && menus == 2 && keys == 1 && Characters[0].weapon == 7);

    reset(); Distribute_Weapon_To_Party(Infantry_Cudgel);
    check(menus == 0 && keys == 0 && Characters[0].weapon == 0);
    for (unsigned i = 0; i < PartySize; ++i) Characters[i].name = Character_Dead;
    Characters[0].weapon = 1;
    Distribute_Weapon_To_Party(7);
    check(Characters[0].weapon == 7 && menus == 0); /* Zero living, displaced weapon discarded. */

    reset(); CombatComputerControl = 2; answer = '\r';
    check(Return_Bool_Allow_Computer_Control_Dialog() == TRUE);
    CombatComputerControl = 1; answer = 'n';
    check(Return_Bool_Allow_Computer_Control_Dialog() == FALSE);

    static const uint8_t structureOrdinals[35] = {
        0,0,0,0,0,0,0, 1,1,1,1,1,1,1, 5,5,5,5,5,5,5,
        6,6,6,6,6,6,6, 2,2, 7,7, 4,4, 3
    };
    for (unsigned i = 0; i < MechStructureLocationCount; ++i) Mechs[0].currentStructure[i] = (uint8_t)(11 + i);
    for (uint16_t slot = 0; slot < MechCriticalSlotCount; ++slot)
        check(Check_If_CriticalSlot_Destroyed(0,(uint16_t)(0x33 + slot)) == 11 + structureOrdinals[slot]);
    check(Check_If_CriticalSlot_Destroyed(0,0xFFFF) == 11); /* Signed threshold comparisons. */
    Mechs[0].upgradePackageBase = 0xC8;
    check(Check_If_CriticalSlot_Destroyed(0,0x55) == 1);
    printf("%u personnel loot/support assertions passed\n",checks);
    return 0;
}
