#include "game.h"

enum {
    LocustStageTwoWalkingMP=7,
    StingerStageOneEnergyWeaponCount=5,
    WaspStageTwoFirstNewLaserOrdinal=2, WaspStageTwoNewLaserOrdinalLimit=5
};

/* Sol: Critical indices below denote confirmed physical entries in the native
 * 35-byte block, not certified anatomical groups (A-005 remains open).
 * Preserved slot ordinals identify the exact original mutations. */
// 11B8:080A: void Mechlube_Modify_Mech(void)
// Called from:
//      Citadel_Building_Dialogs
// Sol: Summary: Select a mech modification package and quote its price.
// Sol: Outline: Check upgrade state, choose the chassis/stage package, and store the package ID and price for the purchase routine.
void Mechlube_Modify_Mech()
{
	// Sol: Bind the original native3092 record to flat Mechs storage.
	// Sol: This routine selects/quotes only; 0925 performs the charge and upgrade.
	Draw_Top_Graphic_Sidebar();
	Display_Text_From_Memory((uint8_t *)"Modify which 'Mech?\r"); //3EDB:1D1E
	Display_Text_Mech_Names();

	Mech* selectedMech = &Mechs[SelectedMechId];

	// Sol: +7Ch is the upgrade-level flag byte. Value 3 means both upgrade
	// Sol: stages are installed; this is not a Chameleon chassis-ID check.
	if (selectedMech->upgradeLevelFlags == MechUpgrade_BothStagesInstalled)
	{
		Draw_Top_Graphic_Sidebar();
		Display_Text_From_Memory((uint8_t *)"Hey, that thing is already pretty powerful.  We can't modify that any further."); //3EDB:1D33
		MechModificationWorkflowEnabled = FALSE;
		return;
	}

	MechModificationWorkflowEnabled = TRUE;
	Draw_Top_Graphic_Sidebar();
	TextColumn = 0x0000;
	TextRow = MechRepairCBillDisplayRow;

	Display_Text_From_Memory((uint8_t *)"C-bills: "); //3EDB:1D82
	Citadel_Building_Dialogs(CitadelDialog_DisplayCBillBalance);
	TextRow = 0x0000;
	TextColumn = 0x0000;

	uint16_t selectedUpgradePackage = selectedMech->upgradePackageBase;

	// Sol: Base values 0..3 select the four supported chassis packages.
	// Sol: Exact level value 1 selects their second-stage entries 4..7.
	if (selectedUpgradePackage > MECH_COMMANDO_LEVEL1)
		selectedUpgradePackage = MECH_UPGRADE_UNSUPPORTED;
	else if (selectedMech->upgradeLevelFlags == MechUpgrade_StageOneInstalled)
		selectedUpgradePackage += MechUpgrade_ChassisCount;

	if (selectedUpgradePackage > MECH_COMMANDO_LEVEL2)
		selectedUpgradePackage = MECH_UPGRADE_UNSUPPORTED;

	SelectedMechUpgradePackage =
		(uint8_t)selectedUpgradePackage;

	if (selectedUpgradePackage == MECH_UPGRADE_UNSUPPORTED)
	{
		// Sol: Original BUG-010: [1D8Ch + 8*2] reads the CR CR bytes at
		// Sol: 1D9Ch. Express the observed 0D0Dh value without C array overrun.
		MechUpgradeCost = MECH_UPGRADE_UNSUPPORTED_PSEUDO_COST;
	}
	else
	{
		MechUpgradeCost =
			MechUpgradeCostByPackage[selectedUpgradePackage];
	}
}

// 11B8:0925: void Mechlube_Upgrade_Mech(void)
// Called from:
//      Citadel_Building_Dialogs
// Sol: Summary: Buy a mech upgrade package.
// Sol: Outline: Select the supported package, check funds and prerequisites, and modify the mech's statistics and equipment.
void Mechlube_Upgrade_Mech()
{
	// Sol: Bind native3092 storage; signed WORD cost remains sign-extended.
	// Sol: All eight package mutations and unsigned DWORD affordability checked.
	Display_Text_From_Memory((uint8_t *)"\r\rAll for the incredibly low price of only "); //3EDB:1D9C
	Display_Text_Dynamic_Value((uint16_t)MechUpgradeCost);
	Display_Text_From_Memory((uint8_t *)" C-bills.  Want to modify your 'Mech?"); //3EDB:1DC8

	if (Prompt_Yes_No(TRUE))
	{
		// Sol: CWD proves the stored WORD is treated as signed before the
		// Sol: unsigned two-WORD comparison with the 32-bit C-bill balance.
		int32_t upgradeCost = (int32_t)MechUpgradeCost;
		if ((uint32_t)upgradeCost > CBills)
		{
			Display_Text_4FA0_Value();
			Display_Text_4FA0_Value();
			Display_Text_Shop_Cannot_Afford_Text();
		}
		else
		{
			// Sol: The charge happens before selector validation. Original
			// Sol: BUG-010 therefore charges pseudo-package 8 before doing no work.
			CBills -= (uint32_t)upgradeCost;

			// Sol: Historical package descriptions retained from the earlier notes.
			// Sol: The case bodies below document the executable's exact mutations.
			//LOCUST
			//1: The two machine guns are swapped by two medium lasers. 
			//2: The fusion generator is swapped by a smaller model, in return two additional small lasers are installed. 

			//WASP
			//1: The SRM-2 is removed in favour of a medium laser and an improved armour. 
			//2: The jump jets are removed to make room for three other medium lasers. 

			//STINGER
			//1: Two machine guns are removed and are more than compensated by four small lasers. 
			//2: The jump jets are removed. For this two medium lasers are installed and the armour is pimped.

			//COMMANDO
			//1: The missile launcher SRM-4 is swapped by two medium lasers and an improved armour.
			//2: The SRM-6 is also removed, a medium laser and two small lasers are added as enhancement. Among experts, the upgrade is called "Blazing Inferno". 

			// Sol: Every package recomputes Mech_Selected * 0x7D in the native
			// Sol: code. This pointer is the cleaned representation for reviewed cases.
			Mech* selectedMech = &Mechs[SelectedMechId];

			Display_Text_CBill_Balance();
			if (SelectedMechUpgradePackage <= MECH_COMMANDO_LEVEL2)
			{
				switch (SelectedMechUpgradePackage)
					{
					case MECH_LOCUST_LEVEL1:
						// Sol: 11B8:09D3-0A0F. The stock Locust's first arm
						// Sol: weapon entries are replaced with medium lasers. Their
						// Sol: paired ammo-state bytes become the laser sentinel 0xFF.
						selectedMech->criticalSlots[14] = Mech_Med_Laser; // record+41h
						selectedMech->criticalSlots[0] = Mech_Med_Laser; // record+33h
						selectedMech->maxAmmo[1] = Ammo_Laser_Infinite;   // record+6Ch
						selectedMech->maxAmmo[0] = Ammo_Laser_Infinite;   // record+6Bh
						selectedMech->currentAmmo[1] = Ammo_Laser_Infinite; // record+28h
						selectedMech->currentAmmo[0] = Ammo_Laser_Infinite; // record+27h
						// Sol: Assignment, not OR: this establishes exact stage-one state.
						selectedMech->upgradeLevelFlags = MechUpgrade_StageOneInstalled; // record+7Ch
						break;

					case MECH_WASP_LEVEL1:
						// Sol: 11B8:0A10-0A97. Replace the first left-leg weapon,
						// Sol: then copy both armour profiles from the reference Locust.
						selectedMech->criticalSlots[28] = Mech_Med_Laser;
						selectedMech->maxAmmo[1] = Ammo_Laser_Infinite;
						selectedMech->currentAmmo[1] = Ammo_Laser_Infinite;
						for (uint16_t armourLocation = 0x0000;
							 armourLocation < MechArmourLocationCount; armourLocation++)
						{							
							selectedMech->currentArmour[armourLocation] =
								MechRefs[MECH_REF_Locust].currentArmour[armourLocation];
							selectedMech->maxArmour[armourLocation] =
								MechRefs[MECH_REF_Locust].maxArmour[armourLocation];
						}
						selectedMech->upgradeLevelFlags = MechUpgrade_StageOneInstalled;
						break;

					case MECH_STINGER_LEVEL1:
						// Sol: 11B8:0A98-0AF2. Four arm critical entries become
						// Sol: small lasers and weapon-state slots 0..4 become infinite.
						selectedMech->criticalSlots[16] = Mech_Small_Laser;
						selectedMech->criticalSlots[15] = Mech_Small_Laser;
						selectedMech->criticalSlots[1] = Mech_Small_Laser;
						selectedMech->criticalSlots[0] = Mech_Small_Laser;
						for (uint16_t weaponSlot = 0x0000;
							 weaponSlot < StingerStageOneEnergyWeaponCount; weaponSlot++)
						{						
							selectedMech->maxAmmo[weaponSlot] = Ammo_Laser_Infinite;
							selectedMech->currentAmmo[weaponSlot] = Ammo_Laser_Infinite;
						}
						selectedMech->upgradeLevelFlags = MechUpgrade_StageOneInstalled;
						break;

					case MECH_COMMANDO_LEVEL1:
						// Sol: 11B8:0AF3-0B75. Preserve the old slot-2 ammo in
						// Sol: slot 3 before converting slots 1 and 2 to energy weapons.
						selectedMech->criticalSlots[23] = Mech_Med_Laser;
						selectedMech->criticalSlots[14] = Mech_Med_Laser;
						selectedMech->currentAmmo[3] = selectedMech->currentAmmo[2];
						selectedMech->maxAmmo[3] = selectedMech->maxAmmo[2];
						selectedMech->maxAmmo[2] = Ammo_Laser_Infinite;
						selectedMech->maxAmmo[1] = Ammo_Laser_Infinite;
						selectedMech->currentAmmo[2] = Ammo_Laser_Infinite;
						selectedMech->currentAmmo[1] = Ammo_Laser_Infinite;
						for (uint16_t armourLocation = 0x0000;
							 armourLocation < MechArmourLocationCount; armourLocation++)
						{				
							uint8_t improvedArmour =
								CommandoStageOneArmour[armourLocation];
							selectedMech->maxArmour[armourLocation] = improvedArmour;
							selectedMech->currentArmour[armourLocation] = improvedArmour;
						}
						selectedMech->upgradeLevelFlags = MechUpgrade_StageOneInstalled;
						break;

					case MECH_LOCUST_LEVEL2:
						// Sol: 11B8:0B76-0BBE. +75h is the verified engine-hit
						// Sol: counter, not a model-of-engine field as the old C claimed.
						selectedMech->walkMove = LocustStageTwoWalkingMP;
						selectedMech->engineHits = 0x00;
						selectedMech->criticalSlots[22] = Mech_Small_Laser;
						selectedMech->criticalSlots[8] = Mech_Small_Laser;
						selectedMech->maxAmmo[4] = Ammo_Laser_Infinite;
						selectedMech->maxAmmo[3] = Ammo_Laser_Infinite;
						selectedMech->currentAmmo[4] = Ammo_Laser_Infinite;
						selectedMech->currentAmmo[3] = Ammo_Laser_Infinite;
						selectedMech->upgradeLevelFlags |= MechUpgrade_StageTwoInstalled;
						break;

					case MECH_WASP_LEVEL2:
						// Sol: 11B8:0BBF-0C31. Remove jump movement and install
						// Sol: three medium-laser entries with ammo-state slots 2..4.
						selectedMech->jumpMove = JumpJets_Removed;
						selectedMech->criticalSlots[32] = Mech_Med_Laser;
						selectedMech->criticalSlots[0] = Mech_Med_Laser;
						selectedMech->criticalSlots[34] = Mech_Med_Laser;
						for (uint16_t laserSlot = WaspStageTwoFirstNewLaserOrdinal;
							 laserSlot < WaspStageTwoNewLaserOrdinalLimit; laserSlot++)
						{						
							selectedMech->maxAmmo[laserSlot] = Ammo_Laser_Infinite;
							selectedMech->currentAmmo[laserSlot] = Ammo_Laser_Infinite;
						}
						selectedMech->upgradeLevelFlags |= MechUpgrade_StageTwoInstalled;
						break;

					case MECH_STINGER_LEVEL2:
						// Sol: 11B8:0C32-0CB9. Both destination armour arrays
						// Sol: receive the Locust template's CURRENT armour bytes.
						selectedMech->jumpMove = JumpJets_Removed;
						for (uint16_t armourLocation = 0x0000;
							 armourLocation < MechArmourLocationCount; armourLocation++)
						{						
							uint8_t locustArmour =
								MechRefs[MECH_REF_Locust].currentArmour[armourLocation];
							selectedMech->maxArmour[armourLocation] = locustArmour;
							selectedMech->currentArmour[armourLocation] = locustArmour;
						}
						selectedMech->criticalSlots[33] = Mech_Med_Laser;
						selectedMech->criticalSlots[32] = Mech_Med_Laser;
						selectedMech->maxAmmo[6] = Ammo_Laser_Infinite;
						selectedMech->maxAmmo[5] = Ammo_Laser_Infinite;
						selectedMech->currentAmmo[6] = Ammo_Laser_Infinite;
						selectedMech->currentAmmo[5] = Ammo_Laser_Infinite;
						selectedMech->upgradeLevelFlags |= MechUpgrade_StageTwoInstalled;
						break;

					case MECH_COMMANDO_LEVEL2:
						// Sol: 11B8:0CBA-0D30 writes 10h to every critical byte
						// Sol: at offsets 4Fh..54h, then writes a MEDIUM laser to +55h.
						// Sol:1AE8/1631 collect each weapon-valued critical byte separately:
						// Sol: these six10h bytes expose six small-laser firing entries, not two.
						// TODO Sol: A-005 still needs the anatomical critical-group labels confirmed.
						selectedMech->criticalSlots[28] = Mech_Small_Laser;
						selectedMech->criticalSlots[29] = Mech_Small_Laser;
						selectedMech->criticalSlots[30] = Mech_Small_Laser;
						selectedMech->criticalSlots[31] = Mech_Small_Laser;
						selectedMech->criticalSlots[32] = Mech_Small_Laser;
						selectedMech->criticalSlots[33] = Mech_Small_Laser;
						selectedMech->criticalSlots[34] = Mech_Med_Laser;
						for (uint16_t weaponSlot = 0x0000;
							 weaponSlot < MechWeaponOrdinalCount; weaponSlot++)
						{
							selectedMech->maxAmmo[weaponSlot] = Ammo_Laser_Infinite;
							selectedMech->currentAmmo[weaponSlot] = Ammo_Laser_Infinite;
						}
						selectedMech->upgradeLevelFlags |= MechUpgrade_StageTwoInstalled;
						break;
					}
			}
			Display_Text_From_Memory((uint8_t *)"\r\rYour 'Mech is done.  It's waiting for you outside."); //3EDB:1DEE
		}
		Keyboard_Get_ASCII_Hex_Input();
	}
}
