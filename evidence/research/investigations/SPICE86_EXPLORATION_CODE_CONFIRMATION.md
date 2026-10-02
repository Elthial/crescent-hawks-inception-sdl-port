# Sol: exploration dump — executable/code confirmation

2026-09-17. Scope: the user's original-game startup and walk with Jason to
below the barracks, beside a shop. Shop identity is not established by this
capture. No building entry or combat scenario was recorded.

## Provenance and reproducible comparison

- Private run: `chinception/validation/run-20260917-133357-c860addc`.
- Emulator: our plain Spice8616.1.0 host, code overrides disabled.
- Packed EXE SHA-256: `F2A9A023D79927B8072DE11DDD6E03DA6D3357D49181D8FBA6D12988BF8CC0EE`.
- Expanded EXE SHA-256: `F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE`.
- Memory-dump SHA-256: `7A55C0D57202E9C582B44344D924443FCF5CE67BE6D8DC02BE2ABE9B7D9CF333`.
- Log: instruction budget stopped execution after1,000,000,001 instructions
  in38,851ms. Exports completed; this was not a recorded crash.

`scripts/Confirm-SpiceDumpCode.ps1` reads the private memory dump, execution
addresses and expanded EXE. It removes the825-paragraph MZ header and applies
all3292 MZ relocation entries to a temporary in-memory copy of the expanded
image, adding the reconstructed load segment017D modulo WORD. Eight independent
32-byte anchors must match before the broad comparison is accepted. A deliberately
wrong017E load base was rejected. Neither original file is modified.

```powershell
$dump = Get-ChildItem chinception/validation/run-20260917-133357-c860addc/spice86 -Directory | Select-Object -First 1
./scripts/Confirm-SpiceDumpCode.ps1 -DumpDirectory $dump.FullName -LoadSegment 0x017D
```

For this run, analysis segment S maps to live segment `S - 0683` (offset
unchanged). Equivalently, image-relative offset `(S - 0800)*16 + offset`
maps to physical memory `017D*16 + image-relative offset`. This is a verified
mapping for this capture, **not** a universal runtime load address and not a
mapping from compressed-file offsets to expanded-file offsets.

Selected live data segments:246C→1DE9,2FE8→2965,3092→2A0F,3EDB→3858.
Terminal register snapshot has DS1DE9 and SS3858. That alone cannot settle
A-003's DS/SS relationship at the crew-assignment call.

## Broad result

Compared the116,416-byte analysis-address region `0800:0000` up to (excluding)
`246C:0000`, including original code, embedded data and padding. **116,402 bytes
match;14 differ, all in documented writable CS204B music state. No unexplained
code-region differences remain.** The game's original deleted/non-EGA paths
are included in byte identity, not restored into maintained pseudo-C.

| Region | Bytes compared | Differences | Distinct executed instruction starts |
| --- | ---: | ---: | ---: |
| 0800 |21104|0|2822|
| 0D27 |2112|0|742|
| 0DAB |8976|0|127|
| 0FDC |7616|0|0|
| 11B8 |6736|0|0|
| 135D |3392|0|0|
| 1431 |864|0|0|
| 1467 |3520|0|0|
| 1543 |3808|0|0|
| 1631 |8352|0|348|
| 183B |10960|0|4*|
| 1AE8 |7856|0|0|
| 1CD3 |6192|0|0|
| 1E56 |3696|0|818|
| 1F3D |2176|0|497|
| 1FC5 |2144|0|0|
| 204B |832|14|99|
| 207F |16080|0|3450|

*The four183B-region starts are actually recorded as live0FAE:20A1/20A6/20A8/
20A9, the trailing call/epilogue of1631:1FDF spanning the physical segment
boundary. No183B combat-parent entry was observed. Region totals are not
function-level coverage certificates.*

The whole trace contains9027 distinct instruction-start addresses;8907 fall
in this region. All8907 **start bytes** match the final relocated image.
The full-region comparison also checks their surrounding code bytes, but this
does not compare every historical decode/register state, recover visit counts
or validate instructions during a possible earlier overwrite/unpack phase.
The listing/CFG may contain disassembled but unexecuted instructions; only the
recorded executed-address set is used for these counts.

## Writable music state explains every difference

Changed offsets relative to204B:
`000E,0010,0012,0015,0016,0017,0018,0019,0205,0206,0207,0208,0209,020A`.
They fall inside known divider/reload, saved IRQ vector, callback FAR,
stream FAR and cadence fields, not instruction bytes. Snapshot views include:

- Divider/reload WORDs000E/0010:000F/0010.
- Saved IRQ vector0012/0014:F000:0006.
- Current callback0016/0018:19C8:0186, corresponding to204B's idle handler.
- Stream0205/0207:1DE9:244B; cadence BYTEs0209/020A:1/4.

IRQ/binding/idle/start paths were visited. PC note emission0187 and stream tick
0298, Tandy note emission01B8 and stream tick024F were **not** recorded as
executed. This does not validate SIF playback, pitch, cadence or audible music.
Calling every204B hit "music playback validated" would be a false conclusion.

## Static gameplay tables also match

These full views match the relocated expanded image, zero differences:

| View | Address | Bytes |
| --- | --- | ---: |
| Eight mech templates |2FE8:02F0|1000|
|33 weapon records |3EDB:2ED8|561|
|11 character-name FAR pointers |3EDB:01CA|44|
|26 BLD filename FAR pointers |3EDB:4EC2|104|
|313-WORD sound library |3EDB:5008|626|

This confirms correspondence of the tables across executable profiles; it
does not settle disputed anatomical names, bit meaning or intended AI policy.

## Executed calls versus maintained main-loop annotations

Observed call edges include:

| Native call site | Target | Maintained interpretation |
| --- | --- | --- |
|0800:0032|1F3D:002F|Pending-input check. |
|0800:0043|1F3D:0259|Live/replay keyboard bridge. |
|0800:0052|1E56:0D1D|Key conversion. |
|0800:007A|0800:218F|Exploration movement handler. |
|0800:0081 /048E|0800:051B|Exploration infantry/mech compositor. |
|0800:0287 /03C9|207F:0BC0|Random-number helper. |
|0800:0353|1631:1FDF|C-bill sidebar formatter/display. |
|0800:045F|0800:24C2|Roaming-NPC update. |

These observed edges support the ASM-backed workflow and relocation transfer.
No high-level C discrepancy was exposed by this comparison, but pseudo-C is
not executable here: equivalent branches, arithmetic and side effects still
need scenario-specific validation. Do not mark a method's whole body verified
merely because its entry address was visited.

## Sprite/state observations and remaining work

All376 FAR sprite entries point inside the captured memory; samples0/12/146
have24-row,3-column headers,16/119 have8-row,1-column headers,124/125 have
14-row,2-column headers and375 has11-row,2-column header. This confirms
instantiation and sampled geometry, not pixel fidelity or temporary clipping
at every draw. Jason's record contains NameId0,Body8,Health80 at the mapped
state base; party position WORDs are packed0C26/C021, not flat pixel numbers.

Next: inspect effective addresses/register state at critical call boundaries,
compare the actual rendered view in DOSBox-X, and capture distinct building,
combat, damage/salvage and audible PC/Tandy music scenarios. Original assets,
memory/flow dumps, generated code and captures remain ignored and local.

Validation: original-file comparison passed, wrong relocation-base rejection
passed,48 memory-reference checks and187,656 startup/music synthetic regression
assertions passed. `git diff --check` passed. Maintained-C edits are evidence
comments only; no native behaviour or high-level algorithm was changed.
