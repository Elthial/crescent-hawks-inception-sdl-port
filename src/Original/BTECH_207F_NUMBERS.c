#include "game.h"

/* Sol: Original207F:3BB6 and3BD2 share the3C15 tail, NOT an independent
 * FAR C entry. Inline that actual tail in both original public routines:
 * no research-core helper enters the preservation build. Explicit WORD
 * division and BYTE digit arithmetic retain even unusual radix behavior.
 * CLD becomes forward C accesses. Radix0/1 nonzero and undersized buffers
 * have no supported host contract; no new validation/clamping is added. */
uint8_t *ASM_Text_Formatting(uint16_t value, uint8_t *text, uint16_t radix)
{
    uint16_t valueLow = value;
    uint16_t valueHigh = radix == NumericRadix_Decimal && (int16_t)value < 0 ? UINT16_MAX : 0;
    uint16_t destinationIndex = 0;
    if (radix == NumericRadix_Decimal && (int16_t)valueHigh < 0)
    {
        text[destinationIndex++] = '-';
        uint16_t originalLow = valueLow;
        valueLow = (uint16_t)(0 - valueLow);
        valueHigh = (uint16_t)(0 - (uint16_t)(valueHigh + (originalLow != 0)));
    }
    uint16_t sourceIndex = destinationIndex;
    do
    {
        uint16_t highQuotient = valueHigh / radix;
        uint16_t highRemainder = valueHigh % radix;
        uint32_t dividend = ((uint32_t)highRemainder << NativeWordBits) | valueLow;
        uint8_t digit = (uint8_t)(dividend % radix);
        valueLow = (uint16_t)(dividend / radix);
        valueHigh = highQuotient;
        uint8_t character = (uint8_t)(digit + '0');
        if (character > '9') character = (uint8_t)(character + NumericLetterDigitAdjustment);
        text[destinationIndex++] = character;
    } while ((valueHigh | valueLow) != 0);
    text[destinationIndex] = 0;
    do
    {
        --destinationIndex;
        uint8_t character = text[sourceIndex++];
        uint8_t lastCharacter = text[destinationIndex];
        text[destinationIndex] = character;
        text[(uint16_t)(sourceIndex-1)] = lastCharacter;
    } while ((uint16_t)(sourceIndex+1) < destinationIndex);
    return text;
}

/* 207F:3BD2/3C08. Sol: Format a DWORD, signed only for decimal. Stored
 * C-bills remain unsigned, but bit31 prints a minus sign in decimal. */
uint8_t *CBill_Text_Formatting(uint32_t value, uint8_t *text, uint16_t radix)
{
    uint16_t valueLow = (uint16_t)value;
    uint16_t valueHigh = (uint16_t)(value >> NativeWordBits);
    uint16_t destinationIndex = 0;
    if (radix == NumericRadix_Decimal && (int16_t)valueHigh < 0)
    {
        text[destinationIndex++] = '-';
        uint16_t originalLow = valueLow;
        valueLow = (uint16_t)(0 - valueLow);
        valueHigh = (uint16_t)(0 - (uint16_t)(valueHigh + (originalLow != 0)));
    }
    uint16_t sourceIndex = destinationIndex;
    do
    {
        uint16_t highQuotient = valueHigh / radix;
        uint16_t highRemainder = valueHigh % radix;
        uint32_t dividend = ((uint32_t)highRemainder << NativeWordBits) | valueLow;
        uint8_t digit = (uint8_t)(dividend % radix);
        valueLow = (uint16_t)(dividend / radix);
        valueHigh = highQuotient;
        uint8_t character = (uint8_t)(digit + '0');
        if (character > '9') character = (uint8_t)(character + NumericLetterDigitAdjustment);
        text[destinationIndex++] = character;
    } while ((valueHigh | valueLow) != 0);
    text[destinationIndex] = 0;
    do
    {
        --destinationIndex;
        uint8_t character = text[sourceIndex++];
        uint8_t lastCharacter = text[destinationIndex];
        text[destinationIndex] = character;
        text[(uint16_t)(sourceIndex-1)] = lastCharacter;
    } while ((uint16_t)(sourceIndex+1) < destinationIndex);
    return text;
}

/* 1F3D:0053. Sol: Format the WORD into native3092:0012 scratch then render.
 * Despite the old unsigned-print summary,3BB6 decimal uses CWD: WORDs
 * 8000..FFFF display as NEGATIVE. Preserve that original behavior. */
void Display_Text_Dynamic_Value(uint16_t value)
{
    ASM_Text_Formatting(value,DynamicString,NumericRadix_Decimal);
    Display_Text_From_Memory(DynamicString);
}

/* 207F:3C6C. Sol: Original signed WORD abs, not modern32-bit libc abs.
 * The most-negative WORD remains8000 after NEG. */
int16_t Native_Abs_Word(int16_t value)
{
    return value < 0 ? (int16_t)(uint16_t)(0 - (uint16_t)value) : value;
}

