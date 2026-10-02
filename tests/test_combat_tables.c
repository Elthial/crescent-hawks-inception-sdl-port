#include "game.h"
#include <stdio.h>
#include <stdlib.h>

static void verify(int passed,unsigned line)
{
    if(!passed) { fprintf(stderr,"Original combat table mismatch line%u\n",line); exit(1); }
}
#define check(condition) verify(!!(condition),__LINE__)
static uint32_t checksum(const uint8_t *bytes,size_t count)
{
    uint32_t result=UINT32_C(2166136261);
    for(size_t i=0;i<count;++i) result=(result^bytes[i])*UINT32_C(16777619);
    return result;
}
static uint32_t cursorChecksum(const AnimationCursor *streams,size_t count)
{
    uint32_t result=UINT32_C(2166136261);
    for(size_t i=0;i<count;++i) {
        check(streams[i].data==AttackAnimationBytecode);
        check(streams[i].dataOffset==CombatAttackBytecodeNativeOffset);
        check(streams[i].offset>=CombatAttackBytecodeNativeOffset);
        check(streams[i].offset<CombatAttackBytecodeNativeOffset+CombatAttackBytecodeBytes);
        /* Original stored FAR selector36DB, before relocation by0800. */
        uint8_t nativePointer[4]={(uint8_t)streams[i].offset,
            (uint8_t)(streams[i].offset>>8),0xDB,0x36};
        for(unsigned byte=0;byte<4;++byte)
            result=(result^nativePointer[byte])*UINT32_C(16777619);
    }
    return result;
}
int main(void)
{
    /* Sol: fingerprints from the private expanded EXE's complete data windows,
     * not hashes of external level/art assets. This verifies table transcription
     * and native indexing ranges, NOT the still-pending combat parent. */
    check(checksum(CombatHitCategoryByFacingDifference,sizeof CombatHitCategoryByFacingDifference)==UINT32_C(0xDC9F2695));
    check(checksum(CombatTargetMovementPenalty,sizeof CombatTargetMovementPenalty)==UINT32_C(0x23D6E6C0));
    check(checksum(CombatMechHitLocationOffsets,sizeof CombatMechHitLocationOffsets)==UINT32_C(0x85A7E709));
    check(checksum(CombatMissileClusterTable,sizeof CombatMissileClusterTable)==UINT32_C(0x77EC619C));
    for(int facing=0;facing<CompassDirectionCount;++facing)
        for(int heading=0;heading<CompassDirectionCount;++heading) {
            unsigned category=CombatHitCategoryByFacingDifference[facing-heading+CombatHitFacingBias];
            check(category<CombatHitLocationCategories);
            for(unsigned roll=2;roll<=12;++roll) {
                unsigned offset=CombatMechHitLocationOffsets[category*CombatHitLocationRollOutcomes+roll-2];
                check(offset>=0x11 && offset<=0x1B); /* original eleven armour BYTEs */
            }
        }
    check(CombatMechHitLocationOffsets[1]==0x13); /* kick random bit3 clear: left leg */
    check(CombatMechHitLocationOffsets[9]==0x18); /* kick random bit3 set: right leg */
    for(unsigned roll=2;roll<=12;++roll) for(unsigned column=2;column<=8;++column) {
        unsigned index=roll*CombatMissileClusterColumns+column-CombatMissileClusterIndexBias;
        check(index<CombatMissileClusterEntries);
        check(CombatMissileClusterTable[index]>=1 && CombatMissileClusterTable[index]<=20);
    }
    check(CombatTargetMovementPenalty[0]==0);
    check(CombatTargetMovementPenalty[CombatMovementSlices]==4);
    check(checksum(AttackAnimationBytecode,sizeof AttackAnimationBytecode)==UINT32_C(0x133696DF));
    check(cursorChecksum(LocustFireAnimationStreams,sizeof LocustFireAnimationStreams/sizeof LocustFireAnimationStreams[0])==UINT32_C(0xE35558E5));
    check(cursorChecksum(CommandoFireAnimationStreams,sizeof CommandoFireAnimationStreams/sizeof CommandoFireAnimationStreams[0])==UINT32_C(0x6063CAA1));
    check(cursorChecksum(LocustKickAnimationStreams,sizeof LocustKickAnimationStreams/sizeof LocustKickAnimationStreams[0])==UINT32_C(0x8CC7EAD5));
    check(cursorChecksum(CommandoKickAnimationStreams,sizeof CommandoKickAnimationStreams/sizeof CommandoKickAnimationStreams[0])==UINT32_C(0xD9FD2E35));
    check(cursorChecksum(MissileAnimationStreams,sizeof MissileAnimationStreams/sizeof MissileAnimationStreams[0])==UINT32_C(0x8506FAE5));
    check(cursorChecksum(PersonnelAttackAnimationStreams,sizeof PersonnelAttackAnimationStreams/sizeof PersonnelAttackAnimationStreams[0])==UINT32_C(0xA6C96EAD));
    check(TargetImpactBytecode[0]==32 && TargetImpactBytecode[4]==AnimationToken_RelativeLoop && TargetImpactBytecode[5]==5);
    check(ProjectileImpactBytecode[0]==124 && ProjectileImpactBytecode[1]==125 && ProjectileImpactBytecode[2]==124 && ProjectileImpactBytecode[3]==AnimationToken_Wait);
    check(LocustMuzzleY[0]==-15 && CommandoMuzzleX[5]==-9);
    check(PersonnelMuzzleX[0]==5 && PersonnelMuzzleY[4]==4);
    check(ProjectilePrimaryStepY[0]==-1 && ProjectileSecondaryStepX[4]==-1);
    puts("Original combat BYTE tables: full fingerprints and native facing, location, kick, cluster and movement indices passed.");
    return 0;
}
