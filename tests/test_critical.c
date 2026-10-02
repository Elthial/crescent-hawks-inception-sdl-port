/* Sol: Native dispatch and dice bodies, scripted RNG and UI-only adapters.
 * These are branch witnesses, not a recorded gameplay certification. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t randomValues[32];
static unsigned randomCount, randomIndex, checks, menus, messages;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Critical check %u failed\n",checks); exit(1); }
}
uint8_t Rand_0x00_to_0xFF(void)
{
    check(randomIndex < randomCount);
    return randomValues[randomIndex++];
}
void Menu_Memory_Variables(uint16_t layout) { check(layout == 4); ++menus; }
void Display_Text_From_Memory(uint8_t *text)
{
    check(!strcmp((char *)text,"\rCritical!")); ++messages;
}
static void prepare(const uint8_t *values, unsigned count)
{
    memset(Mechs,0,sizeof Mechs);
    memset(Mechs[0].currentStructure,10,sizeof Mechs[0].currentStructure);
    memcpy(randomValues,values,count); randomCount=count; randomIndex=0;
    menus=messages=0; MechDestroyedFlag=0; CombatNotificationLatch=0;
    CombatMessageVerbosity=0;
}
int main(void)
{
    uint8_t components[4]={0,0x81,2,3};
    check(Mech_Count_Intact_Criticals(components,4)==2);
    check(Mech_Count_Intact_Criticals(components,-1)==0);
    randomCount=1; randomIndex=0; randomValues[0]=3;
    check(Mech_Destroy_One_Intact_Critical(components,4)==1 && components[3]==0x83);
    check(Mech_Destroy_One_Intact_Critical(components,-1)==0 && randomIndex==1);
    components[0]=1; randomIndex=0;
    check(Mech_Destroy_One_Intact_Critical(components,1)==1 && components[0]==0x81);
    for (unsigned mask=0;mask<256;++mask) {
        Mechs[0].currentActuators[0]=(uint8_t)mask;
        unsigned missing=(!(mask&1))+(!(mask&2))+(!(mask&4));
        check(Mech_Count_Missing_Low_Actuator_Bits(0,0x24)==missing);
    }
    const uint8_t lowRoll[]={0,0}; /* 2D6=2 still gives ONE critical. */
    prepare(lowRoll,sizeof lowRoll);
    uint8_t *record=(uint8_t *)&Mechs[0];
    record[0x1C]=0; record[0x33]=2; record[0x34]=0x83; record[0x24]=0xFF;
    CombatMessageVerbosity=1;
    Combat_Critical_Mech_Damage(0,0x1C);
    check(record[0x33]==0x82 && record[0x34]==0x83 && record[0x35]==0);
    check(record[0x24]==0x0F && !MechDestroyedFlag);
    check(randomIndex==2 && menus==1 && messages==1 && CombatNotificationLatch==1);
    const uint8_t sensors[]={5,5,1,4,2}; /* 12=>3 hits: sensors twice, then head. */
    prepare(sensors,sizeof sensors);
    Combat_Critical_Mech_Damage(0,0x1F);
    check(Mechs[0].sensorHits==2 && record[0x1F]==0 && MechDestroyedFlag);
    check(randomIndex==5 && messages==0);
    const uint8_t retry[]={0,0,0,1}; /* Absent life support retries, sensor accepted. */
    prepare(retry,sizeof retry);
    Combat_Critical_Mech_Damage(0,0x1F);
    check(Mechs[0].sensorHits==1 && randomIndex==4);
    const uint8_t gyro[]={0,0,0,0};
    prepare(gyro,sizeof gyro); Mechs[0].gyroHits=1;
    Combat_Critical_Mech_Damage(0,0x20);
    check(Mechs[0].gyroHits==2 && record[0x20]==0 && MechDestroyedFlag);
    const uint8_t engine[]={0,0,0,5};
    prepare(engine,sizeof engine); Mechs[0].engineHits=2;
    Combat_Critical_Mech_Damage(0,0x20);
    check(Mechs[0].engineHits==3 && record[0x20]==0 && MechDestroyedFlag);
    prepare(engine,sizeof engine); Mechs[0].engineHits=255;
    Combat_Critical_Mech_Damage(0,0x20);
    check(Mechs[0].engineHits==0 && record[0x20]==10 && !MechDestroyedFlag);
    const uint8_t actuator[]={0,0,2,3}; /* Missing bit8 retry; present bit4 removed. */
    prepare(actuator,sizeof actuator); record[0x24]=4;
    Combat_Critical_Mech_Damage(0,0x1E);
    check(record[0x24]==0 && randomIndex==4);
    const uint8_t torso[]={0,0,3};
    prepare(torso,sizeof torso); record[0x3E]=2;
    Combat_Critical_Mech_Damage(0,0x1D);
    check(record[0x3E]==0x82 && randomIndex==3);
    prepare(lowRoll,sizeof lowRoll);
    Combat_Critical_Mech_Damage(0,0x22);
    check(randomIndex==2 && !MechDestroyedFlag); /* Empty section stops, no RNG search. */
    const uint8_t oneSlot[]={0,0,3,3}; /* Head die4, helper start3 wraps to0. */
    prepare(oneSlot,sizeof oneSlot); record[0x55]=9;
    Combat_Critical_Mech_Damage(0,0x1F);
    check(record[0x55]==0x89 && randomIndex==4);
    const uint8_t centralSlot[]={0,0,5,3};
    prepare(centralSlot,sizeof centralSlot); record[0x53]=4;
    Combat_Critical_Mech_Damage(0,0x20);
    check(record[0x53]==0x84 && randomIndex==4 && !MechDestroyedFlag);
    const uint8_t arm[]={0,0,2};
    prepare(arm,sizeof arm); record[0x25]=0x80;
    Combat_Critical_Mech_Damage(0,0x21);
    check(record[0x25]==0 && randomIndex==3);
    const uint8_t repeatedSupport[]={3,3,0,5}; /* 2D6=8 one hit, life supportFF remains valid. */
    prepare(repeatedSupport,3); Mechs[0].lifeSupportState=255;
    Combat_Critical_Mech_Damage(0,0x1F);
    check(Mechs[0].lifeSupportState==255 && randomIndex==3);
    /* Every destroyed structure dispatch uses the EXE-owned start/count,
     * and only limbs alter the corresponding actuator nibble. */
    for (uint16_t location=0;location<MechStructureLocationCount;++location) {
        prepare(lowRoll,sizeof lowRoll);
        memset(Mechs[0].criticalSlots,1,sizeof Mechs[0].criticalSlots);
        Mechs[0].currentActuators[0]=Mechs[0].currentActuators[1]=255;
        record[MechStructureFirstOffset+location]=0;
        Combat_Critical_Mech_Damage(0,(uint16_t)(MechStructureFirstOffset+location));
        for (unsigned index=0;index<sizeof Mechs[0].criticalSlots;++index) {
            unsigned offset=0x33+index;
            int hit=offset>=CriticalSectionStart[location] &&
                offset<(unsigned)(CriticalSectionStart[location]+CriticalSectionCount[location]);
            check(record[offset]==(hit?0x81:1));
        }
        check(record[0x24]==(location==0?15:location==2?240:255));
        check(record[0x25]==(location==5?15:location==7?240:255));
        check(!MechDestroyedFlag && randomIndex==2);
    }
    printf("Mech critical damage: %u checks passed\n",checks);
    return 0;
}
