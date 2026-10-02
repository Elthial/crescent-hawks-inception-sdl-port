#include "game.h"
#include "dos.h"

// 0DAB:04F9: void Salvage_Mechs_Dialog(void)
// Called from:
//      Combat_Run_Encounter
// Sol: Summary: Try to recover a whole defeated mech.
// Sol: Outline: Select a wreck and technician, check recovery conditions and party capacity, then copy the mech record.
void Salvage_Mechs_Dialog(void)
{
	// Sol: Bind native3092 storage to globals; retain signed BYTE loads.
	// Sol: Scratch strings below are native BYTE-address views at3092:0012,
	// Sol: bound to DynamicString; placeholders are aliases into this same array.
	// Sol: 0DAB:04F9-054B. These are 16-bit native locals even though both
	// Sol: selected values originate in byte-sized character fields.
	int16_t highestTechSkill = SkillLevel_Unskilled;
	uint16_t partyTechId = Character_Jason;

	// Sol: Repeat the same strict best-technician search used by the preceding
	// Sol: component-salvage routine. Equal Tech scores retain the earlier slot.
	for (uint16_t partyMemberId = 0; partyMemberId < PartySize; partyMemberId++)
	{
		if (Characters[partyMemberId].name != Character_Dead)
		{
			int16_t techSkill = (int8_t)Characters[partyMemberId].skillTech;
			if (highestTechSkill < techSkill)
			{
				partyTechId = partyMemberId;
				highestTechSkill = techSkill;
			}
		}
	}

	// Sol: The original initializes the candidate party slot to zero and jumps
	// Sol: directly to the eligibility test at 0850. Wreck selection around 054C is
	// Sol: reached only after an eligible pilot accepts the salvage prompt.

	// Sol: 0DAB:084D-08D4. Consider each of the eight party records. A whole
	// Sol: mech can be offered only to a living, trained pilot whose assignment
	// Sol: byte is 8 (on foot) or greater.
	for (uint16_t partyMemberId = 0x00; partyMemberId < PartySize; partyMemberId++)
	{
		if (Characters[partyMemberId].name != Character_Dead
		 && Characters[partyMemberId].skillPiloting != SkillLevel_Unskilled
		 && (int8_t)Characters[partyMemberId].mechAssignment >= Character_OnFoot)
		{
			Draw_Top_Graphic_Sidebar();
			// Sol: Keep the character-name ID distinct from the party-record slot;
			// Sol: the latter is assigned as the recovered mech's PilotId at 0727.
			int16_t pilotCharacterId = (int8_t)Characters[partyMemberId].name;

			Display_Text_From_Memory(CharacterNames[pilotCharacterId]);
			Display_Text_From_Memory((uint8_t *)" is a qualified pilot without a 'Mech. Do you want to salvage one of the wrecks?"); // Memory ref: [3EDB:0FAE]
			uint16_t remainingWreckCount = 0x01;

			if (Prompt_Yes_No(TRUE))
			{
				// Sol: 0DAB:08D5-0944. Acceptance announces the selected
				// Sol: technician, holds the inspection message for 0x78 retraces,
				// Sol: resets candidate state, and enters the random wreck scan.
				Draw_Top_Graphic_Sidebar();
				int16_t selectedTechnicianCharacterId = (int8_t)Characters[partyTechId].name;

				Display_Text_From_Memory(CharacterNames[selectedTechnicianCharacterId]);
				Display_Text_From_Memory((uint8_t *)" is inspecting the wrecks...\r"); // Sol: exact salvage EXE text, 3EDB:0FFF
				Wait_For_N_Vertical_Retraces(SalvageInspectionRetraceCount);

				uint16_t salvageLoopState = SalvageLoop_Searching;
				do
				{
					// Sol: 0DAB:054C-05E9. Choose a starting point in one of the two
					// Sol: four-'Mech combatant ranges. Entry 0926 evaluates the
					// Sol: random within-lance index first, then the range selector.
					// Sol: The seed itself is tested at 055A; advancement occurs only
					// Sol: while the current word is zero ("not a wreck").
					uint16_t candidateWithinLance = Rand_0x00_to_0xFF() & (LanceSize - 1);
					uint16_t candidateCombatantId =
						(Rand_0x00_to_0xFF() & 0x01) * Enemy_All_CombatantId_Range_First + candidateWithinLance;
					// Sol: &1 selects one of two sides; lance IDs are 0..3 or12..15.

					while (CombatantCasualtyFlags[candidateCombatantId] == FALSE)
					{
						candidateCombatantId++;
						if (candidateCombatantId == Enemy_Infantry_CombatantId_Range_First)
							candidateCombatantId = 0x00;
					}

					// Sol: A candidate is consumed even when the following damage test
					// Sol: rejects it. The word count controls the later "no more" path.
					CombatantCasualtyFlags[candidateCombatantId] = FALSE;
					remainingWreckCount = 0x00;

					for (uint16_t combatantId = 0x00;
						 combatantId < Enemy_Infantry_CombatantId_Range_First;
						 combatantId++)
					{
						if (CombatantCasualtyFlags[combatantId] != FALSE)
							remainingWreckCount++;
					}

					// Sol: Combatant IDs 0C..0F map to compact Mechs[] records 4..7;
					// Sol: IDs 00..03 already equal records 0..3. This routine relies on
					// Sol: the invariant that IDs 04..0B never carry a 'Mech-wreck flag.
					uint16_t selectedMechId = candidateCombatantId;
					if (selectedMechId >= Enemy_All_CombatantId_Range_First)
						selectedMechId -= Enemy_All_CombatantId_Range_First - Enemy_Mech_Record_First;

					uint16_t canSalvageWreck = TRUE;
					Mech *selectedMech = &Mechs[selectedMechId];

					// Sol: Complete engine loss, complete gyro loss, or a zero byte at
					// Sol: CurrentStructure[3] is catastrophic damage. An Excellent Tech
					// Sol: can recover such a wreck; lower-skilled technicians cannot.
					// Sol: The anatomical label for structure index 3 remains under A-005.
					if ((selectedMech->engineHits == MechEngineDestroyedHitCount
					  || selectedMech->gyroHits == MechGyroDestroyedHitCount
					  || selectedMech->currentStructure[3] == 0x00)
					 && highestTechSkill < SkillLevel_Excellent)
						canSalvageWreck = FALSE;

					if (canSalvageWreck == FALSE)
					{
						// Sol: 0DAB:07B5-080D. The literal begins with carriage return
						// Sol: and ends at X; there is no period before the remaining name
						// Sol: bytes. Address 002D is that X placeholder in the buffer.
						Append_Large_Text_To_Memory(DynamicString, (uint8_t *)"\rNo chance to salvage this X"); // 3EDB:102F

						DynamicString[SalvageFailureNamePlaceholder] = DestroyedMechNameInitial[selectedMechId];

						// Sol: Name[0] is still the destroyed marker, so rebuild the
						// Sol: display name from its saved initial plus Name[1..15].
						Append_Text_To_Memory(DynamicString, &selectedMech->name[1]); // 3092:C725 + record * 0x7D
						Display_Text_From_Memory(DynamicString);
					}
					else
					{
						// Sol: 0DAB:05EA-06BC. Print the selected technician's name,
						// Sol: then build " is salvaging a <mech>" in the shared buffer.
						// Sol: A destroyed mech's Name[0] is 0xFF, so combat saved that
						// Sol: first character at 3092:323E before marking it destroyed.
						int16_t technicianCharacterId = (int8_t)Characters[partyTechId].name;
						uint8_t recoveredMechNameInitial =
							DestroyedMechNameInitial[selectedMechId];

						Display_Text_From_Memory(CharacterNames[technicianCharacterId]);

						Append_Large_Text_To_Memory(DynamicString, (uint8_t *)" is salvaging a X"); // 3EDB:101D

						// Sol: Buffer byte 0022 is the X placeholder at index 0x10.
						DynamicString[SalvageSuccessNamePlaceholder] = recoveredMechNameInitial;
						Append_Text_To_Memory(DynamicString, &selectedMech->name[1]);
						int16_t lastMessageCharacterIndex =
							Loop_Until_TextPtr_Null(DynamicString) - 0x01;

						// Sol: Mech names are fixed-width and space padded. The original
						// Sol: uses a signed 16-bit local while removing those trailing spaces.
						while (lastMessageCharacterIndex > 0x00
							&& ((uint8_t *)DynamicString)[lastMessageCharacterIndex] == ' ')
						{
							((uint8_t *)DynamicString)[lastMessageCharacterIndex] = 0;
							lastMessageCharacterIndex--;
						}

						Display_Text_From_Memory(DynamicString);
						Display_Sentence_Period();
						salvageLoopState = SalvageLoop_Recovered; // A wreck was successfully recovered.

						// Sol: 0DAB:06BD-070C. Search only the four player-'Mech records
						// Sol: for a slot whose name marker is 0xFF. If no slot is empty,
						// Sol: the original falls through to the post-candidate state checks.
						uint16_t playerMechSlot = 0x00;
						while (playerMechSlot < LanceSize)
						{
							if (Mechs[playerMechSlot].name[0] == MECH_Destroyed)
							{
								// Sol: Copy offsets 01..7C, deliberately excluding Name[0].
								// Sol: The source byte is the 0xFF destroyed marker and the saved
								// Sol: original initial is restored separately at 070D.
								for (uint16_t mechRecordOffset = 0x01;
									 mechRecordOffset < MechRecordSize;
									 mechRecordOffset++)
								{
									((uint8_t *)&Mechs[playerMechSlot])[mechRecordOffset] =
										((uint8_t *)selectedMech)[mechRecordOffset];
								}

								// Sol: 0DAB:070D-07B4. Restore the first name character omitted
								// Sol: from the record copy, bind the eligible on-foot pilot to this
								// Sol: lance slot, and select the only two mech sprite families.
								Mech *recoveredMech = &Mechs[playerMechSlot];
								recoveredMech->name[0] = DestroyedMechNameInitial[selectedMechId];
								recoveredMech->pilotId = (uint8_t)partyMemberId;
								Characters[partyMemberId].mechAssignment = (uint8_t)playerMechSlot;

								CombatantSpriteFamilyOffset[playerMechSlot] = MECH_Sprite_LOCUST;
								if (recoveredMech->name[0] != 'L')
									CombatantSpriteFamilyOffset[playerMechSlot] = MECH_Sprite_COMMANDO;

								// Sol: A recovered mech is never pristine: both critical systems
								// Sol: are forced to one hit. Structure bytes +1F and +20 are also
								// Sol: raised to one when zero, making the wreck minimally operable.
								recoveredMech->gyroHits = 0x01;
								recoveredMech->engineHits = 0x01;

								if (recoveredMech->currentStructure[4] == 0x00) // Record offset +0x20
									recoveredMech->currentStructure[4] = 0x01;

								if (recoveredMech->currentStructure[3] == 0x00) // Record offset +0x1F
									recoveredMech->currentStructure[3] = 0x01;

								// Sol: Setting the loop index to four, followed by the common
								// Sol: increment, forces the empty-slot search to terminate.
								playerMechSlot = LanceSize;
							}
							playerMechSlot++;
						}
					}

					// Sol: 0DAB:080E-0825. State zero after a rejected wreck means
					// Sol: immediately choose another candidate. Exhaustion changes both
					// Sol: state and party index to 9 so neither loop can continue.
					if (remainingWreckCount == 0x00)
					{
						partyMemberId = SalvageLoop_Exhausted;
						salvageLoopState = SalvageLoop_Exhausted; // Candidate table exhausted.
					}

				} while (salvageLoopState == SalvageLoop_Searching);
				// Sol: 0DAB:0826 pauses only after success (state 1) or exhaustion
				// Sol: (state 9); rejected candidates with wrecks remaining bypass it.
				Prompt_And_Wait_For_Key();
			}

			// Sol: 0DAB:082B-084C. Exhaustion produces a second pause around the
			// Sol: explicit battlefield-empty message. Party index 9 is a sentinel;
			// Sol: the common increment at 084D makes it 10 and ends the <8 scan.
			if (remainingWreckCount == 0x00)
			{
				Draw_Top_Graphic_Sidebar();
				Display_Text_From_Memory((uint8_t *)"There are no more salvageable 'Mechs on the battlefield."); // 3EDB:104C
				Prompt_And_Wait_For_Key();
				partyMemberId = SalvageLoop_Exhausted;
			}
		}
	}
}
