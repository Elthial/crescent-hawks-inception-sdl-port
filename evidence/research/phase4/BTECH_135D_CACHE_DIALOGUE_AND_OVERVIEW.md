# Sol: cache dialogue and overview-map swap

Scope135D:0288–03A9. Evidence: original-version local BTECH_135D.asm;
maintained caller Show_Overhead_Map in0800. Research annotations, not compiled
C or native gameplay validation. Security-terminal03AA is outside this block.

## Discovery and dialogue

0288 compares BYTE3092:D34B withzero. Onlyzero dispatches ENDMECH.BLD
(ID16h); every nonzero value suppresses it. This routine does not itself write
PhoenixHawk_Found. Existing C logic is accurate and receives explanatory comments.

02A8 unconditionally opens the message box, displays FAR3EDB:20CE through
17EA's text/retrace wrapper, explicitly reads an ASCII key, and sets WORD
MessageBox_Open D55C=1. The wrapper is not a replacement for the key read.
No new story gate or flag reset was added.

02D2 increments world X as a WORD, masks it with7E, and masks Y with7E.
The X result must equal4E; Y must fall inclusively between0C and11.
Since masking makes Y even, qualifying local Y values are0C,0E,10 only.
Higher region bits and low parity bits are ignored by this native test.
The explicit WORD cast preserves INC wrap, including FFFF->0000. This is
the original coordinate gate, not a full world-coordinate bounding rectangle.

Matching coordinates open the box, display FAR3EDB:2113, read an ASCII key,
then set BYTE HPGTerminal_PowerOn D34C=1 and WORD MessageBox_Open D55C=1.
There is no prior PowerOn test; repeated qualifying calls may repeat the message.
Nonmatching calls do nothing. Existing named coordinate bounds are retained.

## Overview colour-map swap0327

0367 reads the old BYTE at246C:215D+index, CBW-sign-extends it into BP-2,
then restores only its low BYTE after replacing the active value. Thus signed
extension does not change the saved BYTE's identity. The previous C overwrote
the original and copied the replacement into both buffers: a Reko transcription
error, not an original-game bug.

D34E=0 selects alternate base2257; any nonzero selects2351. For each of250
indices, swap the BYTE at215D+index with the selected alternate BYTE. The flag
is tested within the native loop, so the corrected C keeps that placement.
Three named segment-field views replace undefined w-prefixed array expressions.
The index is a WORD; the saved value and both tables are BYTE-valued.

The ranges are215D..2256,2257..2350,2351..244A. The existing256-entry XLAT
domain view at215D is not allocation proof: its last six entries overlap2257.
The swap deliberately covers250, not256. The decoded-asset buffer starts244B,
immediately after the final alternate range, and is untouched by this swap.

Show_Overhead_Map invokes0327 on entry and exit while inside the cache. Two
swaps restore the buffers if D34E is unchanged; no unconditional restoration
claim is made if the selector changes between calls. The alternate names denote
their verified selector association, not an inferred file format or image asset.

## Checks and next boundary

Synthetic assertions cover every WORD X/Y input, all65536 pairs of saved and
replacement BYTEs, double-swap identity, all256 discovery flag values, and full
64KiB memory-range preservation for five selector values. No external assets
are read or copied, and no hardware rendering is claimed.

Passed:590,080 dialogue/swap assertions,11,532 cache-setup regression assertions,
23,115 map-cache regression assertions, and `git diff --check`.

Next:135D:03AA security-terminal lookup and one-use colour-code handling,
then later cache puzzle/door workflows in separately reviewable blocks.
