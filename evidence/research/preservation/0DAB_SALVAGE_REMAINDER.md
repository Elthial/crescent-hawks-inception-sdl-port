# Sol: heat sinks, weapons, scrap and whole-Mech salvage

Baseline `32fb6f0`. Continue the two annotated methods in `Btech/BTECH_0DAB.c`
in place; no replacement implementation or gameplay redesign.

## Component salvage: 0259–04F8

Names now describe enemy wrecks, lance Mechs, critical-slot offsets, component
IDs, recovered weapon buckets and scrap payout; numeric local suffixes are gone.
Enemy records use `Enemy_Mech_Record_First` and `Enemy_Mech_Record_Count`;
friendly records use `LanceSize`. `MechCriticalSlotCount` means the 35 stored
component bytes at record offsets +33h through +55h, inclusive. Read those bytes
through the complete Mech object's BYTE view, not beyond an anatomical subarray.
All temporary Mech/component pointers retain FAR3092.

Average Tech (2) restores destroyed heat-sink components when 183B:273D allows
their critical location. Native 02F5 writes the intact component and decrements
the WORD pool **without testing availability**. Zero wraps to FFFF; no guard was
added. Enemy heat sinks are counted only when intact and their Mech is flagged
as a wreck. Source wreck components are not removed.

Good Tech (3) collects intact weapon critical components 10h–20h and restores
matching destroyed lance components, consuming one BYTE bucket per repaired
critical slot, in lance/slot order. This is component-slot repair, not a weapon
inventory or an ammunition refill. The destroyed bit is removed only after a
nonzero matching bucket and the location helper both allow repair.

**Weapon preservation remains unfinished.** Native 032D zeroes only 16 bytes
at BP-24h..BP-15h, but SRM-6's bucket is BP-14h. It inherits stack contents.
The cleaned source deliberately retains an explicit TODO rather than choosing
zero, randomness or a new persistent global. Reading the current uninitialised
host local is not a faithful or defined C model. A native stack/frame binding
must supply the incoming BYTE before this whole method can be compiled and
certified. No additional Astra confirmation is needed to establish the defect;
its faithful state binding is the outstanding implementation decision.

Scrap payout is `(90 + (randomByte & 127))` C-bills per enemy wreck, multiplied
by the chosen technician's **party index**, not Tech skill. Index zero gets no
payout and consumes no payout RNG calls. Added semantic constants explain 90,
the 0..127 random bonus and catastrophic engine/gyro hit limits. The signed
WORD payout is sign-extended before the DWORD cash addition, matching CWD.
The exact text, green number, white-text controls, plural and pauses remain.

## Whole-Mech salvage: 04F9–094A

Correct native CBW on Tech/name BYTEs and signed assignment comparison against
`Character_OnFoot` (8). Assignment bytes 80h–FFh are negative and do not qualify.
Best-technician selection retains the first strict winner and defaults to Jason.
Mech and record-copy pointers are FAR3092; pilot/assignment writes narrow to BYTE.

The two RNG calls remain ordered: within-lance index first, friendly/enemy range
second. Scan tests the seed before advancing, wraps at combatant ID16, consumes
the selected flag even on rejection, and recounts flags0..15. Enemy combatants
12..15 map to Mech records4..7. Flags4..11 must not represent Mech candidates;
the caller also needs at least one candidate before acceptance. No new safety
guards were inserted into the original circular scan.

Engine exactly3 hits, gyro exactly2 hits, or zero structure at +1Fh requires
Excellent Tech (4). The +1Fh/+20h anatomical naming remains disputed under A-005;
do not invent a tabletop location name to hide that uncertainty.

Recovering copies bytes1..124, restores the saved first name character, assigns
the pilot, selects Locust/Commando graphics, forces engine/gyro damage to1, and
raises zero structure at +1Fh/+20h to1. A full lance still announces success and
consumes the wreck without installing it: preserved original behaviour.
State0 searches, state1 reports recovery, and native sentinel9 terminates both
candidate and party scans. The two exhaustion pauses remain.

Restored the inspection text to ` is inspecting the wrecks...\r` (3EDB:0FFF).
Removed the undeclared WORD scratch view: trimming accesses BYTE scratch memory
at DS:0012+index. Existing shared scratch/placeholder declarations are native
overlapping address views, not a finished portable host struct. The test adapter
models them with an overlapping BYTE-buffer union, including placeholders16/27.

## Verification and limits

`Verify-PreservationSalvageRemainder.ps1` compiles actual source sections using
MSVC C17 `/W4 /WX`. It explicitly omits the weapon block, then compiles the
complete whole-Mech method. Tests cover every heat-sink critical location,
skills0..4, helper acceptance/rejection, zero-pool repairs, all256 payout RNG
bytes, all8 technician slots, 0..4 wrecks, all256 pilot-assignment bytes, every
friendly/enemy Mech candidate, copy/trim, signed-negative Tech, catastrophic
damage and Excellent override, full-lance fallthrough, circular wrap, rejected
candidate retry, sprite family and minimum structure.

These are isolated section tests with UI/helper adapters, not emulator or
gameplay validation. Weapon salvage is not exercised or certified. The audit
records remain conservatively `Checked - mismatch` until inherited-frame and
shared native-memory bindings are completed; corrected local discrepancies and
section coverage are recorded explicitly rather than promoting confidence.

Checkpoint: 71,268 remainder checks, 2,884,316 material-prefix checks, 850,042
repair checks and 789,469 ammunition checks pass. Restored inspection text is
also byte-checked against the private original EXE, without copying assets.
