# `BTECH_0FDC` mission setup dispatch

## Review boundary

This block covers the entry and ten-way setup dispatch in `Mech_Mission_0629`,
from `0FDC:0629` through the common boundary at `0883`. The shared controller
beginning there is reviewed in
[`BTECH_0FDC_MISSION_COMMON_LOOP.md`](BTECH_0FDC_MISSION_COMMON_LOOP.md).

The routine services the named training missions `0..7`, the still
story-unnamed mission mode `8`, and the jailbreak numbered `9`. The assembly
accepts the mission number as a native WORD at `[BP+06]`; the previous byte
parameter was a Reko type error. Verified callers supply only this range.
Values above 9 bypass the setup switch rather than being explicitly rejected,
so new callers must retain the established range contract.

## Setup table

| Mission | Setup before the common loop |
|---:|---|
| `00` | Save and clear map byte `246C:200A`, clear the mission timer, and perform an initial zero-command movement update. |
| `01` | Select one of eight rubble coordinates, replace its tile with `50`, optionally show the Locust warning, and perform the same movement update. |
| `02` | Generate enemies with arguments `(1,0)`, clear the first two actuator bytes, all ten current-ammo bytes, and the walk/jump bytes of enemy mech 0, then enter combat mode 1. |
| `03` | Generate enemies with arguments `(0, 3..6)`, where the second value comes from the low two random bits, then enter combat mode 1. |
| `04` | Set the no-computer-control WORD before falling into the common `04..07` setup. |
| `05` | Use the common `04..07` setup without an additional flag. |
| `06` | Use common setup and give the Kurita interruption a 50% chance. |
| `07` | Use common setup and force the Kurita interruption. |
| `08` | Generate enemies with arguments `(81,0)`, enter combat mode 2, and mark the objective resolved. Its higher-level story name remains open. |
| `09` | Enable parked-jail-mech drawing, rebuild and draw the current view, clear the timer, and initialize four attempted-mech WORD flags plus the tested-mech count. |

For missions `04..07`, a nonzero Kurita flag fills map indexes `0FF0..0FFF`
with tile `40`, then the code calls the enemy generator with
`(missionNumber - 3, 0)`, enters combat mode 1, restores the pre-combat packed
position, clears no-computer-control, and marks the mission objective resolved.

## Mission 0 map byte

The old source treated absolute `246C:200A` as an unexplained standalone field.
The tile payload begins at `246C:101D`, making this exactly:

```text
200A - 101D = 0FED
```

It is therefore named `MapTile[CitadelMission00TemporaryTileIndex]`. Mission 0
saves its byte, writes zero for the mission, and restores it during teardown.

## Rubble coordinate conversion

The two eight-byte tables at `DS:1632` and `DS:163A` are paired candidate X/Y
coordinates. One random index selects both. The source now names them
`TrainingRubbleTargetX_1632` and `TrainingRubbleTargetY_163A`.

The selected coordinate is converted to the MTP tile payload's 8-by-8
block-major order:

```text
index = ((y & 78) << 6)
      + ((y & 07) << 3)
      + ((x & 78) << 3)
      +  (x & 07)
```

This is not the conventional `y * 128 + x` layout. It selects a 64-byte tile
block first, then the tile's row and column within that block. The displaced
tile and computed index survive until mission teardown so the original byte can
be restored.

## Jailbreak attempt state

The prior transcription left four stack writes as invalid `fp` arithmetic.
Assembly shows four WORD slots at `BP-0C`, `BP-0A`, `BP-08`, and `BP-06`, all
cleared in a four-iteration loop. These are now represented as
`JailMechAttempted[4]`. Later jailbreak logic uses the selected parked-mech index
to ensure each mech is tested only once. `JailDistinctMechAttempts` at `BP-24`
begins at zero and is updated for distinct attempts. The later old-value
comparison proves that attempts one and two fail and the third distinct attempt
succeeds; see the dedicated jailbreak-loop review.

## Names and remaining uncertainty

`MissionObjectiveResolved` is the WORD at `BP-02`. Missions which return from
combat set it immediately; navigation objectives set it later in the common
loop. `PrematureReturnWarningShown` at `BP-0E` prevents the training-centre
warning from being repeated. Their uses retain those identities through the
reviewed common loop, objective handlers, jailbreak handler, and
[`mission teardown`](BTECH_0FDC_MISSION_TEARDOWN.md).

The precise semantic names of the first two actuator bytes cleared by mission 2
remain dependent on the outstanding actuator-layout work. The transcription
therefore retains indexed access rather than asserting particular limb parts.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0629..0882`;
- the ten-entry jump table at `086F..0881`;
- mission callers in `BTECH_1CD3.c` and the jailbreak wrapper at `11B8:152F`;
- map payload base `246C:101D` and the direct byte access at `246C:200A`.

The switch mapping, stack widths, calls, flags, tile indexes, and rubble index
formula are directly verified. Mission 8's story-level label and the two
actuator meanings remain open. No Astra review is currently required.
