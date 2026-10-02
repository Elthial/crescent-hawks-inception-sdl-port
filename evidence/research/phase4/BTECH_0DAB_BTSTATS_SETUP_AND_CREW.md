# `BTECH_0DAB` BTSTATS setup and crew display

## Scope

This note covers the opening of `Examine_Screen_BTSTATS_CMP`,
`0DAB:1AFE-1D8A`. Later armament, actuator, animated-gauge, palette, and input
sections remain separate review blocks.

The function accepts a native 16-bit combatant ID. Friendly 'Mech IDs `0..3`
already match `Mechs[0..3]`. Enemy combatant IDs `12..15` are normalized by
subtracting eight, selecting the live enemy records at `Mechs[4..7]`.

## Asset setup

The first invocation increments the WORD cache flag at `3092:4594`, selects
logical Game Disk 1, and loads the original `BTSTATS.CMP` into the shared
BTSTATS/BLD working region at `3092:00A0`. The BLD decoder later processes a
9000-byte span of that region, although its exact reserved boundary is still
not proven. Every invocation decompresses the buffered BTSTATS data to
`246C:244B` and renders the full 320x200 background.

Loading any BLD later clears `3092:4594`, ensuring that the next inspection
reloads BTSTATS rather than interpreting BLD bytes as the image. The maintained
EGA path draws the decompressed image into the `A800` buffer and transfers it to
the `A000` display plane.

The original executable also clears a compatibility word and calls
`207F:00D1(3056:0000)`. That helper rebuilds an adapter-0 colour-conversion map;
it is deliberately omitted from the EGA-only transcription.

## Static labels

`DS:127E` is one carriage-return-delimited string drawn at grid position
`(0,0)`:

```text
Type :
Tons :
Pilot:
Rider:

Armament   Loc
```

The previous annotation retained only the first label, losing the rest of the
single text call.

## Type and tonnage

The selected record name is copied into the scratch string beginning at
`3092:0012`. The explicit NUL write at `3092:001A` limits the displayed Type
field to eight bytes. That byte was previously misidentified as
`Bool_ExamineScreen_Mech_001A`; it is a string terminator, not state.

The same scratch string is then overwritten with the base-10 tonnage and drawn
on the next row.

## Pilot and rider

Friendly records use `PilotId` and `RiderId` as zero-based `Infantry[]` party
slots. Each infantry record's Name byte selects the corresponding far pointer
from `CharacterNameList`. Rider ID `0xFF` displays `None`.

Enemy records do not reveal either crew member. The one original string at
`DS:12AF` is `Unknown\rUnknown`, so one draw fills both the Pilot and Rider rows.
The former pseudo-C represented it as one `Unknown` row and silently omitted
the other.

## Confidence

High. The cache flag, disk request, record normalization, string addresses,
explicit terminator, and pilot/rider effective addresses agree directly with
the clean expanded assembly and initialized EXE data. No Astra review is
required for this section.
