#include "game.h"

/* Sol: Original 1E56:03F5..07CA. Buffer words, flush at panel width,
 * and interpret inline background(02), foreground(06), absolute column(09),
 * padding(13), CR and high-bit end markers. Signed WORD comparisons and the
 * original padding bug are retained. 07CB consumes each flushed buffer.
 * Valid game strings/panel widths must fit the original stack buffers; native
 * stack corruption and FAR-segment wrapping are not host memory contracts.
 * No annotation-only FAR-pointer research helper is included. */
enum { DialogueStackBytes=98, DialogueTokenOffset=4, DialogueLineOffset=48,
    TextControl_Background=2, TextControl_Foreground=6,
    TextControl_AbsoluteColumn=9, TextControl_Padding=19,
    TextEndMarker=0x80, TextCharacterMask=0x7F };

void Display_Text_From_Memory(uint8_t *text)
{
	/* Native SS:BP-62 frame; token at BP-5E, line at BP-32. */
	uint8_t textStack[DialogueStackBytes];
	uint8_t *pendingTokenBuffer = textStack + DialogueTokenOffset;
	uint8_t *pendingLineBuffer = textStack + DialogueLineOffset;
	pendingLineBuffer[0] = 0;
	uint16_t stopAfterMarkedByte = 0;
	uint16_t textReadIndex = 0;
	uint16_t restartLineWidth = TextColumn;

ResetPendingLine:;

	uint16_t pendingTokenLength = 0;
	uint16_t pendingLineWidth = restartLineWidth;

	while (1)
	{
		uint16_t nextTextReadIndex = (uint16_t)(textReadIndex + 1);
		uint8_t rawTextByte = text[textReadIndex];
		textReadIndex = nextTextReadIndex;
		uint8_t textControlByte = rawTextByte;

		if (rawTextByte == 0 || stopAfterMarkedByte != 0)
			break;

		if ((rawTextByte & TextEndMarker) != 0)
		{
			textControlByte = rawTextByte & TextCharacterMask;
			stopAfterMarkedByte = 1;
		}

		uint8_t *advancedLineText;

		if (textControlByte == '\r')
		{
			pendingTokenBuffer[pendingTokenLength] = 0;
			if ((int16_t)(uint16_t)(pendingTokenLength + pendingLineWidth) <= (int16_t)TextPanelWidth)
				goto AppendAndAdvanceLine;

			Display_Text_In_TextBox(pendingLineBuffer, 1);
			advancedLineText = pendingTokenBuffer;
			goto FlushAndAdvanceLine;
		}
		if (textControlByte == TextControl_Background)
		{
			uint8_t *backgroundChangeText;
			pendingTokenBuffer[pendingTokenLength] = 0;
			if ((int16_t)(uint16_t)(pendingTokenLength + pendingLineWidth) <= (int16_t)TextPanelWidth)
			{
				Append_Text_To_Memory(pendingLineBuffer, pendingTokenBuffer);
				backgroundChangeText = pendingLineBuffer;
			}
			else
			{
				Display_Text_In_TextBox(pendingLineBuffer, 1);
				backgroundChangeText = pendingTokenBuffer;
			}
			Display_Text_In_TextBox(backgroundChangeText, 0);
			pendingLineWidth = TextColumn;
			TextBackgroundColour = (uint16_t)(int16_t)(int8_t)text[nextTextReadIndex];
			pendingTokenLength = 0;
			textReadIndex = nextTextReadIndex + 1;
			continue;
		}
		if (textControlByte == TextControl_Foreground)
		{
			uint8_t *foregroundChangeText;
			pendingTokenBuffer[pendingTokenLength] = 0;
			if ((int16_t)(uint16_t)(pendingTokenLength + pendingLineWidth) <= (int16_t)TextPanelWidth)
			{
				Append_Text_To_Memory(pendingLineBuffer, pendingTokenBuffer);
				foregroundChangeText = pendingLineBuffer;
			}
			else
			{
				Display_Text_In_TextBox(pendingLineBuffer, 1);
				foregroundChangeText = pendingTokenBuffer;
			}
			Display_Text_In_TextBox(foregroundChangeText, 0);
			pendingLineWidth = TextColumn;
			TextColour = (uint16_t)(int16_t)(int8_t)text[nextTextReadIndex];

			pendingTokenLength = 0;
			textReadIndex = nextTextReadIndex + 1;
			continue;
		}

		if (textControlByte == TextControl_AbsoluteColumn)
		{
			uint8_t *textToFlush;
			pendingTokenBuffer[pendingTokenLength] = 0;
			if ((int16_t)(uint16_t)(pendingTokenLength + pendingLineWidth) <= (int16_t)TextPanelWidth)
			{
				Append_Text_To_Memory(pendingLineBuffer, pendingTokenBuffer);
				textToFlush = pendingLineBuffer;
			}
			else
			{
				Display_Text_In_TextBox(pendingLineBuffer, 1);
				textToFlush = pendingTokenBuffer;
			}
			Display_Text_In_TextBox(textToFlush, 0);
			uint16_t requestedCursorColumn = (uint16_t)(int16_t)(int8_t)text[nextTextReadIndex];
			TextColumn = requestedCursorColumn;

			pendingTokenLength = 0;
			textReadIndex = nextTextReadIndex + 1;
			pendingLineWidth = requestedCursorColumn;

			if ((int16_t)TextColumn < (int16_t)TextPanelWidth)
				continue;

			TextColumn = 0;
			restartLineWidth = 0;
			goto ResetPendingLine;
		}
		if (textControlByte == TextControl_Padding)
		{
			uint16_t paddingTargetColumn = (uint16_t)(int16_t)(int8_t)text[nextTextReadIndex];
			textReadIndex = nextTextReadIndex + 1;
			uint16_t paddingColumnsRemaining = paddingTargetColumn;

			if ((int16_t)TextColumn >= (int16_t)paddingTargetColumn)
				continue;

			for (; (int16_t)TextColumn < (int16_t)paddingColumnsRemaining; --paddingColumnsRemaining)
			{
				pendingTokenBuffer[pendingTokenLength] = ' ';
				++pendingTokenLength;
			}
			goto AppendPendingToken;
		}
		if (textControlByte == ' ')
		{
			uint16_t pendingEndColumn = (uint16_t)(pendingTokenLength + pendingLineWidth);

			if ((int16_t)TextPanelWidth >= (int16_t)pendingEndColumn)
			{
				if (TextPanelWidth != pendingEndColumn && pendingEndColumn != 0)
				{
					pendingTokenBuffer[pendingTokenLength] = ' ';
					++pendingTokenLength;
				}
AppendPendingToken:
				pendingTokenBuffer[pendingTokenLength] = 0;
				Append_Text_To_Memory(pendingLineBuffer, pendingTokenBuffer);
			}
			else
			{
				uint16_t tokenLengthWithSpace = (uint16_t)(pendingTokenLength + 1);
				pendingTokenBuffer[pendingTokenLength] = ' ';
				pendingTokenBuffer[tokenLengthWithSpace] = 0;
				Display_Text_In_TextBox(pendingLineBuffer, 1);
				Append_Large_Text_To_Memory(pendingLineBuffer, pendingTokenBuffer);
				pendingTokenLength = tokenLengthWithSpace;
				pendingLineWidth = 0;
				while (text[textReadIndex] == ' ')
					++textReadIndex;
			}
			pendingLineWidth += pendingTokenLength;
			pendingTokenLength = 0;
			continue;
		}
		if ((int16_t)(uint16_t)(TextPanelWidth - 1) <= (int16_t)pendingTokenLength)
		{
			if (pendingLineBuffer[0] != 0)
			{
				Display_Text_In_TextBox(pendingLineBuffer, 1);
			}
			pendingTokenBuffer[pendingTokenLength] = textControlByte;
			pendingTokenBuffer[pendingTokenLength + 1] = 0;
			++pendingTokenLength;
AppendAndAdvanceLine:
			Append_Text_To_Memory(pendingLineBuffer, pendingTokenBuffer);
			advancedLineText = pendingLineBuffer;
FlushAndAdvanceLine:
			Display_Text_In_TextBox(advancedLineText, 1);
			restartLineWidth = 0;
			goto ResetPendingLine;
		}
		pendingTokenBuffer[pendingTokenLength] = textControlByte;
		++pendingTokenLength;
	}
	if (pendingTokenLength != 0 || pendingLineBuffer[0] != 0)
	{
		uint8_t *textToFlush;
		pendingTokenBuffer[pendingTokenLength] = 0;
		if ((int16_t)(uint16_t)(pendingTokenLength + pendingLineWidth) <= (int16_t)TextPanelWidth)
		{
			Append_Text_To_Memory(pendingLineBuffer, pendingTokenBuffer);
			textToFlush = pendingLineBuffer;
		}
		else
		{
			Display_Text_In_TextBox(pendingLineBuffer, 1);
			textToFlush = pendingTokenBuffer;
		}
		Display_Text_In_TextBox(textToFlush, 0);
	}
}

