# Sol: tracing the inherited SRM-6 salvage counter

Status: static stack-address match, not a verified runtime initializer.
The preservation C guard remains. No gameplay value is supplied by this note.

## Confirmed normal-input overlap

The untouched expanded ASM shows only one caller of `0DAB:0002`:
`183B:1169`, inside the combat parent. Let `S` be that parent's SP after its
`44h` local allocation and saved DI/SI, outside temporary call arguments.
The salvage FAR call pushes four return bytes; its prologue pushes BP.
Consequently its BP is `S-06h`, and the unassigned SRM-6 BYTE at BP-14h
occupies `S-1Ah`.

When the casualty message at `183B:10CD` is taken, the parent calls
`0800:2A4F` to acknowledge it. On the normal, non-demo keyboard branch:

| Operation | SP relative to S |
| --- | --- |
| FAR call to `0800:2A4F` | -04h |
| FAR call from that prompt to `1F3D:0259` | -08h |
| keyboard bridge pushes BP | -0Ah |
| keyboard bridge allocates four local bytes | -0Eh |
| FAR call to `207F:0B8A` | -12h |
| BIOS keyboard wrapper pushes BP | -14h |
| wrapper pushes DI | -16h |
| wrapper pushes SI | -18h |
| wrapper pushes DS | -1Ah |

The final push therefore writes a WORD at the future SRM-6 counter address.
Its low BYTE is the candidate inherited quantity. `207F:0B8A` saves DS
before loading its own `246Ch` data segment, so the candidate is the incoming
DS byte, not `6Ch` and not the key's ASCII/scan code. `1F3D:0318` restores
SP/BP without clearing those released stack bytes.

This is a concrete writer candidate rather than arbitrary uninitialised host
memory. It does not mean that DS should become a C salvage variable, or that
the analysis segment labels supply the original runtime value.

## Why this does not yet remove the guard

- The casualty-message branch is statically guaranteed on this caller's
  component-salvage path; see the subset proof below. Its acknowledgement
  still needs a native trace to establish which input branch ran and which
  writer last touched the released stack byte.
- Demo playback follows another bridge path, including retrace/pending-input
  calls. It is not certified by the normal-keyboard footprint above.
- Before salvage allocates its frame, released stack bytes may be overwritten
  by timer/BIOS interrupt frames. Matching one preceding writer is not proof
  that it is the final writer.
- The saved DS is relocated runtime state. A byte from one capture must not
  become a universal literal, zero start or supposedly original initializer.

## Caller gate and table relationship confirmed — 2026-09-18

Sol: untouched `BTECH_183B.asm` gives a direct subset proof. At10A8 the
victory-message scan tests WORDs `393C + 2*combatantId` for IDs12..23. Any
nonzero entry causes the10CD message and10DA call to `0800:2A4F`. At111B
the component-salvage gate tests exactly the same base and WORD stride for
IDs12..15; it cannot pass unless the earlier wider scan passed. This holds
for every route to the sole1169 component-salvage call, not just SRM-6 cases.
The intervening10DF..1169 instructions contain no further game calls before
salvage. Interrupt execution can still overwrite released stack storage.

`3954h = 393Ch + 2*12`: it is the enemy-combatant view of the same table,
not separate flags. In `0DAB:0002`, the weapon-pool scan uses `393C` with
enemy combatant IDs12..15. Earlier component scans use `3954` with enemy
Mech indices0..3; both address the same four WORDs. The C pointer macro
`DeadInfantryFlags` is a legacy misleading name for this pre-biased view;
its index interpretation depends on the caller. This proof does not assign
the inherited SRM-6 counter or remove its preservation guard.

## Next native capture

Use a post-combat encounter with a Good or Excellent technician and an intact
enemy or destroyed friendly SRM-6 slot. Break at relocated `0DAB:0002` before
the prologue, recording SS, SP, DS and the caller. The prospective bucket is
`SS:(SP-16h)` with WORD offset wrapping: the entry SP is S-04h and the
prologue will subtract two more bytes before the BP-14h access.

Watch writes to that physical byte through the preceding acknowledgement and
salvage entry. Confirm whether `207F:0B8A`'s PUSH DS is the final writer, then
watch `0DAB:0377` increments, the `03E0` test and the subsequent decrement.
Record the entry byte and final friendly critical slots, including a case
with no matching enemy SRM-6 and one with enough components to wrap the BYTE.
The native counter increments/decrements modulo256; do not clamp it.

Any eventual C storage binding must retain the bug and explain the native
provenance. This investigation does not require implementing BIOS services
in the C game; it requires determining their observable gameplay residue.

## Entry reader

The existing reader defaults to statistics; its new salvage selection checks
the entry CS:IP, original prologue bytes, sole caller's return address and
16-bit stack offsets before reporting the counter BYTE and incoming DS BYTE:

```powershell
./scripts/Read-SpiceStatsEntry.ps1 -Entry ComponentSalvage -McpPort 48181
```

Run only after reaching the entry breakpoint. This leaves execution paused,
does not write game memory or initialize the counter, and checks that CPU
registers/cycles did not change during the read. An offline breakpoint dump
can instead be supplied with `-DumpDirectory`. A final shutdown dump is not
an entry witness and is deliberately rejected.

Sol:29 existing execution-flow dumps were searched for call edges to the
known load017D profile's live `0728:0002`; none recorded that target.
That search does not certify other load bases or provide a salvage witness.
