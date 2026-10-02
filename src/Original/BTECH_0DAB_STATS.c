#include "game.h"
#include "dos.h"

/* Original silhouette/text layout uses eight-pixel text columns and rows.
 * These values are artwork placement, not combat or equipment rules. */
enum { BTStatsPanel=4, BTStatsValueColumn=6, BTStatsWeaponFirstRow=6,
    BTStatsWeaponLocationColumn=11, BTStatsConditionPipColumn=37,
    BTStatsHeatSinkPipColumn=34, BTStatsHeatGaugeX=256 };

/* Sol: original0DAB:1AFE..2304 and .dis epilogue parent; EGA-only maintained pipeline.
 * Incidental BIOS palette-start residue is normalized for the portable build.
 * All storage bindings and signed name/record gates follow the ASM. */
void Examine_Screen_BTSTATS_CMP(uint16_t CombatantId)
{
    EgaMemoryAddress Staging={0,EgaSceneStagingSegment},Screen={0,EgaScreenSegment};
	// Sol: Native Mech-byte reads bind directly to the original global records.
	// Sol: Pilot/rider names are CBW-signed; record gates compare signed WORDs.
	// Sol: Native palette locals are uninitialized stack WORDs, not game rules.
	// Sol: Native Scan->Friends->Detail write watchpoints confirmed BP-22 reuses
	// Sol: the keyboard helper's saved menu SI (0030 for menu3); BP-2C reuses
	// Sol: BIOS interrupt flags (0247 in that Spice86 capture). These are caller/
	// Sol: BIOS stack residue, NOT Mech data or universal initializers. See
	// Sol: docs/phase4/NATIVE_MENU_INPUT_COMPARISON.md for final-writer evidence.
	// Sol: These two WORD locals are not initialized by the original. The first
	// Sol: redraw increments and compares the full frame WORD before masking
	// Sol: it to four bits; the first palette update
	// Sol: likewise masks the phase to two bits, so only the starting phase/delay
	// Sol: is indeterminate rather than any table index.
	// Sol: Portable presentation policy: start a fresh16-redraw palette interval
	// Sol: and phase0. These are intentional initializers, not recovered native
	// Sol: assignments. User approved not reproducing incidental BIOS behaviour.
	// Sol: Retain all subsequent compare/mask/palette order and game-state writes.
	uint16_t PaletteRefreshFrame=0;
	uint16_t PaletteCyclePhase=0;
	// Sol: Native WORD local [BP-1A], used later for destroyed weapons/actuators.
	uint16_t DestroyedStatusColour = EGA_DarkGrey;

	Menu_Memory_Variables(BTStatsPanel);
	Draw_Top_Graphic_Sidebar();

	if (BTStatsAssetLoaded == FALSE)
	{
		BTStatsAssetLoaded++;
		Select_Game_Disk_And_Drive(1);
		Load_File_To_Memory((uint8_t *)"BTSTATS.CMP", BTStatsOrBldMemory); // DS:1272
	}

	// Sol: The shipped program clears this compatibility word and calls
	// Sol: 207F:00D1(3056:0000) to rebuild an adapter-0 colour map. That path is
	// Sol: intentionally omitted from this maintained EGA-only transcription.
	GraphicsCompatibilityFlag = FALSE;
	Decompress_File_Into_Memory(BTStatsOrBldMemory, GraphicsFileWorkspace);
	DrawCall_Image_To_VGA_Memory(GraphicsFileWorkspace, EgaSceneStagingSegment);
	EGA_DrawBox_Operation(Staging, Screen, 0, 0, EgaFramebufferRowBytes, EgaScreenHeight); // Sol: ASM pushes explicit FAR offsets0 with sourceA800/destinationA000.

	// Sol: DS:127E is one CR-delimited block containing the Type, Tons, Pilot,
	// Sol: Rider, and Armament/Loc labels. One text call lays out the whole panel.
	Draw_EGA_Text_To_Screen((uint8_t *)"Type :\rTons :\rPilot:\rRider:\r\rArmament   Loc",
		0x00, 0x00, EGA_BrightWhite, EGA_Black);

	// Sol: Friendly combatants 0..3 already match Mechs[0..3]. Enemy combatants
	// Sol: 12..15 map to live enemy records Mechs[4..7] by subtracting eight.
	uint16_t MechRecordId = CombatantId;
	if ((int16_t)MechRecordId >= Enemy_All_CombatantId_Range_First)
		MechRecordId = (uint16_t)(MechRecordId -
            (Enemy_All_CombatantId_Range_First - Enemy_Mech_Record_First));

	Append_Large_Text_To_Memory(DynamicString,
		&Mechs[MechRecordId].name[0]);
	// Sol: The copy helper brings across the full NUL-terminated record name;
	// Sol: this explicit byte caps the on-screen Type field at eight characters.
	DynamicString[8] = '\0';

	Draw_EGA_Text_To_Screen(DynamicString, BTStatsValueColumn, 0x00, EGA_BrightYellow, EGA_Black);
	ASM_Text_Formatting(Mechs[MechRecordId].tonnage,
		DynamicString, 0x0A);
	Draw_EGA_Text_To_Screen(DynamicString, BTStatsValueColumn, 0x01, EGA_BrightYellow, EGA_Black);

	if ((int16_t)MechRecordId >= LanceSize)
	{
		// Sol: DS:12AF is "Unknown\rUnknown", filling both Pilot and Rider.
		Draw_EGA_Text_To_Screen((uint8_t *)"Unknown\rUnknown", BTStatsValueColumn, 0x02, EGA_BrightYellow, EGA_Black);
	}
	else
	{
		uint16_t PilotPartyMemberIndex = Mechs[MechRecordId].pilotId;
		uint16_t PilotNameId = (uint16_t)(int16_t)(int8_t)Characters[PilotPartyMemberIndex].name;
		Draw_EGA_Text_To_Screen(CharacterNames[PilotNameId],
			BTStatsValueColumn, 0x02, EGA_BrightYellow, EGA_Black);

		uint16_t RiderPartyMemberIndex = Mechs[MechRecordId].riderId;
		uint8_t *RiderText = (uint8_t *)"None"; // DS:12AA
		if (RiderPartyMemberIndex != MECH_NoRider)
		{
			uint16_t RiderNameId = (uint16_t)(int16_t)(int8_t)Characters[RiderPartyMemberIndex].name;
			RiderText = CharacterNames[RiderNameId];
		}
		Draw_EGA_Text_To_Screen(RiderText, BTStatsValueColumn, 0x03, EGA_BrightYellow, EGA_Black);
	}

	// Sol: Healthy pips are the component capacity minus accumulated hits.
	Draw_Component_Status_Pips(BTStatsConditionPipColumn, 0x00, MechEngineHitCapacity,
		(uint16_t)(MechEngineHitCapacity - Mechs[MechRecordId].engineHits));
	Draw_Component_Status_Pips(BTStatsConditionPipColumn, 0x01, MechGyroHitCapacity,
		(uint16_t)(MechGyroHitCapacity - Mechs[MechRecordId].gyroHits));
	Draw_Component_Status_Pips(BTStatsConditionPipColumn, 0x02, MechSensorHitCapacity,
		(uint16_t)(MechSensorHitCapacity - Mechs[MechRecordId].sensorHits));

	// Sol: EngineHeatsinks plus every intact 22h critical byte gives the
	// Sol: operational heat-sink count. Destroyed A2h slots are excluded.
	uint16_t OperationalHeatSinks = Mechs[MechRecordId].engineHeatSinks;
	for (uint16_t CriticalSlotOffset = MECH_ComponentBlock_Start;
		 CriticalSlotOffset <= MECH_ComponentBlock_End; CriticalSlotOffset++)
	{
		uint16_t RawComponent =
			((uint8_t *)&Mechs[MechRecordId])[CriticalSlotOffset];
		if (RawComponent == Heat_Sink)
			OperationalHeatSinks++;
	}

	Draw_Component_Status_Pips(BTStatsHeatSinkPipColumn, BTStatsWeaponFirstRow, BTStatsDisplayedHeatSinkCapacity, OperationalHeatSinks);
	uint16_t WeaponDisplayRow = BTStatsWeaponFirstRow;

	// Sol: Each qualifying critical byte represents one weapon in this game's
	// Sol: simplified loadout. The high bit changes its colour but not its ID.
	for (uint16_t CriticalSlotOffset = MECH_ComponentBlock_Start;
		 CriticalSlotOffset <= MECH_ComponentBlock_End; CriticalSlotOffset++)
	{
		uint16_t RawComponent =
			((uint8_t *)&Mechs[MechRecordId])[CriticalSlotOffset];
		uint16_t ComponentId = RawComponent & MechComponentIdMask;
		if (ComponentId < Mech_Small_Laser || ComponentId > Mech_SRMissile6)
			continue;

		uint16_t WeaponColour = EGA_BrightYellow;
		if ((RawComponent & Component_Destroyed) != FALSE)
			WeaponColour = DestroyedStatusColour;

		// Sol: Critical component IDs are one greater than WeaponStats indexes:
		// Sol: component 10h maps to table index 0Fh (Small Laser).
		Draw_EGA_Text_To_Screen(&WeaponStats[ComponentId - MechComponentToWeaponRecordBias].name[0],
			0x00, WeaponDisplayRow, WeaponColour, EGA_Black);

		uint8_t *LocationText = (uint8_t *)"LA"; // DS:12BF
		if (CriticalSlotOffset >= BTStatsCriticalHeadStart)
			LocationText = (uint8_t *)"H";  // DS:12D4
		else if (CriticalSlotOffset >= BTStatsCriticalCenterTorsoStart)
			LocationText = (uint8_t *)"CT"; // DS:12D1
		else if (CriticalSlotOffset >= BTStatsCriticalRightLegStart)
			LocationText = (uint8_t *)"RL"; // DS:12CE
		else if (CriticalSlotOffset >= BTStatsCriticalLeftLegStart)
			LocationText = (uint8_t *)"LL"; // DS:12CB
		else if (CriticalSlotOffset >= BTStatsCriticalRightTorsoStart)
			LocationText = (uint8_t *)"RT"; // DS:12C8
		else if (CriticalSlotOffset >= BTStatsCriticalRightArmStart)
			LocationText = (uint8_t *)"RA"; // DS:12C5
		else if (CriticalSlotOffset >= BTStatsCriticalLeftTorsoStart)
			LocationText = (uint8_t *)"LT"; // DS:12C2

		Draw_EGA_Text_To_Screen(LocationText, BTStatsWeaponLocationColumn, WeaponDisplayRow,
			WeaponColour, EGA_Black);
		WeaponDisplayRow++;
	}

	Draw_EGA_Text_To_Screen((uint8_t *)"Actuators", 0x00, 0x13, EGA_BrightWhite, EGA_Black); // DS:12D6
	// Sol: The retained EGA path sets the shared text colour before drawing the
	// Sol: labels. The removed adapter-zero path overrides it with colour 1;
	// Sol: direct EGA text calls below still supply their own font colour.
	TextColour = EGA_BrightYellow;
	Draw_EGA_Text_To_Screen((uint8_t *)"Left Leg\rRight Leg\rLeft Arm\rRight Arm",
		0x00, 0x14, EGA_BrightWhite, EGA_Black); // DS:12E0

	// Sol: BTSTATS proves the packed actuator layout: byte zero is the left
	// Sol: side and byte one the right; each low nibble is a leg and each high
	// Sol: nibble an arm. Individual bit-to-joint meanings remain unresolved.
	uint16_t CurrentLeftActuators = Mechs[MechRecordId].currentActuators[0];
	uint16_t CurrentRightActuators = Mechs[MechRecordId].currentActuators[1];
	uint16_t MaximumLeftActuators = Mechs[MechRecordId].maxActuators[0];
	uint16_t MaximumRightActuators = Mechs[MechRecordId].maxActuators[1];
	uint8_t *ActuatorStatusText;
	uint16_t ActuatorStatusColour;

	uint16_t CurrentActuatorNibble = CurrentLeftActuators & MechActuatorLegMask;
	if (CurrentActuatorNibble == MechActuatorLegMask)
	{
		ActuatorStatusText = (uint8_t *)" OK"; // DS:1265; leading space aligns the status.
		ActuatorStatusColour = EGA_BrightWhite;
	}
	else if (CurrentActuatorNibble == 0x00)
	{
		ActuatorStatusText = (uint8_t *)"Gone"; // DS:126D
		ActuatorStatusColour = DestroyedStatusColour;
	}
	else
	{
		ActuatorStatusText = (uint8_t *)"Hit"; // DS:1269
		ActuatorStatusColour = EGA_BrightWhite;
	}
	Draw_EGA_Text_To_Screen(ActuatorStatusText, 0x0A, 0x14,
		ActuatorStatusColour, EGA_Black);

	CurrentActuatorNibble = CurrentRightActuators & MechActuatorLegMask;
	if (CurrentActuatorNibble == MechActuatorLegMask)
	{
		ActuatorStatusText = (uint8_t *)" OK";
		ActuatorStatusColour = EGA_BrightWhite;
	}
	else if (CurrentActuatorNibble == 0x00)
	{
		ActuatorStatusText = (uint8_t *)"Gone";
		ActuatorStatusColour = DestroyedStatusColour;
	}
	else
	{
		ActuatorStatusText = (uint8_t *)"Hit";
		ActuatorStatusColour = EGA_BrightWhite;
	}
	Draw_EGA_Text_To_Screen(ActuatorStatusText, 0x0A, 0x15,
		ActuatorStatusColour, EGA_Black);

	CurrentActuatorNibble = CurrentLeftActuators & MechActuatorArmMask;
	uint16_t MaximumActuatorNibble = MaximumLeftActuators & MechActuatorArmMask;
	if (CurrentActuatorNibble == MaximumActuatorNibble)
	{
		ActuatorStatusText = (uint8_t *)" OK";
		ActuatorStatusColour = EGA_BrightWhite;
	}
	else if (CurrentActuatorNibble == 0x00)
	{
		ActuatorStatusText = (uint8_t *)"Gone";
		ActuatorStatusColour = DestroyedStatusColour;
	}
	else
	{
		ActuatorStatusText = (uint8_t *)"Hit";
		ActuatorStatusColour = EGA_BrightWhite;
	}
	Draw_EGA_Text_To_Screen(ActuatorStatusText, 0x0A, 0x16,
		ActuatorStatusColour, EGA_Black);

	CurrentActuatorNibble = CurrentRightActuators & MechActuatorArmMask;
	MaximumActuatorNibble = MaximumRightActuators & MechActuatorArmMask;
	if (CurrentActuatorNibble == MaximumActuatorNibble)
	{
		ActuatorStatusText = (uint8_t *)" OK";
		ActuatorStatusColour = EGA_BrightWhite;
	}
	else if (CurrentActuatorNibble == 0x00)
	{
		ActuatorStatusText = (uint8_t *)"Gone";
		ActuatorStatusColour = DestroyedStatusColour;
	}
	else
	{
		ActuatorStatusText = (uint8_t *)"Hit";
		ActuatorStatusColour = EGA_BrightWhite;
	}
	Draw_EGA_Text_To_Screen(ActuatorStatusText, 0x0A, 0x17,
		ActuatorStatusColour, EGA_Black);

	uint16_t InputReceived = FALSE; // [BP-2A]
	uint16_t AutoDismissRedrawsRemaining = BTStatsAutoDismissRedraws; // [BP-04]
	while (InputReceived == FALSE)
	{
		// Sol: Redraw the eleven armour silhouettes as stacked green armour and
		// Sol: red internal-structure gauges. Rear torso locations have no second
		// Sol: structure value, represented by a zero offset in DS:1306.
		for (uint16_t ArmourLocation = 0x00; ArmourLocation < MechArmourLocationCount; ArmourLocation++)
		{
			uint16_t StructureOffset = MechStructureOffsetByArmourLocation[ArmourLocation];
			uint16_t CurrentStructure = 0x00;

			if (StructureOffset != 0x00)
				CurrentStructure = ((uint8_t *)&Mechs[MechRecordId])[StructureOffset];

			Draw_Vertical_Mech_Status_Gauge(
				MechStatusGaugeX[ArmourLocation],
				MechStatusGaugeBottomY[ArmourLocation],
				Mechs[MechRecordId].currentArmour[ArmourLocation],
				CurrentStructure);
		}


		if (MechHeatLevel[MechRecordId] < 0)
			MechHeatLevel[MechRecordId] = 0;

		if (MechHeatLevel[MechRecordId] > MechHeatShutdownLevel)
			MechHeatLevel[MechRecordId] = MechHeatShutdownLevel;

		uint16_t CurrentMechHeat = MechHeatLevel[MechRecordId];

		// Sol: The heat gauge is the special flashing instance at pixel Y=0xB7.
		Draw_Vertical_Mech_Status_Gauge(BTStatsHeatGaugeX, MechHeatGaugeBottomY,
			(uint16_t)(MechHeatShutdownLevel - CurrentMechHeat), CurrentMechHeat);
		Wait_For_N_Vertical_Retraces(0x01);

		PaletteRefreshFrame++;

		if (PaletteRefreshFrame == BTStatsPaletteRefreshRedraws)
		{
			// Sol: The original chooses DS:1348 for non-MCGA adapters and DS:1358
			// Sol: for MCGA. This maintained transcription retains only the EGA path.
			// Sol:0525 now retains this original16:16 FAR palette source.
			Set_Palette_registers(BTStatsEgaPalette);

			PaletteCyclePhase = (PaletteCyclePhase + 1) & (BTStatsPaletteCycleCount-1);
			// Sol: Both format tables are patched regardless of which adapter is active.
			BTStatsEgaPalette[BTStatsCyclingPaletteEntry] =
				BTStatsEgaRedCycle[PaletteCyclePhase];
			BTStatsMcgaPalette[BTStatsCyclingPaletteEntry] =
				BTStatsMcgaRedCycle[PaletteCyclePhase];
		}
		PaletteRefreshFrame &= BTStatsPaletteRefreshRedraws-1;

		if (DisableInput == FALSE)
			InputReceived = Pending_Input();
		else
		{
			// Sol: With input disabled, the original leaves the screen up for 600
			// Sol: one-retrace redraws. The former redraw flag was a Reko artefact which
			// Sol: incorrectly reduced this branch to a one-frame dismissal.
			AutoDismissRedrawsRemaining--;
			if (AutoDismissRedrawsRemaining == 0x00)
				InputReceived = TRUE;
		}
	}

	// Sol: The shipped MCGA branch restores 2FE8:0010 instead. EGA restores the
	// Sol: sixteen-byte default palette at 2FE8:0000.
	Set_Palette_registers(DefaultEgaPalette);
	Keyboard_Get_ASCII_Hex_Input();
}
