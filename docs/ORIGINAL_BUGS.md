# Original bugs and compatibility behavior

The supported DOS executable is the behavioral reference. A quirk demonstrated
by assembly or the original runtime is preserved by default, even when it is
undesirable by modern standards.

A change to original behavior is acceptable only when required for host safety,
interoperability or access to a modern platform boundary. Such a change must:

- be the smallest practical intervention;
- remain outside reconstructed game logic whenever possible;
- identify the original procedure/address and observed behavior;
- carry a regression test or reproducible acceptance case; and
- be documented as an SDL/host compatibility decision rather than presented as
  original behavior.

Convenience, stylistic preference and speculative design improvement are not
sufficient reasons to fix an original game bug in the preservation branch.
