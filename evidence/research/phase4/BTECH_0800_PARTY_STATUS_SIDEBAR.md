# `BTECH_0800` party-status and C-Bills sidebar

## Reviewed block

- Address range: `0800:4CAC-4D56`.
- Maintained routine: `Draw_Health_and_C_Bills_Sidebar`.
- The next routine begins at `0800:4D57`.

This controller rebuilds the compact party-status panel, draws up to four
character rows, displays the current C-Bill balance, and leaves the menu system
in its ordinary post-sidebar state.

## Redraw argument

The single argument is a 16-bit stack word. The previous
`Bool_ShowBorders` name was misleading: border 3 and border 4 are drawn on
every call. The argument only controls the call to `Draw_Top_Graphic_Sidebar`
between those two operations, so the maintained name is `RedrawTopGraphic`.

The opening sequence is:

1. select menu/text state 3;
2. draw menu border 3; and
3. redraw the top graphic/sidebar only when `RedrawTopGraphic` is nonzero.

## Character selection

The routine draws the bright-white `BDC` heading at text column 9, row 13.
Those letters label the Body, Dexterity, and Charisma bars reconstructed in the
preceding `0800:4AA6-4CAB` block.

Two independent 16-bit counters then scan the party:

- the party-record index visits slots 0 through 7 in order;
- the displayed-row count stops increasing at four.

At `0800:4D06`, the executable compares the record's `Name` byte at
`3092:C614 + index * 0x11` directly with `FF`. Dead or unused entries are
skipped while the scan continues. Consequently, the panel contains the first
four non-dead party records, not necessarily slots 0 through 3.

Displayed rows use text Y coordinates 14, 16, 18, and 20. The record index and
calculated row are passed as 16-bit words to
`Draw_Character_BDC_Sidebar_Row`.

The old annotated expression compared `&Infantry[index].Name` with
`Character_Dead`. That address comparison was a pseudo-C artefact; the clean
assembly performs a byte comparison against the field value.

## Finalization

After the character scan, `1631:1FDF` draws the C-Bill balance. The controller
then selects menu/text state 4 and draws border 4. This final pair is
unconditional and does not depend on `RedrawTopGraphic`.

## Confidence and porting notes

Confidence is high. The word-sized argument and counters, byte comparison,
four-record display limit, row calculation, and draw order are direct assembly
translations. No Astra review is requested.

A C# implementation can express the original loop as “take the first four
party records whose `NameId != 0xFF`,” while retaining record order and the
fixed row positions. The panel itself is presentation state and should consume
the canonical character/save models rather than own another copy of them.

The per-character geometry is documented in
[character B/D/C sidebar row](BTECH_0800_CHARACTER_BDC_SIDEBAR.md).
