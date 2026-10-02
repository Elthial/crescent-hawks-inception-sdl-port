# `BTECH_0DAB` friendly-combatant description

## Scope

This note covers `0DAB:18E8-1AFD`, renamed
`Display_Friendly_Combatant_Description_18E8`.

All three stack arguments are native 16-bit words:

```text
(combatantId, allowTraitorRecognition, showInfantryDetails)
```

## Friendly 'Mechs

Combatant IDs `0..3` are friendly 'Mechs. The function reads the selected
mech's `PilotId` at record offset `+0x79`, treats it as an `Infantry[]` party
slot, then obtains the character Name ID from that infantry record. It prints:

```text
<pilot name>'s
<mech name>
```

Both option flags are ignored on this path.

## Friendly infantry

Combatant IDs `4..11` map to `Infantry[0..7]` by subtracting four. The function
always prints the party member's name. If `showInfantryDetails` is false, it
returns immediately; this short form is used by the combat command menu.

The scan browser enables the full form. Normally it then prints:

1. the health description selected by `Display_Text_Human_Health`;
2. the labels `Weapon:  Armour:`;
3. the weapon name selected from the 17-byte table at `DS:2ED8`; and
4. the armour description.

Armour type zero prints `None` in bright green. Nonzero armour with zero
durability prints `Ruined` in dark grey. Intact armour remains bright green;
damaged but usable armour is bright yellow. The alternate-adapter colour
exceptions visible in the original assembly are obsolete in the maintained
EGA-only source.

## Traitor recognition

When both detail flags are enabled, a living/in-party traitor whose stored
party slot equals the displayed infantry slot replaces the normal health and
equipment description with:

```text
 looks very suspicious to you
```

The routine performs its timing/input calls, sets the WORD `TraitorWarning`
flag at `3092:374A`, and returns.

The former pseudo-C compared `Traitor_CharacterId` to a constructed text
pointer. The assembly compares `D331` directly with `combatantId - 4`; D331 is
therefore a party-member index, despite its historical field name. This agrees
with its other uses as an `Infantry[]` subscript.

## Corrected decompiler artefacts

The previous annotation also:

- passed a text pointer to the health-description routine instead of the party
  slot;
- referred to an undefined `wArg04` while selecting the weapon;
- read armour type and durability from unrelated `bF2AC/bF2AD` scratch names;
- treated a `Weapon` record and far-pointer tables as scalar text values; and
- declared byte arguments even though every caller pushes WORDs.

All corresponding effective addresses now agree with the clean expanded
assembly. Confidence is high and no Astra review is required for this block.
