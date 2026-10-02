# Sol: 1CD3 arena entry, normal return and escape

Scope: case22, raw action23, native1062–1399. Evidence: local clean expanded-EXE
1CD3 ASM. Prior0FDC arena helper and11B8 map-patch reviews remain dependencies.
Research pseudo-C, not executable C; no external assets staged or modified.

## Entry and shared post-mission work

1062 selects slot-zero sprite family by BYTEC79F, mech+7B (UpgradePackageBase),
not a hands/engine field.1082 assigns Jason to mech0 via characterC620 and sets
mech PilotIdC79D to0; the latter is not an Infantry.Pilot member.
Entry packed coordinates are0A2B/8057, animation cursor FAR2FE8:02A0, expressed
as `seg2FE8->MechWalkAnimationStream_02A0` per the owner's annotation style.
This eight-byte selector06 view overlaps the existing0270 stream family;
the scratchpad declaration is not additional sequential storage. The
advance-animation call returns AL, which is stored as the sprite frame; the
old C discarded it and wrote0. Apply arena patch and run mission8 as before.

10EF checks64 persistent effect slots: `(sprite &7E)==7C` and packed page8A.
This removes fire sprites7C/7D and high-bit variantsFC/FD on pageX=A/Y=8 by
zeroing sprite, packed page and X-low. Y-low remains untouched.1129 latches
D334 when main-characters-alive WORD014A is zero; it does not clear an existing
death latch when alive. Both return branches perform this shared work.

## Normal return: D32F zero

114B removes/randomizes the crowd patch, calls0FDC:1B41 party restoration,
then places the anchor at0A39/805D. The previously reviewed helper performs
rental-versus-owned record writeback and crew restoration. These operations
are bypassed on escape; this pass does not duplicate them in the parent.

## Escape: D32F nonzero

116E places X at0970 and selects Y4036 when WORD406A is nonzero, otherwise
Y403E.11AA restores companion Name BYTEs1..7 from3FE9+slot; Jason's Name is
not overwritten.11DB scans exactly four saved Name BYTEsD452..D455. The
former pointer scan ending2BB2 was false Reko reconstruction.

`KeepArenaSlotZero_2929` preserves the old2929 suffix and names the native
BP-28 decision/result. The three instruction paths are:

| Saved roster | Escape placement |
| --- | --- |
| All four saved namesFF | Keep arena record in live slot0; no backup/name restoration |
| Any saved name present and a saved companion slot1..3 isFF | First vacancy receives original slot-zero125-byte backup; restore its Name from saved0; keep arena record in slot0 |
| Any saved name present, companion slots1..3 all occupied | Restore original125-byte backup to slot0 and all four saved Name BYTEs; arena record is discarded |

In the vacancy path121C first restores saved Name BYTEs for live slots1..3,
1240 copies backup3780 into the first vacancy,126B overrides that record's
Name withD452. Setting BP-28 to1 prevents later vacancies being used.
1295 is only the no-vacancy fallback. Full-record copies are expressed as
FAR BYTE views with125-byte loops, not invalid typed-array assignments to
`Mechs[slot*125+byte]`. Saved0=FF edge cases retain their literal Name writes;
no policy for corrupt/unusual roster states is invented.

## Escape map/crew cleanup and common finish

12D7 builds descriptor cache centered at packed region89. Nine map files
are fetched from2FE8:0030+region for regions78,79,7A/88,89,8A/98,99,9A.
Each nonzero file BYTE is CBW-extended and loaded into row*3+column. Renamed
row-base/row/column locals preserve suffixes2907/2908/2909. The old `(index)[48]`
expression was the damaged base0030 access, not a file-pointer indirection.

1332 rebuilds the nine-grid map, then calls1543:0C72 with0 to save/apply
the temporary starport patch. This is distinct from normal crowd removal.
1347 clears all four saved name markers and live PilotId/RiderId pairs toFF;
live mech Name/other fields are not cleared here.1375 puts all eight characters
on foot.138C resets rental-mode WORDE48E for **both** normal and escape paths.

## Verification and next boundary

`Verify-ArenaReturnTranscriptions.ps1` uses synthetic records and all16 saved
presence masks to compare the relocation loop against an independent case
oracle, verify125-byte copies/name-only restoration and final crew isolation.
It also checks every sprite BYTE against the fire/page filter and all nine
region mappings with CBW file-byte semantics. Models are not gameplay traces
or compilation of research C. Existing roster/healing/shop regressions rerun.

Results:18826 arena assertions,18692 roster,8568 healing and113 shop assertions
pass, together with whitespace validation.

No extra room-making, stolen-mech guarantee, crew assignment, patch removal
or rental policy was added. Emulator traces should compare rental/owned arena
escape with no roster, first vacancy and full roster. Next: remaining1CD3
recruitment/name-dialogue and unmarked consistency branches.
