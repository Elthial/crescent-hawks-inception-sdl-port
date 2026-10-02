#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Sol: actual weapon parent, picker, range, layout, popup and text-formatting
 * methods. Presentation/preview/camera and user choices are isolated inputs. */
static unsigned keys,prompts,yesCalls,previews,choiceIndex,choiceCount;
static uint16_t actor,yesAnswer;
static struct {uint16_t panel,choice;} choices[12];
static char output[8192];
static void verify(int condition,unsigned line){if(!condition){fprintf(stderr,"Weapon parent mismatch line%u\n",line);exit(1);}}
#define check(x) verify(!!(x),__LINE__)
void Display_Text_From_Memory(uint8_t *text){size_t used=strlen(output),length=strlen((char *)text);check(used+length<sizeof output);memcpy(output+used,text,length+1);}
void Draw_Top_Graphic_Sidebar(void){}
void Draw_Menu_Border(uint16_t panel){check(panel<=5);}
void Set_Text_Colour_Bright_Green(void){TextColour=EGA_BrightGreen;}
void Prompt_And_Wait_For_Key(void){++prompts;}
uint16_t Keyboard_Get_ASCII_Hex_Input(void){++keys;return 13;}
uint16_t Prompt_Yes_No(uint16_t defaultYes){check(defaultYes==TRUE);++yesCalls;return yesAnswer;}
void Combat_Render_Movement_Preview(uint16_t id,uint16_t recenter){check(id<AllCombatantCount && recenter==TRUE);++previews;}
void Move_Map_View_To_Packed_Position(uint16_t x,uint16_t y){CrescentHawkMapPositionX=x;CrescentHawkMapPositionY=y;}
void Draw_Horizontal_EGA_Line(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t colour){(void)x0;(void)y0;(void)x1;(void)y1;(void)colour;}
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t x,uint16_t y,uint16_t foreground,uint16_t background){(void)x;(void)y;(void)foreground;(void)background;Display_Text_From_Memory(text);}
uint8_t DrawCall_Combat_Menu(uint16_t x,uint16_t y,uint16_t width,uint16_t colour){(void)x;(void)y;(void)width;(void)colour;return 0;}
uint16_t Display_Menu_Choices_And_Check(uint16_t panel){check(choiceIndex<choiceCount && panel==choices[choiceIndex].panel);return choices[choiceIndex++].choice;}
static void choice(uint16_t panel,uint16_t selected){check(choiceCount<12);choices[choiceCount].panel=panel;choices[choiceCount++].choice=selected;}
static void prepare(uint16_t attacker,unsigned count)
{
 memset(&OriginalSavedState,0,sizeof OriginalSavedState);
 memset(CombatWeaponTarget,255,sizeof CombatWeaponTarget);
 memset(CombatantActive,0,sizeof CombatantActive);
 memset(CombatantActionState,7,sizeof CombatantActionState);
 memset(MechHeatLevel,0,sizeof MechHeatLevel);
 for(unsigned id=0;id<MenuPanelLayoutCount;++id){MenuPanelLayouts[id].column=MenuPanelLayouts[id].row=0;MenuPanelLayouts[id].foreground=15;}
 MenuPanelLayouts[5].height=16;MenuPanelLayouts[5].top=1;MenuPanelContextInitialized=0;CurrentMenuLayoutIndex=0;
 TextRow=TextColumn=TextColour=0;GraphicsAdapter=GraphicsAdapter_Ega;EnemyTargetId=12;
 Mechs[0].name[0]='L';Mechs[4].name[0]='T';Mechs[0].currentAmmo[0]=255;
 for(unsigned slot=0;slot<count;++slot){Mechs[0].criticalSlots[slot]=Mech_Small_Laser;if(slot<MechWeaponOrdinalCount)Mechs[0].currentAmmo[slot]=255;}
 Characters[0].weapon=WeaponIndex_Pistol;Characters[8].weapon=WeaponIndex_Pistol;
 CombatantPackedX[attacker]=0x0220;CombatantPackedY[attacker]=0x3020;
 CombatantPackedX[12]=0x0224;CombatantPackedY[12]=0x3020;CombatantActive[12]=TRUE;
 CombatantPackedX[16]=0x0224;CombatantPackedY[16]=0x3020;CombatantActive[16]=TRUE;
 CrescentHawkMapPositionX=0x0220;CrescentHawkMapPositionY=0x3020;
 actor=attacker;yesAnswer=TRUE;keys=prompts=yesCalls=previews=choiceIndex=choiceCount=0;output[0]=0;
}
static void run(void){Combat_Weapon_UI(actor);check(choiceIndex==choiceCount);}
static void restored(void){check(CurrentMenuLayoutIndex==3 && TextColour==EGA_BrightWhite && CombatantActionState[actor]==0);}
int main(void)
{
 prepare(0,1);choice(5,1);run();restored();
 check(previews==1 && MenuPanelLayouts[5].height==3 && MenuPanelLayouts[5].top==21 && MenuControls[5].optionCount==2);
 check(strstr(output,"Weapon\t\vAmmo Target  Range") && strstr(output,"\006\017Done") && strstr(output,"Full"));
 prepare(0,0);choice(5,0);run();restored();check(MenuControls[5].optionCount==1);
 prepare(0,12);MechHeatLevel[0]=MechHeatShutdownLevel;
 for(unsigned slot=0;slot<CombatWeaponTargetSlots;++slot)CombatWeaponTarget[slot]=12;
 run();restored();check(prompts==1 && previews==0);
 for(unsigned slot=0;slot<CombatWeaponTargetSlots;++slot)check(CombatWeaponTarget[slot]==140);
 prepare(0,1);Mechs[0].sensorHits=2;run();check(CurrentMenuLayoutIndex==4 && keys==1 && previews==0);
 prepare(0,1);Mechs[0].sensorHits=1;choice(5,1);run();restored();check(keys==1 && strstr(output,"Accuracy is impaired."));
 prepare(0,1);Mechs[0].criticalSlots[0]=Mech_Small_Laser|Component_Destroyed;CombatWeaponTarget[0]=12;
 choice(5,0);choice(5,1);run();restored();check(CombatWeaponTarget[0]==255 && keys==1 && strstr(output,"That weapon has been destroyed."));
 prepare(0,1);Mechs[0].currentAmmo[0]=0;choice(5,0);choice(5,1);run();restored();
 check(keys==1 && strstr(output,"That weapon is out of ammunition."));
 prepare(0,11);Mechs[0].walkMove=0;choice(5,10);choice(5,11);run();restored();
 check(keys==1 && strstr(output,"That weapon is out of ammunition.")); /*ordinal10 aliases walk*/
 prepare(0,1);Mechs[0].currentAmmo[0]=7;CombatWeaponTarget[0]=140;choice(5,1);run();restored();
 check(CombatWeaponTarget[0]==12 && strstr(output,"  7")); /*rendering clears fired bit*/
 /* ASM0387/0694 share C33C+id*125: Mech panel must retain character-byte
  * aliases too. Independent native offsets are C62A-C614 and C6A7-C614. */
 for(unsigned target=6;target<=7;++target) {
     prepare(0,1);CombatWeaponTarget[0]=(uint8_t)(target|0x80);CombatantActive[target]=TRUE;
     unsigned nativeNameOffset=target==6?0x16:0x93;
     memcpy(OriginalSavedState.bytes+nativeNameOffset,"AliasedName",12);
     choice(5,1);run();restored();
     check(CombatWeaponTarget[0]==target && strstr(output,"AliasedName"));
 }
 prepare(0,1);CombatWeaponTarget[0]=12;CombatantActive[12]=FALSE;choice(5,1);run();restored();check(CombatWeaponTarget[0]==255);
 prepare(0,1);CombatWeaponTarget[0]=12;yesAnswer=FALSE;choice(5,0);choice(5,1);run();restored();check(yesCalls==1 && CombatWeaponTarget[0]==12);
 prepare(0,1);CombatWeaponTarget[0]=12;choice(5,0);choice(3,CombatTargetChoice_Cancel);choice(5,1);run();restored();
 check(yesCalls==1 && CombatWeaponTarget[0]==255); /*old target cleared before cancellation*/
 prepare(0,1);CombatantPackedX[12]=0x0260;choice(5,0);choice(3,CombatTargetChoice_Target);choice(5,1);run();restored();
 check(CombatWeaponTarget[0]==12 && strstr(output,"OUT")); /*range doesn't reject selection*/
 prepare(4,0);choice(3,CombatTargetChoice_Target);run();restored();check(CombatWeaponTarget[4*CombatWeaponTargetSlots]==12);
 prepare(4,0);CombatWeaponTarget[4*CombatWeaponTargetSlots]=16;CombatantActive[16]=FALSE;yesAnswer=FALSE;
 run();restored();check(yesCalls==1 && CombatWeaponTarget[4*CombatWeaponTargetSlots]==16 && strstr(output,"Human")); /*no infantry active revalidation*/
 prepare(4,0);CombatWeaponTarget[4*CombatWeaponTargetSlots]=8;yesAnswer=FALSE;
 run();restored();check(yesCalls==1 && CombatWeaponTarget[4*CombatWeaponTargetSlots]==8); /*native biased read resolves to Mech0, no modern side rejection*/
 prepare(4,0);CombatWeaponTarget[4*CombatWeaponTargetSlots]=6;yesAnswer=FALSE;
 OriginalSavedState.bytes[22]='X';OriginalSavedState.bytes[23]=0;
 run();restored();check(yesCalls==1 && strstr(output,"against a X at ")); /*native character-byte alias, not host Mechs[-2]*/
 puts("Original complete weapon menu and picker scenarios passed");
 return 0;
}
