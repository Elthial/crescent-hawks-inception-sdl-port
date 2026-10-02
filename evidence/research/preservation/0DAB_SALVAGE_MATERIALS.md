# Sol: salvage materials — first bounded cleanup

Follow-up: [remaining salvage sections](0DAB_SALVAGE_REMAINDER.md) now cleans
heat sinks, weapon names/pointers, scrap payout and whole-Mech salvage. Weapon
salvage still awaits its inherited native-frame BYTE binding; the original
verification scope below remains prefix-only.

Baseline `1b17e2d`. Change `Salvage_Armour_Dialog` in place, retaining0DAB:0002
and its public method name. This block covers technician selection, dialogue,
wreck armour pooling and armour/internal-structure transfers through0258.
Heat sinks, weapons, scrap payout, whole-Mech salvage and soldier loot are NOT
completed by this block. Do not promote the whole-method mismatch certificate.

## Clean names and ASM corrections

- Native Tech BYTE and technician-name BYTE are signed, including80h–FFh.
  `highestTechSkill` starts Unskilled; strict improvement retains the first tie.
  Default `partyTechnicianId` remains Jason/slot0 even if no positive skill wins.
- `wreckCount` replaces NumberOfWrecks_1056 and its remaining uses, without
  changing the later payout logic. Other prefix locals describe wreck combatant
  IDs, enemy Mech records, the Lance Mech, armour/structure points and transfers.
- Add `LanceSize`4 and use it for friendly Mech traversal; enemy record count,
  party-character count, armour11 and internal-structure8 use their own semantic
  constants. Four-C-bill prices elsewhere do NOT become LanceSize.
- Enemy record4..7 comes from Enemy_Mech_Record_First plus the enemy slot;
  wreck flags use the distinct combatant-ID range12..15. Do not conflate them.
- The selected enemy/friendly Mech temporary pointers retain FAR3092. Later
  critical-pointer discrepancies remain queued rather than silently certified.
- Missing armour/internal structure is signed WORD BYTE maximum minus current.
  Cap it using native signed comparison against the WORD pool; narrow pool
  subtraction to uint16_t and add the transfer's low BYTE to current state.
  Over-maximum state is intentionally not clamped to a zero deficit.

Re-read the complete native0002–0258 blocks, all reads/writes, branch conditions,
calls and loops. Armour location priority descends10..0, then friendly Lance
slots ascend0..3 at each location. Structure descends7..0 in the same slot order
and requires Excellent Tech skill4. Armour transfer is not gated on Tech skill.
Only wreck WORD flags decide enemy eligibility, not the enemy Name field.
Source wreck bytes remain unchanged; this method pools their surviving material.
Skill-description lookup has only a lower threshold, not a native upper bound;
ordinary host tables still require known valid name/description indexes.

The signed native deficit matters: maximum5/current10 transfers-5 even with a
zero pool. That reduces current to5 and adds5 to the pool for later locations or
Mechs. Preserve this behaviour. Initial armour pool is at most11220; adding all
possible negative Lance deficits reaches at most22440. Structure reaches at most
16320. Thus these valid BYTE-record pools stay inside signed WORD range, but the
explicit signed comparison documents the native operation.

## Preservation issue: uninitialised weapon salvage

The later native weapon block zeroes buckets10h..1Fh but uses20h/SRM-6 too.
That final BYTE is inherited native stack contents at BP-14h. A host C local
left uninitialised does NOT faithfully preserve this: its value depends on a
different compiler stack, and an indeterminate read cannot be certified as a
defined C simulation. Setting it to zero, assigning a random BYTE or inventing
a persistent global would change original behaviour too.

TODO preservation design before compiling/certifying weapon salvage: represent
the native inherited frame BYTE as explicit state supplied by a preservation
stack/frame view, with documented provenance or emulator captures. The emulator
value can validate a captured execution, not prove a universal initial value.
The current original-defect annotation remains; no guessed replacement is added.
Heat-sink pool underflow and member-slot-based scrap payout likewise remain
preserved for their subsequent bounded passes.

## Verification scope

`Verify-PreservationSalvageMaterials.ps1` mechanically extracts ONLY the actual
0002–0258 prefix, then closes the body for an isolated test compilation. It does
not compile the complete method, later uninitialised bucket or game. Constants
and Mech structure are generated from the actual shared header; the reused UI
adapter models eight Mech records, eight party records and24 wreck flags.

MSVC C17 `/W4 /WX`: 2,884,316 checks pass, including instrumentation, not that
many gameplay scenarios. Tests cover valid/signed-negative Tech BYTEs, dead
members and first ties, wreck flags and enemy-record mapping, dead friendly
Mechs, descending location/Lance ordering, Excellent structure gate, source
immutability and negative-deficit pool increase. Exhaustively check all65,536
maximum/current BYTE pairs for each material with pools0,1,3,255. Positive Tech
values5..127 are deliberately not fed into a three-entry host table; no new
validation is inserted in the game code.

The whole-method audit record remains Checked-mismatch. Platform/frame mechanics
and shared callee contracts remain boundaries; no gameplay validation is claimed.
Next bounded block: heat-sink salvage, then the explicit native-stack weapon issue
and scrap payout; whole-Mech salvage follows.
