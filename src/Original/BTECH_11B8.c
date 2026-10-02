#include "game.h"

// 11B8:0002: void Mechlube_Repair_Mech(void)
// Called from:
//      Citadel_Building_Dialogs
// Sol: Summary: Buy repairs for a party mech.
// Sol: Outline: Inspect damage by component category, calculate charges, obtain confirmation, and apply affordable repairs.
void Mechlube_Repair_Mech(void)
{
	// Sol: Preservation conversion: bind native3092 record/byte views to Mechs;
	// Sol: no segment pseudo-structures or research helpers. Retain explicit
	// Sol: WORD narrowing for deficits, combined damage, quotes and menu subtraction.
	// Sol: Original BUG-008/009 and over-maximum repair behaviour remain unchanged.
	Draw_Top_Graphic_Sidebar();
	Display_Text_From_Memory((uint8_t *)"Fix which 'Mech?\r"); //3EDB:17E5
	Display_Text_Mech_Names();

	Mech *selectedMech = &Mechs[SelectedMechId];
	uint8_t *selectedMechBytes = (uint8_t *)selectedMech;
	uint16_t armourRepairsRemaining = 0;
	uint16_t structureRepairsRemaining = 0;
	uint16_t destroyedHeatSinkCount = 0;
	uint16_t hasDestroyedWeapons = FALSE;
	uint16_t damagedActuatorFlags = 0;
	uint16_t weaponMenuComponentIndexes[MechRepairWeaponScratchCount] = { 0 };
	uint16_t destroyedWeaponCountByType[MechRepairWeaponScratchCount] = { 0 };
	uint8_t *finalRepairStatusText;
	uint8_t *repairResultText;

	// Sol: Sum all eleven missing armour points. The former pseudo-C used the
	// Sol: structure array and overwrote rather than accumulated the total.
	for (uint16_t armourIndex = 0; armourIndex < MechArmourLocationCount; armourIndex++)
	{
		armourRepairsRemaining = (uint16_t)(armourRepairsRemaining +
			selectedMech->maxArmour[armourIndex] - selectedMech->currentArmour[armourIndex]);
	}

	// Sol: Sum all eight missing internal-structure points independently.
	for (uint16_t structureIndex = 0;
		 structureIndex < MechStructureLocationCount;
		 structureIndex++)
	{
		structureRepairsRemaining = (uint16_t)(structureRepairsRemaining +
			selectedMech->maxStructure[structureIndex] - selectedMech->currentStructure[structureIndex]);
	}

	// Sol: Critical offsets 33h-55h are one contiguous 35-byte component block.
	// Sol: Count destroyed heat sinks separately and destroyed weapons by the
	// Sol: verified component-ID range 10h-20h. Kick (21h) is not repair-listed.
	for (uint16_t criticalOffset = MECH_ComponentBlock_Start;
		 criticalOffset <= MECH_ComponentBlock_End;
		 criticalOffset++)
	{
		uint8_t rawComponent = selectedMechBytes[criticalOffset];
		if ((rawComponent & Component_Destroyed) != 0)
		{
			uint8_t componentId = rawComponent & MechComponentIdMask;
			if (componentId == Heat_Sink)
				destroyedHeatSinkCount++;

			if (componentId >= Mech_Small_Laser && componentId <= Mech_SRMissile6)
			{
				destroyedWeaponCountByType[componentId - Mech_Small_Laser]++;
				hasDestroyedWeapons = TRUE;
			}
		}
	}

	// Sol: Bit 0 means the left packed actuator byte differs from maximum;
	// Sol: bit 1 means the right byte differs. These are side flags, not limbs.
	if (selectedMech->currentActuators[MechActuatorSide_Left] != selectedMech->maxActuators[MechActuatorSide_Left])
		damagedActuatorFlags = MechRepairDamagedActuators_Left;
	if (selectedMech->currentActuators[MechActuatorSide_Right] != selectedMech->maxActuators[MechActuatorSide_Right])
		damagedActuatorFlags |= MechRepairDamagedActuators_Right;

	if ((uint16_t)(destroyedHeatSinkCount + armourRepairsRemaining + hasDestroyedWeapons
		+ structureRepairsRemaining + damagedActuatorFlags) == 0)
	{
		uint16_t facilityCannotRepair = FALSE;
		for (uint16_t internalDamageOffset = MECH_Offset_Engine;
			 internalDamageOffset <= MECH_Offset_Sensors;
			 internalDamageOffset++)
		{
			if (selectedMechBytes[internalDamageOffset] != 0)
			{
				Display_Text_From_Memory((uint8_t *)"Our facility doesn't have the equipment to fix your damaged "); // Memory ref: [3EDB:17F7]
				Display_Text_From_Memory(
					UnrepairableMechComponentText[
						internalDamageOffset - MECH_Offset_Engine]);
				facilityCannotRepair = TRUE;
				break;
			}
		}

		if (facilityCannotRepair != FALSE)
		{
WaitForRepairAcknowledgement:
			Keyboard_Get_ASCII_Hex_Input();
			return;
		}
		finalRepairStatusText = (uint8_t *)"\r\rThat 'Mech looks shiny new to me.  It doesn't need anything fixed."; //3EDB:1834
	}
	else
	{		
		
		// Sol: The native test ORs the low and high WORDs of the 32-bit balance.
		// Sol: A zero balance ends the whole shop visit before any repair prompt.
		if (CBills == 0) // D370:D372
		{
			Display_Text_From_Memory((uint8_t *)"\r\rYou don't have any C-bills.  "); //3EDB:1879
			Display_Text_Shop_Cannot_Afford_Text();
			goto WaitForRepairAcknowledgement;
		}

		if (armourRepairsRemaining != 0)
		{
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"Looks like this 'Mech has lost about "); //3EDB:1899
			Display_Text_Dynamic_Value(armourRepairsRemaining);
			Display_Text_From_Memory((uint8_t *)" pieces of armor.  If you want those replaced, it's going to cost "); //3EDB:18BF
			Display_Text_Dynamic_Value((uint16_t)(armourRepairsRemaining * MechRepairArmourPointCostCBills));
			Display_Text_From_Memory((uint8_t *)" C-bills.  You've got "); //3EDB:1902
			Citadel_Building_Dialogs(CitadelDialog_DisplayCBillBalance);
			Display_Text_From_Memory((uint8_t *)"  Want it fixed?"); //3EDB:1919
			if (Prompt_Yes_No(TRUE))
			{
				// Sol: Repair in record order, one point and four C-bills at a time.
				// Sol: This preserves affordable partial work if funds run out midway.
				for (uint16_t armourIndex = 0;
					 armourIndex < MechArmourLocationCount;
					 armourIndex++)
				{
					uint16_t missingArmourAtLocation =
						(uint16_t)(selectedMech->maxArmour[armourIndex] - selectedMech->currentArmour[armourIndex]);

					while (missingArmourAtLocation != 0)
					{
						// Sol: The assembly performs an unsigned 32-bit comparison with four.
						// Sol: On failure it abandons this location; the outer loop continues.
						if (CBills < MechRepairArmourPointCostCBills)
							missingArmourAtLocation = 0;
						else
						{
							CBills -= MechRepairArmourPointCostCBills;
							missingArmourAtLocation--;
							armourRepairsRemaining--;
							selectedMech->currentArmour[armourIndex]++;
							Display_Text_CBill_Balance();
						}
					}
				}

				if (armourRepairsRemaining != 0)
				{
					Draw_Top_Graphic_Sidebar();
					Display_Text_From_Memory((uint8_t *)"We fixed as much as you could afford, but you ran out of cash before the job was done."); //3EDB:192A
					Keyboard_Get_ASCII_Hex_Input(); //1F3D:0259
				}
			}
		}
		if (structureRepairsRemaining != 0)
		{
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"This 'Mech has had some of its internal structure damaged."); //3EDB:1981
			Display_Text_From_Memory((uint8_t *)"  If you want to fix any weapons or heat sinks, you have to have structure to mount them on."); //3EDB:19BC
			Display_Text_From_Memory((uint8_t *)" If you want the structure fixed, it's going to cost "); //3EDB:1A19
			Display_Text_Dynamic_Value((uint16_t)(structureRepairsRemaining * MechRepairStructurePointCostCBills));
			Display_Text_From_Memory((uint8_t *)" C-bills.  You've got "); //3EDB:1A4F
			Citadel_Building_Dialogs(CitadelDialog_DisplayCBillBalance);
			Display_Text_From_Memory((uint8_t *)"  Want it fixed?"); //3EDB:1A66

			if (Prompt_Yes_No(TRUE))
			{
				// Sol: Raw offsets 1Ch-23h are the eight CurrentStructure bytes.
				// Sol: Repair them in record order for nine C-bills per point.
				for (uint16_t structureIndex = 0;
					 structureIndex < MechStructureLocationCount;
					 structureIndex++)
				{
					uint16_t missingStructureAtLocation =
						(uint16_t)(selectedMech->maxStructure[structureIndex] - selectedMech->currentStructure[structureIndex]);

					while (missingStructureAtLocation != 0)
					{
						// Sol: The native comparison and subtraction cover both balance WORDs.
						// Sol: Insufficient funds stop this location but do not roll work back.
						if (CBills < MechRepairStructurePointCostCBills)
							missingStructureAtLocation = 0;
						else
						{
							CBills -= MechRepairStructurePointCostCBills;
							missingStructureAtLocation--;
							structureRepairsRemaining--;
							selectedMech->currentStructure[structureIndex]++;
							Display_Text_CBill_Balance();
						}
					}
				}

				if (structureRepairsRemaining != 0)
				{
					Draw_Top_Graphic_Sidebar();
					Display_Text_From_Memory((uint8_t *)"We fixed as much as you could afford, but you ran out of cash before the job was done."); //3EDB:1A77
					Keyboard_Get_ASCII_Hex_Input(); //1F3D:0259
				}
			}
		}
		if (destroyedHeatSinkCount != 0)
		{
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"You've had "); //3EDB:1ACE
			Display_Text_Dynamic_Value(destroyedHeatSinkCount);
			Display_Text_From_Memory((uint8_t *)" heat sink"); //3EDB:1ADA
			if (destroyedHeatSinkCount > 0x0001)
				Display_Text_From_Memory((uint8_t *)"s"); //3EDB:1AE5

			Display_Text_From_Memory((uint8_t *)"   damaged.  We have a special on heat sinks this month: only 800 C-bills each!  You've got "); //3EDB:1AE7
			Citadel_Building_Dialogs(CitadelDialog_DisplayCBillBalance);
			Display_Text_From_Memory((uint8_t *)"  Want to fix some heat sinks?"); //3EDB:1B44

			if (Prompt_Yes_No(TRUE))
			{
				// Sol: Scan all 35 critical slots in physical record order. Only the
				// Sol: exact destroyed-heat-sink byte A2h is replaced with intact 22h.
				for (uint16_t criticalOffset = MECH_ComponentBlock_Start;
					 criticalOffset <= MECH_ComponentBlock_End;
					 criticalOffset++)
				{
					if (destroyedHeatSinkCount != 0
						&& selectedMechBytes[criticalOffset] == Destroyed_Heat_Sink)
					{
						// Sol: The native comparison and SUB/SBB charge the full
						// Sol: 32-bit balance. Unaffordable slots remain destroyed.
						if (CBills >= MechRepairHeatSinkCostCBills)
						{
							CBills -= MechRepairHeatSinkCostCBills;
							destroyedHeatSinkCount--;
							selectedMechBytes[criticalOffset] = Heat_Sink;
							Display_Text_CBill_Balance();
						}
					}
				}

				if (destroyedHeatSinkCount != 0)
				{
					Draw_Top_Graphic_Sidebar();
					repairResultText = (uint8_t *)"The job's not complete.  You need to come up with some cash before we can finish."; //3EDB:1B63
DisplayRepairResultAndWait:
					Display_Text_From_Memory(repairResultText);
					Keyboard_Get_ASCII_Hex_Input(); //1F3D:0259
				}
			}
		}
		while (hasDestroyedWeapons != FALSE)
		{
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"Here's your weapons estimate:"); //3EDB:1BB5

			// Sol: This twenty-WORD array maps visible menu rows back to the
			// Sol: zero-based mech weapon-type indexes used by the damage counts.
			for (uint16_t menuMapIndex = 0;
				 menuMapIndex < MechRepairWeaponScratchCount;
				 menuMapIndex++)
			{
				weaponMenuComponentIndexes[menuMapIndex] = 0;
			}

			uint16_t visibleWeaponRowCount = 0;

			// Sol: Indexes 0..10h correspond to component IDs 10h..20h.
			for (uint16_t weaponTypeIndex = 0;
				 weaponTypeIndex < MechRepairWeaponTypeCount;
				 weaponTypeIndex++)
			{
				if (destroyedWeaponCountByType[weaponTypeIndex] != 0)
				{
					Display_Text_4FA0_Value();
					Display_Text_Dynamic_Value(
						destroyedWeaponCountByType[weaponTypeIndex]);
					TextColumn++;

					// Sol: 2FD7h is weapon-table index 0Fh (SmallLaser). Each
					// Sol: following mech weapon record has the verified 11h stride.
					Display_Text_From_Memory(
						WeaponStats[
							weaponTypeIndex + (Mech_Small_Laser - MechComponentToWeaponRecordBias)].name);
					weaponMenuComponentIndexes[visibleWeaponRowCount] =
						weaponTypeIndex;
					visibleWeaponRowCount++;
				}
			}

			// Sol: These 305B words are generic menu bounds/state here; their
			// Sol: legacy party-member field names do not describe this use.
			MenuFirstRow = MechRepairWeaponMenuFirstRow;
			MenuLastRow = TextRow;
			MenuSelection = 0;

			Display_Text_From_Memory((uint8_t *)"\rNothing\r\rAt 300 C-bills per item, what do you want to fix?"); //3EDB:1BD3
			TextColumn = 0;
			TextRow = MechRepairCBillDisplayRow;

			Display_Text_From_Memory((uint8_t *)"C-bills: "); //3EDB:1C0F
			Citadel_Building_Dialogs(CitadelDialog_DisplayCBillBalance);

			// Sol: The native caller stores AX as a WORD; the1E56 audit now confirms
			// Sol: Display_Menu_Choices_And_Check has a matching unsigned-short signature.
			uint16_t weaponMenuChoice =
				(uint16_t)Display_Menu_Choices_And_Check(MechRepairWeaponMenuBottomRow);
			if ((uint16_t)(MenuLastRow - 1)
				!= weaponMenuChoice)
			{
				if (CBills >= MechRepairWeaponSelectionCostCBills)
				{
					CBills -= MechRepairWeaponSelectionCostCBills;
					Display_Text_CBill_Balance();

					uint16_t selectedWeaponTypeIndex =
						weaponMenuComponentIndexes[weaponMenuChoice];
					destroyedWeaponCountByType[selectedWeaponTypeIndex]--;

					uint16_t destroyedComponentByte =
						selectedWeaponTypeIndex + Component_Destroyed
						+ Mech_Small_Laser;
					// Sol: Original BUG-008: one purchase clears EVERY matching slot,
					// Sol: although the menu count above is decremented only once.
					for (uint16_t criticalOffset = MECH_ComponentBlock_Start;
						 criticalOffset <= MECH_ComponentBlock_End;
						 criticalOffset++)
					{
						if (selectedMechBytes[criticalOffset]
							== destroyedComponentByte)
						{
							selectedMechBytes[criticalOffset] &= MechComponentIdMask;
						}
					}
					hasDestroyedWeapons = FALSE;

					// Sol: Original BUG-009: the native '< 10h' bound checks only
					// Sol: indexes 0..0Fh and omits SRM-6 index 10h.
					for (uint16_t remainingWeaponTypeIndex = 0;
						 remainingWeaponTypeIndex < MechRepairWeaponContinuationTypeCount;
						 remainingWeaponTypeIndex++)
					{
						if (destroyedWeaponCountByType[remainingWeaponTypeIndex]
							!= 0)
						{
							hasDestroyedWeapons = TRUE;
						}
					}

					if (hasDestroyedWeapons != FALSE)
						continue;

					TextColumn = 0;
					TextRow = MechRepairWeaponResultDisplayRow;
					repairResultText = (uint8_t *)"All your weapons are fixed."; //3EDB:1C19
					goto DisplayRepairResultAndWait;
				}
				else
				{
					hasDestroyedWeapons = FALSE;
					repairResultText = (uint8_t *)"\r\rYou're out of cash."; //3EDB:1C35
					goto DisplayRepairResultAndWait;
				}
			}
			hasDestroyedWeapons = FALSE;
		}
		if (damagedActuatorFlags == 0)
			return;

		Draw_Top_Graphic_Sidebar();
		Display_Text_From_Memory((uint8_t *)"You've got some damaged actuators.  "); //3EDB:1C4B

		// Sol: This is one flat repair price whether one or both packed side
		// Sol: bytes differ from maximum. The native check covers both C-bill WORDs.
		if (CBills >= MechRepairActuatorsCostCBills)
		{
			Display_Text_From_Memory((uint8_t *)"For 200 C-bills, we'll patch them all."); //3EDB:1C70
			Display_Text_From_Memory((uint8_t *)"\r\rIs it a deal?"); //3EDB:1C97

			if (Prompt_Yes_No(TRUE))
			{
				Display_Text_From_Memory((uint8_t *)"\r\rThey're all patched up, good as new."); //3EDB:1CA7
				Keyboard_Get_ASCII_Hex_Input(); //1F3D:0259

				// Sol: Restore the complete right then left packed actuator bytes
				// Sol: from the chassis maxima. The correct value is not always FFh.
				selectedMech->currentActuators[MechActuatorSide_Right] = selectedMech->maxActuators[MechActuatorSide_Right];
				selectedMech->currentActuators[MechActuatorSide_Left] = selectedMech->maxActuators[MechActuatorSide_Left];

				CBills -= MechRepairActuatorsCostCBills;
				Display_Text_CBill_Balance();
			}
			return;
		}
		finalRepairStatusText = (uint8_t *)"However, you don't have the 200 C-bills it takes to get them fixed."; //3EDB:1CCE
	}
	// Sol: Native AX carried a DS string offset to the shared07F6 epilogue;
	// Sol: the host carries the corresponding readable text pointer instead.
	Display_Text_From_Memory(finalRepairStatusText);
	goto WaitForRepairAcknowledgement;
}
