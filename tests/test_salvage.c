#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Sol: Original salvage, text primitives and Yes/No are production bodies.
 * RNG/input/display/wait adapters are test-only, not an emulator recording. */
static unsigned checks, rngReads, pauses, inspections;
static uint8_t rng[8];
static uint16_t answer;
static int recoveredMessage, failedMessage, exhaustedMessage;
static char message[DynamicTextScratchBytes];
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Salvage check %u failed\n",checks); exit(1); }
}
uint8_t Rand_0x00_to_0xFF(void)
{
    check(rngReads < sizeof rng);
    return rng[rngReads++];
}
void Draw_Top_Graphic_Sidebar(void) { TextRow = TextColumn = 0; }
void Display_Text_From_Memory(uint8_t *text)
{
    ++TextRow;
    if (text == DynamicString) {
        size_t bytes = strlen((char *)text) + 1;
        check(bytes <= sizeof message);
        memcpy(message,text,bytes);
        if (strstr(message," is salvaging a ")) recoveredMessage = 1;
        if (strstr(message,"No chance to salvage")) failedMessage = 1;
    } else if (text[0] == 'T' && strstr((char *)text,"no more salvageable")) exhaustedMessage = 1;
}
void Drain_Pending_Keyboard_Input(void) { }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { return answer; }
void Prompt_And_Wait_For_Key(void) { ++pauses; }
void Display_Sentence_Period(void) { }
void Wait_For_N_Vertical_Retraces(uint16_t count)
{
    check(count == SalvageInspectionRetraceCount);
    ++inspections;
}
static void reset(uint8_t techSkill)
{
    memset(Characters,0,sizeof Characters);
    for (unsigned i = 0; i < CharacterRecordCount; ++i) Characters[i].name = Character_Dead;
    Characters[0].name = 0;
    Characters[0].skillTech = techSkill;
    Characters[0].skillPiloting = 1;
    Characters[0].mechAssignment = Character_OnFoot;
    memset(Mechs,0,sizeof Mechs);
    for (unsigned i = 0; i < MechRecordCount; ++i) Mechs[i].name[0] = MECH_Destroyed;
    memset(CombatantCasualtyFlags,0,sizeof CombatantCasualtyFlags);
    memset(DynamicString,0,sizeof DynamicString);
    memset(DestroyedMechNameInitial,0,sizeof DestroyedMechNameInitial);
    memset(CombatantSpriteFamilyOffset,0,sizeof CombatantSpriteFamilyOffset);
    memset(rng,0,sizeof rng); rng[1] = rng[3] = 1;
    answer = 'Y';
    rngReads = pauses = inspections = 0;
    recoveredMessage = failedMessage = exhaustedMessage = 0;
}
static void wreck(unsigned enemySlot, unsigned templateId)
{
    unsigned record = Enemy_Mech_Record_First + enemySlot;
    Mechs[record] = MechRefs[templateId];
    DestroyedMechNameInitial[record] = Mechs[record].name[0];
    Mechs[record].name[0] = MECH_Destroyed;
    CombatantCasualtyFlags[Enemy_All_CombatantId_Range_First + enemySlot] = TRUE;
}
int main(void)
{
    Mech expected, before;
    uint8_t source[20] = "abcdef", buffer[40];
    for (unsigned sourceStart = 0; sourceStart < 2; ++sourceStart) {
        for (unsigned destinationStart = 0; destinationStart < 2; ++destinationStart) {
            uint8_t *destination = buffer + destinationStart;
            check(Append_Large_Text_To_Memory(destination,source + sourceStart) == destination);
            check(!strcmp((char *)destination,(char *)source + sourceStart));
            check(Loop_Until_TextPtr_Null(destination) == 6 - sourceStart);
            check(Append_Text_To_Memory(destination,(uint8_t *)"!") == destination);
            check(destination[6-sourceStart] == '!' && destination[7-sourceStart] == 0);
        }
    }
    check(Loop_Until_TextPtr_Null((uint8_t *)"") == 0);

    reset(3); wreck(0,0); expected = Mechs[4];
    expected.name[0] = 'L'; expected.pilotId = 0; expected.engineHits = expected.gyroHits = 1;
    Salvage_Mechs_Dialog();
    check(!memcmp(&Mechs[0],&expected,sizeof expected));
    check(Characters[0].mechAssignment == 0 && CombatantSpriteFamilyOffset[0] == MECH_Sprite_LOCUST);
    check(!CombatantCasualtyFlags[12] && rngReads == 2 && inspections == 1 && pauses == 2);
    check(recoveredMessage && exhaustedMessage && !strcmp(message," is salvaging a LOCUST"));

    reset(3); wreck(0,1); Mechs[4].engineHits = 3;
    Salvage_Mechs_Dialog();
    check(failedMessage && exhaustedMessage && Mechs[0].name[0] == MECH_Destroyed && !CombatantCasualtyFlags[12]);
    check(DynamicString[SalvageFailureNamePlaceholder] == 'W');

    reset(4); wreck(0,1); Mechs[4].engineHits = 3; Mechs[4].gyroHits = 2;
    Mechs[4].currentStructure[3] = Mechs[4].currentStructure[4] = 0;
    Salvage_Mechs_Dialog();
    check(Mechs[0].name[0] == 'W' && Mechs[0].engineHits == 1 && Mechs[0].gyroHits == 1);
    check(Mechs[0].currentStructure[3] == 1 && Mechs[0].currentStructure[4] == 1);
    check(CombatantSpriteFamilyOffset[0] == MECH_Sprite_COMMANDO);

    reset(3); wreck(0,0); wreck(1,1); Mechs[4].engineHits = 3;
    Salvage_Mechs_Dialog(); check(failedMessage && recoveredMessage && rngReads == 4 && Mechs[0].name[0] == 'W');

    reset(3); wreck(0,0); rng[1] = 0; /* Begin friendly range, wrap/scan to enemy wreck. */
    Salvage_Mechs_Dialog(); check(recoveredMessage && rngReads == 2 && Mechs[0].name[0] == 'L');

    reset(3); wreck(0,0); answer = 'N';
    Salvage_Mechs_Dialog(); check(rngReads == 0 && inspections == 0 && CombatantCasualtyFlags[12]);
    reset(3); wreck(0,0); Characters[0].mechAssignment = 255;
    Salvage_Mechs_Dialog(); check(rngReads == 0 && CombatantCasualtyFlags[12]);

    reset(3); wreck(0,0);
    for (unsigned i = 0; i < LanceSize; ++i) Mechs[i] = MechRefs[i];
    before = Mechs[0];
    Salvage_Mechs_Dialog();
    check(recoveredMessage && !CombatantCasualtyFlags[12] && !memcmp(&before,&Mechs[0],sizeof before));
    check(Characters[0].mechAssignment == Character_OnFoot); /* Full-lance false success retained. */
    printf("%u whole-Mech salvage/text assertions passed\n",checks);
    return 0;
}
