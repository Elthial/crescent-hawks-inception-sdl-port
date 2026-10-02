# Sol: Star League cache entry and secret passage

Scope:135D:0004–0287, two routines only. Evidence: original-version local
`BTECH_135D.asm`; inline source bytes at3EDB:20BC–20CD in `BTECH_3EDB.asm`.
This is a research transcription audit, not gameplay or compiled-port validation.

## Cache setup0004

0014 saves each friendly mech's Name BYTE to3092:3248+slot, then writesFF
to the live Name. It does not save record pointers or all125 bytes. Other
record fields survive. The typed Mechs index is the slot, not slot*125.
The distinct named backup view is `CacheEntryMechNameInitial_3248`, not the
arena/jailbreak name tableD452. No restoration is invented in this routine.

0051 temporarily writes all eight character Piloting BYTEs to0 (mech slot0).
0195 later writes8 (on foot), after entrance drawing and before INSTRUCT.BLD.
These two writes are not interchangeable; their existing purpose is preserved.

006C moves the party to external cache position C06/C07E. Original adapter0
conversion2FE8:01B0 remains omitted from the maintained EGA-only body. The
missing disk2 selection is restored. 00A3 fills576 descriptor BYTEs at0564
withD0, using the existing full nine-grid alias rather than overrunning the
top-left64-byte view. 00BA loads map0E into grid slot4;00CF invalidates three
pending-grid BYTEs. 00E5 sets compatibility WORD4FBC=1 before the tile load.

STARLEAG.ICN is loaded compressed into246C:244B and decoded into3092:4614.
0129 stages tiles atA400 only for adapter2. Tileset WORD3988 becomes2;
the descriptor/map cache is rebuilt, projected and copied to the screen.
The routine waits60 vertical retraces, then calls0AB6 with position C03/097D
and WORDFFFF (-1). The old `Cache_Open` BYTE-like FF definition cannot represent
that signed WORD parameter: a scoped `CacheDoor_LookupByPosition` constant
preserves the actual call without silently changing unrevised0AB6 branches.

After INSTRUCT.BLD,01C1 replays every nonzero saved door latchD34F[0..10]
through0AB6(0,0,index). The eleven-byte named latch view overlaps historical
scratchpad offsets; other old wD34F aliases are deferred to the door audit.

## Secret passage01E9

0204 reads AL from inline3EDB:20BC+row*3+column and stores that BYTE at
246C:101D+A10+row*8+column. Six rows, three columns, eighteen writes;
the other five bytes in each eight-byte destination row remain unchanged.
The former C stored a numeric source address instead of dereferencing it.
The existing CHECK ASM marker is resolved by named source/destination views.

The original inline pattern is:

```text
BB 1C 00
BC 00 01
BD 00 1C
BE 00 01
BF 1C 00
C0 01 00
```

0238 redraws the projected map and units, opens a message box, displays the
passage message through the retrace wrapper, explicitly reads an ASCII key,
then sets MessageBox_Open WORDD55C=1. No new persistent passage flag was added.
The source follows the user's wall-to-passage interpretation; the tile IDs
themselves were not given speculative art labels.

## Verification and next boundary

Synthetic tests cover all256 saved Name values, name-only hiding, passage
BYTE stores/row pitch/untouched surrounding tiles, all2048 combinations of
eleven door latches, signed WORD sentinel and setup source ordering. These
checks do not render original tiles or execute the original BLD/game.

Passed:11,532 cache/passage assertions,914 helper-contract regression assertions,
23,115 map-cache regression assertions, and `git diff --check`.

Next:135D:0288–03A9, cache discovery/power dialogue and overhead-map buffer
swapping. Door/security routines at03AA and0AB6 remain bounded later work;
their damaged lookup loops and FF/FFFF comparisons must be checked against ASM.
No original-game bug is claimed from these Reko transcription errors.
