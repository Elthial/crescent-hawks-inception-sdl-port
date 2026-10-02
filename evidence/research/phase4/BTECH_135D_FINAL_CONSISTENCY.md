# Sol: final135D caller/table consistency

All thirteen maintained135D routines now have a first-pass ASM audit, linked
by the preceding block notes. Evidence: original-version local135D ASM,
0800:1EC9–1FB3 caller dispatch, and3EDB inline table/string dump. This is not
native gameplay validation or a claim that the scratchpad C/header compiles.

## Caller contracts

The three0800 door calls pushFFFF, world Y, then world X adjusted by0/-2/-4
for tile codes7E/7F/80. Named CacheDoor_LookupByPosition replaces the literal
without behavior change. Cache entry also usesFFFF, while replay passes0..10.
No reviewed caller supplies00FF. Sol: the fresh 2026-09-17 comparison with `.dis`
corrects this first-pass conclusion: native playback also testsFFFF(-1).
CacheDoor_AnimatedArgument_00FF in current C is wrong, not an intentional native
distinction; its correction is queued. Thirteen header declarations
document the native WORD coordinates and signed WORD replay argument.

0800 dispatch separates map-room ladder94, toggleable97..F0 and submit8C/8D
from outer-cache terminalsB6/B7, transmitter4D, power switches3A/3D and
map-room trigger28. The caller's prior first-pass annotations remain intact;
no broad reordering or independent-to-else-if conversion was performed.

## Table/range ledger

| Native range | Reviewed view / element |
| --- | --- |
| 3092:3248..324B | four saved mech Name BYTEs |
| 246C:0564..07A3 | nine descriptor grids,576 BYTEs |
| 246C:215D..2256 |250 swapped overview colour BYTEs |
| 246C:2257..2350 /2351..244A | alternate250-BYTE colour maps |
| 3EDB:20BC..20CD |18 passage tile BYTEs |
| 3EDB:21CE..21EE /21F0..2210 /2212..2232 |33 terminal X/Y/colour BYTEs; trailing padding |
| 3EDB:2234..223F |three16:16 FAR colour-text pointers |
| 3EDB:241E..242B |seven WORD star tile offsets |
| 3EDB:246E..2479 |three four-BYTE door-animation frames |
| 3EDB:247A..2485 /2486..2491 |twelve door X/Y BYTEs |
| 3EDB:2492..249D /249E..24A9 /24AA..24B5 |twelve required-code BYTEs per colour |
| 3092:D347..D349 |three selected-code BYTEs |
| 3092:D34F..D35A |twelve door latches; replay reads first eleven |
| 3092:45DE..45FE |33 one-use terminal-code BYTEs |
| 3092:4614..4913 |768-BYTE cache tile backup in reusable asset buffer |

Seven target WORD offsets in the original table are008C,00BA,0145,0155,
0177,01B7,01BB. All lie within the768-byte puzzle scan. Required ordinary
door IDs are1..33 before conversion to0..32; entrance11 haszero entries and
bypasses consumption. Overlapping scratchpad aliases remain intentional;
this ledger does not define a sequential compiled struct layout.

Door errors now retain original strings: RED/BLUE end with period and CR;
YELLOW ends with period and no CR. Their omission in inherited literals was
a transcription/display-flow difference, not an original-game fault.

## Completion and unresolved issues

No active undeclared Reko frame/register variables, reversed lookup loops or
undefined old table aliases remain in135D. Native BYTE/WORD/FAR contracts
and reviewed shared-call shapes are explicit. All six block-verification scripts
are rerun; synthetic assertions do not execute original scripts or rendering.

Passed:825,471 assertions across the six block scripts; `git diff --check`
also passed. Counts describe synthetic model assertions, not coverage percentages.

Keep these port obligations visible: correct the C playback sentinel toFFFF
(the earlier possible-native-defect claim is withdrawn); door Y indexing differs from star-toggle indexing;
replay consumes codes and doesn't validate bounded IDs; negative required IDs
for malformed table data aren't sanitized; WHITE success remains latched on
later failure; return does not restore exploration steps. None was silently fixed.
Hardware/runtime research bindings and reusable buffer field declarations still
need a compilable state/serializer model and integration checks. No expensive
model confirmation was needed for this direct instruction-backed audit.

Next suggested block:1FC5 sound setup/dispatch and its BYTE/WORD stream offsets.
