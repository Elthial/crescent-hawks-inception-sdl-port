# Sol: TODO review, 2026-09-17

## Scope and evidence

Reviewed all 57 case-insensitive `TODO` matching lines in `Btech`, `docs`,
`InceptionTools` and `scripts` before this update. Multi-line comments are
grouped below; 57 lines are not 57 independent tasks. No TODO matches were
found in maintained InceptionTools/scripts. Build outputs, original assets,
clean Reko output and the imported UnBattletech snapshot are not work queues.
`CHECK ASM`/`FIXME` were checked too: the remaining CHECK ASM mentions describe
completed audits or historical next steps, not active unchecked source markers.

This review closes settled wording, not runtime uncertainties. No gameplay
logic or original bug was changed. Historical chronological audit sections
remain as history; current dispositions below supersede their old next steps.

## Source-comment dispositions

| Location / group | Current disposition |
| --- | --- |
| `1F3D:070A` sprite-reader follow-up | **Closed:** all eight0800 readers reconciled; source comment updated. |
| `11B8` weapon-repair menu return width | **Closed:** helper/header now return WORD; source comment updated. |
| `11B8:0CBA–0D30` repeated10h weapons | **Partly closed:** six separate firing entries are established, not hidden two-weapon grouping. Anatomy remains A-005; comment narrowed. |
| `0FDC` disabled-Locust actuator indexes | **Partly closed:** +24/index0 is left, +25/index1 right, low nibble leg/high arm. Individual joint bits remain open; comment narrowed. |
| Header `4004/4036/406A` table extents | **Closed extent question:** canonical24-WORD views already exist; comments updated. Converting all historical aliases is separate cleanup, not a need to re-prove extent. |
| Header `MechWeapons` mask alias | **Actionable locally:** migrate component reads to `MechComponentIdMask`; repeated-attack count deserves its own domain name despite the same7F value. Retain the compatibility alias until all uses are reviewed. |
| `1631:10A2` misleading ammo helper name | **Actionable locally:** returns weapon-table index/FF, not ammo. Rename declaration/definition/all callers together, retaining10A2. Do not add ammo bounds or change ordinal behaviour. |
| Header D390 contiguous-array aliases | **Actionable locally:** canonical strided view exists; inventory remaining legacy callers and migrate proven field views. Do not invent ownership for the16-byte gaps. |
| `1AE8:12C7` legacy parameter suffixes | **Documentation/traceability:** true BP offsets are already recorded; preserve numeric Reko suffixes. They need not equal native BP offsets. Boolean consumers were subsequently named. |
| `1AE8` VisibleProjectileTicks uninitialised BP-4 | **Port policy, not current EGA fix:** only deleted adapter1 consumes its low two bits. No invented initialisation or original-game bug claim. |
| `1543` infantry existing-target high-bit invariant | **Actionable local audit first:** inventory writers/callers of the target BYTE; runtime needed if encoded values can reach the unmasked read. Do not silently mask it. |
| `1467:0838` DS/SS row-map accesses | **Runtime A-003:** trace loader segments and effective addresses. |
| `1467` roster slot versus NameId | **Runtime:** use a roster with different slot/name IDs; do not relabel the stored value from intuition. |
| `1467` zero-row mech menu | **Narrowed:** 1E56 menu helper has now been audited; it has no zero/negative-row guard. Next is caller reachability and observed empty-lance behaviour, not another helper-width review. |
| `0FDC` alternate-BLD index bounds | **Future port safety:** shipped callers locally restrict remapped IDs; native comparison admits10h/11h. Preserve native evidence and add explicit host bounds policy later. |
| `0FDC` last-waiting-NPC condition | **Runtime A-007:** preserved literally; need multiple waiting NPCs and route/flag capture. |
| Four `0DAB` critical-group anatomy comments | **Open A-005:** +33 is established as contiguous raw base; physical labels remain disputed. Do not let current C member names settle it. |
| `0DAB`207F:3CD8 critical-error target | **Deferred platform/runtime plumbing:** semantic/return contract unresolved, but no reason to reconstruct DOS internals before gameplay evidence. |

## Documentation-only / historical dispositions

| Earlier TODO mention | Current disposition |
| --- | --- |
| `0800_COMBATANT_SPRITE_COMPOSITOR`, `0800_DRAW_EXPLORATION_SPRITES` | FAR readers now corrected; stale active paragraphs refreshed. Other older blocks remain history. |
| `0800_MAP_LOADER_SETUP` removed adapter helper | EGA-only policy; do not restore deleted non-EGA colour conversion. |
| `0DAB_BTSTATS_REDRAW_AND_EXIT`, `1CD3_POSITION_STAGING_AND_ENDING` | Palette FAR contract resolved by later1CD3/1F3D review; current-note added. |
| `0DAB_HARDWARE_INITIALIZATION` | Critical-error target still deferred as above. |
| `0FDC_BLD_INTERACTION_ENTRY` | Bounds warning remains relevant to future callers, not a new shipped-code fix. |
| `11B8_MECHLUBE_WEAPON_REPAIR` | WORD menu signature resolved; stale paragraph refreshed. |
| `1631_SEGMENT_REVIEW` | Misleading10A2 name is actionable local rename, not a semantic uncertainty. |
| `COMBAT_MOVEMENT_AND_WEAPON_LINKAGE` legacy boolean/suffix note | Later effects audit named booleans; suffixes retained intentionally. |
| Same document's apparent shooter-frame/projectile-coordinate alias | Later sound/travel audit resolves it as Reko noise; not an original bug. |
| `207F_SEGMENT_REVIEW` stock-pointer TODO | Later consistency and1CD3 audits resolve the DWORD value/stock-index calls. |
| Same document's border/tile/FAR framebuffer caller TODOs | Later1E56 descriptor and EGA caller review resolves the named border/box paths; current-note added. This does not imply every renderer caller elsewhere is correct. |
| Same document's numeric formatter record-index TODO wording | No corresponding literal active source marker remains. Audit callers by contract if revisiting; historical prose is not an outstanding ABI defect. |
| `1CD3_FINAL_CONSISTENCY` obsolete palette TODO mention | Already explicitly reported resolved; no new task. |
| `ANM_PIXEL_LAYOUT` legacy converter migration | Historical extractor is replaced. No maintained code reference to `Write2ModeConverter` was found; current-note added. |

## Next bounded work

1. Mechanical10A2 helper/mask naming and proven legacy-view callers; retain
   numeric suffixes and old-name comments. Verify no missed callers.
2. Local target-BYTE writer inventory and zero-row-menu reachability review.
3. Live post-unpack address mapping, then startup capture/input/menu traces.
4. Scenario-specific runtime questions from
   [Astra queue](ASTRA_REVIEW.md) and [bug register](ORIGINAL_GAME_BUGS.md).

None of these requires an expensive-model run merely to tidy names or compare
observed bytes. Runtime evidence must precede broad compatibility claims.

## Why playing the game is necessary

Spice86 records executed instructions and their control-flow edges; it does not
prove paths that were never visited. A100,000-instruction packed-EXE smoke test
can mostly cover unpacking. Setup choices, title input, new/load game and each
building/combat/puzzle path must actually run to observe their allocations and
state changes. Disk assets are loaded on demand; starting a game does not load
or exercise everything.

Use interactive keyboard input or the emulator's inspected input controls to
reach a checkpoint, save private state, then repeat a bounded scenario with
tracing. Automation must observe prompts and confirm resulting state, not
blindly send a guessed key sequence. Start with graphics/drive options and
enter a new game, then repeat from a known original save. The current launcher
needs a larger instruction budget for this than its short smoke default.

Upstream documents keyboard/mouse support, debugger/MCP CPU-memory-input
inspection and execution-flow collection:
[Spice86 project](https://github.com/OpenRakis/Spice86),
[MCP feature notes](https://github.com/OpenRakis/Spice86/wiki/Spice86-v12-release-notes).
The installed16.1.0 input transport still needs a real prompt-level test; do
not rely on the imported snapshot's unverified keyboard-injection recipe.
No playthrough or new gameplay validation occurred during this TODO refresh.

Validation:46 memory-reference arithmetic/link checks,132,772 sprite-reader
checks and39 sound-field/caller checks passed. `git diff --check` passed.
Source changes in this refresh are comments only.

Sol: Later exploration-dump analysis establishes the first run's relocated
code/table correspondence and mapping. Step3 above now starts with reusing
that verified mapping for event-boundary inspection, not guessing a base.
Gameplay-dependent TODOs remain open; see
[dump evidence](SPICE86_EXPLORATION_CODE_CONFIRMATION.md).
