# Sol: timer binding, fixed-tone delay and speaker-off

Scope1FC5:046E–04F0, with existing reviewed207F bindings001C/0030/0051.
Evidence: original-version local1FC5 ASM. No deeper noise/sweep changes.

046E calls207F:001C: control port43 receivesB6, configuring PIT channel2,
low/high divisor access, mode3, binary. This alone sets no frequency. Removed
the obsolete100Hz claim rather than treating that comment as a new constant.

047B stores WORD divisor3984 and delay multiplierE48C. If divisor is nonzero,
207F:0030 writes low/high bytes to42 and sets port61 bits0/1. Values are
PIT divisors, not frequencies or pointers. Zero SKIPS the enable/divisor call;
it does not explicitly silence a previously enabled speaker. It is therefore
not necessarily a silent/rest record if reached after an existing tone. The
outer repetition wrapper's later04E4 remains responsible for speaker-off.

04D3 performs fifty passes, each with a fresh WORD BP-2 counter atzero.
04BF reloads delay and calibration5006, signed IMUL produces DX:AX, but only
AX is compared against the counter with signed JG. Preserve low16-bit wrap
and signed comparison: stable low products0001..7FFF iterate that many times;
0000/8000..FFFF skip the inner loop. Each outer pass still occurs. The C
uses explicit unsigned32-bit product followed by low-WORD/signed conversion,
which preserves signed IMUL's low bits without relying on host int width or
signed overflow. The operands remain reloaded in the test, as in ASM.

Named WORD locals DelayPassIndex_61 and DelayIterationIndex_64 preserve the
original Reko suffixes. Argument names retain wArg04/wArg06 identity. Undefined
ds->calibration becomes the named seg3EDB field; no time unit is invented.
This is CPU-calibrated busy waiting, not an exact portable duration promise.

04E4 calls207F:0051, clearing port61 bits0/1. It does not restore PIT mode,
divisor, timer tick rate or prior speaker state. Header declarations record
the reviewed call shapes. Scratchpad C remains non-compilable research.

Synthetic checks compare signed/unsigned multiplication low WORDs for every
delay encoding and nine calibration values, signed iteration-count boundaries,
and all256 port61 bytes. No original audio/hardware timing is executed.

Passed:1,179,904 fixed-tone assertions,65,578 wrapper regression assertions,
131,152 dispatcher regression assertions, and `git diff --check`.

Next:04F1/059A noise/gate loops, followed by0643 divisor sweep and0747/07DA
descending/ascending parameter sweeps in bounded ASM-backed blocks.
