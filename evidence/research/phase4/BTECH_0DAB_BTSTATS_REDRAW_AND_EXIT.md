# `BTECH_0DAB` BTSTATS redraw and exit loop

Sol: Current checkpoint (2026-09-17): the later1F3D audit confirms the palette
helper's FAR argument; named EGA palette callers are corrected. The earlier
remaining-helper paragraph below is historical, not an open width TODO.

## Review boundary

This block covers `0DAB:21BF..230D`, the tail of
`Examine_Screen_BTSTATS_CMP`. It repeatedly redraws the mech silhouette and heat
gauge, animates palette entry 4, waits for input or a scripted timeout, restores
the normal palette, and consumes the terminating key.

## Armour, structure, and heat redraw

Each pass draws all eleven armour locations using the three WORD tables at
`DS:1306`, `131C`, and `1332`. A nonzero structure offset selects one byte from
the current mech record; zero supplies no red structure segment for the three
rear-torso armour locations.

Heat is an eight-byte signed runtime array at `3092:006E`, indexed directly by
mech ID. The selected byte is clamped to `0..30`, then drawn at X `0x0100`,
bottom Y `0x00B7`, with `30 - heat` green rows above `heat` flashing rows. The
loop waits for one vertical retrace after every complete redraw.

The byte width is instruction-level evidence: the assembly uses
`es:[bx+006E]` without scaling `bx`, performs signed byte comparisons, and
sign-extends the selected byte before passing it to the WORD drawing helper.
The former `unsigned short Mech_HeatLevel[8]` declaration was therefore wrong.

## Palette animation

Every sixteen redraws, the routine loads the active BTSTATS palette and then
advances a four-phase cycle which patches palette entry 4 for the next load:

| Table | Address | Values |
|---|---:|---|
| EGA BTSTATS palette | `DS:1348` | 16 bytes |
| MCGA BTSTATS palette | `DS:1358` | 16 words |
| EGA entry-4 cycle | `DS:1378` | `06 04 0C 04` |
| MCGA entry-4 cycle | `DS:137C` | `0630 0620 0732 0620` |

Both replacement tables are patched regardless of the active adapter. The
shipped adapter test loads `DS:1348` for non-MCGA modes and `DS:1358` for MCGA;
the maintained transcription retains the EGA load.

The WORD frame and phase locals at `[BP-22]` and `[BP-2C]` are genuinely
uninitialised. This does not permit an out-of-range table access: the frame is
masked with `0x0F` each pass and the phase with `0x03` before indexing. It only
makes the first refresh delay and starting phase depend on prior stack contents.
This is retained as a minor original quirk rather than promoted to a gameplay
bug without evidence of a visible fault.

## Input and automatic dismissal

`3092:3938` (`DisableInput`) chooses between two exit paths:

- when zero, `Pending_Input` is polled after every redraw;
- when nonzero, a WORD countdown initialized to `0x0258` (600) is decremented
  after every one-retrace redraw, and the screen exits when it reaches zero.

The old pseudo-C variable `Bool_wLoc06_1692` has no assembly counterpart. Its
presence made the disabled-input path appear to exit after one iteration. The
real routine deliberately leaves a noninteractive BTSTATS screen visible for
roughly 600 vertical retraces.

On exit, the original restores `2FE8:0000` for non-MCGA adapters or
`2FE8:0010` for MCGA, then calls the keyboard-input routine to consume the key.
The maintained EGA path restores `DefaultEgaPalette_0000`.

## Remaining tooling discrepancy

`Set_Palette_registers` at `1F3D:0525` consumes a 16:16 far pointer, proven by
the two pushed parameter words and `LES SI,[BP+06]`. Its maintained definition
still declares a single byte offset. Calls in this block now show the intended
far-pointer sources and carry a TODO; correcting the helper and all other call
sites belongs in a separate bounded review.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0DAB.asm`, `21BF..230D`;
- `BTech-Reko-expanded/BTECH.reko/BTECH_1F3D.asm`, `0525..05B7`;
- expanded executable bytes at `3EDB:1348..1383`.

Confidence is high. No Astra confirmation is currently warranted.
