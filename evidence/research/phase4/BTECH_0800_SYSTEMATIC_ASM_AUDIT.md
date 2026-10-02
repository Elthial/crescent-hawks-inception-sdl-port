# BTECH_0800 systematic C-to-ASM audit

Sol: Checked 2026-09-17 against the matching expanded executable listing.
Initial baseline source revision: `197967f`; final comparison baseline: `01ea73a`.
**All 49 retained definitions checked**: 28 locally matched, 20 checked with
mismatches, one checked with unresolved memory bindings. File checking is complete;
functional corrections and runtime validation are not. Checked does not mean matched.

Matching private listing provenance (SHA256):

- `BTECH_0800.asm`: `68A1ED61083CF305C599C1779B2CF955FAD9D78D28D262DF39D91B60010D3CD9`.
- Supplementary `BTECH_0800.dis`: `219DAF280668D0BE0A8C227D84B3D9BADECB26836F7E0B902574C2205C6A5DF4`.

The intermediate listing resolves abbreviated immediates, BYTE widths and the
truncated startup tail; it is evidence from the same decompilation, not an
independent gameplay validation.

## Main_Game_Loop — checked with discrepancies

Entire native entry `0800:0000–051A` compared with the current C body.

| Native block | Accounted-for behaviour |
| --- | --- |
| Entry / idle scheduling | WORD input-disable argument, exit reset, intro animation, signed countdown, retrace wait and world-tick gate |
| Input and movement | Keyboard read/drain/conversion, menu layout, movement repetition, entity drawing and message interruption |
| Compass and fog | Eight WORD commands and FAR text pointers; message reset, animation/pause; eight mapper rows or current/adjacent fog bytes |
| Recovery / patch countdown | Saturating recovery BYTE; low-byte decrement with borrow, cached low OR high, coordinate-gated restoration |
| Encounter / cooldowns | RNG call order and mask test, Citadel/cache gates, four saturating BYTE countdowns |
| Finance / stocks | Old D323 test, four DWORD additions, allowance payment and stock freeze, three mask/factor pairs, unsigned multiply/divide and BakPhar quartering |
| Tick completion | Animated tiles, countdown reload, old NPC phase test/reset, live-view rendering |
| Terminal states / return | Required-character loss text, victory replay prompt, new-game load or exit flag, outer-loop exit and FAR return |

### Discrepancies requiring a later approved correction

1. **Wealth comparison signedness.** At `0333–0342` and `0364–0377`,
   ASM orders the high WORD with signed branches, then the low WORD unsigned.
   C declares both balances `unsigned long` and compares unsigned. With allowance
   50 and pre-payment wealth `0x80000000`, native ordering allows the 15 C-Bill
   payment and stock updates; current C suppresses payment and freezes stocks.
   Below bit31 the order agrees. Ordinary gameplay reachability is not established;
   this is a C-to-ASM mismatch, not a verified original-game bug.
2. **Movement repetition WORD semantics.** At `0070–00A1`, ASM tests a
   signed WORD counter against the WORD setting using `JL`, with WORD increment
   and interruption arithmetic. C combines an `int` with an unsigned-short field.
   Normal settings 1–4 agree; bound `0x8000` causes zero native iterations but
   a positive unsigned C bound. Host-int width also changes wrap behaviour.

Only comments were added: operative expressions remain unchanged pending review.

The expanded checkpoint also compared embedded DS strings: the native loss
message at `00FB` has two spaces after `died.`, but C has one. Record this minor
text discrepancy alongside the existing arithmetic findings; no string was fixed.

### Explicit contracts and abstractions

- Compiler frame setup, stack probing and FAR-return mechanics are abstracted.
- DWORD arithmetic requires 32-bit wrap, not a host-dependent `unsigned long`.
  Stock runtime helpers are treated under their unsigned DWORD contracts; their
  bodies are not newly confirmed by this caller review.
- Native scheduler/stock-loop storage reuse is split into private C temporaries.
  Neither escapes; the scheduler is reloaded before the next idle iteration.
- Fog preceding/following rows are DS-offset aliases, not assurances that a
  standalone host array may safely be indexed outside its declared bounds.
- BYTE sign extension of encounter/fog masks does not change the low-byte result:
  the random return is zero-extended, and fog stores retain only AL.
- Near death-name strings represent DS text passed to the shared renderer.
- D343:D345 is locally a Starport-patch countdown; its story-purpose hypothesis
  is not confirmed by this block. Training cooldowns D320/D321 are separate.
- Callee implementations, actual rendering and gameplay timing are not certified.

## Four fully matched local helpers

| Method | Native range | Complete local comparison |
| --- | --- | --- |
| `Play_Sound_If_Enabled` | `19BF–19DC` | Test WORD sound setting; if nonzero pass WORD ID to sound setup; no ID validation |
| `Rand_Dice_Two_D6` | `19DD–19F2` | Call D6 twice, retain first result, return sum 2–12 |
| `Rand_Dice_D6` | `19F3–1A12` | Mask random return with 7; reject 6/7 by repeating; increment accepted 0–5 |
| `Get_CBill_Allowance_Wealth_Limit` | `29F5–2A2A` | Combine D33F low BYTE and D340 high BYTE; return zero-extended WORD in DX:AX |

Stack probes/prologues are abstracted. Sound setup and RNG are callee contracts,
not additional fresh confirmations. The D6 signed `JG` is equivalent to the C
test because masking restricts the value to 0–7.

## Twenty additional whole-body comparisons

All listed local paths were read through their returns, not merely matched by
name. Compiler prologues, stack probes and register saves are abstracted. Callees
remain contracts, not fresh certifications of other files.

| Method / native extent | Fresh result | Accounted-for operations |
| --- | --- | --- |
| `Draw_Infantry_And_Mechs`, `051B–0E4A` | Checked - mismatch | Effects gate; twelve active WORD clears; eight party infantry; eight NPC records; four mechs; all map/parity masks, clipping calls, formation staging/restoration and four jail overlays |
| `Draw_Menu_MultiSelect_0E4B`, `0E4B–1731` | Checked - unresolved binding | Effects; 24 visible BYTE clears; two infantry groups; two mech groups; packed projection and page guards; tile/auxiliary/anchor writes; eight-entry parallel-array collection, signed Y sort, draw offsets and jail overlay |
| `Advance_Combatant_Animation_Stream_1732`, `1732–17BA` | Fully matched under FAR16 contract | Signed BYTE tokens; offset advance; FD selector/extra consumption; FE signed backward distance without consumption; FF rewind/poll; other negative skip and nonnegative return |
| `Move_Map_View_To_Packed_Position_17BB`, `17BB–1816` | Fully matched | Unsigned target/current comparisons; north/south then west/east calls until each axis reaches its target |
| `Move_Map_View_By_Signed_Delta_1817`, `1817–186E` | Fully matched | Decoded raw unpromoted bytes: signed negative gate, north/west increment-to-zero; south/east decrement-to-zero; Y before X |
| `Combat_Move_Position`, `186F–191A` | Fully matched | Unsigned target loops; WORD decrements/increments; bit7 north/west masks and south/east additions; no streaming calls |
| `Offset_Packed_Position`, `191B–19BE` | Fully matched | Signed delta gates and consumed stack copies; same four packed-boundary operations; Y before X |
| `Prompt_Yes_No`, `1A13–1AFC` | Checked - mismatch | Default normalisation; colour save/restore; Y/y/N/n, left/right, Return/Space cases; row decrement/redraw even on confirmation; selected Boolean return |
| `Decode_Draw_Next_ANM_Frame`, `1AFD–1C11` | Checked - mismatch | Decoder FAR argument order; retained EGA conversion/draw; offset-only consumed-byte addition; signed timing lookup/multiplications/shifts; retrace call and frame WORD increment |
| `Update_Animated_Map_Tiles_240B`, `240B–24C1` | Checked - mismatch | Tileset gate; unsigned increment/reset at three; frame offset; ten tile BYTE lookups, SHL5 destinations and source advances by0180; EGA transfer arguments |
| `Display_Text_At_2867`, `2867–28A1` | Fully matched | FAR text argument; WORD column/row writes, white colour and renderer call |
| `Set_Text_Colour_Bright_Green_28A2`, `28A2–28CB` | Fully matched, EGA path | Colour WORD0A; intentional CGA palette1 branch omission |
| `Select_Game_Disk_And_Drive_28CC`, `28CC–2912` | Checked - mismatch | Requested disk WORD store; hard-disk bypass; drive A call, second-drive flag and conditional B call |
| `Request_Game_Disk_2913`, `2913–29F4` | Checked - mismatch | Disk selection; saved/restored menu; sidebar, palette and all prompt pieces; drive-letter BYTE update; drain/read; saved key return |
| `Drain_Pending_Keyboard_Input_2A2B`, `2A2B–2A4E` | Fully matched | DisableInput WORD bypass; pending-input polling and repeated blocking reads |
| `Prompt_And_Wait_For_Key_2A4F`, `2A4F–2A68` | Fully matched | DS:050D literal including carriage return; blocking key read |
| `Display_Plural_Suffix_2A69`, `2A69–2A7D` | Fully matched | DS:051B NUL-terminated `s` renderer call |
| `Display_Sentence_Period_2A7E`, `2A7E–2A92` | Fully matched | DS:051D NUL-terminated period renderer call |
| `Game_Pause_Menu`, `2C50–2DA7` | Checked - mismatch | Three layout WORD initialisers; four live-mech tests; expanded layout; all prompt pieces; both menu dispatches including jump-table order, heal argument0 and final redraw |
| `Load_And_Draw_ANIMATE_ICN`, `320B–32B2` | Checked - mismatch | Entire native disk/setup/load/decompress/conversion/copy/base-load/restore sequence compared; omissions and size issue below |

### Additional findings / correction queue

1. **Exploration NPC X guard is wrong.** `.asm` abbreviates the signed
   immediate as `8Dh`; `.dis` at `0848` preserves `FF8D` (-115).
   C uses `008D` (+141). With NPC position equal to camera, projected X is26
   and Y12: native admits it, while C rejects it. Other jail/combat guards already
   use the correct negative value; do not change those.
2. **On-foot assignment BYTE signedness.** `05BE` and `0BFE` use signed `JL`
   against8. Native skips raw assignments80–FF; unsigned `Piloting >= 8`
   includes them. Known valid assignments0–8 agree. NameFF dead checks are separate.
3. **Combat compositor bindings remain incomplete.** Native DS:0364/039E
   are biased WORD lookup views and246C:0795 is a BYTE map view. Their current
   `ds->a0364/a039E` and `seg246C->w0795` names have no declarations in BTECH.h.
   Do not infer a WORD read from the `w` prefix: `1310` reads AL. The infantry
   position slice also intentionally reaches beyond its declared eight entries
   to alias the enemy slice; the future memory view must represent this explicitly.
   The local instruction flow otherwise corresponds, including all four sort
   arrays, BYTE sprite-sum truncation and anchor-minus(8,16) draw arguments.
4. **Yes/no text pointers are bare offsets.** Native pushes DS and03EE/03FE;
   C passes WORD constants to a FAR-pointer parameter. This needs a named DS text
   binding, not integer-to-host-pointer conversion. The selection state machine
   itself matches. The unused saved row local is deliberately omitted.
5. **Animation timing needs explicit low-WORD truncation before shifting.**
   Native BYTE IMUL yields AX; multiplying by3 retains AX and performs two SARs.
   C shifts a promoted expression before assigning to signed short. Example
   multipliers127 and127: low WORD after multiplying by3 isBD03 (-17149);
   native delay is-4288, while a 32-bit-int expression gives12096. Native-style
   16-bit signed overflow is also not a portable C guarantee. Shipped timing
   values need separate domain validation; no claim of a reachable gameplay fault.
6. **Tile destination sign extension.** Native CBW precedes SHL5. For raw
   tile80, native destination isF000 versus C1000 after WORD reduction. All ten
   shipped04B0 IDs57,58,59,5A,5B,69,6A,6B,6F,71 are below80 and agree.
7. **Disk number comparison.** `28FF` uses signed `JLE 1`; unsigned C selects
   B for8000–FFFF while native stays on A. Valid disk IDs1/2 agree. This is a
   replacement-platform routine, but its caller contract must still be recorded.
8. **Disk prompt loses embedded control bytes.** Native string04EF ends with
   `06 0F` before NUL. The C literal omits that renderer-control tail. Palette-CGA
   omission is intentional; loss of text-control bytes is not documented as such.
9. **Pause text differs in case.** Native0543 spells `'Mechs`; C spells `'mechs`.
   Both dispatch tables and layout mutations agree; this is a minor text mismatch.
10. **ANIMATE loader omits operations.** Native selects disk2, calls207F:00D1
    with FAR2FE8:0150, sets WORD4FBC=1, performs loading, then loads the base
    tileset and selects disk1. The C omits initial/final disk calls, setup call and
    load-state store. Follow-up caller-contract inspection of207F:00D1 confirms
    it returns immediately for adapter2: its CGA setup omission is intentional,
    not a retained-EGA mismatch. The disk and load-state omissions remain. Native conversion/copy counts0780 WORDs mean
    0F00 bytes, covering ten0180-byte groups at D582–E481. C's comments, existing
    animated-tile documentation and `AnimatedMapTileFrames_D582[0780]` header
    declaration incorrectly describe/size this as0780 bytes; they need a scoped
    follow-up correction. The call counts themselves are correct.

FAR cursor arithmetic in1732 must change only the16-bit offset, leaving the
segment untouched. Matched movement routines likewise require native WORD wrap;
they do not certify arbitrary host-width pointer/arithmetic substitutions.
EGA-only adapter branches are intentionally omitted in both compositors, the
animation renderer, green colour helper, disk prompt and tile updater.

The native DS string bytes used above were checked in `BTECH_3EDB.asm`; signed
immediate ambiguities were resolved in `BTECH_0800.dis`, not guessed from the
abbreviated ASM printout. In particularFFFF sentinel checks in0EEA/11B8 and
FF8D/FF8B projection guards must not be mistaken for00FF/positive bounds.

## Remaining 24 whole-body comparisons — file complete

Compiler stack probes/prologues and register saves are abstracted throughout.
The native adapter0/3 alternatives intentionally absent from the maintained EGA
code are not restoration tasks. No callee is newly certified merely by a call.

| Method / native extent | Fresh result | Accounted-for operations |
| --- | --- | --- |
| `Draw_Terrain_Clipped_Sprite_0800`, `extracted clipping blocks` | Fully matched (extracted retained EGA block) | FAR sprite lookup, offset+1 wrap, BYTE shorten/draw/restore and AC00 framebuffer; not an independent native entry |
| `Map_Interactables_Building_Or_Items`, `1C12..218E` | Checked - mismatch | all boundary/terrain checks, on-foot object dispatch, four-mech footprints, building prompt gates and blocked return; CR text, signed threshold and WORD position-add discrepancies |
| `Character_Movement_On_Map`, `218F..231C` | Checked - mismatch | eight directions, interaction gates, Y/X streaming, three queue clears per moved axis, destination reset and unconditional neighbourhood/actor rebuild; map BYTE CBW differs |
| `Update_Friendly_Movement_Animations_231D`, `231D..240A` | Checked - mismatch | eight direction choices, first-match exit, four present mechs and eight infantry cursor/frame updates; near temporaries drop3092 FAR segment |
| `Update_Roaming_Map_Npcs_24C2`, `24C2..2866` | Checked - mismatch | eight1A-byte records, pin/delay/waypoint paths, camera scratch bounds, signed projection, six-WORD Position call, animation, arrival and final restore; near pointers and BYTE indices differ |
| `Draw_Persistent_Map_Effects_2A93`, `2A93..2C4F` | Fully matched (retained EGA path) | 64 effects, packed page/coordinate projection and guards, SHL3 pixels, FAR sprite lookup, drawn-only fire XOR and EGA draw; intentional other-adapter omission |
| `DOS_Load_Map_Files`, `2DA8..320A` | Checked - mismatch | thresholds, filename/disks/tileset, retry and metadata/payload reads, grid descriptors, Starport patch, NPC initialization or clear; FAR temporaries and signed map comparisons differ |
| `Load_Game`, `32B3..35D2` | Checked - mismatch | slot/cancel, filename/disks/INFOCOM, handle/marker paths, save block/position restoration, file close, mech visual reset and common UI exit; missing menu rows and CBW differences |
| `Save_Game`, `35D3..378C` | Checked - mismatch | map-room block, six slots/cancel, filename, disk/INFOCOM, create permissions, four checked writes, close/warning and common exit; menu literal lacks slot rows |
| `Inspect_Characters`, `378D..3BCF` | Checked - mismatch | optional mech view, all character/name/equipment/health/traitor/armour/skill paths and redraw; near character/skill pointers and signed BYTE indices/clamps differ |
| `Menu_Change_Game_Settings`, `3BD0..3D3F` | Fully matched (retained EGA path) | settings/cancel dispatch, movement1/2/4 and map-room override, speed, sound XOR, outtake, two quit prompts, exit and sidebar; exact DS strings |
| `Show_Overhead_Map`, `3D40..3FAD` | Checked - mismatch | entry/exit cache swaps, reveal policy, marker/page navigation with all bounds, restored viewport, neighbourhood reload and final draw/flags; map BYTE CBW differs |
| `Overhead_Map_Draw_3FAE`, `3FAE..45C1` | Checked - mismatch | 5x3 regions and3x3 neighbours, dynamic descriptor assembly, file load, reduction, fog/tile draw, objective/footer, blinking marker,601 attract polls and restoration; near filename and CBW offsets differ |
| `Build_Dynamic_Overhead_Tile_45C2`, `45C2..4620` | Fully matched (retained EGA path) | packed tile reduction then adapter2 in-place EGA conversion of10 WORDs; intentional adapter0 conversion omission |
| `Load_And_Draw_BTTLTECH_ICN`, `4621..46A6` | Fully matched (retained EGA path) | CGA-only00D1 omitted, flag1, disk2, load/decompress, adapter2 A400 transfer and tileset0; FAR source/destination and WORD segment contract |
| `Load_And_Draw_BTTITLE_CMP`, `46A7..476C` | Fully matched (retained EGA path) | EGA palette2FE8:0000, disk2, CGA-only setup omission, load/flag0/decompress, A800 segment transfer,320x200 draw and tilesetFFFF; adapter3 branch intentionally omitted |
| `Load_And_Play_Intro_Music_476D`, `476D..48B6` | Fully matched (native WORD/FAR contract) | disk/open retry, seek/read/close, four zeros, timer install, Tandy/speaker reset/start cadences, pending/completion poll, stop and restore; unused033C stack WORD abstracted |
| `Display_Animation_Scene`, `48B7..4AA5` | Checked - mismatch | filename, random/forced playback, disks, optional probe, clear/cursor/frame loops, siren seven replays and redraw; .dis provesFFFF probe vsC00FF, signed gates/indices differ |
| `Draw_Character_BDC_Sidebar_Row`, `4AA6..4BC0` | Fully matched (retained EGA path) | FAR name lookup, row pixels, signed BYTE body/health IDIV10 and min1, BDC bars and red health-damage rectangle; native standalone entry, not added helper |
| `Draw_BDC_Attribute_Bar`, `4BC1..4CAB` | Fully matched (native WORD contract) | WORD column shift and12-value subtraction, four yellow edges and green interior; native standalone entry, not added helper; adapter0 colour omission |
| `Draw_Health_and_C_Bills_Sidebar`, `4CAC..4D56` | Fully matched (retained EGA path) | menu/border3, optional top draw, BDC heading, eight record scan at most four live rows, cash and final menu/border4 |
| `Menu_Assign_Pilots`, `4D57..4DC6` | Fully matched (native WORD/BYTE contract) | nine terrain BYTE bit80 tests, selector mode0, sidebar/top redraw or exact warning/key; all branch paths |
| `Load_Game_Map_Data`, `4DC7..50C7` | Fully matched (native address-view contract) | destructive new-game state/skills/mechs/security/economy/fog/sprite/effect reset, RNG order, Citadel/grid/render setup,24-entry animations and story/runtime flags; FAR/address aliases explicit |
| `Start_Game`, `50C8..527B` | Fully matched (retained EGA path) | 256 random bytes, flags/animated tiles, startup music, input branches, DEMOFILE deterministic replay, new-game/questions/load/play, title/music repeat and final .dis return |

### Findings from the final pass

1. **Interaction prompts lose a carriage return.** Native DS:0430/0446
   terminate `Will you enter the\r`; the current literal ends with a space.
   All object-code branches and both building loops were compared. Native
   on-foot1E6F and mech215E use signed threshold comparisons; mech1FE7 uses
   unsigned. Do not unify their signedness without an explicit correction.
   Exact-position sums also need low-WORD reduction before comparison.
2. **FAR storage is cast to near temporaries.** Friendly animation tables,
   roaming NPC record/parallel-table views, map cache pointers, character/skill
   views and the overhead filename access3092 or246C in native code. Their
   current near declarations/casts cannot establish that segment in a16-bit
   DS3EDB binding. A flat host memory-view abstraction could replace them, but
   must be explicit; these are not already correct DOS FAR bindings.
3. **BYTE CBW is meaningful.** Movement/load/overhead entry paths sign-extend
   map-file BYTEs before passing WORD arguments. NPC direction/link indices,
   saved outtake selection and overhead metadata are also sign-extended.
   Known valid map IDs, directions and settings agree; raw80–FF do not.
   In the character viewer signed skill `JLE4` accepts a negative extended
   value, whereas unsigned C clamps it to4. Do not call these reachable game
   faults without establishing their input domains.
4. **Save/load menu literals are incomplete.** DS:05C7 and0673 include the
   headings `Load Game:` / `Save Game:` followed by carriage-return-separated
   One, Two, Three, Four, Five, Six, Cancel. C prints only `Load Game` /
   `Save Game`. The native slot dispatches, save layout and write-count checks
   otherwise correspond. Failure and Cancel common exits were included.
5. **Optional animation probe is a transcription mismatch.** Native49A9
   compares `FFFF`, explicitly preserved in `BTECH_0800.dis`; the abbreviated
   ASM prints `FFh`. Current C compares `00FF`. The earlier scene-controller
   note and source explanation have been corrected, but the operative C test
   is untouched. Native closes the returned handle before checking it.
6. **Map width/height shifts are BYTE operations.** The intermediate listing
   records BYTE shifts at the header-reduction step. Apparent WORD shifts in
   an abbreviated/untyped instruction rendering are not evidence that stale
   stack upper bytes affect dimensions. No uninitialised-width bug is recorded.
7. **New-game loader is destructive, not a restore helper.** `4DC7` resets
   the persistent state, initial cash20, allowance50, Jason and Citadel. Saved
   restoration is in32B3. Its human summary has been corrected. Native random
   even mech-family branches leave old entries untouched, matching C; whether
   that affects a new game needs an initial-memory/runtime comparison.
8. **Native BDC entries were misclassified.** `4AA6` and`4BC1` really are
   standalone FAR entries, not added research helpers. Inventory mapping now
   names those addresses. The clipping helper remains an extracted sequence,
   with no independent native entry.
9. **Boundary arithmetic still needs a port binding.** All locally matched
   rows assume native BYTE/WORD wrap, signed BYTE promotion where written and
   FAR offset-only arithmetic. Music terminator indexing and BDC rectangle
   arithmetic are not blanket endorsements of32-bit host-C expressions.
   Negative marker-coordinate shifts in the overhead renderer need defined
   WORD arithmetic instead of portable-C signed-shift assumptions.

### Native error-path observation, not a transcription fix

Load-game invalid-marker path334F..3382 warns and proceeds to34D3 visual reset
without closing the successfully opened save handle; both ASM and C have that
path. This is a static resource-leak candidate on invalid saves, not a validated
gameplay failure. No error-path test was run. Short reads are not checked by the
native loader either; valid-input success does not validate truncated files.

### Caller contracts checked only as needed

- 207F:00D1 tests adapter0 before table rebuilding and returns for EGA2.
  Its omissions in map/title/base/scene/startup code are intentional EGA
  abstractions. ANIMATE still omits disk selections and a flag write.
- 204B:033C reads the callback offset in CS and returns completion without
  reading the caller's pushed WORD1. Its no-argument C abstraction is valid.
  The last XOR/return fragment appears at the start of207F.asm; checking this
  contract does not promote204B to whole-file confirmation.
- The .asm startup tail stops at526D. Intermediate5273 branches to5126;
  5276 restores SP/BP and returns. Both paths are represented by the C loop.
- NPC destination/position record fields occupy offsets0..9 within a1A stride;
  the other10h bytes are not newly interpreted.

C changes in this pass are comments only, including correcting factual size
and sentinel explanations. Functional discrepancies remain queued for review.
All49 source-body hashes are recorded in ASM_AUDIT_RECORDS.json.

## Checkpoint verification

- Method-inventory verification: 312 definitions, 340 checks; only C comments changed.
- `Verify-0800MainLoopAudit.ps1`: 23 boundary-model checks, including bit31 wealth
  and signed movement-bound witnesses. These models do not execute the ASM.
- Expanded checkpoint: nine further model/count checks for the NPC guard,
  assignment signedness, timing truncation, tile destination, disk signedness,
  animated-tile extent and explicit method status totals.
- Final checkpoint adds probe-sentinel, signed skill/map and WORD-position
  witnesses, verifies all49 unique records, and checks the two native BDC mappings.
  The final run passed23 original boundary checks and16 whole-file model/count
  checks. Method-inventory verification passed340 checks for312 definitions;
  startup/music contracts passed187656 synthetic assertions. No gameplay run
  or compilation of the research pseudo-C is claimed.
- Existing startup/music contracts: 187656 synthetic assertions passed.
