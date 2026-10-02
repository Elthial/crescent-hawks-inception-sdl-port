#include "game.h"
#include <stdio.h>
#include <stdlib.h>

/* Sol: complete original1543:0004..07CA parent, original template corrected
 * against ASM signed gates, FAR data bindings and text-control bytes.
 * Full12-entry stack strlen and invalid native stack indices remain explicitly
 * unsupported, not replaced with collector counts or host undefined reads. */
void Combat_Weapon_UI(uint16_t AttackerCombatantId)
{
	// Sol: Native3092 records/text/targets bind to their original global views.
	// Sol: Signed WORD gates, infantry Weapon CBW and text controls are restored.
	// Sol: List strlen retains its native stack-list role on terminated lists.
	// Sol: BYTE list at BP-20h..BP-15h. The original later uses strlen on it;
	// Sol: do not silently replace that with the collector count (see BUG-012).
	uint8_t WeaponComponents[CombatWeaponTargetSlots] = { 0 };
	if ((int16_t)AttackerCombatantId < Enemy_All_CombatantId_Range_First)
		CombatantActionState[AttackerCombatantId] = 0;

	if ((int16_t)AttackerCombatantId >= Friendly_Infantry_Combatant_Range_First)
	{
		// Sol: Friendly on-foot combat IDs 4..11 map to party records 0..7.
		// Sol: C5DB + combatId*11h = C61F + partySlot*11h (weapon byte +0Bh).
		Combat_Render_Movement_Preview(AttackerCombatantId, 1);
		Menu_Memory_Variables(4);
		Draw_Top_Graphic_Sidebar();
		uint16_t OverrideTarget = TRUE;
		uint16_t WeaponIndex = (uint16_t)(int16_t)(int8_t)Characters[AttackerCombatantId - Friendly_Infantry_Combatant_Range_First].weapon;
		// Sol: CBW sign-extends this target byte. Opcode 83's immediate FF is
		// Sol: also sign-extended: the comparison at 064C is with FFFF, NOT 00FF.
		int16_t ExistingTarget = (int8_t)
			CombatWeaponTarget[AttackerCombatantId * CombatWeaponTargetSlots];
		if (ExistingTarget != -1)
		{
			/* Sol: native CBW can produce a prebiased read outside represented
			 * records. Do not mask the fired bit or revalidate active state here. */
			if(ExistingTarget<6 || ExistingTarget>=AllCombatantCount) { /*Sol:6 is first biased Mech name inside represented C614 save window.*/
				fputs("Unrepresented original1543:0004 signed personnel target view\n",stderr);
				abort();
			}
			Display_Text_From_Memory((uint8_t *)"You are using your "); //3EDB:2AC6
			Display_Text_From_Memory(WeaponStats[WeaponIndex].name);
			Display_Text_From_Memory((uint8_t *)" against a "); //3EDB:2ADA
			if (ExistingTarget >= 16)
				Display_Text_From_Memory((uint8_t *)"Human"); //3EDB:2AE6
			else {
				/* Sol: C33C+signedTarget*125. Targets6/7 alias character bytes
				 * preceding C724, not negative host Mech-array elements. */
				int32_t nameOffset=(int32_t)offsetof(OriginalSavedStateStorage,fields.world)
					+(ExistingTarget-Enemy_Infantry_Record_First)*MechRecordSize;
				Display_Text_From_Memory(OriginalSavedState.bytes+nameOffset);
			}
			// Sol: C33C + enemyCombatId*7Dh aliases Mechs[enemyCombatId-8].
			// Sol: This branch itself neither masks the fired bit nor checks active
			// Sol: state. Normal next-round entry follows1631:0C63..0CC9, which
			// Sol: clears that bit and removes inactive targets across ALL24 units.
			// Sol: AI/picker writes also assign raw IDs. Preserve this signed read;
			// Sol: that caller contract does not certify malformed target bytes.
			Display_Text_From_Memory((uint8_t *)" at "); //3EDB:2AEC
			uint16_t RangeBracket = Combat_Calculate_RangeBracket(ExistingTarget, WeaponIndex);
			Display_Text_From_Memory(WeaponRangeText[RangeBracket]);
			Display_Text_From_Memory((uint8_t *)" range already.\r\rOverride?"); //3EDB:2AF1
			OverrideTarget = Prompt_Yes_No(1);
		}
		if (OverrideTarget != FALSE)
		{
			Draw_Top_Graphic_Sidebar();
			Display_Text_From_Memory((uint8_t *)"Use your\r"); //3EDB:2B0C
			Display_Text_From_Memory(WeaponStats[WeaponIndex].name);
			Display_Text_From_Memory((uint8_t *)"\ragainst:"); //3EDB:2B16
			Combat_Select_Weapon_Target_UI(AttackerCombatantId, WeaponIndex, 0);
		}
		goto RestoreCombatPanels;
	}

	Mech *AttackingMech = &Mechs[AttackerCombatantId];
	uint8_t *MechBytes = (uint8_t *)AttackingMech;
	uint16_t CollectedWeaponCount = 0;
	for (uint16_t CriticalOffset = MECH_ComponentBlock_Start; CriticalOffset <= MECH_ComponentBlock_End; CriticalOffset++)
	{
		if (CollectedWeaponCount >= CombatWeaponTargetSlots)
			continue;
		uint8_t Component = MechBytes[CriticalOffset];
		uint8_t ComponentId = Component & MechComponentIdMask;
		if (ComponentId >= Mech_Small_Laser && ComponentId <= Mech_SRMissile6)
			WeaponComponents[CollectedWeaponCount++] = Component;
	}
	Menu_Memory_Variables(4);
	Draw_Top_Graphic_Sidebar();
	// Sol: Exact heat equality with 30, not >=30. Shutdown sets the high bit
	// Sol: in all twelve planned targets, preserving each low-seven-bit target.
	if (MechHeatLevel[AttackerCombatantId] == MechHeatShutdownLevel)
	{
		Display_Text_From_Memory((uint8_t *)"This 'Mech has overheated and shut down."); //3EDB:2904
		Prompt_And_Wait_For_Key();
		for (uint16_t WeaponSlot = 0; WeaponSlot < CombatWeaponTargetSlots; WeaponSlot++)
			CombatWeaponTarget[AttackerCombatantId * CombatWeaponTargetSlots + WeaponSlot] |= 0x80;
		goto RestoreCombatPanels;
	}
	if (AttackingMech->sensorHits == 1)
	{
		Display_Text_From_Memory((uint8_t *)"This Mech's sensors are damaged. Accuracy is impaired."); //3EDB:292D
		Keyboard_Get_ASCII_Hex_Input();
		Draw_Top_Graphic_Sidebar();
	}
	if (AttackingMech->sensorHits == 2)
	{
		Display_Text_From_Memory((uint8_t *)"This Mech's sensors are destroyed. It cannot fire its weapons."); //3EDB:2964
		Keyboard_Get_ASCII_Hex_Input();
		Draw_Top_Graphic_Sidebar();
		return; //Sol: Native jumps directly to the epilogue, skipping panel restore.
	}
	Display_Text_From_Memory((uint8_t *)"Select the weapon to fire."); //3EDB:29A3
	uint16_t WeaponChoice;
	uint16_t WeaponMenuCount;
	do
	{
		Combat_Render_Movement_Preview(AttackerCombatantId, 1);
		// Sol: Geometry is based on a string LENGTH, not a frame pointer. If
		// Sol: twelve entries were collected there is no NUL left in the array.
		if(CollectedWeaponCount==CombatWeaponTargetSlots) {
			fputs("Unresolved original1543:0004 full-list stack strlen\n",stderr);
			abort();
		}
		WeaponMenuCount = Loop_Until_TextPtr_Null(WeaponComponents);
		MenuPanelLayouts[5].height = WeaponMenuCount + 2;
		MenuPanelLayouts[5].top = 0x16 - WeaponMenuCount;
		Menu_Memory_Variables(5);
		Draw_Menu_Border(0);
		Draw_Top_Graphic_Sidebar();
		Set_Text_Colour_Bright_Green();
		Display_Text_From_Memory((uint8_t *)"Weapon\t\vAmmo Target  Range"); //3EDB:29BE (embedded column controls)
		for (uint16_t WeaponSlot = 0; WeaponSlot < CombatWeaponTargetSlots; WeaponSlot++)
		{
			TextColumn = 0;
			TextRow = WeaponSlot + 1;
			uint8_t Component = WeaponComponents[WeaponSlot];
			if (Component == 0)
				continue;
			uint8_t *PlannedTarget = &CombatWeaponTarget[
				AttackerCombatantId * CombatWeaponTargetSlots + WeaponSlot];
			TextColour = EGA_BrightWhite;
			if (*PlannedTarget != 0xFF)
				TextColour = GraphicsAdapter == 0 ? EGA_Blue : EGA_BrightYellow;
			if ((Component & Component_Destroyed) != 0)
				TextColour = GraphicsAdapter == 0 ? EGA_Green : EGA_DarkGrey;
			uint16_t WeaponIndex = (Component & MechComponentIdMask) - 1;
			Display_Text_From_Memory(WeaponStats[WeaponIndex].name);
			TextColumn = 0x0B;
			if ((Component & Component_Destroyed) != 0)
			{
				Display_Text_From_Memory((uint8_t *)"Destroyed"); //3EDB:29D9
				*PlannedTarget = 0xFF;
				continue;
			}
			// Sol: Native reads raw +27h+slot for ALL twelve UI entries. Slots
			// Sol: ten and eleven alias WalkMove and JumpMove, not ammo (BUG-012).
			uint8_t Ammo = MechBytes[offsetof(Mech,currentAmmo) + WeaponSlot];
			if (Ammo == 0xFF)
				Display_Text_From_Memory((uint8_t *)"Full"); //3EDB:29E3
			else if (Ammo == 0)
				Display_Text_From_Memory((uint8_t *)"Out"); //3EDB:29E8
			else
			{
				uint8_t *DynamicText = (uint8_t *)DynamicString;
				ASM_Text_Formatting(Ammo, DynamicText, 10);
				uint16_t DigitCount = Loop_Until_TextPtr_Null(DynamicText);
				for (uint16_t Column = DigitCount; Column < 3; Column++)
					Display_Text_From_Memory((uint8_t *)" "); //Sol: Left-pad numeric ammo to width three.
				Display_Text_From_Memory(DynamicText);
			}
			TextColumn = 0x10;
			if (*PlannedTarget != 0xFF &&
				CombatantActive[*PlannedTarget & 0x7F] == FALSE)
				*PlannedTarget = 0xFF;
			if (*PlannedTarget == 0xFF)
				Display_Text_From_Memory((uint8_t *)"None"); //3EDB:29F4
			else
			{
				// Sol: Merely rendering an active planned target clears its high bit.
				*PlannedTarget &= 0x7F;
				uint16_t TargetCombatantId = *PlannedTarget;
				uint16_t SavedTextRow = TextRow;
				if (TargetCombatantId >= Enemy_Infantry_CombatantId_Range_First)
					Display_Text_From_Memory((uint8_t *)"Human"); //3EDB:29EE
				else {
					// Sol:1543:0387 computes C33C+id*125, just like0694 in
					// the personnel branch. IDs6/7 address character bytes,
					// not host Mechs[-2/-1]. Preserve the native wrong-name read.
					if (TargetCombatantId < 6) { /* first name address inside represented C614 window */
						fputs("Unrepresented original1543:0004 Mech-panel target name view\n",stderr);
						abort();
					}
					int32_t nameOffset=(int32_t)offsetof(OriginalSavedStateStorage,fields.world)
						+((int32_t)TargetCombatantId-Enemy_Infantry_Record_First)*MechRecordSize;
					Display_Text_From_Memory(OriginalSavedState.bytes+nameOffset);
				}
				TextColumn = 0x18;
				TextRow = SavedTextRow;
				uint16_t RangeBracket = Combat_Calculate_RangeBracket(TargetCombatantId, WeaponIndex);
				Display_Text_From_Memory(WeaponRangeText[RangeBracket]);
			}
		}
		TextColumn = 0;
		WeaponMenuCount = Loop_Until_TextPtr_Null(WeaponComponents);
		TextRow = WeaponMenuCount + 1;
		Display_Text_From_Memory((uint8_t *)"\006\017Done"); //3EDB:29F9 (colour controls precede text)
		MenuControls[5].optionCount = WeaponMenuCount + 1; //Sol: WORD row count, not a pointer.
		Menu_Memory_Variables(5);
		WeaponChoice = Display_Menu_Choices_And_Check(5);
		if((int16_t)WeaponChoice<0) {
			fputs("Unresolved original1543:0004 negative menu stack index\n",stderr);
			abort();
		}
		if ((int16_t)WeaponChoice < (int16_t)WeaponMenuCount)
		{
			uint8_t *PlannedTarget = &CombatWeaponTarget[
				AttackerCombatantId * CombatWeaponTargetSlots + WeaponChoice];
			uint8_t *UnavailableText = 0;
			if ((WeaponComponents[WeaponChoice] & Component_Destroyed) != 0)
				UnavailableText = (uint8_t *)"That weapon has been destroyed.\r\rChoose a different one."; //3EDB:2A00
			else if (MechBytes[offsetof(Mech,currentAmmo) + WeaponChoice] == 0)
				UnavailableText = (uint8_t *)"That weapon is out of ammunition.\r\rChoose a different one."; //3EDB:2A39
			if (UnavailableText != 0)
			{
				// Sol: Distinct messages share this popup/redraw tail; the old goto
				// Sol: accidentally replaced the destroyed message with the ammo one.
				Combat_Weapon_Display_Text_And_Menu_Options(UnavailableText);
				DrawCall_Combat_Menu(TextPanelLeft,
					MenuControls[5].baseRow + TextPanelTop + WeaponChoice,
					MenuControls[5].highlightWidth, MenuControls[5].highlightColour);
			}
			else
			{
				uint16_t OverrideTarget = TRUE;
				if (*PlannedTarget != 0xFF)
				{
					Menu_Memory_Variables(4);
					Draw_Top_Graphic_Sidebar();
					Display_Text_From_Memory((uint8_t *)"That weapon is already going to be fired. Override?"); //3EDB:2A74
					OverrideTarget = Prompt_Yes_No(1);
					Draw_Top_Graphic_Sidebar();
				}
				if (OverrideTarget != FALSE)
				{
					*PlannedTarget = 0xFF; //Sol: Clear old target BEFORE the picker.
					Menu_Memory_Variables(4);
					Draw_Menu_Border(4);
					Draw_Top_Graphic_Sidebar();
					Display_Text_From_Memory((uint8_t *)"Choose the enemy to shoot at:"); //3EDB:2AA8
					Combat_Select_Weapon_Target_UI(AttackerCombatantId,
						(WeaponComponents[WeaponChoice] & 0x7F) - 1, WeaponChoice);
					Menu_Memory_Variables(3);
					Draw_Top_Graphic_Sidebar();
					Menu_Memory_Variables(4);
					Draw_Top_Graphic_Sidebar();
				}
			}
		}
	} while (WeaponChoice != WeaponMenuCount);

RestoreCombatPanels:
	TextColour = EGA_BrightWhite;
	Menu_Memory_Variables(4);
	Draw_Top_Graphic_Sidebar();
	Draw_Menu_Border(4);
	TextColour = EGA_BrightWhite;
	Menu_Memory_Variables(3);
	Draw_Top_Graphic_Sidebar();
	Draw_Menu_Border(3);
	TextColour = EGA_BrightWhite;
}
