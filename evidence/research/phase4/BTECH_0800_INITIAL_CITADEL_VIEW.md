# `BTECH_0800` initial Citadel view

## Reviewed block

- Address range: `0800:4FB6-5024`.
- Parent routine: currently `Load_Game_Map_Data`.
- Combatant display-state initialization begins at `0800:5025`.

This block blanks and reconstructs the game display, establishes the starting
packed world position, loads the Citadel into the central map-cache slot, and
renders the initial terrain view.

## Display setup

The sequence is exact:

1. `207F:1FBE` blanks the game screen.
2. `Draw_Health_and_C_Bills_Sidebar(TRUE)` rebuilds sidebar border 3, redraws
   the top graphic, and writes the starting party/economy information.
3. `1E56:0388` draws the top sidebar graphic again.

The third operation appears redundant because the preceding `TRUE` argument
already selects that redraw. Both calls exist in the executable and remain in
the maintained source; this block supplies no evidence that one is removable.

## Packed starting position

The two 16-bit position words are:

```text
X = 0C45
Y = C019
```

As elsewhere in the game, the low seven bits are local position and the upper
nibbles encode the larger world page. Combining X's page nibble in the low half
and Y's page nibble in the high half gives `CC`. Therefore the argument passed
to `207F:104E` is named `CitadelStartingPackedPage`; the former `CitadelMap`
name incorrectly implied that it was a map-file identifier.

The actual Citadel MTP file identifier is `MAP_Citadel = 1`. `DOS_Load_Map_Files`
receives `(4, 1)`, placing `MAP1.MTP` in slot four, the centre of the cached
3×3 map neighbourhood.

## Map preparation and rendering

After the MTP load, `207F:1DA8` rebuilds the nine-grid map data. `207F:1314`
then caches the precise starting coordinate and derives the nine local terrain
selectors surrounding it. Finally `207F:18EF` copies/renders the corresponding
terrain into the retained EGA display buffer.

The calls and arguments are explicit in the assembly. The internals of the
three `207F` map helpers still deserve their own bounded review, but no Astra
confirmation is required to preserve this initialization sequence.
