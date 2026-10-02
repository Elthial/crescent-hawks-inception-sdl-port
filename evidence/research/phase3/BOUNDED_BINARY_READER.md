# Phase 3C — bounded binary reader

## Review boundary

This block introduces one shared explicit-offset binary reader and migrates the
verified save, weapon, and BLD inspection paths to it. It does not change their
public output models or decoded meanings.

The reader provides:

- byte, signed/unsigned little-endian word, and little-endian dword reads;
- copied byte ranges and fixed-width ASCII strings;
- exact structure-length validation;
- named range diagnostics containing offset, requested width, source, and
  available length.

It deliberately has no host-endian conversion and no implicit cursor. Original
DOS words remain explicitly two-byte little-endian values, avoiding both Reko's
32-bit type guesses and accidental stream-position drift.

## Verification

The harness passes **49 assertions**, including byte/word/dword byte order,
fixed ASCII trimming, copy isolation, range failure, and exact-length failure.
All existing save, weapon, and BLD assertions continue to pass.

This is the foundation for replacing the remaining legacy parsers and for the
future save editor, where every write will need the same explicit boundaries.
