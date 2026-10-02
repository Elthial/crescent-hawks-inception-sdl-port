# Original-game bugs and suspicious behaviours

This register records faults and implementation quirks present in the shipped
16-bit game executable. It is deliberately separate from decompiler mistakes,
incorrect historical annotations, and defects in InceptionTools.

The list will grow as annotated code is checked against the clean assembly.
Every entry records what the executable actually does, how confident that
interpretation is, and the compatibility decision a replacement executable
will eventually need to make.

## Classification

- **Verified defect:** the instructions prove unsafe or internally inconsistent
  behaviour without relying on inferred intent.
- **Probable logic bug:** the instructions are verified, while calling the
  behaviour unintended requires a small inference about the game rules.
- **Compatibility quirk:** unusual shipped behaviour is verified, but it may be
  deliberate or harmless and should not yet be called a defect.

## BUG-001 — Heat-sink salvage is not resource limited

- **Location:** `0DAB:0259-031E`.
- **Classification:** Probable logic bug.
- **Confidence:** instruction flow verified; original intent inferred.

The post-battle salvage routine counts intact heat-sink critical slots in enemy
mech wrecks. It then repairs every eligible destroyed heat sink in the friendly
lance, decrementing the count after each repair.

The routine never tests whether the count is nonzero. If no enemy heat sink was
counted, the first repair changes the 16-bit counter from `0x0000` to `0xFFFF`
and repairs continue normally. The counter is not used after this pass.

The effective shipped rule is therefore:

```text
Tech >= Average + surviving supporting location => repair every heat sink
```

rather than limiting repairs to the heat sinks actually recovered. This may be
an abandoned accounting mechanism, but the unconditional `DEC` and absence of
a gate are explicit. See
[heat-sink salvage](../phase4/BTECH_0DAB_HEAT_SINK_SALVAGE.md).

**Porting policy:** reproduce by default for game compatibility. A limited-
inventory version should be exposed only as an intentional rules fix.

## BUG-002 — SRM-6 salvage counter is uninitialized

- **Location:** `0DAB:031F-0429`.
- **Classification:** Verified defect.
- **Confidence:** verified from stack offsets and loop bounds.

Weapon salvage accepts component IDs `0x10..0x20`, requiring seventeen
byte-sized counters. The native table expression maps them to stack bytes
`bp-24..bp-14`. Its initialization loop clears only sixteen bytes,
`bp-24..bp-15`.

The last counter, component `0x20` (SR Missile 6), retains prior stack contents.
Enemy SRM-6 slots increment that unknown byte and destroyed friendly SRM-6
slots test it before repair. Results may therefore depend on unrelated residue
in the function's stack frame. See
[weapon-component salvage](../phase4/BTECH_0DAB_WEAPON_SALVAGE.md).

**Preservation policy:** neither C17 nor managed C# should read an uninitialized
host local or silently zero this counter. The preservation C currently guards
actual accesses pending an evidence-backed native storage binding. The normal
acknowledgement path has a saved-DS overlap candidate; see
[entry trace](../phase4/NATIVE_SRM6_SALVAGE_ENTRY.md). A future C# edition could
offer a labelled zero-start bug fix, but that is not preservation behaviour.

## BUG-003 — Scrap payout uses technician party slot as multiplier

- **Location:** `0DAB:042A-04F2`.
- **Classification:** Probable logic bug.
- **Confidence:** operand selection verified; intended operand inferred.

For each enemy wreck, the game generates a base scrap value from 90 through
217. It then multiplies the sum by `BestTechMemberId`, the selected technician's
zero-based party slot, rather than by `HighestTechSkill`.

The same party-slot value gates the entire payout. If Jason in slot zero is the
best technician, the game skips the reward. Slots 1 through 7 multiply the
reward by 1 through 7, so rearranging which character holds the best Tech score
can change the payment independently of that score.

The assembly local is unambiguous: `[bp-4]` is both tested and multiplied,
whereas the Tech skill is held at `[bp-6]`. See
[post-battle scrap payout](../phase4/BTECH_0DAB_SCRAP_PAYOUT.md).

**Porting policy:** preserve the party-slot calculation in compatibility mode.
A corrected-rules mode would most plausibly gate on wreck count alone and use
Tech skill as the multiplier, but that proposed correction is not evidence of
the original designers' exact intent.

## BUG-004 — Infantry weapon-presence test reads mech-record bytes

- **Location:** `0DAB:0995-09AD`.
- **Classification:** Probable logic bug.
- **Confidence:** effective addresses verified; intended source expression and
  gameplay frequency require confirmation.

The post-battle infantry-loot scan visits combatant IDs `4..11`. For every
lootable-infantry flag, it tests whether a weapon exists by reading:

```text
3092:C6EB + combatantId * 0x11
```

This produces addresses `C72F..C7A6`, which cross the documented 0x7D-byte mech
records beginning at `C724`. Later in the same function, the actual recovered
weapon IDs are consistently read from:

```text
3092:C6A7 + lootSlot * 0x11
```

Those are the `Weapon` bytes of `Infantry[8..15]`. The suspicious first test can
therefore enable or suppress the confiscated-weapons flow according to unrelated
mech bytes. The executable instruction and effective addresses are certain;
calling it unintended is the inference. See
[post-battle infantry loot](../phase4/BTECH_0DAB_INFANTRY_LOOT.md) and Astra
candidate A-006.

**Porting policy:** reproduce the raw address test in strict compatibility mode.
Normal corrected mode should test the eight recovered weapon IDs actually
consumed by the later loop, after a live-memory trace confirms the alias model.

## BUG-005 — Encounter placement reads outside its combat-map cache

- **Location:** `0DAB:1174-11ED` and `0DAB:1206-1250`.
- **Classification:** Verified defect.
- **Confidence:** instruction order and reachable negative indexes verified.

Random-encounter infantry placement calculates a signed candidate index into
the `0x240`-byte combat-map cache at `246C:07AD`. It reads the byte at that
index before testing whether the index is negative, and it performs no upper-
bound test before the read. A later projection check rejects positions outside
the cache, but that happens after the memory access.

A negative candidate is reachable when the party is near the local top edge
and the randomized encounter Y offset is sufficiently negative. In the 16-bit
DOS memory model, the negative subscript reads another byte earlier in segment
`246C` rather than triggering managed bounds enforcement. The value cannot be
accepted while the explicit negative test remains, but the read itself is
outside the declared cache. Forward scanning can likewise read beyond the
cache's upper end before later rejecting the resulting position.

Enemy-'Mech placement compounds the bounds problem by reading both a primary
cell and an intended horizontal neighbour. The later projection bounds cover
only the primary anchor. At cache column 23, a `primary + 1` neighbour wraps to
the next row; at column zero, `primary - 1` wraps to the preceding row unless
it is also below index zero. The routine can therefore accept two bytes from
different cache rows as a horizontal two-cell 'Mech footprint.

See [enemy infantry placement](../phase4/BTECH_0DAB_ENEMY_INFANTRY_PLACEMENT.md)
and [enemy 'Mech placement](../phase4/BTECH_0DAB_ENEMY_MECH_PLACEMENT.md).

**Porting policy:** bounds-check every candidate index against `0..0x23F`
before reading in normal C# code. For a two-cell 'Mech footprint, also require
both indexes to have the same quotient when divided by 24. Reproducing adjacent-
segment reads or row wrapping offers no useful gameplay compatibility and would
create unnecessary dependence on memory layout.

## BUG-006 — One-point green mech-status segments disappear

- **Location:** `0DAB:174C-1857`, exposed through `1F3D:01FB`.
- **Classification:** Probable logic bug.
- **Confidence:** coordinate flow and rectangle rejection verified; intended
  appearance inferred.

The BTSTATS vertical gauge first draws its red segment and then moves the bottom
coordinate one row above it. It recalculates the green segment's top only when
the green height is greater than one. When red is nonzero and green is exactly
one, the cached top still points at the top of the red segment while the new
bottom is one row above it.

The resulting call has `topY > bottomY`. The rectangle primitive at `1F3D:01FB`
explicitly exits when its ending Y is less than its starting Y, so it draws
nothing. This is reachable for a mech location with one point of armour over
nonzero structure, and for the heat gauge at heat 29 (one point of unused heat
capacity over 29 red points).

See [mech-status renderers](../phase4/BTECH_0DAB_MECH_STATUS_RENDERERS.md).

**Porting policy:** preserve the omission in strict compatibility mode. Normal
rendering should treat a height of one as a valid one-row rectangle by resetting
the green top to the adjusted bottom before drawing.

## QUIRK-001 — Every BLD load decodes 9000 bytes

- **Location:** `Load_And_Decode_Indexed_BLD_1D30`, `0FDC:1D30-1DC1`.
- **Classification:** Compatibility quirk; possible overrun/stale-data defect.
- **Confidence:** transform length verified; gameplay impact unresolved.

The executable transforms exactly `0x2328` (9000) bytes beginning at
`3092:00A0` after loading a BLD payload, regardless of that payload's actual
length. Even the largest known external BLD payload is only 8499 bytes. The
decode loop consequently transforms hundreds or thousands of bytes beyond the
new file data, including whatever remains in the shared buffer.

The destination region is large enough for the fixed loop in the observed
memory layout, so this is not yet proven to corrupt an adjacent allocation. It
does process stale/non-file bytes and differs from a conventional bounded
loader. See [BLD format](../formats/BLD.md).

**Porting policy:** decode only the supplied payload in safe tooling and normal
portable runtime code. Retain the fixed 9000-byte behavior as a documented
compatibility option until full BLD execution proves that no script depends on
post-payload buffer state.

## BUG-007 — Repairable damage suppresses unrepairable-damage warnings

- **Location:** `Mechlube_Repair_Mech`, `11B8:0164-01E1`.
- **Classification:** Probable logic bug.
- **Confidence:** branch placement verified; intended user feedback inferred.

The Mech-Lube tests the engine, gyro, and sensor damage counters only when the
mech has no missing armour or structure, destroyed weapons or heat sinks, or
actuator-byte mismatch. If any repairable damage exists, control skips the
unrepairable-component scan and enters the paid repair workflow.

The routine never performs that scan after repairs. A mech with both armour
damage and engine damage can therefore receive armour service without ever
being told that the facility cannot repair its engine. When the scan does run,
it reports only the first nonzero counter in engine, gyro, sensor order. See
[Mech-Lube repair diagnostics](../phase4/BTECH_11B8_MECHLUBE_REPAIR_DIAGNOSTICS.md).

**Porting policy:** preserve the branch order in strict compatibility mode. A
normal replacement UI should report remaining engine, gyro, and sensor damage
after assessing or completing the available repairs.

## BUG-008 — One weapon purchase clears every matching critical slot

- **Location:** `Mechlube_Repair_Mech`, `11B8:068D-06EF`.
- **Classification:** Probable logic bug.
- **Confidence:** mutation/count mismatch verified; intended charging unit
  inferred from the menu wording.

The weapon-repair menu counts every destroyed critical-slot byte by weapon
type and advertises a price of 300 C-bills “per item.” After one selection, it
charges 300 and decrements that type's count by exactly one. Its subsequent
critical-block loop does not stop after one match, however: it clears the
destroyed bit from every slot with the selected raw component byte.

With two matching destroyed slots, the first purchase repairs both record
bytes but leaves a menu count of one. The player can pay again for the phantom
entry, or select “Nothing” and retain both repairs after paying only once. See
[Mech-Lube weapon repair](../phase4/BTECH_11B8_MECHLUBE_WEAPON_REPAIR.md).

**Porting policy:** preserve the mismatched count and mutation in strict
compatibility mode. Normal code should make the transaction atomic: either
charge 300 and clear one matching destroyed slot, or explicitly price and
clear the whole selected group while synchronizing its count.

## BUG-009 — Weapon completion scan omits destroyed SRM-6 launchers

- **Location:** `Mechlube_Repair_Mech`, `11B8:06EF-0717`.
- **Classification:** Verified off-by-one logic bug.
- **Confidence:** loop bounds and component mapping verified.

The repair menu lists all seventeen mech weapon indexes `0x00..0x10`, where
index `0x10` is component `0x20`, SRMissile6. After a purchase, the routine
rebuilds its “destroyed weapons remain” flag using the strict bound
`index < 0x10`, so it inspects only indexes `0x00..0x0F`.

If an SRM-6 remains damaged while all lower-index counts are zero, the routine
announces that all weapons are fixed and leaves the destroyed SRM-6 slot in the
mech record. This can occur when the player repairs the last damaged weapon of
another type before selecting the SRM-6. See
[Mech-Lube weapon repair](../phase4/BTECH_11B8_MECHLUBE_WEAPON_REPAIR.md).

**Porting policy:** preserve the strict `< 0x10` rescan only in compatibility
mode. Normal code should inspect all seventeen entries through index `0x10`
and derive completion from the actual critical-slot state.

## BUG-010 — Unsupported mech modification reads its price from dialogue text

- **Location:** `Mechlube_Modify_Mech`, `11B8:080A-0924`, followed by
  `Mechlube_Upgrade_Mech`, `11B8:0925-0D53`.
- **Classification:** Verified out-of-bounds table read and no-op purchase.
- **Confidence:** selector mapping, adjacent bytes, charge, and switch bound
  verified directly from the expanded executable.

Modification setup supports eight price-table entries for the two stages of
Locust, Wasp, Stinger, and Commando. A selected mech whose byte `+0x7B` is
greater than three is mapped to sentinel selector `8`. The native code still
indexes the eight-WORD table at `3EDB:1D8C` with that value.

Index eight lands at `3EDB:1D9C`, the beginning of the following dialogue.
Its first two bytes are `CR CR`, so they are interpreted little-endian as price
`0x0D0D` (3341 C-bills). The purchase routine can subtract that amount, but its
upgrade dispatch accepts only selectors `0..7`; selector eight applies no mech
mutation before displaying the normal completion message. See
[Mech-Lube modification setup](../phase4/BTECH_11B8_MECHLUBE_MODIFICATION_SETUP.md)
and
[modification purchase](../phase4/BTECH_11B8_MECHLUBE_MODIFICATION_PURCHASE.md).

**Porting policy:** strict compatibility mode may reproduce the 3341-C-bill
no-op. Normal code should reject unsupported package selectors before quoting
or charging and should never express the sentinel as an out-of-bounds access.

## BUG-011 — Rex recruitment overruns the friendly-mech tables when full

- **Location:** `11B8:10DA-114D`.
- **Classification:** Verified missing-bounds defect; ordinary-play
  reachability unresolved.
- **Confidence:** loop exit and effective addresses verified directly.

Rex's scripted recruitment searches `3092:D452..D455` for the first `FF` byte,
which represents a free hidden party-mech slot. The scan stops when its index
reaches four, but the routine does not reject that value before using it. If all
four slots are occupied, it copies the Commando template into mech record 4
(normally the first enemy record), writes `43h` through `D452[4]` onto `D456`
(the next recruit Name ID), and writes the Commando sprite-family value through
friendly-mech index 4 onto `D562`, Jason's on-foot sprite-family byte.

The unsafe instruction sequence is certain. It remains unresolved whether the
normal story can reach the Kurita-party event with four occupied hidden mech
slots; an edited save can certainly create the condition. See
[Rex recruitment and Kurita ambush](../phase4/BTECH_11B8_REX_KURITA_AMBUSH.md).

**Porting policy:** normal C# code should require a free slot and report or
handle a full lance without writing beyond its arrays. Strict compatibility
mode can document the original addresses, but should not reproduce adjacent
memory corruption in managed objects.

## BUG-012 — Dense mech loadouts overrun ammo interpretation and weapon-menu termination

- **Location:** `1543:0041..008E`, `0151..018A`, `024E..02B9`,
  `045A..04BE`, and `0508..0523`.
- **Classification:** Verified inconsistent bounds/missing terminator;
  ordinary-play loadout reachability unresolved.
- **Confidence:** twelve-entry collector, ten-byte record field and indexed
  instruction operands directly verified in the expanded baseline.

The combat weapon menu collects up to twelve weapon-valued critical bytes, but
mech records hold only ten current-ammo bytes at `+27h..30h`. With eleven or more
collected entries, ammo display/availability checks read `+31h` and `+32h`, the
walking/jumping movement fields. These bytes can make an extra weapon appear
to have ammunition without a real corresponding ammo field. Sol: consumption
is now traced at `1AE8:0239..0296`: slot10 passes the `<11` check and decrements
WalkMove at31 unless FF, after range/blocking checks and before the hit roll.
Slot11 takes the separate kick path. Enemy biased baseC363 resolves to the
same current-ammo field in the normalized mech record; it is not a second bug.
See the [combat linkage review](../phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md).

The twelve-byte local list is zero-filled, but twelve collected entries overwrite
every zero. The menu then calls the string-length helper without appending a
terminator; its count/geometry and Done index depend on unrelated following
stack bytes. Those instructions are certain. A densely populated edited save
can create the condition; whether normal templates/upgrades reach it remains
under A-004, including the question of repeated critical-slot occupancy.

See the [1543 segment review](../phase4/BTECH_1543_SEGMENT_REVIEW.md).

**Modern C#/redesign policy (not the preservation C build):** derive an explicit bounded weapon list and keep action slots
separate from ammo-field indexes. Reject or explain unsupported loadouts rather
than reading movement bytes as ammo or scanning beyond a local array. A strict
compatibility mode may document the defect but should not reproduce stack
overreads in managed code.

**Preservation C policy:** retain raw ammo-offset reads, including ordinal10's
WalkMove and ordinal11's JumpMove aliases. The production1631:10A2 lookup
now does so. Do not add a weapon-list terminator, use the collector count in
place of the native strlen, or invoke host-C undefined behaviour to imitate
the stack overread. The1543:0004 conversion must explicitly represent relevant
native stack bytes/state for the dense-list case. BP-20..BP-15 are the twelve
list bytes; the first following WORD atBP-14 is unassigned before the initial
strlen calls and later stores a text row when an active target is displayed.
Its initial contents need a native-stack preservation contract, not a guessed
zero. Normal shorter lists terminate inside their initialized twelve bytes.

## BUG-013 — enemy heat upper clamp uses friendly index

**Address:** `1631:0E4E..0E81`, `Combat_Mech_HeatLevels`.

**Classification:** verified instruction-level index discrepancy; probable
gameplay bug, not runtime-reproduced. Sol: each friendly slot 0..3 is processed
with record offset0 then4. Heat accumulation and negative clamp use slot+offset.
The upper clamp at0E6C instead loads only slot and compares/stores heat30 at
`3092:006E+slot`. Enemy heat can exceed30 while the paired friendly value is
checked a second time. Signed-byte arithmetic can subsequently wrap if enemy
heat continues accumulating; actual reachability needs a live combat trace.

Raw EXE bytes at0E6C load BX from [BP-8] without adding [BP-0A]; `.dis`
confirms the signed comparison. The preceding negative clamp does add the
offset. See the [1631 assembly audit](../phase4/BTECH_1631_SEGMENT_REVIEW.md).

**Porting policy:** clamp the actual mech record in a corrected implementation;
retain this discrepancy in compatibility documentation, not as an unnoticed
friendly/enemy asymmetry. Do not describe it as an observed player exploit yet.

## BUG-014 — enemy kick actuator penalty uses combatant ID as mech-record ID

**Address:** `1AE8:02A8..02E0`, kick to-hit calculation.

**Classification:** verified index discrepancy, probable original game bug;
player-visible effect has not been runtime-reproduced. Sol: the kick path
calculates recordID=enemyID-8 in BP-7A, but both calls to `1631:1B44` instead
push the original combatant ID in BP-2. That helper reads C724+id*7D+24/25,
so enemy IDs12..15 read beyond Mechs[8], instead of records4..7. The correct
normalization exists in kick availability handling earlier in the routine.

Raw bytes at02BC include `FF 76 FE`, push [BP-2], before the far1B44 call;
the second call follows the same pattern. See the
[combat linkage review](../phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md).

**Porting policy:** use the normalized mech record when calculating actuator
penalties in a corrected C# implementation. Retain the original discrepancy
in compatibility notes; do not silently normalize the annotated original call.

## BUG-015 — adapter-1 missile redraw counter starts uninitialized (probable)

Sol: 1AE8:12C7 allocates locals but never writes WORD SS:BP-4 before
193E tests its low BYTE bits03. 1968 increments it only for visible missile
steps. Full primary ASM12C7..1E45 contains no initialization of this local;
other BP-4 writes belong to other routines. Allocator207F:2FDC..2FEE checks the
stack limit and adjusts SP without clearing locals. Adapter1 therefore chooses
its initial every-fourth redraw phase from prior stack contents. Other adapter
paths do not gate drawing on it. This is instruction-level evidence, not a
live DOS reproduction, and likely affects timing/initial frame rather than
damage or game state. It may be obsolete for the EGA-only preservation path.

Compatibility policy: name/document the uninitialized research local; a managed
port should explicitly choose a deterministic initial counter instead of
pretending zero was present in the original. See
[combat effects review](../phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md).

## BUG-016 — personnel-flight target lookup does not strip the fired bit (probable)

Sol:183B:0DF3..0E4D reads enemy personnel's first target BYTE3800+id*12,
CBW sign-extends it and directly indexes position WORD tables4004/4036.
There is no low7-bit mask or FF sentinel rejection. Raw target80..FF becomes
negative index -128..-1, outside the24-entry coordinate views. Mechanics
1AE8 marks executed target entries with bit80, so the flow merits runtime
confirmation; exact live reachability of this flight branch remains unverified.
Incorrect distance input can affect which personnel flee. This is not a newly
introduced pointer error: preserve the observed signed lookup in research C.
Managed port should validate sentinel/ID and explicitly decide masking policy.
See [parent round-end review](../phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md).

## BUG-017 — armour salvage technician check fails to exclude dead records (probable)

Sol:183B:115B CBW sign-extends character Name BYTE C614;115C compares AX
with00FFh. FF becomesFFFF, so equality can never exclude a dead record.
Expanded EXE bytes98-3D-FF-00 and Reko intermediate int8-to-int16 conversion
confirm this is original behavior, not an annotated-C error. With a surviving
friendly mech and enemy mech wreck, a dead character retaining nonzero Tech
at C61D can trigger armour dialog0DAB:0002. Player-visible reachability still
needs confirmation of skill retention and the dialog's own checks. The nearby
pilot/healing loops correctly compare the BYTE directly withFF.
Preserve the signed comparison in research C; explicitly choose corrected
dead-character eligibility when porting. See
[post-combat review](../phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md).

## BUG-018 — movement destination UI lacks a full-row write guard (probable)

Sol:183B:1DF3..1E1B scans mode BYTES in the48-byte order row, incrementing
offset by4; a full row produces offset48. With movement budget remaining,
217B..21AA writes a new four-BYTE destination at that offset without checking
capacity. It therefore overwrites the next row's first destination (or memory
following the last row). ASM confirms the missing guard, not just malformed
Reko C. Player-visible reachability depends on whether a full twelve-order row
can retain movement budget under legal game state; this remains unverified.
Preserve observed research behavior; managed port should reject full-row entry.
See [destination UI review](../phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md).

## BUG-019 — Clipped sprite cleanup writes to an unprepared I/O port

- **Location:** `207F:0377..0447`, cleanup `0568`; effects caller `1631:1FB3..1FD5`.
- **Classification:** Verified defect in register/cleanup control flow; visible
  gameplay impact not yet runtime-observed.
- **Confidence:** High for instruction behavior. Primary ASM and raw expandedEXE
  agree that0568 executes MOV AX,0F02h / OUT DX,AX without loading a port.

Normal sprite drawing leaves DX=03C4h, so cleanup enables all four sequencer
write planes. Fully clipped top/bottom/left/right early exits can branch to
cleanup before DX was initialized to that port. The effects caller explicitly
sets DX=AC00h to push its destination segment; no intervening instruction
changes DX before calling0377. A clipped early return can therefore output
0F02h to AC00h instead of03C4h. Partial top clipping additionally uses unsigned
MUL, so a subsequent early exit may output to the product's high WORD in DX
(normally0 for small sprites).

This is an original register-lifetime defect, not the false BYTE clipping
conditions in Reko C. The player-visible effect may be absent for unused ports,
but could leave write-plane state unrestored; actual consequences require a
safe DOS/emulator trace. Research transcription preserves incoming/MUL-modified
DX through CleanupPort. Future renderer should restore known graphics state
explicitly rather than reproduce arbitrary hardware-port writes, with a
documented compatibility decision.
See [207F planar graphics checkpoint](../phase4/BTECH_207F_SEGMENT_REVIEW.md).

## BUG-020 — Procedural rectangle edges retain mismatched register state

- **Location:** `207F:0DF8`, especially `0EFB..0F34`, `0F34..0F5F`, `0F8B..0FE7`.
- **Classification:** Probable logic bug; shipped instruction behavior verified,
  intent and visible gameplay consequences not runtime-confirmed.
- **Confidence:** High for register arithmetic, provisional for calling it unintended.

Sol: Rectangle subdivision fills left, bottom and right midpoints. Left noise
calculation leaves the BYTE perturbation in DH. Subsequent bottom/right mean
calculations overwrite DL but do not clear DH before ADD DX,BX and SHR DX.
The retained noise's high bits therefore influence their BYTE mean, including
16-bit carry/overflow. A clean independent endpoint average changes the result.

Right-edge noise then uses MOV DH,AL; SHR DH three times. AL still contains
the horizontal half-span, while the corresponding left-edge path uses the
vertical half-span stored at0278h. Normal9x9 subdivision half-widths4/2/1
produce amplitude0 and maskFFh, adding the full seed BYTE rather than the
left edge's smaller balanced perturbation. BYTE wrap occurs before clearing
high-bit results to zero; effects depend on seed, corners and whether the
midpoint was already filled.

Primary ASM and local EXE0FC1 bytes8A-F0-D0-EE-D0-EE-D0-EE agree. Both
asymmetries are preserved in research transcription. They resemble stale/
wrong-register defects, but procedural visual variation could conceivably be
intentional. Capture original lattice/DX/AL around these instructions before
promoting the entry to a verified unintended defect or proposing a correction.
Future C# compatibility should retain literal behavior by default; any symmetric
terrain-generator alternative needs an explicit opt-in compatibility decision.
See [construction notes](../phase4/BTECH_207F_SEGMENT_REVIEW.md) and optional
[A-008 confirmation](ASTRA_REVIEW.md). No expensive-model review was run.

## BUG-021 — Zero-delay waypoint arrival leaves a roaming NPC unspawned (probable)

**Address:**0800:24C2, arrival27BB..2828, delay255E..2582 and respawn2504..2552.
**Classification:** probable logic bug. Instruction behaviour is confirmed;
unintended disappearance and gameplay frequency still require emulator observation.

On reaching a destination, the original assigns MovementDelay = Rand &31, then
clears both combatant world coordinates to zero regardless of that result. The
respawn branch is entered only while MovementDelay is nonzero, and materializes
the NPC when decrement reaches zero. A zero roll therefore bypasses respawn on
subsequent updates: worldX=0 fails the unsigned strict left-boundary comparison.
The waypoint/destination is still advanced. This routine cannot restore the NPC
until another workflow resets its coordinates or delay (for example map loading).
This is not the separate deliberate Rick/lounge hold, which requires nonzero delay.

Evidence: original ASM27BB writes masked AL directly into D399 and clears
4004/4036;257F branches to24EC only for nonzero delay. The C preservation
walking/NPC suite exercises arrival with random0 and a following update, alongside
ordinary positive-delay respawning. It uses a test-only movement-planner boundary,
not a claim of full-game or emulator reproduction. See
[preserved routine](../../CrescentHawksInception/src/Original/BTECH_0800_NPCS.c).
Preserve the zero result and disappearing state; do not silently force delay1.

## BUG-022 — Enemy personnel AI passes rebased IDs to the movement-budget routine (probable)

Location:1631:041B..0420 and046F..047B, with183B:2474..24EF.
The instruction-level argument mismatch is confirmed; gameplay impact remains
probable pending a focused emulator comparison.

Enemy combatant IDs first subtract12 and receive record-bank offset4. The
personnel budget call pushes that side-local ID plus4, rather than restoring
the full combatant ID. Consequently enemies16..19 pass8..11. The budget helper
interprets those as FRIENDLY combatants and reads party character records4..7,
using their Dexterity/armour. Enemies20..23 pass12..15 and receive the helper's
fixed6-point enemy budget. The same routine correctly gets full combatant IDs
from other callers; this discrepancy is not merely malformed pseudo-C.

The preservation AI test witnesses enemy16 using party record4 with Dexterity4
and receiving3 points. Intended enemy-stat/budget policy and ordinary-play
effects are not certified by this synthetic test. Preserve the native argument
in C; a later modern redesign can resolve the inconsistency deliberately.

## BUG-023 — Personnel-versus-Mech impact effects inherit the preceding attack result

**Classification:** compatibility quirk; probable unintended presentation dependency.
The retained flag and effects branch are ASM-confirmed. Ordinary gameplay
frequency and player-visible rendering still require original-EXE observation.

At1AE8:09F1 the weapon table's bit80 marks personnel damage encodings. Against
a Mech, that bit takes09F1/0A05 directly to effects0CD3: no damage is applied
and no new value is written to BP-56. Ordinary attacks previously assign this
WORD on hit/miss paths (including0B23 and the miss branch after0A42). It is not
reset per actor, weapon or slice. Effects1A07 reads it, and nonzero1A10 plays
sound4 before the target-visibility gate. Consequently a no-damage personnel
attack can play the impact sound after an earlier hit but not after an earlier
miss. The earlier attack may belong to a different combatant.

The preservation execution test compares Small Laser alone with Small Laser
followed by Rex's Pistol against the same enemy Mech. Controlled hit and miss
rolls require identical final armour to the laser-only control, and respectively
two versus zero impact sounds. Execution, damage and effects bodies are real;
RNG and presentation/audio calls are isolated test boundaries. This is not a
recorded original-game audio or pixel comparison.

If the bypass is the first attack, BP-56 may instead be unread native stack
residue; the C diagnostic guard still applies when effects would observe it.
That separate entry-state problem is not solved by the assigned-value tests.
Preserve the carried result; do not initialize it for each weapon or invent
personnel damage against Mechs. A later C# redesign can choose independent
effects deliberately. See [execution notes](../phase4/BTECH_1AE8_EXECUTION_C_CONVERSION.md).

## BUG-024 — Play-again restart retains fog revealed in the previous game

**Address:**0800:04A3..04D9 replay branch and0800:4DC7..50C7 new-game reset.
**Classification:** confirmed original state-reset bug; player observation and
complete raw-ASM review agree.

When Jason or Rex dies, accepting `Do you want to play again?` calls the same
new-game initializer used by an ordinary new game. That routine restores the
Citadel start coordinates and reconstructs the Citadel map, but it never clears
the 2,048-byte visibility bitmap at3092:CB0C. It only ORs the Citadel's initial
reveal mask into eight rows. Areas revealed before death therefore remain
visible after an in-process restart. A fresh process starts with zero-filled
storage, and loading a save replaces the saved state block, explaining why
those control paths do not reproduce the retained fog.

Preserve this omission in the C compatibility build. A modern C# edition may
clear fog as an explicit bug fix, but must not change save loading: saved games
are supposed to retain their own explored areas. The outside-Citadel view is
not a second fault—the initializer explicitly restores `0C45:C019`, the native
Citadel-map start position rather than placing Jason inside the HQ building.

## Candidate template

New entries should include:

```text
ID and short title
address/routine
classification and confidence
observed instruction-level behaviour
player-visible or safety impact
evidence link
recommended compatibility policy
```

Do not add an item merely because the annotated pseudo-C is malformed. First
confirm that the behavior exists in the original assembly or executable.
