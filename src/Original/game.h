#ifndef CHI_GAME_H
#define CHI_GAME_H
#include <stddef.h>
#include <stdint.h>
#include "mech.h"

/* Sol: reconstructed storage, not the original authors' unrecoverable names.
 * No host pointers in on-disk/native records. Original BYTE signed loads are
 * explicit at use sites, rather than changing all record members to signed. */
enum { PartySize=8, LanceSize=4, MechRecordCount=8, CharacterRecordCount=16,
       CharacterNameCount=11, CharacterHealthPerBodyPoint=10,
       Character_Dead=255, Character_Jason=0, FALSE=0, TRUE=1,
       SkillLevel_Unskilled=0, SkillLevel_Amateur=1, SkillLevel_Average=2,
       SkillLevel_Good=3, SkillLevel_Excellent=4,
       MedEquip_TornCloth=1, MedEquip_Medkit=2, MedEquip_FieldSurgeryKit=3,
       MedEquip_HospitalFacilities=4, MedicalSkill_HospitalSurgeryTier=5,
       MedicalService_UsePartyMedicAndEquipment=0, MedicalService_LastMedkitTier=2,
       HealingRecoveryWorldTicks=63, HealingRoll_DiceCountLowNibbleMask=15,
       HealingRoll_MultiplierHighNibbleShift=4, EGA_Black=0, EGA_BrightWhite=15 };
typedef struct Character {
    uint8_t name, body, dexterity, charisma;
    uint8_t skillBowsAndBlade, skillPistol, skillRifle, skillGunnery;
    uint8_t skillPiloting, skillTech, skillMedical;
    uint8_t weapon, mechAssignment, armourType, armourValue, health, trainingFlags;
} Character; /* original 3092:C614 + record*17 */
_Static_assert(sizeof(Character)==17, "Native character record must remain17 bytes");
_Static_assert(offsetof(Character,health)==15, "Native health BYTE offset");
_Static_assert(offsetof(Character,skillBowsAndBlade)==4 && offsetof(Character,skillMedical)==10,
    "Native seven-skill BYTE span must remain offsets4..10");
/* Sol: reconstructed declarations for original storage, not new routines. */
enum { MapFogOfWarRowBytes=16, MapFogOfWarRows=128,
    MapFogOfWarBytes=MapFogOfWarRows*MapFogOfWarRowBytes };
/* Native3092:D30C..D36F: scripts address bytes, game methods address named
 * overlapping fields. All members are BYTEs, no alignment/host pointer issue. */
enum { PersistentBldStateBytes=100 };
typedef union PersistentGameState {
    uint8_t bytes[PersistentBldStateBytes];
    struct {
        uint8_t beforeUpgrade[17],selectedMechUpgradePackage,mechModificationWorkflowEnabled;
        uint8_t beforeInjury[6],partyHasInjuredMember,beforeTraitor[10];
        uint8_t traitorBattleProbability,traitorCharacterId,traitorEventOccurred,traitorInParty;
        uint8_t requiredMainCharacterDead,partyHealthRecoveryTimer,beforeAllowance[9];
        uint8_t allowanceLow,allowanceHigh,unknownAfterAllowance,entranceBldState;
        uint8_t starportCountdown,countdownLow,countdownHigh,insideStarLeagueCache;
        uint8_t remainingState[41];
    } fields;
} PersistentGameState;
_Static_assert(sizeof(PersistentGameState)==PersistentBldStateBytes,"Native BLD reset span");
_Static_assert(offsetof(PersistentGameState,fields.partyHealthRecoveryTimer)==41 &&
    offsetof(PersistentGameState,fields.insideStarLeagueCache)==58,"Native persistent aliases");
/* Original adjacent byte-addressed arrays. Fog-row overflow/underflow must
 * alias the real neighbours, not undefined host pointer arithmetic. */
typedef union WorldMapStateStorage {
    struct {
        Mech mechs[MechRecordCount];
        uint8_t fog[MapFogOfWarBytes];
        PersistentGameState persistent;
    } fields;
    uint8_t bytes[MechRecordCount*sizeof(Mech)+MapFogOfWarBytes+PersistentBldStateBytes];
} WorldMapStateStorage;
_Static_assert(offsetof(WorldMapStateStorage,fields.fog)==1000 &&
    offsetof(WorldMapStateStorage,fields.persistent)==3048 &&
    sizeof(WorldMapStateStorage)==3148,"Native C724..D36F adjacency");
#define Mechs WorldMapState.fields.mechs
#define MapFogOfWar WorldMapState.fields.fog
#define PersistentState WorldMapState.fields.persistent
#define KuritaDestroyedCitadel PersistentState.bytes[4] /*D310*/
#define TrainingMissionAndQuizCooldown PersistentState.bytes[20] /*D320*/
#define SchoolTrainingCooldown PersistentState.bytes[21] /*D321*/
#define UnclassifiedWorldCountdownA PersistentState.bytes[29] /*D329*/
#define UnclassifiedWorldCountdownB PersistentState.bytes[22] /*D322*/
#define ComstarFinanceCountdown PersistentState.bytes[23] /*D323*/
#define HasMapper PersistentState.bytes[49] /*D33D*/
#define PartyHealthRecoveryTimer PersistentState.fields.partyHealthRecoveryTimer
#define PartyHasInjuredMember PersistentState.fields.partyHasInjuredMember
#define MechModificationWorkflowEnabled PersistentState.fields.mechModificationWorkflowEnabled
#define SelectedMechUpgradePackage PersistentState.fields.selectedMechUpgradePackage
#define TraitorBattleProbability PersistentState.fields.traitorBattleProbability
#define TraitorCharacterId PersistentState.fields.traitorCharacterId
#define TraitorEventOccurred PersistentState.fields.traitorEventOccurred
#define TraitorInParty PersistentState.fields.traitorInParty
#define InsideStarLeagueCache PersistentState.fields.insideStarLeagueCache
#define StarportCountdown PersistentState.fields.starportCountdown
extern uint16_t TextColour;
extern uint16_t TextRow;
extern uint16_t TextColumn, SelectedMechId;
extern uint16_t TextPanelWidth; /* 3092:3990: interior width in text cells */
extern uint16_t TextBackgroundColour; /* 3092:377E */
extern uint16_t TextPanelLeft,TextPanelTop,TextPanelHeight; /*3092:39A0/39A4/393A*/
enum { MenuPanelLayoutCount=9 };
typedef struct MenuPanelLayout {
    uint16_t left,top,width,height,foreground,background,column,row;
} MenuPanelLayout;
_Static_assert(sizeof(MenuPanelLayout)==16,"Native panel layout record stride");
_Static_assert(offsetof(MenuPanelLayout,foreground)==8 && offsetof(MenuPanelLayout,row)==14,
    "Native mutable panel context fields");
extern MenuPanelLayout MenuPanelLayouts[MenuPanelLayoutCount]; /*305B:0000..008F*/
extern uint16_t MenuPanelContextInitialized; /*3EDB:4FA2*/
enum { BorderStyleCount=5, BorderDescriptorWords=12, BorderCornerCount=4,
    BorderPatternEnd=255, BorderTileBytes=32 };
extern uint16_t BorderDescriptors[3][BorderDescriptorWords]; /*305B:0320/0338/0350*/
extern uint16_t *BorderStyles[BorderStyleCount]; /*3EDB:4FA4 FAR table*/
extern uint8_t *BorderTileset; /*3092:4066 native FAR pointer*/
void DrawCall_SingleTile(uint8_t *tile,uint16_t column,uint16_t row); /*207F:275C*/
uint8_t *Allocate_Far_Buffer(uint32_t bytes); /*1F3D:05BC; positive high-WORD stack return unsupported*/
uint8_t *Runtime_Allocate_Word_Buffer(uint16_t byteCount); /*207F:3835, SDL heap redirect*/
void TileSet_Memory_Operation(uint8_t *display,uint8_t *destination,uint16_t column,uint16_t row); /*1E56:0A3B*/
uint8_t *Create_TileSet_Array(uint8_t *display,uint16_t column,uint16_t row,uint16_t count); /*1E56:0AE5*/
uint16_t DrawCall_Border(uint16_t *descriptor,uint16_t sequenceStart,
    uint16_t column,uint16_t row,uint16_t length,uint16_t horizontal);
void Draw_Horizontal_EGA_Line(uint16_t left,uint16_t top,uint16_t right,uint16_t bottom,uint16_t colour); /*1F3D:01FB*/
void Draw_Clipped_Axis_Aligned_EGA_Line(uint16_t left,uint16_t top,uint16_t right,uint16_t bottom,uint16_t colour); /*1F3D:031C*/
void Draw_EGA_Horizontal_Span(uint16_t row,uint16_t left,uint16_t right,uint16_t colour);
void EGA_Draw_Vertical_Pixel_Run(uint16_t column,uint16_t top,uint16_t bottom,uint16_t colour);
void EGA_Draw_Aligned_Span(uint16_t column,uint16_t row,uint16_t groupCount,uint16_t colour);
extern uint16_t EgaPrimitiveColumn,EgaPrimitiveColour,EgaPrimitiveGroupCount;
extern uint16_t EgaPrimitiveTop,EgaPrimitiveBottom; /*246C:0220/0224/0230/0234/0236*/
extern uint16_t EgaSpanAlignmentMask,EgaSpanEndAlignmentMask,EgaSpanGroupShift; /*3EDB:4FC8/4FD0/4FD8*/
void Display_Text_In_TextBox(uint8_t *text,uint16_t advanceLine); /* 1E56:07CB */
enum { MenuControlCount=41, MenuPreviousCommand=0xFFB8, MenuNextCommand=0xFFB0 };
typedef struct MenuControl {
    uint16_t unknown0,baseRow,highlightWidth,optionCount,selection;
    uint16_t unknownA,highlightColour,unknownE;
} MenuControl;
_Static_assert(sizeof(MenuControl)==16 && offsetof(MenuControl,selection)==8,
    "Native menu control stride and selected-option offset");
extern MenuControl MenuControls[MenuControlCount]; /*305B:0090*/
extern uint16_t MenuRendererFlag;
#define MenuFirstRow MenuControls[23].baseRow
#define MenuLastRow MenuControls[23].optionCount
#define MenuSelection MenuControls[23].selection
enum {
    AllCombatantCount=24, Enemy_All_CombatantId_Range_First=12,
    Enemy_Infantry_CombatantId_Range_First=16, Enemy_Mech_Record_First=4,
    Enemy_Mech_Record_Count=MechRecordCount-LanceSize,
    Character_OnFoot=8, MECH_Destroyed=255,
    MECH_Sprite_LOCUST=0, MECH_Sprite_COMMANDO=0x92,
    MechEngineDestroyedHitCount=3, MechGyroDestroyedHitCount=2,
    SalvageInspectionRetraceCount=120, SalvageLoop_Searching=0,
    SalvageLoop_Recovered=1, SalvageLoop_Exhausted=9,
    DynamicTextScratchBytes=82, /* native3092:0012 up to next WORD at0064 */
    SalvageSuccessNamePlaceholder=16, SalvageFailureNamePlaceholder=27
};
extern uint8_t DynamicString[DynamicTextScratchBytes];
extern uint16_t CombatantCasualtyFlags[AllCombatantCount]; /* 3092:393C */
extern uint8_t DestroyedMechNameInitial[MechRecordCount]; /* 3092:323E */
extern uint8_t CombatantSpriteFamilyOffset[AllCombatantCount]; /* 3092:D55E */
extern uint16_t CombatantPackedX[AllCombatantCount]; /* 3092:4004 */
extern uint16_t CombatantPackedY[AllCombatantCount]; /* 3092:4036 */
extern uint16_t CombatantActive[AllCombatantCount]; /* 3092:406A */
extern uint8_t CombatantSpriteFrame[AllCombatantCount]; /* 3092:409A */

/* Original exploration formation/display tables, EXE DS3EDB:02AE..0371. */
enum { MapCameraCentreCellX=26, MapCameraCentreCellY=12,
    MapViewportLastCellX=39, MapViewportLastCellY=24,
    MapProjectionMinimumX=-115, MapProjectionMaximumX=167,
    MapProjectionMinimumY=-3968, MapProjectionMaximumY=3992,
    PixelsPerMapCell=8, TerrainOcclusionSpecialTileFirst=0xF6,
    TerrainOcclusionCategoryMask=0xF0, TerrainOcclusionCategoryLimit=0x30,
    TerrainOcclusionAllQuadrantsMask=0x0F, TerrainOcclusionMechOddYMaskToggle=5,
    TerrainOcclusionTallCategory=0x20, InfantryTerrainOverlapRows=2,
    InfantryTallTerrainOverlapRows=4, MechTerrainOverlapRows=8,
    MechTallTerrainOverlapRows=16, JailParkedMechFirstX=0x0D13,
    JailParkedMechSpacing=4, JailParkedMechY=0x702C };
extern uint16_t PartyInfantryScreenX[PartySize],PartyInfantryScreenY[PartySize];
extern uint16_t FriendlyMechScreenX[LanceSize],FriendlyMechScreenY[LanceSize];
extern uint16_t FriendlyMechMapCellOffset[LanceSize],FriendlyMechOcclusionMask[LanceSize];
extern uint16_t FriendlyMechOddCameraXOffset[LanceSize],FriendlyMechOddCameraYOffset[LanceSize];
extern uint16_t PartyInfantryMapCellOffset[PartySize],PartyInfantryOcclusionMask[PartySize][4];
extern uint16_t PartyInfantryOddCameraXOffset[PartySize],PartyInfantryOddCameraYOffset[PartySize];
extern uint8_t MapNpcOcclusionMaskByParity[4];
extern int8_t PartyInfantryFormationDeltaX[PartySize],PartyInfantryFormationDeltaY[PartySize];
extern int8_t FriendlyMechFormationDeltaX[LanceSize],FriendlyMechFormationDeltaY[LanceSize];
extern uint16_t OnFootPartyMemberCount; /*3092:006A*/
extern uint8_t TerrainOverlapRows[AllCombatantCount]; /*3092:32AE*/
extern uint16_t MainCharactersAlive, CombatSpeedSetting; /* 3EDB:014A/015E */
enum { PackedPositionLocalCarryBit=0x80, PackedPositionLocalMask=0x7F,
    PackedPositionXRegionMask=0x0F00, PackedPositionYRegionMask=0xF000,
    PackedPositionWestNormalizeMask=0x0F7F, PackedPositionNorthNormalizeMask=0xF07F,
    PackedPositionSouthCarry=0x0F80, MechFootprintWidth=3,
    MechFootprintInfantryDistance=2, OpposingInfantrySideToggle=0x14,
    CombatSpeedKeyWaitSetting=5, CombatSpeedRetracesPerSetting=12,
    Sprite_Impact_Small=0x7E, Sound_SquishedByMech=0x12 };
void Offset_Packed_Position(int16_t deltaX, int16_t deltaY); /* 0800:191B */
void Move_Map_View_To_Packed_Position(uint16_t targetX,uint16_t targetY); /* 0800:17BB */
void Move_Map_View_By_Signed_Delta(int16_t deltaX,int16_t deltaY); /* 0800:1817 */
enum { Command_MoveNorth=0xFFB8, Command_MoveNorthEast=0xFFB7,
    Command_MoveNorthWest=0xFFB9, Command_MoveSouth=0xFFB0,
    Command_MoveSouthEast=0xFFAF, Command_MoveSouthWest=0xFFB1,
    Command_MoveEast=0xFFB3, Command_MoveWest=0xFFB5,
    WorldRegionCount=256, PendingMapSlot_Unused=255 };
extern uint8_t MapFileByWorldRegion[WorldRegionCount]; /* 2FE8:0030 */
void Character_Movement_On_Map(uint16_t movementCommand); /* 0800:218F */
uint16_t Map_Interactables_Building_Or_Items(int16_t deltaX,int16_t deltaY); /* 0800:1C12 */
void Map_NineGrid_Parent(void); /* 207F:1DA8 */
enum { CompassDirectionCount=8, CompassDirectionMask=7,
    Compass_West=1, Compass_East=2, Compass_South=4, Compass_North=8,
    CompassFlagCombinationCount=16, MapCacheWidth=24, MapCacheHeight=24,
    MapCacheTileCount=MapCacheWidth*MapCacheHeight, MapCacheOriginMargin=2,
    MapCacheLocalBlockMask=7, CombatPathCentreOffset=6*MapCacheWidth+6,
    CompassClockwiseTurnLastDifference=4, ArenaRentalPathTargetId=13 };
extern uint16_t CompassCompareX0, CompassCompareY0, CompassCompareX1, CompassCompareY1;
extern uint8_t CompassDirectionLookup[CompassFlagCombinationCount]; /* 246C:0240 */
/* Cached origin WORDs have contiguous storage below (246C:09ED..09F2). */
extern uint16_t BlockingTileCodeThreshold; /* 3EDB:0150 */
extern int16_t CombatPathStepX[CompassDirectionCount], CombatPathStepY[CompassDirectionCount];
extern int16_t CombatPathBoundaryX[CompassDirectionCount], CombatPathBoundaryY[CompassDirectionCount];
extern int16_t CombatPathCacheRowDelta[CompassDirectionCount]; /* 3EDB:328A..32D9 */
int16_t Get_Target_Compass_Direction(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1); /* 207F:0971 */
void Update_Cached_Map_Origin(void); /* 207F:1DF8 */
void PosXY_OffsetGrid(uint16_t packedX,uint16_t packedY); /* 207F:1314 */
enum { MapBlockWidth=8, MapBlockHeight=8, MapBlockTileCount=64,
    MapNeighbourhoodWidth=3, MapNeighbourhoodCount=9,
    MapDescriptorCacheBytes=MapNeighbourhoodCount*MapBlockTileCount,
    MapTemplateDataBytes=0x182E, /* native0C1D..244A: templates/overview aliases */
    MapSpecialBlockOffset=0x1480, MapReflectedBlockOffset=0x14C0,
    MapHorizontalLookupOffset=0x1500, MapVerticalLookupOffset=0x1520,
    MapBlockColumnEnd=8, MapBlockBeforeFirst=255,
    MapTerrainSpecialDescriptor=0x10, MapTerrainSpecialTileBase=0x70,
    MapTerrainCategoryStride=0x10, MapTerrainLastCategoryBase=0x30,
    MapTemplateFixedTileFirst=0x40, MapTemplateVariantMask=0x0F,
    MapReflectionVertical=1, MapReflectionHorizontal=2 };
#define MapReflectedBlock (MapTemplateData+MapReflectedBlockOffset)
#define MapHorizontalReflection (MapTemplateData+MapHorizontalLookupOffset)
#define MapVerticalReflection (MapTemplateData+MapVerticalLookupOffset)
extern uint8_t MapConstructionScratch[7]; /* 246C:0272..0278, reused by rendering */
enum { EgaFramebufferRowBytes=40, MapTileHeight=16, MapTileHalfHeight=8,
    MapTileByteWidth=2, MapViewportLeftByte=13, MapViewportByteWidth=27,
    MapViewportFullTileColumns=13, MapViewportFullBands=12,
    MapTilesetSegment=0xA400, MapViewportSegment=0xAC00 };
extern uint8_t MapCopyHalfHeight,MapCopyLeftHalf,MapCopyBottomHalf,MapFullBandsRemaining; /* A44F/50/51/58 */
extern uint16_t MapTileDestinationWidth,MapFullBandGap,MapHalfBandGap; /* A452/54/56 */
void Copy_Data_To_GraphicsMemory(void); /*207F:18EF*/
void Graphic_Memory_To_Destination(uint8_t tileId,uint16_t destination,uint16_t segment);
void Graphic_Memory_Lower_Half_To_Destination(uint8_t tileId,uint16_t destination,uint16_t segment);
void EGA_Copy_Tile_Full_Width_To_Destination(uint8_t tileId,uint16_t destination,uint16_t segment);
void EGA_Copy_Tile_Two_Columns_To_Destination(uint16_t graphicsOffset,uint16_t destination,uint16_t segment);
void EGA_MemoryCopy_Byte(uint16_t graphicsOffset,uint16_t destination,uint16_t segment);
void EGA_Copy_A400_Memory_Word_Or_Byte_To_Destination(uint16_t graphicsOffset,uint16_t destination,uint16_t segment);
#define MapBlockPhase MapConstructionScratch[0]
#define MapBlockReflection MapConstructionScratch[1]
extern uint8_t MapRenderSelector, MapRenderTerrain;
enum { MapLatticeWidth=9, MapLatticeBytes=81, MapSubdivisionStackBytes=80,
    MapConstructionSeedCount=256, MapUnfilledVertex=255,
    MapConstructionTerrainMask=0xF0, MapCornerTopRight=8,
    MapCornerBottomLeft=72, MapCornerBottomRight=80 };
extern uint8_t MapSubdivisionStack[MapSubdivisionStackBytes]; /* 246C:0279 */
/* MapConstructionSeedIndex:246C:09F9, LOW BYTE increment only; storage below. */
typedef struct MapStaticStorage {
    uint8_t constructionSeeds[MapConstructionSeedCount];
    uint8_t seedToVertexGap[16];
    uint8_t worldVertices[274]; /* native0B0B..0C1C view, not allocation proof */
    uint8_t templates[MapTemplateDataBytes];
} MapStaticStorage;
/* EXE-owned246C:09FB..244A is embedded unchanged in MapRuntime below. */
#define MapConstructionSeeds MapStaticData.constructionSeeds
#define MapWorldVertices MapStaticData.worldVertices
#define MapTemplateData MapStaticData.templates
enum { MapFileTileTemplateOffset=16*MapBlockTileCount }; /* 101D-0C1D=400h */
#define MapFileTiles (MapTemplateData+MapFileTileTemplateOffset)
enum { MapFileMaximumTileBytes=64*64, MapCharacterCount=8, MapBuildingCount=16,
    MapNameBytes=16, MapFileFirstBlockDescriptor=0x90,
    Map_Citadel=1, Map_Starport=2, Map_LastVillage=10,
    Map_DestroyedCitadel=11, Map_StarLeagueCache=14,
    Tileset_BattleTech=0, Tileset_Destruct=1,
    Tileset_None=65535, EgaSceneStagingSegment=0xA800, EgaScreenSegment=0xA000,
    MapOrdinaryBlockingThreshold=0x55, MapCacheBlockingThreshold=0x21,
    RoamingNpcInitialFrame=0x10, AnimationDirection_Refresh=255,
    EgaTilesetBufferSegment=0xA400, GraphicsWorkspaceBytes=32768 };
typedef struct RoamingMapNpc {
    uint16_t currentPositionX,currentPositionY,destinationX,destinationY;
    uint8_t waypointPair,movementDelay;
    uint8_t remainingNativeRecord[16]; /* Not yet interpreted; preserve stride. */
} RoamingMapNpc;
_Static_assert(sizeof(RoamingMapNpc)==26,"Native roaming NPC stride");
_Static_assert(offsetof(RoamingMapNpc,movementDelay)==9,"Native NPC delay offset");
/* The final stride's trailing bytes are NOT reserved independent storage.
 * Known native globals occupy D450 onward inside this address view. */
#define PurchasedMedkit RoamingMapNpcs[MapCharacterCount-1].remainingNativeRecord[0]
#define PurchasedFieldSurgeryKit RoamingMapNpcs[MapCharacterCount-1].remainingNativeRecord[1]
#define StoredPartyMechNameInitial (*(uint8_t (*)[LanceSize])(&RoamingMapNpcs[MapCharacterCount-1].remainingNativeRecord[2]))
#define NextRecruitNameId RoamingMapNpcs[MapCharacterCount-1].remainingNativeRecord[6]
extern uint8_t MapCharacterNames[MapCharacterCount][MapNameBytes]; /* 246C:A461 */
extern uint8_t MapBuildingNames[MapBuildingCount][MapNameBytes]; /* 246C:A561 */
extern uint16_t MapInteractablePositionX[MapBuildingCount],MapInteractablePositionY[MapBuildingCount]; /* 4564/4596 */
extern uint16_t MapPartyPositionX[MapBuildingCount],MapPartyPositionY[MapBuildingCount]; /* 39B4/39D4 */
extern uint8_t AlternativeBldByBuildingId[MapBuildingCount],RoamingNpcWaypointLink[MapCharacterCount]; /* 4602/3768 */
extern uint8_t CombatantAnimationDirection[AllCombatantCount]; /* 3092:396C */
extern uint16_t TilesetId,OverheadMapActive,GraphicsCompatibilityFlag; /* 3988/014C/4FBC */
/* GraphicsFileWorkspace:246C:244B, storage below. */
extern uint8_t GraphicsSceneWorkspace[GraphicsWorkspaceBytes]; /* 3092:4614 */
void DOS_Load_Map_Files(uint16_t mapGridSlot,uint16_t mapNumber); /* 0800:2DA8 */
void Load_And_Draw_BTTLTECH_ICN(void); /* 0800:4621 */
void Load_And_Draw_BTTITLE_CMP(void); /*0800:46A7*/
void Draw_GraphicsFile_In_Memory(uint8_t *fileBuffer,uint16_t column,uint16_t row,
    uint16_t width,uint16_t height); /*1F3D:0086*/
void DrawCall_Image_To_VGA_Memory(uint8_t *image,uint16_t egaSegment); /* 207F:0260, hardware redirect */
void DrawCall_Read_EGAMemory(uint8_t *destination,uint16_t byteColumn,uint16_t tileRow); /* 207F:0313 */
enum { EgaPaletteRegisterCount=16, EgaPaletteFirstHighColour=8,
    EgaPaletteHighColourAdjustment=8 };
extern uint8_t DefaultEgaPalette[EgaPaletteRegisterCount]; /* EXE2FE8:0000 */
enum { BTStatsPaletteCycleCount=4, BTStatsPaletteRefreshRedraws=16,
    BTStatsCyclingPaletteEntry=4, BTStatsAutoDismissRedraws=600 };
enum { MechEngineHitCapacity=3, MechGyroHitCapacity=2, MechSensorHitCapacity=2,
    BTStatsDisplayedHeatSinkCapacity=10,
    BTStatsCriticalLeftTorsoStart=0x3A, BTStatsCriticalRightArmStart=0x41,
    BTStatsCriticalRightTorsoStart=0x48, BTStatsCriticalLeftLegStart=0x4F,
    BTStatsCriticalRightLegStart=0x51, BTStatsCriticalCenterTorsoStart=0x53,
    BTStatsCriticalHeadStart=0x55 };
/* Above critical boundaries describe the ORIGINAL STATS LABELS, not proof
 * of the uncertain critical-group anatomy in damage dispatch. */
extern uint16_t MechStructureOffsetByArmourLocation[MechArmourLocationCount]; /*3EDB:1306*/
extern uint16_t MechStatusGaugeX[MechArmourLocationCount]; /*3EDB:131C, pixels*/
extern uint16_t MechStatusGaugeBottomY[MechArmourLocationCount]; /*3EDB:1332*/
extern uint8_t BTStatsEgaPalette[EgaPaletteRegisterCount]; /*3EDB:1348*/
extern uint16_t BTStatsMcgaPalette[EgaPaletteRegisterCount]; /*3EDB:1358*/
extern uint8_t BTStatsEgaRedCycle[BTStatsPaletteCycleCount]; /*3EDB:1378*/
extern uint16_t BTStatsMcgaRedCycle[BTStatsPaletteCycleCount]; /*3EDB:137C*/
enum { MechStatusGaugeWidth=6, MechHeatGaugeBottomY=183,
    MechHeatGaugeFlashBaseDelay=10, MechHeatGaugeHeatPerDelayStep=6,
    MechHeatGaugeEgaColourToggle=10 }; /*red4 XOR10 -> bright yellow14*/
extern int16_t MechHeatGaugeFlashCountdown,MechHeatGaugeFlashResetDelay; /*3EDB:1218/121A*/
extern uint16_t MechHeatGaugeFlashColour; /*3EDB:121C, XOR modifies low BYTE*/
void Draw_Vertical_Mech_Status_Gauge(uint16_t x,uint16_t bottomY,
    uint16_t greenHeight,uint16_t redHeight);
enum { EmbeddedFontGlyphCount=128, FontGlyphRows=8, FontGlyphIndexMask=127,
    TextCellRowByteStride=FontGlyphRows*EgaFramebufferRowBytes };
extern uint8_t EmbeddedEgaFont[EmbeddedFontGlyphCount*FontGlyphRows]; /* EXE246C:A661 */
extern uint8_t EgaFontForeground,EgaFontBackground; /*246C:B772/B775*/
extern uint16_t TextScreenColumnCount,TextAutoWrapLineCount; /*3EDB:4FB8/4FBE*/
void Set_Text_Colours(uint16_t foreground,uint16_t background);
void EGA_Draw_Glyph(uint16_t glyphOffset,uint16_t rowByteOffset,uint16_t column);
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t column,uint16_t row,uint16_t foreground,uint16_t background);
void Set_Palette_registers(const uint8_t *palette); /* retained1F3D:055D..059E */
void EGA_Set_Palette_Registers_in_out(uint16_t paletteRegister,uint16_t colour); /*207F:022A*/
enum { StarportPatchBytes=38, StarportPatchTileOffset=48*64 }; /* 1C1D-101D */
extern uint8_t StarportMapPatchOverrides[StarportPatchBytes]; /* EXE3EDB:2B8E */
extern uint8_t StarportSavedMapBytes[StarportPatchBytes]; /* 3EDB:5804 */
void Starport_MapPatch_SaveApply_Or_Restore(uint16_t restore); /* 1543:0C72 */
_Static_assert(offsetof(MapStaticStorage,worldVertices)==0x0B0B-0x09FB,"Native vertex span");
_Static_assert(offsetof(MapStaticStorage,templates)==0x0C1D-0x09FB,"Native template span");
void Map_Construct_Nine_Regions(uint16_t centreRegion); /* 207F:104E */
void Map_Copy_Vertex_Corners(uint16_t sourceIndex,uint8_t *lattice); /* NEAR18D8, static-storage index */
enum { MapRegionRowStride=16, MapRegionPreviousRowAndColumn=17 };
/* Pending map SLOT/REGION BYTEs:246C:09F3/09F6, storage below. */
void Map_Move_North(void); /* 207F:158C */
void Map_Move_South(void); /* 207F:163B */
void Map_Move_West(void); /* 207F:16E3 */
void Map_Move_East(void); /* 207F:17C5 */
void Map_Build_Procedural_Block(uint8_t *descriptorOutput,uint8_t *lattice); /* NEAR207F:0BFB */
void Map_Subdivide_Horizontal_Edge(uint8_t *lattice,uint16_t *stackTop); /* NEAR0D07 */
void Map_Subdivide_Vertical_Edge(uint8_t *lattice,uint16_t *stackTop); /* NEAR0D79 */
void Map_Subdivide_Rectangle(uint8_t *lattice,uint16_t *stackTop); /* NEAR0DF8 */
typedef struct MapCacheStorage {
    uint8_t lattice[MapLatticeBytes];
    uint8_t adjacency[MapDescriptorCacheBytes];
    uint8_t descriptors[MapDescriptorCacheBytes];
    uint8_t terrainFlags[MapNeighbourhoodCount];
    uint8_t tiles[MapCacheTileCount];
} MapCacheStorage;
enum { MapRuntimeFirstOffset=0x02D3,MapRuntimeEndOffset=0xA44F,
    MapRuntimeBytes=MapRuntimeEndOffset-MapRuntimeFirstOffset };
/*Original adjacent246C storage. BYTE view preserves encounter pre-check
 * reads into real neighbours; it is NOT a replacement map/game algorithm.
 * Unclassified memory outside this known window is not fabricated here.*/
enum { AnmFrameWorkspaceBytes=0x1E78,AnmFileMaximumBytes=0x3F00 };
typedef union SceneAnimationStorage {
    struct { uint8_t frame[AnmFrameWorkspaceBytes],file[AnmFileMaximumBytes]; } fields;
    uint8_t bytes[AnmFrameWorkspaceBytes+AnmFileMaximumBytes];
} SceneAnimationStorage;
_Static_assert(offsetof(SceneAnimationStorage,fields.file)==0x1E78,"Native244B..42C3 frame/file adjacency");
typedef union MapRuntimeStorage {
    uint8_t bytes[MapRuntimeBytes];
    struct {
        MapCacheStorage cache;
        uint16_t originIndex,originColumn,originRowOffset;
        uint8_t pendingSlots[MapNeighbourhoodWidth],pendingRegions[MapNeighbourhoodWidth];
        uint16_t constructionSeedIndex;
        MapStaticStorage staticData;
        union {
            uint8_t fileWorkspace[GraphicsWorkspaceBytes];
            SceneAnimationStorage sceneAnimation;
        };
        uint16_t partyPositionX,partyPositionY;
    } fields;
} MapRuntimeStorage;
extern MapRuntimeStorage MapRuntime;
#define MapCache MapRuntime.fields.cache
#define CachedMapOriginIndex MapRuntime.fields.originIndex
#define CachedMapOriginColumn MapRuntime.fields.originColumn
#define CachedMapOriginRowOffset MapRuntime.fields.originRowOffset
#define PendingMapGridSlot MapRuntime.fields.pendingSlots
#define PendingMapRegionIndex MapRuntime.fields.pendingRegions
#define MapConstructionSeedIndex MapRuntime.fields.constructionSeedIndex
#define MapStaticData MapRuntime.fields.staticData
#define GraphicsFileWorkspace MapRuntime.fields.fileWorkspace
#define SceneAnimation MapRuntime.fields.sceneAnimation
#define CrescentHawkMapPositionX MapRuntime.fields.partyPositionX
#define CrescentHawkMapPositionY MapRuntime.fields.partyPositionY
_Static_assert(offsetof(MapRuntimeStorage,fields.originIndex)==0x09ED-MapRuntimeFirstOffset,"Native map origin offset");
_Static_assert(offsetof(MapRuntimeStorage,fields.pendingSlots)==0x09F3-MapRuntimeFirstOffset,"Native pending SLOT offset");
_Static_assert(offsetof(MapRuntimeStorage,fields.pendingRegions)==0x09F6-MapRuntimeFirstOffset,"Native pending REGION offset");
_Static_assert(offsetof(MapRuntimeStorage,fields.constructionSeedIndex)==0x09F9-MapRuntimeFirstOffset,"Native map seed offset");
_Static_assert(offsetof(MapRuntimeStorage,fields.staticData)==0x09FB-MapRuntimeFirstOffset,"Native static map offset");
_Static_assert(offsetof(MapRuntimeStorage,fields.fileWorkspace)==0x244B-MapRuntimeFirstOffset,"Native graphics workspace offset");
_Static_assert(offsetof(MapRuntimeStorage,fields.sceneAnimation)==0x244B-MapRuntimeFirstOffset,
    "Native animation and graphics workspace share the same address");
_Static_assert(offsetof(MapRuntimeStorage,fields.partyPositionX)==0xA44B-MapRuntimeFirstOffset &&
    offsetof(MapRuntimeStorage,fields.partyPositionY)==0xA44D-MapRuntimeFirstOffset,"Native party-position offsets");
_Static_assert(sizeof(MapRuntimeStorage)==MapRuntimeBytes,"No invented map runtime padding");
#define MapConstructionLattice MapCache.lattice
#define MapAdjacencyCache MapCache.adjacency
#define MapDescriptorCache MapCache.descriptors
#define LocalTerrainFlags MapCache.terrainFlags
#define CombatMap MapCache.tiles
_Static_assert(offsetof(MapCacheStorage,descriptors)==0x0564-0x02D3,"Native descriptor span");
_Static_assert(offsetof(MapCacheStorage,tiles)==0x07AD-0x02D3,"Native tile span");
_Static_assert(sizeof(MapCacheStorage)==0x09ED-0x02D3,"Native contiguous BYTE cache");
void Map_Rebuild_Adjacency(void); /* NEAR207F:1886 */
void Map_Build_Adjacency_Block(uint16_t descriptorIndex,uint16_t adjacencyIndex); /* NEAR11BB */
uint8_t Map_Adjacency_Left_Edge(uint16_t sourceIndex); /* NEAR12BA */
uint8_t Map_Adjacency_Right_Edge(uint16_t sourceIndex); /* NEAR12D9 */
uint8_t Map_Adjacency_Top_Edge(uint16_t sourceIndex,uint8_t value,uint8_t flags); /* NEAR12F2 */
uint8_t Map_Adjacency_Bottom_Edge(uint16_t sourceIndex,uint8_t value,uint8_t flags); /* NEAR1303 */
enum { MapAdjacency_West=8, MapAdjacency_South=4, MapAdjacency_East=2, MapAdjacency_North=1,
    MapAdjacentBlockColumnDelta=MapBlockTileCount-(MapBlockWidth-1),
    MapAdjacentBlockRowDelta=MapNeighbourhoodWidth*MapBlockTileCount-(MapBlockHeight-1)*MapBlockWidth };
extern uint8_t LocalMapBlockX, LocalMapBlockY;
extern uint16_t CachedMapPositionX, CachedMapPositionY;
uint8_t PosXY_GridSegment(uint8_t blockX,uint8_t blockY,uint16_t *destinationIndex); /* NEAR207F:13D9 */
uint16_t Combat_Check_Terrain_Path(uint16_t actorId,uint16_t targetId,
    uint16_t targetX,uint16_t targetY); /* 1631:1BFE */
uint16_t Combat_Check_Occupancy_And_Crush(uint16_t actorId, int16_t stepX,
    int16_t stepY, uint16_t crushOpposingInfantry); /* 1631:16AB */
void GameSpeed_RateControl(void); /* 1631:1DCC */
void Play_Sound_If_Enabled(uint16_t soundId); /* 0800:19BF */
void Sound_Setup(uint16_t soundId); /*1FC5:0002*/
enum { SoundLibraryWords=313,SoundStream_CommandBase=1000 };
extern const uint16_t SoundLibrary[SoundLibraryWords];
extern uint16_t SoundCommandOrRepeatCount;
extern uint16_t FixedToneDelayMultiplier;
extern uint16_t NoiseCountdownMask;
extern uint16_t NoiseMinimumDelayBits;
extern uint16_t NoiseToggleCount;
extern uint16_t NoiseToggleDelaySeed;
extern uint16_t SweepCentrePitDivisor;
extern uint16_t SweepHalfSpan;
extern uint16_t SweepToneDelayMultiplier;
extern uint16_t SweepRepeatCount;
extern uint16_t SweepDivisorStep;
extern uint16_t NoiseSweepStartMask;
extern uint16_t NoiseSweepStopMask;
extern uint16_t NoiseSweepMinimumBitsSubtract;
extern uint16_t NoiseSweepToggleCount;
extern uint16_t NoiseSweepMaskStep;

void Sound_Play_Fixed_Tone_Repetitions(void); /*original fixed-tone parent*/
void Sound_Play_Seeded_Noise_Repetitions(void); /*original seeded-noise parent*/
void Sound_Play_Divisor_Sweep_Repetitions(void); /*original divisor-sweep parent*/
void Sound_Play_Descending_Noise_Sweep(void); /*original descending-noise parent*/
void Sound_Play_Ascending_Noise_Sweep(void); /*original ascending-noise parent*/
void A_Timer(void);
void A_PC_Speaker_OFF(void);
void Sound_Play_Fixed_Tone_Delay(uint16_t divisor,uint16_t delayMultiplier);
void Sound_Play_Seeded_Noise_Toggles(uint16_t mask,uint16_t minimumBits,uint16_t toggles,uint16_t delaySeed);
void Sound_Play_Divisor_Sweep(uint16_t centre,uint16_t halfSpan,uint16_t delayMultiplier,uint16_t repetitions,uint16_t step);
void Sound_Streaming_1(uint16_t startMask,uint16_t stopMask,uint16_t minimumSubtract,uint16_t toggles,uint16_t step);
void Sound_Streaming_2(uint16_t startMask,uint16_t stopMask,uint16_t minimumSubtract,uint16_t toggles,uint16_t step);
void Sound_Play_Sweep_Tone_Delay(uint16_t divisor,uint16_t delayMultiplier);
extern uint16_t BusyWaitScale,SweepCurrentPitDivisor,SweepCurrentDelayMultiplier;
void Sound_Freq_Loop(uint16_t mask,uint16_t minimumBits,uint16_t toggles);
enum { SoundNoiseDefaultToggleSeed=1000 };
uint16_t PC_Speaker_XOR_ptr_freq(uint16_t seed); /*207F:007D, DX return*/
void Sound_Freq_Countdown_Loop(uint16_t delayState,uint16_t mask,uint16_t minimumBits); /*explicit native DX input*/
void Register_Persistent_Map_Effect(uint16_t spriteId,uint16_t packedX,
    uint16_t packedY); /* 183B:27C9 */
void Salvage_Mechs_Dialog(void); /* 0DAB:04F9 */
uint8_t *Append_Text_To_Memory(uint8_t *destination, uint8_t *source);
uint8_t *Append_Large_Text_To_Memory(uint8_t *destination, uint8_t *source);
uint16_t Loop_Until_TextPtr_Null(uint8_t *text);
void Display_Sentence_Period(void);
/* Native overlapping/pre-biased WORD views, not independent flag arrays. */
#define DeadInfantryFlags (CombatantCasualtyFlags + Enemy_All_CombatantId_Range_First)
#define LootableInfantryFlags (CombatantCasualtyFlags + Enemy_Infantry_CombatantId_Range_First)
enum {
    Friendly_Infantry_Combatant_Range_First=4, Character_Rex=1,
    Enemy_Infantry_Record_First=8, Enemy_Infantry_Record_Count=8,
    LootWeaponGateMechByteBias=11, /* C6EB +4*11h - C724 =0Bh */
    InfantryLootBaseCBills=3, InfantryLootRandomBonusMask=15,
    InfantryLootMinimumCBills=2
};
extern uint16_t CombatComputerControl;
void Loot_Enemy_Soldiers_Dialog(void); /* 0DAB:094B */
uint16_t Return_Bool_Allow_Computer_Control_Dialog(void); /* 0DAB:0B5E */
void Distribute_Weapon_To_Party(uint16_t weaponId); /* original0FDC:15E6 */
void Distribute_Purchased_Armour(uint16_t heldArmourType, uint16_t heldArmourValue); /* 0FDC:13DE */
enum { ArmourType_None=0, ArmourType_FlakVest=1, ArmourType_FlakSuit=2,
    ArmourType_LightEnv=3, ArmourType_HeavyEnv=4, ArmourType_Ablative=5,
    ArmourTypeCount=6 };
extern uint8_t *ArmourTextDescription[ArmourTypeCount]; /* 3EDB:4E8A */
enum {
    Infantry_Cudgel=0, EquipmentDistributionMenuLayout=4,
    EquipmentDistributionEquipmentColumn=10, EquipmentDistributionWarningRow=20
};
#define EquipmentDistributionMenuOptionCount MenuControls[4].optionCount /*305B:00D6*/
enum { ArenaStagingMechAssignment=3 }; /* original temporary slot3 assignment, not skill level */
extern uint8_t ArenaMechRecordBackup[MechRecordSize]; /* 3092:3780 */
extern uint8_t SavedArenaMechPilotId[LanceSize]; /* 3092:430E */
extern uint8_t SavedArenaMechRiderId[LanceSize]; /* 3092:3FFA */
extern uint8_t SavedPartyNameId[PartySize]; /* 3092:3FE9 */
extern uint16_t ArenaRentalMechMode; /* 3092:E48E */
void Prepare_Party_Mech_For_Arena(void); /* 0FDC:1A26 */
void Restore_Party_After_Arena_Combat(void); /* 0FDC:1B41 */
void Prepare_Rental_Locust_For_Arena(void); /* 0FDC:1C9B */
enum { BldFileCount=26, BldFixedDecodeSpan=9000,
    /* Sol: The 9000-byte BLD transform is not this shared region's capacity.
     * The shipped length-prefixed BTSTATS.CMP contributes 12635 payload bytes
     * at 3092:00A0.  Keep enough host storage for that largest verified use;
     * otherwise the ordinary DOS-style loader overwrites unrelated C globals. */
    SharedBtStatsBldBufferBytes=12635,
    BldDecodeAddition=0x29, BldDecodeXor=0xE9,
    BldFirstDiskOneId=2, BldFirstLateDiskTwoId=17,
    NativeFarPointerBytes=4, NativeByteBits=8 };
extern uint8_t *BldFileNameById[BldFileCount]; /* 3EDB:4EC2 */
extern uint8_t BTStatsOrBldMemory[SharedBtStatsBldBufferBytes]; /* 3092:00A0 */
extern uint16_t BldFileIndex, BTStatsAssetLoaded; /* 3092:3FF8/4594 */
void Load_And_Decode_Indexed_BLD(uint16_t bldFileId);
uint16_t Read_Bld_Target(uint8_t *targetBytes);
uint16_t Read_Bld_Immediate_Word(uint8_t *wordBytes);
void Select_Game_Disk_And_Drive(uint16_t requestedDiskNumber);
uint16_t Request_Game_Disk(uint16_t requestedDiskNumber);
extern uint16_t RequestedGameDiskNumber; /* 3EDB:014E */
extern uint16_t HasHardDisk, SecondFloppyDriveAvailable, CurrentMenuLayoutIndex; /* D580/3FFE/4600 */
extern uint8_t DiskDriveNameText[9]; /* 3EDB:0504 */
enum { GameDisk_First=1, GameDisk_Second=2, DOSDrive_A=0, DOSDrive_B=1,
    DiskPromptMenuLayout=4, EGA_BrightRed=12, DiskDriveLetterPosition=6 };
void DOS_Select_Default_Drive(uint16_t drive);
void DOS_Load_File_to_memory(const uint8_t *filename, uint8_t *buffer, uint16_t bytesToRead);
uint16_t Load_File_To_Memory(const uint8_t *filename, uint8_t *memory);
enum { PackedGraphicsOutputBytes=32000, PackedGraphicsRowBytes=160,
    EgaScreenWidth=320, EgaScreenHeight=200, GraphicsFormat_Sequential=1 };
void VGA_Inline_ASM_Loop(uint8_t *source,uint8_t *destination,uint16_t wordsToConvert); /*207F:0572*/
enum { CombatSpriteCount=376, CombatSpriteHeaderBytes=4, SpriteHeightMinusOneByte=1,
    SpriteWidthByte=2, SpritePlanesPerCell=4, SpriteWordsPerCell=2 };
extern uint8_t *CombatSpritePointers[CombatSpriteCount]; /*3092:39FA..3FD9 native FAR table*/
extern uint8_t *TinylandTileset;
void Setup_Game(void); /*0D27:0044*/
void Startup_Print_Nul_String(uint8_t *text);
void Platform_Write_Startup_Character(int16_t character);
void Initialize_Graphics_Runtime(void);
void Start_Game(void); /*0800:50C8*/
enum { GameSeedTableSize=MapConstructionSeedCount, AttractDemoFileBytes=1023,
    AttractDemoBufferOffset=0x2710, /*3092:27B0 minus shared buffer3092:00A0*/
    TextPanel_FirstTimePlayer=6 };
#define GameSeeds MapConstructionSeeds /*same native246C:09FB storage*/
_Static_assert((int)GameSeedTableSize==(int)MapConstructionSeedCount,"One native startup/procedural seed table");
_Static_assert(offsetof(MapRuntimeStorage,fields.staticData.constructionSeeds)==
    0x09FB-MapRuntimeFirstOffset,"Native shared seed address");
void Load_And_Draw_ANIMATE_ICN(void);
enum { AnimatedMapTileHeaderBytes=128, AnimatedMapFrameCount=3,
    AnimatedMapTileCount=10, AnimatedMapPixelsPerTile=16*16,
    AnimatedMapFrameBytes=AnimatedMapPixelsPerTile/2,AnimatedMapTilePlaneBytes=AnimatedMapFrameBytes/4,
    AnimatedMapTileBytes=AnimatedMapFrameCount*AnimatedMapTileCount*AnimatedMapPixelsPerTile/2 };
extern uint8_t AnimatedMapTileFrames[AnimatedMapTileBytes];
void Load_And_Play_Intro_Music(void);
void Load_Game_Map_Data(void);
enum { CacheSecurityCodeCount=33,
    StockCompanyCount=3,PersistentMapEffectSlotCount=64,
    NewGameStartingCBills=20,NewGameAllowanceWealthLimit=50,
    JasonStartingBody=8,JasonStartingDexterity=9,JasonStartingCharisma=7,
    CharacterSpriteFamily_Jason=0x96,CharacterSpriteFamily_EnemyInfantry=0xFE,
    CitadelStartingPositionX=0x0C45,CitadelStartingPositionY=0xC019,CitadelStartingPackedPage=0xCC,
    CitadelInitialRevealIndex=0x062C,CitadelInitialRevealRows=6,CitadelInitialRevealMask=0x1F,
    MapCacheSlot_Centre=4 };
extern uint8_t CacheSecurityCodeUsed[CacheSecurityCodeCount];

enum { Command_Pause=32, AnimationO15_LyranBlastDoor=15,
    AnimationPlayback_RestoreGameView=0, ExplorationIdleTickCountdownReload=10,
    RoamingNpcUpdatePhaseTrigger=2, MapFogSubrowsPerWorldRegion=8,
    FogOfWar_Visible=255, FogOfWarLocalColumnMask=0x70, FogOfWarLocalRowMask=0x70,
    FogOfWarLocalColumnShift=4, PackedWorldLastY=0xF07F,
    StarportPatchFirstWorldX=0x0800, StarportPatchPastLastWorldX=0x0D00,
    StarportPatchFirstWorldY=0x6000, StarportPatchPastLastWorldY=0xB000,
    CombatEncounter_Roaming=0, ComstarAllowancePaymentCBills=15,
    StockGrowthNumerator=100, StockDeclineDenominator=110, StockCompany_BakPhar=2,
    BakPharSplitThresholdCBills=18000, BakPharSplitDivideByFourShift=2 };
extern uint16_t ExplorationStepsPerInput,MessageBoxOpen;
extern uint8_t NpcUpdatePhase,FogOfWarColumnBitMask[CompassDirectionCount];
extern uint16_t MovementCommandByCompass[CompassDirectionCount];
extern uint8_t *CompassDirectionText[CompassDirectionCount];
extern uint16_t StockChangeRollMask[StockCompanyCount],StockChangeFactor[StockCompanyCount];
uint32_t Get_CBill_Allowance_Wealth_Limit(void); /*0800:29F5*/
void Display_Animation_Scene(uint16_t animationId,uint16_t playbackMode); /*0800:48B7*/
void EGA_DrawBox_Wrapper(void); /*1F3D:06C3 retained EGA viewport presentation*/

enum { AnmFrameWidth=88,AnmFrameHeight=88,
    AnmPackedFrameBytes=AnmFrameWidth*AnmFrameHeight/2,
    AnmFileNativeOffset=0x42C3,AnmStreamNativeOffset=0x42F6,
    AnmDelayTableOffset=0x20,AnmTimingScaleOffset=0x32,AnmFirstTimingToken='A',
    AnmSirenRepeatCount=7,AnmFirstFrameHoldRetraces=50,AnmRestoreHoldRetraces=60,
    AnimationO00_MechStartUp=0,AnimationO02_NeuroHelmet=2,
    AnimationO06_CrescentHawkCard=6,AnimationO07_WaspFiring=7,
    AnimationO08_SirenAlarm=8,AnimationO10_HPGTransmission=10,
    AnimationO16_WaspLostArm=16,AnimationPlayback_Force_CallerRedraw=2,
    Sound_MechStartUp=12,OuttakeFrequencySettingCount=3 };
#define AnimationFrameWorkspace SceneAnimation.fields.frame
#define AnimationFileData SceneAnimation.fields.file
#define OuttakeFrequency PersistentState.bytes[79] /*3092:D35B*/
#define WeaponProficiencyUseCount (*(uint8_t (*)[PartySize])(&PersistentState.bytes[80])) /*D35C..D363*/
#define LastWeaponProficiencyCategory (*(uint8_t (*)[PartySize])(&PersistentState.bytes[88])) /*D364..D36B*/
extern uint16_t OuttakeFrequencyRandomMask[OuttakeFrequencySettingCount]; /*DS0AC4*/
extern uint16_t AnimationStreamOffset,AnimationFrameNumber; /*3092:0064/E48A*/
uint16_t Animation_Decode(uint8_t *encoded,uint8_t *frame); /*207F:23EC*/
void Decode_Draw_Next_ANM_Frame(void); /*0800:1AFD*/
void DrawCall_EGA_Animations(void); /*207F:1E37*/
void Update_Friendly_Movement_Animations(uint16_t command);
void Update_Animated_Map_Tiles(void);
void Update_Roaming_Map_Npcs(void);
void Game_Pause_Menu(void); /*0800:2C50*/
enum { PauseMenuPanel=1, PauseMenuTopRow=13, PauseMenuBaseChoices=7,
    PauseMenu_Settings=1, PauseMenu_Allocate=2,
    PauseMenu_WithMechs_Inspect=3, PauseMenu_WithMechs_Heal=4,
    PauseMenu_WithMechs_Load=5, PauseMenu_WithMechs_Save=6, PauseMenu_WithMechs_Map=7,
    PauseMenu_NoMechs_Inspect=2, PauseMenu_NoMechs_Heal=3,
    PauseMenu_NoMechs_Load=4, PauseMenu_NoMechs_Save=5, PauseMenu_NoMechs_Map=6 };
void Menu_Change_Game_Settings(void); /*0800:3BD0*/
extern uint16_t SoundEffectsEnabled; /*3EDB:015C*/
#define CacheMapRoomLoaded PersistentState.bytes[66] /*3092:D34E*/
#define SelectedCacheCodeByColour (*(uint8_t (*)[3])(&PersistentState.bytes[59])) /*D347..D349*/
#define WhiteCacheCodeCorrect PersistentState.bytes[62] /*D34A*/
#define PhoenixHawkFound PersistentState.bytes[63] /*D34B*/
#define HPGTerminalPowerOn PersistentState.bytes[64] /*D34C*/
#define MechPartsCacheFound PersistentState.bytes[65] /*D34D*/
enum { CacheCodeColourCount=3,CacheTerminalWidthCells=3,
    CachePowerControlX=0x4E,CachePowerControlFirstY=0x0C,CachePowerControlLastY=0x11,
    CacheLocalEvenCoordinateMask=0x7E,
    CacheTransmitterFirstX=4,CacheTransmitterFirstY=4,
    CacheTransmitterSecondX=2,CacheTransmitterSecondY=0x0E,
    Bld_EndMech=22,Bld_WinScene=25 };
extern int8_t SecurityTerminalPosX[CacheSecurityCodeCount],SecurityTerminalPosY[CacheSecurityCodeCount];
extern int8_t SecurityTerminalColour[CacheSecurityCodeCount]; /*3EDB:21CE/21F0/2212*/
extern uint8_t *SecurityCodeColourText[CacheCodeColourCount]; /*3EDB:2234*/
void Interact_with_BLD(uint16_t bldId); /*0FDC:0008*/
void StarLeague_Cache_PhoenixHawk(void); /*135D:0288*/
void Display_StarLeague_Cache_Dialog_Window(void); /*135D:02A8*/
void StarLeague_HyperPulse_Power_Dialog_Window(uint16_t worldX,uint16_t worldY); /*135D:02D2*/
void StarLeague_Security_Terminal(uint16_t worldX,uint16_t worldY); /*135D:03AA*/
void HPGTransmitter(uint16_t worldX,uint16_t worldY); /*135D:04AB*/
enum { CacheDoorCount=12,CacheDoorEntrance=11,CacheDoorFrameCount=3,
    CacheDoorFrameTileCount=4,CacheDoorLookupByPosition=-1,
    CacheDoorFrameRetraces=20,Sound_CacheDoor=10,
    CacheDoorCoarseCellMask=0x70,CacheDoorEvenCellWithinBlockMask=0x0E,
    CacheDoorCoarseYByteStride=32,CacheDoorEvenCellByteStride=4,
    CacheDoorNextBlockGap=MapBlockTileCount-MapBlockWidth };
extern int8_t CacheDoorPositionX[CacheDoorCount],CacheDoorPositionY[CacheDoorCount];
extern int8_t CacheDoorRequiredCode[CacheCodeColourCount][CacheDoorCount]; /*3EDB:2492/249E/24AA*/
extern uint8_t CacheDoorAnimationTiles[CacheDoorFrameCount*CacheDoorFrameTileCount]; /*3EDB:246E*/
#define CacheDoorOpened (*(uint8_t (*)[CacheDoorCount])(&PersistentState.bytes[67])) /*D34F..D35A*/
void StarLeague_Key_Codes(uint16_t worldX,uint16_t worldY,int16_t replayDoorId); /*135D:0AB6*/
enum { CacheMapRoomTileBytes=768,CacheStarPuzzleTargetCount=7,
    CacheStarTileRangeFirst=0x97,CacheStarTileRangeLast=0xF0,
    CacheStarSelectedParityMask=1,CacheStarMapCoarseYByteStride=16,
    Sound_MapInteraction=15,Sound_PasswordAccepted=16,Sound_PasswordIncorrect=17,
    CachePartsX=0x7C,CachePartsY=4,CacheWhiteSecurityX=0x48,CacheWhiteSecurityY=0x38,
    CacheMapLadderX=0x0C04,CacheMapLadderY=0xC022,CacheLadderX=0x0C45,CacheLadderY=0xC039,
    CacheEmptyDescriptor=0xD0,CacheMapRoomEmptyDescriptor=0xD1,
    CacheMapRoomDescriptorRows=3,CacheMapRoomDescriptorColumns=4,
    CacheMapRoomBlockingThreshold=0x8B,Tileset_StarLeagueCache=2,Tileset_MapRoom=3,Bld_FindIt=24 };
extern uint16_t CacheStarPuzzleTargetOffsets[CacheStarPuzzleTargetCount]; /*3EDB:241E*/
void StarLeague_Map_Room(uint16_t worldX,uint16_t worldY); /*135D:055A*/
void Draw_STARLEAG_ICN_Scene(void); /*135D:079C*/
void Map_Interactable_Play_Sound(uint16_t worldX,uint16_t worldY); /*135D:0913*/
void Cache_StarMap_CorrectPassword(void); /*135D:0980*/
enum { GameSettingsMenu=33, CombatSpeedMenu=34, OuttakeFrequencyMenu=38,
    MovementRateMenu=39, MovementRateChoiceCount=3,
    GameSettings_MovementRate=0, GameSettings_CombatSpeed=1,
    GameSettings_ToggleSound=2, GameSettings_OuttakeFrequency=3, GameSettings_Quit=4,
    ExplorationSteps_One=1, ExplorationSteps_Three=3, ExplorationSteps_Four=4 };
void Menu_Assign_Pilots(void); /*0800:4D57*/
enum { MapTile_BlockDismounting=128,
    AssignmentMode_SynchronizeExisting=0, RefreshLowerPanel=4, RefreshPartyPanel=3 };
void Assign_Pilot_and_rider_to_Mechs(uint16_t resetAssignments); /*1467:0002*/
enum { CrewAssignmentPanel=6,CrewAssignmentMechSummaryRow=10,
    CrewVehicleNameTemporaryTerminator=11,MECH_NoPilot=255 };
void Menu_Draw_MultiSelect(uint16_t drawPlanningPreview); /*1631:032F*/
void Draw_Menu_MultiSelect(void); /*0800:0E4B*/
enum { CombatMechViewportFirstCellX=11,CombatMechViewportLastCellY=26,
    CombatMechProjectionMinimumX=-117,CombatMechProjectionMaximumY=3994,
    VisibleMechMaximum=2*LanceSize };
extern uint8_t CombatantVisibleOnScreen[AllCombatantCount]; /*3092:42F6*/
extern uint8_t MapTileUnderCombatant[AllCombatantCount]; /*3092:3750*/
extern uint8_t CombatMechYParityAdjustment[AllCombatantCount],CombatMechPreviousMapRowTile[AllCombatantCount]; /*4554/45CE*/
extern uint16_t CombatantScreenPixelX[AllCombatantCount],CombatantScreenPixelY[AllCombatantCount]; /*324C/327C*/
extern const uint16_t CombatMechColumnOffset[128],CombatMechOddCameraColumnOffset[128];
void Inspect_Characters(void); /*0800:378D*/
#define SelectedPartyMemberSlot PersistentState.bytes[14] /*3092:D31A count/selection alias*/
#define NumberOfActiveLanceMechs PersistentState.bytes[16] /*3092:D31C*/
extern int8_t MechHeatLevel[MechRecordCount]; /*3092:006E*/
extern uint8_t ArmourTypeDurability[ArmourTypeCount];
extern uint8_t *SkillLevelDescriptions[SkillLevel_Excellent+1];
enum { CitadelDialog_CountActiveMechs=17, CitadelDialog_CountOtherPartyMembers=14,
    CitadelDialog_SelectPartyMember=13, InspectValueColumn=8, InspectArmourRow=8,
    InspectSkillColumn=12, InspectFirstSkillRow=13, EGA_BrightYellow=14 };
void Examine_Screen_BTSTATS_CMP(uint16_t mechId); /*0DAB:1AFE*/
enum { ComponentPipsFirstRowCount=5, ComponentPipCellWidth=8, EGA_Green=2, EGA_Red=4 };
void Draw_Component_Status_Pips(uint16_t column,uint16_t row,uint16_t totalPips,uint16_t healthyPips);
enum { CombatDescriptionHeadingRow=7, CombatDescriptionWeaponRow=8,
    CombatDescriptionArmourRow=10, EGA_DarkGrey=8 };
void Display_Friendly_Combatant_Description(uint16_t combatantId,uint16_t allowTraitorRecognition,uint16_t showInfantryDetails);
enum { ScanSide_Friends=0, ScanSide_Enemies=1, ScanSide_Cancel=2,
    CombatantPosition_Unused=65535,
    ScanSideChoiceCount=3, ScanMenuFirstChoiceRow=1,
    CombatMenu_MechScanChoice=6, CombatMenu_InfantryScanChoice=4 };
void Scan_Enemies(uint16_t scanningCombatantId); /*183B:2591*/
void Combat_Browse_Scan_Targets(uint16_t scanningCombatantId,uint16_t targetSideBase,uint16_t includeMechDetails); /*0DAB:1467*/
enum { EnemyCombatantToRecordOffset=8, CombatantsPerSide=12,
    ScanBrowserBaseChoiceCount=2, ScanBrowserDetailChoiceCount=3, ScanBrowser_Detail=1 };
void Combat_Render_Movement_Preview(uint16_t combatantId,uint16_t showPlanningPreview); /*183B:1774*/
enum { CombatMovementOrderBytes=48, CombatMovementPlanBytesPerUnit=24,
    CombatMovementPlanEnd=2, CombatPreviewOriginColumn=26, CombatPreviewOriginRow=12,
    CombatPreviewGlyphRowStride=4 }; /*12 signed dx/dy pairs per unit; cell origin*/
enum { CombatPreviewBackgroundBlack=0 };
extern uint8_t CombatMovementOrders[AllCombatantCount*CombatMovementOrderBytes];
extern uint8_t CombatMovementPlanBytes[AllCombatantCount*CombatMovementPlanBytesPerUnit];
extern uint16_t MovementPreviewEndpointColumn,MovementPreviewEndpointRow;
void Combat_Mech_Movement(uint16_t mechId,uint16_t selectedMovementMode); /*183B:22BC*/
enum { MovementMode_Walk=0, MovementMode_Run=1, MovementMode_Jump=2,
    MechLegPrimaryActuatorMask=8, MechLegSecondaryActuatorFirstMask=4,
    MechHeatPerMovementPointLost=5, MechHeatShutdownLevel=30 };
extern uint16_t CurrentMechHeatMovementPenalty,CurrentMechLegDamage;
void Combat_Calculate_Movement(uint16_t combatantId); /*183B:193B*/
enum { CombatOrderXRegionMask=0x0F00,CombatOrderYRegionMask=0xF000,
    CombatMovementTerrainLimit=0x40,CombatTerrainNormalStepCost=1,
    CombatTerrainObstructedStepCost=2,CombatTerrainTallStepCost=3,
    CombatTerrainCameraOddXMaskToggle=10,CombatTerrainCameraOddYMaskToggle=5 };
extern uint8_t CombatantActionState[Enemy_All_CombatantId_Range_First];
enum { CombatMovementDestinationBytes=4,CombatDestinationCursorYellow=14,
    MovementPlanningFinish_Space=32,MovementPlanningFinish_Return=13 };
extern uint8_t *SelectedMovementModeText[MovementMode_Jump+1];
void Combat_Select_Movement_Plan_For_Turn(uint16_t combatantId,uint16_t selectedMovementMode); /*183B:1C1F*/
void Display_Text_Human_Health(uint16_t characterId); /*1631:02E4*/
enum { HealthDescriptionCount=CharacterHealthPerBodyPoint+1 };
extern uint8_t *HealthDescriptions[HealthDescriptionCount]; /*3EDB:2E0C*/
void Save_Game(void); /*0800:35D3*/
enum { SaveSlotMenu=40,SaveSlotCancel=6,OriginalSaveFormatMarker=12,GameDisk_Save=3,ViewedHolodiskCacheMarker=12 };
extern uint8_t SaveGameFileName[6]; /*3EDB:0154*/
void Show_Overhead_Map(void); /*0800:3D40*/
enum { OverheadHorizontalStep=0x0200,OverheadVerticalStep=0x2000,
    OverheadWestMinimumX=0x0300,OverheadEastMaximumX=0x0D00,
    OverheadNorthMinimumY=0x2000,OverheadSouthMaximumY=0xE000,
    CacheOverviewFogCell=0x066C,CacheOverviewFogClearMask=0x7F };
uint16_t Overhead_Map_Draw(uint16_t partyX,uint16_t partyY);
enum { OverviewDynamicFirstTile=0x90,OverviewBufferColumns=40,
    OverviewDedicatedTerrainCategory=0x10,OverviewDedicatedTerrainTile=0x40,
    OverviewTerrainRemapThreshold=0x20,OverviewTerrainCategoryStep=0x10,
    OverviewScratchHighByte=0x40,OverviewPackedTileOffset=0x3FE0,
    OverviewPackedTileBytes=32,GraphicsAdapter_Ega=2 };
void Overhead_Map_Build_Centre_Block(uint16_t destination);
void Pack_Dynamic_Overhead_Tile(uint16_t tileId);
void Build_Dynamic_Overhead_Tile(uint16_t tileId);
enum { OverviewMetadataBytes=98,OverviewMetadataY=14,OverviewMetadataColumns=28,
    OverviewMetadataRows=42,OverviewMetadataSpans=56,OverviewMetadataGlyphX=82,
    OverviewMetadataGlyphY=90,OverviewMtpScratchOffset=0x4000,
    OverviewLastFirstColumn=11,OverviewLastFirstRow=0xD0,
    OverviewRegionRows=3,OverviewRegionColumns=5,OverviewCacheCopyBytes=0x1080,
    OverviewBufferCells=960,OverviewBufferRows=24,OverviewCacheBackgroundAdjustment=0x30,
    OverviewCacheBlockDestination=0x150,OverviewMtpHeaderBytes=0x21D,
    OverviewTerrainPixelsHigh=192,OverviewDemoTimeout=600,
    OverviewObjectiveRegion=0x8A00,OverviewObjectiveX=0x0A38,OverviewObjectiveY=0x8038 };
extern uint8_t OverviewMapMetadata[OverviewMetadataBytes];
#define ShowOverheadObjectiveDirection PersistentState.bytes[47] /*3092:D33B*/
void Combat_Run_Encounter(uint16_t encounterType);
void Combat_Mechanics(uint16_t prearrangedEncounter); /*1AE8:000C; portable first-use no-impact, assigned carry retained*/
extern uint8_t CombatMovementStepCursor[AllCombatantCount]; /*3092:0078..008F*/
extern uint8_t CombatSavedTileCache[MapCacheTileCount]; /*3092:4314..4553*/
uint16_t Combat_StructureHit(uint16_t hitLocationOffset); /*1631:1122*/
enum { CombatGraphics_None=0,CombatMessage_Brief=1,CombatMessage_Verbose=2,
    MechHeatToHitPenaltyFirst=8,MechHeatToHitPenaltySecond=13,
    MechHeatToHitPenaltyThird=17,MechHeatToHitPenaltyFourth=24,
    WeaponIndex_MediumLaser=16,AnimationO05_NeuroSmoking=5 };
void Combat_AudioVisual_Effects(uint16_t shooterId,uint16_t targetCombatantId,
    uint16_t weaponIndex,uint16_t attackDirection,uint16_t playTargetImpactStream,
    uint16_t targetPersonnelDead,uint16_t targetMechDestroyed,uint16_t targetRecordId,
    uint16_t attackApplied,uint8_t *savedCombatMap); /*1AE8:12C7..1E45, EGA2*/
int16_t Word_Absolute(int16_t value); /*207F:3C6C*/
extern uint8_t CombatSavedFacing[AllCombatantCount]; /*3092:45B6*/
enum { MainCharacterCount=2,CombatTargetIdMask=0x7F,
    WeaponIndex_Vibroblade=3,WeaponIndex_Shortbow=4,WeaponIndex_Crossbow=6,
    WeaponIndex_Pistol=7,WeaponIndex_Rifle=8,WeaponIndex_SubmachineGun=9,
    WeaponIndex_PersonnelSRM=10,WeaponIndex_Inferno=11,
    WeaponIndex_LaserPistol=12,WeaponIndex_LaserRifle=13,
    WeaponIndex_SmallLaser=15,WeaponIndex_PPC=18,WeaponIndex_Autocannon2=19,
    WeaponIndex_Autocannon20=22,WeaponIndex_MachineGun=23,WeaponIndex_LRM5=25,WeaponIndex_SRM6=31,
    AttackEffect_None=0,AttackEffect_Missile=1,AttackEffect_PersonnelBeam=2,AttackEffect_MechBeam=3,
    Sprite_Impact_Large=127,Sprite_Locust_Wreck=128,Sprite_Commando_Wreck=129,
    MissileAnimationSpriteBase=104,ProjectileImpactSpriteBase=250,
    AnimationO01_ManPadVsMech=1,AnimationO03_CockpitHit=3,AnimationO04_LocustFiring=4,
    EGA_Cyan=3,EGA_Magenta=5,Sound_Missile=1,Sound_MechEnergyWeapon=2,
    Sound_RepeatingProjectile=3,Sound_InfantryUnknown=4,Sound_VibroBlade=5,
    Sound_SingleShotProjectile=6,Sound_ArenaDestroyedUnknown=7,
    Sound_TerrainDamageUnknown=8,Sound_PersonnelLaser=9,Sound_BowString=11,Sound_BladeImpact=13 };
extern uint8_t *KuritaMissionTextMessages[5]; /*3EDB:3A2E*/
void Combat_Move_Position(uint16_t x,uint16_t y); /*0800:186F*/
void Salvage_Armour_Dialog(void); /*0DAB:0002; SRM-6 stack bucket unsupported*/
extern uint8_t *SalvageDescriptionByTechSkill[3]; /*3EDB:0FA2..0FAD*/
enum { SalvageScrapRandomBonusMask=0x7F,SalvageScrapBaseCBillsPerWreck=90 };
/* Both annotation names refer to the SAME native396C array. */
#define CombatantAnimationSelector CombatantAnimationDirection

enum { CombatantAnimationCursorCount=AllCombatantCount+1,
    EffectAnimationCursorId=AllCombatantCount,
    WalkAnimationStreamBytes=8, WalkAnimationStreamNativeOffset=0x0270,
    AnimationToken_SetDirection=0xFD,AnimationToken_RelativeLoop=0xFE,
    AnimationToken_Wait=0xFF, RoamingNpcDelayRandomMask=31,
    RoamingNpcBoundarySubregionSize=16, RoamingNpcBoundaryLowNibbleMask=15,
    PackedPositionCoarseXMask=0x0F70,PackedPositionCoarseYMask=0xF070 };
extern uint16_t MovementActorPositionX,MovementActorPositionY; /*3092:E486/E488*/
void Combat_CompassPos(uint16_t cameraX,uint16_t cameraY,uint16_t targetX,uint16_t targetY);
uint16_t Combat_Projectile_Octant(int16_t deltaY,int16_t deltaX);
void Combat_Copy_Map_Cache(uint8_t *externalBuffer,uint16_t restoreCache); /*207F:1ECE*/
void Combat_Restore_Map_View_And_Draw_World(uint8_t *savedMap);
void Draw_Combat_Sprites(uint16_t spriteId,uint16_t pixelX,uint16_t pixelY);
extern uint16_t FixedTonePitDivisor; /*3092:3984; low BYTE also selector[24]*/

enum { MovementProbeFlag=0x80,MovementProbeActorMask=0x7F,
    MovementHeading_AtDestination=0xFFFF,MovementSearchBankTogglePeriod=30 };
extern int8_t MovementDirectionSearchOffsets[2*CompassDirectionCount]; /*DS310A*/
extern int16_t MovementDirectionStepX[CompassDirectionCount],MovementDirectionStepY[CompassDirectionCount]; /*311A/312A*/
extern int16_t MovementPackedBoundaryX[CompassDirectionCount],MovementPackedBoundaryY[CompassDirectionCount]; /*313A/314A*/
extern uint16_t MovementSearchBankTimer,MovementSearchBank; /*DS315A/315C*/
extern int16_t CombatChosenStepX,CombatChosenStepY; /*3092:458E/4590*/
extern uint16_t CombatDestinationBlocked; /*3092:D57E, sticky until caller clears*/
/* Host data window for an original byte segment; cursor arithmetic wraps ONLY
 * the native offset WORD. Data may cross offsetFFFF, without segment carry. */
typedef struct AnimationCursor {
    uint8_t *data;
    uint16_t dataOffset,offset;
} AnimationCursor;
extern AnimationCursor CombatantAnimationCursors[CombatantAnimationCursorCount];
extern AnimationCursor FriendlyMechWalkAnimationByDirection[CompassDirectionCount];
extern AnimationCursor FriendlyInfantryWalkAnimationByDirection[CompassDirectionCount];
extern uint8_t WalkAnimationStreams[2*CompassDirectionCount*WalkAnimationStreamBytes];
enum { PersonnelAttackWeaponCount=15,CombatAttackBytecodeBytes=264,
    CombatAttackBytecodeNativeOffset=0x3EF0,CombatTargetImpactBytecodeBytes=6,
    CombatProjectileImpactBytecodeBytes=4 };
extern uint8_t AttackAnimationBytecode[CombatAttackBytecodeBytes];
extern uint8_t TargetImpactBytecode[CombatTargetImpactBytecodeBytes];
extern uint8_t ProjectileImpactBytecode[CombatProjectileImpactBytecodeBytes];
extern AnimationCursor LocustFireAnimationStreams[CompassDirectionCount];
extern AnimationCursor CommandoFireAnimationStreams[CompassDirectionCount];
extern AnimationCursor LocustKickAnimationStreams[CompassDirectionCount];
extern AnimationCursor CommandoKickAnimationStreams[CompassDirectionCount];
extern AnimationCursor MissileAnimationStreams[CompassDirectionCount];
extern AnimationCursor PersonnelAttackAnimationStreams[PersonnelAttackWeaponCount*CompassDirectionCount];
extern AnimationCursor CombatTargetImpactStream,CombatProjectileImpactStream;
extern int8_t LocustMuzzleX[CompassDirectionCount],LocustMuzzleY[CompassDirectionCount];
extern int8_t CommandoMuzzleX[CompassDirectionCount],CommandoMuzzleY[CompassDirectionCount];
extern int8_t PersonnelMuzzleX[CompassDirectionCount],PersonnelMuzzleY[CompassDirectionCount];
extern int8_t ProjectilePrimaryStepX[CompassDirectionCount],ProjectilePrimaryStepY[CompassDirectionCount];
extern int8_t ProjectileSecondaryStepX[CompassDirectionCount],ProjectileSecondaryStepY[CompassDirectionCount];
extern uint8_t CombatantMovementDirection[AllCombatantCount];
enum { MovementDeltaDirectionEntries=11,MovementDeltaDirectionBias=5,MovementDeltaDirectionRowStride=4 };
extern const uint8_t MovementStepDirectionByDelta[MovementDeltaDirectionEntries]; /*EXE-owned3EDB:2ECC..2ED6*/
enum { CombatHitFacingEntries=15, CombatHitFacingBias=7,
    CombatMovementSlices=12, CombatMovementPenaltyEntries=CombatMovementSlices+1,
    CombatHitLocationCategories=4, CombatHitLocationRollOutcomes=11,
    CombatHitLocationEntries=CombatHitLocationCategories*CombatHitLocationRollOutcomes,
    CombatMissileClusterColumns=7, CombatMissileClusterEntries=11*CombatMissileClusterColumns,
    CombatMissileClusterIndexBias=16 };
extern const uint8_t CombatHitCategoryByFacingDifference[CombatHitFacingEntries]; /*3EDB:2D0A*/
extern const uint8_t CombatTargetMovementPenalty[CombatMovementPenaltyEntries]; /*3EDB:2D1A*/
extern const uint8_t CombatMechHitLocationOffsets[CombatHitLocationEntries]; /*3EDB:2E42*/
extern const uint8_t CombatMissileClusterTable[CombatMissileClusterEntries]; /*3EDB:2E6E*/
/* Exploration NPC slots and combat actor visibility overlap at native42F6. */
#define RoamingNpcVisibleOnScreen (*(uint8_t (*)[MapCharacterCount])CombatantVisibleOnScreen)
#define HoldRickAtlasUntilLoungeConversation PersistentState.bytes[45] /*D339*/
extern uint16_t AnimatedMapTileFrame;
extern uint8_t AnimatedMapTileId[AnimatedMapTileCount];
uint8_t Advance_Combatant_Animation_Stream(uint16_t combatantId);
void EGA_Upload_Animated_Tile(uint8_t *source,uint16_t destinationOffset);
void Movement_Select_Next_Step(uint16_t actorId,uint16_t destinationX,uint16_t destinationY,
    uint16_t screenX,uint16_t screenY,uint16_t alternateDirectionSearch);
extern uint8_t ViewedHolodisk,CurrentMap;
extern uint16_t TransmittedCacheFound,DrawJailMissionParkedMechs,TraitorWarning;
void Graphics_Set_Screen_To_Black(void); /*207F:1FBE hardware boundary*/
void Draw_Infantry_And_Mechs(void); /*0800:051B*/
void Draw_Persistent_Map_Effects(void);
/* Native0377 renderer: flattened runtime sprite pointer, explicit destination
 * aperture segment/offset. Verified native callers load DX=AC00 on entry. */
void Main_Game_Loop(uint16_t attractMode);
void Load_Game(void);
void Copy_Strided_Word_Rows(uint8_t *source,uint8_t *destination,
    uint16_t wordsPerRow,uint16_t rows,uint16_t sourceRowGap);
void Capture_Combat_Sprite(uint16_t spriteId,uint16_t column,uint16_t row,uint16_t width,uint16_t height);
typedef struct EgaMemoryAddress { uint16_t offset,segment; } EgaMemoryAddress;
void DrawCall_EGA_CharacterPos(EgaMemoryAddress destination,uint8_t *sprite,int16_t x,int16_t y);
_Static_assert(sizeof(EgaMemoryAddress)==4,"Native offset/segment address words");
/* Original B78A/B78E scratch is reused for RAM decoder and EGA addresses.
 * Host RAM pointers need host width; these working unions are not serialized. */
typedef union GraphicsWorkingAddress {
    uint8_t *ram;
    EgaMemoryAddress ega;
} GraphicsWorkingAddress;
extern GraphicsWorkingAddress GraphicsSourceAddress,GraphicsDestinationAddress;
#define GraphicsTransferSource GraphicsSourceAddress.ram
#define GraphicsTransferDestination GraphicsDestinationAddress.ram
extern uint16_t FramebufferBoxColumn,FramebufferBoxRow,FramebufferBoxWidth,FramebufferBoxHeight; /* B792/94/9A/9C */
void EGA_DrawBox_Operation(EgaMemoryAddress source,EgaMemoryAddress destination,
    uint16_t column,uint16_t row,uint16_t width,uint16_t height); /*207F:245C*/
void DrawCall_EGA_DrawBox(void); /*207F:24D7*/
void Format01_Decode(uint8_t *encodedPayload, uint8_t *graphics);
void Format02_Decode(uint8_t *encodedPayload, uint8_t *graphics);
void Decompress_File_Into_Memory(uint8_t *compressed, uint8_t *graphics);
void Display_Text_At(uint8_t *text, uint16_t column, uint16_t row);
uint16_t Check_If_CriticalSlot_Destroyed(uint16_t mechId, uint16_t componentOffset);
enum { PersonnelEnemyMovementPoints=6, PersonnelMovementMinimumBeforeArmour=3,
    PersonnelMovementMaximum=8, PersonnelDexterityMovementNumerator=3,
    PersonnelDexterityMovementDenominator=4, CombatSettingsMenuLayout=3,
    CombatMessageOptionCount=3, CombatMessageMenuVerticalSections=2 };
extern uint16_t CharacterMovementPointsRemaining; /* 3092:3770 */
extern uint16_t CombatMessageVerbosity, CombatDisplayGraphics; /* 3EDB:2E38/2E3A */
#define CombatMessageMenuVerticalLines MenuControls[3].baseRow
#define CombatMessageMenuOptionCount MenuControls[3].optionCount
#define CombatMessageMenuDefaultOption MenuControls[3].selection /*305B:00C2/00C6/00C8*/
uint8_t DrawCall_Combat_Menu(uint16_t column,uint16_t row,uint16_t width,uint16_t colour); /*207F:2B87*/
void Combat_Infantry_Movement(uint16_t combatantId); /* 183B:2474 */
uint16_t SettingsMenu_CombatMessages(void); /* 183B:24F0 */
uint16_t SettingsMenu_SeeCombatGraphics(void); /* 183B:2556 */
enum { CombatMessage_None=0, CriticalFirstMultipleHitRoll=8,
    MechSensorMaximumHits=2,
    MechActuatorLegMask=0x0F, MechActuatorArmMask=0xF0,
    MECH_Offset_CurrentActuators_Left=0x24, MECH_Offset_CurrentActuators_Right=0x25,
    MechStructureFirstOffset=0x1C, CriticalInitialSelectionMask=3 };
extern uint8_t CriticalLowActuatorHitMask[4], CriticalLowActuatorClearMask[4];
extern uint8_t CriticalHighActuatorHitMask[4], CriticalHighActuatorClearMask[4];
extern uint8_t CriticalSectionStart[MechStructureLocationCount], CriticalSectionCount[MechStructureLocationCount];
extern uint16_t MechDestroyedFlag, CombatNotificationLatch; /* 3092:E484/4586 */
void Combat_Critical_Mech_Damage(uint16_t mechId, uint16_t structureOffset); /* 1631:11AB */
uint16_t Mech_Count_Intact_Criticals(uint8_t *components, int16_t count); /* 1631:15FA */
uint16_t Mech_Destroy_One_Intact_Critical(uint8_t *components, int16_t count); /* 1631:163E */
uint16_t Mech_Count_Missing_Low_Actuator_Bits(uint16_t mechId, uint16_t recordOffset); /* 1631:1B44 */
void Combat_CombatMessageVerbosityFilter(uint8_t *message); /* 1631:1DAB */
typedef struct Weapon {
    uint8_t name[11];
    uint8_t damage, attackCountOrClusterColumn, heat, rangeBracket, maximumRange, skillType;
} Weapon;
enum { WeaponRecordCount=33 };
_Static_assert(sizeof(Weapon)==17, "Native weapon table record size");
extern Weapon WeaponStats[WeaponRecordCount]; /* 3EDB:2ED8 */
enum { CombatWeaponTargetSlots=12, CombatKickTargetSlot=11, WeaponIndex_Kick=32,
    WeaponInfantryAttackFlag=0x80, WeaponMediumRangeMask=0x1F,
    WeaponShortRangeShift=5, MechToPersonnelRangeScale=3,
    RangeBracket_Short=0, RangeBracket_Medium=1, RangeBracket_Long=2,
    RangeBracket_OutOfRange=3, MechFootprintRangeCorrectionThreshold=3,
    CombatTargetChoice_Target=0, CombatTargetChoice_Next=1, CombatTargetChoice_Cancel=2,
    CombatMechMenuKickOption=4 };
extern uint8_t CombatWeaponTarget[AllCombatantCount*CombatWeaponTargetSlots]; /*3092:3800*/
extern uint16_t EnemyTargetId; /*3EDB:2B20*/
extern uint8_t *WeaponRangeText[RangeBracket_OutOfRange+1]; /*3EDB:2EBC*/
uint16_t Combat_Packed_Distance_From_Map_Position(uint16_t packedX,uint16_t packedY);
uint16_t Combat_Calculate_RangeBracket(uint16_t targetId,uint16_t weaponId);
void Combat_Select_Weapon_Target_UI(uint16_t attackerId,uint16_t weaponId,uint16_t weaponSlot);
void Combat_Kick_Target(uint16_t mechId);
extern uint16_t DisableComputerControl; /*3092:0090*/
enum { CombatMechPlanningChoiceCount=10, CombatPersonnelPlanningChoiceCount=8,
    CombatMechChoice_Weapons=3, CombatMechChoice_Computer=5, CombatMechChoice_Scan=6,
    CombatPersonnelChoice_Move=0, CombatPersonnelChoice_ClearMoves=1,
    CombatPersonnelChoice_Weapon=2, CombatPersonnelChoice_Computer=3, CombatPersonnelChoice_Scan=4,
    CombatPlanningBeginOffset=1, CombatPlanningFleeOffset=2, CombatPlanningNextUnitOffset=3 };
uint16_t Combat_UI_Menu_Logic(void); /*183B:14C3*/
void Combat_Computer_Control(uint16_t combatantId,uint16_t planningPreview); /*1631:03AB*/
void Combat_Plan_Computer_Side(uint16_t firstCombatantId); /*183B:1482*/
void Combat_Weapon_UI(uint16_t attackerId); /*1543:0004*/
void Combat_Weapon_Display_Text_And_Menu_Options(uint8_t *message); /*1631:1057*/
uint16_t Combat_Get_Weapon_Index_For_Ordinal(uint16_t mechId,uint16_t weaponOrdinal); /*1631:10A2*/
enum { CombatWeaponOrdinalUnavailable=255, MechCurrentAmmoFirstOffset=0x27 };
extern int8_t CombatWeaponHeat[MechRecordCount]; /*3092:0092*/
extern uint8_t MechInfernoRoundsRemaining[MechRecordCount]; /*3092:D576*/
enum { MechEngineHitHeatPerRound=5, MechJumpMinimumHeat=3,
    MechInfernoHeatPerRound=6, MechTerrainCoolingHeatPerRound=4,
    CombatCoolingTerrainTileEnd=16 };
void Combat_Mech_HeatLevels(uint8_t *successfulMovementSteps); /*1631:0C63*/
extern uint16_t ShowArmShotOffAnimation; /*3092:3986*/
void Combat_Mech_Eject(uint16_t mechCombatantId);
enum { CombatArmourLocationCount=11, CombatMissFireRollLow=5,CombatMissFireRollHigh=9,
    CombatMissFireSprite=0x7C };
extern int8_t CombatArmourLocationOffsets[CombatArmourLocationCount]; /*3EDB:3242*/
extern uint8_t *ArmourLocationText[CombatArmourLocationCount]; /*3EDB:324E*/
void Combat_DisplayText_ArmourHitLocation(uint16_t armourOffset); /*1631:1B8F*/
void Combat_Random_CreateFire(uint16_t entityId,uint16_t offsetX,uint16_t offsetY); /*1AE8:1E46*/
void Combat_Clear_Movement_Plans(void); /*1467:0D7E*/
extern uint16_t FriendlyPersonnelWithdrawal,EnemyPersonnelFlightPossible; /*3092:3992/374C; broader parent roles pending*/
enum { CombatAiWithdrawalCells=6,CombatAiShortRangePackedMask=0xE0,
    CombatAiApproachPackedRangeShift=3 };
enum { CombatEncounterProbeMaximumSteps=30 };
void Combat_Load_9Grid_Map(void);
uint16_t Combat_Assess_FirstEnemy_Reachability(void);
void Combat_Character_Pos_Grid(uint16_t packedX,uint16_t packedY); /*183B:2AA3*/
extern uint8_t MissileAmmoPriceByComponent[7]; /* 3EDB:2060 */
enum { MechAmmoMachineGunRoundCostCBills=2 };
void Mechlube_Buy_Ammo(void); /* 11B8:1762 */
uint32_t Prompt_For_Unsigned_Decimal(void);
enum { NumericEntryMaximumDigits=7,NumericEntryCellPixels=8,
    NumericEntryEnter=13,NumericEntryEscape=27,NumericEntryBackspace=8,EGA_Blue=1 };
extern uint16_t GraphicsAdapter; /*3EDB:4FBA*/
extern uint8_t *UnrepairableMechComponentText[3]; /* 3EDB:1D12 */
enum {
    MECH_ComponentBlock_Start=0x33, MECH_ComponentBlock_End=0x55,
    MECH_Offset_Engine=0x75, MECH_Offset_Sensors=0x77,
    Component_Destroyed=0x80, MechComponentIdMask=0x7F,
    Mech_Small_Laser=0x10, Mech_Med_Laser=0x11, Mech_MachineGun=0x18, Mech_LRMissile5=0x1A,
    Mech_SRMissile2=0x1E, Mech_SRMissile6=0x20,
    Heat_Sink=0x22, Destroyed_Heat_Sink=0xA2,
    MechActuatorSide_Left=0, MechActuatorSide_Right=1,
    MechRepairArmourPointCostCBills=4, MechRepairStructurePointCostCBills=9,
    MechRepairHeatSinkCostCBills=800, MechRepairWeaponSelectionCostCBills=300,
    MechRepairActuatorsCostCBills=200, MechRepairWeaponScratchCount=20,
    MechRepairWeaponTypeCount=17, MechRepairWeaponContinuationTypeCount=16,
    MechComponentToWeaponRecordBias=1,
    MechRepairDamagedActuators_Left=1, MechRepairDamagedActuators_Right=2,
    CitadelDialog_DisplayCBillBalance=0x0A,
    MechRepairWeaponMenuFirstRow=2, MechRepairCBillDisplayRow=22,
    MechRepairWeaponResultDisplayRow=18, MechRepairWeaponMenuBottomRow=23
};
void Mechlube_Repair_Mech(void); /* 11B8:0002 */
enum {
    MECH_LOCUST_LEVEL1=0, MECH_WASP_LEVEL1=1, MECH_STINGER_LEVEL1=2, MECH_COMMANDO_LEVEL1=3,
    MECH_LOCUST_LEVEL2=4, MECH_WASP_LEVEL2=5, MECH_STINGER_LEVEL2=6, MECH_COMMANDO_LEVEL2=7,
    MECH_UPGRADE_UNSUPPORTED=8, MECH_UPGRADE_UNSUPPORTED_PSEUDO_COST=0x0D0D,
    MechUpgrade_ChassisCount=4, MechUpgrade_PackageCount=8,
    MechUpgrade_StageOneInstalled=1, MechUpgrade_StageTwoInstalled=2,
    MechUpgrade_BothStagesInstalled=3, MECH_REF_Locust=0,
    Ammo_Laser_Infinite=255, JumpJets_Removed=0
};
extern Mech MechRefs[MechRecordCount]; /* 2FE8:02F0 */
void Generate_Random_Encounter_Enemies(void); /* 0DAB:0D3D */
extern int16_t MechUpgradeCost; /* 3092:0076 */
extern uint16_t MechUpgradeCostByPackage[MechUpgrade_PackageCount];
extern uint8_t CommandoStageOneArmour[MechArmourLocationCount];
void Mechlube_Modify_Mech(void); /* 11B8:080A */
void Mechlube_Upgrade_Mech(void); /* 11B8:0925 */
uint16_t Display_Text_Mech_Names(void);
enum { PartyMechSelectionMenu=23 };
#define PartyMechNamesHidden PersistentState.bytes[24] /*3092:D324*/
void Display_Text_Shop_Cannot_Afford_Text(void);
void Display_Text_Dynamic_Value(uint16_t value);
enum { NativeWordBits=16, NumericRadix_Decimal=10,
    NumericLetterDigitAdjustment='a'-'0'-10, EGA_BrightGreen=10 };
uint8_t *ASM_Text_Formatting(uint16_t value, uint8_t *text, uint16_t radix);
uint8_t *CBill_Text_Formatting(uint32_t value, uint8_t *text, uint16_t radix);
int16_t Native_Abs_Word(int16_t value);
void Display_Text_CBill_Balance(void);
uint16_t Display_Menu_Choices_And_Check(uint16_t bottomRow);
extern uint8_t *CharacterNames[CharacterNameCount]; /* 3EDB:01CA */
extern uint8_t *MedicalSkillText[4], *MedicalEquipmentText[4];
extern uint8_t HealingDice[8][4]; /* 3EDB:2602 */
enum { CitadelDialog_QueryPartyInjuries=0x17 };
void Citadel_Building_Dialogs(uint16_t action);
enum CitadelAction {
    CitadelAction_StartTraining=1,CitadelAction_TrainingDebrief,CitadelAction_LeaveTraining,
    CitadelAction_ShowTranscript,CitadelAction_BuySchoolTraining,CitadelAction_ShowAccounts,
    CitadelAction_InvestStock,CitadelAction_SellStock,CitadelAction_BuyArmour,
    CitadelAction_ShowCash,CitadelAction_BuyWeapon,CitadelAction_TalkToOccupants,
    CitadelAction_SelectPartyMember,CitadelAction_CountCompanions,CitadelAction_BuyTechTraining,
    CitadelAction_QueryTechTraining,CitadelAction_CountVisibleMechs,CitadelAction_RepairMech,
    CitadelAction_ModifyMech,CitadelAction_UpgradeMech,CitadelAction_BuyMedicalTraining,
    CitadelAction_QueryMedicalTraining,CitadelAction_QueryInjuries,CitadelAction_TreatParty,
    CitadelAction_GrantMedkit,CitadelAction_GrantSurgeryKit,CitadelAction_RebuildMechSprites,
    CitadelAction_HidePartyMechs,CitadelAction_CountStoredMechs,CitadelAction_RecruitRex,
    CitadelAction_QueryArmourType,CitadelAction_RepairArmour,CitadelAction_RentArenaLocust,
    CitadelAction_StageOwnedArenaMech,CitadelAction_RunArena,CitadelAction_ShowTraitorName,
    CitadelAction_KillJason,CitadelAction_ShowRecruitName,CitadelAction_ResetCrewAssignments,
    CitadelAction_Jailbreak,CitadelAction_GrantLasers,CitadelAction_StageAllNpcs,
    CitadelAction_StageFirstNpc,CitadelAction_BuyMechAmmo,CitadelAction_ShowEnding,
    CitadelAction_RestorePartyMechs,CitadelAction_DeductJasonHealth
};
enum { SchoolWeaponSkillCount=3,SchoolScorePerSkillLevel=125,SchoolScoreBase=75,
    SpecialistTrainingCost=500,CharacterTraining_Tech=1,CharacterTraining_Medical=2,
    StockSelectionMenu=7,AccountBalanceRow=23,AccountRightColumn=39,
    TrainingMarchStartX=0x0C3C,TrainingMarchEndX=0x0C40,TrainingMarchY=0xC04F,
    TrainingReturnX=0x0C2E,TrainingReturnY=0xC076,TrainingMarchRedraws=6,
    TrainingMarchCalibrationThreshold=8,TrainingMarchRetraces=3,MechRef_Chameleon=4,
    WeaponShopFirearmOffset=6,WeaponShopHeavyOffset=9,ShopWeaponPriceCount=10,
    MedicalServiceFeeCount=8,HospitalFacilitiesFee=25,HospitalFacilitiesRefund=HospitalFacilitiesFee,
    InfantryLaserRifle=13,ScenarioHealthCapMinimumPerBody=6,
    ScriptHealthDeductionThreshold=5,ScriptHealthDeduction=4,
    ArenaEntryX=0x0A2B,ArenaEntryY=0x8057,ArenaReturnX=0x0A39,ArenaReturnY=0x805D,
    ArenaEscapeX=0x0970,ArenaEscapeRegion=0x89,ArenaEscapeFirstRegion=0x78,
    ArenaEffectPage=0x8A,StoryFireFamilyMask=0x7E,StoryFireFirstFrame=0x7C,
    NpcAllStagingWaypoints=0x77,NpcFirstStagingWaypoints=0x70,NpcStagingDelay=255 };
#define SelectedTrainingMech PersistentState.bytes[2] /*D30E*/
#define SelectedSchoolSkill PersistentState.bytes[8] /*D314*/
#define SchoolTrainingPurchased PersistentState.bytes[9] /*D315*/
#define ShopPaymentSuccessful PersistentState.bytes[11] /*D317*/
#define WeaponShopCategoryOffset PersistentState.bytes[12] /*D318*/
#define SelectedSpecialistTrainingMask PersistentState.bytes[15] /*D31B*/
#define SelectedMedicalServiceTier PersistentState.bytes[26] /*D326*/
#define SelectedPartyArmourType PersistentState.bytes[31] /*D32B*/
#define ArenaEscapeAllowed PersistentState.bytes[35] /*D32F*/
#define ArenaWon PersistentState.bytes[33] /*D32D*/
extern uint16_t StockHoldingCount,CpuTimingCalibration; /*3092:E482/3FF4*/
extern uint8_t *StockMarketNames[StockCompanyCount],*SchoolSkillDescriptions[5],*SchoolWeaponSkillText[3];
extern uint32_t ArmourPurchaseCost[ArmourTypeCount-1],InfantryWeaponPurchaseCost[ShopWeaponPriceCount];
extern uint8_t ArmourRepairCostPerPoint[ArmourTypeCount-1],EndingEgaPalette[EgaPaletteRegisterCount];
extern int16_t MedicalServiceFee[MedicalServiceFeeCount];
void Arena_Select_And_Apply_Combat_Map_Patch(void);
void Arena_Remove_And_Randomize_Combat_Map_Patch(void);
extern uint16_t ArenaCombatMapPatchVariant; /*246C:0010, NOT3092:0010*/
enum { ArenaPatchVariantMask=3,ArenaPatchHorizontalTile=0x6A,ArenaPatchVerticalTile=0x6B,
    ArenaPatchStartTile=0x6F,ArenaPatchEndTile=0x70,ArenaPatchSparseStartTile=0x71,
    ArenaGroundFirstTile=0x40,ArenaGroundVariantMask=3,
    ArenaPatchOneStart=0x0C72,ArenaPatchOneInterior=0x0C73,ArenaPatchOneLength=5,ArenaPatchOneEnd=0x0CB0,
    ArenaPatchTwoStart=0x0A5B,ArenaPatchTwoFirstInterior=0x0A5C,ArenaPatchTwoFirstLength=4,
    ArenaPatchTwoSecondInterior=0x0A98,ArenaPatchTwoSecondLength=6,ArenaPatchTwoEnd=0x0A9E,
    ArenaPatchThreeStart=0x0ABC,ArenaPatchThreeFirstInterior=0x0C84,
    ArenaPatchThreeEnd=0x0CBC,ArenaPatchThreeStride=8 };
void Heal_Characters(uint16_t medicalServiceTier); /* original1431:000A */
uint16_t RollD6(void);
uint16_t Roll2D6(void);
uint8_t Rand_0x00_to_0xFF(void);
uint16_t Prompt_Yes_No(uint16_t defaultYes);
uint16_t Keyboard_Convert_To_MoveCommands(uint16_t key);
enum {
    COMMAND_MOVE_North=0xFFB8, COMMAND_MOVE_East=0xFFB3,
    COMMAND_MOVE_South=0xFFB0, COMMAND_MOVE_West=0xFFB5,
    COMMAND_MOVE_NorthEast=0xFFB7, COMMAND_MOVE_NorthWest=0xFFB9,
    COMMAND_MOVE_SouthEast=0xFFAF, COMMAND_MOVE_SouthWest=0xFFB1
};
extern uint8_t RandomByteLow, RandomByteMiddle, RandomByteHigh;
extern uint8_t RandomStateUpperByte; /* 3EDB:4FC3; not advanced by0BC0 */
enum { CharacterSkillCount=7, CharacterSkill_Piloting=4,
    CharacterSkill_Tech=5, CharacterSkill_Medical=6,
    RecruitFirstReusableNameId=2, RecruitNameCycleLimit=10,
    RecruitNameSeedMultiplier=391, RecruitPilotInjuryPerBodyPoint=2,
    TraitorInitialBattleProbability=31, MECH_NoRider=255,
    SavedBuildingPositionCount=16 };
extern uint16_t DisableInput; /* 3092:3938 */
extern uint16_t EnteredBuildingId; /* 3092:4584 */
extern uint16_t SavedBuildingMapPositionX[SavedBuildingPositionCount]; /* 3092:39B4 */
extern uint16_t SavedBuildingMapPositionY[SavedBuildingPositionCount]; /* 3092:39D4 */
/* Party-position WORDs:246C:A44B/A44D, MapRuntime storage above. */
void Recruit_Crescent_Hawk_Agent(uint16_t specialtySkillIndex); /* 11B8:0D58 */
uint16_t Pending_Input(void); /* 1F3D:002F */
uint16_t Check_Input_For_Character(void); /* 207F:3BDC, OS redirect */
extern uint16_t AttractModeReplayIndex, AttractModeRecordingActive, ExitMainLoop; /* 39F8/458C/3EDB:0152 */
enum { AttractModeDelayToken='H', AttractModeExitToken='P',
    AttractModeDelayRetraces=30, InputPauseRetraces=50 };

/* Temporary original UI contracts. Gameplay is real; their graphical bodies
 * will be converted separately. Tests supply deterministic headless versions. */
void Draw_Message_Box(void);
void Display_Text_From_Memory(uint8_t *text);
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text);
uint16_t Keyboard_Get_ASCII_Hex_Input(void);
void Drain_Pending_Keyboard_Input(void);
void Menu_Memory_Variables(uint16_t layout);
void Draw_Menu_Border(uint16_t layout);
void Draw_Top_Graphic_Sidebar(void);
void Set_Text_Colour_Bright_Green(void);
void Display_Text_4FA0_Value(void);
void Wait_For_50Hz_Then_Check_Input(void);
void Prompt_And_Wait_For_Key(void);
void Display_Plural_Suffix(void);
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh);
void Draw_BDC_Attribute_Bar(int16_t value,uint16_t column,uint16_t pixelY);
void Draw_Character_BDC_Sidebar_Row(uint16_t characterId,uint16_t row);
enum { CacheEntranceCameraX=0x0C06,CacheEntranceCameraY=0xC07E,
    CacheEntranceDoorX=0x0C03,CacheEntranceDoorY=0x097D,
    CacheEntranceRetraces=60,Bld_CacheInstructions=23,
    CachePassageTileOffset=0x0A10,CachePassageRows=6,CachePassageColumns=3 };
extern uint8_t CacheEntryMechNameInitial[LanceSize]; /*3092:3248*/
void Draw_STARLEAG_ICN_AND_Game_Logic(void); /*135D:0004*/
void StarLeague_Secret_Passageway_Discovered(void); /*135D:01E9*/
enum { CacheOverviewColourSwapCount=250,OverviewColourByteDomain=256,
    OverviewColoursOffset=0x215D-0x0C1D,
    CacheOverviewColoursOffset=0x2257-0x0C1D,
    MapRoomOverviewColoursOffset=0x2351-0x0C1D };
#define OverviewTileColours (MapTemplateData+OverviewColoursOffset)
#define CacheOverviewTileColours (MapTemplateData+CacheOverviewColoursOffset)
#define MapRoomOverviewTileColours (MapTemplateData+MapRoomOverviewColoursOffset)
void OverHead_Map_Function(void); /*135D:0327*/
enum { Bld_PlaySound=0xE4,Bld_AddCBills,Bld_SetMapPosition,Bld_BranchIfXEquals,
    Bld_BranchIfRandomMask,Bld_Recruit,Bld_ConditionalScene,Bld_BranchIfSurgeryKit,
    Bld_BranchIfMedkit,Bld_BranchIfPartySkill,Bld_SubtractCBills,Bld_BranchIfCBills,
    Bld_SetTextLayout,Bld_AddStateByte,Bld_TimedWait,Bld_BranchStateTable,
    Bld_SetStateByte,Bld_CallAction,Bld_YesNoBranch,Bld_BranchIfStateNonzero,
    Bld_Branch,Bld_MenuBranchTable,Bld_DrawBorder,Bld_WaitForKey,Bld_DisplayText,
    Bld_RedrawSidebar,Bld_ApplyLayout,Bld_Exit };
void Execute_Bld_Bytecode(uint8_t *payload); /*0FDC:01C0*/
#define BldInteractionFlag PersistentState.bytes[10] /*3092:D316*/
#define HasHoloviewer PersistentState.bytes[48] /*3092:D33C*/
#define HasViewedHolodisk PersistentState.bytes[50] /*3092:D33E*/
enum { Bld_Garage=8,Bld_Viewdisk=18,Bld_BarracksReturn=19,Bld_CacheEntrance=20,
    BuildingEntryAnimationCallerRedraw=1,HolodiskViewedMapMarker=12,
    BuildingExitSidebarLayout=4,BuildingHealthSidebarLayout=3,
    HolodiskPromptLayout=6,CacheEntranceFogFirstRow=96,
    CacheEntranceFogRowCount=8,CacheEntranceFogColumn=12 };
extern uint8_t EnterBuildingAnimations[BldFileCount]; /*3EDB:141A*/
#define NpcActivityBuildingId PersistentState.bytes[13] /*D319*/
#define HoldRickAtlasForConversation PersistentState.bytes[45] /*D339*/
#define RickAtlasConversationTriggered PersistentState.bytes[46] /*D33A*/
enum { NpcActivityReasonCount=11,DestroyedCitadelReplyCount=8,
    NpcBuildingRouteMask=7,NpcCurrentBuildingShift=4,
    NpcLoungeBuildingSlot=7,BuildingOccupantMenuControl=20 };
#define BuildingOccupantMenuOptionCount MenuControls[BuildingOccupantMenuControl].optionCount /*305B:01D6*/
extern uint8_t *NpcActivityReasonText[NpcActivityReasonCount]; /*3EDB:4E32*/
extern uint8_t *DestroyedCitadelNpcReplyText[DestroyedCitadelReplyCount]; /*3EDB:4E5E*/
void Talk_To_Building_Occupants(void); /*0FDC:17B9*/
void Display_No_Stock_Transaction_Text(void); /*0FDC:19E1*/
enum { MissionMechCameraYOffset=2 }; /*native restoration shifts mech0 anchor two cells south*/
void Restore_Mission_Map_View_After_Combat(void); /*0FDC:134B*/
enum { Mission_SoutheastCorner=0,Mission_RubblePickup,Mission_DisabledLocust,
    Mission_InfantryRobots,Mission_LocustWithoutComputer,Mission_TwoLocusts,
    Mission_ThreeLocusts,Mission_KuritaAttack,Mission_Arena,Mission_Jailbreak,
    TrainingRubbleTargetCount=8,TrainingTemporaryTileIndex=0x0FED,
    TrainingRubbleBlockCoordinateMask=0x78, /* local seven-bit coordinate without within-block bits */
    TrainingSoutheastX=0x0C78,TrainingSoutheastY=0xC07C,
    TrainingHangarMaxX=0x0C3C,TrainingHangarMinY=0xC049,TrainingHangarMaxY=0xC04F,
    TrainingRubbleTile=0x50,KuritaAttackTile=0x40,KuritaAttackFirstTile=0x0FF0,
    JailInteractionFirstX=0x0D14,JailInteractionY=0x702D,
    JailEncounterUpdateLimit=80,JailSuccessfulAttemptIndex=2,
    TrainingLocustTimeAllowance=50,TrainingSoutheastPassTime=215,
    MissionTerrainDelayDivisor=8,MissionWorldUpdateCountdown=10,
    MissionNpcUpdatePhaseMask=3,MissionNpcUpdateTriggerPhase=1,
    MissionRobotCountBase=3,MissionRobotCountRandomMask=3,
    MissionArenaDeployment=0x81,MissionJailInfantryDeployment=0x88,
    MissionCombat_Training=1,MissionCombat_Arena=2,MissionCombat_Jailbreak=3 };
#define TrainingMissionPassed PersistentState.bytes[1] /*D30D*/
#define TrainingMechSurvivedKuritaAttack PersistentState.bytes[5] /*D311*/
extern uint16_t KuritaAttackFlag,MissionNpcUpdatePhase; /*3092:3772,3EDB:5802*/
extern int8_t TrainingRubbleTargetX[TrainingRubbleTargetCount],TrainingRubbleTargetY[TrainingRubbleTargetCount]; /*3EDB:1632/163A*/
void Mech_Mission(uint16_t mission);
enum { JailEntranceX=0x0D10,JailEntranceY=0x7024,JailEscapeX=0x0D00,JailEscapeY=0x7014 };
void Run_Jailbreak_Mission_And_Award_Stinger(void);
enum { Infantry_Pistol=7,RexAmbushDataBytes=24,RexAmbushYOffset=8,RexBody=12,RexDexterity=9,RexCharisma=8,
    KuritaPartyAlleyX=0x0A69,KuritaPartyAlleyY=0x805E,
    RexAmbushOriginX=0x0A72,RexAmbushOriginY=0x805D,
    RexAmbushCountMask=3,RexAmbushMinimumRecordEnd=10,RexAmbushWeaponCount=8 };
extern uint8_t RexAmbushData[RexAmbushDataBytes];
void Recruit_Rex_And_Start_KuritaParty_Ambush(void);
void Mission_GenerateEnemies(uint16_t mechDeployment,uint16_t infantryDeployment); /*0FDC:0D49*/
void Play_Failed_Mech_Startup_Scene(void); /*11B8:16B2*/
enum { EnemySpawnTemplateCount=11,EnemySpawnSharedDataBytes=31,
    EnemySpawnYOffset=12,EnemyArmourDamageOffset=24,EnemyDamageLevelMaximum=6,
    EnemyCriticalDamageThreshold=4,EnemySystemDamageThreshold=5,
    EnemyCriticalDamageAttempts=5,EnemyCriticalDamageFirstOffset=0x34,
    EnemyCriticalDamageRandomMask=31,EnemyRandomizedSkillCount=6,
    EnemyInfantryWeaponTypeCount=14,EnemyInitialDirection=6,
    EnemyInitialMechFrame=12,EnemyInitialInfantryFrame=28,
    EnemySpectatorFrame=16,EnemySpectatorDirection=2,
    EnemyTrainingSpawnX=0x0C65,EnemyTrainingSpawnY=0xC059,
    EnemyTrainingSpawnRandomMask=7,EnemyArenaSpawnRandomMask=3,
    EnemyOnFootDeploymentThreshold=128,EnemyChassisDiceMinimum=2,EnemyRentalChassisFirstIndex=3,
    EnemyDisabledLocustSpawnX=0x0C72,EnemyJailSpawnX=0x0D10,EnemyJailSpawnY=0x7024,
    EnemyArenaLeftSpawnX=0x0A10,EnemyArenaRightSpawnX=0x0A28,EnemyArenaSpawnY=0x806F,
    EnemySpectatorX=0x0A06,EnemySpectatorY=0x8066,
    MechRef_Wasp=1,MechRef_Stinger=2,MechRef_Commando=3,
    MechRef_Jenner=5,MechRef_Spectator=6,MechRef_UrbanMech=7,EnemySpectatorStreamOffset=0x4DD8 };
#define EnemyMechDamageLevel PersistentState.bytes[3] /*D30F*/
extern Mech *EnemySpawnTemplates[EnemySpawnTemplateCount]; /*3EDB:13E2*/
extern uint8_t EnemySpawnSpriteFamilies[EnemySpawnTemplateCount]; /*3EDB:140E, CBW at use*/
extern uint8_t EnemySpawnSharedData[EnemySpawnSharedDataBytes]; /*3EDB:1642..1660, overlapping views*/
extern uint8_t EnemySpectatorAnimationStream[3]; /*3EDB:4DD8..4DDA*/
/* The original saves C614..D557 in ONE raw transfer. These are typed views
 * of that storage, not independently serialized copies. D558..D55B are the
 * adjacent unsaved menu rows. No host pointers occur in the native block. */
enum { OriginalSavedStateBytes=0x0F44,OriginalSavedStorageBytes=OriginalSavedStateBytes+LanceSize,
    OriginalNpcEffectsOverlapBytes=9,OriginalUnclassifiedEconomyBytes=16 };
typedef union OriginalSavedStateStorage {
    uint8_t bytes[OriginalSavedStorageBytes];
    struct {
        Character characters[CharacterRecordCount];
        WorldMapStateStorage world;
        uint32_t cbills,stocks[StockCompanyCount];
        uint8_t unclassifiedEconomy[OriginalUnclassifiedEconomyBytes];
        union {
            RoamingMapNpc npcs[MapCharacterCount];
            struct {
                uint8_t beforeEffects[sizeof(RoamingMapNpc)*MapCharacterCount-OriginalNpcEffectsOverlapBytes];
                uint8_t sprite[PersistentMapEffectSlotCount],page[PersistentMapEffectSlotCount];
                uint8_t x[PersistentMapEffectSlotCount],y[PersistentMapEffectSlotCount];
                uint8_t nextSlot,mechMenuRows[LanceSize];
            } effects;
        } roaming;
    } fields;
} OriginalSavedStateStorage;
_Static_assert(sizeof(OriginalSavedStateStorage)==OriginalSavedStorageBytes,"Native save and adjacent menu extent");
_Static_assert(offsetof(OriginalSavedStateStorage,fields.world)==0xC724-0xC614 &&
    offsetof(OriginalSavedStateStorage,fields.cbills)==0xD370-0xC614 &&
    offsetof(OriginalSavedStateStorage,fields.stocks)==0xD374-0xC614 &&
    offsetof(OriginalSavedStateStorage,fields.roaming.npcs)==0xD390-0xC614,"Native save field offsets");
_Static_assert(offsetof(OriginalSavedStateStorage,fields.roaming.effects.sprite)==0xD457-0xC614 &&
    offsetof(OriginalSavedStateStorage,fields.roaming.effects.nextSlot)==OriginalSavedStateBytes-1 &&
    offsetof(OriginalSavedStateStorage,fields.roaming.effects.mechMenuRows)==OriginalSavedStateBytes,"Native effects/NPC and cursor/menu aliases");
extern OriginalSavedStateStorage OriginalSavedState;
#define Characters OriginalSavedState.fields.characters
#define WorldMapState OriginalSavedState.fields.world
#define CBills OriginalSavedState.fields.cbills
#define StockBalances OriginalSavedState.fields.stocks
#define RoamingMapNpcs OriginalSavedState.fields.roaming.npcs
#define MapEffectSpriteIndex OriginalSavedState.fields.roaming.effects.sprite
#define MapEffectPackedPage OriginalSavedState.fields.roaming.effects.page
#define MapEffectPositionXLow OriginalSavedState.fields.roaming.effects.x
#define MapEffectPositionYLow OriginalSavedState.fields.roaming.effects.y
#define NextMapEffectSlot OriginalSavedState.fields.roaming.effects.nextSlot
#define MechSlotByMenuRow OriginalSavedState.fields.roaming.effects.mechMenuRows
#endif
