#include "game.h"
#include "dos.h"

int8_t TrainingRubbleTargetX[TrainingRubbleTargetCount]={29,55,53,60,59,48,32,31};
int8_t TrainingRubbleTargetY[TrainingRubbleTargetCount]={15,17,24,33,44,46,48,27};

/* Original0FDC:0629..0D48. Original mission numbers0..9 are the caller
 * contract. Locals initialized for host warning cleanliness are assigned
 * natively before any use on these paths; no residual stack value is needed.
 * Generation/combat remain their original parents, not replacement algorithms. */
void Mech_Mission(uint16_t mission)
{
    uint16_t timer=0,rubbleTarget=0,savedTile=0,rubbleTile=0,distinctAttempts=0;
    uint16_t attempted[LanceSize]={0};
    uint16_t warned=FALSE,objectiveResolved=FALSE;
    switch(mission) {
    case Mission_SoutheastCorner:
        savedTile=MapFileTiles[TrainingTemporaryTileIndex]; MapFileTiles[TrainingTemporaryTileIndex]=0;
        Character_Movement_On_Map(FALSE); break;
    case Mission_RubblePickup: {
        rubbleTarget=Rand_0x00_to_0xFF()&(TrainingRubbleTargetCount-1);
        uint16_t x=(uint16_t)(int16_t)TrainingRubbleTargetX[rubbleTarget],y=(uint16_t)(int16_t)TrainingRubbleTargetY[rubbleTarget];
        /* Eight-by-eight blocks, eight blocks per row in this64-cell map. */
        rubbleTile=(uint16_t)((y&TrainingRubbleBlockCoordinateMask)*MapBlockTileCount+
            (y%MapBlockHeight)*MapBlockWidth+(x&TrainingRubbleBlockCoordinateMask)*MapBlockWidth+x%MapBlockWidth);
        savedTile=MapFileTiles[rubbleTile]; MapFileTiles[rubbleTile]=TrainingRubbleTile;
        if(Mechs[0].name[0]=='L') {
            Draw_Message_Box(); Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Other cadets snicker as you leave the training center.");
            (void)Keyboard_Get_ASCII_Hex_Input();
        }
        Character_Movement_On_Map(FALSE); break;
    }
    case Mission_DisabledLocust:
        Mission_GenerateEnemies(1,0);
        Mechs[Enemy_Mech_Record_First].currentActuators[1]=0; Mechs[Enemy_Mech_Record_First].currentActuators[0]=0;
        for(uint16_t ammo=0;ammo<MechWeaponOrdinalCount;++ammo) Mechs[Enemy_Mech_Record_First].currentAmmo[ammo]=0;
        Mechs[Enemy_Mech_Record_First].jumpMove=0; Mechs[Enemy_Mech_Record_First].walkMove=0;
        Combat_Run_Encounter(MissionCombat_Training); Restore_Mission_Map_View_After_Combat(); objectiveResolved=TRUE; break;
    case Mission_InfantryRobots:
        Mission_GenerateEnemies(0,(Rand_0x00_to_0xFF()&MissionRobotCountRandomMask)+MissionRobotCountBase);
        Combat_Run_Encounter(MissionCombat_Training); Restore_Mission_Map_View_After_Combat(); objectiveResolved=TRUE; break;
    case Mission_LocustWithoutComputer:
        DisableComputerControl=TRUE; /* Original fall-through into common setup. */
    case Mission_TwoLocusts: case Mission_ThreeLocusts: case Mission_KuritaAttack:
        if(mission==Mission_ThreeLocusts && (Rand_0x00_to_0xFF()&1)!=0) KuritaAttackFlag=TRUE;
        if(mission==Mission_KuritaAttack) KuritaAttackFlag=TRUE;
        if(KuritaAttackFlag!=FALSE)
            for(uint16_t tile=KuritaAttackFirstTile;tile<MapFileMaximumTileBytes;++tile) MapFileTiles[tile]=KuritaAttackTile;
        Mission_GenerateEnemies(mission-Mission_InfantryRobots,0);
        Combat_Run_Encounter(MissionCombat_Training); Restore_Mission_Map_View_After_Combat();
        DisableComputerControl=FALSE; objectiveResolved=TRUE; break;
    case Mission_Arena:
        Mission_GenerateEnemies(MissionArenaDeployment,0); Combat_Run_Encounter(MissionCombat_Arena); objectiveResolved=TRUE; break;
    case Mission_Jailbreak:
        DrawJailMissionParkedMechs=TRUE;
        PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
        Copy_Data_To_GraphicsMemory(); Draw_Infantry_And_Mechs(); EGA_DrawBox_Wrapper(); break;
    }
    uint16_t exitLoop=KuritaAttackFlag;
    if(mission==Mission_Arena) exitLoop=TRUE;
    int16_t updateCountdown=0;
    while(exitLoop==FALSE) {
        if(CrescentHawkMapPositionX<=TrainingHangarMaxX && CrescentHawkMapPositionY>=TrainingHangarMinY && CrescentHawkMapPositionY<=TrainingHangarMaxY) {
            if(objectiveResolved!=FALSE) exitLoop=TRUE;
            else {
                if(warned==FALSE) {
                    Draw_Message_Box(); Display_Text_From_Memory((uint8_t *)"Don't come back here until you complete your mission!");
                    Wait_For_50Hz_Then_Check_Input(); (void)Keyboard_Get_ASCII_Hex_Input();
                }
                warned=TRUE;
            }
        }
        if(exitLoop!=FALSE) continue;
        uint16_t updateWorld=FALSE;
        if(Pending_Input()!=FALSE) {
            uint16_t command=Keyboard_Convert_To_MoveCommands(Keyboard_Get_ASCII_Hex_Input());
            Drain_Pending_Keyboard_Input(); Update_Friendly_Movement_Animations(command);
            Character_Movement_On_Map(command); updateWorld=TRUE;
        } else {
            Wait_For_N_Vertical_Retraces(1); updateCountdown=(int16_t)(uint16_t)(updateCountdown-1);
            if(updateCountdown<0) updateWorld=TRUE;
        }
        if(updateWorld==FALSE) continue;
        updateCountdown=MissionWorldUpdateCountdown;
        Update_Animated_Map_Tiles(); ++MissionNpcUpdatePhase;
        if(MissionNpcUpdatePhase==MissionNpcUpdateTriggerPhase) Update_Roaming_Map_Npcs();
        MissionNpcUpdatePhase&=MissionNpcUpdatePhaseMask;
        Copy_Data_To_GraphicsMemory(); Draw_Infantry_And_Mechs(); EGA_DrawBox_Wrapper();
        if(mission==Mission_SoutheastCorner) {
            ++timer;
            if(TerrainOverlapRows[0]!=0) {
                /* Native SAR3 rounds negative values down, not toward zero. */
                int16_t rows=(int8_t)TerrainOverlapRows[0],delay=rows/MissionTerrainDelayDivisor;
                if(rows<0 && rows%MissionTerrainDelayDivisor!=0) --delay;
                timer=(uint16_t)(timer+delay);
            }
            if(CrescentHawkMapPositionX>=TrainingSoutheastX && CrescentHawkMapPositionY>=TrainingSoutheastY && objectiveResolved==FALSE) {
                ++objectiveResolved; Draw_Message_Box();
                Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You've made it to the southeast corner.  Now get back to the training center!");
                (void)Keyboard_Get_ASCII_Hex_Input();
            }
            continue;
        }
        if(mission==Mission_RubblePickup) {
            int16_t x=(CrescentHawkMapPositionX&PackedPositionLocalMask)/2,y=(CrescentHawkMapPositionY&PackedPositionLocalMask)/2;
            int16_t targetX=TrainingRubbleTargetX[rubbleTarget],targetY=TrainingRubbleTargetY[rubbleTarget];
            if(x<targetX-1 || x>targetX || y!=targetY+1 || objectiveResolved!=FALSE) continue;
            ++objectiveResolved; Draw_Message_Box();
            if(Mechs[0].name[0]=='L') Display_Text_From_Memory((uint8_t *)"You realize your Locust has no hands and can't pick it up.");
            else { Display_Text_From_Memory((uint8_t *)"You use your 'Mech's hands to retrieve the rubble."); MapFileTiles[rubbleTile]=(uint8_t)savedTile; }
            Wait_For_50Hz_Then_Check_Input(); (void)Keyboard_Get_ASCII_Hex_Input(); continue;
        }
        if(mission==Mission_Jailbreak) {
            uint16_t oldTimer=timer++;
            if((int16_t)oldTimer>=JailEncounterUpdateLimit) {
                Mission_GenerateEnemies(0,MissionJailInfantryDeployment); Combat_Run_Encounter(MissionCombat_Jailbreak); timer=0;
                if(MainCharactersAlive==FALSE) { exitLoop=TRUE; objectiveResolved=TRUE; }
            }
            if(MainCharactersAlive!=FALSE && CrescentHawkMapPositionY==JailInteractionY)
                for(uint16_t mech=0;mech<LanceSize;++mech)
                    if(CrescentHawkMapPositionX==JailInteractionFirstX+mech*JailParkedMechSpacing && attempted[mech]==FALSE) {
                        attempted[mech]=TRUE; Menu_Memory_Variables(HolodiskPromptLayout); Draw_Top_Graphic_Sidebar(); Draw_Menu_Border(0);
                        Display_Text_From_Memory((uint8_t *)"You quickly scramble up the boarding ladders of this 'Mech, ");
                        Display_Text_From_Memory((uint8_t *)"hoping that it will function when you get to the cockpit.");
                        Wait_For_50Hz_Then_Check_Input(); (void)Keyboard_Get_ASCII_Hex_Input();
                        Display_Text_From_Memory((uint8_t *)"\r\rYou flip the activation switch...");
                        uint16_t oldAttempts=distinctAttempts++;
                        if(oldAttempts!=JailSuccessfulAttemptIndex) {
                            Play_Failed_Mech_Startup_Scene(); Menu_Memory_Variables(HolodiskPromptLayout);
                            Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"but this 'Mech refuses to start.");
                        } else {
                            Display_Animation_Scene(AnimationO00_MechStartUp,AnimationPlayback_RestoreGameView); Menu_Memory_Variables(HolodiskPromptLayout);
                            Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"and the 'Mech starts up!"); objectiveResolved=TRUE; exitLoop=TRUE;
                        }
                        (void)Keyboard_Get_ASCII_Hex_Input();
                    }
        }
    }
    TrainingMissionPassed=FALSE;
    if(mission==Mission_SoutheastCorner) {
        if(Mechs[0].name[0]=='L') timer=(uint16_t)(timer-TrainingLocustTimeAllowance);
        if(objectiveResolved!=FALSE && (int16_t)timer<TrainingSoutheastPassTime) TrainingMissionPassed=TRUE;
        MapFileTiles[TrainingTemporaryTileIndex]=(uint8_t)savedTile;
    } else if(mission==Mission_RubblePickup) {
        MapFileTiles[rubbleTile]=(uint8_t)savedTile;
        if(Mechs[0].name[0]!='L') TrainingMissionPassed=TRUE;
        int16_t piloting=(int8_t)Characters[0].skillPiloting;
        if(piloting<SkillLevel_Good) ++piloting;
        Characters[0].skillPiloting=(uint8_t)piloting;
    } else if((int16_t)mission>=Mission_DisabledLocust && (int16_t)mission<=Mission_KuritaAttack) {
        if(Mechs[0].name[0]!=MECH_Destroyed && FriendlyPersonnelWithdrawal==FALSE) TrainingMissionPassed=TRUE;
        if((int16_t)mission>Mission_LocustWithoutComputer) {
            int16_t gunnery=(int8_t)Characters[0].skillGunnery;
            if(gunnery<SkillLevel_Excellent) gunnery+=Rand_0x00_to_0xFF()&1;
            Characters[0].skillGunnery=(uint8_t)gunnery;
            int16_t piloting=(int8_t)Characters[0].skillPiloting;
            if(piloting<SkillLevel_Excellent) piloting+=Rand_0x00_to_0xFF()&1;
            Characters[0].skillPiloting=(uint8_t)piloting;
        }
        if(KuritaAttackFlag!=FALSE) { KuritaDestroyedCitadel=TRUE; if(Mechs[0].name[0]!=MECH_Destroyed) TrainingMechSurvivedKuritaAttack=TRUE; }
        KuritaAttackFlag=FALSE;
    }
    DrawJailMissionParkedMechs=FALSE;
    if(BTStatsAssetLoaded!=FALSE) Load_And_Decode_Indexed_BLD(BldFileIndex);
}
