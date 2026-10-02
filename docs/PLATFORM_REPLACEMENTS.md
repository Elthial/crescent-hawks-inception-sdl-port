# Sol: original method -> SDL boundary

Original methods remain under `Original/`; redirected bodies contain no port
access, DOS interrupt scaffolding or SDL library types. Replacement implementations
are exclusively under `SDL/`, accessed through backend.h.

| Original contract | SDL replacement |
| --- | --- |
| Timer_8253_5 | SDLBackend_ConfigureSpeaker |
| PC_Speaker_ON_ptr_freq | SDLBackend_SpeakerOn |
| PC_Speaker_OFF / PC_speaker_OFF_2 | SDLBackend_SpeakerOff |
| Keyboard_GetKey | SDLBackend_ReadKeyboard |
| Wait_For_Retrace | SDLBackend_WaitRetrace |
| Set_BIOS_Video_Mode_0B73 | SDLBackend_SetVideoMode |
| Get_FileHandle | SDLBackend_OpenFile |
| DOS_read_file_handler | SDLBackend_ReadFile |
| DOS_write_memory_to_save_file | SDLBackend_WriteFile |
| DOS_close_file | SDLBackend_CloseFile |
| DOS_set_file_position | SDLBackend_SeekFile |

File open supports the two observed modes: binary read8000 and binary
write/create8101. Creation does not truncate existing files. Permissions are
host-controlled, not DOS emulated. Unsupported modes return failure explicitly.
The temporary64-entry handle table is an SDL implementation limit, not a
recovered game constant. Counts/error returns retain native WORD representation.

Speaker mode3 synthesis uses the original divisor contract, including0=65536.
SDL streams PCM continuously; off stops generated speaker samples. This is an
initial hardware replacement, not a complete PC-speaker/Tandy emulator or sound
effects conversion. Native noise/gating methods and Tandy paths remain pending.

Keyboard maps ASCII/Shift/Caps and common BIOS navigation scans to the original
signed BYTE-extended WORD values. Other extended keys/code-page handling remain
pending. The higher-level1F3D:0259 routine owns demo/replay logic and must retain
that logic; it must NOT become a wholesale SDL event redirect.

Retrace currently uses a configurable host rate, default70Hz. Rising/falling
port-phase and exact timing remain unverified approximations in SDL, explicitly
not certified original behaviour. The original WORD wait-count loop is retained.
World/BIOS/50Hz clocks must be converted separately, not treated as milliseconds.

Higher-level original drawing and map/asset code are still to be converted.
No new map parser or black-window bootstrap substitutes for those methods.
The platform test main is a test runner, not the game entry point.

## EGA image transfer and tile capture

Original207F:0260 and0313 now redirect to separate SDL EGA methods. The hardware
replacement owns four64KiB planes. NativeA000/A400/A800 segments resolve to
views of that same backing, not unrelated image arrays. Full32000-byte packed
images expand into8000 plane-byte columns; each high/low nibble is one colour.
Transfer preserves inherited sequencer map-mask and raster replace/AND/OR/XOR
selection, sets mode2 and finishes with zero bitmask as the ASM does.
Capture readsA800, eight rows at40-byte stride and four sequential plane bytes
per row, leaving read-map-select3, mode2 and zero bitmask.

These are hardware replacements, not new game rendering algorithms. Legal
native EGA aperture views, rectangles and sufficiently large host arrays are
the current contract. Full EGA controller emulation, latches for other drawing
operations, palette updates and presenting the game display are still pending.
The internal plane-read API used by tests observes memory without changing
controller state; actual capture updates its original read-map-select state.
Tests verify all pixel colours, shared segment views, plane/raster state and
capture ordering/guards. Local original image integration routes all12 decoded
images through0260 and compares every pixel; this is staging validation, not
visible SDL rendering or emulator parity.

Original1F3D:0525 retains its EGA palette sequencing/signed adjustment; only
207F:022A attribute-controller IO redirects to SDL palette-register state.
The backend stores actual six-bit hardware values. No visible RGB conversion
is certified yet;320x200 monitor interpretation remains an explicit follow-up.
Retrace global3092:32AC is now correctly declared WORD, narrowed only at0B40's
actual low-BYTE hardware contract.

Original207F:18EF viewport composition and six native tile transfer methods
now retain their original control flow in Original and call the separate
SDL-type-free mode2/mask0 latch-copy hardware API. This API has an explicit
configured-latch-transfer contract; it is not yet a general EGA CPU-write model.
It shares all four aperture planes with0260/0313 and respects inherited enabled
planes. Map composition writesAC00 staging only; it does not invent an implicit
screen-present call or bypass the original framebuffer-copy workflow.

Original207F:245C/24D7 framebuffer copies now use explicit16-bit offset/segment
views and the same shared EGA planes. Original mode1 is honored, including
inherited enabled planes and unchanged bitmask/raster settings. The latch-copy
boundary supports mode1 and mode2/mask0, not arbitrary other write modes.
Game rectangle clipping/loop/overlap logic stays in Original; no OS-level
memmove substitutes for native forward copying. Visible presentation is still
separate and pending; copying toA000 does not invent a game refresh call.
