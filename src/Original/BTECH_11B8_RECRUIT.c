#include "game.h"

/* 11B8:0D58..104D. Sol: Fill the first vacant party slot with a specialist.
 * Roll attributes and skills, assign a free passenger seat, describe the
 * recruit, consider traitor selection, then restore the building return point.
 * Original call order and signed BYTE loads retained. Valid specialty0..6,
 * character/name/assignment and saved-building indexes are required. */
void Recruit_Crescent_Hawk_Agent(uint16_t specialtySkillIndex)
{
    for (uint16_t partyMemberId = 0; partyMemberId < PartySize; ++partyMemberId)
    {
        if (Characters[partyMemberId].name == Character_Dead)
        {
            Character *newRecruit = &Characters[partyMemberId];
            uint8_t recruitNameId = NextRecruitNameId;
            /* Sol: Native CBW/IMUL391/CWD retains only the low product WORD,
             * then sign-extends it. Bind both seed WORDs to the SAME four
             * bytes as0BC0, not independent seed globals. */
            uint16_t seedLowWord = (uint16_t)((int32_t)(int8_t)recruitNameId * RecruitNameSeedMultiplier);
            uint16_t seedHighWord = (seedLowWord & UINT16_C(0x8000)) ? UINT16_MAX : 0;
            RandomByteLow = (uint8_t)seedLowWord;
            RandomByteMiddle = (uint8_t)(seedLowWord >> 8);
            RandomByteHigh = (uint8_t)seedHighWord;
            RandomStateUpperByte = (uint8_t)(seedHighWord >> 8);

            newRecruit->name = recruitNameId;
            ++NextRecruitNameId;
            if ((int8_t)NextRecruitNameId >= RecruitNameCycleLimit)
                NextRecruitNameId = RecruitFirstReusableNameId;

            newRecruit->trainingFlags = 0;
            newRecruit->armourValue = 0;
            newRecruit->armourType = ArmourType_None;
            newRecruit->weapon = Infantry_Cudgel;
            newRecruit->body = (uint8_t)Roll2D6();
            newRecruit->dexterity = (uint8_t)Roll2D6();
            newRecruit->charisma = (uint8_t)Roll2D6();
            newRecruit->health = (uint8_t)((int8_t)newRecruit->body * CharacterHealthPerBodyPoint);
            CombatantSpriteFamilyOffset[Friendly_Infantry_Combatant_Range_First + partyMemberId] = 0;
            newRecruit->mechAssignment = Character_OnFoot;

            /* Sol: Traverse the byte representation of the whole record;
             * the seven original skill BYTEs begin at native record offset4. */
            uint8_t *recruitSkills = (uint8_t *)newRecruit + offsetof(Character,skillBowsAndBlade);
            for (uint16_t skillIndex = 0; skillIndex < CharacterSkillCount; ++skillIndex)
                recruitSkills[skillIndex] = Rand_0x00_to_0xFF() & 1;
            newRecruit->skillPiloting = SkillLevel_Unskilled;
            recruitSkills[specialtySkillIndex] = SkillLevel_Good;
            if (specialtySkillIndex == CharacterSkill_Piloting)
                newRecruit->health = (uint8_t)(newRecruit->health -
                    (uint8_t)(newRecruit->body * RecruitPilotInjuryPerBodyPoint));

            for (uint16_t mechLanceId = 0; mechLanceId < LanceSize; ++mechLanceId)
            {
                Mech *availableMech = &Mechs[mechLanceId];
                if (availableMech->name[0] != MECH_Destroyed && availableMech->riderId == MECH_NoRider)
                {
                    availableMech->riderId = (uint8_t)partyMemberId;
                    newRecruit->mechAssignment = (uint8_t)mechLanceId;
                    break;
                }
            }

            Display_Text_From_Memory((uint8_t *)"His name is "); /* 3EDB:1E2F */
            Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)newRecruit->name]);
            Display_Text_From_Memory((uint8_t *)" and he will be "); /* 3EDB:1E3C */
            if (newRecruit->mechAssignment == Character_OnFoot)
                Display_Text_From_Memory((uint8_t *)"on foot."); /* 3EDB:1E4D */
            else
            {
                Display_Text_From_Memory((uint8_t *)"riding as a passenger in "); /* 3EDB:1E56 */
                uint8_t pilotPartyMemberId = Mechs[(int16_t)(int8_t)newRecruit->mechAssignment].pilotId;
                uint8_t pilotNameId = Characters[pilotPartyMemberId].name; /* native unsigned MUL17 */
                Display_Text_From_Memory(CharacterNames[(int16_t)(int8_t)pilotNameId]);
                Display_Text_From_Memory((uint8_t *)"'s 'Mech."); /* 3EDB:1E70 */
            }
            Draw_Health_and_C_Bills_Sidebar(TRUE);
            if (DisableInput == FALSE)
                while (Pending_Input() == FALSE)
                    Rand_0x00_to_0xFF();
            Drain_Pending_Keyboard_Input();
            Keyboard_Get_ASCII_Hex_Input();
            if (TraitorEventOccurred == FALSE && (Rand_0x00_to_0xFF() & 1) != FALSE)
            {
                TraitorCharacterId = (uint8_t)partyMemberId;
                TraitorEventOccurred = TRUE;
                TraitorInParty = TRUE;
                TraitorBattleProbability = TraitorInitialBattleProbability;
            }
            break;
        }
    }
    CrescentHawkMapPositionX = SavedBuildingMapPositionX[EnteredBuildingId];
    CrescentHawkMapPositionY = SavedBuildingMapPositionY[EnteredBuildingId];
}
