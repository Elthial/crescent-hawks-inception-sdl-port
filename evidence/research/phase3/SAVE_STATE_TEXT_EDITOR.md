# Phase 3N — command-line save-state text editor

## Review boundary

This block adds a non-interactive, command-line save editor. It fetches the
verified save fields into a diff-friendly `.txt` file, accepts hand edits to
that file, and applies the edits over the original save to produce a new save.
There is no UI and no attempt to name undocumented flags.

Unknown bytes are preserved by design. Import begins with an exact clone of the
source save and patches only fields whose parsed values changed. An unchanged
export/import therefore produces a byte-identical file.

## Commands

Export editable state:

```powershell
dotnet run --project InceptionTools -- export-save-state GAME1 `
  --game-dir chinception `
  --output GAME1-state.txt
```

After editing values following `=`, import them into a new save:

```powershell
dotnet run --project InceptionTools -- import-save-state GAME1 GAME1-state.txt `
  --game-dir chinception `
  --output GAME1.edited
```

Defaults are `GAME1-state.txt` and `GAME1.edited`. Existing output files are
collision-protected unless `--force` is supplied. Import always refuses to use
the source save itself as its output, even with `--force`.

The import command prints every changed byte with its save offset, field path,
old value, and new value.

## Text format

The format is one `key=value` field per line. Blank lines and lines beginning
with `#` are ignored. Decimal numbers and `0x`-prefixed hexadecimal numbers are
accepted; arrays are comma-separated byte values. Unknown or duplicate keys
are rejected so spelling mistakes cannot silently disappear.

The document contains:

- header, probable story-state byte, credits, stocks, and party coordinates;
- all 16 character records, including attributes, seven skills, equipment,
  armour, health, assignments, and training flags;
- all eight complete mech records, including name, armour, internal structure,
  ammunition, critical slots, movement, damage, pilot/rider IDs, and the
  conservatively named actuator/life-support/upgrade bytes.

Each mech exposes that shared high bit as an editable inverse boolean:

```text
mech.enemy.2.active=false
```

`active=true` uses an ordinary first name byte; `active=false` writes the
`0xFF` sentinel over it. The interpretation is still **Probable**, so the text
comments that provenance. When the stored first byte is `0xFF`, the editor
restores the display name only when the remaining suffix exactly matches one of
the eight verified chassis names. Thus `FF OCUST` exports as `name=LOCUST`, and
changing only `active=false` to `active=true` restores the `L`. Unknown suffixes
are not guessed. Changing a name preserves the selected active state; a truly
empty `0xFF` slot requires a complete non-empty name when activated.

The probable 2,048-byte map-visibility region and other undocumented state are
not expanded into editable fields yet. They remain unchanged in the binary.

Each export includes the original save's SHA-256 fingerprint. Import requires
that fingerprint to match the selected source save. A state document therefore
cannot accidentally be applied to another slot or another version of the same
slot.

## Reusable API

The command-line adapter uses these UI-independent operations:

```text
SaveStateEditor.FetchState(byte[] original, string sourceName)
SaveStateTextFormat.Write(SaveState state)
SaveStateTextFormat.Parse(string text, SaveState baseline)
SaveStateEditor.UpdateState(byte[] original, SaveState edited)
```

`SaveStateFileService` supplies the installation/file adapter used by the two
commands. A future UI can bind directly to `SaveState` and the fetch/update API
without parsing command output or duplicating offsets.

## Verification

The dependency-free harness passes **154 assertions**. The save-editor checks
cover editable text content, byte-exact unchanged round trips, edits to finance,
character, and mech fields, changed-byte reporting, defensive byte arrays,
unknown-byte preservation, fingerprint enforcement, range validation, unknown
keys, output creation, source-overwrite refusal, readable marked mech names,
active-bit editing, and safe population of an empty slot.

All six ignored original `GAME1`–`GAME6` files were independently exported and
re-imported without edits. Each generated save matched its source SHA-256
exactly. Original saves were read only; generated test outputs remain ignored.
