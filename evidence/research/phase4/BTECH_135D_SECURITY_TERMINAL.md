# Sol: security-terminal lookup and code selection

Scope135D:03AA–04AA. Evidence: original-version local BTECH_135D.asm,
and inline tables in BTECH_3EDB.asm. Later transmitter/door routines are not
changed in this block. Research transcription, not native gameplay validation.

03AA increments X as a WORD then masks X/Y with7F. Unlike power-switch7E,
this keeps odd local coordinates. 0454 visits exactly33 WORD-counter indices
0..32; the old <=21 loop added an invalid34th entry. X/Y tables are BYTEs
sign-extended with CBW. A match requires equal Y and signed X in[start,start+3).
No early exit follows a match: native code continues scanning all terminals.

The coordinate tables start3EDB:21CE/21F0 and colour table2212. Each has33
entries followed by one padding byte, rather than34 valid terminal entries.
Colour is a BYTE, not the inferred DWORD in the old header. Actual colours
are0..2 and index three FAR colour-name pointers at2234, stride4, corresponding
to RED/BLUE/YELLOW. The corrected display loads the pointer value, not the
address of that table slot. Signed colour lookup is retained without adding
unverified bounds guards for malformed table values.

Matching terminals open the message box. HasCodeBeenUsed BYTE45DE[index]
zero allows the offer; any nonzero selects the already-used message, retrace
bridge and explicit ASCII key read. Available offers display index+1 to the
player and prompt default-yes. A nonzero answer reloads the colour BYTE and
stores the zero-based index atD347+colour, replacing any prior same-colour code.
No answer changes no selected code. All matching paths set MessageBox_Open
WORDD55C=1; nonmatching paths leave state unchanged.

The named three-byte SelectedCacheCodeByColour_D347 view overlaps existing
RED/BLUE/YELLOW scalar notes; it is not a new sequential header layout.
Crucially this routine DOES NOT write HasCodeBeenUsed. The later0AB6 door
workflow contains those writes and remains queued for its own ASM review.
Selection is not consumption; the original one-use behavior must not be
inferred solely from this prompt. No original-game bug is claimed for the
bad loop bound, BYTE/DWORD confusion or pointer-address transcription.

Synthetic verification covers all WORD X inputs, all256 encoded terminal X
bytes against128 local coordinates, and all33 code IDs with256 used flags
and yes/no responses. Source guards protect the loop/pointer/no-consumption
contract. These do not execute game dialogue or render external assets.

Passed:148,993 terminal assertions,590,080 cache-dialogue/overview regression
assertions, and `git diff --check`.

Next:135D:04AB transmitter gating, then055A map-room discovery/puzzle entry.
