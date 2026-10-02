#include "game.h"

/* Original0FDC:01C0..05F6. Native BYTE operand CBW is explicit at each
 * signed use. Payload/operand addresses must belong to their original
 * loaded views; arbitrary out-of-segment scripts are not host contracts.
 * No instruction cap, unknown-opcode error, or new opcode semantics. */
void Execute_Bld_Bytecode(uint8_t *payload)
{
    uint16_t cursor=0,exitInterpreter=FALSE;
    while(exitInterpreter==FALSE) {
        uint16_t opcode=payload[cursor++];
        uint16_t targetOffset=cursor,branch=FALSE;
        switch(opcode) {
        case Bld_PlaySound: Play_Sound_If_Enabled((uint16_t)(int16_t)(int8_t)payload[cursor++]); break;
        case Bld_AddCBills:
            CBills+=(uint32_t)(int32_t)(int16_t)Read_Bld_Immediate_Word(payload+cursor);
            cursor+=2; Display_Text_CBill_Balance(); break;
        case Bld_SetMapPosition:
            CrescentHawkMapPositionX=Read_Bld_Immediate_Word(payload+cursor); cursor+=2;
            CrescentHawkMapPositionY=Read_Bld_Immediate_Word(payload+cursor); cursor+=2; break;
        case Bld_BranchIfXEquals:
            branch=CrescentHawkMapPositionX==Read_Bld_Immediate_Word(payload+cursor);
            cursor+=2; targetOffset=cursor; goto ResolveBranch;
        case Bld_BranchIfRandomMask: {
            uint16_t random=Rand_0x00_to_0xFF();
            int16_t mask=(int8_t)payload[cursor++];
            branch=(random&(uint16_t)mask)!=0; targetOffset=cursor; goto ResolveBranch;
        }
        case Bld_Recruit: Recruit_Crescent_Hawk_Agent((uint16_t)(int16_t)(int8_t)payload[cursor++]); break;
        case Bld_ConditionalScene: {
            uint16_t scene=(uint16_t)(int16_t)(int8_t)payload[cursor++];
            if(DisableInput==FALSE) Display_Animation_Scene(scene,(uint16_t)(int16_t)(int8_t)payload[cursor]);
            ++cursor; break;
        }
        case Bld_BranchIfSurgeryKit: branch=PurchasedFieldSurgeryKit!=0; goto ResolveBranch;
        case Bld_BranchIfMedkit: branch=PurchasedMedkit!=0; goto ResolveBranch;
        case Bld_BranchIfPartySkill: {
            int16_t skill=(int8_t)payload[cursor++],minimum=(int8_t)payload[cursor++];
            for(uint16_t member=0;member<PartySize;++member)
                if(Characters[member].name!=Character_Dead &&
                    (int8_t)((uint8_t *)Characters)[member*sizeof(Character)+offsetof(Character,skillBowsAndBlade)+skill]>=minimum)
                    branch=TRUE;
            targetOffset=cursor; goto ResolveBranch;
        }
        case Bld_SubtractCBills: {
            uint32_t amount=(uint32_t)(int32_t)(int16_t)Read_Bld_Immediate_Word(payload+cursor);
            cursor+=2; CBills=CBills>=amount?CBills-amount:0; Display_Text_CBill_Balance(); break;
        }
        case Bld_BranchIfCBills: {
            uint32_t amount=(uint32_t)(int32_t)(int16_t)Read_Bld_Immediate_Word(payload+cursor);
            cursor+=2; branch=CBills>=amount; targetOffset=cursor; goto ResolveBranch;
        }
        case Bld_SetTextLayout:
            TextColumn=(uint16_t)(int16_t)(int8_t)payload[cursor++];
            TextRow=(uint16_t)(int16_t)(int8_t)payload[cursor++]; break;
        case Bld_AddStateByte: {
            int16_t index=(int8_t)payload[cursor++];
            uint8_t *state=WorldMapState.bytes+offsetof(WorldMapStateStorage,fields.persistent)+index;
            *state=(uint8_t)(*state+payload[cursor++]); break;
        }
        case Bld_TimedWait: Wait_For_50Hz_Then_Check_Input(); break;
        case Bld_BranchStateTable: {
            int16_t index=(int8_t)payload[cursor++];
            int16_t value=(int8_t)WorldMapState.bytes[offsetof(WorldMapStateStorage,fields.persistent)+index];
            targetOffset=(uint16_t)(cursor+value*2); branch=TRUE; goto ResolveBranch;
        }
        case Bld_SetStateByte: {
            int16_t index=(int8_t)payload[cursor++];
            WorldMapState.bytes[offsetof(WorldMapStateStorage,fields.persistent)+index]=payload[cursor++]; break;
        }
        case Bld_CallAction: Citadel_Building_Dialogs((uint16_t)(int16_t)(int8_t)payload[cursor++]); break;
        case Bld_YesNoBranch: branch=Prompt_Yes_No(TRUE)!=0; goto ResolveBranch;
        case Bld_BranchIfStateNonzero: {
            int16_t index=(int8_t)payload[cursor++];
            branch=WorldMapState.bytes[offsetof(WorldMapStateStorage,fields.persistent)+index]!=0;
            targetOffset=cursor; goto ResolveBranch;
        }
        case Bld_Branch: branch=TRUE; goto ResolveBranch;
        case Bld_MenuBranchTable: {
            uint16_t menu=(uint16_t)(int16_t)(int8_t)payload[cursor++];
            uint16_t selection=Display_Menu_Choices_And_Check(menu);
            targetOffset=(uint16_t)(cursor+(uint16_t)(selection*2)); branch=TRUE; goto ResolveBranch;
        }
        case Bld_DrawBorder: Draw_Menu_Border((uint16_t)(int16_t)(int8_t)payload[cursor++]); break;
        case Bld_WaitForKey: (void)Keyboard_Get_ASCII_Hex_Input(); break;
        case Bld_DisplayText:
            Display_Text_From_Memory(payload+cursor);
            cursor=(uint16_t)(cursor+Loop_Until_TextPtr_Null(payload+cursor)+1); break;
        case Bld_RedrawSidebar: Draw_Top_Graphic_Sidebar(); break;
        case Bld_ApplyLayout: Menu_Memory_Variables((uint16_t)(int16_t)(int8_t)payload[cursor++]); break;
        case Bld_Exit: exitInterpreter=TRUE; break;
        default: break; /* All bytes below E4 are native one-byte no-ops. */
        }
        continue;
ResolveBranch:
        if(branch!=FALSE) cursor=Read_Bld_Target(payload+targetOffset);
        else cursor+=2;
    }
}
