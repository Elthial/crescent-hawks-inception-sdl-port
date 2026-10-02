# `BTECH_0DAB` hardware initialization and timing calibration

## Reviewed range

- Address range: `0DAB:0B5E-0D3C`.
- Contains functions at `0B5E`, `0B95`, `0C8F`, `0D12`, and `0D26`.
- The combat setup routine begins separately at `0D3D`.

The original annotated C contained only the computer-control prompt and then
jumped from `0B95` directly to `0D3D`. The missing range is the game's startup
graphics/timing calibration, shutdown video-mode reset, and DOS critical-error
handler wrapper.

## Computer-control prompt (`0B5E-0B94`)

The routine selects menu configuration 3, redraws the sidebar, and displays:

```text
Do you want the computer to fight for you?
```

It passes the existing 16-bit `Combat_Computer_Control` word as the prompt's
default and returns the prompt result in 16-bit AX. The former C return type was
incorrectly narrowed to a byte.

## CPU and retrace calibration (`0B95-0C8E`)

The routine reads the BIOS 32-bit timer counter through far address `0000:046C`
(the same physical address as BIOS Data Area `0040:006C`). Interrupts are
disabled only while each 32-bit snapshot is taken, preventing a timer interrupt
from changing the two-word value mid-read.

After synchronizing to a tick transition, it sets a target four ticks ahead and
counts how many blocks of 200 empty decrement iterations execute before that
target. It converts the 16-bit block count with signed 32-bit helpers:

```text
CpuTimingCalibration_3FF4 = (SpinBlocks * 455) / 10000
```

The second calibration samples EGA input-status port `03DAh`, bit 3, exactly
10,000 times. Address `3092:32AC` is left at zero unless high samples outnumber
low samples. Retrace-wait routines later use this dominant/inactive status level
to choose which edge order to wait for.

## Graphics runtime initialization (`0C8F-0D11`)

Startup performs these operations:

1. records far address `3092:4614` as the reusable file/decompression buffer;
2. propagates the detected graphics-adapter ID;
3. selects a BIOS video mode from the word table at `3EDB:1140`:
   `04h`, `09h`, `0Dh`, or `13h` for adapter IDs `0..3`;
4. clears the selected display memory;
5. runs the CPU/retrace calibration;
6. calculates signed `BusyWaitScale_5006 = (CpuTimingCalibration_3FF4 - 4) / 6`,
   clamped to at least one;
7. applies adapter-specific graphics setup;
8. installs `0DAB:0D26` as DOS interrupt `24h`, the critical-error handler.

The project now retains all four historical mode-table entries as executable
evidence even though the maintained game path is EGA-only.

## Shutdown and critical-error wrappers

Function `0D12` selects BIOS mode 2 (80x25 monochrome text) and is called from
the main shutdown path at `0D27:0839`.

The small handler at `0D26` passes a zero word to runtime target `207F:3CD8`.
Its installation as interrupt `24h` is proven, but the runtime target's exact
semantic name and return convention remain a `Sol TODO` for the later runtime
library pass. This is a local disassembly task and does not currently justify
an Astra review.
