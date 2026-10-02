# `BTECH_0FDC` mission 0 and 1 objectives

## Review boundary

This block covers the mission-specific handlers at `0FDC:0999..0AA7` inside
`Mech_Mission_0629`: the southeast-corner objective for mission 0 and rubble
interaction for mission 1. The jailbreak handler beginning at `0AA8` remains
the next block. Missions `2..8` have no per-redraw handler in this dispatch.

## Mission dispatch after redraw

After every shared world redraw, the routine tests the mission WORD in this
order:

- zero branches to the southeast-corner handler;
- one branches to the rubble handler;
- nine branches to the jailbreak handler;
- every other value returns directly to the common loop condition.

The combat-driven missions therefore finish their objective work before this
point and do not execute additional navigation tests here.

## Mission 0: southeast corner

Each processed world update increments the WORD `MissionTimer`. If byte
`3092:32AE` (`TerrainOverlapRows[0]`) is nonzero, the executable sign-extends
that byte, shifts it arithmetically right by three, and adds the result to the
timer. The former pseudo-C evaluated this expression without assigning it back,
which discarded a real timer adjustment.

The objective triggers once when both unsigned packed-position comparisons
succeed:

```text
packedX >= 0C78
packedY >= C07C
```

The maintained constants are now named `TrainingMission00SoutheastTargetX` and
`TrainingMission00SoutheastTargetY`; their old `Mission01` prefix conflicted
with the zero-based mission constants. On arrival the code increments
`MissionObjectiveResolved`, opens a message box, reports that the southeast
corner has been reached, waits through the text wrapper, and performs a separate
blocking key read. The player must then navigate back to the hangar gate handled
by the common loop.

Teardown later subtracts 50 ticks for a Locust and marks the mission passed when
the objective is resolved and the adjusted signed timer is below `0x00D7`; see
[`BTECH_0FDC_MISSION_TEARDOWN.md`](BTECH_0FDC_MISSION_TEARDOWN.md). No semantic
claim is made here about why terrain-overlap rows alter the timer.

## Mission 1: rubble interaction

The handler extracts each packed coordinate's low seven bits and shifts right
once. These are the reduced X/Y grid values used by this mission's interaction
test; this block does not assume a physical unit such as pixels or half-tiles.
The comparisons are signed (`JGE` and `JLE`), not unsigned.

Interaction occurs only at:

```text
playerObjectiveGridX = rubbleX - 1 or rubbleX
playerObjectiveGridY = rubbleY + 1
```

and only while `MissionObjectiveResolved` is zero. This describes the two
horizontal approach positions immediately south of the selected rubble rather
than a general distance or collision check.

The interaction always increments `MissionObjectiveResolved`. When Jason's
mech-name byte is `L`, the game reports that the Locust has no hands and leaves
the temporary rubble tile in place until teardown. For any other mech it reports
successful retrieval and immediately restores the displaced tile. Both paths
perform the timed wait and then a blocking key read. Whether the encounter
counts as a successful training result is evaluated later; the local flag here
means the interaction has been resolved and permits returning to the hangar.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0999..0AA7`;
- signed `CBW`, `SAR`, `JGE`, and `JLE` instructions in the two handlers;
- unsigned `JNC` threshold tests for mission 0's packed coordinates;
- text and tile-restoration branches at `09ED..0AA7`.

The dispatch, timer arithmetic, thresholds, approach positions, flag changes,
and Locust/non-Locust tile behaviour are directly verified. The later grading
conditions are documented in the teardown review; any removed debrief consumer
remains open. No Astra review is required.
