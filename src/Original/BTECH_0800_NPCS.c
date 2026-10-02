#include "game.h"

/* Original0800:24C2..2866. Camera is borrowed as boundary scratch and restored
 * only after the entire pass/on an accepted NPC. Rejected slots keep their
 * existing world positions; delayed slots materialize only when delay reaches0. */
void Update_Roaming_Map_Npcs(void)
{
    uint16_t savedCameraX=CrescentHawkMapPositionX,savedCameraY=CrescentHawkMapPositionY;
    for (uint16_t slot=0;slot<MapCharacterCount;++slot) {
        RoamingMapNpc *npc=&RoamingMapNpcs[slot];
        uint16_t actor=Enemy_Infantry_CombatantId_Range_First+slot;
        RoamingNpcVisibleOnScreen[slot]=FALSE;
        if (npc->movementDelay) {
            if (!slot && HoldRickAtlasUntilLoungeConversation) npc->movementDelay=255;
            --npc->movementDelay;
            if (!npc->movementDelay) {
                uint16_t startWaypoint=npc->waypointPair>>4;
                CombatantPackedX[actor]=MapInteractablePositionX[startWaypoint];
                CombatantPackedY[actor]=MapInteractablePositionY[startWaypoint];
            }
            continue;
        }
        CrescentHawkMapPositionX=savedCameraX&PackedPositionCoarseXMask;
        CrescentHawkMapPositionY=savedCameraY&PackedPositionCoarseYMask;
        Offset_Packed_Position(-RoamingNpcBoundarySubregionSize,0);
        uint16_t worldX=CombatantPackedX[actor];
        if (CrescentHawkMapPositionX>=worldX) continue;
        CrescentHawkMapPositionX=savedCameraX|RoamingNpcBoundaryLowNibbleMask;
        Offset_Packed_Position(RoamingNpcBoundarySubregionSize,0);
        if (CrescentHawkMapPositionX<=worldX) continue;
        uint16_t worldY=CombatantPackedY[actor];
        Offset_Packed_Position(0,-RoamingNpcBoundarySubregionSize);
        if (CrescentHawkMapPositionY>=worldY) continue;
        CrescentHawkMapPositionY=savedCameraY|RoamingNpcBoundaryLowNibbleMask;
        Offset_Packed_Position(0,RoamingNpcBoundarySubregionSize);
        if (CrescentHawkMapPositionY<=worldY) continue;
        RoamingNpcVisibleOnScreen[slot]=TRUE;
        CrescentHawkMapPositionX=savedCameraX; CrescentHawkMapPositionY=savedCameraY;
        MovementActorPositionX=worldX; MovementActorPositionY=worldY;
        uint16_t screenX=(uint16_t)(worldX-savedCameraX),screenY=(uint16_t)(worldY-savedCameraY);
        if ((int16_t)screenX<-PackedPositionLocalCarryBit) screenX+=PackedPositionLocalCarryBit;
        if ((int16_t)screenX>PackedPositionLocalCarryBit) screenX-=PackedPositionLocalCarryBit;
        screenX+=MapCameraCentreCellX;
        if ((int16_t)screenY<-PackedPositionSouthCarry) screenY+=PackedPositionSouthCarry;
        if ((int16_t)screenY>PackedPositionSouthCarry) screenY-=PackedPositionSouthCarry;
        screenY+=MapCameraCentreCellY;
        Movement_Select_Next_Step(actor,npc->destinationX,npc->destinationY,screenX,screenY,TRUE);
        CombatantPackedX[actor]=MovementActorPositionX;
        CombatantPackedY[actor]=MovementActorPositionY;
        uint8_t direction=CombatantMovementDirection[actor];
        if (CombatantAnimationDirection[actor]!=direction)
            CombatantAnimationCursors[actor]=FriendlyInfantryWalkAnimationByDirection[(int8_t)direction];
        CombatantSpriteFrame[actor]=Advance_Combatant_Animation_Stream(actor);
        if (CombatantPackedX[actor]==npc->destinationX && CombatantPackedY[actor]==npc->destinationY) {
            npc->movementDelay=Rand_0x00_to_0xFF()&RoamingNpcDelayRandomMask;
            CombatantPackedX[actor]=0; CombatantPackedY[actor]=0;
            uint8_t nextWaypoint=RoamingNpcWaypointLink[npc->waypointPair&CompassDirectionMask];
            npc->waypointPair=(uint8_t)((npc->waypointPair<<4)|nextWaypoint);
            /* Native CBW of link BYTE; shipped waypoint IDs must remain valid. */
            npc->destinationX=MapInteractablePositionX[(int8_t)nextWaypoint];
            npc->destinationY=MapInteractablePositionY[(int8_t)nextWaypoint];
        }
    }
    CrescentHawkMapPositionX=savedCameraX; CrescentHawkMapPositionY=savedCameraY;
}
