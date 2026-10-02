# Sol: original C runtime data/stack alias evidence

2026-09-18. Static original-EXE evidence, not a new emulator capture.

## Finding

The expanded EXE startup explicitly puts its stack in the C data segment,
then binds DS to SS before C runtime initialization and the main wrapper.
This provides the missing compiler-model explanation for indexed local-array
accesses which lose BP from the effective address and therefore default to DS.
Do not treat every such access as a Reko frame-pointer error or an original bug.

The relevant original image is BTech-Reko-expanded/BTECH.EXE, SHA256
F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE.
The825-paragraph MZ header ends at file offset3390. Its entry is187F:2D82
(analysis207F:2D82), not directly0800:0000. The compiler main wrapper is
analysis0D27:0044 and eventually calls0800:50C8, which calls0800:0000.

## Byte-level evidence

Authoritative startup bytes appear as raw data in BTECH_207F.asm,2D82–2E39,
and as instructions in the broad BTECH_code_0000.asm listing. The latter
contains much non-code elsewhere; these instructions were checked against the
actual EXE, not accepted from the broad listing alone.

| Analysis address | Original instruction | Meaning |
| --- | --- | --- |
|207F:2D8C|`MOV DI,36DB` before relocation|Choose data-segment base; relocation becomes analysis3EDB|
|207F:2D9F|`MOV SS,DI`|Move the stack into that same segment|
|207F:2DA1|`ADD SP,582E`|Retarget stack offset; preserve startup failure branch|
|207F:2DE4|`MOV SS:[52F7],DS`|Save the old PSP DS, not the new C data DS|
|207F:2DE9|`PUSH SS; POP ES`|Use stack/data segment for BSS initialization|
|207F:2DF8|`PUSH SS; POP DS`|Explicit DS=SS before runtime call2E50|
|207F:2DFF|`PUSH SS; POP DS`|Explicit DS=SS again before runtime call31CE|
|207F:2E21|FAR call to original0527:0044|Relocation calls analysis0D27:0044 main wrapper|

File offset01E90D is the36DB immediate, and is a real MZ relocation entry.
The call's segment at01E9A4 is another relocation entry, containing0527.
The termination/reset immediate at01E9AD is36DB and is also relocated.
For the independently established exploration load base017D,36DB+017D=3858.
That matches the captured live SS3858 and the verified mapping of analysis
3EDB to live3858. The terminal DS1DE9 is live246C, not evidence that startup
installed a different permanent C stack/data model. It can be an adapter's
temporary DS; a retrace snapshot alone cannot prove caller restoration.

## Specific consumers and conversion policy

-0800:0E4B collects local parallel mech arrays using SS:BP+DI. Its sort at
  1442 onward computes BX=BP+index and reads/writes DS:[BX-offset]. Actual
  bytes at1442 include8B-77-C6 and8B-7F-C6 with NO SS prefix. Drawing later
  accesses the arrays with BP+SI and consequently SS again.
-1467:0838 crew row-map writeback similarly folds BP into BX, then reads DS.
  This is consistent with the compiler's startup DS=SS model, not sufficient
  by itself to prove an accidental wrong-segment read in ordinary gameplay.
-207F:2FDC stack probing does NOT change DS/SS; startup established the alias.

Normal C local arrays are the intended conversion when the routine's caller
and hardware/runtime children retain the startup DS=SS contract. Before
certifying the whole workflow, check DS-changing children save/restore the
caller register and retain an emulator comparison at the relevant call site.
Do not manufacture separate file pointers, global copies of stack locals or a
zero-initialized substitute. Do not emulate arbitrary corrupted DS state as if
it were an established original gameplay requirement.

Remaining runtime confirmation: capture DS,SS,BP at0800:1442 and1467:0838,
compare the actual effective array addresses and contents, and verify the
native sort/crew outputs. The startup alias itself is now byte-confirmed;
the point-in-time caller invariant and gameplay outcomes remain unconfirmed.
No expensive-model pass was started or required for this static finding.
