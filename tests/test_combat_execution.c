#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned sounds[32],soundCount,compositions,presentations,retraces,lines,sprites,scenes,messages;
static uint16_t lastSprite,lastScene;
static uint8_t *lastText;
static void verify(int passed,unsigned line) {if(!passed){fprintf(stderr,"Effects parent mismatch line%u\n",line);exit(1);}}
#define check(x) verify(!!(x),__LINE__)
/* Sol: isolated presentation/audio boundaries, never production replacements.
 * Original effects parent, animation interpreter, effect registration, Mech
 * templates, WORD abs and octant helper are the actual C implementations. */
void Play_Sound_If_Enabled(uint16_t id){check(soundCount<32);sounds[soundCount++]=id;}
void Combat_Restore_Map_View_And_Draw_World(uint8_t *map){check(map!=NULL);++compositions;}
void EGA_DrawBox_Wrapper(void){++presentations;}
void Wait_For_N_Vertical_Retraces(uint16_t n){retraces+=n;}
void Draw_Combat_Sprites(uint16_t sprite,uint16_t x,uint16_t y){(void)x;(void)y;lastSprite=sprite;++sprites;}
void Draw_Clipped_Axis_Aligned_EGA_Line(uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t colour){
 check(x==x1 && y==y1);check(colour==EGA_BrightYellow || colour==EGA_BrightGreen || colour==EGA_BrightRed || colour==EGA_Magenta);++lines;
}
void Display_Animation_Scene(uint16_t scene,uint16_t playback){check(playback==AnimationPlayback_RestoreGameView);lastScene=scene;++scenes;}
void Map_Move_North(void){--CrescentHawkMapPositionY;}
void Map_Move_South(void){++CrescentHawkMapPositionY;}
void Map_Move_West(void){--CrescentHawkMapPositionX;}
void Map_Move_East(void){++CrescentHawkMapPositionX;}
void PosXY_OffsetGrid(uint16_t x,uint16_t y){(void)x;(void)y;}
void Combat_Copy_Map_Cache(uint8_t *map,uint16_t restore){check(map!=NULL && restore==FALSE);}
void EGA_Upload_Animated_Tile(uint8_t *source,uint16_t offset){(void)source;(void)offset;}
void Copy_Data_To_GraphicsMemory(void){}
void Draw_Menu_MultiSelect(void){}
void Draw_Message_Box(void){}
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text){lastText=text;}
uint16_t Keyboard_Get_ASCII_Hex_Input(void){return 13;}
void Display_Text_From_Memory(uint8_t *text){lastText=text;}
void Set_Text_Colour_Bright_Green(void){TextColour=10;}
void Draw_Top_Graphic_Sidebar(void){}
static uint8_t randomByte=5;
uint8_t Rand_0x00_to_0xFF(void){return randomByte;}
void Menu_Memory_Variables(uint16_t layout){check(layout==4);}

/* Sol: actual execution/effects/dice/range/path/occupancy/critical/ejection/
 * heat methods. Random bytes are controlled inputs. Map-cache production and
 * camera/presentation are isolated, so this is NOT full-map gameplay parity. */
static void prepare(uint16_t shooter,uint16_t target)
{
 memset(&OriginalSavedState,0,sizeof OriginalSavedState);
 memset(CombatantActive,0,sizeof CombatantActive);
 memset(CombatMovementOrders,255,sizeof CombatMovementOrders);
 memset(CombatMovementPlanBytes,CombatMovementPlanEnd,sizeof CombatMovementPlanBytes);
 memset(CombatMovementStepCursor,77,sizeof CombatMovementStepCursor);
 memset(CombatWeaponTarget,255,sizeof CombatWeaponTarget);
 memset(CombatantCasualtyFlags,0,sizeof CombatantCasualtyFlags);
 memset(CombatantVisibleOnScreen,0,sizeof CombatantVisibleOnScreen);
 memset(CombatantAnimationDirection,0,sizeof CombatantAnimationDirection);
 memset(MechHeatLevel,0,sizeof MechHeatLevel);
 memset(CombatWeaponHeat,0,sizeof CombatWeaponHeat);
 memset(MapTileUnderCombatant,0,sizeof MapTileUnderCombatant);
 memset(TerrainOverlapRows,0,sizeof TerrainOverlapRows);
 memset(CombatMap,0,MapCacheTileCount);
 for(unsigned id=0;id<AllCombatantCount;++id)CombatantPackedX[id]=CombatantPackedY[id]=65535;
 for(unsigned id=0;id<CharacterRecordCount;++id){Characters[id].name=0;Characters[id].health=100;Characters[id].weapon=WeaponIndex_Pistol;Characters[id].mechAssignment=Character_OnFoot;}
 for(unsigned id=0;id<MechRecordCount;++id){
  Mechs[id].name[0]='L';Mechs[id].tonnage=20;Mechs[id].pilotId=(uint8_t)(id<4?0:8);
  Mechs[id].riderId=MECH_NoRider;Mechs[id].engineHeatSinks=10;
  Mechs[id].currentActuators[0]=Mechs[id].currentActuators[1]=15;
  memset(Mechs[id].currentArmour,100,sizeof Mechs[id].currentArmour);
  memset(Mechs[id].currentStructure,100,sizeof Mechs[id].currentStructure);
  Mechs[id].criticalSlots[0]=Mech_Small_Laser;Mechs[id].currentAmmo[0]=10;
 }
 MainCharactersAlive=TRUE;CombatDisplayGraphics=FALSE;CombatMessageVerbosity=CombatMessage_None;
 GraphicsAdapter=GraphicsAdapter_Ega;ArenaRentalMechMode=FALSE;BlockingTileCodeThreshold=0x55;
 CombatSpeedSetting=0;MechDestroyedFlag=CombatNotificationLatch=ShowArmShotOffAnimation=0;
 CrescentHawkMapPositionX=0x0220;CrescentHawkMapPositionY=0x3020;
 CombatantPackedX[shooter]=0x0220;CombatantPackedY[shooter]=0x3020;
 CombatantPackedX[target]=0x0224;CombatantPackedY[target]=0x3020;
 CombatantActive[shooter]=CombatantActive[target]=TRUE;
 CombatWeaponTarget[shooter*CombatWeaponTargetSlots]=(uint8_t)target;
 soundCount=compositions=presentations=retraces=lines=sprites=scenes=messages=0;
 randomByte=5;
}
static void round_finished(void)
{
 check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
 check(CombatWeaponTarget[4*CombatWeaponTargetSlots]==16);
 check(presentations==0 && lines==0 && sprites==0);
}
static unsigned countImpactSounds(void)
{
 unsigned count=0;
 for(unsigned index=0;index<soundCount;++index) if(sounds[index]==Sound_InfantryUnknown) ++count;
 return count;
}
static void inheritedAttackResult(uint8_t rollByte)
{
 enum { FirstMech=0,RexInfantry=Friendly_Infantry_Combatant_Range_First+Character_Rex,
        TargetMech=Enemy_All_CombatantId_Range_First,TargetRecord=LanceSize,VisibleGraphics=1 };
 uint8_t armourAfterLaser[MechArmourLocationCount];
 unsigned laserImpacts;
 /* Control: an ordinary laser assigns native BP-56 before the skipped
  * personnel-versus-Mech branch. Presentation alone is isolated here. */
 prepare(FirstMech,TargetMech); randomByte=rollByte; CombatDisplayGraphics=VisibleGraphics;
 Characters[Character_Jason].mechAssignment=FirstMech;
 Characters[Enemy_Infantry_Record_First].mechAssignment=TargetRecord;
 Combat_Mechanics(FALSE);
 memcpy(armourAfterLaser,Mechs[TargetRecord].currentArmour,sizeof armourAfterLaser);
 laserImpacts=countImpactSounds();
 check(laserImpacts==(rollByte==5?1u:0u));
 prepare(FirstMech,TargetMech); randomByte=rollByte; CombatDisplayGraphics=VisibleGraphics;
 Characters[Character_Jason].mechAssignment=FirstMech;
 Characters[Enemy_Infantry_Record_First].mechAssignment=TargetRecord;
 Characters[Character_Rex].name=Character_Rex;
 Characters[Character_Rex].weapon=WeaponIndex_Pistol;
 CombatantActive[RexInfantry]=TRUE;
 CombatantPackedX[RexInfantry]=CombatantPackedX[FirstMech];
 CombatantPackedY[RexInfantry]=CombatantPackedY[FirstMech];
 CombatWeaponTarget[RexInfantry*CombatWeaponTargetSlots]=TargetMech;
 Combat_Mechanics(FALSE);
 /* Pistol bit80 bypasses damage AND the flag assignment. Its impact
  * sound therefore inherits the preceding laser's hit or miss result. */
 check(Mechs[FirstMech].currentAmmo[0]==9);
 check(!memcmp(armourAfterLaser,Mechs[TargetRecord].currentArmour,sizeof armourAfterLaser));
 check(countImpactSounds()==laserImpacts*2);
 check((CombatWeaponTarget[RexInfantry*CombatWeaponTargetSlots]&CombatTargetIdMask)==TargetMech);
}
int main(void)
{
 /* Two D6 at six plus3 =15 pistol damage; armour halves it once to7. */
 prepare(4,16);Characters[8].armourValue=10;
 Combat_Mechanics(FALSE);round_finished();
 check(Characters[8].health==93 && Characters[8].armourValue==3);
 check(WeaponProficiencyUseCount[0]==0 && LastWeaponProficiencyCategory[0]==1);
 /* Repeated use count wraps and raises the selected skill, not gunnery. */
 prepare(4,16);LastWeaponProficiencyCategory[0]=1;WeaponProficiencyUseCount[0]=255;
 Combat_Mechanics(FALSE);check(Characters[0].skillPistol==1 && WeaponProficiencyUseCount[0]==0);
 prepare(4,16);randomByte=0;
 Combat_Mechanics(FALSE);check(Characters[8].health==100 && Characters[8].armourValue==0);
 prepare(4,16);randomByte=0;Characters[0].skillPistol=2;
 Combat_Mechanics(FALSE);check(Characters[8].health==95); /*2D6 equals target: hit*/
 /* CBW/SAR signed-cover boundary: -3 halves to -2, not -1. At this
  * controlled 2D6 roll, -4/-3 cover hits; -2/-1/0/positive cover misses. */
 for(int cover=-4;cover<=2;++cover) {
     prepare(4,16);randomByte=0;TerrainOverlapRows[16]=(uint8_t)cover;
     Combat_Mechanics(FALSE);check(Characters[8].health==(cover<=-3?95:100));
 }
 /* Native shared damage is halved successively across four SMG repeats. */
 prepare(4,16);Characters[0].weapon=WeaponIndex_SubmachineGun;Characters[8].armourValue=100;
 Combat_Mechanics(FALSE);check(Characters[8].health==84 && Characters[8].armourValue==84);
 prepare(4,16);Characters[8].health=1;
 Combat_Mechanics(FALSE);check(Characters[8].name==Character_Dead && Characters[8].health==0);
 check(CombatantCasualtyFlags[16] && !CombatantActive[16] && MapEffectSpriteIndex[0]==Sprite_Impact_Small);
 check(CombatWeaponTarget[4*CombatWeaponTargetSlots]==255);
 /* Mech Small Laser: ammo once, facing/12-roll location20, damage3. */
 prepare(0,12);Combat_Mechanics(FALSE);
 check(Mechs[0].currentAmmo[0]==9 && ((uint8_t *)&Mechs[4])[20]==97);
 check(CombatWeaponHeat[0]==0 && MechHeatLevel[0]==0);
 prepare(0,12);randomByte=0;Combat_Mechanics(FALSE);
 check(Mechs[0].currentAmmo[0]==9 && ((uint8_t *)&Mechs[4])[20]==100);
 prepare(12,0);Combat_Mechanics(FALSE);
 check(Mechs[4].currentAmmo[0]==9 && ((uint8_t *)&Mechs[0])[20]==97); /*enemy record bias*/
 prepare(0,12);MechHeatLevel[0]=MechHeatShutdownLevel;Combat_Mechanics(FALSE);
 check(Mechs[0].currentAmmo[0]==10); /*shutdown heat inhibits firing*/
 prepare(0,12);CombatantPackedX[12]=0x0260;Combat_Mechanics(FALSE);
 check(Mechs[0].currentAmmo[0]==10); /*out of range consumes no ammo*/
 /* Armour overflow reaches an already empty fatal section. The unspecified
  * transfer AX is stored but never dereferenced after real ejection. */
 prepare(0,12);((uint8_t *)&Mechs[4])[20]=0;((uint8_t *)&Mechs[4])[31]=0;
 Combat_Mechanics(FALSE);check(Mechs[4].name[0]==MECH_Destroyed && !CombatantActive[12]);
 check(MapEffectSpriteIndex[0]==Sprite_Locust_Wreck);
 /* The unknown entry flag is irrelevant to the disabled-graphics gate. */
 prepare(4,12);Combat_Mechanics(FALSE);check(((uint8_t *)&Mechs[4])[20]==100);
 /* Portable first-use presentation has no inherited hit impact. The actual
  * mechanics/effects run with graphics enabled; the native no-damage branch
  * is retained. Later assigned-result carry remains checked below. */
 prepare(4,12);CombatDisplayGraphics=1;
 Combat_Mechanics(FALSE);
 check(((uint8_t *)&Mechs[4])[20]==100 && countImpactSounds()==0);
 check(Characters[0].health==100 && Mechs[4].currentStructure[0]==100);
 /* Execute one real packed step, then sentinel ends the generated plan. */
 prepare(4,16);CombatWeaponTarget[4*CombatWeaponTargetSlots]=255;
 CombatMovementPlanBytes[4*CombatMovementPlanBytesPerUnit]=1;
 CombatMovementPlanBytes[4*CombatMovementPlanBytesPerUnit+1]=0;
 Combat_Mechanics(FALSE);check(CombatantPackedX[4]==0x0221 && CombatMovementStepCursor[4]==1);
 /* Native mapped transfers and fatal flag; don't certify unspecified AX. */
 const uint16_t transfer[]={0x1D,0x20,0x22,0x12,0x15,0x12,0,0,0x17,0x15,0x17};
 for(uint16_t offset=0x11;offset<=0x18;++offset)check(Combat_StructureHit(offset)==offset+11);
 for(uint16_t offset=0x19;offset<=0x23;++offset){
  MechDestroyedFlag=0;uint16_t next=Combat_StructureHit(offset);
  if(offset==0x1F || offset==0x20)check(MechDestroyedFlag);else check(next==transfer[offset-0x19] && !MechDestroyedFlag);
 }
 inheritedAttackResult(5); /* Assigned hit, followed by no-damage Pistol against Mech. */
 inheritedAttackResult(0); /* Assigned miss; the Pistol must not invent an impact. */
 puts("Original complete execution parent scenarios passed");
 return 0;
}
