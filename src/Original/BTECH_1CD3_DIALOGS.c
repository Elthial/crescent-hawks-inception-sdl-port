#include "game.h"
#include "dos.h"

/* Original1CD3:0004..17C5. Sol: Dispatch all47 building-script actions.
 * Training, finance, equipment, services and story changes remain in their
 * native parent. Raw F5 actions are one-based; action0 wraps and is rejected.
 * BYTE initial accesses, fire-cleanup gate, right alignment, exact EXE text,
 * WORD arena-position loads and final Health store incorporate ASM corrections.
 * Native-valid selectors/table indices and DF-clear storage are contracts;
 * corrupt selectors do not acquire new bounds checks or gameplay validation. */
void Citadel_Building_Dialogs(uint16_t action)
{
    uint16_t actionIndex=(uint16_t)(action-1);
    uint8_t *dialogue;
    uint16_t resetAssignments;
    if(actionIndex>CitadelAction_DeductJasonHealth-1) return;
    switch(action) {
    case CitadelAction_StartTraining: {
        Mech *trainingMech=&MechRefs[MECH_REF_Locust];
        CombatantSpriteFamilyOffset[Character_Jason]=MECH_Sprite_LOCUST;
        if(SelectedTrainingMech!=0) {
            CombatantSpriteFamilyOffset[Character_Jason]=MECH_Sprite_COMMANDO;
            trainingMech=&MechRefs[MechRef_Wasp];
            if(SelectedTrainingMech==2) trainingMech=&MechRefs[MechRef_Chameleon]; /*2FE8:04E4 Chameleon, record4.*/
        }
        Characters[Character_Jason].mechAssignment=0;
        for(uint16_t byte=0;byte<MechRecordSize;++byte) ((uint8_t *)&Mechs[0])[byte]=((uint8_t *)trainingMech)[byte];
        Mechs[0].pilotId=Character_Jason;
        if(DisableInput==FALSE) Display_Animation_Scene(AnimationO00_MechStartUp,AnimationPlayback_RestoreGameView);
        CrescentHawkMapPositionY=TrainingMarchY;
        CombatantAnimationCursors[Character_Jason]=(AnimationCursor){WalkAnimationStreams,WalkAnimationStreamNativeOffset,WalkAnimationStreamNativeOffset+2*WalkAnimationStreamBytes};
        CrescentHawkMapPositionX=TrainingMarchStartX;
        while(CrescentHawkMapPositionX<TrainingMarchEndX) {
            CombatantSpriteFrame[Character_Jason]=Advance_Combatant_Animation_Stream(Character_Jason);
            for(uint16_t redraw=0;redraw<TrainingMarchRedraws;++redraw) {
                Update_Animated_Map_Tiles(); PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
                Copy_Data_To_GraphicsMemory(); Draw_Infantry_And_Mechs(); EGA_DrawBox_Wrapper();
                if((int16_t)CpuTimingCalibration>TrainingMarchCalibrationThreshold) Wait_For_N_Vertical_Retraces(TrainingMarchRetraces);
            }
            ++CrescentHawkMapPositionX;
        }
        CombatantSpriteFrame[Character_Jason]=Advance_Combatant_Animation_Stream(Character_Jason);
        Mech_Mission((uint16_t)(int16_t)(int8_t)PersistentState.bytes[0]);
        break;
    }
    case CitadelAction_TrainingDebrief:
        if(Mechs[0].name[0]=='L') dialogue=(uint8_t *)"Locust, the results are hardly surprising. "; /*48B3*/
        else {
            Display_Text_From_Memory((uint8_t *)(Mechs[0].name[0]=='W'?"Wasp":"Chameleon")); /*48DF/48E4*/
            dialogue=(uint8_t *)", the results are all the more impressive. "; /*48EE*/
        }
        goto DisplayDialogue;
    case CitadelAction_LeaveTraining:
        Mechs[0].name[0]=MECH_Destroyed; Characters[Character_Jason].mechAssignment=Character_OnFoot;
        CrescentHawkMapPositionX=TrainingReturnX; CrescentHawkMapPositionY=TrainingReturnY;
        if(KuritaDestroyedCitadel==FALSE) for(uint16_t effect=0;effect<PersistentMapEffectSlotCount;++effect) {
            if((MapEffectSpriteIndex[effect]&StoryFireFamilyMask)==StoryFireFirstFrame) {
                MapEffectPositionXLow[effect]=0; MapEffectPackedPage[effect]=0; MapEffectSpriteIndex[effect]=0;
            }
        }
        break;
    case CitadelAction_ShowTranscript:
        for(uint16_t skill=0;skill<SchoolWeaponSkillCount;++skill) {
            int16_t score=(int16_t)(SchoolScorePerSkillLevel*(int8_t)((uint8_t *)&Characters[0])[offsetof(Character,skillBowsAndBlade)+skill]+SchoolScoreBase);
            ASM_Text_Formatting((uint16_t)score,DynamicString,NumericRadix_Decimal);
            Draw_EGA_Text_To_Screen(DynamicString,(uint16_t)(AccountRightColumn-Loop_Until_TextPtr_Null(DynamicString)),
                (uint16_t)(skill+5),EGA_BrightGreen,0); /*0231: transcript rows5..7.*/
        }
        break;
    case CitadelAction_BuySchoolTraining: {
        Display_Text_From_Memory((uint8_t *)"By looking at your transcript, I see you are "); /*491A*/
        int16_t level=(int8_t)((uint8_t *)&Characters[0])[offsetof(Character,skillBowsAndBlade)+(int8_t)SelectedSchoolSkill];
        Display_Text_From_Memory(SchoolSkillDescriptions[level]);
        Display_Text_From_Memory(SchoolWeaponSkillText[(int8_t)SelectedSchoolSkill]);
        if(level==SkillLevel_Excellent) Display_Text_From_Memory((uint8_t *)"I'm afraid that we here can't possibly teach you any more than you already know."); /*4948*/
        else {
            uint8_t *skill=(uint8_t *)&Characters[0]+offsetof(Character,skillBowsAndBlade)+(int8_t)SelectedSchoolSkill;
            int16_t tuition=(int16_t)(SchoolScorePerSkillLevel*(int8_t)*skill+SchoolScoreBase);
            uint32_t cost=(uint32_t)(int32_t)tuition; /*Signed WORD IMUL/CWD, then unsigned DWORD affordability.*/
            if(cost<=CBills) {
                SchoolTrainingPurchased=TRUE;
                ++((uint8_t *)&Characters[0])[offsetof(Character,skillBowsAndBlade)+(int8_t)SelectedSchoolSkill];
                CBills-=cost; goto DisplayBalance;
            }
            Display_Text_From_Memory((uint8_t *)"\r\rI'm afraid you don't have enough C-bills for tuition.  "); /*4999*/
            Display_Text_Shop_Cannot_Afford_Text();
        }
        SchoolTrainingPurchased=FALSE; break;
    }
    case CitadelAction_ShowAccounts: {
        uint16_t colour=(uint16_t)(GraphicsAdapter==0?1:2);
        CBill_Text_Formatting(CBills,DynamicString,NumericRadix_Decimal);
        Draw_EGA_Text_To_Screen(DynamicString,(uint16_t)(AccountRightColumn-Loop_Until_TextPtr_Null(DynamicString)),1,colour,0);
        StockHoldingCount=0;
        for(uint16_t stock=0;stock<StockCompanyCount;++stock) if(StockBalances[stock]!=0) {
            TextColour=EGA_BrightWhite; Display_Text_From_Memory(StockMarketNames[stock]);
            CBill_Text_Formatting(StockBalances[stock],DynamicString,NumericRadix_Decimal);
            uint16_t row=TextRow;
            Draw_EGA_Text_To_Screen(DynamicString,(uint16_t)(AccountRightColumn-Loop_Until_TextPtr_Null(DynamicString)),row,colour,0);
            ++StockHoldingCount;
        }
        if(StockHoldingCount==0) { dialogue=(uint8_t *)"\rNone"; goto DisplayDialogue; } /*49D3*/
        break;
    }
    case CitadelAction_SellStock: {
        if(StockHoldingCount==0) { dialogue=(uint8_t *)"\r\rYou have no stock to sell."; goto DisplayDialogue; } /*49D9*/
        Draw_Top_Graphic_Sidebar(); Display_Text_From_Memory((uint8_t *)"Sell which stock?\r"); /*49F6*/
        for(uint16_t stock=0;stock<StockCompanyCount;++stock) Display_Text_From_Memory(StockMarketNames[stock]);
        uint16_t stock=Display_Menu_Choices_And_Check(StockSelectionMenu);
        if(StockBalances[stock]==0) { dialogue=(uint8_t *)"\r\rYou can't sell any stock you don't own!"; goto DisplayDialogue; } /*4ACB*/
        Display_Text_From_Memory((uint8_t *)"\r\rSell how much?  "); /*4A09*/
        Draw_EGA_Text_To_Screen((uint8_t *)"Stock account:",TextPanelLeft,AccountBalanceRow,EGA_BrightWhite,0); /*4A1C*/
        uint16_t colour=(uint16_t)(GraphicsAdapter==0?1:2);
        CBill_Text_Formatting(StockBalances[stock],DynamicString,NumericRadix_Decimal);
        Draw_EGA_Text_To_Screen(DynamicString,(uint16_t)(AccountRightColumn-Loop_Until_TextPtr_Null(DynamicString)),AccountBalanceRow,colour,0);
        uint32_t amount=Prompt_For_Unsigned_Decimal(),available=StockBalances[stock];
        if(amount>available) { amount=available; Display_Text_From_Memory((uint8_t *)"\r\rYou don't have that much, but you sell all that you do have."); } /*4A2B*/
        if(amount==0) { Display_Text_From_Memory((uint8_t *)"\r\rYou sell"); Display_No_Stock_Transaction_Text(); break; } /*4AC0*/
        Display_Text_From_Memory((uint8_t *)"\r\rYou have sold a total of "); /*4A6A*/
        CBill_Text_Formatting(amount,DynamicString,NumericRadix_Decimal); TextColour=(uint16_t)(GraphicsAdapter==0?1:2);
        Display_Text_From_Memory(DynamicString); Display_Text_From_Memory((uint8_t *)"\006\017 of "); /*4A86*/
        Display_Text_From_Memory(StockMarketNames[stock]); Display_Text_From_Memory((uint8_t *)"stock, bringing your balance in that stock to "); /*4A8D*/
        StockBalances[stock]-=amount; CBills+=amount;
        CBill_Text_Formatting(StockBalances[stock],DynamicString,NumericRadix_Decimal); TextColour=colour;
        Display_Text_From_Memory(DynamicString); dialogue=(uint8_t *)"\006\017."; goto DisplayTransactionEnd; /*4ABC*/
    }
    case CitadelAction_InvestStock: {
        if(CBills==0) { dialogue=(uint8_t *)"\r\rYou need some cash in your C-bill account before you can buy stock."; goto DisplayDialogue; } /*4AF5*/
        Draw_Top_Graphic_Sidebar(); Display_Text_From_Memory((uint8_t *)"Invest in which stock?"); /*4B3B*/
        for(uint16_t stock=0;stock<StockCompanyCount;++stock) Display_Text_From_Memory(StockMarketNames[stock]);
        uint16_t stock=Display_Menu_Choices_And_Check(StockSelectionMenu);
        Display_Text_From_Memory((uint8_t *)"\r\rInvest how much?  "); /*4B52*/
        Draw_EGA_Text_To_Screen((uint8_t *)"C-bill account:",TextPanelLeft,AccountBalanceRow,EGA_BrightWhite,0); /*4B67*/
        uint16_t colour=(uint16_t)(GraphicsAdapter==0?1:2);
        CBill_Text_Formatting(CBills,DynamicString,NumericRadix_Decimal);
        Draw_EGA_Text_To_Screen(DynamicString,(uint16_t)(AccountRightColumn-Loop_Until_TextPtr_Null(DynamicString)),AccountBalanceRow,colour,0);
        uint32_t amount=Prompt_For_Unsigned_Decimal();
        if(amount>CBills) { amount=CBills; Display_Text_From_Memory((uint8_t *)"\r\rYou don't have that much, but you invest all that you do have."); } /*4B77*/
        if(amount==0) { Display_Text_From_Memory((uint8_t *)"\r\rYou invest"); Display_No_Stock_Transaction_Text(); break; } /*4C0B*/
        Display_Text_From_Memory((uint8_t *)"\r\rYou have invested a total of "); /*4BB8*/
        CBill_Text_Formatting(amount,DynamicString,NumericRadix_Decimal); TextColour=(uint16_t)(GraphicsAdapter==0?1:2);
        Display_Text_From_Memory(DynamicString); Display_Text_From_Memory((uint8_t *)"\006\017 in "); /*4BD8*/
        Display_Text_From_Memory(StockMarketNames[stock]); Display_Text_From_Memory((uint8_t *)"bringing your balance in that stock to "); /*4BDF*/
        StockBalances[stock]+=amount; CBills-=amount;
        CBill_Text_Formatting(StockBalances[stock],DynamicString,NumericRadix_Decimal); TextColour=colour;
        Display_Text_From_Memory(DynamicString); dialogue=(uint8_t *)"\006\017.";
DisplayTransactionEnd:
        Display_Text_From_Memory(dialogue);
        goto DisplayBalance;
    }
    case CitadelAction_BuyArmour: {
        uint16_t type=MenuControls[14].selection;
        if(BldInteractionFlag!=FALSE) type=MenuControls[13].selection;
        ShopPaymentSuccessful=FALSE; uint32_t cost=ArmourPurchaseCost[type-1];
        if(CBills>=cost) {
            CBills-=cost; ShopPaymentSuccessful=TRUE; Display_Text_CBill_Balance();
            Distribute_Purchased_Armour(type,(uint16_t)(int16_t)(int8_t)ArmourTypeDurability[type]);
        }
        break;
    }
    case CitadelAction_ShowCash:
        CBill_Text_Formatting(CBills,DynamicString,NumericRadix_Decimal); TextColour=(uint16_t)(GraphicsAdapter==0?1:2);
        Display_Text_From_Memory(DynamicString); dialogue=(uint8_t *)"\006\017."; goto DisplayDialogue; /*4C18*/
    case CitadelAction_BuyWeapon: {
        uint16_t weapon=MenuControls[16].selection;
        if(WeaponShopCategoryOffset==WeaponShopFirearmOffset) weapon=(uint16_t)(MenuControls[17].selection+WeaponShopFirearmOffset);
        if(WeaponShopCategoryOffset==WeaponShopHeavyOffset) weapon=(uint16_t)(MenuControls[18].selection+WeaponShopHeavyOffset);
        --weapon; ShopPaymentSuccessful=FALSE; uint32_t cost=InfantryWeaponPurchaseCost[weapon];
        if(CBills>=cost) { CBills-=cost; ShopPaymentSuccessful=TRUE; Display_Text_CBill_Balance(); Distribute_Weapon_To_Party((uint16_t)(weapon+1)); }
        break;
    }
    case CitadelAction_TalkToOccupants: Talk_To_Building_Occupants(); break;
    case CitadelAction_SelectPartyMember: {
        Citadel_Building_Dialogs(CitadelAction_CountCompanions);
        uint8_t slots[PartySize]={0}; uint16_t living=0;
        MenuControls[23].baseRow=TextRow; MenuControls[23].optionCount=(uint16_t)((int8_t)SelectedPartyMemberSlot+1); MenuControls[23].selection=0;
        for(uint16_t member=0;member<PartySize;++member) if(Characters[member].name!=Character_Dead) {
            Display_Text_From_Memory(CharacterNames[(int8_t)Characters[member].name]); Display_Text_4FA0_Value(); slots[living++]=(uint8_t)member;
        }
        SelectedPartyMemberSlot=slots[Display_Menu_Choices_And_Check(PartyMechSelectionMenu)]; break;
    }
    case CitadelAction_CountCompanions:
        SelectedPartyMemberSlot=0;
        for(uint16_t member=1;member<PartySize;++member) if(Characters[member].name!=Character_Dead) ++SelectedPartyMemberSlot;
        break;
    case CitadelAction_BuyTechTraining: {
        if(CBills<SpecialistTrainingCost) { SelectedPartyMemberSlot=FALSE; break; }
        CBills-=SpecialistTrainingCost; ++Characters[(int8_t)SelectedPartyMemberSlot].skillTech;
        Characters[(int8_t)SelectedPartyMemberSlot].trainingFlags|=CharacterTraining_Tech;
        SelectedPartyMemberSlot=TRUE; goto DisplayBalance;
    }
    case CitadelAction_QueryTechTraining:
        SelectedSpecialistTrainingMask=Characters[(int8_t)SelectedPartyMemberSlot].trainingFlags&CharacterTraining_Tech; break;
    case CitadelAction_CountVisibleMechs:
        NumberOfActiveLanceMechs=0;
        for(uint16_t mech=0;mech<LanceSize;++mech) if(Mechs[mech].name[0]!=MECH_Destroyed) ++NumberOfActiveLanceMechs;
        break;
    case CitadelAction_RepairMech: Mechlube_Repair_Mech(); break;
    case CitadelAction_ModifyMech: Mechlube_Modify_Mech(); break;
    case CitadelAction_UpgradeMech: Mechlube_Upgrade_Mech(); break;
    case CitadelAction_BuyMedicalTraining:
        if(CBills<SpecialistTrainingCost) { SelectedPartyMemberSlot=FALSE; break; }
        CBills-=SpecialistTrainingCost; ++Characters[(int8_t)SelectedPartyMemberSlot].skillMedical;
        Characters[(int8_t)SelectedPartyMemberSlot].trainingFlags|=CharacterTraining_Medical;
        SelectedPartyMemberSlot=TRUE; goto DisplayBalance;
    case CitadelAction_QueryMedicalTraining:
        SelectedSpecialistTrainingMask=Characters[(int8_t)SelectedPartyMemberSlot].trainingFlags&CharacterTraining_Medical; break;
    case CitadelAction_QueryInjuries:
        PartyHasInjuredMember=FALSE;
        for(uint16_t member=0;member<PartySize;++member) if(Characters[member].name!=Character_Dead &&
            (int8_t)Characters[member].body*CharacterHealthPerBodyPoint!=(int8_t)Characters[member].health) PartyHasInjuredMember=TRUE;
        break;
    case CitadelAction_TreatParty: {
        if(SelectedMedicalServiceTier==MedicalService_UsePartyMedicAndEquipment) { Heal_Characters(MedicalService_UsePartyMedicAndEquipment); break; }
        int16_t tier=(int8_t)SelectedMedicalServiceTier;
        uint32_t cost=(uint32_t)(int32_t)MedicalServiceFee[tier];
        if(CBills>=cost) { CBills-=cost; Heal_Characters((uint16_t)tier); }
        else {
            Display_Text_4FA0_Value(); Display_Text_Shop_Cannot_Afford_Text();
            Wait_For_50Hz_Then_Check_Input(); Keyboard_Get_ASCII_Hex_Input(); CBills+=HospitalFacilitiesRefund;
        }
DisplayBalance:
        Display_Text_CBill_Balance(); break;
    }
    case CitadelAction_GrantMedkit: PurchasedMedkit=TRUE; break;
    case CitadelAction_GrantSurgeryKit: PurchasedFieldSurgeryKit=TRUE; break;
    case CitadelAction_HidePartyMechs:
        for(uint16_t mech=0;mech<LanceSize;++mech) {
            StoredPartyMechNameInitial[mech]=Mechs[mech].name[0]; Mechs[mech].name[0]=MECH_Destroyed;
            Characters[mech+LanceSize].mechAssignment=Character_OnFoot; Characters[mech].mechAssignment=Character_OnFoot;
        }
        break;
    case CitadelAction_CountStoredMechs:
        NumberOfActiveLanceMechs=0;
        for(uint16_t mech=0;mech<LanceSize;++mech) if((int8_t)StoredPartyMechNameInitial[mech]>='A' && (int8_t)StoredPartyMechNameInitial[mech]<='Z') ++NumberOfActiveLanceMechs;
        break;
    case CitadelAction_RecruitRex: Recruit_Rex_And_Start_KuritaParty_Ambush(); break;
    case CitadelAction_QueryArmourType: SelectedPartyArmourType=Characters[(int8_t)SelectedPartyMemberSlot].armourType; break;
    case CitadelAction_RepairArmour: {
        int16_t member=(int8_t)SelectedPartyMemberSlot,type=(int16_t)((int8_t)Characters[member].armourType-1);
        int16_t remaining=(int8_t)Characters[member].armourValue,full=(int8_t)ArmourTypeDurability[type+1];
        if(full==remaining) { dialogue=(uint8_t *)"Why, this armor looks brand new to me!  It doesn't need any repair."; goto DisplayRepairResult; } /*4C1C*/
        Display_Text_From_Memory((uint8_t *)"This armor has lost "); /*4C60*/
        int16_t deficit=(int16_t)(full-remaining);
        if(remaining==0) Display_Text_From_Memory((uint8_t *)"all"); /*4C75*/
        else { Display_Text_Dynamic_Value((uint16_t)deficit); Display_Text_From_Memory((uint8_t *)" points"); } /*4C79*/
        Display_Text_From_Memory((uint8_t *)" of its protective capacity.  It would cost "); /*4C81*/
        Display_Text_Dynamic_Value((uint16_t)((int8_t)ArmourRepairCostPerPoint[type]*deficit));
        Display_Text_From_Memory((uint8_t *)" C-bills to repair it, and you have "); /*4CAE*/
        Citadel_Building_Dialogs(CitadelAction_ShowCash); Display_Text_From_Memory((uint8_t *)"  Do you want it repaired?"); /*4CD3*/
        if(Prompt_Yes_No(TRUE)==FALSE) break;
        uint16_t anyRepaired=FALSE;
        while(deficit!=0) {
            uint32_t cost=(uint32_t)(int32_t)(int8_t)ArmourRepairCostPerPoint[type];
            if(cost>CBills) break;
            anyRepaired=TRUE; deficit=(int16_t)(uint16_t)(deficit-1);
            CBills-=(uint32_t)(int32_t)(int8_t)ArmourRepairCostPerPoint[type];
            ++Characters[member].armourValue; Display_Text_CBill_Balance();
        }
        if(deficit!=0) {
            Display_Text_From_Memory((uint8_t *)"\r\rYou did not have enough C-bills to "); /*4CEE*/
            dialogue=(uint8_t *)(anyRepaired?"finish the job, but we repaired all that you could afford.":"get your armor patched."); /*4D14/4D4F*/
        } else dialogue=(uint8_t *)"\r\rThere you are, sir, good as new!"; /*4D67*/
DisplayRepairResult:
        Display_Text_From_Memory(dialogue); Keyboard_Get_ASCII_Hex_Input(); break;
    }
    case CitadelAction_RentArenaLocust: Prepare_Rental_Locust_For_Arena(); break;
    case CitadelAction_StageOwnedArenaMech: Prepare_Party_Mech_For_Arena(); break;
    case CitadelAction_RunArena: {
        CombatantSpriteFamilyOffset[Character_Jason]=(uint8_t)(Mechs[0].upgradePackageBase==0?MECH_Sprite_LOCUST:MECH_Sprite_COMMANDO);
        Characters[Character_Jason].mechAssignment=0; Mechs[0].pilotId=Character_Jason;
        Display_Animation_Scene(AnimationO00_MechStartUp,AnimationPlayback_RestoreGameView);
        CrescentHawkMapPositionX=ArenaEntryX; CrescentHawkMapPositionY=ArenaEntryY;
        CombatantAnimationCursors[Character_Jason]=(AnimationCursor){WalkAnimationStreams,WalkAnimationStreamNativeOffset,WalkAnimationStreamNativeOffset+6*WalkAnimationStreamBytes};
        CombatantSpriteFrame[Character_Jason]=Advance_Combatant_Animation_Stream(Character_Jason);
        Arena_Select_And_Apply_Combat_Map_Patch(); Mech_Mission(Mission_Arena);
        for(uint16_t effect=0;effect<PersistentMapEffectSlotCount;++effect) if((MapEffectSpriteIndex[effect]&StoryFireFamilyMask)==StoryFireFirstFrame && MapEffectPackedPage[effect]==ArenaEffectPage) {
            MapEffectPositionXLow[effect]=0; MapEffectPackedPage[effect]=0; MapEffectSpriteIndex[effect]=0;
        }
        if(MainCharactersAlive==FALSE) PersistentState.fields.requiredMainCharacterDead=TRUE;
        if(ArenaEscapeAllowed==FALSE) {
            Arena_Remove_And_Randomize_Combat_Map_Patch(); Restore_Party_After_Arena_Combat();
            CrescentHawkMapPositionX=ArenaReturnX; CrescentHawkMapPositionY=ArenaReturnY;
        } else {
            CrescentHawkMapPositionX=ArenaEscapeX; CrescentHawkMapPositionY=CombatantPackedY[0];
            if(CombatantActive[0]==FALSE) CrescentHawkMapPositionY=CombatantPackedY[LanceSize];
            for(uint16_t member=1;member<PartySize;++member) Characters[member].name=SavedPartyNameId[member];
            uint16_t keepArena=TRUE;
            for(uint16_t mech=0;mech<LanceSize;++mech) if(StoredPartyMechNameInitial[mech]!=MECH_Destroyed) keepArena=FALSE;
            for(uint16_t mech=1;mech<LanceSize;++mech) if(keepArena==FALSE && StoredPartyMechNameInitial[mech]==MECH_Destroyed) {
                keepArena=TRUE;
                for(uint16_t restore=1;restore<LanceSize;++restore) Mechs[restore].name[0]=StoredPartyMechNameInitial[restore];
                for(uint16_t byte=0;byte<MechRecordSize;++byte) ((uint8_t *)&Mechs[mech])[byte]=ArenaMechRecordBackup[byte];
                Mechs[mech].name[0]=StoredPartyMechNameInitial[0];
            }
            if(keepArena==FALSE) {
                for(uint16_t byte=0;byte<MechRecordSize;++byte) ((uint8_t *)&Mechs[0])[byte]=ArenaMechRecordBackup[byte];
                for(uint16_t mech=0;mech<LanceSize;++mech) Mechs[mech].name[0]=StoredPartyMechNameInitial[mech];
            }
            Map_Construct_Nine_Regions(ArenaEscapeRegion);
            uint16_t region=ArenaEscapeFirstRegion;
            for(uint16_t row=0;row<MapNeighbourhoodWidth;++row) {
                for(uint16_t column=0;column<MapNeighbourhoodWidth;++column) {
                    uint8_t file=MapFileByWorldRegion[region+column];
                    if(file!=0) DOS_Load_Map_Files((uint16_t)(row*MapNeighbourhoodWidth+column),(uint16_t)(int16_t)(int8_t)file);
                }
                region+=MapRegionRowStride;
            }
            Map_NineGrid_Parent(); Starport_MapPatch_SaveApply_Or_Restore(FALSE);
            for(uint16_t mech=0;mech<LanceSize;++mech) {
                StoredPartyMechNameInitial[mech]=MECH_Destroyed; Mechs[mech].riderId=MECH_NoRider; Mechs[mech].pilotId=MECH_NoPilot;
            }
            for(uint16_t member=0;member<PartySize;++member) Characters[member].mechAssignment=Character_OnFoot;
        }
        ArenaRentalMechMode=FALSE; break;
    }
    case CitadelAction_ShowTraitorName: dialogue=CharacterNames[(int8_t)Characters[(int8_t)TraitorCharacterId].name]; goto DisplayDialogue;
    case CitadelAction_KillJason: MainCharactersAlive=FALSE; Characters[Character_Jason].name=Character_Dead; break;
    case CitadelAction_ShowRecruitName:
        dialogue=CharacterNames[(int8_t)NextRecruitNameId];
DisplayDialogue:
        Display_Text_From_Memory(dialogue); break;
    case CitadelAction_ResetCrewAssignments:
        resetAssignments=TRUE; goto OpenCrewAssignments; /*13E6:1467's argument resets ALL seats; not character slot1.*/
    case CitadelAction_Jailbreak:
        Run_Jailbreak_Mission_And_Award_Stinger();
        if(MainCharactersAlive==FALSE) PersistentState.fields.requiredMainCharacterDead=TRUE;
        break;
    case CitadelAction_GrantLasers:
        if(SelectedPartyMemberSlot!=0) {
            for(int16_t grant=0;grant<=(int8_t)SelectedPartyMemberSlot;++grant) {
                uint16_t weapon=InfantryLaserRifle;
                if(grant!=0) weapon=(uint16_t)(weapon-(Rand_0x00_to_0xFF()&1));
                Distribute_Weapon_To_Party(weapon);
            }
        } else {
            Draw_Top_Graphic_Sidebar(); Display_Text_From_Memory((uint8_t *)"Do you want to drop your "); /*4D8A*/
            Display_Text_From_Memory(WeaponStats[(int8_t)Characters[Character_Jason].weapon].name);
            Display_Text_From_Memory((uint8_t *)" and take the Laser Rifle?"); /*4DA4*/
            if(Prompt_Yes_No(TRUE)!=FALSE) Characters[Character_Jason].weapon=InfantryLaserRifle;
        }
        for(uint16_t member=0;member<PartySize;++member) if(Characters[member].name!=Character_Dead) {
            int16_t multiplier=(int16_t)(ScenarioHealthCapMinimumPerBody+(Rand_0x00_to_0xFF()&1));
            int16_t cap=(int16_t)(multiplier*(int8_t)Characters[member].body);
            if((int8_t)Characters[member].health>cap) Characters[member].health=(uint8_t)cap;
        }
        break;
    case CitadelAction_StageAllNpcs:
        for(uint16_t npc=0;npc<MapCharacterCount;++npc) {
            uint16_t actor=Enemy_Infantry_CombatantId_Range_First+npc;
            CombatantPackedX[actor]=CrescentHawkMapPositionX; RoamingMapNpcs[npc].currentPositionX=CrescentHawkMapPositionX;
            CombatantPackedY[actor]=CrescentHawkMapPositionY; RoamingMapNpcs[npc].currentPositionY=CrescentHawkMapPositionY;
            RoamingMapNpcs[npc].movementDelay=(uint8_t)npc; RoamingMapNpcs[npc].waypointPair=NpcAllStagingWaypoints;
        }
        ++CrescentHawkMapPositionY; break;
    case CitadelAction_StageFirstNpc:
        CombatantPackedX[Enemy_Infantry_CombatantId_Range_First]=MapInteractablePositionX[7]; RoamingMapNpcs[0].currentPositionX=MapInteractablePositionX[7];
        CombatantPackedY[Enemy_Infantry_CombatantId_Range_First]=MapInteractablePositionY[7]; RoamingMapNpcs[0].currentPositionY=MapInteractablePositionY[7];
        RoamingMapNpcs[0].movementDelay=NpcStagingDelay; RoamingMapNpcs[0].waypointPair=NpcFirstStagingWaypoints;
        RoamingMapNpcs[0].destinationX=MapInteractablePositionX[0]; RoamingMapNpcs[0].destinationY=MapInteractablePositionY[0]; break;
    case CitadelAction_BuyMechAmmo: Mechlube_Buy_Ammo(); break;
    case CitadelAction_ShowEnding:
        GraphicsCompatibilityFlag=FALSE; Select_Game_Disk_And_Drive(GameDisk_Second);
        Load_File_To_Memory((uint8_t *)"ENDMECH.CMP",GraphicsSceneWorkspace); /*4DBF*/
        Decompress_File_Into_Memory(GraphicsSceneWorkspace,GraphicsFileWorkspace);
        if(GraphicsAdapter==GraphicsAdapter_Ega) DrawCall_Image_To_VGA_Memory(GraphicsFileWorkspace,EgaSceneStagingSegment);
        Set_Palette_registers(EndingEgaPalette);
        Draw_GraphicsFile_In_Memory(GraphicsFileWorkspace,0,0,EgaFramebufferRowBytes,EgaScreenHeight);
        GraphicsCompatibilityFlag=TRUE;
        Load_File_To_Memory((uint8_t *)"STARLEAG.ICN",GraphicsFileWorkspace); /*4DCB*/
        Decompress_File_Into_Memory(GraphicsFileWorkspace,GraphicsSceneWorkspace);
        Wait_For_50Hz_Then_Check_Input(); Keyboard_Get_ASCII_Hex_Input(); Graphics_Set_Screen_To_Black();
        Set_Palette_registers(DefaultEgaPalette); Draw_Health_and_C_Bills_Sidebar(TRUE); Draw_Top_Graphic_Sidebar(); break;
    case CitadelAction_RestorePartyMechs:
        for(uint16_t mech=0;mech<LanceSize;++mech) Mechs[mech].name[0]=StoredPartyMechNameInitial[mech];
        /*Native raw2E falls through the common sprite/crew tail.*/
        goto RebuildMechSprites;
    case CitadelAction_RebuildMechSprites:
RebuildMechSprites:
        for(uint16_t mech=0;mech<LanceSize;++mech) CombatantSpriteFamilyOffset[mech]=(uint8_t)(Mechs[mech].name[0]=='L'?MECH_Sprite_LOCUST:MECH_Sprite_COMMANDO);
        resetAssignments=FALSE;
OpenCrewAssignments:
        Assign_Pilot_and_rider_to_Mechs(resetAssignments); break;
    case CitadelAction_DeductJasonHealth:
        if((int8_t)Characters[Character_Jason].health>ScriptHealthDeductionThreshold) Characters[Character_Jason].health-=ScriptHealthDeduction;
        break;
    }
}
