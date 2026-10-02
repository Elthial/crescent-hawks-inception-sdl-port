# Sol: final 1FC5 caller and field consistency review

Sol: Fresh confirmation is in [the systematic1FC5 audit](BTECH_1FC5_SYSTEMATIC_ASM_AUDIT.md).
All15 routines now have explicit body-hash-bound records. The missing final
ascending comparison is confirmed by this profile's .dis and expanded EXE;
the earlier full-listing attribution below is not needed as fresh evidence.

The first-pass annotation audit covers all fifteen maintained functions in
`Btech/BTECH_1FC5.c`, from `0002` through `07DA`. No new gameplay behavior or
external asset changes are introduced by this closing review.

Evidence is this project's unpacked EXE output: `BTECH_1FC5.asm`, the caller
in `BTECH_0800.asm`, and shared primitives in `BTECH_207F.asm` under
`Btech-Reko-expanded/BTECH.reko`. The full `BTECH_code_0000.asm` supplies
the final ascending comparison omitted from the per-segment listing.

## Caller and dispatch contracts

`0800:19BF` tests the sound-enable WORD at DS:015C and forwards the unchanged
one-based WORD ID through its FAR call at `19D3` to `1FC5:0002`. This is the
only maintained external caller of the segment. Setup decrements the ID;
there is no native ID/bounds validation or asynchronous sound queue.

| Record | Total WORDs | Wrapper | Playback helper |
| --- | ---: | --- | --- |
| Signed command <=1000: repetitions, divisor, delay | 3 | 02A3 | 047B fixed PIT tone |
| 1001: repetitions plus four noise parameters | 6 | 02EB | 059A seeded speaker toggling |
| 1002: repetitions plus five divisor-sweep parameters | 7 | 0345 | 0643, then 06F6 |
| 1003: repetitions plus five noise-mask parameters | 7 | 03A8 | 0747 descending, then 04F1 |
| 1004: repetitions plus five noise-mask parameters | 7 | 040B | 07DA ascending, then 04F1 |

The next two zero WORDs identify a separator, not an unconditional test of
the current command. Traversal separately stops on a current zero WORD.
High-bit command values take the signed direct-record branch; do not replace
the comparison with an unsigned one. Unknown positive extended commands
advance by one WORD. All five wrappers set timer mode and finally switch
the speaker off, even when their signed repetition count skips playback.

## Shared fields and hardware handoff

All nineteen `seg3092` sound fields have descriptive names in both their
declarations and every maintained use. Their original `w` names/offsets
remain in header comments. The fifteen function declarations match their
definitions, including WORD argument widths. The old header warning that
playback helpers were still unaudited is now superseded.

`4612` is deliberately command-or-repeat scratch, not a pointer. `0000` in
this segment's scratch view is the divisor-sweep centre, unrelated to the
other segment's same-offset view. The header remains overlapping memory
research, not a compilable sequential structure.

`207F:007D` leaves an evolved WORD in DX; `00A9` consumes that implicit
register state as `(DX & mask) | minimumBits`. A C# port must make that
handoff explicit rather than treating these as independent stateless calls.
Timer setup `046E` binds `207F:001C`; tone writes bind `0030`; speaker-off
binds `0051`; noise initialization/toggling/countdown bind `0067/007D/00A9`.

The dispatcher batches some local cursor increments after a helper call,
where ASM increments before each parameter read. The cursor is a private
BP-4 WORD, not shared helper state; current valid-record parameter/call
ordering is preserved. Host pointer arithmetic must still explicitly model
native WORD/address wrap when implementing the replacement, not assume
these research array expressions validate arbitrary malformed streams.

## Verification and remaining limits

`Verify-SoundFieldConsistency.ps1` guards declarations, nineteen WORD fields,
the enable/ID gateway and implicit DX binding. Run it alongside the six
existing dispatch, repetition, fixed-tone, noise-loop, divisor-sweep and
noise-parameter-sweep verification scripts.

Passed all seven scripts: 3,801,726 existing synthetic assertions plus 39
caller/field source checks. `git diff --check` also passed.

Synthetic/source checks are not original-game execution or proof of audible
fidelity. CPU-calibrated waits, PIT/port behavior, malformed-stream termination
and zero-step hangs remain native behavior requiring separate port decisions.
Fixed-tone divisor zero skips the divisor write; the sweep helper writes zero
as a PIT divisor encoding 65536. Do not silently merge those cases.

No expensive model confirmation is needed for the caller/field contracts
resolved directly against ASM here. Toolkit playback fidelity remains a
separate task; this review does not claim to certify its renderer.
