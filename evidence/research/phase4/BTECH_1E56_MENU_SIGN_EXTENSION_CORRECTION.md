# Menu navigation immediate correction

Sol: found while preparing actual hospital-script integration. The C17 and
annotated menu loops compared positive00B8/00B0, rejecting the actual keyboard
converter's FFB8/FFB0 commands. Prior audit notes incorrectly called this a
native inconsistency. It is a conversion error, not an original-game bug.

The private expanded EXE at1E56:0C40 contains83 7E F8 B8;0C46 contains
83 7E F8 B0. These are CMP WORD [BP-8], sign-extended immediate BYTE. Thus the
compared WORDs areFFB8/FFB0. The generated `.asm` printout displays misleading
positive immediates, but the `.dis` intermediate explicitly retainsFFB8/FFB0.
The repeated previous-command check likewise uses83 7E F8 B8 at0C7C.

Both code versions now compare the actual North/South command values. The
existing SDL-backed menu regression uses the real converter, lowercase letters,
keypad digits, ignored positive00B0, wraparound and XOR highlight pixels.
No original bug was fixed; native menu behaviour was restored.

The hospital script's ConditionalScene opcode remains unchanged. No cached-BLD
exception or scene suppression was introduced to make a fixture pass.
