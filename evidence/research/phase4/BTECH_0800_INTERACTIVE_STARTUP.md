# `BTECH_0800` interactive startup branch

## Reviewed block

- Address range: `0800:5126-51F8`.
- Parent routine: `Start_Game`.
- Idle attract-mode setup begins at `0800:51F9`.

This block tests for title-screen input. If input is present, it prepares a
fresh game state, optionally offers to load a save, enters normal interactive
play, and then arranges for `Start_Game` to return.

## Input split

The word at `3092:3938` is cleared before `1F3D:002F` tests for pending input.
A zero result branches to the idle-demo path at `51F9`; any nonzero result:

1. drains pending keyboard input;
2. constructs and draws a fresh initial game using `Load_Game_Map_Data`; and
3. either displays or bypasses the startup questions according to `458C`.

`3092:458C` is conclusively a 16-bit word, not the byte formerly declared in
`BTECH.h`. A nonzero value bypasses the legal/first-time prompt sequence. It is
also consulted by the keyboard/replay code, but its producer and best domain
name remain unproven, so the cautious `Bool_Input_458C` name is retained.

## Exact prompt flow

When `458C` is zero, the game draws menu border zero and displays four legal
notices followed by:

```text
Is this your first time playing BattleTech?
```

`Prompt_Yes_No(TRUE)` returns one for Yes and zero for No. The executable
branches directly to play on Yes. Only a No response displays:

```text
You can load a previously saved game, or start a new game.  Do you want to load an old game?
```

A Yes response to this second prompt invokes `Load_Game`; No retains the fresh
state already created. The old annotated C incorrectly nested the load question
under a Yes response to the first-time question.

The legal strings in the maintained C now match `3EDB:0B0E-0C38`, including
the previously omitted board-game credit and Infocom copyright line. These
strings are executable-owned material explicitly permitted for preservation
source; no external copyrighted asset has been copied.

## Entering and leaving normal play

Both fresh and loaded paths call `Main_Game_Loop(FALSE)`, where zero selects
normal interactive play rather than attract replay. When it returns, `458C`
and the native continue-loop local are cleared, causing `Start_Game` to return
through its tail.

The enclosing high-level loop has now been reconstructed from its attract-mode
branch and tail. No Astra confirmation is required for this branch: the prompt
return convention, jumps, word widths, and string offsets are all explicit.
