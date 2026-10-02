# `BTECH_0FDC` BLD interaction entry

## Review boundary

This block covers `Interact_with_BLD` at `0FDC:0008..01BF`. It selects and
loads a building script, optionally plays its entrance animation, invokes the
BLD interpreter, handles the two holodisk follow-up paths, restores the sidebar,
and dispatches the special `ENTRANCE.BLD` continuation. The BLD bytecode
interpreter beginning at `0FDC:01C0` is outside this block.

## Requested building versus resolved BLD

The argument and saved value at `3092:4584` are native WORDs. The routine stores
the original requested building ID before applying any map-specific remap. That
distinction matters because later Mech-Lube exit code uses the saved ID to
restore the corresponding map-return coordinates.

For every ID below `ENDMECH.BLD` except `VIEWDISK.BLD`, the routine replaces its
local ID with a byte from `3092:4602 + requestedId`. It then selects game disk 1,
loads the resolved BLD, plays the nonzero animation indexed by `DS:141A`, and
executes the decoded script buffer at `3092:00A0`.

The map loader reads exactly 16 bytes into `3092:4602..4611`, while the generic
remap comparison would also admit IDs `0x10` (THEATER) and `0x11` (FROB). Those
indexes would read beyond the verified table. They are unreachable in the
verified shipped call graph: exterior interaction passes only slots `0..0x0B`,
the recursive path passes `VIEWDISK`, and direct story calls pass `ENDMECH` or
later IDs which bypass remapping. A source TODO retains the bound as a warning
for future callers and the contradiction register records the resolution.

## Disabled copyright gate

The prior `if (true)` reflects a deliberate shipped-code bypass. At `0056`, an
unconditional jump goes directly to the proceed-flag test at `0085`, skipping
the otherwise coherent bytes at `0058..0084`.

The dormant sequence would:

1. run only for resolved BLD ID zero (`TRAINING.BLD`);
2. skip the quiz while cooldown byte `D320` is nonzero;
3. skip it when either `DisableInput` or the scripted/demo-input mode is active;
4. call the mech-diagram copyright quiz at `1467:08A8`;
5. clear the local proceed flag when the quiz returns nonzero, preventing the
   training BLD from loading.

This is consistent with the project-owner account that the original
mech-component diagram check was cut out before this executable was obtained,
leaving its code and data behind. The maintained transcription documents the
bypassed logic but follows the shipped unconditional jump.

## Holodisk chaining

After script execution and the common building-dialog call:

- `GARAGE.BLD` recursively runs `VIEWDISK.BLD` when the persistent viewed flag
  is clear, the party owns a holoviewer, and at least one party member exists;
- `BARRACK2.BLD` recursively runs `VIEWDISK.BLD` whenever the party is nonempty,
  without testing the holoviewer or persistent viewed flag.

Both paths assign `0x0C` to the runtime `2FE8:0064` holodisk display state after
the recursive interaction returns. The BLD itself owns any persistent story
flag changes.

## Entrance continuation and fog bytes

The tail compares the resolved ID with `0x14`, which is `ENTRANCE.BLD`. The old
annotation incorrectly named this `BLD_MAYOR` (`0x0E`). When byte `D342` is
nonzero, it clears eight bytes with a `0x10` stride beginning at `D118`, then
calls `Draw_STARLEAG_ICN_AND_Game_Logic`.

`D118` is not a separate Mayor-BLD array. Relative to the visibility bitmap at
`CB0C`, it is index `0x060C`. The loop therefore clears byte-column 12 in fog
rows 96 through 103. The precise story meaning of `D342` remains open, so it is
named neutrally as `EntranceBldState_D342`.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0008..01BF`;
- dormant raw instructions at `0058..0084`;
- the sixteen-byte read in `DOS_Load_Map_Files` and MTP offsets `0205..0214`;
- fog bitmap base and stride established by the save and overhead-map paths.

The active control flow, widths, ID comparisons, fog addresses, and shipped
caller bounds are high confidence. No Astra review is currently required.
