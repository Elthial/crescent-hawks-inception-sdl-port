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
void Combat_CombatMessageVerbosityFilter(uint8_t *text){lastText=text;++messages;}
void Combat_Restore_Map_View_And_Draw_World(uint8_t *map){check(map!=NULL);++compositions;}
void EGA_DrawBox_Wrapper(void){++presentations;}
void Wait_For_N_Vertical_Retraces(uint16_t n){retraces+=n;}
void Draw_Combat_Sprites(uint16_t sprite,uint16_t x,uint16_t y){(void)x;(void)y;lastSprite=sprite;++sprites;}
void Draw_Clipped_Axis_Aligned_EGA_Line(uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t colour){
 check(x==x1 && y==y1);check(colour==EGA_BrightYellow || colour==EGA_BrightGreen || colour==EGA_BrightRed || colour==EGA_Magenta);++lines;
}
void Display_Animation_Scene(uint16_t scene,uint16_t playback){check(playback==AnimationPlayback_RestoreGameView);lastScene=scene;++scenes;}
void Move_Map_View_To_Packed_Position(uint16_t x,uint16_t y){CrescentHawkMapPositionX=x;CrescentHawkMapPositionY=y;}
void PosXY_OffsetGrid(uint16_t x,uint16_t y){(void)x;(void)y;}
void Combat_Copy_Map_Cache(uint8_t *map,uint16_t restore){check(map!=NULL && restore==FALSE);}
void EGA_Upload_Animated_Tile(uint8_t *source,uint16_t offset){(void)source;(void)offset;}
void Copy_Data_To_GraphicsMemory(void){}
void Draw_Menu_MultiSelect(void){}
void Draw_Message_Box(void){}
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text){lastText=text;}
uint16_t Keyboard_Get_ASCII_Hex_Input(void){return 13;}
void Menu_Memory_Variables(uint16_t layout){check(layout==4);}
static uint8_t savedMap[1];
static void reset(void){
 memset(&OriginalSavedState,0,sizeof OriginalSavedState);
 memset(CombatantActive,0,sizeof CombatantActive);memset(CombatWeaponTarget,255,sizeof CombatWeaponTarget);
 memset(CombatantVisibleOnScreen,0,sizeof CombatantVisibleOnScreen);
 memset(CombatantSpriteFamilyOffset,0,sizeof CombatantSpriteFamilyOffset);
 memset(CombatantCasualtyFlags,0,sizeof CombatantCasualtyFlags);
 memset(CombatantSpriteFrame,0,sizeof CombatantSpriteFrame);
 memset(CombatantAnimationDirection,0,sizeof CombatantAnimationDirection);
 MainCharactersAlive=TRUE;GraphicsAdapter=GraphicsAdapter_Ega;CombatDisplayGraphics=1;
 Characters[Character_Jason].mechAssignment=Character_OnFoot;
 for(unsigned i=0;i<AllCombatantCount;++i){CombatantPackedX[i]=0x0120;CombatantPackedY[i]=0x2020;}
 soundCount=compositions=presentations=retraces=lines=sprites=scenes=messages=0;lastText=NULL;
 ArenaRentalMechMode=FALSE;
}
int main(void){
 for(unsigned raw=0;raw<65536;++raw){int16_t value=(int16_t)(uint16_t)raw;check(Word_Absolute(value)==(value<0?(int16_t)(uint16_t)(0u-raw):value));}
 /* All ordinary weapon/direction paths: hidden units still advance attacks
  * and select sound, but skip traveling effects and impact playback. */
 for(uint16_t weapon=0;weapon<WeaponRecordCount;++weapon)for(uint16_t direction=0;direction<CompassDirectionCount;++direction){
  reset();uint16_t shooter=weapon<PersonnelAttackWeaponCount?4:0;CombatantSpriteFrame[shooter]=51;
  Combat_AudioVisual_Effects(shooter,16,weapon,direction,TRUE,FALSE,FALSE,8,FALSE,savedMap);
  check(compositions==1 && presentations==0 && retraces==0 && sprites==0 && lines==0);
  check(CombatantAnimationSelector[shooter]==255);
  if(shooter==0)check(CombatantSpriteFrame[shooter]==51);
  check(!CombatantCasualtyFlags[16] && MainCharactersAlive);
  if(weapon>=WeaponIndex_LRM5 && weapon<=WeaponIndex_SRM6)check(soundCount==1 && sounds[0]==Sound_Missile);
  if(weapon==WeaponIndex_MachineGun || (weapon>=WeaponIndex_Autocannon2 && weapon<=WeaponIndex_Autocannon20))
   check(soundCount==1 && sounds[0]==Sound_RepeatingProjectile);
 }
 reset();CombatDisplayGraphics=FALSE;
 for(unsigned actor=0;actor<AllCombatantCount;++actor)for(unsigned slot=0;slot<CombatWeaponTargetSlots;++slot)
  CombatWeaponTarget[actor*CombatWeaponTargetSlots+slot]=(uint8_t)(16 | ((slot&1)?128:0));
 CombatantActive[16]=TRUE;
 Combat_AudioVisual_Effects(4,16,7,0,FALSE,TRUE,FALSE,8,TRUE,savedMap);
 check(messages==1 && !strcmp((char *)lastText,"\rKilled him!"));
 check(Characters[8].name==Character_Dead && !CombatantActive[16] && CombatantCasualtyFlags[16]);
 check(CombatantPackedX[16]==65535 && CombatantPackedY[16]==65535);
 check(CombatantSpriteFrame[16]==Sprite_Impact_Small && MapEffectSpriteIndex[0]==Sprite_Impact_Small);
 for(unsigned i=0;i<sizeof CombatWeaponTarget;++i)check(CombatWeaponTarget[i]==255);
 check(compositions==0 && soundCount==0 && MainCharactersAlive);
 reset();CombatDisplayGraphics=FALSE;
 Combat_AudioVisual_Effects(12,4,7,0,FALSE,TRUE,FALSE,Character_Jason,TRUE,savedMap);
 check(!MainCharactersAlive);
 reset();CombatDisplayGraphics=FALSE;CombatantPackedX[12]=0x0100;CombatantPackedY[12]=0x2000;
 Combat_AudioVisual_Effects(4,12,7,0,FALSE,FALSE,TRUE,4,TRUE,savedMap);
 check(MapEffectSpriteIndex[0]==Sprite_Locust_Wreck && MapEffectPositionXLow[0]==127 && MapEffectPositionYLow[0]==127);
 check(MapEffectPackedPage[0]==0x10 && soundCount==1 && sounds[0]==Sound_TerrainDamageUnknown);
 reset();CombatDisplayGraphics=FALSE;ArenaRentalMechMode=TRUE;
 Combat_AudioVisual_Effects(0,13,15,0,FALSE,FALSE,TRUE,5,TRUE,savedMap);
 check(!ArenaRentalMechMode && ArenaEscapeAllowed);
 check(!memcmp(&Mechs[6],&MechRefs[MechRef_UrbanMech],sizeof(Mech)));
 check(!memcmp(&Mechs[7],&MechRefs[MechRef_UrbanMech],sizeof(Mech)));
 check(CombatantActive[14] && CombatantActive[15] && !CombatantActive[13]);
 check(CombatantPackedY[14]==0x8030 && CombatantPackedY[15]==0x8070);
 check(CombatantAnimationCursors[14].offset==0x02A0 && CombatantMovementDirection[14]==4 && CombatantMovementDirection[15]==0);
 check(soundCount==1 && sounds[0]==Sound_ArenaDestroyedUnknown);
 reset();CombatantVisibleOnScreen[4]=CombatantVisibleOnScreen[16]=TRUE;
 CombatantScreenPixelX[4]=120;CombatantScreenPixelY[4]=100;
 CombatantScreenPixelX[16]=148;CombatantScreenPixelY[16]=100;
 Combat_AudioVisual_Effects(4,16,WeaponIndex_LaserPistol,2,FALSE,FALSE,FALSE,8,FALSE,savedMap);
 check(lines==24 && sprites==0 && soundCount==1 && sounds[0]==Sound_LaserUnknown);
 reset();CombatantVisibleOnScreen[4]=CombatantVisibleOnScreen[16]=TRUE;
 CombatantScreenPixelX[4]=120;CombatantScreenPixelY[4]=100;
 CombatantScreenPixelX[16]=148;CombatantScreenPixelY[16]=100;
 Combat_AudioVisual_Effects(4,16,WeaponIndex_PersonnelSRM,2,TRUE,FALSE,FALSE,8,TRUE,savedMap);
 check(sprites==10 && lastSprite==374 && scenes==0 && retraces==25);
 check(soundCount==2 && sounds[0]==Sound_Missile && sounds[1]==Sound_InfantryUnknown);
 puts("Complete original EGA effects parent: hidden attack paths, casualty/wreck/arena cleanup, beam, missile and impact playback passed.");
 return 0;
}
