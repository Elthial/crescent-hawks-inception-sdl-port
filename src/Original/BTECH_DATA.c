#include "game.h"
uint16_t SoundEffectsEnabled=TRUE; /* Original3EDB:015C, initialized WORD1. */

/* Original EXE-owned FAR text table3EDB:3A2E; no external asset content. */
uint8_t *KuritaMissionTextMessages[5]={
    (uint8_t *)"They're trying to actually kill you!  This is no training mission!",
    (uint8_t *)"Those are Kurita 'Mechs!  You can see the Dragon symbol on them!",
    (uint8_t *)"You look around and see the citadel has been destroyed!",
    (uint8_t *)"Kuritans must have raided Pacifica and destroyed your army!",
    (uint8_t *)"They broke down the southeast wall!"
};
uint8_t *TinylandTileset; /*3092:4588 native FAR pointer*/
uint16_t StockHoldingCount,CpuTimingCalibration; /*3092:E482/3FF4*/
uint16_t ArenaCombatMapPatchVariant; /*246C:0010*/
uint8_t *StockMarketNames[StockCompanyCount]={(uint8_t *)"DefHes\r",(uint8_t *)"NasDiv\r",(uint8_t *)"BakPhar\r"}; /*3EDB:4E7E FAR table*/
uint8_t *SchoolSkillDescriptions[5]={(uint8_t *)"completely unskilled",(uint8_t *)"an amateur",
    (uint8_t *)"competent",(uint8_t *)"a whiz",(uint8_t *)"a grand master"}; /*3EDB:4EA2*/
uint8_t *SchoolWeaponSkillText[3]={(uint8_t *)" in the use of bows and blades.  ",
    (uint8_t *)" when it comes to pistols and handguns.  ",(uint8_t *)" with a rifle or submachinegun.  "}; /*3EDB:4EB6*/
uint32_t ArmourPurchaseCost[ArmourTypeCount-1]={50,150,200,10000,1000}; /*3EDB:4F2A*/
uint32_t InfantryWeaponPurchaseCost[ShopWeaponPriceCount]={2,10,100,10,20,35,40,80,250,1500}; /*3EDB:4F44*/
uint8_t ArmourRepairCostPerPoint[ArmourTypeCount-1]={1,2,5,20,20}; /*3EDB:4F3E*/
int16_t MedicalServiceFee[MedicalServiceFeeCount]={0,50,100,150,200,400,600,750}; /*3EDB:4F6E*/
uint8_t EndingEgaPalette[EgaPaletteRegisterCount]={0,0,2,3,4,5,6,7,8,1,10,11,12,9,14,15}; /*3058:0000*/
uint8_t SaveGameFileName[6]="Game0"; /*EXE-owned3EDB:0154*/
uint16_t GraphicsAdapter; /*3EDB:4FBA, original initial WORD0*/
int8_t MechHeatLevel[MechRecordCount]; /*3092:006E, initialized zero.*/

uint16_t ExplorationStepsPerInput=1; /*3EDB:015A, initial WORD*/
uint16_t MessageBoxOpen; /*3092:D55C*/
uint8_t NpcUpdatePhase=255; /*3EDB:57FE, exact expanded EXE initial BYTE*/
uint8_t FogOfWarColumnBitMask[CompassDirectionCount]={224,224,112,56,28,14,7,7}; /*3EDB:029A*/
uint16_t StockChangeRollMask[StockCompanyCount]={255,7,1}; /*3EDB:02A2*/
uint16_t StockChangeFactor[StockCompanyCount]={96,90,48}; /*3EDB:02A8*/
uint16_t MovementCommandByCompass[CompassDirectionCount]={
    Command_MoveNorth,Command_MoveNorthEast,Command_MoveEast,Command_MoveSouthEast,
    Command_MoveSouth,Command_MoveSouthWest,Command_MoveWest,Command_MoveNorthWest}; /*3EDB:0160*/
static uint8_t compassNorth[]="\rNorth",compassNorthEast[]="\rNortheast";
static uint8_t compassEast[]="\rEast",compassSouthEast[]="\rSoutheast";
static uint8_t compassSouth[]="\rSouth",compassSouthWest[]="\rSouthwest";
static uint8_t compassWest[]="\rWest",compassNorthWest[]="\rNorthwest";
uint8_t *CompassDirectionText[CompassDirectionCount]={
    compassNorth,compassNorthEast,compassEast,compassSouthEast,
    compassSouth,compassSouthWest,compassWest,compassNorthWest}; /*3EDB:01AA*/

/* Sol: Original mutable DS3EDB tables, exact expanded EXE initial values.
 * Screen/map slots compact survivors; sprite/overlap arrays retain record IDs. */
uint16_t PartyInfantryScreenX[PartySize]={208,208,208,216,200,216,200,216}; /*02AE*/
uint16_t PartyInfantryScreenY[PartySize]={96,88,104,96,96,88,104,104}; /*02BE*/
uint16_t FriendlyMechScreenX[LanceSize]={200,200,224,176}; /*02CE*/
uint16_t FriendlyMechScreenY[LanceSize]={64,112,88,88}; /*02D6*/
uint16_t FriendlyMechMapCellOffset[LanceSize]={126,198,152,149}; /*02DE*/
uint16_t FriendlyMechOcclusionMask[LanceSize]={1,1,4,4}; /*02E6*/
uint16_t FriendlyMechOddCameraXOffset[LanceSize]={1,1,0,0}; /*02EE*/
uint16_t FriendlyMechOddCameraYOffset[LanceSize]={0,0,24,24}; /*02F6*/
uint16_t PartyInfantryMapCellOffset[PartySize]={150,126,150,151,150,127,150,151}; /*02FE*/
uint16_t PartyInfantryOddCameraXOffset[PartySize]={1,1,1,0,0,0,0,0}; /*030E*/
uint16_t PartyInfantryOddCameraYOffset[PartySize]={0,24,24,0,0,24,24,24}; /*031E*/
uint8_t MapNpcOcclusionMaskByParity[4]={9,3,12,6}; /*032E*/
uint16_t PartyInfantryOcclusionMask[PartySize][4]={
    {3,9,6,12},{6,12,3,9},{6,12,3,9},{9,3,12,6},
    {9,3,12,6},{12,6,9,3},{12,6,9,3},{12,6,9,3}}; /*0332*/
int8_t PartyInfantryFormationDeltaX[PartySize]={0,0,0,1,-1,1,-1,1}; /*3A16*/
int8_t PartyInfantryFormationDeltaY[PartySize]={0,-1,1,0,0,-1,1,1}; /*3A1E*/
int8_t FriendlyMechFormationDeltaX[LanceSize]={0,0,3,-3}; /*3A26*/
int8_t FriendlyMechFormationDeltaY[LanceSize]={-2,4,1,1}; /*3A2A*/
uint16_t OnFootPartyMemberCount; /*3092:006A*/
uint8_t TerrainOverlapRows[AllCombatantCount]; /*3092:32AE*/
/* Sol: EXE-owned tables resolved to normal pointers. 3EDB:01CA,25E2,25F2,2602.
 * Local expanded EXE SHA256 F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE.
 * No external copyrighted assets are embedded. */
static uint8_t jasonName[] = "Jason";
static uint8_t rexName[] = "Rex";
static uint8_t edwardName[] = "Edward";
static uint8_t russName[] = "Russ";
static uint8_t rickName[] = "Rick";
static uint8_t zekeName[] = "Zeke";
static uint8_t possumName[] = "Possum";
static uint8_t marcoName[] = "Marco";
static uint8_t rustyName[] = "Rusty";
static uint8_t hunterName[] = "Hunter";
static uint8_t hawkName[] = "Hawk";
static uint8_t firstAidText[] = "First Aid";
static uint8_t advancedFirstAidText[] = "Adv. First Aid";
static uint8_t fieldSurgeryText[] = "Field Surgery";
static uint8_t hospitalSurgeryText[] = "Hospital Surgery";
static uint8_t tornClothText[] = "torn cloth strips";
static uint8_t medkitText[] = "MedKit";
static uint8_t fieldSurgeryKitText[] = "Field Surgery Kit";
static uint8_t hospitalFacilitiesText[] = "Hospital Facilities";
uint8_t HealingDice[8][4] = {
    {0x01,0x01,0x01,0x01},
    {0x02,0x02,0x02,0x02},
    {0x02,0x02,0x03,0x04},
    {0x03,0x04,0x06,0xa1},
    {0x06,0xa1,0xc1,0x82},
    {0xa1,0xc1,0x82,0xa2},
    {0xc1,0x82,0xa2,0xc2},
    {0x82,0xa2,0xc2,0xa3},
};

/* Native game storage. Original startup, not an invented reset, sets live state. */
OriginalSavedStateStorage OriginalSavedState;
uint8_t ArmourTypeDurability[ArmourTypeCount]={0,25,40,30,50,50}; /*Original DS4DDB*/
/* Original EXE-owned3EDB:4E8A shared FAR pointer table; storage only. */
uint8_t *ArmourTextDescription[ArmourTypeCount] = {
    (uint8_t *)"None", (uint8_t *)"Flak Vest", (uint8_t *)"Flak Suit",
    (uint8_t *)"Lt Env Suit", (uint8_t *)"Hv Env Suit", (uint8_t *)"Ablative"
};
uint8_t *CombatSpritePointers[CombatSpriteCount];
uint8_t CacheSecurityCodeUsed[CacheSecurityCodeCount];
uint8_t ViewedHolodisk,CurrentMap;
uint16_t TransmittedCacheFound,DrawJailMissionParkedMechs,TraitorWarning;
uint16_t TextColour;
uint16_t TextRow; /* 3092:374E: original text cursor row WORD */
uint16_t TextColumn; /* 3092:3748 */
uint16_t TextPanelWidth; /* native EXE initial WORD0, set by panel setup */
uint16_t TextBackgroundColour; /* native EXE initial WORD0 */
uint16_t TextPanelLeft,TextPanelTop,TextPanelHeight; /* original panel geometry */
uint16_t EgaPrimitiveColumn,EgaPrimitiveColour,EgaPrimitiveGroupCount;
uint16_t EgaPrimitiveTop,EgaPrimitiveBottom;
uint16_t EgaSpanAlignmentMask=7,EgaSpanEndAlignmentMask=0x01F8,EgaSpanGroupShift=3;
uint16_t SelectedMechId; /* 3092:0068 */
static uint8_t engineRepairText[] = "engine.";
static uint8_t gyroRepairText[] = "gyro.";
static uint8_t sensorsRepairText[] = "sensors.";
uint8_t *UnrepairableMechComponentText[3] = {
    engineRepairText, gyroRepairText, sensorsRepairText
};
uint16_t CombatComputerControl;
uint16_t CharacterMovementPointsRemaining;
uint16_t MechDestroyedFlag, CombatNotificationLatch;
/* Original EXE316D/3175/317D/3185 and318A/3192. */
uint8_t CriticalLowActuatorHitMask[4] = {8,4,2,1};
uint8_t CriticalLowActuatorClearMask[4] = {0xF7,0xFB,0xFD,0xFE};
uint8_t CriticalHighActuatorHitMask[4] = {0x80,0x40,0x20,0x10};
uint8_t CriticalHighActuatorClearMask[4] = {0x7F,0xBF,0xDF,0xEF};
uint8_t CriticalSectionStart[MechStructureLocationCount] = {0x33,0x3A,0x4F,0x55,0x53,0x41,0x48,0x51};
uint8_t CriticalSectionCount[MechStructureLocationCount] = {7,7,2,1,2,7,7,2};
uint16_t CombatMessageVerbosity = 2, CombatDisplayGraphics = TRUE; /* original3EDB:2E38/2E3A */
uint8_t ArenaMechRecordBackup[MechRecordSize];
uint8_t SavedArenaMechPilotId[LanceSize], SavedArenaMechRiderId[LanceSize];
uint8_t SavedPartyNameId[PartySize];
uint16_t ArenaRentalMechMode;
uint8_t BTStatsOrBldMemory[SharedBtStatsBldBufferBytes];
_Static_assert(SharedBtStatsBldBufferBytes >= BldFixedDecodeSpan,
    "Shared asset storage must contain the complete native BLD transform span");
_Static_assert(SharedBtStatsBldBufferBytes >= AttractDemoBufferOffset+AttractDemoFileBytes,
    "Shared asset storage must contain the pre-biased attract-demo view");
uint16_t BldFileIndex, BTStatsAssetLoaded;
uint16_t RequestedGameDiskNumber, HasHardDisk, SecondFloppyDriveAvailable, CurrentMenuLayoutIndex;
uint8_t DiskDriveNameText[9] = "drive A:";
GraphicsWorkingAddress GraphicsSourceAddress,GraphicsDestinationAddress;
uint16_t FramebufferBoxColumn,FramebufferBoxRow,FramebufferBoxWidth,FramebufferBoxHeight;
uint8_t EgaFontForeground,EgaFontBackground;
uint16_t TextScreenColumnCount=40,TextAutoWrapLineCount;
uint8_t DynamicString[DynamicTextScratchBytes];
uint16_t CombatantCasualtyFlags[AllCombatantCount];
uint8_t DestroyedMechNameInitial[MechRecordCount];
uint8_t CombatantSpriteFamilyOffset[AllCombatantCount];
uint16_t CombatantPackedX[AllCombatantCount], CombatantPackedY[AllCombatantCount];
uint16_t CombatantActive[AllCombatantCount];
uint8_t CombatantSpriteFrame[AllCombatantCount];
uint16_t MainCharactersAlive=1, CombatSpeedSetting=2;
uint16_t CompassCompareX0, CompassCompareY0, CompassCompareX1, CompassCompareY1;
uint8_t CompassDirectionLookup[CompassFlagCombinationCount]={
    255,6,2,255,4,5,3,255,0,7,1,255,255,255,255,255
};
uint8_t MapCharacterNames[MapCharacterCount][MapNameBytes];
uint8_t MapBuildingNames[MapBuildingCount][MapNameBytes];
uint16_t MapInteractablePositionX[MapBuildingCount],MapInteractablePositionY[MapBuildingCount];
uint16_t MapPartyPositionX[MapBuildingCount],MapPartyPositionY[MapBuildingCount];
uint8_t AlternativeBldByBuildingId[MapBuildingCount],RoamingNpcWaypointLink[MapCharacterCount];
uint8_t CombatantAnimationDirection[AllCombatantCount];
uint16_t TilesetId,OverheadMapActive,GraphicsCompatibilityFlag;
uint8_t DefaultEgaPalette[EgaPaletteRegisterCount]={0,0,2,3,4,9,6,7,8,1,10,11,12,13,14,15};
uint8_t GraphicsSceneWorkspace[GraphicsWorkspaceBytes];
uint8_t StarportSavedMapBytes[StarportPatchBytes];
/* EXE-owned3EDB:2B8E. Zero means keep the underlying tile, not tile zero. */
uint8_t StarportMapPatchOverrides[StarportPatchBytes]={
    0xC5,0xCC,0xAF,0xB0,0x58,0x42,0,0,0xC6,0xCC,0xD9,0xCE,0xB0,0x42,0,0,
    0x50,0x42,0x53,0x40,0x30,0x54,0,0,0x40,0x54,0x4F,0x50,0x40,0x42,0,0,
    0xBE,0xBF,0xAC,0xD7,0xD7,0x53
};
uint8_t MapConstructionScratch[7], MapRenderSelector, MapRenderTerrain;
uint8_t MapCopyHalfHeight,MapCopyLeftHalf,MapCopyBottomHalf,MapFullBandsRemaining;
uint16_t MapTileDestinationWidth,MapFullBandGap,MapHalfBandGap;
uint8_t MapSubdivisionStack[MapSubdivisionStackBytes];
uint8_t LocalMapBlockX, LocalMapBlockY;
uint16_t CachedMapPositionX, CachedMapPositionY;
uint16_t BlockingTileCodeThreshold=0x55;
int16_t CombatPathStepX[CompassDirectionCount]={0,1,1,1,0,-1,-1,-1};
int16_t CombatPathStepY[CompassDirectionCount]={-1,-1,0,1,1,1,0,-1};
/* EXE328A..32D9: one-cell heading steps, X local-to-region carry80h,
 * Y carry0F80h (=3968), and one24-column cache row. Negative entries are
 * native signed WORD corrections, not generic world-coordinate deltas. */
int16_t CombatPathBoundaryX[CompassDirectionCount]={0,128,128,128,0,-128,-128,-128};
int16_t CombatPathBoundaryY[CompassDirectionCount]={-3968,-3968,0,3968,3968,3968,0,-3968};
int16_t CombatPathCacheRowDelta[CompassDirectionCount]={-24,-24,0,24,24,24,0,-24};
int16_t MechUpgradeCost;
/* Original 3EDB:1D8C and1E24; no external game assets. */
uint16_t MechUpgradeCostByPackage[MechUpgrade_PackageCount] = {
    11000,10400,12200,13800,15800,14000,13200,17600
};
uint8_t CommandoStageOneArmour[MechArmourLocationCount] = {8,9,12,9,12,8,9,12,3,4,3};
/* Original component1A..20 ammunition prices: LRM5/10/15/20, SRM2/4/6. */
uint8_t MissileAmmoPriceByComponent[7] = {10,15,25,30,30,60,80};
/* Original EXE bytes at 3EDB:4FC0..4FC2. Startup may replace this seed. */
uint8_t RandomByteLow = 4, RandomByteMiddle = 3, RandomByteHigh = 2;
uint8_t RandomStateUpperByte = 1; /* original3EDB:4FC3 */
uint16_t DisableInput;
uint16_t AttractModeReplayIndex, AttractModeRecordingActive, ExitMainLoop;
uint16_t EnteredBuildingId;
uint16_t SavedBuildingMapPositionX[SavedBuildingPositionCount];
uint16_t SavedBuildingMapPositionY[SavedBuildingPositionCount];
uint8_t *CharacterNames[CharacterNameCount] = {
    jasonName,rexName,edwardName,russName,rickName,zekeName,
    possumName,marcoName,rustyName,hunterName,hawkName
};
uint8_t *MedicalSkillText[4] = { firstAidText,advancedFirstAidText,fieldSurgeryText,hospitalSurgeryText };
uint8_t *MedicalEquipmentText[4] = { tornClothText,medkitText,fieldSurgeryKitText,hospitalFacilitiesText };
