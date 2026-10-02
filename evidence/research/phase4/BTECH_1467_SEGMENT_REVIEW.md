# `BTECH_1467` — crew assignment, written quiz and movement-plan reset

## Scope and source profile

The entire maintained `BTECH_1467.c` is reviewed in this pass. The source of
address-level evidence is `reko-expanded-2020`, not packed installation offsets.
This is annotated research pseudo-C, not a compiled or runtime-tested C# port.

| Address | Routine | Result |
|---|---|---|
| `0002..08A7` | `Assign_Pilot_and_rider_to_Mechs` | Crew assignment workflow |
| `08A8..0B97` | `Copyright_Check_Mech_Quiz` | WORD: zero passes, one fails |
| `0B98..0D7D` | `Display_Text_Mech_Names` | WORD: number of displayed mechs; selection in `3092:0068` |
| `0D7E..0DC3` | `Combat_Clear_Movement_Plans_0D7E` | Clear movement-plan bytes |

The quiz is raw code in the clean ASM listing, not a recovered function in its
`.dis`. Its instruction bytes were traced explicitly. The final routine's
listing omits the epilogue text, but `.dis` retains the ordinary return.

## Crew assignment (`0002`)

### Entry modes and local row maps

The argument is a WORD reset mode, not a character ID. Nonzero clears all eight
party character assignment bytes to `8` (on foot) and both seats of all four
party mechs to `FF` (empty). Zero synchronizes occupied seats on existing mechs
back into their characters' assignment bytes without first clearing everyone.

Existence here means `Mech.Name[0] != FF`; do not substitute the probable
high-bit enemy-state interpretation. The record stride is `7D` bytes for mechs
and `11` bytes for characters, with no additional multiplication inside a typed
array index.

Eight WORDs at `BP-18h..BP-0Ah` map compact living-character menu rows to actual
party slots. Four WORDs at `BP-08h..BP-02h` later map compact vehicle rows to
actual mech slots. Both maps initialize to `FFFF`, not byte `FF`.

The first panel shows each living character's name and on-foot/pilot/rider
status. A mounted character's padded mech name is copied after a leading space
in the dynamic string and trimmed using the actual returned string length,
not a fixed eleven-byte limit. The second panel shows each existing mech and
its occupants; names there are resolved through `Infantry[occupant].Name`.

### Changing an assignment

1. Map the character row to its party slot and display its piloting skill.
2. Build the existing-mech menu plus a final `None` row.
3. Remove that character from every old pilot/rider seat before reading the
   vehicle choice. There is no rollback branch in this routine.
4. `None` puts the character on foot and redraws the assignment menu.
5. An unskilled character can only become a rider. A qualified character takes
   an empty pilot seat automatically; replacing an occupied pilot requires a
   yes answer. A no answer selects the rider seat instead.
6. Replacing a rider dismounts that rider. Replacing a pilot dismounts any
   existing rider and moves the old pilot to the rider seat; the old pilot's
   character assignment stays linked to the mech.
7. Write the selected party slot into the chosen seat and the actual mech slot
   into that character's assignment byte.

The traitor warning applies when the selected party slot is the active traitor,
even if the selected seat is the rider seat. Its cockpit wording does not gate
the assignment. It sets the shared WORD at `3092:374A`, consumes a key, then
continues normally.

### Done and abandonment

`Done` succeeds when every existing mech has a pilot. Characters may remain on
foot, and rider seats need not be filled.

If any mech lacks a pilot, the routine counts all living qualified characters
and all existing mechs. When there are enough qualified pilots, it requires the
player to finish assigning them and does not offer abandonment. When there are
too few, it offers to abandon unpiloted mechs. Confirmation writes only `FF`
to each abandoned mech's first name byte and sets characters linked to that
mech on foot; the remaining record bytes are not erased.

After abandonment the menu is redrawn with completion still false. The player
must select `Done` again. Successful completion clears `D452..D455` to `FF` and
clears `Party_UnassignedMechs_D324`. These are hidden-name snapshot bytes,
not writes to `3092:0000..0003` as the former comment suggested.

At `0838`, the machine code adds `BP` to `BX` and reads the local maps through
default `DS`, rather than the usual BP-based `SS` view. The literal instructions
and `.dis` agree. The reconstruction assumes those segment views alias for
this stack; confirming the startup DS/SS relationship is recorded under A-003.
This does not justify changing the effective address or declaring a game bug.

## Dormant written quiz (`08A8`)

The bypass is at the caller, `0FDC:0056`. This baseline initializes the question
counter to zero at `1467:0974`; the old claim that this routine's counter had
been patched to `81` is not supported by these bytes.

If the shared BTSTATS cache flag is clear, the routine requests disk 1, loads
`BTSTATS.CMP`, and clears the graphics compatibility flag. The original then
rebuilds adapter-0 colour-conversion tables; that deleted graphics path remains
a comment, not a new dependency in the maintained EGA reconstruction. The
initial full-image blit is conditional on graphics adapter 2. The diagram is
redrawn for every question.

There are twenty displayed part-name choices, referenced by twenty 16:16
pointers at `3EDB:4DE2`. Only ten target positions are used. Three distinct
target ordinals are selected with `RandomByte() % 10`; duplicate detection is
reset for every retry at `0B20`. The former C kept its duplicate flag true and
could misleadingly suggest an infinite retry loop. All three questions are
asked even after a wrong answer, because the failure flag is sticky.

The tables at `28C4`, `28D8`, and `28EC` each contain ten WORDs:

| Target ordinal | X | Y | Correct menu row | Stored part name |
|---:|---:|---:|---:|---|
| 0 | 151 | 186 | 2 | Foot Unit |
| 1 | 230 | 189 | 4 | Foot Casting |
| 2 | 240 | 168 | 5 | Foot Actuator |
| 3 | 221 | 120 | 7 | Leg Mainshaft |
| 4 | 239 | 88 | 8 | Balance Strut |
| 5 | 248 | 55 | 10 | Elbow Actuator |
| 6 | 213 | 69 | 18 | Gyro Housing |
| 7 | 217 | 26 | 15 | Torso Mainframe |
| 8 | 193 | 38 | 0 | Intercooler |
| 9 | 157 | 95 | 16 | Jump Jet Intake |

All 30 WORD values were checked against the listing bytes; names were resolved
through the corresponding original far-pointer table entries.

The target box spans `(X-3,Y-3)..(X+5,Y+5)`. Its four edges are white. The
connector is an inclusive filled rectangle `(128,Y-1)..(X-3,Y+3)`, not a thin
line despite the legacy helper name. The vertical divider uses colour 8 for
adapter 0, otherwise 9; it is not always white as the old C implied.

On exit, the routine clears the screen and restores the health/C-Bills sidebar.
Any incorrect answer displays the failure message and sets the byte cooldown
at `3092:D320` to `FF`. `MOV AX,[BP-12h]` at `0B8F` returns the failure WORD.
The existing caller uses that result to suppress the training interaction.

`CHICODE1.GIF` remains an external labelled reference that the game never opens;
only executable-resident answer/name tables are documented here.

## Friendly-mech selector (`0B98`)

The four-byte table `3092:D558..D55B` is a compact row-to-mech-slot map. The
routine clears it, then includes slots with a non-`FF` first name byte. During
hidden-name staging (`D324 != 0`), it tests the saved initials at `D452` instead.

For hidden names it constructs a temporary string from the saved initial and
the remaining name bytes beginning at `record+1` (`C725`). That address is
inside `Name[16]`, not the tonnage byte at `record+10h`. The hidden record itself
is not restored by displaying it.

The original ownership-name lookup directly indexes `CharacterNameList` with
the mech's `PilotId`, without the `Infantry[PilotId].Name` indirection used by
the crew panel. This is preserved explicitly. A live roster with differing
party-slot/name IDs should be checked before classifying its displayed result
as a confirmed naming bug. The routine also has no `PilotId == FF` guard.

After menu `17h`, the selected compact row is translated through `D558` into
the global WORD `Mech_Selected` at `3092:0068`. The function's actual AX return
is the displayed row count, not the selection. It restores generic menu WORD
`305B:0202` to one.

There is no early exit for an empty list: menu `17h` is still called with zero
rows. The cleared row map does not establish that the menu's returned index is
safe. This remains a caller/menu-helper precondition to audit, not a silently
inserted safety fix in the reconstruction.

## Movement-plan reset (`0D7E`)

Two WORD loop counters clear exactly 24 rows of 24 bytes at
`3092:40B4..42F3`, totaling `240h` bytes. Related combat code uses the rows as
up to twelve adjacent X/Y movement pairs per combatant. Zeroing is distinct
from filling them with the `02` end-of-plan marker used by plan construction.

The routine does not clear current world positions, active flags, the step
indexes at `0078`, or the visibility table at `42F6`. The stale generated call
`A_fn1467_0D7E()` in `Combat_Mechanics` now calls the maintained descriptive
routine. Other legacy `a40B4/a40B5` aliases are left for the combat segment pass.

## Dependency correction and verification

The directly used string helper `207F:3B9E` is reconstructed as a WORD-returning
byte-string-length scan over a far pointer. `REPNE SCASB` starts with
`CX=FFFF`, then `NOT CX; DEC CX` excludes the terminator. The prior C used a
byte counter, reversed the terminating condition and declared a void result.
The rewritten helper retains the native scan ceiling rather than inventing a
bounded host-string contract.

Confidence is high for branch conditions, record strides, menu maps, mutations,
table widths and return values. Unresolved work is limited to startup stack/data
alias confirmation, selector naming/empty-list preconditions and later combat
alias consolidation. No expensive model run was started.

Checks: all 30 quiz WORDs and ten target names checked against the clean ASM
data; `git diff --check`; the existing InceptionTools verification suite
(182 assertions passed). The
tooling suite is a regression check, not proof that this pseudo-C executes.
