# Sol: systematic BTECH_1AE8 C-to-ASM comparison

Checked2026-09-17 against baseline `1b7d0d3`: all3 retained methods.
1 locally matched,2 mismatched. Executable C is unchanged. Sol TODO comments,
corrected human summaries and fresh registry records document this checking pass.

## Evidence and boundaries

### Sol: narration correction follow-up2026-09-18

Rechecked0445..071B and corrected the existing annotated parent in place.
FAR name-table entries are loaded as strings, signed name bytes use CBW,
friendly infantry targets normalize actor4..11 to records0..7, and Mech
targets use their unsigned PilotId. Weapon narration uses the Name field.
Replaced malformed WORD strings and missing0712 label with structured native
branches; recovered exact spacing, brief Kick, possessive text, spectator
sentence and the single terminal period. Retained unconditional filter calls,
disabled-mode scratch construction and independent live arena-mode rereads.
This repairs the template, not the still-unconverted executable combat parent.

### Sol: hit/damage follow-up2026-09-18

Rechecked0758..0ED5. Fixed native WORD flags/normalized record local, signed
heat cutoff, mech-miss WORD Y=-1, arm-loss target alias, signed Jason Piloting
comparison and saved anchor restoration. Read exact EXE hit-message bytes:
DS3EDB:3ED6 is0D060D+Hit!, 3EE7 is0D060D+Hit+space. Retained damage ordering,
shared inferno/kick mutations and existing high-bit residual attack flag issue.
The four native combat tables are now EXE-owned real C data with transcription
and valid-index tests; this does not certify the full parent as executable C.

Private `BTech-Reko-expanded/BTECH.reko/BTECH_1AE8.asm` SHA256:
`55201810830D1E0D16116961360B6526048712D0A90AFBA1E7D0168327D7D643`.
Corresponding `.dis` SHA256:
`6C7B3020BF0FD6C16683BCB94935793045AD9083159B5C0CF618A5975D659854`.

Read the entire annotated file and original ASM. `.dis` completes1E46's exported
tail after POP SI with frame restoration/return, and distinguishes genuine00FF
from FFFF operands. Read3EDB initialized dialogue and selector cells562A..569A;
their246C/3092/3EDB/2FE8 bindings agree with the documented native addresses.
Rechecked current header constants and relevant shared animation/absolute-value
contracts: this exposes errors that address-only review misses.

Compared all meaningful arguments, temporaries, reads/writes, calls, conditions,
loops and exits. Compiler frames/probes/saved registers are abstracted. Matching
means local semantics under native BYTE/WORD,16:16 FAR memory-view/offset-wrap
and shared-call contracts; it does not certify modern compilation, struct layout,
shared callees, rendering/music timing or gameplay. Legacy overlapping views are
address bindings, not sequential allocations. Stack counts use native SS:BP-78.

## Whole-method ledger

| Method / native range | Result | Local workflow checked |
| --- | --- | --- |
| Combat_Mechanics /000C–12C6 | Mismatch | Full24-actor initialization, twelve interleaved movement/fire slices, movement/collision/crushing/animation, redraw, weapon/target/range/path/skill/ammo/accuracy, all dialogue, personnel repeats/damage/armour/death, mech hit location/cluster/damage/criticals/ejection, effect handoff, fired-bit/anchor cleanup, completed-order removal and final heat call |
| Combat_AudioVisual_Effects /12C7–1E45 | Mismatch | Full graphics gate, anchor/facing/streams, all weapon sound/classification, visibility/coordinate/muzzle setup, octant/error travel, impact/cutscene, frame restoration, death target clearing, ordinary wrecks and arena reinforcement branch; unassigned adapter1 tick phase remains unresolved |
| Combat_Random_CreateFire /1E46–complete split tail | Matched |2D6 gate, WORD X/Y offset additions, bit80 boundary masks, fire7C registration and return |

## Combat_Mechanics discrepancies

### Movement and animation1037–1285

Native1037 CBWs the cursor BYTE before pair indexing; the audited baseline C zero-extends.
Ordinary0..12 cursors agree; raw128..255 do not.1167 reads both WORDs of a
FAR stream entry at3EDB:025A+selector*4 and writes the actor's01F6 cursor.
Current BYTE ax_198/dx_199 and address-assignment expressions are damaged Reko
output, not equivalent pointer operations.1191 stores AL returned by1732 into
sprite-frame409A; C discards that return and writes its old ax_169 instead.
Native3920 is the actor-facing BYTE table; t3920 is an unbound legacy name.
The redraw anchor's C unsigned Piloting<8 differs from native signed BYTE<8
with CBW when the field has its high bit set. The malformed call punctuation
also remains; this research file cannot currently compile.

Correction2026-09-18: the annotated movement block now sign-extends the cursor,
stores the complete named FAR walk stream, stores1732's returned frame and
uses the named3920 facing table. The malformed redraw call is corrected.
The redraw anchor now likewise uses signed BYTE Piloting and CBW extension.
The prebiased direction base2ED1 is now the actual11-BYTE2ECC table with a
signed multiplication/bias index (no negative signed left shift). This is
preparation of the original template, NOT a claim the full mechanics parent
is converted/compiled. Target/dialogue/proficiency/stack-residue issues remain.

### Training/proficiency01D0–0238

Native reads an unsigned weapon skill index and compares it to a CBW signed
last-category BYTE. C compares unsigned bytes. On counter wrap, native constructs
FAR3092:(C5D4+actor*17+skill), tests its signed BYTE<4, then increments the BYTE.
C reads a BYTE into ptrLoc82_2620 and dereferences that value as a pointer;
neither the pointer contract nor the signed test is reproduced. Counter/category
storage D358/D360 retains legacy unbound names. Do not invent an extra skill
category or change the original wrap-to-increment rule.

Correction2026-09-18: annotated01D0..0238 now reads Weapon.SkillType, compares
the last category after CBW, and forms a real FAR skill-byte pointer with a
signed BYTE ceiling check. D358/D360 are prebiased actor bases: actor4..11
uses party-relative countersD35C..D363/categoriesD364..D36B. Named C views
alias PersistentState bytes80..95 and remain part of the saved state, not
independent initialized tables. Full combat parent conversion still pending.

### Dialogue0466–071A and hit notifications

The native branch tree separates friendly/enemy and mech/personnel narration.
Attacker-side goto labels generally retain that branch ordering despite their
awkward placement; this is not evidence of a skipped friendly attacker name.
The target spectator branch jumps to l1AE8_0712, which has no definition in C;
native0712 pushes the selected FAR text and calls1631:1DAB. The enemy target
Mech/human description also calls an undeclared Display_Text_From_Memory_IF_11832
instead of that native verbosity helper. Several parameters are WORDs assigned
strings, array entries are addressed rather
than read as FAR pointers, weapon records are passed without their text address,
pilot IDs are confused with addresses, and the target-name FAR buffer operations
are not reconstructed. Current target-name append adds two periods;068C adds one.
Old3092 references for3E9F/3EA7 literals are wrong: the literal segment is DS3EDB.

Exact current/native differences include:

| Literal offset | Native bytes/text | Current C discrepancy |
| --- | --- | --- |
|3E60 | space + `uses a` + space | trailing space omitted |
|3E81 | `Mech` + space | trailing space omitted |
|3E95 | `Kick` | uses ` on ` |
|3E9F | `'s Mech` | uses `s mech` |
|3EC0 | `a spectator.` | uses `a spectator ` |
|3ECD/3EDE | CR + `Missed!` | CR omitted |
|3ED6 | CR,06,CR + `Hit!` | prints textual `[CR][CR] Hit!` |
|3EE7 | CR,06,CR + `Hit ` | prints textual `[CR][CR] Hit!` |

The prefix06 is a text control BYTE, not the letters `[CR]` or a cosmetic space.
Both hit cases have distinct endings because the mech case appends hit location.
Friendly names are CBW signed indices, not unsigned bytes or pointer-table addresses.

### Mech-miss fire call and effects cleanup

0A99 supplies `(target,0,FFFF)`; C supplies `(target,0,00FF)`, changing Y offset
from-1 to+255. The personnel miss call0839 is already `(target,FFFF,0)` and agrees.
Both Mechanics missed-shot fire predicates currently evaluate10h..12h or0B,
matching native0810/0A79 despite using misleading Small..Large component names.
Table10..12 means medium laser, large laser and PPC;0B is infantry inferno.
Do not mechanically subtract one in these already-matching predicates. Give
them correct table-index names during fixing while preserving their numbers.
Current wLoc2A_4549 in arm-loss rental suppression is unbound; native0D20 tests
target BP-28 against13.0D9F restores saved WORD anchor BP-2A/-36; current ax_494/
ax_497 are unbound residues. The smoke-cutscene comparison uses native CBW Jason
Piloting versus target record BP-0C; C compares its unsigned BYTE directly.
The existing Bool_CombatFlagUnknown_w4586 accesses agree with native4586;
the confusing name alone is not a mismatch. The hit-location presentation call
Combat_Display_Text_From_Memory is undeclared; native0B23 calls1631:1B8F, now
named Combat_DisplayText_ArmourHitLocation. Reconcile that callee during fixing.

Native0CCA tests the incoming-damage WORD for another iteration; inferno's BP-34
clear is dead local state in this mech branch, not evidence of a missing outer
repeat loop. No invented repeated-mech-attack implementation was added.

Native BP-56 attack-applied state is not initialized with the other per-target
flags at0DE6. For a mech target,09F1 jumps straight to effects when weapon
encoding bit80 is set, before assigning BP-56 in the mech hit/miss branches.
Current Bool_wLoc58 is likewise uninitialized on that path, but host undefined
behavior does not reproduce a particular native residual WORD. Retain this as
an unresolved caller/stack/runtime contract within the mismatched method, not
a verified gameplay bug. Check actual weapon/target reachability before choosing
a port policy; no default was invented.

## Combat_AudioVisual_Effects discrepancies

### Component IDs versus zero-based weapon-table indices

Header Mech_* weapon constants are one-based component codes; Infantry_* values
are already zero-based table indices. Current comparisons mix the two. Comments
claiming zero-based comparisons were not enough to make their expressions correct.

| Purpose | Native weapon-table predicate | Current C predicate after expanding defines |
| --- | --- | --- |
| Repeating-projectile sound3 |17 or9 or13..16 |18 or9 or14..17 |
| Suppress target impact stream for mech missiles |19..1F |1A..20 |
| Traveling mech missile classification |19..1F |1A..20 |
| Mech laser/PPC classification |0F..12 |10..13 |

These differ for valid weapon indices, not just corrupt saves: first LRM19 is
missed and kick20 becomes a missile; small laser0F is missed and autocannon13
becomes a beam; autocannon13 misses sound3 and flamer18 gains it. Personnel tests,
kick32 stream selection, palette/sound numbers and family-zero stream selection
otherwise agree. A correcting pass should retain distinct component-code and
table-index names, subtracting one only for component constants.

### Text, signed gates and callee binding

Native41FC is CR + `Killed him!`; C replaces CR with a space. Arena messages4209/
4235 agree exactly. Native WORD ID/weapon relational gates are signed; C unsigned
parameters differ for8000..FFFF, including death-record<2. Ordinary caller IDs
agree, but define valid-input/sanitization policy explicitly when porting.
Native beam plot call is1F3D:031C, now named
`Draw_Clipped_Axis_Aligned_EGA_Line`; current `Draw_EGA_Line` is undeclared.
Bind the real helper rather than infer a general-purpose line algorithm.

### Unassigned adapter1 redraw phase

Native BP-4 has no assignment before193E reads its low two bits, and1968 increments
it on visible missile ticks. This reconfirms probable BUG-015; C's uninitialized
VisibleProjectileTicks is not a defined host reproduction of residual native
stack state. EGA adapter2 skips the phase gate, so it does not select a redraw
phase there. Runtime evidence/approved policy is needed for retained adapter1
compatibility. No automatic initializer or gameplay-bug claim was added.

## Confirmed native subflows and remaining validation

Initialization0034 clears24 local movement counters and24 global pair cursors.
Each of twelve slices moves all currently active actors, optionally redraws,
then scans24 actors/12 target slots. An executed attack marks its target bit80
and terminates that actor's inner scan for this slice. Active/alive state is read
during traversal, not snapshotted for the round. Completed first friendly orders
are retired by44 forward overlapping BYTE copies and setting only byte44=FF;
native local-X/Y comparisons use CBW, while C compares unsigned bytes. Both
reject raw128..255 against masked coordinates0..127, so that signedness difference
does not change order retirement. This completed-order block locally agrees.
Final view restore and heat call
occur only while main characters remain alive; heat gets FAR SS:BP-78.

Weapon resolution normalizes only record lookup: friendly infantry4..11 ->0..7,
enemy16..23 ->8..15, mechs12..15 ->4..7. Range/path checks precede ammo consumption;
ordinary ammo precedes hit roll. Slot10's WalkMove decrement and enemy kick's
combatant-versus-record argument bug remain native BUG-012/014. Accuracy starts
4+2*bracket or kick3, adds signed mode+1/movement/sensor/heat penalties, then
subtracts the selected signed skill. Personnel cover uses SAR1, mech cover SAR3.

Personnel damage is rolled once, minimum1 precedes armour, one hit roll controls
all repeats, and damage is mutated by each repeat's armour. Repeats do not stop
on zero health. Mech hit-category difference is not modulo8; location roll occurs
even before a miss/kick override. Kick mutates shared damage byte3103 and bit3
selects raw leg offset13/18. Missile cluster lookup uses2E5E+roll*7+column. Armour/
structure transfer, critical dispatch, arm-loss flag and ejection branches were
traced; signed native damage/location comparisons need native WORD contracts.
1631:1122's unassigned fatal return is stored but not dereferenced again when
ejection marks the record destroyed, under that shared-call contract.

Effects advance attack streams until the next FF token, delay only visible
shooters, and use slot24 for projectile/impact. Hidden units use compass-relative
pixel positions; muzzles are signed offsets. Native abs is a signed-WORD helper:
its argument conversion narrows subtraction, and8000 stays negative. Do not
invent a promoted host abs mismatch without checking that helper. Travel uses
signed WORD error, missile steps*4 and exact endpoint equality; no timeout or
overshoot guard exists. Impact delay5 occurs after the entire sequence; mech
shooter frame is restored. Death cleanup clears all24x12 masked target references;
ordinary wrecks apply X0F7F/YF07F masks after-1/-1. Arena target13 instead spawns
UrbanMech records6/7 (actors14/15), installs FAR2FE8:02A0, sets positions/flags,
then removes the destroyed target. Independent array-write ordering changes in
this C have no intervening calls or overlapping targets and are harmless.

Fire helper is not a destruction-only helper: both native callers are missed
shots.2D6 outside5..9 occurs12/36, a one-third chance. Offset sums wrap to WORD;
X bit80 applies0F7F, Y appliesF07F (C `~0F80` agrees in low16 bits). Sprite7C
is registered without an extra terrain check inside this helper. The caller
supplies eligibility. Legacy eight-element CharacterPosX/Y_Mech research views
must map4004/4036 across all24 combatants, not become managed array bounds.

Static models check witnesses, not executable C/gameplay. Remaining review
includes the existing A-002 runtime round lifecycle and BUG-015 phase capture.
Straightforward constant, pointer and text mismatches do not justify an expensive
Astra run. Correcting these files and validating gameplay are separate steps.
