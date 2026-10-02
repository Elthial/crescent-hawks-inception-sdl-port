# Sol: memory-variable naming reference

Memory addresses now live here rather than in variable names. These are naming-only
changes to annotated pseudo-code; storage, aliases and behaviour are unchanged.
The cleanup covers 352 memory-view names and 53 stack/local parameter names.
Unknown fields remain explicitly unclassified; aliases are not merged.
Native method addresses and genuine numbered game concepts remain intact.
The preservation C variables already used suffix-free names; no executable
logic changes were necessary there. Historical audit notes retain their old names.

## Stack/local parameter names

### Follow-up suffix cleanup

Sol: Removed another 29 leftover generated suffixes from variables and descriptive
goto labels. Preservation C already uses clean variable names. These are naming-only
changes; no storage, calculations, branching or original bugs changed. Method
address suffixes and genuine numbered game concepts remain intact.

| Previous name | Clean name |
| --- | --- |
| `GeneralMechToSpawn_1178` | `MechSpawnTemplate` |
| `Last_11` | `LastEndpoint` |
| `TranscriptScore_2E` | `TranscriptScore` |
| `TextLength_0E` | `TextLength` |
| `XStart_0C` / `YStart_0E` | `DestinationX` / `DestinationY` |
| `OriginalBuffer_0E` | `OriginalBuffer` |
| `DestinationOffset_0A` | `DestinationOffset` |
| `TonePeriod_019E` | `TonePeriods` (204B:019E) |
| `Bool_Combat_w374C` | `EnemyPersonnelFlightPossible` (3092:374C) |
| `Bool_CombatFlagUnknown_w4586` | `CombatNotificationLatch` (3092:4586) |
| `CGAMemory_t0000` | `CGAMemory` (305B:0000; unused CGA view) |
| `DynamicStringVariable_t0012` | `DynamicString` (3092:0012) |
| `DynamicStringVariable_t0015` | `DynamicStringMapNumberText` (3092:0015; interior view) |
| `MechSnapMemory_a39FA` | `SpritePointerOffsets` (3092:39FA) |
| `MechSnapMemory_t39FC` | `SpritePointerSegments` (3092:39FC) |
| `MenuUnknown_t0094` / `MenuUnknown_t0096` / `MenuUnknown_t009C` | `MenuUnclassifiedPairA` / `MenuUnclassifiedPairB` / `MenuUnclassifiedPairC` (305B:0094/0096/009C) |
| `SpawnedCombatant_b3928` | `SpawnedCombatant` (3092:3928; meaning still uncertain) |

Descriptive labels also lost their address suffixes: `SpriteCleanup`,
`Reset_Pending_Line`, `Append_And_Flush_Advanced_Line`, `Flush_Advanced_Line`,
`Append_Pending_Token_To_Line`, `AssignSelectedCharacterCrew`,
`DisplayCharacterName`, `DisplaySelectedDialogueText` and
`RebuildFriendlyMechSprites`. Existing ASM-address comments retain traceability.

| Previous name | Clean name |
| --- | --- |
| `StringIndex_BP02` | `StringIndex` |
| `SpriteIndex_BP0E` | `SpriteIndex` |
| `SourceRow_BP16` | `SourceRow` |
| `DriveChoice_BP10` | `DriveChoice` |
| `SplashRetracesRemaining_BP02` | `SplashRetracesRemaining` |
| `CacheOpenedId_wArg08` | `CacheOpenedId` |
| `ActorCombatantId_wArg04` | `ActorCombatantId` |
| `ShowPlanningPreview_wArg06` | `ShowPlanningPreview` |
| `Bool_AllowEscape_wLoc0E` | `EscapeAllowed` |
| `MenuIndex_wArg04` | `MenuIndex` |
| `TextCursor_BP04` | `TextCursor` |
| `InitialColumn_BP06` | `InitialColumn` |
| `RowByteOffset_BP0A` | `RowByteOffset` |
| `GlyphByteOffset_BP0C` | `GlyphByteOffset` |
| `DrawColumn_BP08` | `DrawColumn` |
| `Swap_BP02` | `Swap` |
| `AlignmentMask_BP04` | `AlignmentMask` |
| `GroupCount_BP02` | `GroupCount` |
| `RequestedWord_BP02` | `RequestedWord` |
| `AllocationResult_BP06` | `AllocationResult` |
| `RetryOpen_BP04` | `RetryOpen` |
| `FileHandle_BP06` | `FileHandle` |
| `PayloadByteCount_BP02` | `PayloadByteCount` |
| `PayloadBytes_BP06` | `PayloadBytes` |
| `SpriteBuffer_BP04` | `SpriteBuffer` |
| `SourceRowGap_BP08` | `SourceRowGap` |
| `FileHandle_BP02` | `FileHandle` |
| `PitDivisor_wArg04` | `PitDivisor` |
| `DelayMultiplier_wArg06` | `DelayMultiplier` |
| `DelayMask_wArg04` | `DelayMask` |
| `MinimumDelayBits_wArg06` | `MinimumDelayBits` |
| `ToggleCount_wArg08` | `ToggleCount` |
| `DelayMask_wArg94` | `DelayMask` |
| `ToggleDelaySeed_wArg0A` | `ToggleDelaySeed` |
| `CentreDivisor_wArg04` | `CentreDivisor` |
| `HalfSpan_wArg06` | `HalfSpan` |
| `DelayMultiplier_wArg08` | `DelayMultiplier` |
| `SweepCount_wArg0A` | `SweepCount` |
| `DivisorStep_wArg0C` | `DivisorStep` |
| `StartMask_wArg04` | `StartMask` |
| `StopMask_wArg06` | `StopMask` |
| `MinimumBitsSubtract_wArg08` | `MinimumBitsSubtract` |
| `ToggleCount_wArg0A` | `ToggleCount` |
| `MaskStep_wArg0C` | `MaskStep` |
| `MinimumDelay_wArg06` | `MinimumDelay` |
| `MechSnap_OffsetPtr_wArg08` | `MechSnap_OffsetPtr` |
| `MechSnap_SegPtr_wArg0A` | `MechSnap_SegPtr` |
| `SourceOffset_wArg04` | `SourceOffset` |
| `SourceSegment_wArg06` | `SourceSegment` |
| `DestinationOffset_wArg08` | `DestinationOffset` |
| `CentreRegion_wArg04` | `CentreRegion` |
| `Value_wArg04` | `Value` |
| `Radix_wArg0A` | `Radix` |

| Previous name | Clean name | Native memory address |
| --- | --- | --- |
| `DefaultMcgaPalette_0010` | `DefaultMcgaPalette` | `2FE8:0010` |
| `Adapter0ColourMap_0130` | `Adapter0ColourMapBankA` | `2FE8:0130` |
| `Adapter0ColourMap_0150` | `Adapter0ColourMapBankB` | `2FE8:0150` |
| `Adapter0ColourMap_0170` | `Adapter0ColourMapBankC` | `2FE8:0170` |
| `Adapter0ColourMap_0190` | `Adapter0ColourMapBankD` | `2FE8:0190` |
| `Adapter0ColourMap_01B0` | `Adapter0ColourMapBankE` | `2FE8:01B0` |
| `Adapter0ColourMap_01D0` | `Adapter0ColourMapBankF` | `2FE8:01D0` |
| `Adapter0ColourMap_01F0` | `Adapter0ColourMapBankG` | `2FE8:01F0` |
| `Adapter0ColourMap_0210` | `Adapter0ColourMapBankH` | `2FE8:0210` |
| `Adapter0ColourMap_0230` | `Adapter0ColourMapBankI` | `2FE8:0230` |
| `Adapter0ColourMap_0250` | `Adapter0ColourMapBankJ` | `2FE8:0250` |
| `MapPos_0279` | `MapSubdivisionScratchA` | `246C:0279` |
| `MapPos_027A` | `MapSubdivisionScratchB` | `246C:027A` |
| `MapPos_027B` | `MapSubdivisionScratchC` | `246C:027B` |
| `MapPos_027C` | `MapSubdivisionScratchD` | `246C:027C` |
| `MapBlockPhase_0272` | `MapBlockPhase` | `246C:0272` |
| `MapBlockReflection_0273` | `MapBlockReflection` | `246C:0273` |
| `MapRenderSelector_02C9` | `MapRenderSelector` | `246C:02C9` |
| `MapRenderTerrain_02CA` | `MapRenderTerrain` | `246C:02CA` |
| `MapAdjacencyCache_0324` | `MapAdjacencyCache` | `246C:0324` |
| `MapReflectedBlock_20DD` | `MapReflectedBlock` | `246C:20DD` |
| `MapHorizontalReflection_211D` | `MapHorizontalReflection` | `246C:211D` |
| `MapVerticalReflection_213D` | `MapVerticalReflection` | `246C:213D` |
| `Map_NineGrid_TopLeft_0564` | `Map_NineGrid_TopLeft` | `246C:0564` |
| `Map_NineGrid_TopCentre_05A4` | `Map_NineGrid_TopCentre` | `246C:05A4` |
| `Map_NineGrid_TopRight_05E4` | `Map_NineGrid_TopRight` | `246C:05E4` |
| `Map_NineGrid_MiddleLeft_0624` | `Map_NineGrid_MiddleLeft` | `246C:0624` |
| `Map_NineGrid_MiddleRight_06A4` | `Map_NineGrid_MiddleRight` | `246C:06A4` |
| `Map_NineGrid_BottomLeft_06E4` | `Map_NineGrid_BottomLeft` | `246C:06E4` |
| `Map_NineGrid_BottomCentre_0724` | `Map_NineGrid_BottomCentre` | `246C:0724` |
| `Map_NineGrid_BottomRight_0764` | `Map_NineGrid_BottomRight` | `246C:0764` |
| `MapGenerationSeedIndex_09F9` | `MapGenerationSeedIndex` | `246C:09F9` |
| `MapSubdivisionStack_0279` | `MapSubdivisionStack` | `246C:0279` |
| `MapGenerationLattice_02D3` | `MapGenerationLattice` | `246C:02D3` |
| `Bool_ByteVsWordCopy_A44F` | `Bool_ByteVsWordCopy` | `246C:A44F` |
| `Bool_PositionValue_A450` | `Bool_PositionValue` | `246C:A450` |
| `Bool_Unknown_A451` | `Bool_Unknown` | `246C:A451` |
| `EGAColour_Font_B773` | `EGAColour_Font` | `246C:B773` |
| `EGAColour_Background_B776` | `EGAColour_Background` | `246C:B776` |
| `EndingMcgaPalette_0010` | `EndingMcgaPalette` | `3058:0010` |
| `MenuPosY_0092` | `MenuPosY` | `305B:0092` |
| `MenuPosX_0098` | `MenuPosX` | `305B:0098` |
| `InfantryValueArray_C60F` | `InfantryValueArray` | `3092:C60F` |
| `MapPos_0000` | `UnclassifiedMapPositionTableA` | `3EDB:0000` |
| `MapPos_0008` | `UnclassifiedMapPositionTableB` | `3EDB:0008` |
| `MapPos_0048` | `UnclassifiedMapPositionTableC` | `3EDB:0048` |
| `MapPos_0050` | `UnclassifiedMapPositionTableD` | `3EDB:0050` |
| `SharedEquipmentLineBreak_4FA0` | `SharedEquipmentLineBreak` | `3EDB:4FA0` |
| `LaserRifleSwapPromptPrefix_4D8A` | `LaserRifleSwapPromptPrefix` | `3EDB:4D8A` |
| `LaserRifleSwapPromptSuffix_4DA4` | `LaserRifleSwapPromptSuffix` | `3EDB:4DA4` |
| `AllocationNullReturnText_4FE9` | `AllocationNullReturnText` | `3EDB:4FE9` |
| `AllocationTooLargeText_4FDA` | `AllocationTooLargeText` | `3EDB:4FDA` |
| `AlternateArmourShopSelection_0178` | `AlternateArmourShopSelection` | `305B:0178` |
| `AlternativeBldByBuildingId_4602` | `AlternativeBldByBuildingId` | `3092:4602` |
| `AnimatedMapTileFrame_5800` | `AnimatedMapTileFrame` | `3EDB:5800` |
| `AnimatedMapTileFrames_D582` | `AnimatedMapTileFrames` | `3092:D582` |
| `AnimatedMapTileId_04B0` | `AnimatedMapTileId` | `3EDB:04B0` |
| `AnimationStreamOffset_0064` | `AnimationStreamOffset` | `3092:0064` |
| `AnimationStreamSegment_0066` | `AnimationStreamSegment` | `3092:0066` |
| `ArenaCombatMapPatchVariant_0010` | `ArenaCombatMapPatchVariant` | `3092:0010` |
| `ArenaMechRecordBackup_3780` | `ArenaMechRecordBackup` | `3092:3780` |
| `ArenaRentalMechMode_E48E` | `ArenaRentalMechMode` | `3092:E48E` |
| `ArmourPurchaseCost_4F2A` | `ArmourPurchaseCost` | `3EDB:4F2A` |
| `ArmourRepairCostPerPoint_4F3E` | `ArmourRepairCostPerPoint` | `3EDB:4F3E` |
| `ArmourShopSelection_0168` | `ArmourShopSelection` | `305B:0168` |
| `BiosVideoModeByGraphicsAdapter_1140` | `BiosVideoModeByGraphicsAdapter` | `3EDB:1140` |
| `BldFileNameById_4EC2` | `BldFileNameById` | `3EDB:4EC2` |
| `BldInteractionFlag_D316` | `BldInteractionFlag` | `3092:D316` |
| `BlockingTileCodeThreshold_0150` | `BlockingTileCodeThreshold` | `3EDB:0150` |
| `Bool_AllowEscape_D32F` | `Bool_AllowEscape` | `3092:D32F` |
| `Bool_DeadInfantry_3954` | `Bool_DeadInfantry` | `3092:3954` |
| `Bool_EnemiesWithinRange_3992` | `Bool_EnemiesWithinRange` | `3092:3992` |
| `Bool_Input_458C` | `Bool_Input` | `3092:458C` |
| `Bool_ShowArmShotOffAnimation_3986` | `Bool_ShowArmShotOffAnimation` | `3092:3986` |
| `BTStatsAssetLoaded_4594` | `BTStatsAssetLoaded` | `3092:4594` |
| `BTStatsEgaPalette_1348` | `BTStatsEgaPalette` | `3EDB:1348` |
| `BTStatsEgaRedCycle_1378` | `BTStatsEgaRedCycle` | `3EDB:1378` |
| `BTStatsMcgaPalette_1358` | `BTStatsMcgaPalette` | `3EDB:1358` |
| `BTStatsMcgaRedCycle_137C` | `BTStatsMcgaRedCycle` | `3EDB:137C` |
| `BusyWaitScale_5006` | `BusyWaitScale` | `3EDB:5006` |
| `CachedMapGridSlotPointer_0170` | `CachedMapGridSlotPointer` | `3EDB:0170` |
| `CachedMapOriginColumn_09EF` | `CachedMapOriginColumn` | `246C:09EF` |
| `CachedMapOriginIndex_09ED` | `CachedMapOriginIndex` | `246C:09ED` |
| `CachedMapOriginRowOffset_09F1` | `CachedMapOriginRowOffset` | `246C:09F1` |
| `CachedMapPositionX_02CF` | `CachedMapPositionX` | `246C:02CF` |
| `CachedMapPositionY_02D1` | `CachedMapPositionY` | `246C:02D1` |
| `CacheDoorAnimationTiles_246E` | `CacheDoorAnimationTiles` | `3EDB:246E` |
| `CacheDoorOpened_D34F` | `CacheDoorOpened` | `3092:D34F` |
| `CacheDoorPositionX_247A` | `CacheDoorPositionX` | `3EDB:247A` |
| `CacheDoorPositionY_2486` | `CacheDoorPositionY` | `3EDB:2486` |
| `CacheDoorRequiredBlue_249E` | `CacheDoorRequiredBlue` | `3EDB:249E` |
| `CacheDoorRequiredRed_2492` | `CacheDoorRequiredRed` | `3EDB:2492` |
| `CacheDoorRequiredYellow_24AA` | `CacheDoorRequiredYellow` | `3EDB:24AA` |
| `CacheEntryMechNameInitial_3248` | `CacheEntryMechNameInitial` | `3092:3248` |
| `CacheOverviewTileColours_2257` | `CacheOverviewTileColours` | `246C:2257` |
| `CacheStarPuzzleTargetOffsets_241E` | `CacheStarPuzzleTargetOffsets` | `3EDB:241E` |
| `Combat_AudioVisual_4314` | `Combat_AudioVisual` | `3092:4314` |
| `CombatantActionState_3994` | `CombatantActionState` | `3092:3994` |
| `CombatantActive_406A` | `CombatantActive` | `3092:406A` |
| `CombatantAnimationCursor_01F6` | `CombatantAnimationCursor` | `3EDB:01F6` |
| `CombatantAnimationSelector_396C` | `CombatantAnimationSelector` | `3092:396C` |
| `CombatantFacing_3920` | `CombatantFacing` | `3092:3920` |
| `CombatantPackedX_4004` | `CombatantPackedX` | `3092:4004` |
| `CombatantPackedY_4036` | `CombatantPackedY` | `3092:4036` |
| `CombatantScreenX_324C` | `CombatantScreenX` | `3092:324C` |
| `CombatantScreenY_327C` | `CombatantScreenY` | `3092:327C` |
| `CombatantSpriteFamilyOffset_D55E` | `CombatantSpriteFamilyOffset` | `3092:D55E` |
| `CombatantSpriteFrame_409A` | `CombatantSpriteFrame` | `3092:409A` |
| `CombatChosenStepX_458E` | `CombatChosenStepX` | `3092:458E` |
| `CombatChosenStepY_4590` | `CombatChosenStepY` | `3092:4590` |
| `CombatDestinationBlocked_D57E` | `CombatDestinationBlocked` | `3092:D57E` |
| `CombatHitCategoryByFacingDifference_2D0A` | `CombatHitCategoryByFacingDifference` | `3EDB:2D0A` |
| `CombatImpactAnimationStream_41D8` | `CombatImpactAnimationStream` | `3EDB:41D8` |
| `CombatMap_07AD` | `CombatMap` | `246C:07AD` |
| `CombatMechHitLocationOffsets_2E42` | `CombatMechHitLocationOffsets` | `3EDB:2E42` |
| `CombatMechPreviousMapRowTile_45CE` | `CombatMechPreviousMapRowTile` | `3092:45CE` |
| `CombatMechYParityAdjustment_4554` | `CombatMechYParityAdjustment` | `3092:4554` |
| `CombatMissileClusterTable_2E6E` | `CombatMissileClusterTable` | `3EDB:2E6E` |
| `CombatMovementOrders_32C6` | `CombatMovementOrders` | `3092:32C6` |
| `CombatMovementPlanBytes_40B4` | `CombatMovementPlanBytes` | `3092:40B4` |
| `CombatMovementPreviewGlyphs_3B06` | `CombatMovementPreviewGlyphs` | `3EDB:3B06` |
| `CombatMovementStepCursor_0078` | `CombatMovementStepCursor` | `3092:0078` |
| `CombatMovementTerrainMasks_3B12` | `CombatMovementTerrainMasks` | `3EDB:3B12` |
| `CombatPathBoundaryX_32AA` | `CombatPathBoundaryX` | `3EDB:32AA` |
| `CombatPathBoundaryY_32BA` | `CombatPathBoundaryY` | `3EDB:32BA` |
| `CombatPathCacheRowDelta_32CA` | `CombatPathCacheRowDelta` | `3EDB:32CA` |
| `CombatPathStepX_328A` | `CombatPathStepX` | `3EDB:328A` |
| `CombatPathStepY_329A` | `CombatPathStepY` | `3EDB:329A` |
| `CombatSavedFacing_45B6` | `CombatSavedFacing` | `3092:45B6` |
| `CombatSpritePointers_39FA` | `CombatSpritePointers` | `3092:39FA` |
| `CombatTargetImpactStream_2E3C` | `CombatTargetImpactStream` | `3EDB:2E3C` |
| `CombatTargetMovementPenalty_2D1A` | `CombatTargetMovementPenalty` | `3EDB:2D1A` |
| `CombatWeaponHeat_0092` | `CombatWeaponHeat` | `3092:0092` |
| `CombatWeaponTarget_3800` | `CombatWeaponTarget` | `3092:3800` |
| `CombinedCounter_D343` | `CombinedCounter` | `3092:D343` |
| `CommandoFireAnimationStreams_2D78` | `CommandoFireAnimationStreams` | `3EDB:2D78` |
| `CommandoKickAnimationStreams_2DB8` | `CommandoKickAnimationStreams` | `3EDB:2DB8` |
| `CommandoMuzzleX_2D38` | `CommandoMuzzleX` | `3EDB:2D38` |
| `CommandoMuzzleY_2D40` | `CommandoMuzzleY` | `3EDB:2D40` |
| `CommandoStageOneArmour_1E24` | `CommandoStageOneArmour` | `3EDB:1E24` |
| `CompassDirectionLookup_0240` | `CompassDirectionLookup` | `246C:0240` |
| `ComstarDecrementTimer_D323` | `ComstarDecrementTimer` | `3092:D323` |
| `Counter_D344` | `WorldCountdownLowByte` | `3092:D344` |
| `Counter_D345` | `WorldCountdownHighByte` | `3092:D345` |
| `CpuTimingCalibration_3FF4` | `CpuTimingCalibration` | `3092:3FF4` |
| `CrescentHawks_0208` | `CrescentHawks` | `305B:0208` |
| `CriticalHighActuatorClearMask_3185` | `CriticalHighActuatorClearMask` | `3EDB:3185` |
| `CriticalHighActuatorHitMask_317D` | `CriticalHighActuatorHitMask` | `3EDB:317D` |
| `CriticalLowActuatorClearMask_3175` | `CriticalLowActuatorClearMask` | `3EDB:3175` |
| `CriticalLowActuatorHitMask_316D` | `CriticalLowActuatorHitMask` | `3EDB:316D` |
| `CriticalSectionCount_3192` | `CriticalSectionCount` | `3EDB:3192` |
| `CriticalSectionStart_318A` | `CriticalSectionStart` | `3EDB:318A` |
| `CurrentMenuLayoutIndex_4600` | `CurrentMenuLayoutIndex` | `3092:4600` |
| `DecrementTimer_D322` | `UnclassifiedWorldCountdownB` | `3092:D322` |
| `DecrementTimer_D329` | `UnclassifiedWorldCountdownA` | `3092:D329` |
| `DefaultEgaPalette_0000` | `DefaultEgaPalette` | `2FE8:0000` |
| `DestroyedMechNameInitial_323E` | `DestroyedMechNameInitial` | `3092:323E` |
| `DiskDriveNameText_0504` | `DiskDriveNameText` | `3EDB:0504` |
| `DynamicStringTerminator_001A` | `DynamicStringTerminator` | `3092:001A` |
| `EgaPrimitiveColour_0224` | `EgaPrimitiveColour` | `246C:0224` |
| `EgaPrimitiveGroupCount_0230` | `EgaPrimitiveGroupCount` | `246C:0230` |
| `EgaPrimitiveX_0220` | `EgaPrimitiveX` | `246C:0220` |
| `EgaPrimitiveYEnd_0236` | `EgaPrimitiveYEnd` | `246C:0236` |
| `EgaPrimitiveYStart_0234` | `EgaPrimitiveYStart` | `246C:0234` |
| `EndingEgaPalette_0000` | `EndingEgaPalette` | `3058:0000` |
| `EnemyInfantryWeaponByRollSum_2CF4` | `EnemyInfantryWeaponByRollSum` | `3EDB:2CF4` |
| `EnemyMechArmourDamageByLevel_165A` | `EnemyMechArmourDamageByLevel` | `3EDB:165A` |
| `EnemyMechDamageLevel_D30F` | `EnemyMechDamageLevel` | `3092:D30F` |
| `EnemyMechSpriteFamilyBy2D6Minus2_140E` | `EnemyMechSpriteFamilyBy2D6Minus2` | `3EDB:140E` |
| `EnemyMechTemplateBy2D6Minus2_13E2` | `EnemyMechTemplateBy2D6Minus2` | `3EDB:13E2` |
| `EnemySpawnOffsetX_1642` | `EnemySpawnOffsetX` | `3EDB:1642` |
| `EnemySpawnOffsetY_164E` | `EnemySpawnOffsetY` | `3EDB:164E` |
| `EnteredBuildingId_4584` | `EnteredBuildingId` | `3092:4584` |
| `EntranceBldState_D342` | `EntranceBldState` | `3092:D342` |
| `FirearmShopSelection_01A8` | `FirearmShopSelection` | `305B:01A8` |
| `FixedToneDelayMultiplier_E48C` | `FixedToneDelayMultiplier` | `3092:E48C` |
| `FixedTonePitDivisor_3984` | `FixedTonePitDivisor` | `3092:3984` |
| `FrameNumber_E48A` | `FrameNumber` | `3092:E48A` |
| `FriendlyInfantryWalkAnimationByDirection_027A` | `FriendlyInfantryWalkAnimationByDirection` | `3EDB:027A` |
| `FriendlyMechWalkAnimationByDirection_025A` | `FriendlyMechWalkAnimationByDirection` | `3EDB:025A` |
| `Graphic_DestinationIndex_B78E` | `Graphic_DestinationIndex` | `246C:B78E` |
| `Graphic_DestinationSegment_B790` | `Graphic_DestinationSegment` | `246C:B790` |
| `Graphic_SourceIndex_B78A` | `Graphic_SourceIndex` | `246C:B78A` |
| `Graphic_SourceSegment_B78C` | `Graphic_SourceSegment` | `246C:B78C` |
| `GraphicsCompatibilityFlag_4FBC` | `GraphicsCompatibilityFlag` | `3EDB:4FBC` |
| `HealingDiceBySkillAndEquipment_2602` | `HealingDiceBySkillAndEquipment` | `3EDB:2602` |
| `HeavyWeaponShopSelection_01B8` | `HeavyWeaponShopSelection` | `305B:01B8` |
| `HoldRickAtlasUntilLoungeConversation_D339` | `HoldRickAtlasUntilLoungeConversation` | `3092:D339` |
| `HorizontalSpanAlignmentMask_4FC4` | `HorizontalSpanAlignmentMask` | `3EDB:4FC4` |
| `HorizontalSpanEndAlignmentMask_4FCC` | `HorizontalSpanEndAlignmentMask` | `3EDB:4FCC` |
| `HorizontalSpanGroupShift_4FD4` | `HorizontalSpanGroupShift` | `3EDB:4FD4` |
| `InfantryWalkAnimationStreams_02B0` | `InfantryWalkAnimationStreams` | `2FE8:02B0` |
| `InfantryWeaponPurchaseCost_4F44` | `InfantryWeaponPurchaseCost` | `3EDB:4F44` |
| `KuritaPartyEnemyDeltaX_1E7A` | `KuritaPartyEnemyDeltaX` | `3EDB:1E7A` |
| `KuritaPartyEnemyDeltaY_1E82` | `KuritaPartyEnemyDeltaY` | `3EDB:1E82` |
| `LastWeaponProficiencyCategory_D364` | `LastWeaponProficiencyCategory` | `3092:D364` |
| `LocalMapBlockX_02CD` | `LocalMapBlockX` | `246C:02CD` |
| `LocalMapBlockY_02CE` | `LocalMapBlockY` | `246C:02CE` |
| `LocalTerrainFlags_07A4` | `LocalTerrainFlags` | `246C:07A4` |
| `LocustFireAnimationStreams_2D58` | `LocustFireAnimationStreams` | `3EDB:2D58` |
| `LocustKickAnimationStreams_2D98` | `LocustKickAnimationStreams` | `3EDB:2D98` |
| `LocustMuzzleX_2D28` | `LocustMuzzleX` | `3EDB:2D28` |
| `LocustMuzzleY_2D30` | `LocustMuzzleY` | `3EDB:2D30` |
| `LootableInfantryFlags_395C` | `LootableInfantryFlags` | `3092:395C` |
| `Map_NineGrid_MiddleCentre_0664` | `Map_NineGrid_MiddleCentre` | `246C:0664` |
| `MapCopyBottomHalf_A451` | `MapCopyBottomHalf` | `246C:A451` |
| `MapCopyHalfHeight_A44F` | `MapCopyHalfHeight` | `246C:A44F` |
| `MapCopyLeftHalf_A450` | `MapCopyLeftHalf` | `246C:A450` |
| `MapDescriptorCache_0564` | `MapDescriptorCache` | `246C:0564` |
| `MapEffectPackedPage_D497` | `MapEffectPackedPage` | `3092:D497` |
| `MapEffectPositionXLow_D4D7` | `MapEffectPositionXLow` | `3092:D4D7` |
| `MapEffectPositionYLow_D517` | `MapEffectPositionYLow` | `3092:D517` |
| `MapEffectSpriteIndex_D457` | `MapEffectSpriteIndex` | `3092:D457` |
| `MapFullBandGap_A454` | `MapFullBandGap` | `246C:A454` |
| `MapFullBandsRemaining_A458` | `MapFullBandsRemaining` | `246C:A458` |
| `MapHalfBandGap_A456` | `MapHalfBandGap` | `246C:A456` |
| `MapRoomOverviewTileColours_2351` | `MapRoomOverviewTileColours` | `246C:2351` |
| `MapTileDestinationWidth_A452` | `MapTileDestinationWidth` | `246C:A452` |
| `Mech_Heat_D576` | `Mech_Heat` | `3092:D576` |
| `MechHeatGaugeFlashColour_121C` | `MechHeatGaugeFlashColour` | `3EDB:121C` |
| `MechHeatGaugeFlashCountdown_1218` | `MechHeatGaugeFlashCountdown` | `3EDB:1218` |
| `MechHeatGaugeFlashResetDelay_121A` | `MechHeatGaugeFlashResetDelay` | `3EDB:121A` |
| `MechId_D558` | `MechId` | `3092:D558` |
| `MechModificationWorkflowEnabled_D31E` | `MechModificationWorkflowEnabled` | `3092:D31E` |
| `MechQuizAnswerRow_28EC` | `MechQuizAnswerRow` | `3EDB:28EC` |
| `MechQuizTargetX_28C4` | `MechQuizTargetX` | `3EDB:28C4` |
| `MechQuizTargetY_28D8` | `MechQuizTargetY` | `3EDB:28D8` |
| `MechSnapSeg_025C` | `MechSnapSeg` | `246C:025C` |
| `MechStatusGaugeBottomY_1332` | `MechStatusGaugeBottomY` | `3EDB:1332` |
| `MechStatusGaugeX_131C` | `MechStatusGaugeX` | `3EDB:131C` |
| `MechStructureOffsetByArmourLocation_1306` | `MechStructureOffsetByArmourLocation` | `3EDB:1306` |
| `MechTemplateByType_2DF8` | `MechTemplateByType` | `3EDB:2DF8` |
| `MechUpgradeCostByPackage_1D8C` | `MechUpgradeCostByPackage` | `3EDB:1D8C` |
| `MechWalkAnimationStream_02A0` | `MechWalkAnimationStream` | `2FE8:02A0` |
| `MechWalkAnimationStreams_0270` | `MechWalkAnimationStreams` | `2FE8:0270` |
| `MedicalEquipmentText_25F2` | `MedicalEquipmentText` | `3EDB:25F2` |
| `MedicalServiceFee_4F6E` | `MedicalServiceFee` | `3EDB:4F6E` |
| `MedicalSkillText_25E2` | `MedicalSkillText` | `3EDB:25E2` |
| `MeleeWeaponShopSelection_0198` | `MeleeWeaponShopSelection` | `305B:0198` |
| `MenuTextBackgroundColour_377E` | `MenuTextBackgroundColour` | `3092:377E` |
| `MissileAmmoPriceByComponent_2060` | `MissileAmmoPriceByComponent` | `3EDB:2060` |
| `MissileAnimationStreams_2DD8` | `MissileAnimationStreams` | `3EDB:2DD8` |
| `MissionNpcUpdatePhase_5802` | `MissionNpcUpdatePhase` | `3EDB:5802` |
| `MovementDirectionSearchOffsets_310A` | `MovementDirectionSearchOffsets` | `3EDB:310A` |
| `MovementDirectionStepX_311A` | `MovementDirectionStepX` | `3EDB:311A` |
| `MovementDirectionStepY_312A` | `MovementDirectionStepY` | `3EDB:312A` |
| `MovementPackedBoundaryX_313A` | `MovementPackedBoundaryX` | `3EDB:313A` |
| `MovementPackedBoundaryY_314A` | `MovementPackedBoundaryY` | `3EDB:314A` |
| `MovementRateDefaultSelection_0308` | `MovementRateDefaultSelection` | `305B:0308` |
| `MovementRateOptionCount_0306` | `MovementRateOptionCount` | `305B:0306` |
| `MovementRateSubmenuActive_0302` | `MovementRateSubmenuActive` | `305B:0302` |
| `MovementSearchBank_315C` | `MovementSearchBank` | `3EDB:315C` |
| `MovementSearchBankBytes_315C` | `MovementSearchBankBytes` | `3EDB:315C` |
| `MovementSearchBankTimer_315A` | `MovementSearchBankTimer` | `3EDB:315A` |
| `MovementStepDirectionByDelta_2ECC` | `MovementStepDirectionByDelta` | `3EDB:2ECC` |
| `MtpBlockColumns_0A54` | `MtpBlockColumns` | `3EDB:0A54` |
| `MtpBlockOffsetX_0A38` | `MtpBlockOffsetX` | `3EDB:0A38` |
| `MtpBlockOffsetY_0A46` | `MtpBlockOffsetY` | `3EDB:0A46` |
| `MtpBlockRows_0A62` | `MtpBlockRows` | `3EDB:0A62` |
| `MtpOverheadReadSpan_0A70` | `MtpOverheadReadSpan` | `3EDB:0A70` |
| `NewGameStateReset_D30C` | `NewGameStateReset` | `3092:D30C` |
| `NextMapEffectSlot_D557` | `NextMapEffectSlot` | `3092:D557` |
| `NextRecruitNameId_D456` | `NextRecruitNameId` | `3092:D456` |
| `NoiseCountdownMask_006C` | `NoiseCountdownMask` | `3092:006C` |
| `NoiseMinimumDelayBits_3776` | `NoiseMinimumDelayBits` | `3092:3776` |
| `NoiseSweepMaskStep_009C` | `NoiseSweepMaskStep` | `3092:009C` |
| `NoiseSweepMinimumBitsSubtract_39F6` | `NoiseSweepMinimumBitsSubtract` | `3092:39F6` |
| `NoiseSweepStartMask_398C` | `NoiseSweepStartMask` | `3092:398C` |
| `NoiseSweepStopMask_39A2` | `NoiseSweepStopMask` | `3092:39A2` |
| `NoiseSweepToggleCount_3FF2` | `NoiseSweepToggleCount` | `3092:3FF2` |
| `NoiseToggleCount_4312` | `NoiseToggleCount` | `3092:4312` |
| `NoiseToggleDelaySeed_398A` | `NoiseToggleDelaySeed` | `3092:398A` |
| `NpcActivityBuildingId_D319` | `NpcActivityBuildingId` | `3092:D319` |
| `NpcUpdatePhase_57FE` | `NpcUpdatePhase` | `3EDB:57FE` |
| `NumberOfPartyMembers_0206` | `NumberOfPartyMembers` | `305B:0206` |
| `ObjectiveDirectionGlyphX_0A8A` | `ObjectiveDirectionGlyphX` | `3EDB:0A8A` |
| `ObjectiveDirectionGlyphY_0A92` | `ObjectiveDirectionGlyphY` | `3EDB:0A92` |
| `OuttakeFrequencyMenuSelection_02F8` | `OuttakeFrequencyMenuSelection` | `305B:02F8` |
| `OverheadMapActive_014C` | `OverheadMapActive` | `3EDB:014C` |
| `OverviewTileColours_215D` | `OverviewTileColours` | `246C:215D` |
| `Party_UnassignedMechs_D324` | `Party_UnassignedMechs` | `3092:D324` |
| `PauseMenuOptionCount_00A6` | `PauseMenuOptionCount` | `305B:00A6` |
| `PauseMenuWindowHeight_0016` | `PauseMenuWindowHeight` | `305B:0016` |
| `PauseMenuWindowTopRow_0012` | `PauseMenuWindowTopRow` | `305B:0012` |
| `PendingMapGridSlot_09F3` | `PendingMapGridSlot` | `246C:09F3` |
| `PendingMapRegionIndex_09F6` | `PendingMapRegionIndex` | `246C:09F6` |
| `PersistentBldState_D30C` | `PersistentBldState` | `3092:D30C` |
| `PersonnelAttackAnimationStreams_3FF8` | `PersonnelAttackAnimationStreams` | `3EDB:3FF8` |
| `PersonnelMuzzleX_2D48` | `PersonnelMuzzleX` | `3EDB:2D48` |
| `PersonnelMuzzleY_2D50` | `PersonnelMuzzleY` | `3EDB:2D50` |
| `PosX_0262` | `PosX` | `246C:0262` |
| `PosX_Div8_0266` | `PosX_Div8` | `246C:0266` |
| `PosY_0264` | `PosY` | `246C:0264` |
| `ProjectilePrimaryStepX_41DC` | `ProjectilePrimaryStepX` | `3EDB:41DC` |
| `ProjectilePrimaryStepY_41E4` | `ProjectilePrimaryStepY` | `3EDB:41E4` |
| `ProjectileSecondaryStepX_41EC` | `ProjectileSecondaryStepX` | `3EDB:41EC` |
| `ProjectileSecondaryStepY_41F4` | `ProjectileSecondaryStepY` | `3EDB:41F4` |
| `RandomByte0_4FC0` | `RandomByte0` | `3EDB:4FC0` |
| `RandomByte1_4FC1` | `RandomByte1` | `3EDB:4FC1` |
| `RandomByte2_4FC2` | `RandomByte2` | `3EDB:4FC2` |
| `RandomSeed_4FC0` | `RandomSeed` | `3EDB:4FC0` |
| `RandomState_4FC2` | `RandomState` | `3EDB:4FC2` |
| `RequestedGameDiskNumber_014E` | `RequestedGameDiskNumber` | `3EDB:014E` |
| `RequiredMainCharacterDead_D334` | `RequiredMainCharacterDead` | `3092:D334` |
| `RexStartingSkills_1E7A` | `RexStartingSkills` | `3EDB:1E7A` |
| `RickAtlasConversationTriggered_D33A` | `RickAtlasConversationTriggered` | `3092:D33A` |
| `RoamingNpcWaypointLink_3768` | `RoamingNpcWaypointLink` | `3092:3768` |
| `SalvageDescriptionByTechSkill_0FA2` | `SalvageDescriptionByTechSkill` | `3EDB:0FA2` |
| `SalvageFailureNamePlaceholder_002D` | `SalvageFailureNamePlaceholder` | `3092:002D` |
| `SalvageSuccessNamePlaceholder_0022` | `SalvageSuccessNamePlaceholder` | `3092:0022` |
| `SavedArenaMechPilotId_430E` | `SavedArenaMechPilotId` | `3092:430E` |
| `SavedArenaMechRiderId_3FFA` | `SavedArenaMechRiderId` | `3092:3FFA` |
| `SavedPartyNameId_3FE9` | `SavedPartyNameId` | `3092:3FE9` |
| `SaveGameFileName_0154` | `SaveGameFileName` | `3EDB:0154` |
| `SchoolTrainingCooldown_D321` | `SchoolTrainingCooldown` | `3092:D321` |
| `SecondFloppyDriveAvailable_3FFE` | `SecondFloppyDriveAvailable` | `3092:3FFE` |
| `SecretPassageTilePattern_20BC` | `SecretPassageTilePattern` | `3EDB:20BC` |
| `SelectedCacheCodeByColour_D347` | `SelectedCacheCodeByColour` | `3092:D347` |
| `SelectedMechUpgradePackage_D31D` | `SelectedMechUpgradePackage` | `3092:D31D` |
| `SelectedPartyArmourType_D32B` | `SelectedPartyArmourType` | `3092:D32B` |
| `SelectedPartyMemberSlot_D31A` | `SelectedPartyMemberSlot` | `3092:D31A` |
| `SelectedSpecialistTrainingMask_D31B` | `SelectedSpecialistTrainingMask` | `3092:D31B` |
| `ShowOverheadObjectiveDirection_D33B` | `ShowOverheadObjectiveDirection` | `3092:D33B` |
| `SkillLevelName_0194` | `SkillLevelName` | `3EDB:0194` |
| `SoundCommandOrRepeatCount_4612` | `SoundCommandOrRepeatCount` | `3092:4612` |
| `SpeakerDelayMask_0254` | `SpeakerDelayMask` | `246C:0254` |
| `SpeakerDelayMinimum_0256` | `SpeakerDelayMinimum` | `246C:0256` |
| `SpeakerDelayState_0252` | `SpeakerDelayState` | `246C:0252` |
| `SpecialistTrainingPurchaseResult_D31A` | `SpecialistTrainingPurchaseResult` | `3092:D31A` |
| `SpectatorAnimationStream_4DD8` | `SpectatorAnimationStream` | `3EDB:4DD8` |
| `SpriteColumns_0268` | `SpriteColumns` | `246C:0268` |
| `SpriteRows_026C` | `SpriteRows` | `246C:026C` |
| `SpriteSourceStride_026A` | `SpriteSourceStride` | `246C:026A` |
| `StarportMapPatchOverrides_2B8E` | `StarportMapPatchOverrides` | `3EDB:2B8E` |
| `StarportSavedMapBytes_5804` | `StarportSavedMapBytes` | `3EDB:5804` |
| `StoredPartyMechNameInitial_D452` | `StoredPartyMechNameInitial` | `3092:D452` |
| `StridedCopySourceSegment_025A` | `StridedCopySourceSegment` | `246C:025A` |
| `SweepCentrePitDivisor_0000` | `SweepCentrePitDivisor` | `3092:0000` |
| `SweepCurrentDelayMultiplier_3246` | `SweepCurrentDelayMultiplier` | `3092:3246` |
| `SweepCurrentPitDivisor_3FF6` | `SweepCurrentPitDivisor` | `3092:3FF6` |
| `SweepDivisorStep_0062` | `SweepDivisorStep` | `3092:0062` |
| `SweepHalfSpan_39F4` | `SweepHalfSpan` | `3092:39F4` |
| `SweepRepeatCount_4034` | `SweepRepeatCount` | `3092:4034` |
| `SweepToneDelayMultiplier_4000` | `SweepToneDelayMultiplier` | `3092:4000` |
| `TrainingMissionAndQuizCooldown_D320` | `TrainingMissionAndQuizCooldown` | `3092:D320` |
| `TrainingRubbleTargetX_1632` | `TrainingRubbleTargetX` | `3EDB:1632` |
| `TrainingRubbleTargetY_163A` | `TrainingRubbleTargetY` | `3EDB:163A` |
| `UnrepairableMechComponentText_1D12` | `UnrepairableMechComponentText` | `3EDB:1D12` |
| `VerticalOffset_0202` | `VerticalOffset` | `305B:0202` |
| `VideoStatusInactiveLevel_32AC` | `VideoStatusInactiveLevel` | `3092:32AC` |
| `WeaponProficiencyUseCount_D35C` | `WeaponProficiencyUseCount` | `3092:D35C` |
| `WeaponShopCategoryOffset_D318` | `WeaponShopCategoryOffset` | `3092:D318` |
