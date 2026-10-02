#include "game.h"
#include <stdio.h>
#include <stdlib.h>

/* Sol: original0DAB:0002..04F8 complete parent. Original repair priority,
 * signed deficits, pool underflow and slot-index payout multiplier retained.
 * SRM-6's unassigned native stack BYTE remains a loud unsupported path, not
 * a zero-filled extra bucket. */
void Salvage_Armour_Dialog(void)
{
	// Sol:0002–0258 preservation correction: signed Tech/name BYTEs, FAR3092
	// Sol: records and signed WORD salvage caps now match native operations.
	// Sol: Heat-sink/weapon/payout paths below retain native ordering and widths.
	// Sol TODO: The inherited SRM-6 stack byte still requires an explicit native
	// Sol: frame binding. Explicit guards below prevent inventing its value.
	// Sol: 0DAB:0002-004F uses 16-bit stack locals throughout. The old
	// Sol: address-of comparisons were Reko artefacts: Name and Skill_Tech are
	// Sol: individual bytes in each 0x11-byte character record.
	uint16_t wreckCount = 0;
	int16_t highestTechSkill = SkillLevel_Unskilled;
	uint16_t partyTechnicianId = Character_Jason; // Sol: native default slot0 remains even when no positive Tech skill wins

	// Sol: Search all eight party slots for the living member with the strictly
	// Sol: highest Tech skill. Equal skills retain the earlier party member.
	for (uint16_t partyMemberId = 0; partyMemberId < PartySize; partyMemberId++)
	{
		if (Characters[partyMemberId].name != Character_Dead)
		{
			int16_t techSkill = (int8_t)Characters[partyMemberId].skillTech;
			if (highestTechSkill < techSkill)
			{
				partyTechnicianId = partyMemberId;
				highestTechSkill = techSkill;
			}
		}
	}

	// Sol: 0DAB:0050-00CA. The character Name byte is an index into the // Sol: pointer table at 3EDB:01CA, not an inline string or pointer.
	Display_Text_From_Memory((uint8_t *)"\r\r"); // 3EDB:0EFB
	int16_t technicianNameId = (int8_t)Characters[partyTechnicianId].name;
	Display_Text_From_Memory(CharacterNames[technicianNameId]);
	Display_Text_From_Memory((uint8_t *)" uses his tech training to scavenge armor"); // 3EDB:0EFE

	// Sol: Native pre-biased0F9A+TechSkill*4 selects three FAR pointers at0FA2..0FAD
	// Sol: for known skill2..4. Native has NO upper-bound check for higher values.
	if (highestTechSkill > SkillLevel_Amateur)
	{
		Display_Text_From_Memory(
			SalvageDescriptionByTechSkill[
				highestTechSkill - SkillLevel_Average]);
	}

	Display_Text_From_Memory((uint8_t *)" from the enemy 'Mechs."); // 3EDB:0F28
	Prompt_And_Wait_For_Key();

	// Sol: 0DAB:00CB-0114. Pool the surviving armour bytes from each enemy mech
	// Sol: whose word-sized combatant flag marks it as a wreck.
	uint16_t salvagedArmourPoints = 0;
	for (uint16_t enemyMechId = 0; enemyMechId < Enemy_Mech_Record_Count; enemyMechId++)
	{
		uint16_t enemyCombatantId =
			Enemy_All_CombatantId_Range_First + enemyMechId;
		if (CombatantCasualtyFlags[enemyCombatantId] != FALSE)
		{
			// Sol: The original C33C + CombatantId*7D expression is pre-biased:
			// Sol: combatant 0C resolves to C918, the first live enemy mech.
			Mech *enemyMech = &Mechs[Enemy_Mech_Record_First + enemyMechId];
			for (uint16_t armourLocation = 0; armourLocation < MechArmourLocationCount; armourLocation++)
			{
				salvagedArmourPoints += enemyMech->currentArmour[armourLocation];
			}
			wreckCount++;
		}
	}

	// Sol: 0DAB:0115-018C. Apply the pool in reverse location order, from
	// Sol: CurrentArmour[10] (left-torso rear) through [0] (right arm). For each
	// Sol: location, friendly mech slots are repaired in party order 0..3.
	for (int16_t armourLocation = MechArmourLocationCount - 1; armourLocation >= 0; armourLocation--)
	{
		for (uint16_t lanceMechId = 0; lanceMechId < LanceSize; lanceMechId++)
		{
			Mech *lanceMech = &Mechs[lanceMechId];
			if (lanceMech->name[0] != MECH_Destroyed)
			{
				int16_t armourPointsToTransfer =
					(int16_t)(lanceMech->maxArmour[armourLocation]
					- lanceMech->currentArmour[armourLocation]);
				// Sol: Signed native cap: over-maximum armour transfers a negative
				// Sol: deficit, reducing this location and increasing the shared pool.
				if (armourPointsToTransfer != 0)
				{
					if (armourPointsToTransfer > (int16_t)salvagedArmourPoints)
						armourPointsToTransfer = (int16_t)salvagedArmourPoints;

					lanceMech->currentArmour[armourLocation]
						+= (uint8_t)armourPointsToTransfer;
					salvagedArmourPoints = (uint16_t)(salvagedArmourPoints - armourPointsToTransfer);
				}
			}
		}
	}

	// Sol: 0DAB:018D-0258. Only an Excellent (level 4) technician can salvage
	// Sol: and apply surviving internal structure in the field.
	if (highestTechSkill >= SkillLevel_Excellent)
	{
		uint16_t salvagedStructurePoints = 0;

		for (uint16_t enemyMechId = 0; enemyMechId < Enemy_Mech_Record_Count; enemyMechId++)
		{
			uint16_t enemyCombatantId =
				Enemy_All_CombatantId_Range_First + enemyMechId;
			if (CombatantCasualtyFlags[enemyCombatantId] != FALSE)
			{
				Mech *enemyMech = &Mechs[Enemy_Mech_Record_First + enemyMechId];
				for (uint16_t structureLocation = 0;
					 structureLocation < MechStructureLocationCount;
					 structureLocation++)
				{
					salvagedStructurePoints
						+= enemyMech->currentStructure[structureLocation];
				}
			}
		}

		// Sol: As with armour, location priority is descending and the four
		// Sol: friendly mech slots are visited in ascending order per location.
		for (int16_t structureLocation = MechStructureLocationCount - 1; structureLocation >= 0; structureLocation--)
		{
			for (uint16_t lanceMechId = 0;
				 lanceMechId < LanceSize;
				 lanceMechId++)
			{
				Mech *lanceMech = &Mechs[lanceMechId];
				if (lanceMech->name[0] != MECH_Destroyed)
				{
					int16_t structurePointsToTransfer =
						(int16_t)(lanceMech->maxStructure[structureLocation]
						- lanceMech->currentStructure[structureLocation]);
					if (structurePointsToTransfer != 0)
					{
						if (structurePointsToTransfer > (int16_t)salvagedStructurePoints)
							structurePointsToTransfer = (int16_t)salvagedStructurePoints;

						lanceMech->currentStructure[structureLocation]
							+= (uint8_t)structurePointsToTransfer;
						salvagedStructurePoints = (uint16_t)(salvagedStructurePoints - structurePointsToTransfer);
					}
				}
			}
		}
	}

	// Sol: 0DAB:0259-031E. An Average (level 2) or better technician restores
	// Sol: destroyed heat-sink critical slots whose location passes 183B:273D.
	if (highestTechSkill >= SkillLevel_Average)
	{
		uint16_t heatSinksSalvagedFromEnemyMechs = 0;

		for (uint16_t enemyMechId = 0; enemyMechId < Enemy_Mech_Record_Count; enemyMechId++)
		{
			uint16_t enemyCombatantId =
				Enemy_All_CombatantId_Range_First + enemyMechId;
			if (CombatantCasualtyFlags[enemyCombatantId] != FALSE)
			{
				Mech *enemyMech = &Mechs[Enemy_Mech_Record_First + enemyMechId];
				// Sol TODO(A-005): Critical_L_Arm is the current header name for
				// Sol TODO: the first stored critical group at +33; anatomy is disputed.
				uint8_t *enemyCriticalSlots = (uint8_t *)enemyMech + MECH_ComponentBlock_Start;
				for (uint16_t criticalSlotIndex = 0;
					 criticalSlotIndex <
						MechCriticalSlotCount;
					 criticalSlotIndex++)
				{
					if (enemyCriticalSlots[criticalSlotIndex] == Heat_Sink)
						heatSinksSalvagedFromEnemyMechs++;
				}
			}
		}

		for (uint16_t friendlyMechId = 0; friendlyMechId < LanceSize; friendlyMechId++)
		{
			Mech *friendlyMech = &Mechs[friendlyMechId];
			if (friendlyMech->name[0] != MECH_Destroyed)
			{
				// Sol TODO(A-005): Treat +33 as the neutral contiguous-block base
				// Sol TODO: until its left/right anatomical label is confirmed.
				uint8_t *friendlyCriticalSlots = (uint8_t *)friendlyMech + MECH_ComponentBlock_Start;
				for (uint16_t criticalSlotIndex = 0;
					 criticalSlotIndex <
						MechCriticalSlotCount;
					 criticalSlotIndex++)
				{
					uint16_t criticalSlotRecordOffset =
						MECH_ComponentBlock_Start + criticalSlotIndex;
					if (friendlyCriticalSlots[criticalSlotIndex] == Destroyed_Heat_Sink
						&& Check_If_CriticalSlot_Destroyed(
							friendlyMechId,
							criticalSlotRecordOffset) != FALSE)
					{
						friendlyCriticalSlots[criticalSlotIndex] = Heat_Sink;

						// Sol: The executable never tests this count before repairing.
						// Sol: Zero therefore underflows to FFFF; preserve that quirk.
						heatSinksSalvagedFromEnemyMechs--;
					}
				}
			}
		}
	}

	// Sol: 0DAB:031F-0429. A Good (level 3) or better technician can use
	// Sol: intact weapon-component slots from enemy wrecks to repair matching
	// Sol: destroyed component slots on friendly mechs.
	if (highestTechSkill >= SkillLevel_Good)
	{
		uint8_t salvagedWeaponSlotsByComponentId[Mech_SRMissile6-Mech_Small_Laser]; /* Sol: native16 assigned buckets only. */

		// Sol: The original zeroes exactly 16 stack bytes for IDs 10..1F.
		// Sol: Native also accepts ID20, whose BP-14 bucket inherits stack bytes.
		// Sol: Only assigned buckets are represented here. A read/increment of
		// Sol: the extra native bucket explicitly stops, pending that contract.
		for (uint16_t componentId = Mech_Small_Laser;
			 componentId < Mech_SRMissile6;
			 componentId++)
		{
			salvagedWeaponSlotsByComponentId[componentId-Mech_Small_Laser] = 0;
		}

		for (uint16_t enemyMechId = 0; enemyMechId < Enemy_Mech_Record_Count; enemyMechId++)
		{
			uint16_t enemyCombatantId =
				Enemy_All_CombatantId_Range_First + enemyMechId;
			if (CombatantCasualtyFlags[enemyCombatantId] != FALSE)
			{
				Mech *enemyMech = &Mechs[Enemy_Mech_Record_First + enemyMechId];
				// Sol TODO(A-005): +33 is the neutral contiguous critical-block
				// Sol TODO: base; its current anatomical field label is disputed.
				uint8_t *enemyCriticalSlots = (uint8_t *)enemyMech + MECH_ComponentBlock_Start;
				for (uint16_t criticalSlotIndex = 0;
					 criticalSlotIndex <
						MechCriticalSlotCount;
					 criticalSlotIndex++)
				{
					uint8_t componentId = enemyCriticalSlots[criticalSlotIndex];
					if (componentId >= Mech_Small_Laser
						&& componentId <= Mech_SRMissile6)
					{
						if(componentId==Mech_SRMissile6) {
							fputs("Unresolved original0DAB:0002 SRM-6 stack bucket\n",stderr);
							abort();
						}
						salvagedWeaponSlotsByComponentId[componentId-Mech_Small_Laser]++;
					}
				}
			}
		}

		for (uint16_t friendlyMechId = 0; friendlyMechId < LanceSize; friendlyMechId++)
		{
			Mech *friendlyMech = &Mechs[friendlyMechId];
			if (friendlyMech->name[0] != MECH_Destroyed)
			{
				// Sol TODO(A-005): See the enemy-block note above.
				uint8_t *friendlyCriticalSlots = (uint8_t *)friendlyMech + MECH_ComponentBlock_Start;
				for (uint16_t criticalSlotIndex = 0;
					 criticalSlotIndex <
						MechCriticalSlotCount;
					 criticalSlotIndex++)
				{
					uint8_t rawComponent = friendlyCriticalSlots[criticalSlotIndex];
					if ((rawComponent & Component_Destroyed) != FALSE)
					{
						uint16_t componentId =
							rawComponent & ~Component_Destroyed;
						uint16_t criticalSlotRecordOffset =
							MECH_ComponentBlock_Start + criticalSlotIndex;
						if(componentId==Mech_SRMissile6) {
							fputs("Unresolved original0DAB:0002 SRM-6 stack bucket\n",stderr);
							abort();
						}
						if (componentId >= Mech_Small_Laser
							&& componentId <= Mech_SRMissile6
							&& salvagedWeaponSlotsByComponentId[componentId-Mech_Small_Laser] != 0
							&& Check_If_CriticalSlot_Destroyed(
								friendlyMechId,
								criticalSlotRecordOffset) != FALSE)
						{
							friendlyCriticalSlots[criticalSlotIndex]
								&= ~Component_Destroyed;
							salvagedWeaponSlotsByComponentId[componentId-Mech_Small_Laser]--;
						}
					}
				}
			}
		}
	}

	// Sol: 0DAB:042A-04F2. The payout is skipped if there were no wrecks or
	// Sol: if party slot zero was selected as best technician.
	if (wreckCount != 0 && partyTechnicianId != 0)
	{
		int16_t scrapPayout = 0;
		for (uint16_t wreckIndex = 0;
			 wreckIndex < wreckCount;
			 wreckIndex++)
		{
			scrapPayout +=
				(Rand_0x00_to_0xFF() & SalvageScrapRandomBonusMask) + SalvageScrapBaseCBillsPerWreck;
		}

		// Sol: The executable multiplies by [bp-4], partyTechnicianId, rather
		// Sol: than [bp-6], highestTechSkill. This slot-index multiplier and the
		// Sol: slot-zero gate above appear to be an original variable-selection bug.
		scrapPayout = (int16_t)(scrapPayout * partyTechnicianId);

		Display_Text_From_Memory((uint8_t *)"\r\rYou are able to scrounge together "); // 3EDB:0F40
		Set_Text_Colour_Bright_Green();
		Display_Text_Dynamic_Value((uint16_t)scrapPayout);

		// Sol: Source 0F65 begins with text control bytes 06 0F, which restore
		// Sol: bright-white text after the green numeric value.
		Append_Large_Text_To_Memory(
			DynamicString,
			(uint8_t *)"\x06\x0F C-bills worth of scrap metal from the destroyed 'Mech");
		if (wreckCount > 1)
			Append_Text_To_Memory(DynamicString, (uint8_t *)"s");
		Append_Text_To_Memory(DynamicString, (uint8_t *)".");
		Display_Text_From_Memory(DynamicString);
		Prompt_And_Wait_For_Key();

		// Sol: CWD sign-extends the 16-bit payout before adding it to D370:D372.
		CBills += (uint32_t)(int32_t)scrapPayout;
	}
}
