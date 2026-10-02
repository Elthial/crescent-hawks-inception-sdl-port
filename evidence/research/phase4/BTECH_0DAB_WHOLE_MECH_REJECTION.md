# `BTECH_0DAB` whole-mech rejection message

## Reviewed block

- Address range: `0DAB:07B5-080D`.
- Parent routine: `Salvage_Mechs_Dialog`.
- Common candidate-state handling begins at `080E`.

When the damage/technician gate rejects a wreck, this block constructs and
displays its failure message. It does not alter the loop state; control rejoins
the common path, which either tries another marked wreck or stops when the table
is exhausted.

## Reconstructed name

The literal at `3EDB:102F` is copied to the shared buffer at `3092:0012`:

```text
<carriage return>No chance to salvage this X
```

The leading byte is `0x0D`. Buffer byte `3092:002D` is the `X` placeholder. Its
position also confirms that the carriage return is part of the copied string.
The placeholder is overwritten with the
selected wreck's original first name character from
`DestroyedMechNameInitial_323E`, after which `Mech.Name[1..]` is appended.
This avoids using `Name[0]`, which remains the `0xFF` destroyed marker.

The former annotated literal incorrectly included a period after `X`. Since the
name tail is appended after copying the literal, that transcription would have
produced punctuation in the middle of the mech name. The expanded executable
confirms both the leading carriage return and that the string terminates
immediately after `X`.

Unlike the success path, this block does not trim the fixed-width name's trailing
spaces and does not call the sentence-period helper.

No Astra review is required: the string bytes and buffer operations are explicit.
