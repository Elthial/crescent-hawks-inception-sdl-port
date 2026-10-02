# Sol:1CD3 stock balance and transaction CHECK ASM audit

Scope: stock account display and investment/sale arithmetic only. Evidence is
clean expanded-EXE1CD3 ASM; not UnBattletech, not assumptions from Reko types.
Personnel/movement and unrelated shopping blocks remain unchanged.

3092:D370/D372 is cash DWORD. D374/D376, D378/D37A and D37C/D37E are three
DWORD stock balances. Native stock ID*4 is a byte offset; typed Stock[3] uses
ID, not ID*4.03F0 tests low OR high balance WORD, not its FAR address. Menu
selection and monetary input return WORD ID and DWORD amount respectively.

Investment0822..0854 compares unsigned request against cash: high WORD first,
then low WORD if equal. Request above cash is clamped to cash with the original
message. The3092 literals were false segment extrapolations, not limits.
0910 ADD/ADC credits selected stock; SUB/SBB debits cash. The C had omitted
stock credit entirely behind a broken CHECK ASM comment; it is restored.

Sale05DE..0604 similarly clamps DWORD request to selected stock balance. The
old WORD monetary local, pointer-valued balance and unbound register/record
expressions are replaced.06B0 SUB/SBB debits stock and ADD/ADC credits cash.
The C's cash subtraction was a transcription error, NOT an original-game bug.
Both transfers wrap unsigned32-bit additions as native WORD carry operations;
zero amount bypasses transaction messaging/update via existing branches.

Only these confirmed CHECK ASM markers were resolved; three1CD3 personnel/
skill markers remain. Other stale names, switch/goto workflow and total-stock
availability flagE482 still need their own bounded review. No invented fees,
index validation or overflow policy was added. No external game assets changed.

Verify-StockTransactionTranscriptions.ps1 compares unsigned clamp and WORD
carry/borrow transfers with numeric oracles across boundary balances/requests.
It is synthetic, not execution of annotated C. Runtime arithmetic, text and
menu-selection regressions also pass. Live transaction/UI traces remain needed.

2058 stock assertions,49980 runtime arithmetic,6443 text and3175 menu-selection
assertions pass. Whitespace validation passes; these are not gameplay traces.

Next: queued1CD3 skill display/personnel CHECK ASM locations, or a dedicated
full stock-dialog control-flow pass if desired.

Follow-up: the three skill/personnel markers are now resolved in
[the skill/personnel audit](BTECH_1CD3_SKILL_AND_PERSONNEL.md). The remaining
scope above refers to the state at the time of this stock-only pass.
