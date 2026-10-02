#include "game.h"

/* Sol: Native0800:051B..0E4A EGA path, instruction-checked. Drawing uses original
 * record IDs; formation/active tables use compacted surviving-unit slots. */
void Draw_Infantry_And_Mechs(void)
{
    EgaMemoryAddress Destination = {0, MapViewportSegment};
    // Sol: 0800:051B is the exploration-map sprite compositor.  It draws, in
    // Sol: separate passes, party infantry, map NPC/enemy infantry, lance mechs, and
    // Sol: the special jail sprites, then refreshes their world-position arrays.
    if (InsideStarLeagueCache == FALSE) //bD346 - On StarLeague-Cache Map
        Draw_Persistent_Map_Effects();

    OnFootPartyMemberCount = 0;
    uint16_t FriendlyMechSpriteSlot = 0;
    uint16_t OnFootSpriteSlot = 0;

    // Sol: The assembly clears twelve WORD entries at 3092:406A.  These are the
    // Sol: friendly combatant-active slots (four mechs plus eight infantry), not
    // Sol: merely an eight-element infantry visibility array as BTECH.h suggests.
    for(int FriendlyId = 0; FriendlyId < Enemy_All_CombatantId_Range_First; FriendlyId++)
    {
        CombatantActive[FriendlyId] = FALSE;  //a406A = Visible / On Map?
    }

    for(int PartyMemberId = 0; PartyMemberId < PartySize; PartyMemberId++)
    {
        // Sol: These are byte-value tests in the original, not address tests.
        // Sol: Character +0x0C is MechAssignment: values 0-7 select a mech and
        // Sol: value 8 means the character is on foot.
        if (Characters[PartyMemberId].name != Character_Dead
        && (int8_t)Characters[PartyMemberId].mechAssignment >= Character_OnFoot)
        {
            ++OnFootPartyMemberCount;

            // Sol: The 02AE/02BE tables give screen coordinates for the compacted
            // Sol: on-foot slot; 02FE gives its base index in the visible map window.
            uint16_t ScreenPosX = PartyInfantryScreenX[OnFootSpriteSlot]; // DS:02AE
            uint16_t ScreenPosY = PartyInfantryScreenY[OnFootSpriteSlot]; // DS:02BE
            uint16_t MapCellIndex = PartyInfantryMapCellOffset[OnFootSpriteSlot] + CachedMapOriginIndex; // DS:02FE

            if ((CrescentHawkMapPositionX & 1) != FALSE) //A44B - Character Position X
                MapCellIndex += PartyInfantryOddCameraXOffset[OnFootSpriteSlot]; // DS:030E

            if ((CrescentHawkMapPositionY & 1) != FALSE) //A44D - Character position Y
                MapCellIndex += PartyInfantryOddCameraYOffset[OnFootSpriteSlot]; // DS:031E

            uint16_t MapParity = ((CrescentHawkMapPositionY & 1) << 1)
                + (CrescentHawkMapPositionX & 1);
            uint16_t OcclusionMask = PartyInfantryOcclusionMask[OnFootSpriteSlot][MapParity]; // DS:0332
            uint16_t MapTile = CombatMap[MapCellIndex];
            uint8_t OverlapRows = 0;

            if (InsideStarLeagueCache == FALSE //bD346 - On StarLeague-Cache Map
            && (MapTile < TerrainOcclusionSpecialTileFirst
            && ((MapTile & OcclusionMask) != 0
            && (MapTile & TerrainOcclusionCategoryMask) < TerrainOcclusionCategoryLimit)))
            {
                // Sol: The map tile can hide the bottom two or four pixel rows of
                // Sol: the infantry sprite, producing the behind-wall/terrain effect.
                OverlapRows = InfantryTerrainOverlapRows;
                if ((MapTile & TerrainOcclusionCategoryMask) == TerrainOcclusionTallCategory)
                    OverlapRows = InfantryTallTerrainOverlapRows;
            }

            TerrainOverlapRows[Friendly_Infantry_Combatant_Range_First + PartyMemberId] = OverlapRows; //3092:32B2
            // Sol: DS:409E/D562 are the friendly-infantry slices of 409A/D55E.
            uint16_t SpritePointerIndex = CombatantSpriteFrame[
                Friendly_Infantry_Combatant_Range_First + PartyMemberId]
                + CombatantSpriteFamilyOffset[
                    Friendly_Infantry_Combatant_Range_First + PartyMemberId];
            /* Original inline temporary header edit, undone after the draw. */
            uint8_t *Sprite = CombatSpritePointers[SpritePointerIndex];
            Sprite[SpriteHeightMinusOneByte] -= OverlapRows;
            DrawCall_EGA_CharacterPos(Destination, Sprite, (int16_t)ScreenPosX, (int16_t)ScreenPosY);
            Sprite[SpriteHeightMinusOneByte] += OverlapRows;

            OnFootSpriteSlot++;
        }
    }
    // Sol: 0800:074E begins the map-NPC/enemy-infantry pass (combatant IDs
    // Sol: 16-23). These slots cover the grey civilian/hostile human sprites.
    // Sol: World coordinates use the game's packed 16-bit representation, so
    // Sol: the signed bounds below must retain their exact bit patterns rather
    // Sol: than become host integers.
    uint16_t CameraPosX = CrescentHawkMapPositionX; //A44B
    uint16_t CameraPosY = CrescentHawkMapPositionY; //A44D

    uint16_t EnemyInfantryId = Enemy_Infantry_CombatantId_Range_First;
    while (EnemyInfantryId < AllCombatantCount)
    {
        // Sol: Native D1F9 + combatantId*26 addresses NPC movementDelay.
        if (RoamingMapNpcs[EnemyInfantryId - Enemy_Infantry_CombatantId_Range_First].movementDelay == 0)
        {
            uint16_t EnemyWorldX = CombatantPackedX[EnemyInfantryId]; //4004
            uint16_t EnemyWorldY = CombatantPackedY[EnemyInfantryId]; //4036
            int16_t EncodedScreenX = (int16_t)(uint16_t)(EnemyWorldX - CameraPosX + MapCameraCentreCellX);
            int16_t EncodedScreenY = (int16_t)(uint16_t)(EnemyWorldY - CameraPosY + MapCameraCentreCellY);
            uint16_t RejectXWrap = FALSE;
            uint16_t RejectYWrap = FALSE;

            // Sol: Comparing only the high bytes detects a false low-byte wrap in
            // Sol: packed coordinates. Same-page values must also be in the normal
            // Sol: visible cell ranges X=13..39 and Y=0..24.
            if ((EnemyWorldX & 0xFF00) == (CameraPosX & 0xFF00)
            && (EncodedScreenX < MapViewportLeftByte || EncodedScreenX > MapViewportLastCellX))
                RejectXWrap = TRUE;

            if ((EnemyWorldY & 0xFF00) == (CameraPosY & 0xFF00)
            && (EncodedScreenY < 0 || EncodedScreenY > MapViewportLastCellY))
                RejectYWrap = TRUE;
            if (EncodedScreenX >= MapProjectionMinimumX
            && EncodedScreenX <= MapProjectionMaximumX
            && EncodedScreenY >= MapProjectionMinimumY
            && EncodedScreenY <= MapProjectionMaximumY
            && RejectXWrap + RejectYWrap == FALSE)
            {
                uint16_t ScreenCellX = ((uint16_t)EncodedScreenX & PackedPositionLocalMask);
                uint16_t ScreenCellY = ((uint16_t)EncodedScreenY & PackedPositionLocalMask);
                int16_t MapRelativeX = (int16_t)(uint16_t)(ScreenCellX - MapViewportLeftByte);
                /* Native SAR rounds negative odd offsets DOWN, unlike C division. */
                int16_t HalfMapRelativeX = (int16_t)(MapRelativeX >= 0 ? MapRelativeX / 2
                    : -((-(int32_t)MapRelativeX + 1) / 2));
                uint16_t MapCellIndex = (uint16_t)((ScreenCellY / 2) * MapCacheWidth
                    + HalfMapRelativeX + CachedMapOriginIndex);

                if ((MapRelativeX & 1) != FALSE
                && (CameraPosX & 1) != FALSE)
                    MapCellIndex += 1;

                if ((ScreenCellY & 1) != FALSE
                && (CameraPosY & 1) != FALSE)
                    MapCellIndex += MapCacheWidth;

                uint16_t MapParity = ((ScreenCellY ^ CameraPosY) & 1) << 1;
                MapParity += (MapRelativeX ^ CameraPosX) & 1;
                uint8_t OcclusionMask = MapNpcOcclusionMaskByParity[MapParity]; // DS:032E
                uint8_t MapTile = CombatMap[MapCellIndex];
                uint8_t OverlapRows = 0;

                if (InsideStarLeagueCache == FALSE //bD346 - On StarLeague-Cache Map
                && (MapTile < TerrainOcclusionSpecialTileFirst
                && ((MapTile & OcclusionMask) != 0
                && (MapTile & TerrainOcclusionCategoryMask) < TerrainOcclusionCategoryLimit)))
                {
                    OverlapRows = InfantryTerrainOverlapRows;
                    if ((MapTile & TerrainOcclusionCategoryMask) == TerrainOcclusionTallCategory)
                        OverlapRows = InfantryTallTerrainOverlapRows;
                }

                uint16_t SpritePointerIndex = CombatantSpriteFrame[EnemyInfantryId]
                    + CombatantSpriteFamilyOffset[EnemyInfantryId];
                uint8_t *Sprite = CombatSpritePointers[SpritePointerIndex];
                Sprite[SpriteHeightMinusOneByte] -= OverlapRows;
                DrawCall_EGA_CharacterPos(Destination, Sprite,
                    (int16_t)(ScreenCellX * PixelsPerMapCell), (int16_t)(ScreenCellY * PixelsPerMapCell));
                Sprite[SpriteHeightMinusOneByte] += OverlapRows;
            }
        }
        EnemyInfantryId++;
    }

    // Sol: 0800:09FE begins the friendly lance-mech pass. Destroyed/empty mech
    // Sol: records are skipped, and the surviving mechs are compacted into the
    // Sol: fixed screen/map lookup tables using FriendlyMechSpriteSlot.
    for(int LanceMechId = 0; LanceMechId < LanceSize; LanceMechId++)
    {
        // Sol: C724 + LanceMechId*0x7D is already represented by Mechs[id].
        // Sol: The original compares the first byte of the 16-byte name field.
        if (Mechs[LanceMechId].name[0] != MECH_Destroyed)
        {
            uint16_t ScreenPosX = FriendlyMechScreenX[FriendlyMechSpriteSlot]; // DS:02CE
            uint16_t ScreenPosY = FriendlyMechScreenY[FriendlyMechSpriteSlot]; // DS:02D6
            uint16_t MapCellIndex = FriendlyMechMapCellOffset[FriendlyMechSpriteSlot] + CachedMapOriginIndex; // DS:02DE

            if ((CrescentHawkMapPositionX & 1) != FALSE) //A44B - Character Position X
                MapCellIndex += FriendlyMechOddCameraXOffset[FriendlyMechSpriteSlot]; // DS:02EE

            uint16_t OcclusionMask = FriendlyMechOcclusionMask[FriendlyMechSpriteSlot]; // DS:02E6


            if ((CrescentHawkMapPositionY & 1) != FALSE) //A44D - Character position Y
            {
                MapCellIndex += FriendlyMechOddCameraYOffset[FriendlyMechSpriteSlot]; // DS:02F6
                // Sol: Only the low byte is XORed in the assembly. The high byte
                // Sol: of this word mask is preserved.
                OcclusionMask = (OcclusionMask & 0xFF00)
                    | ((OcclusionMask ^ TerrainOcclusionMechOddYMaskToggle) & 0x00FF);
            }

            uint16_t SpritePointerIndex = CombatantSpriteFrame[LanceMechId]
                + CombatantSpriteFamilyOffset[LanceMechId]; //D55E: Locust 0, humanoid mechs 0x92

            uint8_t MapTile = CombatMap[MapCellIndex];

            FriendlyMechSpriteSlot++;

            uint8_t OverlapRows = 0;

            if (InsideStarLeagueCache == FALSE //bD346 - On StarLeague-Cache Map
            && (MapTile < TerrainOcclusionSpecialTileFirst
            && ((MapTile & OcclusionMask) != 0
            && (MapTile & TerrainOcclusionCategoryMask) < TerrainOcclusionCategoryLimit)))
            {
                // Sol: Mechs use eight/sixteen-row terrain overlap rather than the
                // Sol: two/four rows used by human sprites.
                OverlapRows = MechTerrainOverlapRows;
                if (((MapTile & TerrainOcclusionCategoryMask) == TerrainOcclusionTallCategory || (MapTile & TerrainOcclusionCategoryMask) == 0)
                && (MapTile & TerrainOcclusionAllQuadrantsMask) == TerrainOcclusionAllQuadrantsMask)
                    OverlapRows = MechTallTerrainOverlapRows;
            }

            TerrainOverlapRows[LanceMechId] = OverlapRows; //3092:32AE
            /* Original inline temporary header edit, undone after the draw. */
            uint8_t *Sprite = CombatSpritePointers[SpritePointerIndex];
            Sprite[SpriteHeightMinusOneByte] -= OverlapRows;
            DrawCall_EGA_CharacterPos(Destination, Sprite, (int16_t)ScreenPosX, (int16_t)ScreenPosY);
            Sprite[SpriteHeightMinusOneByte] += OverlapRows;
        }
    }

    // Sol: 0800:0BC9 now reverses the display compaction: it rebuilds world
    // Sol: coordinates and active flags for friendly units around the saved
    // Sol: party anchor. The formation tables contain signed byte deltas.
    uint16_t PartyAnchorX = CrescentHawkMapPositionX; //246C:A44B
    uint16_t PartyAnchorY = CrescentHawkMapPositionY; //246C:A44D

    uint16_t FriendlyMechSlot = 0;
    uint16_t OnFootSlot = 0;


    for(int PartyMemberId = 0; PartyMemberId < PartySize; PartyMemberId++)
    {
        if (Characters[PartyMemberId].name != Character_Dead
        && (int8_t)Characters[PartyMemberId].mechAssignment >= Character_OnFoot)
        {
            // Sol: 191B applies signed deltas to A44B/A44D with the game's packed
            // Sol: coordinate wrapping; its inputs are not absolute positions.
            Offset_Packed_Position(
                PartyInfantryFormationDeltaX[OnFootSlot], // DS:3A16
                PartyInfantryFormationDeltaY[OnFootSlot]); // DS:3A1E

            CombatantPackedX[Friendly_Infantry_Combatant_Range_First + OnFootSlot] = CrescentHawkMapPositionX; //3092:400C
            CombatantPackedY[Friendly_Infantry_Combatant_Range_First + OnFootSlot] = CrescentHawkMapPositionY; //3092:403E
            CombatantActive[Friendly_Infantry_Combatant_Range_First + OnFootSlot] = TRUE; //3092:4072

            // Sol: Each formation delta is relative to the same anchor, not the
            // Sol: position generated for the preceding party member.
            CrescentHawkMapPositionX = PartyAnchorX;
            CrescentHawkMapPositionY = PartyAnchorY;
            OnFootSlot++;
        }
    }

    CrescentHawkMapPositionX = PartyAnchorX;
    CrescentHawkMapPositionY = PartyAnchorY;

    // Sol: The mech pass repeats the process for surviving lance records. The
    // Sol: position/active arrays use compacted friendly-mech slots 0..3.
    for(int LanceMechId = 0; LanceMechId < LanceSize; LanceMechId++)
    {
        if (Mechs[LanceMechId].name[0] != MECH_Destroyed)
        {
            Offset_Packed_Position(
                FriendlyMechFormationDeltaX[FriendlyMechSlot], // DS:3A26
                FriendlyMechFormationDeltaY[FriendlyMechSlot]); // DS:3A2A

            CombatantPackedX[FriendlyMechSlot] = CrescentHawkMapPositionX; //3092:4004
            CombatantPackedY[FriendlyMechSlot] = CrescentHawkMapPositionY; //3092:4036
            CombatantActive[FriendlyMechSlot] = TRUE; //3092:406A

            FriendlyMechSlot++;
            CrescentHawkMapPositionX = PartyAnchorX;
            CrescentHawkMapPositionY = PartyAnchorY;
        }
    }

    // Sol: 0800:0D30-0E46 is a Mission09-only overlay. It projects four fixed
    // Sol: world positions (0D13, 0D17, 0D1B, 0D1F):702C and draws sprite-table
    // Sol: entry 0x92. The pointer at 3092:3C42 is exactly
    // Sol: 39FA + (MECH_Sprite_COMMANDO * 4), so the image uses the generic
    // Sol: humanoid/Commando-base artwork. Mission09's interaction code proves
    // Sol: these are the parked jail-courtyard 'Mechs Jason tests after escaping
    // Sol: his cell. The third unique 'Mech attempted starts and is stolen.
    if (DrawJailMissionParkedMechs != FALSE) //3092:398E
    {
        for (int JailMechId = 0; JailMechId < LanceSize; ++JailMechId)
        {
            // Sol: The original performs 16-bit subtraction before adding the map
            // Sol: viewport origins. Reko had moved +1A/+0C below the tests and then
            // Sol: misread the AND instructions as additions of masked constants.
            int16_t ScreenCellX = (int16_t)(
                (JailParkedMechFirstX + JailMechId * JailParkedMechSpacing)
                - CrescentHawkMapPositionX + MapCameraCentreCellX); //246C:A44B
            int16_t ScreenCellY = (int16_t)(
                JailParkedMechY - CrescentHawkMapPositionY + MapCameraCentreCellY); //246C:A44D

            uint16_t OutsideScreenX = (ScreenCellX < MapViewportLeftByte || ScreenCellX > MapViewportLastCellX);
            uint16_t OutsideScreenY = (ScreenCellY < 0 || ScreenCellY > MapViewportLastCellY);

            // Sol: The clean .dis preserves the first bound as 0xFF8D (-115).
            // Sol: The .asm prints only 8Dh, which led the pseudo-C to use +141.
            // Sol: These broad signed guards precede the narrower visible-window test.
            if (ScreenCellX >= MapProjectionMinimumX
            && ScreenCellX <= MapProjectionMaximumX
            && ScreenCellY >= MapProjectionMinimumY
            && ScreenCellY <= MapProjectionMaximumY
            && OutsideScreenX + OutsideScreenY == 0)
            {
                ScreenCellX &= PackedPositionLocalMask;
                ScreenCellY &= PackedPositionLocalMask;

                // Sol: EGA-only call; DS:3C42/3C44 is FAR entry 0x92.
                DrawCall_EGA_CharacterPos(Destination, CombatSpritePointers[MECH_Sprite_COMMANDO],
                    (int16_t)(ScreenCellX * PixelsPerMapCell), (int16_t)(ScreenCellY * PixelsPerMapCell));
            }
        }
    }
}
