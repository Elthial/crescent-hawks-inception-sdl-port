# Candidate sections for Astra confirmation

Sol: 2026-09-18 A-003 evidence update: original EXE startup explicitly installs
SS in the C data segment and executes PUSH SS/POP DS before C initialization.
The relocation maps both to3858 in the verified exploration run. This explains
DS-indexed local arrays in0800's mech sort and1467 crew writeback; do not label
them frame-pointer errors or bugs merely because a terminal hardware snapshot
has DS!=SS. [Byte-confirmed startup evidence](ORIGINAL_DATA_STACK_ALIAS.md).
Normal-call DS preservation and call-site captures remain workflow checks.
Sol: 2026-09-18 subsequent conversions now include full0800:0E4B compositor
and1467:0002 crew assignment with ordinary local arrays under that startup
contract. Crew tests cover13 workflows; compositor has SDL pixel checks.
Neither replaces the pending call-site emulator capture or certifies corrupted
DS state. A-003 is not a reason to keep these original C workflows absent.

This is the queue for code blocks that may justify a focused pass with the more
expensive Astra model at Medium reasoning. Listing an item does not authorize or
start that pass. Each request should be narrow, include the relevant original
assembly as well as the annotated decompile, and ask Astra to return evidence and
uncertainties rather than a wholesale rewrite.

## Selection criteria

Add a block here when at least one of these is true:

- several reasonable control-flow reconstructions fit the decompiler output;
- segmented-pointer aliasing makes the owning structure unclear;
- byte-versus-word access changes the meaning or boundaries of a structure;
- Reko's 32-bit fallback makes a near pointer, far pointer, word, or dword
  interpretation materially ambiguous;
- computed jumps, indirect calls, or overlapping code/data resist local tracing;
- the result controls a large part of the future C# architecture;
- two independent local analyses remain in material disagreement.

Do not use Astra merely for naming, mechanical cleanup, straightforward table
extraction, or work that an original-file fixture can settle conclusively.

## Initial candidates

### A-001 — BLD interpreter and control-flow recovery

- **Scope:** the loader/interpreter cluster around `0FDC`, its computed jumps,
  structural text bytes, and dispatch into `1CD3`.
- **Why it may qualify:** mixed bytecode and encoded text, absolute jump targets,
  conditional skips, and multiple dispatcher layers create several opportunities
  for a superficially plausible but wrong high-level reconstruction.
- **Prerequisite:** complete the local opcode/address cross-reference in
  `BLD_EXECUTABLE_TARGETS.md` and preserve raw BLD offsets in the disassembly.
- **Desired confirmation:** instruction-pointer semantics, operand widths,
  bytecode start, exit conditions, and call/dispatch boundaries.
- **Status:** narrowed candidate. Local assembly analysis has now verified the
  length-prefixed payload, file/runtime origins, transformation order, all 28
  opcode widths, absolute branch targets, exit condition, and all 36 local F3/F9
  table extents. Reserve Astra for whole-script control-flow validation,
  executable target boundaries that remain ambiguous after assembly tracing,
  and uncertain gameplay semantics rather than rechecking settled mechanics.

### A-002 — Combat coordinator/state machine

- **Scope:** the central combat workflow spanning the `183B`, `1AE8`, and related
  combat functions, including per-combatant arrays and stage dispatch.
- **Why it may qualify:** large stateful functions, many global aliases, indirect
  dispatch, and consequences for the portable runtime architecture.
- **Prerequisite:** identify callers/callees and produce a side-effect table for
  one bounded combat block.
- **Desired confirmation:** phase boundaries, loop ownership, combatant indexing,
  and which state belongs to infantry, mechs, or the encounter as a whole.
- **Status:** candidate; defer until Phase 4 combat work.

Sol: Fresh systematic comparison2026-09-17 covers every local operation in183B
and1AE8. The1AE8 audit still finds unreconstructed pointer/dialogue/restore code
and one-based mech-component constants used as zero-based table indices. These
have direct ASM witnesses and do not need an expensive-model confirmation.
Keep A-002 for runtime round/state-lifecycle comparison after correcting the C;
adapter1's unassigned missile redraw phase remains probable BUG-015, not a
reason to initialize a host local silently. No Astra run was invoked. See
[systematic1AE8 report](../phase4/BTECH_1AE8_SYSTEMATIC_ASM_AUDIT.md).

Sol:183B:28DB local audit resolves the parent encounter gate as a30-step
probe toward the first active enemy from Jason's position. Remaining deeper
confirmation, if warranted: Position0006's actor80h collision/range policy,
whether already-at-target false is reachable, and whether first-enemy-only
assessment can miss other reachable enemies. Do not spend on Astra for the
resolved WORD locals/call order. Evidence:
[final183B helper review](../phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md).

Sol: follow-up0006 reconstruction now confirms80h skips occupancy but retains
mech footprint/tile blocking. No Astra confirmation needed for that resolved
branch. Pending deep dependency is16AB occupancy/crushing and1BFE turning-path
collision; defer model spend until their local ASM/caller reconstruction.

### A-003 — Segmented-memory structure and alias reconstruction

- **Scope:** pointer tables around `3EDB:5384..5722`, segment pointers near
  `DS:54xx`, and aliases into the large `3092` state/save region.
- **Why it may qualify:** Reko C types can obscure 16-bit near offsets versus
  four-byte far pointers—especially because the decompiler used a 32-bit model—
  and several symbolic names may describe overlapping views of the same storage.
- **Prerequisite:** build an address-use inventory from the original assembly.
- **Desired confirmation:** pointer width, base segment, alias groups, and which
  proposed C structures are real versus documentation conveniences.
- **Status:** likely useful after the initial memory-map cleanup.
- **Local evidence added:** crew assignment at `1467:0838` reads two local
  WORD row maps through BX-based default-DS addresses after adding BP. Most
  accesses to those maps use BP-based SS addresses. Confirm startup stack/data
  segment aliasing before carrying this literal pointer convention into the
  C# architecture; the clean `.dis` also reports DS for the final reads.

### A-004 — Mech/weapon linkage during combat

- **Scope:** the link between critical component IDs, ten mech ammo bytes, the
  17-byte weapon table, volley size, range, and destroyed/infinite-ammo flags.
- **Why it may qualify:** current sources disagree on weapon field offsets and
  bit meanings; a wrong choice would contaminate both loaders and combat logic.
- **Prerequisite:** locate every indexed weapon-table read and capture the exact
  byte/word instructions.
- **Desired confirmation:** weapon record layout, flag masking, ammo-bin mapping,
  and per-missile versus per-volley damage semantics.
- **Status:** candidate after local table extraction; may be resolved without
  Astra if original-byte traces are decisive.
- **Local evidence added:** the complete Mech-Lube package dispatcher at
  `11B8:09D3-0D30` is now traced. In particular, Commando stage two writes
  component `10h` to all six raw critical bytes `+4F..+54`, component `11h` to
  `+55`, and marks all ten ammo-state pairs infinite. The older design note says
  "two small lasers," while the game's weapon collector appears to treat each
  weapon-valued critical byte as an entry. Confirming whether these are six
  usable small lasers, repeated component occupancy, or evidence of a wider
  critical-to-weapon mapping remains suitable for this focused review.
- **Additional local evidence:** `Mechlube_Buy_Ammo` at `11B8:1762` scans raw
  critical bytes `+33h..+55h`, appends at most ten weapon-valued bytes in scan
  order, and uses each resulting ordinal directly for `CurrentAmmo[ordinal]`
  and `MaxAmmo[ordinal]`. This settles the shop routine's local indexing but
  reinforces the need to explain repeated component bytes before treating each
  collected entry as a distinct combat weapon.
- **Combat-menu evidence:** `1543:0041..008E` accepts up to twelve weapon-
  valued critical bytes, whereas `11B8:1762` stops at ten. The combat UI then
  indexes raw current-ammo address `record+27h+slot`, so slots ten and eleven
  alias walking/jumping movement. Its twelve-byte local weapon list also loses
  NUL termination if completely filled (BUG-012). Distinguish the twelve action
  targets, including kick slot eleven, from the ten actual ammo fields before
  settling weapon-instance/critical-occupancy semantics.

Sol: Additional A-004 evidence from the [1631 audit](../phase4/BTECH_1631_SEGMENT_REVIEW.md):
computer movement range at `1631:064A` indexes weapon records by raw critical
code, while `10A2` returns raw-code-minus-one. The movement threshold masks E0
then shifts by three, not five. This may be deliberate movement policy plus
an offset table interpretation; confirm before normalizing the two paths.

Sol: A-004 local resolution checkpoint in the
[combat linkage review](../phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md):
1AE8:0059..00E9 passes `(normalizedMechRecord, weaponOrdinal)` to10A2;
0239..0296 consumes the same ordinal's current-ammo byte, except FF unlimited.
Duplicate critical codes are not grouped; destroyed codes retain ordinal and
disable firing. Ordinary critical/ammo linkage and the BYTE cluster-table load
are locally resolved. Commando's repeated10 codes expose separate firing
entries, not a hidden two-weapon grouping. Remaining Astra-worthy scope is
the AI raw-code range indexing/scaling and intended upgrade policy, not basic
ammo lookup. No expensive-model run was started.

Sol: remaining1631 completion locally resolves the AI/heat flow and caller
contracts. Raw component indexing and fourfold AI range scaling are retained
exactly; A-004's remaining question is intended movement policy, not how to
transcribe those instructions. Heat BYTE arithmetic/inferno, BUG-013 clamp
omission and sprite-wrapper FAR word loads did not need an Astra run. Shared
renderer/table extent needs loader/ASM tracing first, not costly speculation.

### A-005 — Critical-slot to internal-structure anatomical mapping

- **Scope:** helper `183B:273D`, mech critical groups at record `+0x33..+0x55`,
  and current/max structure bytes at `+0x1C..+0x23` and `+0x61..+0x68`.
- **Why it may qualify:** the helper's threshold sequence proves the byte-to-byte
  mapping, but it conflicts with the current left/right and leg/torso labels in
  `BTECH.h` and `MECH_RECORD.md`. Byte `+0x7B == 0xC8` also bypasses the normal
  supporting-structure read.
- **Prerequisite:** cross-check reference mech critical layouts and every call
  to `183B:273D` against the original assembly.
- **Desired confirmation:** anatomical labels for all eight structure bytes and
  eight critical groups, plus the meaning of the `+0x7B == 0xC8` special case.
- **Local evidence added:** `Mechlube_Modify_Mech` uses values `0..3` as the
  four supported chassis upgrade-package bases and maps every larger value to
  unsupported selector `8`. This explains the field locally but does not
  explain why structure-support code treats `0xC8` specially.
- **Status:** candidate; does not block the verified heat-sink salvage control
  flow at `0DAB:0259-031E`.

Sol: Additional A-005 evidence: `1631:11AB` dispatches structure offsets
1C/21 to high actuator nibbles and critical blocks33/41; 1D/22 to3A/48;
1E/23 to low actuator nibbles and4F/51; 1F to head systems78/77/55;
20 to engine/gyro75/76 and block53. `1122` also exposes raw damage-transfer
offsets. See the linked 1631 audit; reconcile these with `183B:273D` and
templates before replacing anatomical names. No Astra execution is required
or authorized by this note.

### A-006 — Post-battle infantry weapon-presence address

- **Scope:** `Loot_Enemy_Soldiers_Dialog`, especially the byte read at
  `0DAB:0995-09AD` from `3092:C6EB + combatantId * 0x11` for IDs `4..11`.
- **Why it may qualify:** the later weapon-consumption loop reads the weapon byte
  at `3092:C6A7 + lootSlot * 0x11` for slots `0..7`. Both instructions and
  constants are clear, but the two address/index schemes do not presently map
  to the same character records and the first crosses the documented mech-table
  region. This may expose an overlapping combat union, a bad record-base model,
  or an original indexing defect.
- **Prerequisite:** inventory all reads/writes of `C6A7`, `C6EB`, and the word
  flags at `395C..396A`, then compare a live post-combat memory capture.
- **Desired confirmation:** which records the first byte test addresses and
  whether it reliably means that at least one fallen infantry weapon exists.
- **Status:** candidate; exact control flow is documented using the raw address,
  without assigning an unproven structure member.

### A-007 — Special held-NPC conversation condition

- **Scope:** `Talk_To_Building_Occupants` at `0FDC:18C6-18F2`, together with
  roaming-NPC route state at `3092:D390` and flags `D339/D33A`.
- **Why it may qualify:** the instructions test the waypoint of the *last*
  waiting NPC scanned, while separately requiring compact menu choice zero and
  NPC slot zero to be present. A more natural source condition would use the
  current building or selected NPC. The path then sets `D33A`, which
  `LOUNGE.BLD` reads indirectly as persistent-state index `2E`, and clears Rick
  Atlas's slot-zero movement delay.
- **Prerequisite:** capture the eight route records and flags immediately before
  this branch in a live game where `D339` is active.
- **Desired confirmation:** whether the last-scanned waypoint dependency is an
  intentional Rick/lounge invariant, a source-level variable error, or evidence
  that another overlapping interpretation of the route records remains missing.
- **Status:** candidate. Exact executable behavior is retained and documented;
  it does not block the surrounding occupant-menu reconstruction.

### A-008 — Procedural rectangle edge register asymmetry

- **Scope:** `207F:0DF8..0FED`, particularly left perturbation `0EFB..0F34`,
  bottom/right means `0F34`/`0F8B`, right amplitude `0FC1`.
- **Why it may qualify:** two interacting instruction-verified asymmetries:
  left noise remains in DH and affects following WORD means; right vertical
  amplitude derives from AL's horizontal half-span rather than0278h's vertical
  half-span. They look like original register-state errors, but intent/visible
  terrain impact needs more than a tidied decompile.
- **Prerequisite:** original emulator trace with saved four corners, GameSeeds,
  seed index, lattice and AL/DX at each midpoint; compare literal generator output
  against a separately labelled symmetric alternative without changing source.
- **Desired confirmation:** audit AL/DH liveness from0E69 through0FE7, verify
  literal output against the trace, and assess whether any surrounding invariant
  makes the apparent asymmetry deliberate or harmless.
- **Status:** optional expensive Astra Medium review candidate, not invoked.
  Does not block the literal transcription. See BUG-020 and the207F construction
  notes; never silently replace the preserved formula with the symmetric one.

### A-009 — 1F3D oversized allocation result remains uninitialised

- **Scope:** `1F3D:05BC–0639`, particularly positive signed high-WORD branch
  `05DA–060B`, and callers `1E56:0AE5` / `1F3D:070A`.
- **Evidence:** Oversize branch prints FAR3EDB:4FDA and waits for input, then
  tests BP-6/BP-4 without storing an allocation result. Normal branch calls3835
  with the low WORD and stores DX:AX. Negative high WORDs take that normal
  branch. The literal research transcription retains these facts.
- **Desired confirmation:** emulator stack/register trace across the error
  message/input path; check whether a nonlocal exit or call convention makes
  its apparent fallthrough unreachable, and establish which callers can reach
  it. Do not initialise the result or substitute a safe host allocator silently.
- **Status:** optional expensive Astra Medium candidate, not invoked; not yet
  a confirmed original-game defect. See the1F3D segment review.

## Sol: preservation implementation follow-up — native residual state, 2026-09-18

No Astra run is authorized or started. A focused extension of A-002/A-003 is
needed before claiming complete combat preservation in ordinary host C:

- `1543:0004`: weapon BYTE list occupies SS:BP-20..BP-15. All twelve can
  be nonzero; native `207F:3B9E` then scans BP-14 and subsequent stack bytes.
  BP-14 is not assigned before the initial length calls at0151. Later it is
  written as a saved text-row WORD. Replacing strlen with collector count,
  adding a terminator, or using host uninitialized storage would not preserve
  BUG-012. Capture entry/scan stack bytes for a full list and a repeated menu.
- `1AE8:000C`: high-bit attack encoding against a Mech can reach effects
  without assigning SS:BP-56 attack-applied WORD. `1AE8:12C7` also reads its
  own BP-4 visible-projectile counter before initialization. Establish the
  observable consumers and entry stack state; do not guess zero or rely on
  the modern compiler's unrelated stack contents.
- `183B:0DF3..0E4D`: BUG-016 CBWs the first enemy-personnel target and reads
  WORDs at3092:(4004+signedTarget*2) and(4036+signedTarget*2), without masking.
  Full parent ASM000A..1481 re-read in this implementation pass confirms the
  path. Negative indices alias the tail of the native FAR sprite table and
  adjacent arena/name state. Host pointer-table bytes are NOT interchangeable
  with original16:16 pointer words. Capture actual signed target, effective
  addresses and WORD contents when the personnel-flight branch is reached.

Ask for an evidence-backed representation of these original native reads,
including minimum memory/stack extent and caller/callee interactions, not a
rewrite, sanitizer or fixed-rule replacement. The next safe implementation
step is an explicit native-storage contract informed by these instructions
and targeted capture, preserving observable reads without host C undefined
behaviour. A model opinion alone cannot supply unknown live stack bytes or
relocated heap-pointer values. Independent original methods can continue
conversion while these contracts are established.

## Completed reviews

Sol:2026-09-18 new local investigation before any model spend: terminal capture
`run-20260918-165639-8b2876d7` changes the1631:0B1E operand from55A2 to0000.
Three earlier dumps retain55A2. The correspondence verifier rejects the dump;
do not whitelist the mutation or transplant it into C. First capture the native
write and writer/call context. Only if that evidence leaves ambiguous control
flow or aliasing should a focused Astra Medium confirmation be considered.
No model review is authorized or invoked. Details:
[post-exit operand investigation](../phase4/NATIVE_MENU_INPUT_COMPARISON.md).

Sol: follow-up resolves that writer locally: a new-game Quit-only capture
reproduces the operand overwrite at emulator root termination. The pinned
`DosProcessManager.TerminateProcess` writes the return IP through parent
SS0060:SPFFFE, physical67070, which aliases the child operand. This is not
an unresolved original-game branch and needs no Astra confirmation. Keep
terminal correspondence rejection; no game-code correction or whitelist.

None yet.

## Sol: 2026-09-17 local-evidence checkpoint

This is queue maintenance, not an Astra review or gameplay confirmation.
The earlier prerequisite/status paragraphs are historical; use this checkpoint
when selecting new work. No expensive-model run was started.

- **A-001:** widths/origins and local dispatch remain instruction-confirmed;
  whole-script state transitions still need live BLD execution traces.
- **A-002:** the `183B`, `1AE8` and `1631` first-pass audits now include `16AB`
  occupancy/crushing and `1BFE` turning/path collision. Their reconstruction is
  no longer a prerequisite to be done. Encounter reachability and complete
  round/state lifecycle still need runtime comparison.
- **A-003:** memory ownership and DS/SS aliasing remain open. Imported Spice86
  dumps must not settle this: the configured executable hash differs from both
  local EXEs and the loader/runtime provenance is not established.
- **A-004:** duplicate critical entries and ordinal ammo linkage are locally
  resolved; the remaining issue is AI range policy and upgrade intent, not
  whether repeated Commando critical codes expose separate firing entries.
- **A-005:** anatomical names and selector `+7B == C8` remain open; retain raw
  offsets until templates, dispatch and runtime damage agree.
- **A-006/A-007:** require the specified live memory captures; static plausibility
  or another model's agreement cannot establish the observed gameplay effect.
- **A-008:** literal register asymmetries are instruction-confirmed; generator
  output and practical terrain consequences require a seeded emulator trace.
- **A-009:** apparent uninitialised fallthrough is instruction-confirmed;
  reachability and nonlocal-exit behaviour still need a safe error-path trace.

The startup capture now confirms the sprite table has 376 FAR entries, and
all eight `0800` readers have been reconciled with that table. Those widths,
strides and transient header edits are no longer candidates for model spend.
Rendering correctness still needs live comparison. See
[sprite-reader evidence](../phase4/BTECH_0800_SPRITE_POINTER_READERS.md) and
[runtime validation protocol](RUNTIME_VALIDATION.md).

Sol: The user's first exploration dump now establishes a live relocated image
mapping and confirms code/table identity against the expanded EXE. DS/SS differ
at the terminal retrace snapshot; this does not settle A-003's crew-assignment
effective addresses. Music setup/idle ran, but actual PC/Tandy note handlers
were not observed. See [dump evidence](SPICE86_EXPLORATION_CODE_CONFIRMATION.md).

When a review is completed, record the date, exact model/reasoning level, input
scope, conclusion, remaining uncertainty, and the local evidence that accepts or
rejects the conclusion. Do not promote a result to Verified solely because Astra
agrees with it.
