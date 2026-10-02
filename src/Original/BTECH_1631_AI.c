#include "game.h"

/* Original1631:03AB..0BB4, complete raw ASM checked. Inline target search,
 * approach/withdrawal and weapon orders remain in their original method.
 * Caller must supply a targetable opponent: native fallback can scan past
 * valid tables when every opponent is absent/excluded. No safe escape added.
 * Approach policy intentionally uses RAW component weapon-table index and
 * fourfold short-range decoding, unlike firing's component-minus-one. */
void Combat_Computer_Control(uint16_t actorId,uint16_t showPlanningPreview)
{
    uint16_t savedMapX=CrescentHawkMapPositionX,savedMapY=CrescentHawkMapPositionY;
    Move_Map_View_To_Packed_Position(CombatantPackedX[actorId],CombatantPackedY[actorId]);
    for(uint16_t byte=0;byte<CombatMovementOrderBytes;++byte)
        CombatMovementOrders[actorId*CombatMovementOrderBytes+byte]=UINT8_MAX;
    uint16_t recordBank=0;
    if((int16_t)actorId>=Enemy_All_CombatantId_Range_First) {
        actorId-=Enemy_All_CombatantId_Range_First; recordBank=Enemy_Mech_Record_First;
    } else CombatantActionState[actorId]=1;
    uint16_t movementMode=MovementMode_Run;
    if((int16_t)actorId>=Friendly_Infantry_Combatant_Range_First || MechHeatLevel[actorId+recordBank]!=0)
        movementMode=MovementMode_Walk;
    if((int16_t)actorId<Friendly_Infantry_Combatant_Range_First) Combat_Mech_Movement((uint16_t)(actorId+recordBank),movementMode);
    else Combat_Infantry_Movement((uint16_t)(actorId+recordBank)); /*Native side-local+4, not restored combatant*/
    uint16_t candidatesRemaining=LanceSize,candidateId=recordBank?0:Enemy_All_CombatantId_Range_First;
    uint16_t targetX=CombatantPosition_Unused,targetY=CombatantPosition_Unused,targetId=AllCombatantCount-1;
    int16_t nearestDistance=INT16_MAX;
    if((int16_t)actorId>=Friendly_Infantry_Combatant_Range_First) {
        candidatesRemaining=PartySize;
        candidateId=recordBank?Friendly_Infantry_Combatant_Range_First:Enemy_Infantry_CombatantId_Range_First;
    }
    while(candidatesRemaining--!=0) {
        if(CombatantActive[candidateId]) {
            uint16_t candidateX=CombatantPackedX[candidateId],candidateY=CombatantPackedY[candidateId];
            uint16_t allowed=TRUE;
            if(ArenaRentalMechMode && candidateId==ArenaRentalPathTargetId) allowed=FALSE;
            if(recordBank && TraitorInParty && (uint16_t)((int16_t)(int8_t)TraitorCharacterId+Friendly_Infantry_Combatant_Range_First)==candidateId)
                allowed=FALSE;
            if(allowed && (int16_t)Combat_Packed_Distance_From_Map_Position(candidateX,candidateY)<nearestDistance) {
                nearestDistance=(int16_t)Combat_Packed_Distance_From_Map_Position(candidateX,candidateY); /*Native second call*/
                targetX=candidateX; targetY=candidateY; targetId=candidateId;
            }
        }
        ++candidateId;
        if(!candidatesRemaining && targetX==CombatantPosition_Unused) {
            if(candidateId==Enemy_All_CombatantId_Range_First || candidateId==AllCombatantCount) candidateId-=Enemy_All_CombatantId_Range_First;
            candidatesRemaining=PartySize;
        }
    }
    if(recordBank) actorId+=Enemy_All_CombatantId_Range_First;
    uint16_t approachDistance;
    if((int16_t)actorId<Enemy_Infantry_CombatantId_Range_First &&
       ((int16_t)actorId<Friendly_Infantry_Combatant_Range_First || (int16_t)actorId>=Enemy_All_CombatantId_Range_First)) {
        uint16_t minimumPackedShortRange=UINT8_MAX,recordId=actorId;
        if(recordBank) recordId-=Enemy_Infantry_Record_First;
        for(uint16_t offset=MECH_ComponentBlock_Start;offset<=MECH_ComponentBlock_End;++offset) {
            uint16_t component=((uint8_t *)&Mechs[recordId])[offset];
            if(component && !(component&Component_Destroyed) && component>=Mech_Small_Laser && component<=Mech_SRMissile6) {
                uint16_t packedRange=WeaponStats[component].rangeBracket&CombatAiShortRangePackedMask;
                if((int16_t)packedRange<(int16_t)minimumPackedShortRange) minimumPackedShortRange=packedRange;
            }
        }
        approachDistance=minimumPackedShortRange==UINT8_MAX?0:minimumPackedShortRange>>CombatAiApproachPackedRangeShift;
    } else {
        uint16_t characterId=(uint16_t)(actorId-Friendly_Infantry_Combatant_Range_First);
        if((int16_t)actorId>=Enemy_Infantry_CombatantId_Range_First) characterId=(uint16_t)(actorId-Enemy_Infantry_Record_First);
        int16_t weaponId=(int8_t)Characters[characterId].weapon;
        approachDistance=(WeaponStats[weaponId].rangeBracket>>WeaponShortRangeShift)&7;
    }
    uint16_t targetPathClear=Combat_Check_Terrain_Path(actorId,targetId,CombatantPackedX[targetId],CombatantPackedY[targetId]);
    if(((int16_t)actorId>=Enemy_Infantry_CombatantId_Range_First ||
        ((int16_t)actorId>=Friendly_Infantry_Combatant_Range_First && (int16_t)actorId<Enemy_All_CombatantId_Range_First)) &&
       ((int16_t)targetId<Friendly_Infantry_Combatant_Range_First ||
        ((int16_t)targetId>=Enemy_All_CombatantId_Range_First && (int16_t)targetId<Enemy_Infantry_CombatantId_Range_First))) {
        FriendlyPersonnelWithdrawal=(int16_t)actorId<Enemy_All_CombatantId_Range_First?TRUE:FALSE;
        if((int16_t)actorId>=Enemy_Infantry_CombatantId_Range_First) EnemyPersonnelFlightPossible=TRUE;
        uint16_t actorX=CrescentHawkMapPositionX,actorY=CrescentHawkMapPositionY;
        if(actorX>targetX) {
            targetX=(uint16_t)(actorX+CombatAiWithdrawalCells);
            if(targetX&PackedPositionLocalCarryBit) targetX=(uint16_t)(targetX+PackedPositionLocalCarryBit);
        } else if(actorX<targetX) {
            targetX=(uint16_t)(actorX-CombatAiWithdrawalCells);
            if(targetX&PackedPositionLocalCarryBit) targetX&=PackedPositionWestNormalizeMask;
        }
        if(actorY>targetY) {
            targetY=(uint16_t)(actorY+CombatAiWithdrawalCells);
            if(targetY&PackedPositionLocalCarryBit) targetY=(uint16_t)(targetY+PackedPositionSouthCarry);
        } else if(actorY<targetY) {
            targetY=(uint16_t)(actorY-CombatAiWithdrawalCells);
            if(targetY&PackedPositionLocalCarryBit) targetY&=PackedPositionNorthNormalizeMask;
        }
        targetPathClear=FALSE; approachDistance=0;
    }
    uint16_t orderStart=(uint16_t)(actorId*CombatMovementOrderBytes);
    if(!approachDistance || !targetPathClear) {
        CombatMovementOrders[orderStart]=(uint8_t)movementMode;
        CombatMovementOrders[orderStart+1]=(uint8_t)(((targetX&PackedPositionXRegionMask)|(targetY&PackedPositionYRegionMask))>>8);
        targetX&=PackedPositionLocalMask; targetY&=PackedPositionLocalMask;
        CombatMovementOrders[orderStart+2]=(uint8_t)targetX; CombatMovementOrders[orderStart+3]=(uint8_t)targetY;
    } else {
        if(nearestDistance<=(int16_t)approachDistance) goto AssignAttackTargets;
        uint16_t approachX=CrescentHawkMapPositionX,approachY=CrescentHawkMapPositionY;
        int16_t stepX=0,stepY=0;
        if(approachX>targetX) stepX=-1; else if(approachX<targetX) stepX=1;
        if(approachY>targetY) stepY=-1; else if(approachY<targetY) stepY=1;
        while(nearestDistance-->(int16_t)approachDistance) {
            approachX=(uint16_t)(approachX+stepX);
            if(approachX&PackedPositionLocalCarryBit) {
                if(stepX==-1) approachX&=PackedPositionWestNormalizeMask;
                else if(stepX==1) approachX=(uint16_t)(approachX+PackedPositionLocalCarryBit);
            }
            approachY=(uint16_t)(approachY+stepY);
            if(approachY&PackedPositionLocalCarryBit) {
                if(stepY==-1) approachY&=PackedPositionNorthNormalizeMask;
                else if(stepY==1) approachY=(uint16_t)(approachY+PackedPositionSouthCarry);
            }
        }
        CombatMovementOrders[orderStart]=(uint8_t)movementMode;
        CombatMovementOrders[orderStart+1]=(uint8_t)(((approachX&PackedPositionXRegionMask)|(approachY&PackedPositionYRegionMask))>>8);
        approachX&=PackedPositionLocalMask; approachY&=PackedPositionLocalMask;
        CombatMovementOrders[orderStart+2]=(uint8_t)approachX; CombatMovementOrders[orderStart+3]=(uint8_t)approachY;
    }
AssignAttackTargets:
    if(((int16_t)actorId>=Friendly_Infantry_Combatant_Range_First && (int16_t)actorId<Enemy_All_CombatantId_Range_First) ||
       (int16_t)actorId>=Enemy_Infantry_CombatantId_Range_First) {
        CombatWeaponTarget[actorId*CombatWeaponTargetSlots]=(uint8_t)targetId;
        if(TraitorInParty && (uint16_t)((int16_t)(int8_t)TraitorCharacterId+Friendly_Infantry_Combatant_Range_First)==actorId)
            CombatWeaponTarget[actorId*CombatWeaponTargetSlots]=UINT8_MAX;
    } else {
        if(((int16_t)actorId<Friendly_Infantry_Combatant_Range_First && (int16_t)targetId<Enemy_Infantry_CombatantId_Range_First) ||
           ((int16_t)actorId>=Enemy_All_CombatantId_Range_First && (int16_t)actorId<Enemy_Infantry_CombatantId_Range_First &&
            (int16_t)targetId<Friendly_Infantry_Combatant_Range_First)) {
            uint16_t actorX=CombatantPackedX[actorId],actorY=CombatantPackedY[actorId];
            uint16_t opposingX=CombatantPackedX[targetId],opposingY=CombatantPackedY[targetId];
            uint16_t actorWorldX=(uint16_t)(((actorX&PackedPositionXRegionMask)>>1)|(actorX&PackedPositionLocalMask));
            uint16_t actorWorldY=(uint16_t)(((actorY&PackedPositionYRegionMask)>>5)|(actorY&PackedPositionLocalMask));
            uint16_t targetWorldX=(uint16_t)(((opposingX&PackedPositionXRegionMask)>>1)|(opposingX&PackedPositionLocalMask));
            uint16_t targetWorldY=(uint16_t)(((opposingY&PackedPositionYRegionMask)>>5)|(opposingY&PackedPositionLocalMask));
            ++actorWorldX; ++targetWorldX;
            if(targetPathClear && Native_Abs_Word((int16_t)(targetWorldX-actorWorldX))<=MechFootprintWidth &&
               Native_Abs_Word((int16_t)(targetWorldY-actorWorldY))<MechFootprintInfantryDistance) {
                CombatantAnimationSelector[actorId]=UINT8_MAX; CombatMovementOrders[orderStart]=UINT8_MAX;
                CombatantMovementDirection[actorId]=(uint8_t)Get_Target_Compass_Direction(actorX,actorY,opposingX,opposingY);
            }
        }
        uint16_t targetStart=(uint16_t)(actorId*CombatWeaponTargetSlots);
        for(uint16_t slot=0;slot<CombatWeaponTargetSlots;++slot) CombatWeaponTarget[targetStart+slot]=UINT8_MAX;
        uint16_t mechRecordId=actorId;
        if((int16_t)actorId>=Enemy_All_CombatantId_Range_First) mechRecordId-=Enemy_Infantry_Record_First;
        if(MechHeatLevel[mechRecordId]<MechHeatShutdownLevel) {
            for(uint16_t ordinal=0;ordinal<MechWeaponOrdinalCount;++ordinal)
                if(Combat_Get_Weapon_Index_For_Ordinal(mechRecordId,ordinal)!=CombatWeaponOrdinalUnavailable)
                    CombatWeaponTarget[targetStart+ordinal]=(uint8_t)targetId;
            CombatWeaponTarget[targetStart+CombatKickTargetSlot]=(uint8_t)targetId;
        } else {
            CombatMovementOrders[orderStart]=UINT8_MAX;
            for(uint16_t slot=0;slot<CombatWeaponTargetSlots;++slot) CombatWeaponTarget[targetStart+slot]|=Component_Destroyed;
        }
    }
    Combat_Calculate_Movement(actorId);
    if(!recordBank && showPlanningPreview) Combat_Render_Movement_Preview(actorId,TRUE);
    Move_Map_View_To_Packed_Position(savedMapX,savedMapY);
}
