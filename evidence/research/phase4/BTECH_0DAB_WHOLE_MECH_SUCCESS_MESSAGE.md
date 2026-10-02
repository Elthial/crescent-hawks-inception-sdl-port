# `BTECH_0DAB` whole-mech salvage success message

## Reviewed block

- Address range: `0DAB:05EA-06BC`.
- Parent routine: `Salvage_Mechs_Dialog`.
- The next block at `06C0` searches the four player-mech records for an empty
  lance slot and installs the recovered mech.

This block announces a successful wreck inspection, reconstructs the destroyed
mech's name, removes its fixed-width padding, and marks the candidate loop as
complete.

## Message construction

The selected technician's party record supplies a character-name ID. That ID is
used to fetch the far string pointer from the character-name table and display
the technician's name directly.

The routine then copies the literal at `3EDB:101D` into the shared buffer at
`3092:0012`:

```text
 is salvaging a X
```

The leading space is part of the original executable string. The former C
transcription omitted it.

## Preserved mech-name initial

Destroyed mechs have byte `Name[0]` at record offset `+0x00` replaced with
`0xFF`. Before doing that, the destruction path at `1543:0AB2-0AD9` saves the
original first character in the eight-byte table at `3092:323E`, indexed by the
compact `Mechs[]` record ID.

The success-message code copies that saved character over the `X` placeholder
at buffer address `3092:0022`, then appends the selected mech record beginning
at `Name[1]` (`3092:C725 + recordId * 0x7D`). Together these operations rebuild
the complete name without temporarily restoring the wreck record itself.

The same mechanism is used by the failure message: its `X` placeholder is at
`3092:002D`.

## Padding removal

Mech names occupy a fixed-width 16-byte field and may end in spaces. After
appending the name, the routine obtains the string length, begins at the final
character, and replaces consecutive trailing spaces with null bytes. The stack
index is a signed 16-bit word. Modelling it as an unsigned byte, as the former C
did, risks wrapping after the stop assignment and does not represent the
assembly's signed `JLE` termination.

The trimmed message is displayed, the usual sentence-period helper is called,
and the 16-bit salvage-loop state is set to `1` for success. It may later take
the value `9` when no wreck candidates remain, so it is a state word rather than
a Boolean.

No Astra review is required: the string bytes, saved-name write, field offsets,
and signed loop branches are directly visible in the executable and assembly.
