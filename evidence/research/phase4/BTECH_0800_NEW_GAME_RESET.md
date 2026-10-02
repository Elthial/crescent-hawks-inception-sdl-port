# `BTECH_0800` new-game reset prefix

## Reviewed block

- Address range: `0800:4DC7-4E78`.
- Parent routine: currently `Load_Game_Map_Data`.
- This is the first bounded section of that larger routine; initialization of
  Jason and the Citadel begins at `0800:4E79`.

Despite its current broad name, `0800:4DC7` constructs a fresh game state and
starting Citadel rather than loading a saved game. The routine will be renamed
after all of its sections have been reviewed. This block is its destructive
reset prefix.

## Mech and party sentinels

The initializer writes `FF` to the four stored friendly-mech name initials at
`3092:D452-D455`. It separately writes `FF` to byte zero of all eight live mech
records:

```text
3092:C724 + mechIndex * 0x7D, mechIndex 0..7
```

This is an exact use of the empty/destroyed sentinel. It does not assign a
value to the complete 16-byte mech-name field, as the former pseudo-C implied.
The broader interpretation of any high first-name bit remains probable, but
literal `FF` here is verified.

Party slots 1 through 7 then receive `NameId = FF`. Slot zero is deliberately
left for the Jason initialization that follows at `4E79`.

## Jason skill reset

The seven contiguous bytes at `3092:C618-C61E` are cleared. These are Jason's
Bows and Blades, Pistol, Rifle, Gunnery, Piloting, Tech, and Medical skills.
The executable indexes bytes from the first field; it does not dereference a
32-bit `Skill` object or pointer.

## New-game state span

The loop at `0800:4E43-4E5D` clears:

```text
3092:D30C through 3092:D36F inclusive
length = 0x64 bytes = 100 decimal
```

This corrects the earlier `PrimaryBoolList[64]` interpretation. The assembly
compares the 16-bit loop index with hexadecimal `0064`; it does not clear 64
decimal (`0x40`) bytes. The range ends immediately before the four-byte C-Bill
balance at `D370`.

All named story, menu, timer, equipment, and settings fields in that address
range are overlay views of these 100 bytes. They are not structure members
located after an independent array. The scratch-pad view is now named
`NewGameStateReset_D30C[0x64]` to make both purpose and extent explicit.

## Cache security-code state

Finally, the initializer clears `0x21` bytes—33 decimal—at
`3092:45DE-45FE`. Other Cache code indexes these as one-use security-code
state. This range is not sequential storage after `D455`; its declaration in
`BTECH.h` is explicitly an address-view alias.

## Confidence and porting notes

Confidence is high. Every destination, stride, byte width, loop bound, and
sentinel is explicit in the clean assembly. No Astra review is requested.

A C# new-game constructor should clear logical state directly instead of
reproducing the segmented bulk writes, but compatibility tests should assert
the same 100-byte saved-state reset span and 33-byte external security-code
reset. Original external assets are not involved in this prefix.
