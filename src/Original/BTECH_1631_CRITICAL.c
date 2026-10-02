#include "game.h"

/* Sol: Complete original11AB critical dispatch. Offsets retain annotated raw
 * critical-block provenance where anatomical grouping remains uncertain. */
void Combat_Critical_Mech_Damage(uint16_t mechId, uint16_t structureOffset)
{
	// Sol: Incorporated ASM315E text correction: no trailing CR.
	// Sol:11AB..15F9 WORD mech RECORD ID0..7 and structure BYTE OFFSET1C..23.
	uint8_t * record = (uint8_t *)&Mechs[mechId];
	uint16_t sectionDestroyed = record[structureOffset] == 0; // SS:BP-4 flag, not pointer.
	uint16_t criticalHitsRemaining = 1; // BP-0A independent of dice/result/register scratch.
	int16_t criticalRoll = (int16_t)Roll2D6(); // BP-0C; original always rolls, including destroyed section.
	if (criticalRoll >= CriticalFirstMultipleHitRoll)
		criticalHitsRemaining = ((criticalRoll - CriticalFirstMultipleHitRoll) >> 1) + 1; // Sol:11EA roll8/9 ->1,10/11 ->2,12 ->3; below8 remains1.
	if (CombatMessageVerbosity != CombatMessage_None && criticalHitsRemaining != 0)
	{
		Menu_Memory_Variables(4);
		Combat_CombatMessageVerbosityFilter((uint8_t *)"\rCritical!"); // DS:315E; verified EXE text.
		CombatNotificationLatch = TRUE; // Sol:1219 WORD4586 notification latch, not hit count.
	}
	if (sectionDestroyed != FALSE)
	{
		criticalHitsRemaining = 0;
		int16_t start = (int8_t)CriticalSectionStart[structureOffset - MechStructureFirstOffset];
		int16_t count = (int8_t)CriticalSectionCount[structureOffset - MechStructureFirstOffset]; // Sol:122F/1267 original tables indexed as316E/3176+structure offset.
		for (int16_t index = 0; index < count; ++index)
			if (record[start + index] != 0) record[start + index] |= Component_Destroyed; // Sol: nonempty slot marked destroyed, including already marked ones.
		switch (structureOffset)
		{
		case 0x1C: record[MECH_Offset_CurrentActuators_Left] &= MechActuatorLegMask; break; // Sol:128C clears high arm nibble at24.
		case 0x21: record[MECH_Offset_CurrentActuators_Right] &= MechActuatorLegMask; break; // Sol:12BA high arm nibble at25.
		case 0x1E: record[MECH_Offset_CurrentActuators_Left] &= MechActuatorArmMask; break; // Sol:12CE clears low leg nibble at24.
		case 0x23: record[MECH_Offset_CurrentActuators_Right] &= MechActuatorArmMask; break; // Sol:12E2 low leg nibble at25.
		}
		return; // Sol: section-only destruction does not newly set E484 here.
	}
	while (criticalHitsRemaining != 0)
	{
		uint16_t componentStart, actuatorOffset;
		int16_t componentCount;
		uint8_t nibbleMask;
		switch (structureOffset) // Sol:15E2 eight-entry dispatch; raw offsets intentional under A-005.
		{
		case 0x1F: // Sol:12F6 head-critical systems.
		{
			uint16_t roll = RollD6();
			if (roll == 1 || roll == 6)
			{
				if (Mechs[mechId].lifeSupportState == 0) continue; // Sol:131C +78 probable life-support; only ZERO rejected.
				Mechs[mechId].lifeSupportState = 0xFF; // Sol: alreadyFF can be hit again; preserve original nonzero gate.
				--criticalHitsRemaining;
			}
			else if (roll == 2 || roll == 5)
			{
				if (Mechs[mechId].sensorHits >= MechSensorMaximumHits) continue; // Sol:1347 unsigned BYTE threshold, two sensor hits maximum.
				++Mechs[mechId].sensorHits;
				--criticalHitsRemaining;
			}
			else if (roll == 3)
			{
				record[0x1F] = 0; // Sol:136D head structure destroyed.
				criticalHitsRemaining = 0;
				MechDestroyedFlag = TRUE; // DS:E484 WORD.
			}
			else if (roll == 4)
				criticalHitsRemaining -= Mech_Destroy_One_Intact_Critical(record + 0x55, 1); // Sol:1393 head critical BYTE; helper0 retries without consumption.
			continue;
		}
		case 0x20: // Sol:13B5 central engine/gyro systems and two-slot block53.
			if ((int16_t)RollD6() <= 3)
			{
				uint16_t destroyed;
				--criticalHitsRemaining;
				if ((int16_t)RollD6() <= 3)
				{
					++Mechs[mechId].gyroHits; // +76 BYTE increment.
					destroyed = Mechs[mechId].gyroHits >= MechGyroDestroyedHitCount;
				}
				else
				{
					++Mechs[mechId].engineHits; // +75 BYTE increment.
					destroyed = Mechs[mechId].engineHits >= MechEngineDestroyedHitCount;
				}
				if (destroyed != FALSE)
				{
					record[0x20] = 0; // Sol:13E8 centre structure.
					criticalHitsRemaining = 0;
					MechDestroyedFlag = TRUE;
				}
			}
			else
				criticalHitsRemaining -= Mech_Destroy_One_Intact_Critical(record + 0x53, 2); // Sol:1410 two critical BYTES53/54; not a leg-field alias.
			continue;
		case 0x1D: case 0x22: // Sol:14ED seven-slot torso sections, no actuator nibble.
			componentStart = structureOffset == 0x22 ? 0x48 : 0x3A;
			if (Mech_Count_Intact_Criticals(record + componentStart, 7) == 0)
				criticalHitsRemaining = 0; // Sol:1573 no eligible component ends remaining criticals.
			else
				criticalHitsRemaining -= Mech_Destroy_One_Intact_Critical(record + componentStart, 7);
			continue;
		case 0x1E: case 0x23: // Sol:1422 low-nibble leg actuators and two-slot blocks4F/51.
			actuatorOffset = structureOffset == 0x23 ? MECH_Offset_CurrentActuators_Right : MECH_Offset_CurrentActuators_Left;
			componentStart = structureOffset == 0x23 ? 0x51 : 0x4F;
			componentCount = 2;
			nibbleMask = MechActuatorLegMask;
			break;
		case 0x1C: case 0x21: // Sol:1525 high-nibble arm actuators and seven-slot blocks33/41.
			actuatorOffset = structureOffset == 0x21 ? MECH_Offset_CurrentActuators_Right : MECH_Offset_CurrentActuators_Left;
			componentStart = structureOffset == 0x21 ? 0x41 : 0x33;
			componentCount = 7;
			nibbleMask = MechActuatorArmMask;
			break;
		default: continue; // Sol:12A7 invalid offset retries forever; legal caller contract1C..23, no silent new validation.
		}
		if ((record[actuatorOffset] & nibbleMask) == 0 && Mech_Count_Intact_Criticals(record + componentStart, componentCount) == 0)
		{
			criticalHitsRemaining = 0;
			continue;
		}
		uint16_t roll = RollD6(); // Sol:1473/157B roll1..2 component,3..6 actuator if any nibble remains.
		if ((int16_t)roll >= 3 && (record[actuatorOffset] & nibbleMask) != 0)
		{
			uint8_t hitMask = nibbleMask == MechActuatorLegMask
				? CriticalLowActuatorHitMask[roll - 3]
				: CriticalHighActuatorHitMask[roll - 3];
			if ((record[actuatorOffset] & (int16_t)(int8_t)hitMask) == 0) continue; // Sol:14A3/15AB CBW mask, missing chosen actuator retries.
			uint8_t clearMask = nibbleMask == MechActuatorLegMask
				? CriticalLowActuatorClearMask[roll - 3]
				: CriticalHighActuatorClearMask[roll - 3];
			record[actuatorOffset] &= clearMask; // Sol:14C1 BYTE AND, consumes exactly one hit.
			--criticalHitsRemaining;
		}
		else
			criticalHitsRemaining -= Mech_Destroy_One_Intact_Critical(record + componentStart, componentCount); // Sol:14DA/1520 helper removes one intact critical; zero retries.
	}
}

/* 1631:15FA. Sol: Count nonempty entries without the destruction high bit. */
uint16_t Mech_Count_Intact_Criticals(uint8_t *components, int16_t count)
{
    uint16_t intactCount = 0;
    for (int16_t index = 0; index < count; ++index)
        if (components[index] != 0 && (components[index] & Component_Destroyed) == 0) ++intactCount;
    return intactCount;
}
/* 1631:163E. Sol: Initial random start is0..3, NOT modulo range size;
 * normalize BEFORE dereferencing, then scan cyclically and mark one entry. */
uint16_t Mech_Destroy_One_Intact_Critical(uint8_t *components, int16_t count)
{
    if (Mech_Count_Intact_Criticals(components,count) == 0) return 0;
    int16_t index = Rand_0x00_to_0xFF() & CriticalInitialSelectionMask;
    for (;;)
    {
        if (index >= count) index = 0;
        if (components[index] != 0 && (components[index] & Component_Destroyed) == 0)
        {
            components[index] |= Component_Destroyed;
            return 1;
        }
        ++index;
    }
}
/* 1631:1B44. Sol: Return the number of missing low THREE actuator bits.
 * Argument2 is a record BYTE offset, not a component mask or boolean. */
uint16_t Mech_Count_Missing_Low_Actuator_Bits(uint16_t mechId, uint16_t recordOffset)
{
    uint8_t actuators = ((uint8_t *)&Mechs[mechId])[recordOffset];
    uint16_t missingCount = 0;
    if ((actuators & 4) == 0) ++missingCount;
    if ((actuators & 2) == 0) ++missingCount;
    if ((actuators & 1) == 0) ++missingCount;
    return missingCount;
}
/* 1631:1DAB. Sol: Both Brief and Verbose allow this message; only None hides it. */
void Combat_CombatMessageVerbosityFilter(uint8_t *message)
{
    if (CombatMessageVerbosity != CombatMessage_None) Display_Text_From_Memory(message);
}
