#include "game.h"

/* Sol: Complete original183B:000A..1481 controller, template from annotated
 * parent, expanded ASM rechecked. No replacement turn engine or DOS loop.
 * Native-valid traitor/member/name and weapon-target table indices remain
 * contracts. BUG-016 raw negative target memory needs separate representation;
 * do not mistake host pointer bytes for native neighbouring data. */
enum { DestroyedCitadelMap=11,EncounterLocustWreckSprite=0x80,
    EncounterHumanoidWreckSprite=0x81,KuritaEncounterSouthBoundary=0xD000,
    KuritaEncounterEastBoundary=0x0D00,KuritaEncounterNorthBoundary=0xB07F,
    KuritaEncounterWestBoundary=0x0B7F,EnemyPersonnelFlightDistance=25,
    ArenaExitWestBoundary=0x0900,ArenaExitEastBoundary=0x0A07,
    TraitorProbabilityAfterDiscovery=127 };
void Combat_Run_Encounter(uint16_t encounterType)
{
	// Sol: SS:BP-1C..BP-14, nine saved terrain BYTES. Reko's fp is a
	// Sol: frame-pointer placeholder, not a file pointer; fp-30 was off by two.
	uint8_t SavedCombatTerrainFlags[MapNeighbourhoodCount];
	/* Native assignments below precede every observable use in valid calls.
	 * Defined host initializers avoid compiler false-positive path warnings.
	 * KuritaAttackFlag must remain stable during a combat encounter; changing
	 * it from0 to1 mid-round would expose original unassigned stack state. */
	uint16_t KuritaMessageIndex=0; // WORD SS:BP-8, assigned by Kurita setup.
	uint16_t arenaEscapeLatched=0; // WORD BP-0C, assigned at round setup.
	EnemyTargetId = Enemy_All_CombatantId_Range_First;
	uint16_t savedMapX = CrescentHawkMapPositionX;
	uint16_t savedMapY = CrescentHawkMapPositionY;
	for (uint16_t GridId = 0; GridId < MapNeighbourhoodCount; ++GridId) // Sol: local 3x3 terrain cache, preserved for post-combat salvage.
		SavedCombatTerrainFlags[GridId] = LocalTerrainFlags[GridId];

	uint16_t EncounterAssessmentResult = FALSE; // Sol: WORD BP-0A encounter gate; first-active-enemy path reachability from28DB.
	if (encounterType == FALSE) // Sol: random encounter mode; nonzero callers supply prearranged combat state.
	{
		for (uint16_t NpcSlot = 0; NpcSlot < Enemy_Infantry_Record_Count; ++NpcSlot)
		{
			RoamingMapNpc *Npc=&RoamingMapNpcs[NpcSlot];
			Npc->currentPositionX = CombatantPackedX[NpcSlot + Enemy_Infantry_CombatantId_Range_First]; // Sol: original WORD4024+slot*2.
			Npc->currentPositionY = CombatantPackedY[NpcSlot + Enemy_Infantry_CombatantId_Range_First]; // Sol: original WORD4056+slot*2.
		}
	}
	// Sol: 00A1..017C's split aliases cover complete matrices. No calls
	// Sol: observe the intermediate clear order; express the same BYTE writes directly.
	for (uint16_t CombatantId = 0; CombatantId < AllCombatantCount; ++CombatantId)
	{
		for (uint16_t PlanByte = 0; PlanByte < CombatMovementPlanBytesPerUnit; ++PlanByte) // Sol: twelve signed X/Y step pairs.
			CombatMovementPlanBytes[CombatantId * CombatMovementPlanBytesPerUnit + PlanByte] = 2; // Sol: stop sentinel in either axis.
		for (uint16_t WeaponSlot = 0; WeaponSlot < CombatWeaponTargetSlots; ++WeaponSlot) // Sol: twelve target slots, kick is slot11.
			CombatWeaponTarget[CombatantId * CombatWeaponTargetSlots + WeaponSlot] = 0xFF; // Sol: no assigned target.
		for (uint16_t OrderByte = 0; OrderByte < CombatMovementOrderBytes; ++OrderByte) // Sol: twelve four-byte orders per combatant.
			CombatMovementOrders[CombatantId * CombatMovementOrderBytes + OrderByte] = 0xFF;
		if (encounterType == FALSE)
		{
			CombatantCasualtyFlags[CombatantId] = FALSE; // Sol: WORD death/wreck state.
			CombatantActive[CombatantId] = FALSE; // Sol: prearranged mode preserves supplied active/position state.
			TerrainOverlapRows[CombatantId] = 0;
			CombatantMovementDirection[CombatantId] = 0xFF;
			CombatantPackedY[CombatantId] = 0xFFFF; // Sol: CBW of FF produces WORD -1.
			CombatantPackedX[CombatantId] = 0xFFFF;
		}
	}
	// Sol: 0181 clears eight BYTES at3994 and3998, overlapping by four:
	// Sol: union is twelve action-state BYTES3994..399F, not sixteen WORDs.
	for (uint16_t FriendlyId = 0; FriendlyId < Enemy_All_CombatantId_Range_First; ++FriendlyId)
		CombatantActionState[FriendlyId] = 0;
	for (uint16_t MechRecordId = 0; MechRecordId < MechRecordCount; ++MechRecordId) // Sol: all eight friendly/enemy mech records.
	{
		MechInfernoRoundsRemaining[MechRecordId] = 0; // Sol: BYTE timed/inferno heat state.
		MechHeatLevel[MechRecordId] = 0; // Sol: BYTE current heat.
	}
	int16_t escapeResult = FALSE; // Sol: WORD BP-38, reused in later turn/menu logic.
	uint16_t InfantryFormationSlot = 0, MechFormationSlot = 0;
	for (uint16_t PartyMemberId = 0; PartyMemberId < Enemy_Infantry_Record_First; ++PartyMemberId) // Sol: friendly character records0..7.
	{
		if (Characters[PartyMemberId].name != Character_Dead
			&& (int8_t)Characters[PartyMemberId].mechAssignment >= Character_OnFoot)
		{
			Offset_Packed_Position(PartyInfantryFormationDeltaX[InfantryFormationSlot], PartyInfantryFormationDeltaY[InfantryFormationSlot]); // Sol: compact formation index advances only for living on-foot members.
			uint16_t CombatantId = PartyMemberId + Friendly_Infantry_Combatant_Range_First;
			CombatantPackedX[CombatantId] = CrescentHawkMapPositionX;
			CombatantPackedY[CombatantId] = CrescentHawkMapPositionY;
			++CombatantActive[CombatantId]; // Sol: ASM increments WORD4072+record*2; do not replace with boolean assignment.
			Combat_Move_Position(savedMapX, savedMapY);
			++InfantryFormationSlot;
		}
	}
	for (uint16_t MechRecordId = 0; MechRecordId < Friendly_Infantry_Combatant_Range_First; ++MechRecordId) // Sol: friendly mech records/combatants0..3.
	{
		if (*((uint8_t  *)&Mechs[MechRecordId]) != MECH_Destroyed) // Sol: unsigned first Name BYTE atC724, not doubled typed-array stride.
		{
			Offset_Packed_Position(FriendlyMechFormationDeltaX[MechFormationSlot], FriendlyMechFormationDeltaY[MechFormationSlot]);
			++MechFormationSlot;
			CombatantPackedX[MechRecordId] = CrescentHawkMapPositionX;
			CombatantPackedY[MechRecordId] = CrescentHawkMapPositionY;
			++CombatantActive[MechRecordId]; // Sol: original WORD increment, including prearranged combat.
			Combat_Move_Position(savedMapX, savedMapY);
		}
	}

	// Sol: 02DB..048A: prompt/result WORD BP-6 is only consumed when encounter gate is nonzero.
	uint16_t engagementAccepted=FALSE; /* Native consent gate prevents reading an unassigned value. */
	if (encounterType != FALSE)
		engagementAccepted = TRUE; // Sol: prearranged encounters bypass consent prompt.
	else
	{
		Generate_Random_Encounter_Enemies();
		Menu_Memory_Variables(4); // Sol: force-description/sidebar layout.
		Draw_Top_Graphic_Sidebar();
		TextColour = EGA_BrightWhite;
		EncounterAssessmentResult = FALSE;
		for (uint16_t EnemyId = Enemy_All_CombatantId_Range_First; EnemyId < AllCombatantCount; ++EnemyId) // Sol: all enemy combatants12..23.
		{
			if (CombatantActive[EnemyId] != FALSE
				&& CombatantPackedX[EnemyId] != 0xFFFF
				&& CombatantPackedY[EnemyId] != 0xFFFF) // Sol: WORD -1 sentinels, confirmed in clean Reko intermediate.
				EncounterAssessmentResult = TRUE;
		}
		if (EncounterAssessmentResult != FALSE)
			EncounterAssessmentResult = Combat_Assess_FirstEnemy_Reachability(); // Sol: first-active-enemy30-step probe, not movement-budget query.
		if (EncounterAssessmentResult != FALSE)
		{
			uint16_t NumberOfEnemyMechs = 0;
			Display_Text_From_Memory((uint8_t *)"Attacking force:\r"); // Memory ref: [3EDB:3402]
			for (uint16_t MechRecordId = Friendly_Infantry_Combatant_Range_First; MechRecordId < Enemy_Infantry_Record_First; ++MechRecordId) // Sol: enemy mech records4..7.
				if (*((uint8_t  *)&Mechs[MechRecordId]) != MECH_Destroyed)
					++NumberOfEnemyMechs; // Sol: first Name BYTE; this count does not separately test active/position.
			if (NumberOfEnemyMechs != 0)
			{
				Display_Text_Dynamic_Value(NumberOfEnemyMechs);
				Display_Text_From_Memory((uint8_t *)" 'Mech"); // Memory ref: [3EDB:3414]
				if (NumberOfEnemyMechs > 1) Display_Plural_Suffix();
				Display_Text_From_Memory((uint8_t *)" and\r"); // Memory ref: [3EDB:341B]
			}
			uint16_t NumberOfEnemyHumans = 0;
			for (uint16_t CharacterRecordId = Enemy_Infantry_Record_First; CharacterRecordId < Enemy_Infantry_CombatantId_Range_First; ++CharacterRecordId) // Sol: enemy character records8..15, not combatant IDs.
			{
				uint16_t EnemyId = CharacterRecordId + PartySize; // Sol: combatant IDs16..23.
				if (Characters[CharacterRecordId].name != Character_Dead
					&& CombatantActive[EnemyId] != FALSE
					&& CombatantPackedX[EnemyId] != 0xFFFF
					&& CombatantPackedY[EnemyId] != 0xFFFF)
					++NumberOfEnemyHumans;
			}
			Display_Text_Dynamic_Value(NumberOfEnemyHumans);
			Display_Text_From_Memory((uint8_t *)" human"); // Memory ref: [3EDB:3421]
			if (NumberOfEnemyHumans != 1) Display_Plural_Suffix(); // Sol: zero also pluralizes.
			Display_Sentence_Period();
			Menu_Memory_Variables(3); // Sol: consent/escape message layout.
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"Engage in combat?"); // Memory ref: [3EDB:3428]
			engagementAccepted = Prompt_Yes_No(TRUE);
		}
	}
	if (encounterType != FALSE)
		EncounterAssessmentResult = TRUE;
	if (EncounterAssessmentResult != FALSE
		&& (engagementAccepted != FALSE || (Rand_0x00_to_0xFF() & 3) == 0)) // Sol: declined engagement still forces combat when random low two bits are zero (1/4 under uniform bits).
	{
		if (engagementAccepted == FALSE)
		{
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"You did not evade them!"); // Memory ref: [3EDB:343A]
			Prompt_And_Wait_For_Key();
			engagementAccepted = TRUE; // Sol: 04A2..04BE stores1 after forced engagement.
		}
		if (KuritaAttackFlag != FALSE)
		{
			Draw_Message_Box();
			Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"These 'Mechs are unlike any you've fought against.  You feel uneasy."); // Sol: verified EXE text3EDB:3452; prior duplicate consent text was wrong.
			Keyboard_Get_ASCII_Hex_Input();
			KuritaMessageIndex = 0; // Sol:04CA initializes persistent five-message cursor before combat rounds.
		}
		if (encounterType == 2) // Sol: prearranged rules variant2 performs an immediate map refresh.
		{
			PosXY_OffsetGrid(CrescentHawkMapPositionX, CrescentHawkMapPositionY);
			Copy_Data_To_GraphicsMemory();
			Draw_Menu_MultiSelect();
			EGA_DrawBox_Wrapper();
		}
		CombatComputerControl = FALSE;
		Drain_Pending_Keyboard_Input();
		if (DisableComputerControl == FALSE)
			CombatComputerControl = Return_Bool_Allow_Computer_Control_Dialog();
		Drain_Pending_Keyboard_Input();
		CombatMessageVerbosity = SettingsMenu_CombatMessages();
		Drain_Pending_Keyboard_Input();
		CombatDisplayGraphics = SettingsMenu_SeeCombatGraphics();
		arenaEscapeLatched = FALSE;

		uint16_t RoundExitStatus = FALSE; // Sol: WORD BP-CombatWeaponTargetSlots is round-exit status, not a duplicate boolean.
		while (RoundExitStatus == FALSE)
		{
			uint16_t roundMapX = CrescentHawkMapPositionX; // Sol: round anchor WORD BP-20.
			uint16_t roundMapY = CrescentHawkMapPositionY; // Sol: round anchor WORD BP-CombatMovementPlanBytesPerUnit.
			FriendlyPersonnelWithdrawal = FALSE;
			if (CombatComputerControl != FALSE)
			{
				// Sol: 07D0 handles friendly AI instead of opening the manual menu.
				Combat_Plan_Computer_Side(0); // Sol: start at friendly combatant0.
				if (FriendlyPersonnelWithdrawal != FALSE)
				{
					escapeResult = TRUE;
					for (uint16_t MechRecordId = 0; MechRecordId < Friendly_Infantry_Combatant_Range_First; ++MechRecordId)
						if (*((uint8_t  *)&Mechs[MechRecordId]) != MECH_Destroyed)
							escapeResult = FALSE; // Sol: any surviving friendly mech suppresses this AI-only exit check.
					if (escapeResult != FALSE)
					{
						escapeResult = Rand_0x00_to_0xFF() & 3; // Sol: retain result0..3, not normalized boolean; zero continues.
						RoundExitStatus = escapeResult;
					}
				}
				goto ExecuteEnemyPlanningAndRound;
			}
			for (uint16_t FriendlyId = 0; FriendlyId < Enemy_All_CombatantId_Range_First; ++FriendlyId)
			{
				if (CombatantActive[FriendlyId] != FALSE)
				{
					if (CombatantPackedX[FriendlyId] == 0xFFFF || CombatantPackedY[FriendlyId] == 0xFFFF)
						CombatantActive[FriendlyId] = FALSE;
					for (uint16_t PlanByte = 0; PlanByte < CombatMovementPlanBytesPerUnit; ++PlanByte) // Sol: still clears plan for a unit just deactivated; twelve step pairs.
						CombatMovementPlanBytes[FriendlyId * CombatMovementPlanBytesPerUnit + PlanByte] = 2;
				}
			}
			escapeResult = Combat_UI_Menu_Logic(); // Sol: WORD result;0 executes orders, nonzero requests exit.
			if (KuritaAttackFlag != FALSE
				&& (CombatantPackedY[0] >= KuritaEncounterSouthBoundary
					|| (int16_t)CombatantPackedX[0] >= KuritaEncounterEastBoundary
					|| CombatantPackedY[0] < KuritaEncounterNorthBoundary
					|| (int16_t)CombatantPackedX[0] < KuritaEncounterWestBoundary)) // Sol: target is mech slot0; unsigned Y, signed X exactly match JNC/JC versus JGE.
				escapeResult = 2; // Sol: special Kurita boundary exit, bypasses ordinary escape roll.
			if (escapeResult != FALSE)
			{
				if (escapeResult != 2)
					escapeResult = (Rand_0x00_to_0xFF() & 3) != 0; // Sol: SBB/INC yields0 for low bits00,1 otherwise (3/4 under uniform bits).
				Menu_Memory_Variables(3); // Sol: escape-result text layout.
				Draw_Top_Graphic_Sidebar();
				if ((int16_t)encounterType >= 2)
					escapeResult = FALSE; // Sol: prearranged rules>=2 prohibit escape, including boundary result2.
				if (KuritaAttackFlag != FALSE && escapeResult != 2)
				{
					Display_Text_From_Memory((uint8_t *)"You cannot escape these Jenners!"); // Memory ref: [3EDB:3497]
					escapeResult = FALSE;
					Prompt_And_Wait_For_Key();
				}
				else if (escapeResult == FALSE)
				{
					Display_Text_From_Memory((uint8_t *)"You do not escape! The enemies engage in combat."); // Memory ref: [3EDB:34B8]
					Prompt_And_Wait_For_Key();
				}
				else if ((int16_t)encounterType > 0)
					FriendlyPersonnelWithdrawal = TRUE; // Sol: successful prearranged exit feeds later end-of-round state.
			}
			RoundExitStatus = escapeResult;
			if (escapeResult == FALSE)
			{
				for (uint16_t FriendlyId = 0; FriendlyId < Enemy_All_CombatantId_Range_First; ++FriendlyId) // Sol: friendly mech/personnel combatants0..11.
				{
					if (CombatantActive[FriendlyId] == FALSE) continue;
					if (CombatantActionState[FriendlyId] != 0)
					{
						Combat_Computer_Control(FriendlyId, FALSE); // Sol:1631:03AB handles per-unit AI action state.
						continue;
					}
					if (CombatMovementPlanBytes[FriendlyId * CombatMovementPlanBytesPerUnit] != 2
						|| CombatMovementOrders[FriendlyId * CombatMovementOrderBytes] == 0xFF)
						continue; // Sol: skip existing step plan or absent first order.
					Move_Map_View_To_Packed_Position(CombatantPackedX[FriendlyId], CombatantPackedY[FriendlyId]);
					PosXY_OffsetGrid(CrescentHawkMapPositionX, CrescentHawkMapPositionY);
					Update_Cached_Map_Origin();
					if (FriendlyId < Friendly_Infantry_Combatant_Range_First)
						Combat_Mech_Movement(FriendlyId, (int16_t)(int8_t)CombatMovementOrders[FriendlyId * CombatMovementOrderBytes]); // Sol: CBW sign-extends movement mode BYTE.
					else
						Combat_Infantry_Movement(FriendlyId);
					Combat_Calculate_Movement(FriendlyId);
				}
			}
ExecuteEnemyPlanningAndRound:
			if (escapeResult == FALSE)
			{
				// Sol:0826..089D enemy AI planning and twelve-slice mechanics handoff.
				Move_Map_View_To_Packed_Position(roundMapX, roundMapY);
				EnemyPersonnelFlightPossible = FALSE; // Sol: WORD374C cleared before enemy AI; later influences personnel flight.
				Combat_Plan_Computer_Side(Enemy_All_CombatantId_Range_First); // Sol: enemy combatants start at12.
				Move_Map_View_To_Packed_Position(roundMapX, roundMapY);
				PosXY_OffsetGrid(CrescentHawkMapPositionX, CrescentHawkMapPositionY);
				Combat_Mechanics((int16_t)encounterType > 0); // Sol: signed WORD rules comparison -> WORD boolean; no StatusTextPtr assignment exists.

				if (MainCharactersAlive != FALSE && TraitorInParty != FALSE && TraitorWarning != FALSE)
				{
					Menu_Memory_Variables(6); // Sol: betrayal narrative layout.
					Draw_Top_Graphic_Sidebar();
					Draw_Menu_Border(0);
					int16_t TraitorMemberId = (int8_t)TraitorCharacterId; // Sol: BYTE D331 sign-extends into BP-44.
					int16_t TraitorMechId = (int8_t)Characters[TraitorMemberId].mechAssignment; // Sol: BYTE C620 -> WORD; read value, not address.
					uint8_t TraitorNameId = Characters[TraitorMemberId].name;
					if (TraitorMechId == Character_OnFoot)
					{
						// Sol:08FF..09AD on-foot traitor is killed under Rex's interrogation.
						Display_Text_From_Memory((uint8_t *)"You watched "); //3EDB:34E9
						Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)TraitorNameId]);
						Display_Text_From_Memory((uint8_t *)" carefully, and saw that "); //3EDB:34F6
						Display_Text_From_Memory((uint8_t *)" the Dracos didn't shoot at him at all.  Under Rex's interrogation, "); //3EDB:3510
						Display_Text_From_Memory((uint8_t *)"which involved fists, it turns out that he was a Kurita "); //3EDB:3555
						Display_Text_From_Memory((uint8_t *)"double agent!  You put an end to that situation, to say the least."); //3EDB:358E
						uint16_t TraitorCombatantId = (uint16_t)(TraitorMemberId + Friendly_Infantry_Combatant_Range_First); // Sol: character record -> on-foot combatant.
						Register_Persistent_Map_Effect(Sprite_Impact_Small, CombatantPackedX[TraitorCombatantId], CombatantPackedY[TraitorCombatantId]);
						CombatantPackedY[TraitorCombatantId] = 0xFFFF;
						CombatantPackedX[TraitorCombatantId] = 0xFFFF;
					}
					else if (Mechs[TraitorMechId].pilotId != TraitorMemberId)
					{
						// Sol:0BD4..0CA1 traitor is a rider; existing pilot survives.
						Mechs[TraitorMechId].riderId = MECH_NoRider;
						Register_Persistent_Map_Effect(Sprite_Impact_Small, CombatantPackedX[TraitorMechId], CombatantPackedY[TraitorMechId]);
						Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)TraitorNameId]);
						Display_Text_From_Memory((uint8_t *)" knew you were suspicious of him, and he tried to sabotage the Mech "); //3EDB:37B8
						Display_Text_From_Memory((uint8_t *)"he was riding in.  He was a Kurita double agent!  "); //3EDB:37FD
						uint8_t PilotRecordId = Mechs[TraitorMechId].pilotId;
						Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)Characters[PilotRecordId].name]);
						Display_Text_From_Memory((uint8_t *)" was able to overcome him, and threw the doublecrosser out the cockpit.  "); //3EDB:3830
						Display_Text_From_Memory((uint8_t *)"Then he \"accidentally\" stepped on him, to rid you of the problem."); //3EDB:387A
					}
					else
					{
						// Sol:09C8 reads unsigned rider BYTE; sentinel is WORD00FF, NOTFFFF.
						uint16_t RiderRecordId = Mechs[TraitorMechId].riderId;
						Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)TraitorNameId]);
						Display_Text_From_Memory((uint8_t *)" tried to turn and fire on you!  He's a Kurita double agent!  "); //3EDB:35D1
						if (RiderRecordId != MECH_NoRider && Characters[RiderRecordId].skillPiloting != 0)
						{
							// Sol:0B40 skilled rider takes over, traitor dies; mech retained.
							Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)Characters[RiderRecordId].name]);
							Display_Text_From_Memory((uint8_t *)" was able to overcome him and throw him out the cockpit.  "); //3EDB:36F6
							Display_Text_From_Memory((uint8_t *)"Then he took command of the Mech, but in the process he must have "); //3EDB:3731
							Display_Text_From_Memory((uint8_t *)"jolted the controls, because the Mech stepped on the doublecrosser."); //3EDB:3774
							Mechs[TraitorMechId].pilotId = (uint8_t)RiderRecordId;
							Mechs[TraitorMechId].riderId = MECH_NoRider;
							Register_Persistent_Map_Effect(Sprite_Impact_Small, CombatantPackedX[TraitorMechId], CombatantPackedY[TraitorMechId]);
						}
						else
						{
							if (RiderRecordId == MECH_NoRider)
							{
								Display_Text_From_Memory((uint8_t *)"You were able to stop him, but it involved destroying the Mech he "); //3EDB:3610
								Display_Text_From_Memory((uint8_t *)"was piloting also."); //3EDB:3653
							}
							else
							{
								Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)Characters[RiderRecordId].name]);
								Display_Text_From_Memory((uint8_t *)" was able to stop him, but in the process he destroyed the Mech's "); //3EDB:3666
								Display_Text_From_Memory((uint8_t *)"cockpit.  He was able to eject in time, but the doublecrossing pilot didn't."); //3EDB:36A9
								Characters[RiderRecordId].mechAssignment = Character_OnFoot;
								uint16_t RiderCombatantId = RiderRecordId + Friendly_Infantry_Combatant_Range_First;
								CombatantActive[RiderCombatantId] = TRUE;
								CombatantPackedX[RiderCombatantId] = CombatantPackedX[TraitorMechId];
								CombatantPackedY[RiderCombatantId] = CombatantPackedY[TraitorMechId];
							}
							// Sol:0AD4 destroys first Name BYTE; wreck uses exact anchor, unlike normal -1/-1 placement.
							*((uint8_t  *)&Mechs[TraitorMechId]) = MECH_Destroyed;
							uint8_t WreckSprite = CombatantSpriteFamilyOffset[TraitorMechId] == MECH_Sprite_LOCUST ? EncounterLocustWreckSprite : EncounterHumanoidWreckSprite; // Sol: CMP family,1 / SBB / INC / ADD80.
							Register_Persistent_Map_Effect(WreckSprite, CombatantPackedX[TraitorMechId], CombatantPackedY[TraitorMechId]);
							CombatantPackedY[TraitorMechId] = 0xFFFF;
							CombatantPackedX[TraitorMechId] = 0xFFFF;
							CombatantActive[TraitorMechId] = FALSE;
						}
					}
					Wait_For_50Hz_Then_Check_Input();
					Keyboard_Get_ASCII_Hex_Input();
					Characters[TraitorMemberId].name = Character_Dead;
					CombatantActive[TraitorMemberId + Friendly_Infantry_Combatant_Range_First] = FALSE;
					TraitorInParty = FALSE;
					TraitorBattleProbability = TraitorProbabilityAfterDiscovery; // BYTE D330 restored after discovery.
				}
				// Sol:0CDB..0D0E latches special arena-escape condition; signed X only, no Y check.
				if (ArenaEscapeAllowed != FALSE && CombatantActive[0] != FALSE
					&& (int16_t)CombatantPackedX[0] > ArenaExitWestBoundary && (int16_t)CombatantPackedX[0] < ArenaExitEastBoundary)
					arenaEscapeLatched = TRUE; // Sol: open X interval0900..0A07 for active mech slot0.

				RoundExitStatus = TRUE;
				for (uint16_t EnemyId = Enemy_All_CombatantId_Range_First; EnemyId < AllCombatantCount; ++EnemyId)
				{
					if (CombatantPackedX[EnemyId] == 0xFFFF || CombatantPackedY[EnemyId] == 0xFFFF)
						CombatantActive[EnemyId] = FALSE;
					if (CombatantActive[EnemyId] != FALSE) RoundExitStatus = FALSE;
				}
				if (RoundExitStatus == FALSE && EnemyPersonnelFlightPossible != FALSE && encounterType == FALSE)
				{
					uint16_t AnyEnemyMechActive = FALSE;
					for (uint16_t EnemyId = Enemy_All_CombatantId_Range_First; EnemyId < Enemy_Infantry_CombatantId_Range_First; ++EnemyId)
						if (CombatantActive[EnemyId] != FALSE) AnyEnemyMechActive = TRUE;
					if (AnyEnemyMechActive != FALSE)
					{
						uint16_t SavedMapX = CrescentHawkMapPositionX, SavedMapY = CrescentHawkMapPositionY;
						EnemyPersonnelFlightPossible = FALSE; // Sol: suppresses subsequent global-flight coin flip in this branch.
						uint16_t AnyPersonnelFled = FALSE;
						for (uint16_t EnemyId = Enemy_Infantry_CombatantId_Range_First; EnemyId < AllCombatantCount; ++EnemyId)
						{
							if (CombatantActive[EnemyId] == FALSE) continue;
							CrescentHawkMapPositionX = CombatantPackedX[EnemyId];
							CrescentHawkMapPositionY = CombatantPackedY[EnemyId];
							int16_t TargetId = (int8_t)CombatWeaponTarget[EnemyId * CombatWeaponTargetSlots]; // Sol:0DF3 CBW, no low7-bit mask or sentinel guard in original.
							if ((int16_t)Combat_Packed_Distance_From_Map_Position(CombatantPackedX[TargetId], CombatantPackedY[TargetId]) > EnemyPersonnelFlightDistance) // Signed range metric, not Euclidean pixels.
							{
								CombatantActive[EnemyId] = FALSE;
								CombatantPackedY[EnemyId] = 0xFFFF;
								CombatantPackedX[EnemyId] = 0xFFFF;
								AnyPersonnelFled = TRUE;
							}
						}
						if (AnyPersonnelFled != FALSE)
						{
							Draw_Message_Box();
							Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Some of the enemy humans have fled!"); //3EDB:38BC
							Keyboard_Get_ASCII_Hex_Input();
						}
						CrescentHawkMapPositionX = SavedMapX;
						CrescentHawkMapPositionY = SavedMapY;
					}
					if (EnemyPersonnelFlightPossible != FALSE)
						EnemyPersonnelFlightPossible = Rand_0x00_to_0xFF() & 1; // Sol: no enemy mech active -> 50/50 global flight, assuming uniform bit0.
					if (EnemyPersonnelFlightPossible != FALSE)
					{
						Draw_Message_Box();
						Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"The enemy humans have fled!"); //3EDB:38E0
						Keyboard_Get_ASCII_Hex_Input();
						RoundExitStatus = TRUE;
					}
				}
				if (encounterType == 1 && CombatantActive[0] == FALSE)
					RoundExitStatus = TRUE; // Sol: rules1 ends when combat mech0 is inactive.
				if (arenaEscapeLatched == FALSE && encounterType == 2
					&& CombatantActive[0] == FALSE && ArenaEscapeAllowed == FALSE)
					RoundExitStatus = TRUE; // Sol: rules2 has additional escape-story flag gating.
				if (arenaEscapeLatched != FALSE || MainCharactersAlive == FALSE) RoundExitStatus = TRUE;
				if (KuritaAttackFlag != FALSE)
				{
					uint16_t MessagesRemaining = RoundExitStatus != FALSE ? (uint16_t)(5 - KuritaMessageIndex) : 1; // Sol: ongoing rounds show one message; final round drains remaining five-entry sequence.
					while (MessagesRemaining != 0) // Sol:0FF0 tests OLD count before decrement, not post-decrement equality.
					{
						--MessagesRemaining;
						if (KuritaMessageIndex >= 5) continue;
						if (KuritaMessageIndex == 1)
							Display_Animation_Scene(AnimationO02_NeuroHelmet, BuildingEntryAnimationCallerRedraw); // Sol: helmet cutscene before message1; does NOT skip/increment message.
						Draw_Message_Box();
						uint16_t MessageToShow = KuritaMessageIndex++;
						Display_Text_From_Memory_ScreenRetrace_KeyboardInput(KuritaMissionTextMessages[MessageToShow]); // Sol: FAR pointer stride4 handled by typed table.
						Keyboard_Get_ASCII_Hex_Input();
						if (KuritaMessageIndex == 3) // Sol: after message2 (Citadel destroyed), load ruined Citadel.
						{
							DOS_Load_Map_Files(4, DestroyedCitadelMap);
							Map_NineGrid_Parent();
							CurrentMap = DestroyedCitadelMap;
						}
						Menu_Memory_Variables(4);
						Draw_Top_Graphic_Sidebar();
					}
				}
			}
			if (CombatComputerControl != FALSE  //w009E = Combat_Computer_Control
			&& DisableInput == FALSE //w3938 - Disable Input
			&& Pending_Input() != FALSE)
			{
				CombatComputerControl = FALSE; //w009E = Combat_Computer_Control
				Drain_Pending_Keyboard_Input();
			}
		}
	}

	// Sol:1037..1481 post-combat outcomes. BP-0A is an encounter gate,
	// Sol: not a damage sprite or movement budget; helper28DB supplies its result.
	if (EncounterAssessmentResult == FALSE)
	{
		Combat_Character_Pos_Grid(savedMapX, savedMapY); // Sol:140D original outer WORD map anchor.
		goto ExitCombat;
	}

	Menu_Memory_Variables(0x03);
	Draw_Top_Graphic_Sidebar();

	if ((int16_t)encounterType < 1 || encounterType == 3) // Sol: random encounter / salvage-enabled special mode.
	{
		// Sol:1060 requires accepted engagement, normal victory (exit status0),
		// Sol: and surviving main characters. Reko lost the BP-6 consent gate.
		if (engagementAccepted == FALSE || escapeResult != 0
		 || MainCharactersAlive == FALSE)
		{
			if (MainCharactersAlive != FALSE)
			{
				Draw_Top_Graphic_Sidebar();
				Display_Text_From_Memory((uint8_t *)"You have eluded your enemies!"); // DS:3936
				Prompt_And_Wait_For_Key();
			}
		}
		else
		{
			Menu_Memory_Variables(0x01);
			Draw_Menu_Border(0x01);
			Draw_Top_Graphic_Sidebar();

			uint16_t AnyEnemyCasualty = FALSE; // Sol: reused WORD BP-3A, not mech-only.
			for (uint16_t EnemyId = Enemy_All_CombatantId_Range_First; EnemyId < AllCombatantCount; ++EnemyId)
				if (CombatantCasualtyFlags[EnemyId] != FALSE) // DS:393C WORD casualty/wreck table.
					AnyEnemyCasualty = TRUE;
			if (AnyEnemyCasualty != FALSE)
			{
				Display_Text_From_Memory((uint8_t *)"After an invigorating scuffle, you defeated the Kuritans."); // DS:38FC
				Prompt_And_Wait_For_Key();
			}

			uint16_t HasFriendlyMech = FALSE;
			for (uint16_t MechId = 0; MechId < Friendly_Infantry_Combatant_Range_First; ++MechId)
				if (*((uint8_t *)&Mechs[MechId]) != MECH_Destroyed) // DS:C724 + MechId*0x7D; first name BYTE.
					HasFriendlyMech = TRUE;

			uint16_t HasEnemyMechWreck = FALSE;
			for (uint16_t EnemyId = Enemy_All_CombatantId_Range_First; EnemyId < Enemy_Infantry_CombatantId_Range_First; ++EnemyId)
				if (CombatantCasualtyFlags[EnemyId] != FALSE)
					HasEnemyMechWreck = TRUE;
			if (HasFriendlyMech != FALSE && HasEnemyMechWreck != FALSE)
			{
				// Sol: enemy Mech IDs12..15 are a subset of the earlier casualty
				// scan12..23, so the victory acknowledgement necessarily ran.
				for (uint16_t PartyId = 0; PartyId < Enemy_Infantry_Record_First; ++PartyId) // Sol: friendly records0..7.
				{
					// Sol:115B CBW;115C CMP AX,00FFh. Preserve BUG-017:
					// Sol: a dead FF name becomes FFFF and DOES NOT fail this test.
					if ((int16_t)(int8_t)Characters[PartyId].name != Character_Dead
					 && Characters[PartyId].skillTech != 0) // DS:C61D
					{
						Salvage_Armour_Dialog(); // Sol:0DAB:0002 once, first qualifying tech.
						break;
					}
				}
			}

			uint16_t HasFriendlyMechSalvageFlag = FALSE;
			for (uint16_t MechId = 0; MechId < Friendly_Infantry_Combatant_Range_First; ++MechId)
				if (CombatantCasualtyFlags[MechId] != FALSE
				 || DeadInfantryFlags[MechId] != FALSE)
					HasFriendlyMechSalvageFlag = TRUE; // Sol:3954=393C+12 WORDs: this view tests enemy Mech casualties12..15; legacy name is misleading.

			uint16_t HasAvailablePilot = FALSE;
			for (uint16_t PartyId = 0; PartyId < Enemy_Infantry_Record_First; ++PartyId)
				if (Characters[PartyId].name != Character_Dead
				 && Characters[PartyId].skillPiloting != 0 // DS:C61C
				 && (int8_t)Characters[PartyId].mechAssignment >= Character_OnFoot) // DS:C620 signed BYTE;8+ unassigned/on foot.
					HasAvailablePilot = TRUE;

			uint16_t HasFreeMechSlot = FALSE;
			for (uint16_t MechId = 0; MechId < Friendly_Infantry_Combatant_Range_First; ++MechId)
				if (*((uint8_t *)&Mechs[MechId]) == MECH_Destroyed)
					HasFreeMechSlot = TRUE; // Sol: FF slot can accept a salvaged mech.
			uint16_t TerrainAllowsMechSalvage = TRUE;
			for (uint16_t GridId = 0; GridId < MapNeighbourhoodCount; ++GridId)
				if ((SavedCombatTerrainFlags[GridId] & 0x80) != 0) // Sol:1232 saved 3x3 terrain BYTE high bit prohibits mech salvage.
					TerrainAllowsMechSalvage = FALSE;
			if (HasFriendlyMechSalvageFlag != FALSE && HasAvailablePilot != FALSE
			 && HasFreeMechSlot != FALSE && TerrainAllowsMechSalvage != FALSE)
				Salvage_Mechs_Dialog(); // Sol:0DAB:04F9; no independent enemy-wreck test on this branch.

			Loot_Enemy_Soldiers_Dialog(); // Sol:1251 called on every victory path, regardless of either salvage dialog.
			PartyHealthRecoveryTimer = 0; // DS:D335 BYTE reset.
			Menu_Memory_Variables(0x03);
			Draw_Top_Graphic_Sidebar();

			uint16_t TotalMissingHealth = 0; // Sol: WORD BP-3A accumulator, NOT boolean; wraps at16 bits.
			for (uint16_t PartyId = 0; PartyId < Enemy_Infantry_Record_First; ++PartyId)
				if (Characters[PartyId].name != Character_Dead)
					TotalMissingHealth = (uint16_t)(TotalMissingHealth
					 + CharacterHealthPerBodyPoint * (int8_t)Characters[PartyId].body
					 - (int8_t)Characters[PartyId].health); // Sol:128F..12A0 signed BYTE inputs; max health =10*Body.
			if (TotalMissingHealth != 0)
				Heal_Characters(MedicalService_UsePartyMedicAndEquipment); // Sol:1431:000A WORD0; this does not pass the injury sum.
		}

		if (MainCharactersAlive != FALSE)
		{
			Menu_Memory_Variables(0x04);
			Draw_Top_Graphic_Sidebar();
			Draw_Health_and_C_Bills_Sidebar(TRUE);
			Draw_Message_Box();
			Display_Text_From_Memory((uint8_t *)"You return to where you were before combat took place."); // DS:3954
			Drain_Pending_Keyboard_Input();
			Keyboard_Get_ASCII_Hex_Input();

			// Sol:1330..136A restores the OUTER pre-combat map anchor,
			// Sol: not the shadowed round-local combat viewport coordinates.
			Move_Map_View_To_Packed_Position(savedMapX, savedMapY);
			Combat_Load_9Grid_Map();
			PosXY_OffsetGrid(CrescentHawkMapPositionX, CrescentHawkMapPositionY);
			Copy_Data_To_GraphicsMemory();
		}
		goto ExitCombat;
	}

	if (MainCharactersAlive == FALSE)
		goto ExitCombat;
	if (KuritaAttackFlag != FALSE) // Sol:136D..140C scripted outcome takes precedence over normal mode messages.
	{
		if (escapeResult == 2) // Sol: Kurita boundary exit, not ordinary escape1 or AI exit3.
		{
			Menu_Memory_Variables(0x03);
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"You have eluded your enemies!"); // DS:39F7
			Prompt_And_Wait_For_Key();
		}
		goto ExitCombat;
	}
	if (encounterType == 1) // Sol: training/base-return mode.
	{
		Draw_Message_Box();
		Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You should return to base now."); // DS:398B
	}
	else
	{
		if (arenaEscapeLatched != FALSE) // Sol: escape latch suppresses arena outcome.
			goto ExitCombat;
		Draw_Message_Box();
		ArenaWon = TRUE; // DS:D32D BYTE; outcome tests friendly mech slot0, not character health.
		if (*((uint8_t *)&Mechs[Character_Jason]) == MECH_Destroyed)
		{
			Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"The crowd boos resoundingly.  You have lost."); // DS:39CA
			ArenaWon = FALSE;
		}
		else
			Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Congratulations!  You have won!"); // DS:39AA; verified expanded EXE text.
	}
	Keyboard_Get_ASCII_Hex_Input();

ExitCombat:
	// Sol:141A..147B common to ALL exits, including no encounter and death.
	for (uint16_t NpcSlot = 0; NpcSlot < Enemy_Infantry_Record_Count; ++NpcSlot)
	{
		RoamingMapNpc *Npc=&RoamingMapNpcs[NpcSlot];
		uint16_t CombatantId = Enemy_Infantry_CombatantId_Range_First + NpcSlot;
		CombatantPackedX[CombatantId] = Npc->currentPositionX; // DS:4024 + slot*2 WORD.
		CombatantPackedY[CombatantId] = Npc->currentPositionY; // DS:4056 + slot*2 WORD.
		CombatantAnimationSelector[CombatantId] = 0xFF; // DS:397C; no animation.
		CombatantAnimationSelector[NpcSlot] = 0xFF; // Sol: DS:396C only slots0..7, NOT all friendly0..11.
		CombatantSpriteFrame[CombatantId] = 0x10; // DS:40AA; world NPC standing frame.
	}
}
