# Sol: startup console and drive redirects

Expanded ASM207F:014C and0213 checked in full. Both native INT21h calls use
the argument's low BYTE DL. The retained original entry points redirect to
the separate SDL platform folder; no gameplay method is substituted.

Select-default-drive records the logical BYTE drive. In the installed build,
both floppy asset sets reside in the original-asset working directory, as
original hard-disk installations already do. Separate removable-volume paths
are not implemented; there is no host-drive switch or automatic asset copying.

Startup direct-console output writes and flushes the BYTE to the host console.
FF is the original nonblocking-input selector, not text: it consumes a pending
key and otherwise returns immediately. The original void caller ignores DOS
return registers. This is not an emulation of DOS extended-key byte streams
for hypothetical new callers; startup uses ordinary NUL-terminated text.

An isolated wrapper test checks every65536 WORD argument for both redirects.
The real SDL platform test checks logical-drive truncation and FF consuming a
pending key without blocking when empty. All99 headless /117 SDL-local suites
pass. The real executable link now has eight unresolved dependencies; it is
not yet playable. Console visibility/focus still needs an interactive check
once the complete startup chain links.
