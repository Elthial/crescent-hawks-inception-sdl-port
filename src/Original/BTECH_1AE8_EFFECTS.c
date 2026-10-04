#include "game.h"
#include "dos.h"

/* Sol: complete original1AE8:12C7..1E45 parent, translated from annotated C
 * with ASM-correct TABLE indices and native text. Supported original pipeline:
 * EGA adapter2, selected by retained Setup_Game. Removed adapter1 stack-phase
 * redraw behaviour is NOT implemented/certified. State cleanup is unconditional
 * even with graphics disabled. BYTE and WORD storage remains original-width. */
void Combat_AudioVisual_Effects(uint16_t ShooterId, uint16_t TargetCombatantId, uint16_t WeaponIndex, uint16_t AttackDirection, uint16_t PlayTargetImpactStream, uint16_t TargetPersonnelDead, uint16_t TargetMechDestroyed, uint16_t TargetRecordId, uint16_t AttackApplied, uint8_t *SavedCombatMap)
{

	uint16_t SavedShooterFrame=0; /* Sol: used only after native Mech branch assigns it. */
	uint16_t EGA_Colour=0; /* Sol: only beam branches read the assigned colour. */

	uint16_t AnimationScene = AnimationO00_MechStartUp;

	if (CombatDisplayGraphics == FALSE
	 || MainCharactersAlive == FALSE)
	{
Remove_Casualties:

		if (TargetPersonnelDead != FALSE)
		{
			Combat_CombatMessageVerbosityFilter((uint8_t *)"\rKilled him!");
			CombatantCasualtyFlags[TargetCombatantId] = TRUE;
			CombatantActive[TargetCombatantId] = FALSE;
			Characters[TargetRecordId].name = Character_Dead;
			CombatantSpriteFrame[TargetCombatantId] = Sprite_Impact_Small;
			CombatantSpriteFamilyOffset[TargetCombatantId] = MECH_Sprite_LOCUST;
			Register_Persistent_Map_Effect(Sprite_Impact_Small, CombatantPackedX[TargetCombatantId], CombatantPackedY[TargetCombatantId]);

			for (uint16_t ActorId = 0; ActorId < AllCombatantCount; ++ActorId)
			{
				for (uint16_t WeaponSlot = 0; WeaponSlot < CombatWeaponTargetSlots; ++WeaponSlot)
				{
					uint8_t *AssignedTarget = &CombatWeaponTarget[ActorId * CombatWeaponTargetSlots + WeaponSlot];
					if ((*AssignedTarget & CombatTargetIdMask) == TargetCombatantId)
						*AssignedTarget = Character_Dead;
				}
			}
			if (TargetRecordId < MainCharacterCount)
				MainCharactersAlive = FALSE;
			CombatantPackedY[TargetCombatantId] = 0xFFFF;
			CombatantPackedX[TargetCombatantId] = 0xFFFF;
		}
		if (TargetMechDestroyed != FALSE)
		{
			const uint16_t ArenaOpponentId = 13;
			if (ArenaRentalMechMode == FALSE || TargetCombatantId != ArenaOpponentId)
			{
				uint16_t WreckSprite = Sprite_Locust_Wreck;
				if (CombatantSpriteFamilyOffset[TargetCombatantId] != MECH_Sprite_LOCUST)
					WreckSprite = Sprite_Commando_Wreck;
				uint16_t WreckX = CombatantPackedX[TargetCombatantId] - 1;
				uint16_t WreckY = CombatantPackedY[TargetCombatantId] - 1;
				if ((WreckX & 0x80) != 0)
					WreckX &= 0x0F7F;
				if ((WreckY & 0x80) != 0)
					WreckY &= 0xF07F;
				Register_Persistent_Map_Effect(WreckSprite, WreckX, WreckY);
				Play_Sound_If_Enabled(Sound_TerrainDamageUnknown);
			}
			if (ArenaRentalMechMode != FALSE && TargetCombatantId == ArenaOpponentId)
			{

				Draw_Message_Box();
				Play_Sound_If_Enabled(Sound_ArenaDestroyedUnknown);
				Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Your 'Mech has destroyed part of the Arena!");
				Keyboard_Get_ASCII_Hex_Input();
				Move_Map_View_To_Packed_Position(CombatantPackedX[0], CombatantPackedY[0]);
				PosXY_OffsetGrid(CrescentHawkMapPositionX, CrescentHawkMapPositionY);
				Combat_Copy_Map_Cache(SavedCombatMap, FALSE);
				Update_Animated_Map_Tiles();
				Copy_Data_To_GraphicsMemory();
				Draw_Menu_MultiSelect();
				EGA_DrawBox_Wrapper();
				Draw_Message_Box();
				Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Some Kurita UrbanMechs have been dispatched to kill you!");
				Keyboard_Get_ASCII_Hex_Input();

				uint8_t *UrbanTemplate = (uint8_t *)&MechRefs[MechRef_UrbanMech];
				for (uint16_t RecordByte = 0; RecordByte < sizeof(Mech); ++RecordByte)
				{
					((uint8_t *)&Mechs[6])[RecordByte] = UrbanTemplate[RecordByte];
					((uint8_t *)&Mechs[7])[RecordByte] = UrbanTemplate[RecordByte];
				}
				for (uint16_t ReinforcementId = 14; ReinforcementId <= Enemy_Infantry_CombatantId_Range_First-1; ++ReinforcementId)
				{
					CombatantSpriteFrame[ReinforcementId] = 0;
					CombatantSpriteFamilyOffset[ReinforcementId] = MECH_Sprite_LOCUST;
					CombatantAnimationCursors[ReinforcementId] = (AnimationCursor){WalkAnimationStreams,WalkAnimationStreamNativeOffset,0x02A0};
					CombatantActive[ReinforcementId] = TRUE;
					CombatantPackedX[ReinforcementId] = 0x097D;
				}
				CombatantMovementDirection[14] = 4;
				CombatantMovementDirection[15] = 0;
				CombatantAnimationSelector[14] = 4;
				CombatantAnimationSelector[15] = 0;
				CombatantPackedY[14] = 0x8030;
				CombatantPackedY[15] = 0x8070;
				ArenaRentalMechMode = FALSE;
				ArenaEscapeAllowed = TRUE;
				Menu_Memory_Variables(4);
			}

			CombatantPackedY[TargetCombatantId] = 0xFFFF;
			CombatantPackedX[TargetCombatantId] = 0xFFFF;
			CombatantActive[TargetCombatantId] = FALSE;
		}
		if (CombatDisplayGraphics != FALSE)
			CombatantAnimationSelector[ShooterId] = 0xFF;

		return;
	}

	int16_t ViewCombatantId = (int8_t)Characters[Character_Jason].mechAssignment;
	if (ViewCombatantId >= Character_OnFoot)
		ViewCombatantId = Friendly_Infantry_Combatant_Range_First;
	uint16_t CameraPackedX = CombatantPackedX[ViewCombatantId];
	uint16_t CameraPackedY = CombatantPackedY[ViewCombatantId];
	if (CombatantAnimationSelector[ShooterId] != 0xFF)
		CombatSavedFacing[ShooterId] = CombatantAnimationSelector[ShooterId];

	if ((ShooterId >= Friendly_Infantry_Combatant_Range_First && ShooterId < Enemy_All_CombatantId_Range_First)
		|| ShooterId >= Enemy_Infantry_CombatantId_Range_First)
	{

		CombatantAnimationCursors[ShooterId] =
					PersonnelAttackAnimationStreams[WeaponIndex * CompassDirectionCount + AttackDirection];
	}
	else
	{
		SavedShooterFrame = CombatantSpriteFrame[ShooterId];
		if (WeaponIndex != WeaponIndex_Kick)
			CombatantAnimationCursors[ShooterId] =
				CombatantSpriteFamilyOffset[ShooterId] == MECH_Sprite_LOCUST
				? LocustFireAnimationStreams[AttackDirection]
				: CommandoFireAnimationStreams[AttackDirection];
		else
			CombatantAnimationCursors[ShooterId] =
				CombatantSpriteFamilyOffset[ShooterId] == MECH_Sprite_LOCUST
				? LocustKickAnimationStreams[AttackDirection]
				: CommandoKickAnimationStreams[AttackDirection];
	}

	if (WeaponIndex == WeaponIndex_PersonnelSRM || WeaponIndex == WeaponIndex_Inferno)
	{
		PlayTargetImpactStream = FALSE;
		if (TargetCombatantId < Friendly_Infantry_Combatant_Range_First
			|| (TargetCombatantId >= Enemy_All_CombatantId_Range_First && TargetCombatantId < Enemy_Infantry_CombatantId_Range_First))
			AnimationScene = AnimationO01_ManPadVsMech;
	}
	if (WeaponIndex >= WeaponIndex_LRM5 && WeaponIndex <= WeaponIndex_SRM6)
		PlayTargetImpactStream = FALSE;
	if (PlayTargetImpactStream != FALSE)
	{
		if (CombatantAnimationSelector[TargetCombatantId] != 0xFF)
			CombatSavedFacing[TargetCombatantId] = CombatantAnimationSelector[TargetCombatantId];
		CombatantAnimationCursors[TargetCombatantId] = CombatTargetImpactStream;
	}

	while (CombatantAnimationCursors[ShooterId].data[(uint16_t)(CombatantAnimationCursors[ShooterId].offset-CombatantAnimationCursors[ShooterId].dataOffset)] != 0xFF)
	{
		CombatantSpriteFrame[ShooterId] = Advance_Combatant_Animation_Stream(ShooterId);
		if (PlayTargetImpactStream != FALSE)
			CombatantSpriteFrame[TargetCombatantId] = Advance_Combatant_Animation_Stream(TargetCombatantId);
		if (CombatantVisibleOnScreen[ShooterId] != FALSE)
		{
			Combat_Restore_Map_View_And_Draw_World(SavedCombatMap);
			EGA_DrawBox_Wrapper();
			Wait_For_N_Vertical_Retraces(5);
		}
	}

	if (WeaponIndex < WeaponIndex_Vibroblade)
		Play_Sound_If_Enabled(Sound_BladeImpact);
	if (WeaponIndex == WeaponIndex_MachineGun || WeaponIndex == WeaponIndex_SubmachineGun
		|| (WeaponIndex >= WeaponIndex_Autocannon2 && WeaponIndex <= WeaponIndex_Autocannon20))
		Play_Sound_If_Enabled(Sound_RepeatingProjectile);
	if (WeaponIndex == WeaponIndex_Vibroblade)
		Play_Sound_If_Enabled(Sound_VibroBlade);
	if (WeaponIndex == WeaponIndex_Pistol || WeaponIndex == WeaponIndex_Rifle)
		Play_Sound_If_Enabled(Sound_SingleShotProjectile);
	if (WeaponIndex >= WeaponIndex_Shortbow && WeaponIndex <= WeaponIndex_Crossbow)
		Play_Sound_If_Enabled(Sound_BowString);

	uint16_t EffectType = AttackEffect_None;
	if (WeaponIndex == WeaponIndex_PersonnelSRM || WeaponIndex == WeaponIndex_Inferno
		|| (WeaponIndex >= WeaponIndex_LRM5 && WeaponIndex <= WeaponIndex_SRM6))
		EffectType = AttackEffect_Missile;
	uint16_t UsePersonnelLaserSound = FALSE;
	if (WeaponIndex == WeaponIndex_LaserPistol || WeaponIndex == WeaponIndex_LaserRifle)
	{
		EffectType = AttackEffect_PersonnelBeam;
		EGA_Colour = ShooterId >= Enemy_Infantry_CombatantId_Range_First ? EGA_BrightGreen : EGA_BrightYellow;
		if (GraphicsAdapter == 0)
			EGA_Colour = EGA_Cyan;
		UsePersonnelLaserSound = TRUE;
	}
	if (WeaponIndex >= WeaponIndex_SmallLaser && WeaponIndex <= WeaponIndex_PPC)
	{
		EffectType = AttackEffect_MechBeam;
		EGA_Colour = EGA_BrightRed;
		if (ShooterId >= Enemy_All_CombatantId_Range_First)
		{
			EGA_Colour = EGA_Magenta;
			if (TargetCombatantId == 0 && CombatantSpriteFamilyOffset[ShooterId] != MECH_Sprite_LOCUST)
				AnimationScene = AnimationO03_CockpitHit;
		}
		else
			AnimationScene = CombatantSpriteFamilyOffset[ShooterId] == MECH_Sprite_LOCUST
				? AnimationO04_LocustFiring : AnimationO07_WaspFiring;
		if (GraphicsAdapter == 0)
			EGA_Colour = EGA_Green;
	}
	if (EffectType == AttackEffect_None)
		goto Play_Target_Impact;
	if (EffectType == AttackEffect_Missile)
		Play_Sound_If_Enabled(Sound_Missile);
	if (CombatantVisibleOnScreen[ShooterId] == FALSE && CombatantVisibleOnScreen[TargetCombatantId] == FALSE)
		goto Play_Target_Impact;

	int16_t ProjectileX, ProjectileY, TargetX, TargetY;
	if (CombatantVisibleOnScreen[ShooterId] != FALSE)
	{
		ProjectileX = CombatantScreenPixelX[ShooterId];
		ProjectileY = CombatantScreenPixelY[ShooterId];
	}
	else
	{
		Combat_CompassPos(CameraPackedX, CameraPackedY, CombatantPackedX[ShooterId], CombatantPackedY[ShooterId]);
		ProjectileX = MovementActorPositionX << 3;
		ProjectileY = MovementActorPositionY << 3;
	}
	if (CombatantVisibleOnScreen[TargetCombatantId] != FALSE)
	{
		TargetX = CombatantScreenPixelX[TargetCombatantId];
		TargetY = CombatantScreenPixelY[TargetCombatantId];
	}
	else
	{
		Combat_CompassPos(CameraPackedX, CameraPackedY, CombatantPackedX[TargetCombatantId], CombatantPackedY[TargetCombatantId]);
		TargetX = MovementActorPositionX << 3;
		TargetY = MovementActorPositionY << 3;
	}
	if (TargetCombatantId < Friendly_Infantry_Combatant_Range_First
		|| (TargetCombatantId >= Enemy_All_CombatantId_Range_First && TargetCombatantId < Enemy_Infantry_CombatantId_Range_First))
		TargetY -= 8;
	if (EffectType != AttackEffect_Missile)
	{
		TargetX += 3; TargetY += 3;
		if ((ShooterId >= Friendly_Infantry_Combatant_Range_First && ShooterId < Enemy_All_CombatantId_Range_First)
			|| ShooterId >= Enemy_Infantry_CombatantId_Range_First)
		{
			ProjectileX += (int8_t)PersonnelMuzzleX[AttackDirection];
			ProjectileY += (int8_t)PersonnelMuzzleY[AttackDirection];
		}
		else if (CombatantSpriteFamilyOffset[ShooterId] == MECH_Sprite_LOCUST)
		{
			ProjectileX += (int8_t)LocustMuzzleX[AttackDirection];
			ProjectileY += (int8_t)LocustMuzzleY[AttackDirection];
		}
		else
		{
			ProjectileX += (int8_t)CommandoMuzzleX[AttackDirection];
			ProjectileY += (int8_t)CommandoMuzzleY[AttackDirection];
		}
	}
	CombatantAnimationCursors[AllCombatantCount] = MissileAnimationStreams[AttackDirection];
	if (Word_Absolute((int16_t)(uint16_t)(ProjectileX - TargetX)) <= 8 && Word_Absolute((int16_t)(uint16_t)(ProjectileY - TargetY)) <= 8)
		goto Play_Beam_Sound;

	int16_t DeltaY = ProjectileY - TargetY, DeltaX = TargetX - ProjectileX;
	int16_t MajorDistance = Word_Absolute(DeltaY), MinorDistance = Word_Absolute(DeltaX);
	if (MinorDistance > MajorDistance)
	{
		int16_t SwapDistance = MajorDistance; MajorDistance = MinorDistance; MinorDistance = SwapDistance;
	}
	uint16_t Octant = Combat_Projectile_Octant(DeltaY, DeltaX);
	int16_t ErrorAccumulator = MajorDistance >> 1;
	int16_t PrimaryStepX = (int8_t)ProjectilePrimaryStepX[Octant];
	int16_t PrimaryStepY = (int8_t)ProjectilePrimaryStepY[Octant];
	int16_t SecondaryStepX = (int8_t)ProjectileSecondaryStepX[Octant];
	int16_t SecondaryStepY = (int8_t)ProjectileSecondaryStepY[Octant];
	if (EffectType == AttackEffect_Missile)
	{
		PrimaryStepX *= 4; PrimaryStepY *= 4; SecondaryStepX *= 4; SecondaryStepY *= 4;
	}

    /* Sol: adapter1 BP-4 redraw residue is not observable in the maintained EGA2 pipeline. */
	while (ProjectileX != TargetX || ProjectileY != TargetY)
	{
		if (EffectType == AttackEffect_Missile)
		{
			uint16_t MissileSprite = Advance_Combatant_Animation_Stream(EffectAnimationCursorId) + MissileAnimationSpriteBase;
			if (ProjectileX >= MapViewportLeftByte * 8 && ProjectileX < EgaScreenWidth && ProjectileY >= 0 && ProjectileY < EgaScreenHeight)
			{
				/* Sol: EGA draws every visible missile step, unlike removed adapter1. */
				{
					Combat_Restore_Map_View_And_Draw_World(SavedCombatMap);
					Draw_Combat_Sprites(MissileSprite, ProjectileX, ProjectileY);
					EGA_DrawBox_Wrapper();
				}

			}
		}
		else if (ProjectileX >= MapViewportLeftByte * 8 && ProjectileX < EgaScreenWidth && ProjectileY >= 0 && ProjectileY < EgaScreenHeight)
			Draw_Clipped_Axis_Aligned_EGA_Line(ProjectileX, ProjectileY, ProjectileX, ProjectileY, EGA_Colour);
		if (ProjectileX != TargetX) ProjectileX += PrimaryStepX;
		if (ProjectileY != TargetY) ProjectileY += PrimaryStepY;
		ErrorAccumulator -= MinorDistance;
		if (ErrorAccumulator < 0)
		{
			if (ProjectileX != TargetX) ProjectileX += SecondaryStepX;
			if (ProjectileY != TargetY) ProjectileY += SecondaryStepY;
			ErrorAccumulator += MajorDistance;
		}
	}
Play_Beam_Sound:
	if (EffectType != AttackEffect_Missile)
		Play_Sound_If_Enabled(UsePersonnelLaserSound ? Sound_PersonnelLaser : Sound_MechEnergyWeapon);
Play_Target_Impact:
	if (AttackApplied == FALSE)
		Combat_Restore_Map_View_And_Draw_World(SavedCombatMap);
	else
	{
		Play_Sound_If_Enabled(Sound_InfantryUnknown);
		if (WeaponIndex < WeaponIndex_Pistol || CombatantVisibleOnScreen[TargetCombatantId] == FALSE)
			goto Restore_Shooter_Frame;
		CombatantAnimationCursors[AllCombatantCount] = CombatProjectileImpactStream;
		int16_t ImpactX = CombatantScreenPixelX[TargetCombatantId], ImpactY = CombatantScreenPixelY[TargetCombatantId];
		if (TargetCombatantId < Friendly_Infantry_Combatant_Range_First
			|| (TargetCombatantId >= Enemy_All_CombatantId_Range_First && TargetCombatantId < Enemy_Infantry_CombatantId_Range_First))
			ImpactY -= 8;
		while (CombatantAnimationCursors[AllCombatantCount].data[(uint16_t)(CombatantAnimationCursors[AllCombatantCount].offset-CombatantAnimationCursors[AllCombatantCount].dataOffset)] != 0xFF)
		{
			uint16_t ImpactFrame = Advance_Combatant_Animation_Stream(AllCombatantCount);
			Combat_Restore_Map_View_And_Draw_World(SavedCombatMap);
			Draw_Combat_Sprites(ImpactFrame + ProjectileImpactSpriteBase, ImpactX, ImpactY);
			EGA_DrawBox_Wrapper();
		}
		Wait_For_N_Vertical_Retraces(5);
		if (AnimationScene != 0)
			Display_Animation_Scene(AnimationScene, AnimationPlayback_RestoreGameView);
	}
Restore_Shooter_Frame:
	if (ShooterId < Friendly_Infantry_Combatant_Range_First
		|| (ShooterId >= Enemy_All_CombatantId_Range_First && ShooterId < Enemy_Infantry_CombatantId_Range_First))
		CombatantSpriteFrame[ShooterId] = (uint8_t)SavedShooterFrame;
	goto Remove_Casualties;
}
