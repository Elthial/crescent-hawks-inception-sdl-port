# `BTECH_0800` attract mode and startup-loop tail

## Reviewed block

- Address range: `0800:51F9-5276`.
- Parent routine: `Start_Game`.
- Interactive startup occupies `0800:5126-51F8`.

This block loads the recorded self-playing demo when the title screen receives
no input, establishes deterministic random state, runs the demo through the
ordinary main loop, and then restores the title screen for another attempt.

## Loading the replay

The idle branch first selects logical game disk 2, then reads exactly `0x03FF`
bytes from the external `DEMOFILE` asset into `3092:27B0`. This confirms that
the destination is a byte buffer. The former `short[0x03FF]` scratch-pad
declaration incorrectly doubled its apparent size.

Word `3092:39F8` is initialized to `0x2710`. The input routine does not use it
as an absolute address: it reads from `3092:00A0 + index` and increments the
word after every byte. Consequently:

```text
0x00A0 + 0x2710 = 0x27B0
```

The maintained name `AttractModeReplayIndex` describes that pre-biased byte
index. It must not be normalized to zero unless the replay reader's base
address is changed at the same time.

## Deterministic game setup

Before starting the demo, the executable writes:

```text
3EDB:4FC0 = 0x1325   random seed
3EDB:4FC2 = 0x0090   random state
3092:3988 = 0xFFFF   no currently loaded tileset
3092:3938 = 0x0001   use recorded input
```

It then calls `Load_Game_Map_Data` and `Main_Game_Loop(TRUE)`. The fixed PRNG
state and exact replay bytes are both required to keep the demonstration
synchronized; a C# port cannot substitute a platform random-number generator.

## Reconstructed startup loop

The native word local at `[bp-2]` is initialized to one. The attract branch
leaves it unchanged, whereas the interactive branch clears it after normal
play returns. At the common tail:

- a nonzero word reloads `BTTITLE.CMP`, restarts the intro music, and returns to
  the title-input test at `5126`;
- zero skips both reload calls and returns from `Start_Game`.

This proves that the old Reko-shaped `while (Bool_MainLoop == FALSE)` and nested
`do/while` were inverted artifacts. The maintained C now expresses one direct
`while (ContinueStartupLoop != FALSE)` loop. The second local initialized at
`[bp-1A]` is never read in this routine and was omitted as dead compiler output.

No Astra confirmation is required: the branch targets, word values, replay
address calculation, and common-tail tests are explicit in the clean assembly.
