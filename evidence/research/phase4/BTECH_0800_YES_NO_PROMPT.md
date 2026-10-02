# `BTECH_0800` yes/no prompt

## Review block (`0800:1A13-1AFC`)

`0800:1A13` is the game's shared modal yes/no selector. Its maintained name is
now `Prompt_Yes_No`. The sole argument is not a display flag: it chooses the
initial/default answer.

```text
DefaultYes == 0     -> No initially selected
DefaultYes != 0     -> Yes initially selected
```

The routine returns the final selection as a 16-bit Boolean in `AX`: zero for
No and one for Yes.

## Display strings

The executable contains two control-code variants of the same `Yes  No` line:

| DS offset | Display state |
|---:|---|
| `03EE` | Yes highlighted/selected |
| `03FE` | No highlighted/selected |

After every input, the routine decrements the live text-line field at
`3092:374E` and displays the appropriate variant. The text renderer advances
the line again, so this redraws the prompt over the same row rather than adding
another line.

The current EGA text colour at `3092:37FE` is saved before the prompt and
restored before returning because the embedded text-control codes may change
it. The routine also copies `3092:374E` to a stack local at `1A4B`, but the
assembly never reads that saved local; it is dead compiler output, not a
missing restoration.

## Input behavior

Raw keyboard input is passed through `1E56:0D1D`, the same mapper which converts
movement keys into the game's signed internal movement commands.

| Input | Effect | Confirm immediately? |
|---|---|:---:|
| `Y` or `y` | Select Yes | Yes |
| `N` or `n` | Select No | Yes |
| West/left command `FFB5` | Select Yes | No |
| East/right command `FFB3` | Select No | No |
| Enter or Space | Keep current selection | Yes |
| Any other key | Keep current selection | No |

There is no distinct Escape/cancel result in this routine. Unrecognized input
causes the current choice to be redrawn and the input loop continues.

`0800:2A2B` is called once before the loop. In normal play it does nothing; when
the global input-disable/replay flag is active, it drains currently pending
input before the prompt begins.

## BLD integration

BLD opcode `F6` calls `Prompt_Yes_No(1)`, giving Yes the initial highlight. A
Yes result follows the opcode's encoded branch target; No continues after that
target word. This establishes a stable future C# interface:

```text
bool PromptYesNo(bool defaultYes)
```

Rendering and keyboard adaptation can sit behind that interface without
changing the original default-selection or branch semantics.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1A13-1AFC`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, prompt text/control bytes at
  `3EDB:03EE` and `3EDB:03FE`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, BLD `F6` call and branch at
  `0FDC:02BD-02D0`.
