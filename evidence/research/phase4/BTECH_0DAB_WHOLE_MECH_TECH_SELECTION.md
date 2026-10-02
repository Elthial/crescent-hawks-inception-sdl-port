# `BTECH_0DAB` whole-mech salvage technician selection

## Reviewed block

- Address range: `0DAB:04F9-054B`.
- Parent routine: `Salvage_Mechs_Dialog`.
- The next linear block at `054C` is the random wreck-candidate engine, although
  initial control first jumps forward to the pilot scan at `0850`.

This routine begins by selecting the party's best living technician for the
subsequent attempt to recover a complete enemy mech.

## Record scan

All eight player character records are visited at:

```text
3092:C614 + partySlot * 0x11
```

A record is skipped when its byte `+0x00` (`Name`) is `0xFF`. For every other
record, byte `+0x09` (`Skill_Tech`) is compared with the best value seen so far.
Both the best skill and its party slot are stored in 16-bit stack locals; only
the source record fields are bytes.

The comparison is strict. A later equal-skilled technician does not replace an
earlier party member. As in the component-salvage routine, both locals begin at
zero, leaving slot zero as the implicit fallback if no living character has a
Tech value above zero. There is no explicit all-dead/all-unskilled handler in
this block.

The former C compared addresses rather than the `Name` and `Skill_Tech` byte
values and narrowed the selected locals to bytes. Those were decompiler
transcription errors, not shipped-game behaviour.

## Non-linear transition

After technician selection, the executable stores zero in the candidate-party
local and jumps to `0DAB:0850`. That later block scans for living characters
with nonzero Piloting who are currently on foot. Only after such a character
accepts the salvage prompt does execution choose a random wreck and jump back
to the engine beginning at `054C`.

This non-linear layout explains why reading only the source addresses in order
can make the wreck-selection loop look like the immediate next operation.

No Astra confirmation is required: field offsets, word locals, comparison, and
the jump target are explicit in the clean assembly.
