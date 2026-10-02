#include "game.h"

/* 183B:2474..24EF. Sol: Set an infantry combatant's movement budget.
 * Friendly4..11 uses floor(3*Dexterity/4), minimum3 THEN heavy-armour half,
 * maximum8 LAST. Enemy16..23 uses fixed6. This is a combatant ID, not a
 * character record ID. Other/malformed IDs lack a valid host contract. */
void Combat_Infantry_Movement(uint16_t combatantId)
{
    CharacterMovementPointsRemaining = PersonnelEnemyMovementPoints;
    if ((int16_t)combatantId < Enemy_All_CombatantId_Range_First)
    {
        uint16_t partyRecordId = (uint16_t)(combatantId - Friendly_Infantry_Combatant_Range_First);
        int16_t points = PersonnelDexterityMovementNumerator * (int8_t)Characters[partyRecordId].dexterity;
        /* Sol: Two native SARs round negative products downward. C signed
         * division truncates toward zero, so explicitly preserve that floor. */
        points = (int16_t)(points >= 0 ? points / PersonnelDexterityMovementDenominator :
            -((-points + PersonnelDexterityMovementDenominator-1) / PersonnelDexterityMovementDenominator));
        CharacterMovementPointsRemaining = (uint16_t)points;
        if (points < PersonnelMovementMinimumBeforeArmour)
            CharacterMovementPointsRemaining = PersonnelMovementMinimumBeforeArmour;
        if ((int8_t)Characters[partyRecordId].armourType > ArmourType_FlakVest)
            CharacterMovementPointsRemaining = (uint16_t)((int16_t)CharacterMovementPointsRemaining / 2);
    }
    if ((int16_t)CharacterMovementPointsRemaining > PersonnelMovementMaximum)
        CharacterMovementPointsRemaining = PersonnelMovementMaximum;
}

/* 183B:24F0..2555. Sol: Offer None/Brief/Verbose using the current setting
 * as default, clear temporary vertical lines, return the full selected WORD.
 * The combat parent, NOT this method, stores the setting. */
uint16_t SettingsMenu_CombatMessages(void)
{
    Menu_Memory_Variables(CombatSettingsMenuLayout);
    Draw_Top_Graphic_Sidebar();
    Display_Text_From_Memory((uint8_t *)"Combat messages:\rNone\rBrief\rVerbose"); /* 3EDB:3D3A; no trailing CR in EXE */
    CombatMessageMenuVerticalLines = CombatMessageMenuVerticalSections;
    CombatMessageMenuOptionCount = CombatMessageOptionCount;
    CombatMessageMenuDefaultOption = CombatMessageVerbosity;
    uint16_t selectedOption = Display_Menu_Choices_And_Check(CombatSettingsMenuLayout);
    CombatMessageMenuVerticalLines = 0;
    return selectedOption;
}

/* 183B:2556..2590. Sol: Ask whether combat effects should be shown and
 * return original Yes/No WORD. Parent stores it; no forced boolean cast. */
uint16_t SettingsMenu_SeeCombatGraphics(void)
{
    Menu_Memory_Variables(CombatSettingsMenuLayout);
    Draw_Top_Graphic_Sidebar();
    Display_Text_From_Memory((uint8_t *)"See combat graphics:"); /* 3EDB:3D5E */
    return Prompt_Yes_No(CombatDisplayGraphics);
}
