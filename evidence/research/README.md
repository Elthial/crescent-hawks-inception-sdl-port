# BattleTech: The Crescent Hawk's Inception preservation research

This directory is the authoritative home for research produced in this
repository. The objective is to document the original DOS game well enough to
load its original data files and reproduce its behaviour in a portable C#
implementation. Original copyrighted game assets are not part of this work and
must be supplied by the user. The intended deliverable replaces the original
`.EXE` while continuing to load the original installation's levels, artwork,
animations, and other external data files.

## Repository evidence sources

The sources do not have equal authority:

1. Original executable/data bytes and repeatable runtime traces are the
   strongest evidence available when resolving a claim. `../Chinception/` is the
   ignored compatibility target for the replacement executable.
2. `BTech-Reko-expanded/` is the ignored, pre-annotation Reko baseline. It
   contains its source executable plus real-mode x86 ASM, Reko's `.dis`
   intermediate, and generated pseudo-C. Use the ASM and `.dis` output to audit
   annotation changes before trusting the generated C types.
3. `Btech/` contains the long-running, hand-annotated Reko output and
   `BTECH.h` research scratch pad. This is the primary body of prior semantic
   research and should be compared against the clean baseline by address and
   instruction sequence.
4. `../InceptionTools/` is the maintained C# tooling owned by this project and is
   the destination for future loader and inspection improvements.
5. `UnBattletech-main/` is an imported research and tooling snapshot. Its
   Spice86/DOSBox-X setups, BLD research, documentation, and generated C# may be
   used as supporting evidence, but its Reko output is not assumed to use the
   same executable profile. Discoveries from this project will not be pushed
   into UnBattletech itself.

The two local executable copies are not byte-identical: the Reko baseline copy
is an expanded 260,416-byte image, while `../Chinception/BTECH.EXE` is 152,429
bytes. Address equivalence is therefore verified, not assumed. See
[Executable profiles](reference/EXECUTABLE_PROFILES.md).

## Documentation map

- [Game method inventory: summaries, ASM review and port disposition](reference/METHOD_INVENTORY.md)
- [Fresh systematic C-to-ASM audit and file queue](phase4/ASM_SYSTEMATIC_AUDIT.md)
- [InceptionTools user and CLI guide](../../InceptionTools/README.md)
- [Project plan](PROJECT_PLAN.md)
- [InceptionTools long-term toolkit scope](INCEPTIONTOOLS_ROADMAP.md)
- [Phase 2A InceptionTools baseline and installation inventory](phase2/INCEPTIONTOOLS_BASELINE.md)
- [Phase 2B bounded raw-file inspection](phase2/RAW_INSPECTION.md)
- [Phase 2C structured save, character, and mech dump](phase2/STRUCTURED_SAVE_DUMP.md)
- [Phase 2D structured weapon-table dump](phase2/WEAPON_TABLE_DUMP.md)
- [Phase 2E lossless BLD disassembly](phase2/BLD_DISASSEMBLY.md)
- [Phase 3A .NET 8 migration](phase3/DOTNET_8_MIGRATION.md)
- [Phase 3B dependency security refresh](phase3/DEPENDENCY_REFRESH.md)
- [Phase 3C bounded binary reader](phase3/BOUNDED_BINARY_READER.md)
- [Phase 3D canonical record and save models](phase3/CANONICAL_RECORD_MODELS.md)
- [Phase 3E targeted save-record queries](phase3/TARGETED_SAVE_RECORDS.md)
- [Phase 3F ANM container inspection](phase3/ANM_CONTAINER_INSPECTION.md)
- [Phase 3G ANM frame decoding](phase3/ANM_FRAME_DECODING.md)
- [Phase 3H ANM pixel layout](phase3/ANM_PIXEL_LAYOUT.md)
- [Phase 3I ANM frame PNG export](phase3/ANM_FRAME_EXPORT.md)
- [Phase 3J ANM numbered PNG sequence export](phase3/ANM_SEQUENCE_EXPORT.md)
- [Phase 3K ANM timing table and retrace counts](phase3/ANM_TIMING.md)
- [Phase 3L looping ANM GIF export](phase3/ANM_GIF_EXPORT.md)
- [Phase 3M1 bounded CMP/ICN image decoder](phase3/CMP_BOUNDED_DECODER.md)
- [Phase 3M2 MECHSHAP sequence spritesheet export](phase3/MECH_SPRITESHEET_EXPORT.md)
- [Phase 3N command-line save-state text editor](phase3/SAVE_STATE_TEXT_EDITOR.md)
- [Phase 3O SIF and executable sound audio](phase3/SIF_AND_SOUND_EFFECT_AUDIO.md)
- [Phase 3P portable CMP/ICN graphics export](phase3/PORTABLE_GRAPHICS_EXPORT.md)
- [Phase 3Q portable MTP map export](phase3/PORTABLE_MAP_EXPORT.md)
- [Phase 3R legacy extractor retirement](phase3/LEGACY_EXTRACTOR_RETIREMENT.md)
- [Research method and confidence levels](RESEARCH_METHOD.md)
- [Original-files and distribution policy](DISTRIBUTION_POLICY.md)
- [File-format reference](formats/README.md)
- [BLD decoded opcode reference](formats/BLD_OPCODES.md)
- [BLD opcode executable-target register](investigations/BLD_EXECUTABLE_TARGETS.md)
- [Demo input stream and SIF sound data](formats/DEMO_AND_SOUND.md)
- [Mech records](data-structures/MECH_RECORD.md)
- [Weapon records and arrays](data-structures/WEAPON_RECORDS.md)
- [Character records and arrays](data-structures/CHARACTER_RECORDS.md)
- [Packed map-coordinate representation](data-structures/PACKED_MAP_COORDINATES.md)
- [Executable sound effects](data-structures/SOUND_EFFECTS.md)
- [Segmented memory reference](memory/README.md)
- [Asset and game identifiers](reference/ASSET_AND_GAME_IDS.md)
- [`BTECH.h` consolidation audit](reference/BTECH_H_AUDIT.md)
- [DOS hardware and input constants](reference/DOS_HARDWARE_AND_INPUT.md)
- [Random-number and dice helpers](reference/RANDOM_AND_DICE.md)
- [Copy protection and player reference materials](reference/COPY_PROTECTION_AND_PLAYER_REFERENCES.md)
- [Executable profiles and decompiler provenance](reference/EXECUTABLE_PROFILES.md)
- [Contradictions and open questions](investigations/CONTRADICTIONS.md)
- [Original-game bugs and suspicious behaviours](investigations/ORIGINAL_GAME_BUGS.md)
- [Candidate sections for Astra review](investigations/ASTRA_REVIEW.md)
- [Original-executable runtime validation and unresolved-behaviour comparisons](investigations/RUNTIME_VALIDATION.md)
- [Spice86 plain-emulation host and private bounded runs](../tools/Btech.Spice86/README.md)
- [Refreshed TODO dispositions and actionable follow-ups](investigations/TODO_REVIEW.md)
- [Exploration dump: relocated code/table identity and observed call paths](investigations/SPICE86_EXPLORATION_CODE_CONFIRMATION.md)
- [`BTECH_0800` yes/no prompt](phase4/BTECH_0800_YES_NO_PROMPT.md)
- [`BTECH_0800` ANM frame decoder and renderer](phase4/BTECH_0800_ANM_FRAME_RENDERER.md)
- [`BTECH_0800` map-interaction boundary check](phase4/BTECH_0800_MAP_INTERACTION_BOUNDARY_CHECK.md)
- [`BTECH_0800` on-foot movement map projection](phase4/BTECH_0800_ON_FOOT_MAP_PROJECTION.md)
- [`BTECH_0800` on-foot building interaction](phase4/BTECH_0800_ON_FOOT_BUILDING_INTERACTION.md)
- [`BTECH_0800` Star League Cache tile dispatcher](phase4/BTECH_0800_STAR_LEAGUE_TILE_DISPATCH.md)
- [`BTECH_0800` friendly-mech movement and building check](phase4/BTECH_0800_FRIENDLY_MECH_MOVEMENT_CHECK.md)
- [`BTECH_0800` movement-command decode](phase4/BTECH_0800_MOVEMENT_COMMAND_DECODE.md)
- [`BTECH_0800` directional movement and map streaming](phase4/BTECH_0800_DIRECTIONAL_MAP_STREAMING.md)
- [`BTECH_0800` movement-cache finalization](phase4/BTECH_0800_MOVEMENT_CACHE_FINALIZATION.md)
- [`BTECH_0800` friendly movement animations](phase4/BTECH_0800_FRIENDLY_MOVEMENT_ANIMATIONS.md)
- [`BTECH_0800` animated map-tile updater](phase4/BTECH_0800_ANIMATED_MAP_TILES.md)
- [`BTECH_0800` roaming map-NPC updater](phase4/BTECH_0800_ROAMING_MAP_NPCS.md)
- [`BTECH_0800` text and game-disk helpers](phase4/BTECH_0800_TEXT_AND_DISK_HELPERS.md)
- [`BTECH_0800` input and text utilities](phase4/BTECH_0800_INPUT_AND_TEXT_UTILITIES.md)
- [`BTECH_0800` persistent map effects](phase4/BTECH_0800_PERSISTENT_MAP_EFFECTS.md)
- [`BTECH_0800` pause menu](phase4/BTECH_0800_PAUSE_MENU.md)
- [`BTECH_0800` map-loader setup](phase4/BTECH_0800_MAP_LOADER_SETUP.md)
- [`BTECH_0800` MTP reads and cached-grid descriptors](phase4/BTECH_0800_MTP_READ_AND_GRID_DESCRIPTORS.md)
- [`BTECH_0800` map-loader roaming-NPC initialization](phase4/BTECH_0800_MAP_LOADER_NPC_INITIALIZATION.md)
- [`BTECH_0800` load-game selection and input](phase4/BTECH_0800_LOAD_GAME_INPUT.md)
- [`BTECH_0800` load-game state reconstruction](phase4/BTECH_0800_LOAD_GAME_RECONSTRUCTION.md)
- [`BTECH_0800` load-game finalization](phase4/BTECH_0800_LOAD_GAME_FINALIZATION.md)
- [`BTECH_0800` save-game workflow](phase4/BTECH_0800_SAVE_GAME.md)
- [`BTECH_0800` character and 'Mech inspection](phase4/BTECH_0800_INSPECT_CHARACTERS.md)
- [`BTECH_0800` game-settings menu](phase4/BTECH_0800_GAME_SETTINGS.md)
- [`BTECH_0800` overhead-map controller](phase4/BTECH_0800_OVERHEAD_MAP_CONTROLLER.md)
- [`BTECH_0800` overhead-map renderer](phase4/BTECH_0800_OVERHEAD_MAP_RENDERER.md)
- [`BTECH_0800` dynamic overhead tiles](phase4/BTECH_0800_DYNAMIC_OVERHEAD_TILES.md)
- [`BTECH_0800` BattleTech tileset loader](phase4/BTECH_0800_BATTLETECH_TILESET_LOADER.md)
- [`BTECH_0800` title-screen loader](phase4/BTECH_0800_TITLE_SCREEN_LOADER.md)
- [`BTECH_0800` intro-music controller](phase4/BTECH_0800_INTRO_MUSIC.md)
- [`BTECH_0800` animation-scene controller](phase4/BTECH_0800_ANIMATION_SCENE_CONTROLLER.md)
- [`BTECH_0800` character B/D/C sidebar row](phase4/BTECH_0800_CHARACTER_BDC_SIDEBAR.md)
- [`BTECH_0800` party-status and C-Bills sidebar](phase4/BTECH_0800_PARTY_STATUS_SIDEBAR.md)
- [`BTECH_0800` pilot-assignment dismount guard](phase4/BTECH_0800_PILOT_ASSIGNMENT_GUARD.md)
- [`BTECH_0800` new-game reset prefix](phase4/BTECH_0800_NEW_GAME_RESET.md)
- [`BTECH_0800` Jason and initial economy state](phase4/BTECH_0800_JASON_INITIAL_STATE.md)
- [`BTECH_0800` combatant sprite-family initialization](phase4/BTECH_0800_COMBATANT_SPRITE_FAMILIES.md)
- [`BTECH_0800` new-game runtime-state reset](phase4/BTECH_0800_NEW_GAME_RUNTIME_RESET.md)
- [`BTECH_0800` initial Citadel view](phase4/BTECH_0800_INITIAL_CITADEL_VIEW.md)
- [`BTECH_0800` new-game combatant display state](phase4/BTECH_0800_NEW_GAME_COMBATANT_DISPLAY_STATE.md)
- [`BTECH_0800` start-game initialization](phase4/BTECH_0800_START_GAME_INITIALIZATION.md)
- [`BTECH_0800` interactive startup branch](phase4/BTECH_0800_INTERACTIVE_STARTUP.md)
- [`BTECH_0800` attract mode and startup-loop tail](phase4/BTECH_0800_ATTRACT_MODE.md)
- [`BTECH_0DAB` salvage technician selection](phase4/BTECH_0DAB_SALVAGE_TECH_SELECTION.md)
- [`BTECH_0DAB` armour salvage and field repair](phase4/BTECH_0DAB_ARMOUR_SALVAGE.md)
- [`BTECH_0DAB` internal-structure salvage](phase4/BTECH_0DAB_STRUCTURE_SALVAGE.md)
- [`BTECH_0DAB` heat-sink salvage](phase4/BTECH_0DAB_HEAT_SINK_SALVAGE.md)
- [`BTECH_0DAB` weapon-component salvage](phase4/BTECH_0DAB_WEAPON_SALVAGE.md)
- [`BTECH_0DAB` post-battle scrap payout](phase4/BTECH_0DAB_SCRAP_PAYOUT.md)
- [`BTECH_0DAB` whole-mech salvage technician selection](phase4/BTECH_0DAB_WHOLE_MECH_TECH_SELECTION.md)
- [`BTECH_0DAB` wreck candidate selection and catastrophic-damage gate](phase4/BTECH_0DAB_WRECK_CANDIDATE_SELECTION.md)
- [`BTECH_0DAB` whole-mech salvage success message](phase4/BTECH_0DAB_WHOLE_MECH_SUCCESS_MESSAGE.md)
- [`BTECH_0DAB` whole-mech record copy](phase4/BTECH_0DAB_WHOLE_MECH_RECORD_COPY.md)
- [`BTECH_0DAB` whole-mech installation and forced damage](phase4/BTECH_0DAB_WHOLE_MECH_INSTALL.md)
- [`BTECH_0DAB` whole-mech rejection message](phase4/BTECH_0DAB_WHOLE_MECH_REJECTION.md)
- [`BTECH_0DAB` salvage candidate state handling](phase4/BTECH_0DAB_SALVAGE_CANDIDATE_STATE.md)
- [`BTECH_0DAB` post-battle infantry loot](phase4/BTECH_0DAB_INFANTRY_LOOT.md)
- [`BTECH_0DAB` hardware initialization and timing calibration](phase4/BTECH_0DAB_HARDWARE_INITIALIZATION.md)
- [`BTECH_0DAB` encounter origin and enemy infantry generation](phase4/BTECH_0DAB_ENCOUNTER_INFANTRY_GENERATION.md)
- [`BTECH_0DAB` random enemy 'Mech generation](phase4/BTECH_0DAB_ENEMY_MECH_GENERATION.md)
- [`BTECH_0DAB` enemy infantry placement](phase4/BTECH_0DAB_ENEMY_INFANTRY_PLACEMENT.md)
- [`BTECH_0DAB` enemy 'Mech placement](phase4/BTECH_0DAB_ENEMY_MECH_PLACEMENT.md)
- [`BTECH_0DAB` combat scan target browser](phase4/BTECH_0DAB_COMBAT_SCAN_BROWSER.md)
- [`BTECH_0DAB` mech-status gauges and component pips](phase4/BTECH_0DAB_MECH_STATUS_RENDERERS.md)
- [`BTECH_0DAB` friendly-combatant description](phase4/BTECH_0DAB_FRIENDLY_COMBATANT_DESCRIPTION.md)
- [`BTECH_0DAB` BTSTATS setup and crew display](phase4/BTECH_0DAB_BTSTATS_SETUP_AND_CREW.md)
- [`BTECH_0DAB` BTSTATS component pips and armament list](phase4/BTECH_0DAB_BTSTATS_COMPONENTS_AND_ARMAMENT.md)
- [`BTECH_0DAB` BTSTATS actuator display](phase4/BTECH_0DAB_BTSTATS_ACTUATORS.md)
- [`BTECH_0DAB` BTSTATS redraw and exit loop](phase4/BTECH_0DAB_BTSTATS_REDRAW_AND_EXIT.md)
- [`BTECH_0FDC` BLD interaction entry](phase4/BTECH_0FDC_BLD_INTERACTION_ENTRY.md)
- [`BTECH_0FDC` BLD bytecode interpreter](phase4/BTECH_0FDC_BLD_BYTECODE_INTERPRETER.md)
- [`BTECH_0FDC` BLD target reader](phase4/BTECH_0FDC_BLD_TARGET_READER.md)
- [`BTECH_0FDC` mission setup dispatch](phase4/BTECH_0FDC_MISSION_SETUP_DISPATCH.md)
- [`BTECH_0FDC` common mission loop](phase4/BTECH_0FDC_MISSION_COMMON_LOOP.md)
- [`BTECH_0FDC` mission 0 and 1 objectives](phase4/BTECH_0FDC_MISSION_OBJECTIVES_00_01.md)
- [`BTECH_0FDC` jailbreak mission loop](phase4/BTECH_0FDC_JAILBREAK_LOOP.md)
- [`BTECH_0FDC` mission teardown](phase4/BTECH_0FDC_MISSION_TEARDOWN.md)
- [`BTECH_0FDC` enemy-generator reset](phase4/BTECH_0FDC_ENEMY_GENERATOR_RESET.md)
- [`BTECH_0FDC` enemy spawn selection](phase4/BTECH_0FDC_ENEMY_SPAWN_SELECTION.md)
- [`BTECH_0FDC` enemy mech generation](phase4/BTECH_0FDC_ENEMY_MECH_GENERATION.md)
- [`BTECH_0FDC` enemy infantry generation](phase4/BTECH_0FDC_ENEMY_INFANTRY_GENERATION.md)
- [`BTECH_0FDC` post-combat mission-view restore](phase4/BTECH_0FDC_POST_COMBAT_VIEW_RESTORE.md)
- [`BTECH_0FDC` purchased-armour distribution](phase4/BTECH_0FDC_ARMOUR_DISTRIBUTION.md)
- [`BTECH_0FDC` party weapon distribution](phase4/BTECH_0FDC_WEAPON_DISTRIBUTION.md)
- [`BTECH_0FDC` building-occupant conversations](phase4/BTECH_0FDC_BUILDING_OCCUPANT_CONVERSATIONS.md)
- [`BTECH_0FDC` stock suffix and BLD immediate-word reader](phase4/BTECH_0FDC_TEXT_AND_WORD_HELPERS.md)
- [`BTECH_0FDC` Arena party-mech setup](phase4/BTECH_0FDC_ARENA_PARTY_MECH_SETUP.md)
- [`BTECH_0FDC` Arena post-combat restoration](phase4/BTECH_0FDC_ARENA_POST_COMBAT_RESTORE.md)
- [`BTECH_0FDC` Arena rental-Locust setup](phase4/BTECH_0FDC_ARENA_RENTAL_MECH_SETUP.md)
- [`BTECH_0FDC` indexed BLD load and decode](phase4/BTECH_0FDC_BLD_LOAD_AND_DECODE.md)
- [`BTECH_11B8` Mech-Lube repair diagnostics](phase4/BTECH_11B8_MECHLUBE_REPAIR_DIAGNOSTICS.md)
- [`BTECH_11B8` Mech-Lube armour repair](phase4/BTECH_11B8_MECHLUBE_ARMOUR_REPAIR.md)
- [`BTECH_11B8` Mech-Lube internal-structure repair](phase4/BTECH_11B8_MECHLUBE_STRUCTURE_REPAIR.md)
- [`BTECH_11B8` Mech-Lube heat-sink repair](phase4/BTECH_11B8_MECHLUBE_HEAT_SINK_REPAIR.md)
- [`BTECH_11B8` Mech-Lube weapon repair](phase4/BTECH_11B8_MECHLUBE_WEAPON_REPAIR.md)
- [`BTECH_11B8` Mech-Lube actuator repair](phase4/BTECH_11B8_MECHLUBE_ACTUATOR_REPAIR.md)
- [`BTECH_11B8` Mech-Lube modification setup](phase4/BTECH_11B8_MECHLUBE_MODIFICATION_SETUP.md)
- [`BTECH_11B8` Mech-Lube modification purchase](phase4/BTECH_11B8_MECHLUBE_MODIFICATION_PURCHASE.md)
- [`BTECH_11B8` Mech-Lube Locust first-stage modification](phase4/BTECH_11B8_MECHLUBE_LOCUST_STAGE_ONE.md)
- [`BTECH_11B8` complete Mech-Lube upgrade packages](phase4/BTECH_11B8_MECHLUBE_UPGRADE_PACKAGES.md)
- [`BTECH_11B8` scripted Crescent Hawk recruitment](phase4/BTECH_11B8_CRESCENT_HAWK_RECRUITMENT.md)
- [`BTECH_11B8` Rex recruitment and Kurita-party ambush](phase4/BTECH_11B8_REX_KURITA_AMBUSH.md)
- [`BTECH_11B8` Arena combat map-patch selection](phase4/BTECH_11B8_ARENA_COMBAT_MAP_PATCH.md)
- [`BTECH_11B8` Arena combat map cleanup](phase4/BTECH_11B8_ARENA_COMBAT_MAP_CLEANUP.md)
- [`BTECH_11B8` Jailbreak staging and Stinger award](phase4/BTECH_11B8_JAILBREAK_STAGING_AND_STINGER.md)
- [`BTECH_11B8` failed mech-startup scene](phase4/BTECH_11B8_FAILED_MECH_STARTUP_SCENE.md)
- [`BTECH_11B8` Mech-Lube ammunition purchase](phase4/BTECH_11B8_MECHLUBE_AMMO_PURCHASE.md)
- [`BTECH_1467` complete segment: crew assignment, written quiz and movement-plan reset](phase4/BTECH_1467_SEGMENT_REVIEW.md)
- [`BTECH_1543` complete segment: weapon planning, target selection, destruction and numeric input](phase4/BTECH_1543_SEGMENT_REVIEW.md)
- [`BTECH_1631` completed local workflow review: movement, AI, heat and combat helpers; shared-renderer boundaries](phase4/BTECH_1631_SEGMENT_REVIEW.md)
- [`BTECH_207F` large logical-chunk review: early EGA transfer, sprite clipping and planar conversion](phase4/BTECH_207F_SEGMENT_REVIEW.md)
- [`BTECH_1E56` menus, text wrapping/scrolling, tilesets and keyboard/input bridge](phase4/BTECH_1E56_SEGMENT_REVIEW.md)
- [`BTECH_1CD3` stock balance and transaction CHECK ASM audit](phase4/BTECH_1CD3_STOCK_TRANSACTIONS.md)
- [`BTECH_1CD3` skill scores and living-party slot selection](phase4/BTECH_1CD3_SKILL_AND_PERSONNEL.md)
- [`BTECH_1CD3` Citadel training skill access and tuition](phase4/BTECH_1CD3_TRAINING.md)
- [Training callers, purchase result and script-driven cooldowns](phase4/BTECH_TRAINING_LIFECYCLE.md)
- [`BTECH_1CD3` armour/weapon purchases and shared cash dialogue](phase4/BTECH_1CD3_SHOP_PURCHASES.md)
- [`BTECH_1CD3` specialist training, armour repairs and medical services](phase4/BTECH_1CD3_SPECIALIST_AND_SERVICES.md)
- [`BTECH_1431` healing, medic selection, dice matrix and recovery lifecycle](phase4/BTECH_1431_HEALING.md)
- [Hospital recovery exemption, fee gating and facilities refund](phase4/BTECH_HOSPITAL_SCRIPT_GATING.md)
- [`BTECH_1CD3` friendly roster hiding, counts and name restoration](phase4/BTECH_1CD3_FRIENDLY_ROSTER.md)
- [`BTECH_1CD3` arena entry, normal return and escape roster placement](phase4/BTECH_1CD3_ARENA_RETURN.md)
- [`BTECH_1CD3` character-name dialogue, crew handoffs and laser/health-cap scenario](phase4/BTECH_1CD3_NAME_AND_EQUIPMENT_DIALOGS.md)
- [`BTECH_1CD3` NPC position staging and ending asset workflow](phase4/BTECH_1CD3_POSITION_STAGING_AND_ENDING.md)
- [`BTECH_1CD3` final consistency, palette/text FAR contracts and raw2F flags](phase4/BTECH_1CD3_FINAL_CONSISTENCY.md)
- [`BTECH_135D` Star League cache setup and secret-passage tile patch](phase4/BTECH_135D_CACHE_SETUP_AND_PASSAGE.md)
- [`BTECH_135D` cache discovery/power dialogue and overview colour-map swapping](phase4/BTECH_135D_CACHE_DIALOGUE_AND_OVERVIEW.md)
- [`BTECH_135D` security-terminal lookup and one-use code selection](phase4/BTECH_135D_SECURITY_TERMINAL.md)
- [`BTECH_135D` transmitter gating and map-room entry/buffer reuse](phase4/BTECH_135D_TRANSMITTER_AND_MAP_ROOM.md)
- [`BTECH_135D` map-room return and star-selection validation](phase4/BTECH_135D_RETURN_AND_STAR_PUZZLE.md)
- [`BTECH_135D` door lookup, code consumption, replay and sentinel mismatch](phase4/BTECH_135D_DOOR_CODES.md)
- [`BTECH_135D` final caller/table consistency and remaining port obligations](phase4/BTECH_135D_FINAL_CONSISTENCY.md)
- [`BTECH_1FC5` sound setup, WORD cursor and stream dispatch](phase4/BTECH_1FC5_SOUND_DISPATCH.md)
- [`BTECH_1FC5` signed repetition wrappers and helper argument contracts](phase4/BTECH_1FC5_REPETITION_WRAPPERS.md)
- [`BTECH_1FC5` timer binding, fixed-tone delay and speaker-off](phase4/BTECH_1FC5_FIXED_TONE.md)
- [`BTECH_1FC5` noise/gate counters, named fields and implicit DX countdown](phase4/BTECH_1FC5_NOISE_GATE_LOOPS.md)
- [`BTECH_1FC5` divisor sweep and low-WORD calibrated delay](phase4/BTECH_1FC5_DIVISOR_SWEEP.md)
- [`BTECH_1FC5` descending/ascending noise-mask parameter sweeps](phase4/BTECH_1FC5_NOISE_PARAMETER_SWEEPS.md)
- [`BTECH_1FC5` final caller/field consistency and first-pass completion](phase4/BTECH_1FC5_FINAL_CONSISTENCY.md)
- [`BTECH_1F3D` whole-segment EGA, input, file-loading and allocation review](phase4/BTECH_1F3D_SEGMENT_REVIEW.md)
- [Startup, missing music/EGA routines and portable platform boundaries](phase4/BTECH_STARTUP_MUSIC_PLATFORM_AUDIT.md)
- [`0800` sprite-pointer readers and transient terrain clipping](phase4/BTECH_0800_SPRITE_POINTER_READERS.md)
- [Combat movement orders, step aliases and critical-slot/ammo execution linkage](phase4/COMBAT_MOVEMENT_AND_WEAPON_LINKAGE.md)

## Documentation rules

- Every numeric offset is hexadecimal unless explicitly marked decimal.
- The target is a 16-bit little-endian DOS program. A native word or near
  offset is normally two bytes and a segmented far pointer is normally four,
  but packed file/save records may still contain individual byte fields.
- Reko was used with a 32-bit model and may default uncertain `int` and pointer
  types to 32 bits. Rendered C types never override 16-bit instruction, stack,
  segment-register, stride, or address-spacing evidence.
- File offsets are written as `file+0xNN`; segmented addresses as
  `segment:offset`; structure offsets as `record+0xNN`.
- Names describe verified meaning where possible. Unknown fields retain neutral
  names rather than acquiring a speculative semantic name.
- A new interpretation must cite its evidence and confidence.
- Conflicting claims stay in the contradiction register until resolved.
- Historical notes are preserved. Cleaning the documentation does not erase the
  original `BTECH.h` scratch pad.
