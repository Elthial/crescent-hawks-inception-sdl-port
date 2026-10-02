# Sol: full combat parent conversion work

The preservation Combat_Mechanics remains pending; no substitute round or
movement-only gameplay function is provided. The original annotated parent
is being corrected before its direct C translation.

Expanded ASM1AE8:1037..1285 rechecked2026-09-18. Template corrections:

- CBW-sign-extend the global movement cursor BYTE before wrapped WORD indexing.
- Load/store the complete named16:16 walk-animation pointer, not BYTE offset
  and address-of-segment-cell Reko expressions.
- Store1732's returned AL as frame; do not reuse old facing/offset values.
- Bind3920 to the known combatant facing BYTE table.
- Correct malformed redraw-call punctuation.
- Retain signed BYTE Piloting and CBW extension for the redraw view anchor.
- Recover11-BYTE direction table3EDB:2ECC..2ED6 directly from the expanded EXE.
  Native2ED1 is a biased base, not a pointer global. Index deltaY*4+deltaX+5
  uses multiplication, avoiding undefined C negative signed left shifts.

The exact direction table is included as EXE-owned C data and the existing
packed-position test checks all nine valid step combinations. No external
asset is embedded or exported. These changes do not claim the uncompiled
annotated parent passes C tests; only the existing C code/data suite is built.

The reported attack-applied residue remains real: high-bit weapon-type paths
can reach the effects handoff without assigning BP-56; effects use that flag
when graphics are enabled. It must not be silently initialized as a false
hit or reproduced with undefined C reads. Statistics palette starting phase,
allocator oversized-request return and weapon/salvage residual locals also
need explicit original-state contracts before full preservation certification.

Next: reconstruct target/proficiency/dialogue aliases, then translate the full
round parent and effects handoff with original bugs retained. The parent must
eventually be tested with real movement, target, hit/damage and NPC mechanics;
isolated controller tests are not a headless combat sequence.

## Proficiency01D0..0238 —2026-09-18

Rechecked the whole raw ASM block. Corrected annotated skill-type field,
last-category signed CBW comparison and actual FAR skill-address construction.
Counters wrap at256 same-category uses; only then does a signed skill BYTE
below excellent4 increment. A changed category resets the counter and stores
that category. Categories128..255 cannot equal their signed stored BYTE under
the native zero-extended versus CBW comparison; no normalization was invented.

Native D358/D360 actor-indexed bases are biased: friendly infantry actor4..11
addresses countersD35C..D363 and categoriesD364..D36B. These do NOT touch
OuttakeFrequencyD35B for valid friendly actor IDs. Added named C views over
the existing saved PersistentState bytes80..95, not new standalone tables.
The existing saved-storage test verifies the full alias bytes on every seed.

This is still template/shared-state preparation. No additional research helper
or replacement proficiency algorithm enters production. The full original
parent and its observable residual attack-applied flag remain pending.

## Attack narration0445..071B —2026-09-18

Reconstructed the complete attacker/target tree directly against expanded ASM.
Removed WORD-valued strings, addresses of FAR pointer cells, invalid character
record indexing and the missing0712 jump. Character name IDs are signed BYTEs;
Mech pilot IDs are unsigned BYTEs. Weapon text addresses the record's Name.
Recovered exact spaces, brief Kick (not " on "), possessive "'s Mech", one
terminal period and "a spectator.". The historical copy helper is strcpy,
not concatenation; brief mode overwrites kicks with Kick.

Friendly target scratch construction still occurs when messages are disabled.
Weapon-name/kick filter calls still occur without an explicit verbose gate.
The two arena mode gates independently reread shared state after output calls.
No helper algorithm or replacement combat method was added. This annotated
block is not compiled or validated by the existing executable C test suite.

## Hit/damage handoff and native tables —2026-09-18

Rechecked0758..0ED5, including missed shots, damage transfer, effects handoff,
special death scenes and anchor restoration. Corrected signed BYTE heat cutoff,
WORD death/impact flags and target-record local, mech-miss WORD Y=-1, actual
target alias in arm-loss scene gating, signed Jason Piloting comparison, and
saved view-anchor arguments instead of unbound Reko ax residues.
The personnel and Mech hit messages are EXE bytes0D060D followed respectively
by "Hit!" and "Hit "; literal [CR] markers were wrong executable text.

Added exact EXE-owned facing-category15, movement-penalty13, hit-location44
and missile-cluster77 BYTE arrays in src/Original/BTECH_1AE8_DATA.c. Recovered
from private expanded EXE DS3EDB:2D0A/2D1A/2E42/2E6E. Named counts/biases
describe four location categories, eleven2D6 outcomes, seven missile columns
and twelve interleaved slices. No modern rules table replaces native values.
New C test checks complete transcription fingerprints, all normal facing/
location/cluster indices, both kick leg offsets and movement-penalty endpoints.
It validates C data, not the uncompiled parent or an actual combat sequence.

## Attack/effects bytecode and pointers —2026-09-18

Added src/Original/BTECH_1AE8_ANIMATION_DATA.c with original mutable EXE-owned
DS3EDB:3EF0..3FF7 attack bytecode (264 bytes), target impact2E3C..2E41,
projectile impact41D8..41DB, six signed muzzle tables and four signed projectile
step tables. Original FAR selectors36DB relocate to analysis3EDB, not2FE8.
Locust/Commando fire and kick tables and missile table each contain8 pointers;
personnel table3FF8 contains15 weapon rows*8 directions =120 pointers.
41D8 immediately following the personnel table begins impact BYTEs, not another
weapon's pointer row. All160 recovered table pointers share the same mutable
attack byte window; duplicate direction pointers intentionally alias.

Extended existing tests, rather than inventing a replacement effects algorithm.
Combat-table tests fingerprint complete BYTE window and all native pointer words.
Walking tests run the real0800:1732 interpreter over all152 finite attack
streams, missile six-frame loops, target four-frame loop, three impact frames,
and mutation observed through shared direction aliases. FF is not called:
the original interpreter waits forever on it and the effects parent peeks first.
No production timeout or alternative interpreter was added.

These tests establish original data/interpreter integration, not the full
Combat_AudioVisual_Effects parent, screen rendering, sound synchronisation or
combat gameplay. Complete effects parent conversion remains required.
