# Sol: sound setup and WORD-stream dispatch

Scope1FC5:0002–02A2 only. Original-version local BTECH_1FC5.asm is evidence;
the already-transcribed EXE table is documented in data-structures/SOUND_EFFECTS.md.
Playback helpers02A3 onward are deliberately deferred: their damaged repetition
and register-derived loops need separate ASM reconstruction.

Sound ID is a one-based WORD; DEC WORD converts it to the number of effects to
skip, with native wrap forzero. Cursor BP-4 is a WORD INDEX, scaled by SHL1
before every read from DS:5008. SoundLibrary is313 WORDs, not BYTEs. A BYTE
cursor silently wraps after255; WORD cursor and WORD repetition scratch value
are now explicit. No new malformed-ID guard or normalization was introduced.

Traversal first checks current WORD nonzero, then whether the number remaining
iszero. Following WORDs index+1/index+2 bothzero identify an effect separator;
the old C used+2/+4 as though these were byte offsets. Playback stops on the
same following-WORD test. Actual table delimiters are1,0,0; currentzero stops
outer traversal. A zero command with nonzero following parameters is not
treated as an unconditional playback stop.

Signed CMP/JG1000 selects direct records<=1000 (three WORDs).1001 uses six,
1002/1003/1004 seven; unknown positive commands advance one WORD. High-bit
command values compare as negative and therefore enter the direct family,
not the unknown-command path. Normal table commands are unaffected by this
explicit preservation of native signedness.

Playback loads command into4612. Direct records load3984/E48C and call02A3.
1001 loads repetition4612 plus006C/3776/4312/398A and calls02EB;1002 loads
repetition plus0000/39F4/4000/4034/0062 and calls0345.1003/1004 retain their
command kind while loading a separate WORD repetition scratch, followed by
398C/39A2/39F6/3FF2/009C; then repetition is assigned to4612 and03A8/040B
is called. Unknown kinds consume only their command WORD in playback.
The current globals retain neutral native-offset names pending helper review.

The0800 sound-enabled gateway already forwards the original WORD ID unchanged.
Toolkit extraction and existing sound documentation already know the WORD table;
this audit aligns the annotated C without changing portable synthesis or copying
external music/assets. Exact hardware/timing remains a later verification boundary.

Synthetic checks cover every encoded command WORD's signed width, WORD repeat
preservation and mixed synthetic traversal beyond255 WORDs. These are semantic
models, not execution of the original dispatcher or sound playback.

Passed:131,152 synthetic assertions and `git diff --check`.

Next:02A3–046D five repetition wrappers, then low-level sound/timing helpers.
