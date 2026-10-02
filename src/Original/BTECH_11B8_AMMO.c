#include "game.h"

// 11B8:1762: void Mechlube_Buy_Ammo(void)
// Called from:
//      Citadel_Building_Dialogs
// Sol: Summary: Purchase ammunition for a mech.
// Sol: Outline: Find eligible ammo components, calculate quantities and cost, then update the paid-for ammo state.
void Mechlube_Buy_Ammo(void)
{
	// Sol: Preservation correction: native3092 record/global binding, signed deficit/request cap,
	// Sol: signed missile-price BYTE and native prompt/control bytes retained.
	// Sol: No autocannon shop support or corrupted-state validation is added.
	Draw_Top_Graphic_Sidebar();
	Display_Text_From_Memory((uint8_t *)"Buy ammo for:\r"); // Memory ref: [3EDB:1E99]; Sol: exact ammo EXE text
	Display_Text_Mech_Names();

	Mech *selectedMech = &Mechs[SelectedMechId];
	uint8_t *selectedMechBytes = (uint8_t *)selectedMech;
	uint8_t weaponComponents[MechWeaponOrdinalCount] = { 0 };
	uint16_t criticalSlotOffset = MECH_ComponentBlock_Start;
	uint16_t weaponSlotCount = 0;

	// Sol: Build the same ten-entry list used by CurrentAmmo and MaxAmmo. The
	// Sol: executable scans raw mech bytes 33h..55h in order and appends every
	// Sol: critical byte whose masked component ID is 10h..20h. The destroyed
	// Sol: high bit is deliberately retained for the later damaged-weapon test.
	while (criticalSlotOffset <= MECH_ComponentBlock_End &&
		weaponSlotCount < MechWeaponOrdinalCount)
	{
		uint8_t criticalComponent = selectedMechBytes[criticalSlotOffset++];
		uint8_t componentId = criticalComponent & MechComponentIdMask;
		if (componentId >= Mech_Small_Laser && componentId <= Mech_SRMissile6)
			weaponComponents[weaponSlotCount++] = criticalComponent;
	}

	uint16_t hasDamagedWeapon = FALSE;
	uint16_t hasFunctionalAmmoWeapon = FALSE;

	for (uint16_t weaponSlot = 0; weaponSlot < MechWeaponOrdinalCount; weaponSlot++)
	{
		uint8_t criticalComponent = weaponComponents[weaponSlot];
		uint8_t componentId = criticalComponent & MechComponentIdMask;

		if ((criticalComponent & Component_Destroyed) != 0)
			hasDamagedWeapon = TRUE;

		// Sol: The signed comparisons in the original exclude high-bit/destroyed
		// Sol: entries. Only the machine gun and the seven missile components are
		// Sol: offered here; autocannons 14h..17h are not handled by this shop path.
		if ((criticalComponent & Component_Destroyed) == 0 &&
			(componentId == Mech_MachineGun ||
			(componentId >= Mech_LRMissile5 && componentId <= Mech_SRMissile6)))
		{
			hasFunctionalAmmoWeapon = TRUE;
		}
	}

	// Sol: No functional shop-supported ammunition weapon; this is not a full-ammo test.
	if (hasFunctionalAmmoWeapon == FALSE)
	{
		Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You don't have any functional weapons that require ammunition."); // Memory ref: [3EDB:1FC9]; Sol: exact ammo EXE text
		if (hasDamagedWeapon == FALSE)
			Keyboard_Get_ASCII_Hex_Input();
	}
	else
	{
		for (uint16_t weaponSlot = 0; weaponSlot < MechWeaponOrdinalCount; weaponSlot++)
		{
			uint8_t criticalComponent = weaponComponents[weaponSlot];
			uint8_t componentId = criticalComponent & MechComponentIdMask;
			if ((criticalComponent & Component_Destroyed) == 0 &&
				(componentId == Mech_MachineGun ||
				(componentId >= Mech_LRMissile5 && componentId <= Mech_SRMissile6)))
			{
				Draw_Top_Graphic_Sidebar();
				Display_Text_From_Memory((uint8_t *)"Your "); // Memory ref: [3EDB:1EA8]; Sol: exact ammo EXE text
				// Sol: Critical component IDs are one greater than their zero-based
				// Sol: 11h-byte WeaponStats record indexes.
				Display_Text_From_Memory(WeaponStats[componentId - MechComponentToWeaponRecordBias].name);

				if (selectedMech->currentAmmo[weaponSlot] != selectedMech->maxAmmo[weaponSlot])
				{
					Display_Text_From_Memory((uint8_t *)" could use some ammunition.  You've expended "); // Memory ref: [3EDB:1ED1]; Sol: exact ammo EXE text

					int16_t missingRounds = (int16_t)(
						selectedMech->maxAmmo[weaponSlot] - selectedMech->currentAmmo[weaponSlot]);

					Set_Text_Colour_Bright_Green();
					Display_Text_Dynamic_Value((uint16_t)missingRounds);
					Display_Text_From_Memory((uint8_t *)"\006\017 rounds.\r\rReloads cost "); // Memory ref: [3EDB:1EFF]; Sol: exact ammo EXE text
					Set_Text_Colour_Bright_Green();

					int16_t ammoPrice = MechAmmoMachineGunRoundCostCBills; // Machine-gun round price.
					if (componentId != Mech_MachineGun)
						ammoPrice = (int8_t)MissileAmmoPriceByComponent[
							componentId - Mech_LRMissile5];

					Display_Text_Dynamic_Value((uint16_t)ammoPrice);
					Display_Text_From_Memory((uint8_t *)"\006\017 C-bills each.  How many do you want to buy? "); // Memory ref: [3EDB:1F19]; Sol: exact ammo EXE text
					uint32_t requestedRounds = Prompt_For_Unsigned_Decimal();
					if (requestedRounds != 0)
					{
						uint16_t roundsRemainingToLoad;
						// Sol: Native cap interprets the decimal parser's DWORD bit pattern as signed.
						int32_t signedRequestedRounds = requestedRounds <= INT32_MAX
							? (int32_t)requestedRounds
							: -1 - (int32_t)(UINT32_MAX - requestedRounds);

						if (signedRequestedRounds > missingRounds)
						{
							Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"\rThat's more than you can carry for that weapon.  We'll max out your ammo."); // Memory ref: [3EDB:1F49]; Sol: exact ammo EXE text
							Keyboard_Get_ASCII_Hex_Input();
							roundsRemainingToLoad = (uint16_t)missingRounds;
						}
						else
							roundsRemainingToLoad = (uint16_t)requestedRounds;

						// Sol: The executable buys one round at a time and redraws the
						// Sol: balance after every successful debit.
						while (roundsRemainingToLoad != 0)
						{
							// Sol: CBW/CWD sign-extends price, then affordability compares unsigned DWORDs.
							uint32_t roundCostCBills = (uint32_t)(int32_t)ammoPrice;
							if (roundCostCBills > CBills)
								break;

							selectedMech->currentAmmo[weaponSlot]++;
							CBills -= roundCostCBills;

							roundsRemainingToLoad--; // Sol: native194A purchase block decrements BEFORE the display call

							Display_Text_CBill_Balance();
						}

						if (roundsRemainingToLoad != 0)
						{
							Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"\r\rYou ran out of money before the ammo bay was full."); // Memory ref: [3EDB:1F94]; Sol: exact ammo EXE text
							Keyboard_Get_ASCII_Hex_Input();
						}

						Display_Text_CBill_Balance();
					}
				}
				else
				{
					Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)" has maximum ammunition right now."); // Memory ref: [3EDB:1EAE]; Sol: exact ammo EXE text
					Keyboard_Get_ASCII_Hex_Input();
				}
			}
		}
	}
	if (hasDamagedWeapon != FALSE)
	{
		Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"\r\r\rSome of your weaponry is damaged, so selling you ammunition for it would be a waste."); // Memory ref: [3EDB:2008]; Sol: exact ammo EXE text
		Keyboard_Get_ASCII_Hex_Input();
	}
}
