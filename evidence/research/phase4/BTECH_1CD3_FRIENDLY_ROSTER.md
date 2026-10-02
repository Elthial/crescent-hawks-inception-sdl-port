# Sol: 1CD3 friendly roster hiding, counting and restoration

Scope: cases10,1A,1B,1C,2D only. Evidence: clean expanded-EXE1CD3 ASM
0CA4–0CD7,0DDC–0ED1 and jump-table entries1762–17BE. The larger arena
combat/escape branch in case22 remains for its own pass. No external assets
changed. Research pseudo-C, not executable host C.

## Native address arithmetic versus typed indexes

Four friendly mech records start3092:C724, stride125 bytes. Typed
`Mechs[slot]` already supplies that stride; `Mechs[slot * MECH_RecordSize]`
was a double-stride transcription error. This pass fixes those indexed
accesses in the named branches only; similarly damaged unreviewed code is
not globally rewritten without checking its instructions.

Raw action11 counts current friendly Name BYTEs !=FF and writes D31C.
Raw action1D instead counts saved D452..D455 BYTEs in signed ASCII A..Z.
These tests are deliberately different: not every non-FF byte is an uppercase
initial. The stale wLoc20_3035 reference is now the actual counter,
`SavedMechSlot_3035`, retaining its numeric suffix.

## Hide/save: raw action1C,0DDC

For slots0..3, save only each mech Name BYTE into D452+slot, then write FF
to that Name field. All other124 bytes remain untouched. Here FF marks the
temporarily unavailable record; it does not prove the mech was destroyed.
The old C tried to assign a whole record to a BYTE and included false
address-of expressions for character assignments.

The paired BYTE stores C620+slot*17 and C664+slot*17 cover character
assignment fields for slots0..3 and4..7 respectively: C664=C620+4*17.
Every one of the eight characters is put on foot(value8), including dead
slots, without changing other fields. This is not a second mech-sized offset
or a malformed write to character slot multiplied by17.

## Restore/rebuild: raw action2E,0E2C

Restore four Name BYTEs from D452..D455, not whole records. Fall through
0E54 to rebuild four sprite-family offsets: exact Name=='L' selects Locust0;
every other Name BYTE selects Commando92, including FF. Visibility/active
state is handled elsewhere; no extra live-record filter is invented here.
Finally0E90/0E92 calls1467:0002 for character0(Jason) to reassign crew.
This does not merely restore all eight earlier assignment BYTEs from a backup.

Raw action1B enters0E54 directly: rebuild sprite families and assign Jason's
crew **without** restoring saved mech names first. The shared label is now
`RebuildFriendlyMechSprites_0E54`, with the original label recorded in a
Sol comment. No native block order change is implied.

## Verification and next boundary

`Verify-FriendlyRosterTranscriptions.ps1` uses synthetic byte records and all
16 four-mech live/destroyed masks to verify name-only save/hide/restore,
unrelated-field isolation and all-eight character assignments. It also checks
all256 encoded name BYTEs for signed uppercase range/exact-L sprite choice
and verifies the paired character-address alias. These are model checks,
not compilation/gameplay tests. Existing shop/healing regressions are rerun.

Results:18692 roster assertions,8568 healing,113 shop and3374 specialist/service
assertions pass, together with whitespace validation.

Next: case22's arena combat return/escape logic, including its damaged saved
mech scan and restoration loops. Related arena helpers already have their own
earlier reviews; this pass does not certify that unreviewed parent branch.

Follow-up: case22 is now checked in [the arena return audit](BTECH_1CD3_ARENA_RETURN.md).
