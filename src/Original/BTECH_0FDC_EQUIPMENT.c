#include "game.h"

/* 0FDC:13DE..15E5. Sol: Give armour to a living party member, then offer
 * their displaced armour to another recipient until dropped or none remains.
 * Native WORD menu map and signed BYTE loads retained; valid table indexes
 * required, as with weapon distribution. No inventory helper replaces it. */
void Distribute_Purchased_Armour(uint16_t heldArmourType, uint16_t heldArmourValue)
{
    uint16_t partyMemberByMenuIndex[PartySize];
    for (uint16_t menuIndex = 0; menuIndex < PartySize; ++menuIndex)
        partyMemberByMenuIndex[menuIndex] = UINT16_MAX;

    uint16_t livingPartyMemberCount = 0;
    for (uint16_t partyMemberId = 0; partyMemberId < PartySize; ++partyMemberId)
        if (Characters[partyMemberId].name != Character_Dead)
            partyMemberByMenuIndex[livingPartyMemberCount++] = partyMemberId;

    while (heldArmourType != ArmourType_None)
    {
        uint16_t menuChoice;
        if (livingPartyMemberCount <= 1)
        {
            /* Sol: Original shortcut always equips Jason and discards his
             * previous armour, including when he is dead. */
            Characters[Character_Jason].armourType = (uint8_t)heldArmourType;
            Characters[Character_Jason].armourValue = (uint8_t)heldArmourValue;
            menuChoice = livingPartyMemberCount;
        }
        else
        {
            Draw_Top_Graphic_Sidebar();
            Display_Text_From_Memory((uint8_t *)"Give "); /* 3EDB:1661 */
            Display_Text_From_Memory(ArmourTextDescription[heldArmourType]);
            Display_Text_From_Memory((uint8_t *)" to:\r"); /* 3EDB:1667 */
            for (uint16_t menuIndex = 0; menuIndex < livingPartyMemberCount; ++menuIndex)
            {
                Character *partyMember = &Characters[partyMemberByMenuIndex[menuIndex]];
                if (partyMember->name != Character_Dead)
                {
                    Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)partyMember->name]);
                    TextColumn = EquipmentDistributionEquipmentColumn;
                    Display_Text_From_Memory(
                        ArmourTextDescription[(int16_t)(int8_t)partyMember->armourType]);
                    Display_Text_4FA0_Value();
                }
            }
            Display_Text_From_Memory((uint8_t *)"Drop it\r\r\rArmor points left: "); /* 3EDB:166D */
            Display_Text_Dynamic_Value(heldArmourValue);
            EquipmentDistributionMenuOptionCount = livingPartyMemberCount + 1;
            menuChoice = Display_Menu_Choices_And_Check(EquipmentDistributionMenuLayout);
        }

        if (menuChoice == livingPartyMemberCount)
            heldArmourType = ArmourType_None;
        else
        {
            Character *recipient = &Characters[partyMemberByMenuIndex[menuChoice]];
            int16_t displacedArmourType = (int8_t)recipient->armourType;
            int16_t displacedArmourValue = (int8_t)recipient->armourValue;
            recipient->armourType = (uint8_t)heldArmourType;
            recipient->armourValue = (uint8_t)heldArmourValue;
            /* Sol: Swap precedes the warning, even when the new suit has
             * fewer points. The displaced points continue through the chain. */
            if (heldArmourType == (uint16_t)displacedArmourType)
            {
                Display_Text_At((uint8_t *)"He already has that type of armor.",
                    0, EquipmentDistributionWarningRow); /* 3EDB:168B */
                Wait_For_50Hz_Then_Check_Input();
                Keyboard_Get_ASCII_Hex_Input();
            }
            heldArmourType = (uint16_t)displacedArmourType;
            heldArmourValue = (uint16_t)displacedArmourValue;
        }
    }
}

// 0FDC:15E6: void Distribute_Weapon_To_Party(Stack WORD heldWeaponId)
// Called from:
//      Loot_Enemy_Soldiers_Dialog
//      Citadel_Building_Dialogs
// Sol: Summary: Give a purchased or recovered weapon to a party member.
// Sol: Outline: Select a recipient, handle existing equipment, and update the character's weapon slot.
void Distribute_Weapon_To_Party(uint16_t heldWeaponId)
{
	// Sol: Native3092 recipients bind to flat Characters; name/current weapon
	// Sol: BYTEs use native CBW. Exact leading-space16B4 text retained.
	// Sol: These FFFF stores initialize a local WORD menu map. The former
	// Sol: pseudo-C incorrectly turned them into destructive Infantry[] writes.
	uint16_t partyMemberByMenuIndex[PartySize];
	for (uint16_t menuIndex = 0x0000; menuIndex < PartySize; menuIndex++)
		partyMemberByMenuIndex[menuIndex] = UINT16_MAX;

	uint16_t livingPartyMemberCount = 0x0000;
	for (uint16_t partyMemberId = 0x0000; partyMemberId < PartySize; partyMemberId++)
	{
		if (Characters[partyMemberId].name != Character_Dead)
			partyMemberByMenuIndex[livingPartyMemberCount++] = partyMemberId;
	}

	// Sol: Weapon ID zero is both the Cudgel and this transfer loop's empty/
	// Sol: finished sentinel. A held or displaced Cudgel ends distribution.
	while (heldWeaponId != Infantry_Cudgel)
	{
		uint16_t menuChoice;
		if (livingPartyMemberCount <= 1)
		{
			// Sol: With zero or one living member, equip Jason directly and
			// Sol: discard the weapon previously held in record zero.
			Characters[Character_Jason].weapon =
				(uint8_t)heldWeaponId;
			heldWeaponId = Infantry_Cudgel;
			menuChoice = livingPartyMemberCount;
		}
		else
		{
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"Give "); //3EDB:16AE
			Display_Text_From_Memory(WeaponStats[heldWeaponId].name);
			Display_Text_From_Memory((uint8_t *)" to:\r"); //3EDB:16B4

			for (uint16_t menuIndex = 0x0000;
				 menuIndex < livingPartyMemberCount;
				 menuIndex++)
			{
				Character *partyMember =
					&Characters[partyMemberByMenuIndex[menuIndex]];
				int16_t characterNameId = (int8_t)partyMember->name;
				if (partyMember->name != Character_Dead)
				{
					Display_Text_From_Memory(
						CharacterNames[characterNameId]);
					TextColumn = EquipmentDistributionEquipmentColumn;
					Display_Text_From_Memory(
						WeaponStats[(int16_t)(int8_t)partyMember->weapon].name);
					Display_Text_4FA0_Value(); //3EDB:4FA0 is carriage return
				}
			}

			Display_Text_From_Memory((uint8_t *)"Drop it"); //3EDB:16BA
			EquipmentDistributionMenuOptionCount = livingPartyMemberCount + 1;
			menuChoice = Display_Menu_Choices_And_Check(EquipmentDistributionMenuLayout);
		}

		if (menuChoice == livingPartyMemberCount)
		{
			heldWeaponId = Infantry_Cudgel;
		}
		else
		{
			Character *recipient =
				&Characters[partyMemberByMenuIndex[menuChoice]];
			int16_t displacedWeaponId = (int8_t)recipient->weapon;

			recipient->weapon = (uint8_t)heldWeaponId;
			// Sol: As with armour distribution, the exchange precedes the warning.
			if (heldWeaponId == (uint16_t)displacedWeaponId)
			{
				Display_Text_At(
					(uint8_t *)"He already has that weapon.", 0, EquipmentDistributionWarningRow); //3EDB:16C2
				Wait_For_50Hz_Then_Check_Input();
				Keyboard_Get_ASCII_Hex_Input();
			}

			heldWeaponId = (uint16_t)displacedWeaponId;
		}
	}
}
