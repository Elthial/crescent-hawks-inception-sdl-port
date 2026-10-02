# Connected medical and ammunition services

Sol: these local-asset fixtures use real Setup_Game, original sprite/tile
loading, original text/menu/input methods, SDL events and actual service
parents. No gameplay, RNG, money or graphics method is a test double. Inputs
are synthetic party/shop states; external assets are read locally, not exported.

## Medical service

`gameplay.local_original_medical_services` calls the actual Citadel treatment
dispatcher and Heal_Characters, using service tier3 and Jason at40/100 Health.
It checks three paths:

- Exact service funds: deduct the native fee, heal within the EXE table's D6
  range with the100-point cap, and set the63-world-tick recovery timer.
- Insufficient funds: do not heal or roll dice, preserve the zero timer, and
  credit the native25-C-bill facilities refund.
- Recovery blocked: the direct dispatcher still deducts its fee before the
  healing helper rejects recovery. Health/RNG/timer remain unchanged. This is
  a direct-call contract, not evidence that normal hospital interaction
  charges while blocked: HOSPITAL.BLD clears the timer before this call.

Real SDL Return events acknowledge the original dialogs after their drains.

## Ammunition service

`gameplay.local_original_ammunition_services` calls actual Mechlube_Buy_Ammo
with one Wasp and three missing SRM-2 rounds. Actual Mech selection and decimal
entry receive SDL keys via main-thread callbacks; the timer worker never reads
game globals. Each scenario confirms selection, resulting ammo and C-bills:

- Buy three rounds with exact funds: fill the bay and spend all funds.
- Funds below one round's price: buy nothing and retain the balance.
- Request nine rounds with room for three: apply the native capacity message
  and cap the purchase to three rounds.

The named SRM-2 component constant1Eh was added to the C17 header, matching
the annotated header and avoiding an unexplained price-table index.

## Evidence limits

All152 SDL/local tests pass. These cases do not exercise navigation into the
buildings, the complete BLD hospital workflow, all medical tiers/equipment,
partial ammo funding, every ammo family or visible/audio parity. Original
diagnostic guards remain unresolved; passing service fixtures does not certify
the full preservation executable as playable.

## Actual hospital BLD workflow

Sol: `gameplay.local_original_hospital_script` now enters the real indexed
HOSPITAL.BLD through Interact_with_BLD, its original loader/decoder and bytecode
interpreter. Synthetic entry state gives Jason Good Medical skill,40/100 Health,
100 C-bills, an already-active63-tick recovery timer, neither medical kit and the
script's professional-help-unavailable flag. No script bytes are copied into
the test and no production call is replaced or suppressed.

Real SDL Return chooses treatment; the script clears the old recovery timer,
charges25 facilities C-bills and invokes own-party healing. The fixture checks
health gain,75 remaining C-bills, service tier zero and a newly-set63-tick timer.
Actual South-key commands then select the native root's last row and the
original FF Exit opcode returns through the building parent's normal cleanup.
The decoded hospital buffer remains loaded, as natively; it is not treated as
an exception to opcode dispatch.

The first fixture timed out because its operator assumed a five-choice root.
With neither kit owned the script selects menu28, which has seven choices;
four South commands selected a purchase rather than Exit. Only the test input
was corrected using the actual menu option count. This exposed no interpreter
error and justified no gameplay change.

All153 SDL/local tests pass. This proves the selected own-party treatment and
normal exit path, not hired-doctor menus, every root inventory variant, walking
into the building or other branches' ConditionalScene calls. ConditionalScene
semantics remain untouched; no cached-hospital scene-skip exception was added.
