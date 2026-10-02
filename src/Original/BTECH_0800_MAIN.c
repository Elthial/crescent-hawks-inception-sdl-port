#include "game.h"
#include "dos.h"

/* Sol: Original0800:29F5..2A2A. The allowance is an unaligned little-endian
 * WORD in persistent script state; DX is cleared, not sign-extended. */
uint32_t Get_CBill_Allowance_Wealth_Limit(void)
{
    return (uint32_t)(PersistentState.fields.allowanceLow
        | ((uint16_t)PersistentState.fields.allowanceHigh << 8));
}

/* Sol: Original0800:0000..051A, complete ASM branch review. Input advances
 * world time immediately; idle time advances after the signed countdown expires.
 * Original financial comparisons and neighbouring fog writes are retained. */
void Main_Game_Loop(uint16_t disableInput)
{
    DisableInput=disableInput;
    ExitMainLoop=FALSE;
    Display_Animation_Scene(AnimationO15_LyranBlastDoor,AnimationPlayback_RestoreGameView);
    uint16_t idleWorldTickCountdown=0;
    while (!ExitMainLoop) {
        uint16_t runWorldTick=FALSE;
        if (!Pending_Input()) {
            Wait_For_N_Vertical_Retraces(1);
            --idleWorldTickCountdown;
            if ((int16_t)idleWorldTickCountdown<0) runWorldTick=TRUE;
        } else {
            uint16_t key=Keyboard_Get_ASCII_Hex_Input();
            Drain_Pending_Keyboard_Input();
            key=Keyboard_Convert_To_MoveCommands(key);
            Menu_Memory_Variables(4);
            runWorldTick=TRUE;
            uint16_t step=0;
            while ((int16_t)step<(int16_t)ExplorationStepsPerInput) {
                if (key!=Command_Pause) Character_Movement_On_Map(key);
                Draw_Infantry_And_Mechs();
                if (MessageBoxOpen) step=(uint16_t)(ExplorationStepsPerInput+1);
                ++step; /* Native WORD increment, even after the message assignment. */
            }
            if (!MessageBoxOpen) {
                for (uint16_t direction=0;direction<CompassDirectionCount;++direction)
                    if (MovementCommandByCompass[direction]==key) {
                        TextColour=EGA_BrightWhite;
                        Display_Text_From_Memory(CompassDirectionText[direction]);
                    }
            }
            MessageBoxOpen=FALSE;
            Update_Friendly_Movement_Animations(key);
            if (key==Command_Pause) Game_Pause_Menu();
            /* Y page nibble *128 fog bytes: shift12-to7. X page *1 byte. */
            uint16_t fogIndex=(uint16_t)((CrescentHawkMapPositionY&PackedPositionYRegionMask)>>5);
            fogIndex=(uint16_t)(fogIndex+(CrescentHawkMapPositionX>>8));
            if (HasMapper && !InsideStarLeagueCache) {
                /* Reveal eight local Y subrows of the current world-region row. */
                for (uint16_t row=0;row<MapFogSubrowsPerWorldRegion;++row)
                    MapFogOfWar[fogIndex+row*MapFogOfWarRowBytes]=FogOfWar_Visible;
            } else {
                uint8_t mask=FogOfWarColumnBitMask[
                    (CrescentHawkMapPositionX&FogOfWarLocalColumnMask)>>FogOfWarLocalColumnShift];
                /* Native OR of Y bits occurs BEFORE addition of X's high byte. */
                fogIndex=(uint16_t)(((CrescentHawkMapPositionY&PackedPositionYRegionMask)>>5
                    | (CrescentHawkMapPositionY&FogOfWarLocalRowMask))
                    +(CrescentHawkMapPositionX>>8));
                MapFogOfWar[fogIndex]|=mask;
                /* CAFC/CB1C are real aliases, including overflow into adjacent
                 * records/state at the north/south extremes. Do not clamp them. */
                size_t storageIndex=offsetof(WorldMapStateStorage,fields.fog)+fogIndex;
                if (CrescentHawkMapPositionY!=0)
                    WorldMapState.bytes[storageIndex-MapFogOfWarRowBytes]|=mask;
                if (CrescentHawkMapPositionY<PackedWorldLastY)
                    WorldMapState.bytes[storageIndex+MapFogOfWarRowBytes]|=mask;
            }
        }
        if (!runWorldTick) continue;
        if (PartyHealthRecoveryTimer) --PartyHealthRecoveryTimer;
        if (StarportCountdown) {
            uint8_t previousLow=PersistentState.fields.countdownLow;
            --PersistentState.fields.countdownLow;
            if (!previousLow) --PersistentState.fields.countdownHigh;
            StarportCountdown=(uint8_t)(PersistentState.fields.countdownLow
                | PersistentState.fields.countdownHigh);
            if (!StarportCountdown
                && CrescentHawkMapPositionX>=StarportPatchFirstWorldX
                && CrescentHawkMapPositionX<StarportPatchPastLastWorldX
                && CrescentHawkMapPositionY>=StarportPatchFirstWorldY
                && CrescentHawkMapPositionY<StarportPatchPastLastWorldY)
                Starport_MapPatch_SaveApply_Or_Restore(TRUE);
        }
        uint16_t encounterRoll=Rand_0x00_to_0xFF();
        if (!(encounterRoll&(uint16_t)(int16_t)(int8_t)TraitorBattleProbability)
            && KuritaDestroyedCitadel && !InsideStarLeagueCache)
            Combat_Run_Encounter(CombatEncounter_Roaming);
        if (UnclassifiedWorldCountdownA) --UnclassifiedWorldCountdownA;
        if (TrainingMissionAndQuizCooldown) --TrainingMissionAndQuizCooldown;
        if (SchoolTrainingCooldown) --SchoolTrainingCooldown;
        if (UnclassifiedWorldCountdownB) --UnclassifiedWorldCountdownB;
        uint8_t financeCountdownBeforeDecrement=ComstarFinanceCountdown;
        --ComstarFinanceCountdown;
        if (!financeCountdownBeforeDecrement) {
            uint32_t totalWealth=CBills+StockBalances[0]+StockBalances[1]+StockBalances[2];
            uint32_t allowanceLimit;
            if (!KuritaDestroyedCitadel) {
                allowanceLimit=Get_CBill_Allowance_Wealth_Limit();
                /* Native signed high WORD, then unsigned low WORD. */
                if ((int16_t)(allowanceLimit>>16)>(int16_t)(totalWealth>>16)
                    || ((uint16_t)(allowanceLimit>>16)==(uint16_t)(totalWealth>>16)
                        && (uint16_t)allowanceLimit>(uint16_t)totalWealth))
                    CBills+=ComstarAllowancePaymentCBills;
                Display_Text_CBill_Balance();
            }
            if (!KuritaDestroyedCitadel) {
                allowanceLimit=Get_CBill_Allowance_Wealth_Limit();
                if ((int16_t)(allowanceLimit>>16)<(int16_t)(totalWealth>>16)
                    || ((uint16_t)(allowanceLimit>>16)==(uint16_t)(totalWealth>>16)
                        && (uint16_t)allowanceLimit<=(uint16_t)totalWealth))
                    goto UpdateWorldAnimations;
            }
            for (uint16_t stock=0;stock<StockCompanyCount;++stock) {
                uint16_t roll=Rand_0x00_to_0xFF();
                /* Native CWD sign-extends the WORD factor into unsigned DWORD
                 * runtime arithmetic. Tables' initial factors are positive. */
                uint32_t factor=(uint32_t)(int32_t)(int16_t)StockChangeFactor[stock];
                if (roll&StockChangeRollMask[stock]) {
                    StockBalances[stock]*=StockGrowthNumerator;
                    StockBalances[stock]/=factor;
                    if (stock==StockCompany_BakPhar && StockBalances[stock]>BakPharSplitThresholdCBills)
                        StockBalances[stock]>>=BakPharSplitDivideByFourShift;
                } else {
                    StockBalances[stock]*=factor;
                    StockBalances[stock]/=StockDeclineDenominator;
                }
            }
        }
UpdateWorldAnimations:
        idleWorldTickCountdown=ExplorationIdleTickCountdownReload;
        Update_Animated_Map_Tiles();
        uint8_t previousNpcPhase=NpcUpdatePhase;
        ++NpcUpdatePhase;
        if (previousNpcPhase==RoamingNpcUpdatePhaseTrigger) {
            Update_Roaming_Map_Npcs();
            NpcUpdatePhase=0;
        }
        if (MainCharactersAlive) {
            PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
            Copy_Data_To_GraphicsMemory();
            Draw_Infantry_And_Mechs();
            EGA_DrawBox_Wrapper();
        }
        if (!MainCharactersAlive || TransmittedCacheFound) {
            Draw_Message_Box();
            if (!MainCharactersAlive) {
                Display_Text_From_Memory(Characters[Character_Jason].name==Character_Dead
                    ? (uint8_t *)"Jason" : (uint8_t *)"Rex");
                Display_Text_From_Memory_ScreenRetrace_KeyboardInput(
                    (uint8_t *)" has died. You cannot finish the game without him.");
                (void)Keyboard_Get_ASCII_Hex_Input();
                Draw_Top_Graphic_Sidebar();
            }
            Display_Text_From_Memory((uint8_t *)"Do you want to play again?");
            if (Prompt_Yes_No(TRUE)) Load_Game_Map_Data();
            else ExitMainLoop=TRUE;
        }
    }
}
