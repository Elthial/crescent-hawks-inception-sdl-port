# Sol: original cash display and corrected selector evidence

Converted the whole1631:1FDF method using the annotated template, expanded ASM
and expanded EXE data. Retains layout3, column0/row9, original text control
bytes06/0F and trailing CR, DWORD decimal formatting, signed minimum-width10
right padding, colour restoration and previous-menu restoration.

The earlier systematic audit incorrectly claimed selector3EDB:55C8=3EDB.
Reading the unpacked EXE gives stored WORD2892; the analysis image adds0800,
giving3092. Thus the final ES:37FE WORD15 store DOES restore TextColour.
The unrelocated target text bytes at3EDB:37FE are2065, but they are not the
destination of this write. No betrayal-text corruption bug is justified.
The new C and annotated-method comments correct that claim rather than
preserving a mistaken annotation. Dynamic output bytes likewise now match
the original label, not the old space-padded literal.

Tests call the real original numeric formatter and string-length method,
checking boundary balances including signed DWORD decimal negative values,
padding, original label/control bytes, call order, menu and colour restoration.
UI drawing is isolated; these are not an emulator visual comparison.
All100 headless /118 SDL-local suites pass; executable linkage still has
seven unresolved original dependencies. Oversized allocator05BC requests
remain an unresolved native stack-pointer contract, not a new null substitute.
