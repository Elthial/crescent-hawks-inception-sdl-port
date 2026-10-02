# Sol: original persistent effect registration

Converted complete183B:27C9..2834 from annotated template and full expanded ASM.
Stores sprite low BYTE, packed-page high BYTE, X low7 bits and Y low7 bits.
The sprite/page/X stores reread the live cursor. The Y slot is captured before
the native WORD increment at3092:D557; Y is then stored, and only the cursor's
low BYTE is tested/reset when greater than63. That order is essential for
out-of-ring slots whose Y writes overlap the counter or adjacent menu bytes.

Uses whole OriginalSavedState storage, not separately bounded effect arrays,
so represented aliases remain defined C accesses. Ordinary slots0..63 form
the original ring. Slot64 writes Y onto D557 and may change the final cursor;
slot65 writes Y onto the counter-high/menu BYTE; slots66..68 reach further
represented menu bytes. No new entry guard or ring-index masking was added.

Whole-byte oracle tests cover every slot0..68, every counter-high/menu BYTE
and six paired coordinate patterns, checking all saved/menu bytes for exact
sequential-store effects and untouched surrounding storage. Sprite truncation,
page merging, local-coordinate masking and wrap63 are included.

Contract boundary: all addresses must remain in represented3092:C614..D55B.
Larger corrupted cursor values can reach later globals not yet joined to this
storage, and FF's carry into D558 requires earlier out-of-range stores before
the increment. Those cases are not certified or silently clamped. The WORD
increment itself retains both bytes, but full corrupt-index preservation
needs that wider native-memory representation.

All106 headless /124 SDL-local suites pass. Executable link now has five
unresolved dependencies: allocation, combat mechanics, component salvage,
statistics screen and weapon UI. This is not proof of playable completion.
