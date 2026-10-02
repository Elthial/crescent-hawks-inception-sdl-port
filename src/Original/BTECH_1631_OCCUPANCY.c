#include "game.h"
#include "dos.h"

/* Sol: Original1631:16AB..1B43. Shared proposed position, WORD result:
 * 1 blocks the step,0 allows it, including after opposing infantry crushing. */
uint16_t Combat_Check_Occupancy_And_Crush(uint16_t actorId, int16_t stepX, int16_t stepY,
	uint16_t crushOpposingInfantry)
{
	// Sol:16AB..1B43 four WORD arguments, AX WORD blocked result.
	// Sol: stepX/stepY are passed but never read; candidate is global A44B/A44D.
	(void)stepX; (void)stepY; /* Native arguments are never read. */
	uint16_t blocked = FALSE; // SS:BP-6; independent of distance/Y scratch.
	if (((int16_t)actorId >= Friendly_Infantry_Combatant_Range_First && (int16_t)actorId < Enemy_All_CombatantId_Range_First)
	 || (int16_t)actorId >= Enemy_Infantry_CombatantId_Range_First)
	{
		// Sol:16EE exact-position infantry checks have NO active/sentinel filter.
		for (uint16_t friendlyId = Friendly_Infantry_Combatant_Range_First; friendlyId < Enemy_All_CombatantId_Range_First; ++friendlyId)
		{
			if (friendlyId != actorId
			 && CombatantPackedX[friendlyId] == CrescentHawkMapPositionX
			 && CombatantPackedY[friendlyId] == CrescentHawkMapPositionY)
				blocked = TRUE;
			uint16_t enemyId = friendlyId + Enemy_All_CombatantId_Range_First; // Sol:401C/404E aliases -> canonical16..23.
			if (blocked == FALSE && enemyId != actorId
			 && CombatantPackedX[enemyId] == CrescentHawkMapPositionX
			 && CombatantPackedY[enemyId] == CrescentHawkMapPositionY)
				blocked = TRUE;
			if (blocked != FALSE) break;
		}
		if (blocked != FALSE) return blocked;
		uint16_t savedX = CrescentHawkMapPositionX; // BP-4 candidate anchor.
		uint16_t savedY = CrescentHawkMapPositionY; // BP-0A.
		for (uint16_t relativeMech = 0; relativeMech < LanceSize && blocked == FALSE; ++relativeMech)
			for (uint16_t sideBase = 0; sideBase <= Enemy_All_CombatantId_Range_First && blocked == FALSE; sideBase += Enemy_All_CombatantId_Range_First)
			{
				uint16_t mechId = relativeMech + sideBase; // Sol:0..3 and12..15; no active guard in this footprint branch.
				CrescentHawkMapPositionX = CombatantPackedX[mechId];
				CrescentHawkMapPositionY = CombatantPackedY[mechId];
				if (savedY == CrescentHawkMapPositionY)
				{
					if (savedX == CrescentHawkMapPositionX)
						blocked = TRUE;
					else
					{
						Offset_Packed_Position(-1, 0); // Sol:17A4 left footprint cell, including packed region wrap.
						if (savedX == CrescentHawkMapPositionX)
							blocked = TRUE;
						else
						{
							Offset_Packed_Position(2, 0); // Sol:17C0 left+2 -> right footprint cell.
							if (savedX == CrescentHawkMapPositionX) blocked = TRUE;
						}
					}
				}
			}
		CrescentHawkMapPositionX = savedX;
		CrescentHawkMapPositionY = savedY; // Sol:184B always restore candidate anchor.
		return blocked; // Sol: infantry never enters optional crushing branch.
	}

	// Sol:1864 mech candidate X+1, then packed low-bit80 correction+80h.
	uint16_t correctedX = CrescentHawkMapPositionX + 1;
	if ((correctedX & PackedPositionLocalCarryBit) != 0) correctedX += PackedPositionLocalCarryBit;
	int16_t mechUnpackedXPlusOne = (int16_t)(((correctedX & PackedPositionXRegionMask) >> 1) | (correctedX & PackedPositionLocalMask)); // BP-1C; regionX*128 + localX.
	for (uint16_t relativeMech = 0; relativeMech < LanceSize && blocked == FALSE; ++relativeMech)
		for (uint16_t sideBase = 0; sideBase <= Enemy_All_CombatantId_Range_First && blocked == FALSE; sideBase += Enemy_All_CombatantId_Range_First)
		{
			uint16_t otherId = relativeMech + sideBase;
			if (CombatantActive[otherId] != FALSE && otherId != actorId
			 && CombatantPackedY[otherId] == CrescentHawkMapPositionY)
			{
				uint16_t otherX = CombatantPackedX[otherId] + 1;
				if ((otherX & PackedPositionLocalCarryBit) != 0) otherX += PackedPositionLocalCarryBit;
				int16_t otherUnpackedXPlusOne = (int16_t)(((otherX & PackedPositionXRegionMask) >> 1) | (otherX & PackedPositionLocalMask));
				int16_t distance = (int16_t)(otherUnpackedXPlusOne - mechUnpackedXPlusOne);
				distance = Native_Abs_Word(distance); // Sol:18F5 FAR207F:3C6C signed WORD abs; preserve16-bit overflow, not BYTE helper signature.
				if (distance < MechFootprintWidth) blocked = TRUE; // Sol: overlapping three-wide mech footprints, same packed Y.
			}
		}
	uint16_t sameSideInfantryBase = (int16_t)actorId < Enemy_All_CombatantId_Range_First
		? Friendly_Infantry_Combatant_Range_First : Enemy_Infantry_CombatantId_Range_First; // BP-10 side base4/16.
	int16_t mechUnpackedX = (int16_t)(mechUnpackedXPlusOne - 1); // Sol:193C decrement AFTER unpack/correction.
	for (uint16_t infantryId = sameSideInfantryBase; infantryId < sameSideInfantryBase + PartySize; ++infantryId)
		if (CombatantActive[infantryId] != FALSE
		 && CombatantPackedY[infantryId] == CrescentHawkMapPositionY)
		{
			uint16_t infantryX = CombatantPackedX[infantryId];
			int16_t infantryUnpackedX = (int16_t)(((infantryX & PackedPositionXRegionMask) >> 1) | (infantryX & PackedPositionLocalMask));
			int16_t distance = (int16_t)(mechUnpackedX - infantryUnpackedX);
			distance = Native_Abs_Word(distance);
			if (distance < MechFootprintInfantryDistance) { blocked = TRUE; break; } // Sol: friendly personnel under own mech footprint block movement.
		}
	if (blocked != FALSE || crushOpposingInfantry == FALSE) return blocked;
	uint16_t opposingInfantryBase = sameSideInfantryBase ^ OpposingInfantrySideToggle; // Sol:19C9 XOR low BYTE14 switches4<->16; not record-index XOR.
	for (uint16_t infantryId = opposingInfantryBase; infantryId < opposingInfantryBase + PartySize; ++infantryId)
		if (CombatantActive[infantryId] != FALSE
		 && CombatantPackedY[infantryId] == CrescentHawkMapPositionY)
		{
			uint16_t infantryX = CombatantPackedX[infantryId];
			int16_t infantryUnpackedX = (int16_t)(((infantryX & PackedPositionXRegionMask) >> 1) | (infantryX & PackedPositionLocalMask));
			int16_t distance = (int16_t)(mechUnpackedX - infantryUnpackedX);
			distance = Native_Abs_Word(distance);
			if (distance >= MechFootprintInfantryDistance) continue;
			uint16_t infantryRecordId = infantryId - Friendly_Infantry_Combatant_Range_First; // BP-20; friendly combatants4..11 -> records0..7.
			Menu_Memory_Variables(4);
			Draw_Top_Graphic_Sidebar();
			if (infantryRecordId < Enemy_All_CombatantId_Range_First)
				Combat_CombatMessageVerbosityFilter(CharacterNames[(int16_t)(int8_t)Characters[infantryRecordId].name]); // Sol:19D6 CBW BYTE name index -> FAR table.
			else
			{
				infantryRecordId -= LanceSize; // Sol:1B2F enemy combatants16..23 -> records8..15 (id-8).
				Combat_CombatMessageVerbosityFilter((uint8_t *)"Enemy human"); // DS:319A, actual DS3EDB; exact EXE text.
			}
			Combat_CombatMessageVerbosityFilter((uint8_t *)" is squashed underfoot."); // DS:31A6
			Play_Sound_If_Enabled(Sound_SquishedByMech); // Sol: sound12h.
			if ((int16_t)CombatSpeedSetting < CombatSpeedKeyWaitSetting)
			{
				GameSpeed_RateControl();
				if (infantryRecordId < PartySize) GameSpeed_RateControl(); // Sol: friendly death gets a second delay.
			}
			Register_Persistent_Map_Effect(Sprite_Impact_Small,
				CombatantPackedX[infantryId], CombatantPackedY[infantryId]);
			CombatantSpriteFrame[infantryId] = Sprite_Impact_Small; // Sol:1A35 death tile7E.
			CombatantSpriteFamilyOffset[infantryId] = 0; // Sol:1A57 family base0, not mech-type inference.
			CombatantActive[infantryId] = FALSE;
			Characters[infantryRecordId].health = 0;
			Characters[infantryRecordId].name = Character_Dead;
			if (infantryRecordId < 2) MainCharactersAlive = FALSE; // Sol:1A9F Jason/Rex record0/1.
			// Sol: no position invalidation,393C casualty/395C loot flag write, or blocked change on crush; scan continues.
		}
	return blocked; // Sol:1B3B AX BP-6 stays0 after crushing, so execution caller accepts the mech move.
}


/* Sol: Original1631:1DCC..1DF7. Preserve signed gate and WORD product. */
void GameSpeed_RateControl(void)
{
    if ((int16_t)CombatSpeedSetting < CombatSpeedKeyWaitSetting)
        Wait_For_N_Vertical_Retraces((uint16_t)(CombatSpeedSetting * CombatSpeedRetracesPerSetting));
    else
        (void)Keyboard_Get_ASCII_Hex_Input();
}
