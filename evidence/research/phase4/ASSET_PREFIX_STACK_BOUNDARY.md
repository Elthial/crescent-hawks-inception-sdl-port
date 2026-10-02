# Length-prefix residual stack boundary

Sol: `src/Original/BTECH_1F3D_FILES.c`, native `1F3D:0665`, reads two
bytes into uninitialised SS:BP-2 and immediately uses that WORD as the next
read count. The ASM ignores both read returns and the close return. A complete
prefix therefore has a portable little-endian translation; a failed or partial
prefix also depends on whichever native stack bytes the read did not replace.

The earlier C translation read an uninitialised local BYTE array in that case,
which is host-C undefined behaviour, not a faithful residual-stack model.
It now stops with an explicit unsupported-path diagnostic before that read.
This is **not original game behaviour** and does not finish this exceptional
path's preservation. Do not substitute a zero prefix, retry the prefix, close
and return success, or treat this guard as a native bug fix. Recovery requires
native stack provenance just as the other retained stack-state guards do.

Full prefixes still proceed in native order, ignoring payload/close status;
short payloads retain the destination tail. The existing loader checks cover
successful prefixes, failed payload reads, open retries and raw short reads.
The new subprocess regression checks prefix returns -1, 0 and 1: each must
terminate with the specific diagnostic, rather than succeed or time out.
These tests use synthetic input only; external assets are not embedded.

On MSVC the subprocess disables CRT abort-report dialogs in its test entry
only; production abort behaviour is unchanged. This matters because a dialog
could otherwise masquerade as a stopped process. The harness rejects timeouts
even if the expected diagnostic was printed. The strict headless suite passes
117 tests, the SDL/local-original-asset suite passes 158 tests, and the new
three-case probe passed ten consecutive repetitions. These are bounded loader
and existing workflow checks, not complete gameplay or emulator certification.
