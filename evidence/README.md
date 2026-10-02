# Preservation evidence

This directory keeps the reconstruction's principal evidence inside the same
repository as the C17 source.

- `annotations` contains the human-annotated decompiler output used during
  reconstruction.
- `assembly` contains segmented assembly listings used to settle behavior and
  data-layout questions.
- `metadata` contains the decompiler project description. It does not contain
  the original executable.
- `research` preserves format notes, method inventories, audit records and
  investigation history.

Evidence records may retain old paths, names and superseded hypotheses because
they document how conclusions were reached. Current implementation policy is
defined in `../docs/PRESERVATION_PLAN.md`; executable behavior and assembly are
authoritative when historical notes disagree.

The original executable, captured screenshots and game assets are deliberately
excluded. Historical notes may mention private captures that are not shipped.
Users must supply their own legally obtained installation for runtime and
opt-in tests.
