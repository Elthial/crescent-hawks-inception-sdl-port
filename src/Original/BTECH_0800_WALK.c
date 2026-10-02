#include "game.h"

/* Original0800:1732..17BA. Unknown negative tokens are consumed; FF waits
 * forever on itself. No invented timeout or end-of-stream validation. */
uint8_t Advance_Combatant_Animation_Stream(uint16_t combatantId)
{
    AnimationCursor *cursor=&CombatantAnimationCursors[combatantId];
    for (;;) {
        int16_t token=(int8_t)cursor->data[(uint16_t)(cursor->offset-cursor->dataOffset)];
        ++cursor->offset;
        if (token>=0) return (uint8_t)token;
        if ((uint8_t)token==AnimationToken_SetDirection) {
            uint8_t selector=cursor->data[(uint16_t)(cursor->offset-cursor->dataOffset)];
            /* Native effect slot24 aliases the LOW BYTE of sound WORD3984. */
            if (combatantId==EffectAnimationCursorId)
                FixedTonePitDivisor=(uint16_t)((FixedTonePitDivisor&0xFF00)|selector);
            else CombatantAnimationDirection[combatantId]=selector;
            ++cursor->offset;
        } else if ((uint8_t)token==AnimationToken_RelativeLoop) {
            int16_t distance=(int8_t)cursor->data[(uint16_t)(cursor->offset-cursor->dataOffset)];
            cursor->offset=(uint16_t)(cursor->offset-distance);
        } else if ((uint8_t)token==AnimationToken_Wait) {
            --cursor->offset;
        }
    }
}

/* Original0800:231D..240A. First matching direction wins; absent infantry
 * are deliberately advanced too, whereas absent mech records are skipped. */
void Update_Friendly_Movement_Animations(uint16_t command)
{
    for (uint16_t direction=0;direction<CompassDirectionCount;++direction) {
        if (MovementCommandByCompass[direction]!=command) continue;
        for (uint16_t mech=0;mech<LanceSize;++mech) {
            if (Mechs[mech].name[0]==MECH_Destroyed) continue;
            if ((int16_t)(int8_t)CombatantAnimationDirection[mech]!=(int16_t)direction)
                CombatantAnimationCursors[mech]=FriendlyMechWalkAnimationByDirection[direction];
            CombatantSpriteFrame[mech]=Advance_Combatant_Animation_Stream(mech);
        }
        for (uint16_t infantry=Friendly_Infantry_Combatant_Range_First;
            infantry<Enemy_All_CombatantId_Range_First;++infantry) {
            if ((int16_t)(int8_t)CombatantAnimationDirection[infantry]!=(int16_t)direction)
                CombatantAnimationCursors[infantry]=FriendlyInfantryWalkAnimationByDirection[direction];
            CombatantSpriteFrame[infantry]=Advance_Combatant_Animation_Stream(infantry);
        }
        break;
    }
}

/* Original0800:240B..24C1 retained EGA. Ten16x16 planar128-byte frames
 * replace contiguous32-byte tile slots in A400. Destination BYTE uses CBW. */
void Update_Animated_Map_Tiles(void)
{
    if (TilesetId!=Tileset_BattleTech) return;
    ++AnimatedMapTileFrame;
    if (AnimatedMapTileFrame>=AnimatedMapFrameCount) AnimatedMapTileFrame=0;
    uint16_t sourceOffset=(uint16_t)(AnimatedMapTileFrame*AnimatedMapFrameBytes);
    for (uint16_t tile=0;tile<AnimatedMapTileCount;++tile) {
        uint16_t destinationOffset=(uint16_t)((uint16_t)(int16_t)(int8_t)AnimatedMapTileId[tile]
            * AnimatedMapTilePlaneBytes);
        EGA_Upload_Animated_Tile(AnimatedMapTileFrames+sourceOffset,destinationOffset);
        sourceOffset+=AnimatedMapFrameCount*AnimatedMapFrameBytes;
    }
}
