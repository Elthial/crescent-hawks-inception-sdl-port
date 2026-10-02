# Sol: startup, missing music routines and platform contracts

Scope:0D27 startup, the valid0/1 music-dispatch paths in204B, and the missing
EGA/text/snapshot helpers used by reviewed callers. Evidence: matching local
`BTECH_0D27.asm`, `BTECH_204B.asm`, `BTECH_207F.asm`, full `BTECH_code_0000.asm`,
and expanded EXE strings/tables. No external original asset is copied.

Owner direction: do not reproduce DOS internals. Native file, allocation,
video and interrupt operations are contracts for replacement services, not
requirements to emulate DOS in the eventual C or C# application.

## Startup0D27

000A emits a NUL-terminated FAR string one BYTE at a time, CBW signed extension,
WORD index and offset wrap, through0213. The maintained reconstruction names
that external operation as a replaceable startup-presentation binding.

0044 originally offers graphics choices ASCII1..4, storing the ASCII choice
temporarily in4FBA, then subtracting'1' at0112. The reconstructed `Setup_Game`
retains only EGA index2, explicitly skipping the obsolete adapter-choice menu
and all deleted adapter bodies. This is an EGA-only preservation view, not
a claim that the original game forced EGA or a proposed modern startup UI.

Drive choices1/2/3 produce single-floppy, second-floppy and hard-disk layout
flags. Choice3 resets3FFE to0 and setsD580 to1; choice2 leaves3FFE=1. Associated
instruction screens wait for a key. These flags configure asset-source lookup,
not story or combat state. A modern installation-directory selection can
replace this entire presentation without changing gameplay rules.

Order after selection:

1. Initialise presentation/timing through0DAB:0C8F; select original disk2.
2. Load/decompress INFOCOM.CMP, stage atA800, apply the default16-byte palette,
   draw320x200, drain input and show it for up to700 retraces or a key.
3. Drain input; show BTTITLE through existing0800:46A7.
4. Load/decompress/stage BTBORDER.CMP; capture8 tiles into4066/4068 (256 bytes).
5. Load/decompress/stage TINYLAND.CMP; capture66 tiles into4588/458A (2112 bytes).
6. Load/decompress MECHSHAP.CMP; EGA03F5 converts it IN PLACE through207F:0572,
   with count16000 WORDs (32000 source bytes), into interleaved plane bytes.
7. Capture sprite snapshots, then enter0800:50C8 `Start_Game`.
8. On return,0DAB:0D12 selects BIOS text mode2; the portable equivalent releases
   presentation resources. Full listing supplies the truncated startup return.

No per-sprite free calls occur in this startup/shutdown sequence; lifetime is
process-long natively. The port should own and release these resources through
its asset/presentation services, not copy the implicit process-exit lifetime.

## Snapshot table: confirmed376 entries

| IDs inclusive | Capture source X/Y, width/height | ASM cluster |
| --- | --- | --- |
| 0–11 | X=3*id,Y=0,3x24 | 0410 |
| 12–15 | X=3*id-36,Y=24,3x24 | 043B |
| 16–35 | X=id-4,Y=24,1x8 | 046A |
| 36–119 | Seven rowsY72..120;X12..23,1x8 | 0491 |
| 120–129 | Ten explicit rectangles atY96/120/144/160 | 04DB–05E7 |
| 130–145 | Four rowsY144..168;X4..7,1x8 | 05F8 |
| 146–157 | X=3*(id-146),Y48,3x24 | 0641 |
| 158–161 | X=3*(id-158),Y72,3x24 | 0671 |
| 162–165 | Four explicit3x24 rectangles atY96/120 | 069C–06FE |
| 166–185 | X=id-154,Y32,1x8 | 0710 |
| 186–269 | Seven rowsY72..120;X24..35,1x8 | 0738 |
| 270–289 | X=id-258,Y40,1x8 | 0787 |
| 290–373 | Seven rowsY128..176;X12..23,1x8 | 07B3 |
| 374–375 | X0/2,Y144,2x11 | 07FD/081C |

IDs are contiguous with no duplicates or holes.39FA+376*4 ends at3FD9,
before3FFE. The previous256-entry header view was too short; corrected to376.
Captures request24240 bytes including four-byte headers (40 large,330 small,
six effect-sized rectangles). This does not count all game allocations.

X/width are eight-pixel cells, Y/height scanlines. The buffer entering070A is
already INTERLEAVED PLANAR, not packed4bpp: the earlier1F3D note was corrected
after tracing its startup caller. Both representations use160 bytes per row,
which made the stride alone insufficient evidence for the original claim.

## Music204B: valid original stream paths

Named CS state covers000E/0010 system divider,0012/0014 saved IRQ0,
0016/0018 active FAR callback,0205/0207 stream cursor,0209 countdown,
020A reload. It is an overlapping research view, not a serialized C# model.

0020 calls the active callback first. If the OLD low BYTE of000E is zero,
it reloads16 and jumps to the saved IRQ0. Otherwise it decrements AL, sends
PIC EOI and IRET. Initial counter0 therefore chains immediately, then every
17 ticks: sixteen decrement-only ticks followed by a chaining tick. The former
"every16" comment was wrong; no timer formula is silently corrected. The
callback cadence still uses PIT divisor0FFF, independently of IRQ chaining.

0233 sets the16:16 stream cursor, reload=low BYTE(cadence), countdown=1.
020B adds PSG volume3 setup for three tone channels. First music IRQ reads
immediately; subsequent intervals are4 ticks for PC or20 for Tandy. Reload0
means256 ticks, not immediate repetition. The reconstructed tick routines are
reference effects; `Music_Clock_Tick_0020` is explicitly a model, not a C ISR.

0298 PC consumer reads one BYTE. A single zero consumes a second BYTE in the
SAME tick; two consecutive zeros stop without committing the temporary SI.
024F Tandy consumes four BYTEs; first zero stops, likewise without committing
SI. Channels are PSG0, PC speaker, PSG1, PSG2 in that order.0179 switches to
idle0186 and disables speaker gate, without writing a new frequency.0160 also
mutes three PSG tone channels.033C compares callback OFFSET with0186; it is
not a whole-FAR-pointer comparison.

0139 computes unsigned note quotient/remainder by12, divides native DWORD
1234DE by the selected frequency WORD, then shifts its WORD divisor left by
BYTE(8-octave).8086 CL shift counts are not masked to five bits.0187/0298
write PIT divisor14 for ANY high-bit note BYTE: it is not a literal gate-off
operation. Tooling's audible-rest approximation remains separate.

01B8 reads a period WORD, applies SAR by BYTE(octave-2), and emits PSG low-nibble
latch plus six-bit high portion. It has no separate high-bit rest branch. The
ordinary80h rest code yields period4 (table entry0432h shifted right8),
not zero or a volume-mute command. Other high-bit inputs can yield other small
periods or zero; do not replace the whole high-bit range with a single native
mute operation. The Tandy WAV toolkit remains
an approximation, not electrical chip emulation; it was not rewritten here.

02E4/0306 use signed JGE13, then stride4 indirect dispatch. Only selector0/1
have valid confirmed records. This is NOT evidence for thirteen safe entries;
negative signed selectors and invalid low selectors can read outside the table.
Source tests now express that signed predicate and explicit WORD address wrap.
Dormant raw-byte pitch-interpolation helpers00DE/00F5/010A are outside the valid
intro table paths; they are not required for the two confirmed SIF consumers.

## Missing EGA and copy helpers

Reconstructed EGA-only bodies2127 (colours),05D0 (vertical pixel run),0780
(aligned span),2251 (glyph),0931 (strided snapshot copy). WORD coordinate/count
stores are added as header views beside legacy BYTE aliases, not destructively
substituted into a purported packed host structure.

05D0/0780 initial row address uses BYTE(Y)*40, matching MUL DL.05D0 writes
once before signed endpoint comparison.0780's unguarded LOOP with zero count
writes65536 bytes.2251 uses the embedded font at246C:A661, eight rows, writing
foreground through glyph mask then background through complement mask. Latch
reads matter; no final bitmask reset occurs.0931 uses BYTE widths/heights,
zero height=256 rows, and assumes DF clear. These bodies record valid caller
effects, not bounds-checked general rendering APIs. Odd WORD access across
offsetFFFF and malformed dimensions need emulator validation if preserved.

Existing0260 staging,0313 tile capture,0377 transparent sprite and0572 conversion
were already reviewed. No deleted adapter code is restored.33D0/3580/3336 file
operations and3835 allocation remain documented service contracts; DOS heaps,
interrupt installation and error-handler internals are deliberately deferred.

## Portable module boundaries (direction, not implementation)

| Original responsibility | Replacement boundary |
| --- | --- |
| Disk prompts, open/read/close and buffer allocation | Original-installation asset provider + ordinary owned buffers |
| Startup prompts, menus, text, palette and EGA writes | Presentation/UI and renderer; no game rules inside them |
| IRQ/PIT/PSG writes and song completion | Audio sequence decoder + playback clock/backend |
| Party/map/combat/story decisions | Game-state/rules modules; emit presentation/audio requests |
| BIOS mode restoration and process exit | Application host disposing services |

SDL is a possible C backend, not a selected dependency. C# should receive
decoded asset models and explicit state/events, not segment addresses, DOS
interrupts, rendering calls or UI choices embedded in its game logic.

Verification: `Verify-StartupMusicContracts.ps1`, existing hardware/input/tileset
models and toolkit verification. Synthetic checks do not run the original game.
Remaining follow-ups:0800 FAR-sprite readers, live music/rendering comparisons,
and uncertain original quirks already queued. No expensive model invoked.

Passed187,656 new startup/music/EGA model/source assertions; hardware6659,
input296, tileset3155, sound-field39 and toolkit182 regressions, plus whitespace
validation. Toolkit verification exercises the existing renderer, not the new
native research bodies or electrical emulation.
