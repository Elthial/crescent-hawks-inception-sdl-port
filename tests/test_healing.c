/* Sol: Native-rule witnesses using the real source body with UI/RNG stubs.
 * These are not emulator or gameplay certification. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, rolls, messages, keys, sidebars;
static unsigned char tags[16], *shown[64];
static unsigned shownCount;
static int changeHealthDuringRoll;
static void check(int condition) {
    ++checks;
    if (!condition) { fprintf(stderr, "Healing check %u failed\n", checks); exit(1); }
}
static int wasShown(unsigned char *text) {
    for (unsigned i = 0; i < shownCount; ++i) if (shown[i] == text) return 1;
    return 0;
}
static void reset(void) {
    memset(Characters,0,sizeof Characters);
    memset(Mechs,0,sizeof Mechs);
    PartyHealthRecoveryTimer=PartyHasInjuredMember=PurchasedMedkit=PurchasedFieldSurgeryKit=0;
    TextColour=0; CBills=0;
    memset(HealingDice,0,sizeof HealingDice);
    rolls = messages = keys = sidebars = shownCount = 0;
    changeHealthDuringRoll = 0;
    for (unsigned i = 0; i < 16; ++i) Characters[i].name = 0xFF;
    for (unsigned i = 0; i < 8; ++i) CharacterNames[i] = &tags[i];
    for (unsigned i = 0; i < 4; ++i) {
        MedicalSkillText[i] = &tags[8 + i];
        MedicalEquipmentText[i] = &tags[12 + i];
    }
    Characters[0].name = 0;
    Characters[0].body = 5;
}
void Draw_Message_Box(void) { ++messages; }
void Display_Text_From_Memory(unsigned char *text) {
    check(shownCount < 64);
    shown[shownCount++] = text;
}
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(unsigned char *text) { Display_Text_From_Memory(text); }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
/* Headless adapter for the ORIGINAL dialogue dispatcher. Not production code. */
void Citadel_Building_Dialogs(uint16_t action) {
    check(action==CitadelDialog_QueryPartyInjuries);
    PartyHasInjuredMember=FALSE;
    for(unsigned i=0;i<PartySize;++i)
        if(Characters[i].name!=Character_Dead &&
           (int8_t)Characters[i].health!=CharacterHealthPerBodyPoint*(int8_t)Characters[i].body)
            PartyHasInjuredMember=TRUE;
}
void Drain_Pending_Keyboard_Input(void) {}
void Menu_Memory_Variables(uint16_t layout) { check(layout == 1); }
void Draw_Menu_Border(uint16_t layout) { check(layout == 1); }
void Draw_Top_Graphic_Sidebar(void) {}
void Set_Text_Colour_Bright_Green(void) {}
void Display_Text_4FA0_Value(void) {}
uint16_t RollD6(void) {
    ++rolls;
    if (changeHealthDuringRoll) Characters[0].health = 20;
    return 6;
}
void Wait_For_50Hz_Then_Check_Input(void) {}
void Prompt_And_Wait_For_Key(void) {}
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { check(refresh == 0); ++sidebars; }

int main(void) {
    check(sizeof(Character)==17 && sizeof(Mech)==125);
    check(strcmp((char *)CharacterNames[0],"Jason")==0);
    check(HealingDice[5][3]==0xA2); /* hospital tier5:2D6 x10 */
    /* Every possible packed table BYTE, including CBW's high-bit cases. */
    for (unsigned entry = 0; entry < 256; ++entry) {
        reset();
        Characters[0].body = 127;
        HealingDice[0][0] = (uint8_t)entry;
        Heal_Characters(0);
        unsigned count = entry & 15;
        unsigned multiplier = entry >> 4;
        if (!multiplier) multiplier = 1;
        unsigned result = count * 6 * multiplier;
        if (result > 1270) result = 1270;
        check(rolls == count);
        check(Characters[0].health == (uint8_t)result);
        check(PartyHealthRecoveryTimer == 63 && sidebars == 1);
    }
    /* All signed body/health BYTE combinations: equality gate, upper clamp,
     * no lower clamp, and low-BYTE store, with zero healing dice. */
    for (unsigned body = 0; body < 256; ++body) {
        for (unsigned health = 0; health < 256; ++health) {
            reset();
            Characters[0].body = (uint8_t)body;
            Characters[0].health = (uint8_t)health;
            int maximum = (int8_t)(uint8_t)body * 10;
            int original = (int8_t)(uint8_t)health;
            int expected = original > maximum ? maximum : original;
            Heal_Characters(0);
            check(Characters[0].health == (uint8_t)expected);
            check(PartyHealthRecoveryTimer == (original == maximum ? 0 : 63));
            check(rolls == 0);
        }
    }
    /* Party equipment and description thresholds, including first medic tie. */
    for (unsigned skill = 0; skill < 5; ++skill) {
        for (unsigned owned = 0; owned < 4; ++owned) {
            reset();
            Characters[0].skillMedical = (uint8_t)skill;
            PurchasedMedkit = (uint8_t)(owned & 1);
            PurchasedFieldSurgeryKit = (uint8_t)(owned & 2);
            unsigned equipment = skill >= 3 && (owned & 2) ? 2 : (owned & 1 ? 1 : 0);
            HealingDice[skill][equipment] = 1;
            Heal_Characters(0);
            check(rolls == 1 && Characters[0].health == 6);
            if (skill) check(wasShown(MedicalEquipmentText[equipment]));
        }
    }
    for (unsigned tier = 1; tier < 8; ++tier) {
        reset();
        Characters[0].skillMedical = 4;
        unsigned equipment = tier <= 2 ? 1 : 3;
        HealingDice[tier][equipment] = 1;
        Heal_Characters((uint16_t)tier);
        check(rolls == 1 && Characters[0].health == 6);
        unsigned description = tier >= 5 ? 3 : (tier >= 3 ? tier - 2 : tier - 1);
        check(wasShown(MedicalSkillText[description]));
        check(wasShown(MedicalEquipmentText[equipment]));
    }
    reset();
    Characters[0].health = 50;
    Characters[1].name = 1; Characters[1].body = 5; Characters[1].skillMedical = 3;
    Characters[2].name = 2; Characters[2].body = 5; Characters[2].skillMedical = 3;
    Characters[3].skillMedical = 4; /* dead slot must not win */
    Heal_Characters(0);
    check(shown[0] == CharacterNames[1]);
    reset(); PartyHealthRecoveryTimer = 7;
    Heal_Characters(3);
    check(rolls == 0 && messages == 1 && keys == 1);
    check(PartyHealthRecoveryTimer == 7 && sidebars == 0);
    check(strcmp((const char *)shown[0], "Your characters need more time to recover.") == 0);
    reset(); PartyHealthRecoveryTimer = 7; Characters[0].health = 50;
    Heal_Characters(0);
    check(strcmp((const char *)shown[0], "Nobody is wounded.") == 0);
    reset(); Characters[0].health = 50;
    Heal_Characters(3);
    check(shownCount == 0 && messages == 0 && rolls == 0 && PartyHealthRecoveryTimer == 0);
    /* Native rereads health AFTER the calls; deliberate stub mutation verifies it. */
    reset(); HealingDice[0][0] = 1; changeHealthDuringRoll = 1;
    Heal_Characters(0);
    check(Characters[0].health == 26);
    printf("Passed %u actual-source healing checks.\n", checks);
    return 0;
}
