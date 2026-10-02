# Sol: EGA screen presentation — 2026-09-18

The original drawing redirects already wrote the shared four EGA planes, but
the SDL window only received an initial black image. No production call decoded
or presented the current screen. Successful plane/pixel tests and dummy-driver
gameplay did not prove that the game window could display the game.

## Hardware-layer correction

`src/SDL/ega.c` now decodes physical A000:0000..1F3F (8000 bytes per plane) into
320x200 opaque RGBA pixels. Four plane bits select one of sixteen attribute
palette registers. For the game's200-line output, bit4 supplies shared RGBI
intensity, bits0..2 select blue/green/red, and bits3/5 are ignored by the
monitor interpretation. Dark yellow is brown. Output uses RGBA bytes, independent of
host integer endianness. Scanout reads backing planes without changing EGA
latches, register state, source buffers, palette registers or game globals.

`src/SDL/video_audio.c` presents that screen at original input/retrace boundaries
once graphics runtime is active. Busy polling is throttled to a host 70 Hz
presentation cadence. The completed menu is also presented immediately before
a blocking keyboard read. Shutdown disables scanout before destroying SDL
resources. Existing nearest-neighbour scaling and letterboxing remain.

This is host display plumbing, not a game-loop rewrite or a claim of exact
EGA scan timing. No original game method changed. External artwork is loaded
locally as before. The narrowly scoped README images under `docs/images` are
documented original-rights exceptions; no raw game resource or general asset
export is checked in.

## Verification

`graphics.original_ega_screen_scanout` checks all sixteen plane indices against
all sixty-four palette codes, channel/alpha byte order, first/last screen pixels,
background pixels and unchanged hardware registers. It also performs a real
SDL texture upload/render/present with dummy drivers.

All 141 SDL/local-asset tests pass, including actual startup, new-game entry and
exploration. Presentation exposed a fixture timing bug: the first-time panel ID
survives into animation frame zero, so injecting another Y then left a stray key
ahead of movement. The original pending-key drain correctly discarded movement.
The fixture now stops first-time key injection as soon as O15.ANM starts; native
input/drain behaviour remains unchanged.

Visible OS-driver usability, screenshots compared with the original emulator,
palette/transient timing and a full playthrough still need validation. The
statistics screen now uses the separately documented portable presentation
initializers; this scanout correction does not recover native stack residue.

## Human inspection correction

Sol: the owner's2026-09-18 screenshot exposed pale-green text/borders even on
startup. The previous decoder used350-line independent secondary intensities;
register17h therefore becameAAFFAA instead of white. The decoder now uses
200-line RGBI:17h isFFFFFF,10h is555555, and06h isAA5500. Register values
and original palette-writing routines are unchanged. Reference monitor wiring:
[DOSBox-X palette output](https://dosbox-x.com/doxygen/html/vga__attr_8cpp_source.html).
The updated16-index/64-register-code scanout test passes. A fresh human run
is still required to confirm the visible correction.
