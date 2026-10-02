#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Sol: actual full salvage parent and structure-support predicate. Only
 * text/input/colour/number presentation and random BYTE input are isolated. */
static unsigned prompts,randomCalls,numbers;
static uint16_t lastNumber;
static uint8_t randomByte;
static char output[4096];
static void verify(int condition,unsigned line){if(!condition){fprintf(stderr,"Component salvage mismatch line%u\n",line);exit(1);}}
#define check(x) verify(!!(x),__LINE__)
void Display_Text_From_Memory(uint8_t *text){size_t used=strlen(output),length=strlen((char *)text);check(used+length<sizeof output);memcpy(output+used,text,length+1);}
void Prompt_And_Wait_For_Key(void){++prompts;}
void Set_Text_Colour_Bright_Green(void){TextColour=EGA_BrightGreen;}
void Display_Text_Dynamic_Value(uint16_t value){lastNumber=value;++numbers;}
uint8_t Rand_0x00_to_0xFF(void){++randomCalls;return randomByte;}
static void prepare(unsigned technician,unsigned skill,unsigned wrecks)
{
 memset(&OriginalSavedState,0,sizeof OriginalSavedState);
 memset(CombatantCasualtyFlags,0,sizeof CombatantCasualtyFlags);
 for(unsigned id=0;id<PartySize;++id)Characters[id].name=Character_Dead;
 Characters[0].name=0;Characters[technician].name=0;Characters[technician].skillTech=(uint8_t)skill;
 for(unsigned id=0;id<MechRecordCount;++id){
  Mechs[id].name[0]=MECH_Destroyed;
  memset(Mechs[id].currentArmour,10,sizeof Mechs[id].currentArmour);
  memset(Mechs[id].maxArmour,10,sizeof Mechs[id].maxArmour);
  memset(Mechs[id].currentStructure,10,sizeof Mechs[id].currentStructure);
  memset(Mechs[id].maxStructure,10,sizeof Mechs[id].maxStructure);
 }
 Mechs[0].name[0]='L';
 for(unsigned id=0;id<wrecks;++id){
  CombatantCasualtyFlags[Enemy_All_CombatantId_Range_First+id]=TRUE;
  memset(Mechs[Enemy_Mech_Record_First+id].currentArmour,0,MechArmourLocationCount);
  memset(Mechs[Enemy_Mech_Record_First+id].currentStructure,0,MechStructureLocationCount);
 }
 CBills=100;prompts=randomCalls=numbers=0;lastNumber=0;randomByte=5;output[0]=0;
}
int main(void)
{
 prepare(0,0,1);Mechs[4].currentArmour[0]=7;
 Mechs[0].currentArmour[10]=0;Mechs[1].name[0]='C';Mechs[1].currentArmour[10]=0;
 Salvage_Armour_Dialog();
 check(Mechs[0].currentArmour[10]==7 && Mechs[1].currentArmour[10]==0);
 check(CBills==100 && randomCalls==0 && prompts==1); /*slot-zero payout bug*/
 check(strstr(output,"uses his tech training to scavenge armor")!=NULL);
 /* Negative deficit creates a pool even without wreck armour. */
 prepare(0,0,0);Mechs[0].currentArmour[10]=15;Mechs[1].name[0]='C';Mechs[1].currentArmour[10]=3;
 Salvage_Armour_Dialog();check(Mechs[0].currentArmour[10]==10 && Mechs[1].currentArmour[10]==8);
 prepare(0,SkillLevel_Good,1);Mechs[4].currentStructure[0]=6;Mechs[0].currentStructure[7]=0;
 Salvage_Armour_Dialog();check(Mechs[0].currentStructure[7]==0);
 prepare(0,SkillLevel_Excellent,1);Mechs[4].currentStructure[0]=6;Mechs[0].currentStructure[7]=0;
 Salvage_Armour_Dialog();check(Mechs[0].currentStructure[7]==6);
 check(strstr(output,", heat sinks, weapons and structure")!=NULL);
 /* Native sink restoration does not require a nonempty salvage pool. */
 prepare(0,SkillLevel_Average,0);Mechs[0].criticalSlots[0]=Destroyed_Heat_Sink;
 Salvage_Armour_Dialog();check(Mechs[0].criticalSlots[0]==Heat_Sink);
 prepare(0,SkillLevel_Average,1);Mechs[0].criticalSlots[0]=Destroyed_Heat_Sink;
 Mechs[0].currentStructure[0]=0;Mechs[4].criticalSlots[0]=Heat_Sink;
 Salvage_Armour_Dialog();check(Mechs[0].criticalSlots[0]==Destroyed_Heat_Sink);
 /* Good skill spends one matching component; no replacement of whole weapon. */
 prepare(0,SkillLevel_Good,1);Mechs[0].criticalSlots[0]=Mech_Small_Laser|Component_Destroyed;
 Mechs[0].criticalSlots[1]=Mech_Small_Laser|Component_Destroyed;Mechs[4].criticalSlots[0]=Mech_Small_Laser;
 Salvage_Armour_Dialog();check(Mechs[0].criticalSlots[0]==Mech_Small_Laser);
 check(Mechs[0].criticalSlots[1]==(Mech_Small_Laser|Component_Destroyed));
 check(Mechs[4].criticalSlots[0]==Mech_Small_Laser && Mechs[0].currentAmmo[0]==0);
 prepare(0,SkillLevel_Good,1);Mechs[0].criticalSlots[0]=Mech_Small_Laser|Component_Destroyed;
 Mechs[0].currentStructure[0]=0;Mechs[4].criticalSlots[0]=Mech_Small_Laser;
 Salvage_Armour_Dialog();check(Mechs[0].criticalSlots[0]==(Mech_Small_Laser|Component_Destroyed));
 prepare(0,SkillLevel_Average,1);Mechs[0].criticalSlots[0]=Mech_Small_Laser|Component_Destroyed;
 Mechs[4].criticalSlots[0]=Mech_Small_Laser;
 Salvage_Armour_Dialog();check(Mechs[0].criticalSlots[0]==(Mech_Small_Laser|Component_Destroyed));
 /* SRM-6 presence alone isn't unsupported: only an actual bucket access is.
  * Destroyed enemy components and intact friendly components skip that access. */
 prepare(0,SkillLevel_Good,1);Mechs[0].criticalSlots[0]=Mech_SRMissile6;
 Mechs[4].criticalSlots[0]=Mech_SRMissile6|Component_Destroyed;
 Salvage_Armour_Dialog();check(Mechs[0].criticalSlots[0]==Mech_SRMissile6);
 /* Ties retain the first living technician; dead and signed-negative skills lose. */
 prepare(1,SkillLevel_Good,1);Characters[2].name=0;Characters[2].skillTech=SkillLevel_Good;
 Characters[3].skillTech=SkillLevel_Excellent;
 Salvage_Armour_Dialog();check(CBills==195 && lastNumber==95 && randomCalls==1 && numbers==1 && prompts==2);
 check(strstr(output,"\x06\x0F C-bills worth of scrap metal from the destroyed 'Mech.")!=NULL);
 prepare(7,SkillLevel_Amateur,2);Salvage_Armour_Dialog();
 check(CBills==1430 && lastNumber==1330 && randomCalls==2);
 check(strstr(output,"destroyed 'Mechs.")!=NULL);
 prepare(1,255,1);Salvage_Armour_Dialog();check(CBills==100 && randomCalls==0);
 prepare(1,SkillLevel_Excellent,0);Salvage_Armour_Dialog();check(CBills==100 && randomCalls==0);
 prepare(1,SkillLevel_Amateur,1);CBills=UINT32_MAX-94;Salvage_Armour_Dialog();check(CBills==0);
 /* Non-wreck enemy records contribute neither materials nor payout. */
 prepare(0,0,0);Mechs[0].currentArmour[10]=0;Mechs[4].currentArmour[0]=255;
 Salvage_Armour_Dialog();check(Mechs[0].currentArmour[10]==0 && CBills==100);
 puts("Original complete component salvage scenarios passed");
 return 0;
}
