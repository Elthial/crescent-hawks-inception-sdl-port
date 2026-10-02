/* Sol: Original recruitment and2D6 bodies. RNG/UI/input are deterministic
 * test adapters; synthetic ASM-derived witnesses, not an emulator capture. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, randomReads, pendingReads, waitPolls, draws, drains, keys;
static uint8_t randomValues[32];
static uint16_t expectedSeed;
static char description[256];
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Recruit check %u failed\n",checks); exit(1); }
}
uint8_t Rand_0x00_to_0xFF(void)
{
    check(randomReads < 32);
    if (randomReads == 0) {
        check(RandomByteLow == (uint8_t)expectedSeed);
        check(RandomByteMiddle == (uint8_t)(expectedSeed >> 8));
        check(RandomByteHigh == 0 && RandomStateUpperByte == 0);
    }
    return randomValues[randomReads++];
}
void Display_Text_From_Memory(uint8_t *text)
{
    check(strlen(description) + strlen((char *)text) < sizeof description);
    memcpy(description + strlen(description),text,strlen((char *)text) + 1);
}
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh)
{
    check(refresh == TRUE && randomReads == 13); ++draws;
}
uint16_t Pending_Input(void)
{
    check(draws == 1 && drains == 0 && keys == 0);
    return pendingReads++ >= waitPolls;
}
void Drain_Pending_Keyboard_Input(void) { check(draws == 1 && keys == 0); ++drains; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { check(drains == 1); ++keys; return ' '; }
static void reset(void)
{
    memset(Characters,0,sizeof Characters); memset(Mechs,0,sizeof Mechs);
    for (unsigned i = 0; i < PartySize; ++i) Characters[i].name = 0;
    memset(&Characters[2],0xAA,sizeof Characters[2]); Characters[2].name = Character_Dead;
    Characters[5].name = Character_Dead;
    for (unsigned i = 0; i < MechRecordCount; ++i) Mechs[i].name[0] = MECH_Destroyed;
    memset(CombatantSpriteFamilyOffset,0x92,sizeof CombatantSpriteFamilyOffset);
    memset(randomValues,0,sizeof randomValues);
    randomValues[0] = 3; randomValues[1] = 3; /* Body8 */
    randomValues[2] = 2; randomValues[3] = 3; /* Dexterity7 */
    randomValues[4] = 5; randomValues[5] = 5; /* Charisma12 */
    randomValues[7] = randomValues[9] = randomValues[10] = randomValues[12] = 1;
    randomValues[13] = 1; /* First eligible traitor coin flip. */
    NextRecruitNameId = 2; expectedSeed = 2 * RecruitNameSeedMultiplier;
    RandomByteLow = RandomByteMiddle = RandomByteHigh = RandomStateUpperByte = 0xAA;
    DisableInput = TRUE; TraitorEventOccurred = TraitorInParty = 0;
    TraitorCharacterId = 7; TraitorBattleProbability = 0;
    EnteredBuildingId = SavedBuildingPositionCount - 1;
    SavedBuildingMapPositionX[EnteredBuildingId] = UINT16_C(0x1234);
    SavedBuildingMapPositionY[EnteredBuildingId] = UINT16_C(0xABCD);
    CrescentHawkMapPositionX = CrescentHawkMapPositionY = 0;
    randomReads = pendingReads = waitPolls = draws = drains = keys = 0;
    description[0] = 0;
}
static void checkReturnPoint(void)
{
    check(CrescentHawkMapPositionX == UINT16_C(0x1234));
    check(CrescentHawkMapPositionY == UINT16_C(0xABCD));
}
int main(void)
{
    for (uint16_t specialty = 0; specialty < CharacterSkillCount; ++specialty) {
        reset(); Recruit_Crescent_Hawk_Agent(specialty);
        Character *recruit = &Characters[2];
        check(recruit->name == 2 && NextRecruitNameId == 3);
        check(Characters[5].name == Character_Dead);
        check(recruit->body == 8 && recruit->dexterity == 7 && recruit->charisma == 12);
        check(recruit->health == (specialty == CharacterSkill_Piloting ? 64 : 80));
        check(recruit->weapon == Infantry_Cudgel && recruit->armourType == ArmourType_None);
        check(recruit->armourValue == 0 && recruit->trainingFlags == 0);
        uint8_t *skills = (uint8_t *)recruit + offsetof(Character,skillBowsAndBlade);
        for (unsigned skill = 0; skill < CharacterSkillCount; ++skill) {
            uint8_t expected = randomValues[6 + skill] & 1;
            if (skill == CharacterSkill_Piloting) expected = SkillLevel_Unskilled;
            if (skill == specialty) expected = SkillLevel_Good;
            check(skills[skill] == expected);
        }
        check(recruit->mechAssignment == Character_OnFoot);
        check(CombatantSpriteFamilyOffset[6] == 0 && CombatantSpriteFamilyOffset[5] == 0x92);
        check(!strcmp(description,"His name is Edward and he will be on foot."));
        check(draws == 1 && drains == 1 && keys == 1 && pendingReads == 0 && randomReads == 14);
        check(TraitorCharacterId == 2 && TraitorEventOccurred && TraitorInParty);
        check(TraitorBattleProbability == TraitorInitialBattleProbability);
        checkReturnPoint();
    }
    reset(); Mechs[0].name[0] = 'L'; Mechs[0].riderId = 1;
    Mechs[1].name[0] = 'C'; Mechs[1].riderId = MECH_NoRider; Mechs[1].pilotId = 0;
    Mechs[2].name[0] = 'W'; Mechs[2].riderId = MECH_NoRider;
    Recruit_Crescent_Hawk_Agent(CharacterSkill_Tech);
    check(Characters[2].mechAssignment == 1 && Mechs[1].riderId == 2 && Mechs[1].pilotId == 0);
    check(Mechs[2].riderId == MECH_NoRider && Mechs[0].riderId == 1);
    check(!strcmp(description,"His name is Edward and he will be riding as a passenger in Jason's 'Mech."));
    reset(); Mechs[3].name[0] = 'L'; Mechs[3].riderId = MECH_NoRider;
    Mechs[3].pilotId = 8; Characters[8].name = 9;
    Recruit_Crescent_Hawk_Agent(CharacterSkill_Tech);
    check(Characters[2].mechAssignment == 3 && Mechs[3].riderId == 2);
    check(!strcmp(description,"His name is Edward and he will be riding as a passenger in Hunter's 'Mech."));
    reset(); Mechs[4].name[0] = 'L'; Mechs[4].riderId = MECH_NoRider;
    Recruit_Crescent_Hawk_Agent(CharacterSkill_Tech);
    check(Characters[2].mechAssignment == Character_OnFoot && Mechs[4].riderId == MECH_NoRider);

    reset(); NextRecruitNameId = 9; expectedSeed = 9 * RecruitNameSeedMultiplier;
    Recruit_Crescent_Hawk_Agent(CharacterSkill_Medical);
    check(Characters[2].name == 9 && NextRecruitNameId == RecruitFirstReusableNameId);

    reset(); DisableInput = FALSE; waitPolls = 2;
    randomValues[13] = randomValues[14] = 0; randomValues[15] = 1;
    Recruit_Crescent_Hawk_Agent(CharacterSkill_Tech);
    check(pendingReads == 3 && randomReads == 16 && TraitorEventOccurred);

    reset(); randomValues[13] = 0; Recruit_Crescent_Hawk_Agent(CharacterSkill_Tech);
    check(!TraitorEventOccurred && !TraitorInParty && TraitorCharacterId == 7);
    reset(); TraitorEventOccurred = TRUE; TraitorInParty = TRUE; TraitorBattleProbability = 5;
    Recruit_Crescent_Hawk_Agent(CharacterSkill_Tech);
    check(randomReads == 13 && TraitorCharacterId == 7 && TraitorBattleProbability == 5);

    reset(); Characters[2].name = Characters[5].name = 0;
    Recruit_Crescent_Hawk_Agent(CharacterSkill_Medical);
    check(randomReads == 0 && draws == 0 && keys == 0 && NextRecruitNameId == 2);
    check(RandomByteLow == 0xAA && RandomByteMiddle == 0xAA && RandomByteHigh == 0xAA && RandomStateUpperByte == 0xAA);
    checkReturnPoint();
    reset(); Characters[2].name = 0; Recruit_Crescent_Hawk_Agent(CharacterSkill_Medical);
    check(Characters[5].name == 2 && TraitorCharacterId == 5);
    reset(); Characters[2].name = Characters[5].name = 0; Characters[PartySize-1].name = Character_Dead;
    Recruit_Crescent_Hawk_Agent(CharacterSkill_Tech);
    check(Characters[PartySize-1].name == 2 && TraitorCharacterId == PartySize-1);
    printf("Specialist recruitment: %u checks passed\n",checks);
    return 0;
}
