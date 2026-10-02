# Original-executable runtime validation

Sol: 2026-09-17. **Spice86 loading/execution smoke tests have run; no gameplay
behaviour has yet been confirmed.**
This register separates instruction evidence, synthetic arithmetic checks and
observed original-game behaviour. Model agreement is not runtime evidence.

## Tooling preflight

- `dosbox` and `dosbox-x` were not found on PATH. DOSBox-X was also absent at
  the three checked common locations (`C:\Program Files\DOSBox-X`,
  `C:\Program Files (x86)\DOSBox-X`, `C:\DOSBox-X`). This does not prove it
  is absent elsewhere; obtain its actual location before installing anything.
- The imported `UNBATTLETECH.csproj` requires two missing local projects:
  `F:\Source Code\Own Code\Spice86\src\Spice86\Spice86.csproj` and
  `F:\Source Code\Own Code\Spice86\src\BattleTechMcpTools\BattleTechMcpTools.csproj`.
  .NET 10 SDK is available, but cannot substitute for these dependencies.
- Imported DOSBox configs mount a Linux directory and invoke `UNBTECH.exe`.
  The debug config selects `svga_s3` and disables mixer/PC-speaker output.
  Do not use it unchanged for EGA or music comparisons.
- Imported `Program.cs` selects an override supplier and hash
  `E29007761FADD8679521D1FB1DC6B488F87C718ED8A4636CB4FFBE4BC4ED5306`.
  Neither local EXE matches it. Old dumps/generated overrides are not evidence
  for this executable without independent provenance and relocation checking.

Local SHA-256 identities (metadata only; no game bytes included):

| Local input | SHA-256 |
| --- | --- |
| `chinception/BTECH.EXE` (packed distribution input) | `F2A9A023D79927B8072DE11DDD6E03DA6D3357D49181D8FBA6D12988BF8CC0EE` |
| `BTech-Reko-expanded/BTECH.EXE` (unpacked audit input) | `F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE` |

Different packed/unpacked hashes are expected and do not by themselves prove
different gameplay code. Establish their loaded-code correspondence before
mapping audit addresses to the running packed executable. The original was
already modified to remove the copyright challenge before this project; do
not describe this as a pristine commercial-release reference.

## Execution and evidence rules

1. Use the user's local original assets. Never commit/upload external game
   files, save files, memory dumps containing assets, frames or audio captures.
   Store private outputs under an ignored local directory such as
   `chinception/validation`; do not mount an unprotected canonical save for
   tests that can write it. Create an isolated working copy before such runs.
2. Establish a plain emulator baseline without generated gameplay overrides.
   Record emulator version, CPU/core/cycle settings, graphics/audio choice,
   executable hash, initial save hash, input sequence and RNG/timer controls.
3. Translate static segment addresses using the actual loader relocation and
   post-unpack memory map. `3092`/`0800` notation is an analysis address, not
   a promise of those literal live segment-register values.
4. Capture pre/post state and relevant instruction/register traces at the
   same event boundary. Breakpoints can change real-time music/input behaviour;
   distinguish paused state inspection from uninterrupted playback tests.
5. Compare literal research behaviour first. Any proposed symmetric/safe/fixed
   alternative is a separate experiment, never the reference expectation.
6. Record pass/fail/unreachable/inconclusive plus the exact tested scenario.
   A passing example does not validate the entire routine or prove intent.

## Prioritised comparisons

| Priority / scope | Required observation | Status |
| --- | --- | --- |
| 1: startup and `0800` rendering | Loaded table of 376 FAR entries; party/NPC/mech draws; both jailbreak overlays; fire effects. Observe each clipping site's header BYTE before, during and after draw; confirm pixel placement and unchanged table. | Exploration capture confirms instantiated table/sample geometry, code identity and selected call edges. Pixel/clipping/overlay comparison still pending. |
| 2: music, PC speaker then Tandy | Initial callback delay, BYTE stream advancement, stop cursor, channel order, divisor/PSG writes and BIOS timer chaining. Compare uninterrupted playback separately; tooling waveform approximations are not chip-accurate validation. | Setup/idle handler observed; PC/Tandy note-stream handlers not observed in exploration trace. Audible comparison still pending. |
| 3: A-002 combat workflow | Encounter entry, planning, movement/collision/crushing, firing/ammo, heat, betrayal, round completion, salvage and restoration; trace one bounded scenario at a time. | Local ASM audit complete; runtime pending. |
| 4: A-001 BLD transitions | One shop/training/hospital script with bytecode offsets, branch outcomes, native dispatch and state flags; then arena exit. | Runtime pending. |
| 5: A-003 DS/SS | Loader setup and `1467:0838` row-map accesses; capture actual DS/SS and effective addresses, not just register values from an unrelated old dump. | Runtime pending. |
| 6: A-004/A-005 | Commando upgrade firing ordinals/ammo; AI selected distance; controlled hit/critical per body group and the C8 selector. | Policy/anatomy unresolved. |
| 7: A-006 salvage address | At `0DAB:0995–09AD`, inventory both raw address schemes, flags `395C–396A`, actual record contents and resulting loot menu. | Runtime pending. |
| 8: A-007 held NPC | Eight route records, compact menu selection and D339/D33A before/after `0FDC:18C6–18F2`. Include multiple waiting NPCs. | Runtime pending. |
| 9: A-008 map generation / BUG-020 | Four corners, seeds/index, lattice and AL/DX across midpoint generation; exact literal output before assessing terrain impact. | Runtime pending. |
| 10: A-009 allocation error | Establish a reachable oversize caller; trace error/input return and BP-6/BP-4 safely. Forced malformed input is not normal-game reachability evidence. | Runtime pending. |

Suspected original bugs need individual reproductions from
[the bug register](ORIGINAL_GAME_BUGS.md), beginning with zero-resource heat-sink
salvage (BUG-001) and the Commando weapon-list/ammo boundary (BUG-012).
Keep existing instruction-based classifications, but add an explicit observed
result only after a matching runtime trace. Do not silently fix these paths.

## Result record template

First recorded code/table correspondence and live address mapping:
[exploration dump confirmation](SPICE86_EXPLORATION_CODE_CONFIRMATION.md).
It does not close the outstanding scenario comparisons above.

- ID / exact routine or bug:
- Emulator/version/settings; EXE hash; load/relocation mapping:
- Initial state/save hash, RNG/timer conditions and input sequence:
- Observation boundary and private local capture location:
- Expected literal behaviour; actual state/register/render/audio result:
- Verdict: pass / fail / unreachable / inconclusive:
- What this establishes; what remains uncertain:
- Annotated-source discrepancy and proposed bounded change, if any:

## Current blocker / next action

### Sol: installation located

The user supplied `F:\Applications\Dosbox-x`. Its `dosbox-x.exe` reports file
version `2026.08.31`; the executable contains `DEBUGBOX`, `BPINT`, `-helpdebug`
and `-break-start` strings. This supports debugger availability but does not
prove working interactive tracing. A hidden `-helpdebug` probe produced no
redirected output and did not exit within five seconds; that probe process was
stopped. No game was run by this probe.

The imported Spice86 checkout remains absent; a separate pinned NuGet host is
now configured at `tools/Btech.Spice86`. It is not required for DOSBox-X runs.
Its initial plain-emulation smoke executed 100,003 instructions from the packed
local EXE and exported private dumps, exit code 0. This is execution evidence,
not confirmation of startup completion, workflow, graphics or audio fidelity.
See the [host instructions and limits](../../tools/Btech.Spice86/README.md).
The earlier PATH/common-location checks remain historical preflight evidence,
not the current installation blocker.

Run `./scripts/Prepare-DosBoxValidation.ps1` from PowerShell. It creates a fresh
ignored `chinception/validation/run-*` directory, verifies disposable copies of
all top-level game/save files, generates an EGA/PC-speaker config and records
input/config/emulator hashes in a private manifest. It prints an interactive
launch command, but does not launch or overwrite previous runs. Captures and
emulator save states use private directories. The fixed 3000-cycle setting is
an initial reproducible baseline, **not calibrated original-PC timing**.

Choose EGA and the hard-disk option if the original game asks. Establish a
normal startup first, then verify interactive debugger entry and derive the
loaded-code address mapping before setting audit breakpoints. Tandy audio is
a separate subsequent configuration experiment; do not change the EGA graphics
baseline merely to enable it.

Launch options are documented in the
[DOSBox-X command-line reference](https://github.com/joncampbell123/dosbox-x/wiki/DOSBox%E2%80%90X%E2%80%99s-Command%E2%80%90Line-Options).
The installed `dosbox-x.reference.full.conf` supplies the local config keys.
No gameplay result is promoted from preparation or configuration parsing.
Expensive model review remains separately approval-gated.

### Practical interactive capture

`./scripts/Start-SpiceValidation.ps1` now runs without an instruction limit.
Headless mode retains a100,000-instruction default, and either mode accepts an
explicit positive limit. Close the **Spice86 window** normally, then wait for
the launcher's verified-export message. The printed private run directory
retains upstream logs, launcher transcript, arguments and a session result
with memory/register/flow export checks. Killing the process or closing the
PowerShell console is not an equivalent shutdown and can lose dumps.

Normal application-window close was tested, including the updated launcher
end-to-end: exit0, no instruction cap, all three required dumps saved. This
verifies capture shutdown, not gameplay; auxiliary generated/CFG exports are
not covered by the required-export guarantee.
