# `BTECH_11B8` Mech-Lube modification purchase

## Review boundary

This block covers `Mechlube_Upgrade_Mech` from `11B8:0925` through `09CA`:
the price prompt, confirmation, affordability test, charge, and package-selector
gate. The first package mutation begins at `11B8:09CB` and remains the next
review block.

## Prompt and decline path

The routine prints the stored prompt around the WORD prepared by
`Mechlube_Modify_Mech`:

```text
All for the incredibly low price of only [price] C-bills.
Want to modify your 'Mech?
```

`Prompt_Yes_No(1)` consumes the response. Declining jumps directly to the far
return at `0D53`; it does not alter the balance, selected package, or mech and
does not perform an additional keyboard wait.

## Signed WORD cost and 32-bit comparison

The price at `3092:0076` is a signed 16-bit value. The native routine loads it
into `AX` and executes `CWD`, producing a sign-extended `DX:AX`, then compares
that pair with the two WORD halves of the 32-bit C-bill balance. The old
`unsigned long` scratch declaration was therefore both too wide and signed
incorrectly.

All eight real package prices and the accidental selector-eight price are below
`0x8000`, so their sign extension has a zero high WORD. If the extended price
exceeds the balance, the shop emits its shared formatting text twice, displays
the common “Come back when you can afford it.” refusal, waits for one keyboard
input at `0D4E`, and returns.

## Charge and dispatch gate

An affordable confirmed purchase subtracts the sign-extended price from the
32-bit balance with a low-WORD `SUB` and high-WORD `SBB`, then redraws the
balance. Only after charging does the routine load selected package byte
`3092:D31D` and test it against seven.

- selectors `0..7` enter the eight-entry jump table at `11B8:0D31`;
- selectors above seven skip every mutation case and branch to the normal
  completion message at `0D41`.

This ordering completes the evidence for original-game `BUG-010`: unsupported
selector eight is quoted at the accidental `0x0D0D` price, charged 3341
C-bills, applies no modification, and still reaches the ordinary success text.
The cleaned C preserves that behavior explicitly without performing an
out-of-bounds table access.

## Package jump table

The native WORD jump table maps selectors in this order:

| Selector | Target | Package |
|---:|---:|---|
| `0` | `09D3` | Locust first stage |
| `1` | `0A10` | Wasp first stage |
| `2` | `0A98` | Stinger first stage |
| `3` | `0AF3` | Commando first stage |
| `4` | `0B76` | Locust second stage |
| `5` | `0BBF` | Wasp second stage |
| `6` | `0C32` | Stinger second stage |
| `7` | `0CBA` | Commando second stage |

All eight target mutations have now been verified; see
[`BTECH_11B8_MECHLUBE_UPGRADE_PACKAGES.md`](BTECH_11B8_MECHLUBE_UPGRADE_PACKAGES.md).

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:0925-09CA` and jump
  table `0D31-0D3F`;
- exact strings at `3EDB:1D9C`, `1DC8`, and common refusal `4F7E`;
- native `CWD`, two-WORD compare, `SUB`/`SBB`, and post-charge selector bound;
- verified selector and price state at `3092:D31D` and `3092:0076`.

The prompt, decline and refusal paths, signed width, affordability comparison,
charge order, dispatch bound, jump-table mapping, and BUG-010 impact are
directly verified. No Astra review is required for this block.
