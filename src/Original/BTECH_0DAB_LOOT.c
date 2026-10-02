#include "game.h"

// 0DAB:094B: void Loot_Enemy_Soldiers_Dialog(void)
// Called from:
//      Combat_Run_Encounter
// Sol: Summary: Collect equipment from defeated personnel.
// Sol: Outline: Inspect casualty flags, award cash, and distribute eligible weapons through Rex or Jason's exchange prompt.
void Loot_Enemy_Soldiers_Dialog()
{
	// Sol: Native C6EB gate is bound to its actual Mechs byte addresses;
	// Sol: fallen/current weapon BYTEs are explicitly sign-extended to WORD.
	// Sol: A-006's loot-gate address asymmetry is native, not corrected by this audit.
	// Sol: 0DAB:094B-09C2. All locals and flag entries are 16-bit words.
	uint16_t lootableInfantryCount = 0x00;
	uint16_t hasLootableWeapons = FALSE;
	uint16_t lootedCBills = 0x00;
	uint16_t hasAnyLootableCasualty = FALSE;

	for (uint16_t combatantId = Friendly_Infantry_Combatant_Range_First;
		 combatantId < Enemy_All_CombatantId_Range_First;
		 combatantId++)
	{
		if (CombatantCasualtyFlags[combatantId] != FALSE
		 || DeadInfantryFlags[combatantId] != FALSE)
			hasAnyLootableCasualty = TRUE;

		if (DeadInfantryFlags[combatantId] != FALSE)
		{
			// Sol: The executable checks C6EB + combatantId * 0x11 here.
			// Sol: For IDs4..11 this reads Mechs bytes11+17*(ID-4),
			// Sol: NOT the C6A7 enemy weapon records consumed below (A-006).
			if (((uint8_t *)Mechs)[LootWeaponGateMechByteBias
                + (combatantId - Friendly_Infantry_Combatant_Range_First) * sizeof(Character)] != 0x00)
				hasLootableWeapons = TRUE;
			lootableInfantryCount++;
		}
	}

	if (hasAnyLootableCasualty == FALSE)
		return;

	// Sol: 0DAB:09C3-0A75. Cash is 3..18 C-bills for each flagged infantry
	// Sol: record, with an unconditional minimum of two whenever any qualifying
	// Sol: casualty exists (including one found only through the 393C table).
	Menu_Memory_Variables(0x01);
	Draw_Top_Graphic_Sidebar();
	Draw_Menu_Border(0x00);
	Display_Text_From_Memory((uint8_t *)"You dig through the soldiers' pockets and find "); // 3EDB:1085

	while (lootableInfantryCount != 0x00)
	{
		lootableInfantryCount--;
		lootedCBills += (Rand_0x00_to_0xFF() & InfantryLootRandomBonusMask) + InfantryLootBaseCBills;
	}

	if (lootedCBills < InfantryLootMinimumCBills)
		lootedCBills = InfantryLootMinimumCBills;

	Set_Text_Colour_Bright_Green();
	Display_Text_Dynamic_Value(lootedCBills);
	TextColour = EGA_BrightWhite;
	Display_Text_From_Memory((uint8_t *)" C-bills in cash"); // 3EDB:10B5
	CBills += lootedCBills;

	if (hasLootableWeapons != FALSE)
		Display_Text_From_Memory((uint8_t *)", and confiscate their weapons"); // 3EDB:10C6

	Display_Sentence_Period();
	Wait_For_50Hz_Then_Check_Input();
	Keyboard_Get_ASCII_Hex_Input();

	if (hasLootableWeapons == FALSE)
		return;

	// Sol: 0DAB:0A76-0B58. Each word flag at 395C corresponds to the
	// Sol: weapon byte in Infantry[8 + slot]. When Rex is absent, Jason gets a
	// Sol: simple one-for-one prompt; with Rex present, the inventory handler runs.
	for (uint16_t lootSlot = 0x00; lootSlot < Enemy_Infantry_Record_Count; lootSlot++)
	{
		if (LootableInfantryFlags[lootSlot] == 0x00)
			continue;

		uint16_t fallenWeaponId = (uint16_t)(int16_t)(int8_t)Characters[Enemy_Infantry_Record_First + lootSlot].weapon;
		if (Characters[Character_Rex].name != Character_Dead)
		{
			Distribute_Weapon_To_Party(fallenWeaponId);
			continue;
		}

		Draw_Top_Graphic_Sidebar();
		uint16_t currentWeaponId = (uint16_t)(int16_t)(int8_t)Characters[Character_Jason].weapon;
		if (fallenWeaponId == 0x00)
			continue;

		Display_Text_From_Memory((uint8_t *)"Do you want to drop your "); // 3EDB:10E5
		Display_Text_From_Memory(WeaponStats[currentWeaponId].name);
		Display_Text_From_Memory((uint8_t *)" in exchange for a "); // 3EDB:10FF
		Display_Text_From_Memory(WeaponStats[fallenWeaponId].name);
		Display_Text_From_Memory((uint8_t *)"?"); // 3EDB:1113

		if (Prompt_Yes_No(0x00))
			Characters[Character_Jason].weapon = (uint8_t)fallenWeaponId;
	}
}

// 0DAB:0B5E: Register uint16 Return_Bool_Allow_Computer_Control_Dialog()
// Called from:
//      Combat_Run_Encounter
// Sol: Summary: Ask whether the computer should control combat actions.
// Sol: Outline: Display the question and return the player's yes-or-no response.
uint16_t Return_Bool_Allow_Computer_Control_Dialog(void)
{
	// Sol: Audit: complete0B5E..0B94 local sequence matches, including DS1115
	// Sol: prompt and unchanged WORD default/AX result; Prompt_Yes_No is a callee contract.
	// Sol: 0DAB:0B5E-0B94. The prompt returns its native 16-bit AX value and
	// Sol: uses the previous computer-control word as the default selection.
	Menu_Memory_Variables(0x03);
	Draw_Top_Graphic_Sidebar();
	Display_Text_From_Memory((uint8_t *)"Do you want the computer to fight for you?"); // 3EDB:1115

	return Prompt_Yes_No(CombatComputerControl);
}
