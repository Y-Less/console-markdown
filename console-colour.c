#include "console-colour.h"

#ifdef CONMD_WINDOWS

#include <stdio.h>

static int OutputC(wchar_t c, struct stream_s * const stream)
{
	return WriteConsoleW(stream->Handle, &c, 1, NULL, NULL) ? 1 : 0;
}

static int OutputA(char const * c, int len, struct stream_s * const stream)
{
	DWORD
		ret = 0;
	WriteConsoleA(stream->Handle, c, len, &ret, NULL);
	return ret;
}

static int OutputW(wchar_t const * c, int len, struct stream_s * const stream)
{
	DWORD
		ret = 0;
	WriteConsoleW(stream->Handle, c, len, &ret, NULL);
	return ret;
}

struct stream_s
	gStream = {
		0x80,
		STATE_NONE,
		true,
	};

static void Colour(struct stream_s * const stream)
{
	if (!stream->Coloured)
	{
		// Colouring is disabled.  Just do nothing.
		//SetConsoleTextAttribute(stream->Handle, stream->DefaultStyle & ~COMMON_LVB_REVERSE_VIDEO);
	}
	else if (stream->DefaultStyle & COMMON_LVB_REVERSE_VIDEO)
	{
		SetConsoleTextAttribute(stream->Handle, stream->CurrentStyle >> 4 & 0x0F | stream->CurrentStyle << 4 & 0xF0);
	}
	else
	{
		SetConsoleTextAttribute(stream->Handle, stream->CurrentStyle);
	}
}

void Backout(struct stream_s * const stream)
{
	// Back out of the buffering.
	char backout[32] = "";
	// Run the state machine.
	switch (stream->State)
	{
	case STATE_SKIP:
		// Skip might be an issue - we are in the middle of a multi-byte character...
	case STATE_NONE:
		return;
	case STATE_ESC:
		sprintf_s(backout, sizeof (backout), "\x1B");
		break;
	case STATE_START:
		sprintf_s(backout, sizeof (backout), "\x1B[");
		break;
	case STATE_A00:
		sprintf_s(backout, sizeof (backout), "\x1B[%d", stream->Attr0);
		break;
	case STATE_S0:
		sprintf_s(backout, sizeof (backout), "\x1B[%d;", stream->Attr0);
		break;
	case STATE_A10:
		sprintf_s(backout, sizeof (backout), "\x1B[%d;%d", stream->Attr0, stream->Attr1);
		break;
	case STATE_S1:
		sprintf_s(backout, sizeof (backout), "\x1B[%d;%d;", stream->Attr0, stream->Attr1);
		break;
	case STATE_A20:
		sprintf_s(backout, sizeof (backout), "\x1B[%d;%d;%d", stream->Attr0, stream->Attr1, stream->Attr2);
		break;
	case STATE_S2:
		sprintf_s(backout, sizeof (backout), "\x1B[%d;%d;%d;", stream->Attr0, stream->Attr1, stream->Attr2);
		break;
	case STATE_A30:
		sprintf_s(backout, sizeof (backout), "\x1B[%d;%d;%d;%d", stream->Attr0, stream->Attr1, stream->Attr2, stream->Attr3);
		break;
	case STATE_S3:
		sprintf_s(backout, sizeof (backout), "\x1B[%d;%d;%d;%d;", stream->Attr0, stream->Attr1, stream->Attr2, stream->Attr3);
		break;
	case STATE_A40:
		sprintf_s(backout, sizeof (backout), "\x1B[%d;%d;%d;%d;%d", stream->Attr0, stream->Attr1, stream->Attr2, stream->Attr3, stream->Attr4);
		break;
	}
	if (backout[0])
	{
		Colour(stream);
		OutputA(backout, (int)strlen(backout), stream);
	}
	stream->State = STATE_NONE;
	stream->Attr0 = stream->Attr1 = stream->Attr2 = stream->Attr3 = stream->Attr4 = 0;
}

static const WORD
	FOREGROUND = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	BACKGROUND = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;

static WORD NToColour(unsigned char attr)
{
	switch (attr)
	{
		// Original colours.
	case 0:
		// Black.
		return 0;
	case 1:
		// Red.
		return FOREGROUND_RED;
	case 2:
		// Green.
		return FOREGROUND_GREEN;
	case 3:
		// Yellow.
		return FOREGROUND_RED | FOREGROUND_GREEN;
	case 4:
		// Blue.
		return FOREGROUND_BLUE;
	case 5:
		// Magenta.
		return FOREGROUND_RED | FOREGROUND_BLUE;
	case 6:
		// Cyan.
		return FOREGROUND_GREEN | FOREGROUND_BLUE;
	case 7:
		// White.
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	case 8 + 0:
		// Black.
		return FOREGROUND_INTENSITY;
	case 8 + 1:
		// Red.
		return FOREGROUND_RED | FOREGROUND_INTENSITY;
	case 8 + 2:
		// Green.
		return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	case 8 + 3:
		// Yellow.
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	case 8 + 4:
		// Blue.
		return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case 8 + 5:
		// Magenta.
		return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case 8 + 6:
		// Cyan.
		return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case 8 + 7:
		// White.
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	}
	return 0;
}

static unsigned char Shuffle6Colour(unsigned char colour)
{
	// The search results are better using a different search order.
	const unsigned char
		ORDER[16] = { 0, 15, 1, 2, 4, 3, 6, 5, 9, 10, 12, 11, 14, 13, 8, 7 };
	return ORDER[colour];
}

static WORD Make6Colour(int r, int g, int b, struct stream_s* const stream)
{
	// There's no easy way to map 16 colours to 3 components.  So we need full colour space mapping.
	// I didn't want to have to write that...  I KNOW there are better mapping functions based on
	// what colours are eyes are better at perceiving, but I'm only mapping to 16 so it hardly
	// matters.
	//
	// I did a LOT of experimenting with orders to get this as the best order I could find.
	const int
		CONSOLE_COLOURS[16][3] = {
			{0, 0, 0},
			{5, 5, 5},
			{2, 0, 0},
			{0, 2, 0},
			{0, 0, 2},
			{2, 2, 0},
			{0, 2, 2},
			{2, 0, 2},
			{4, 0, 0},
			{0, 4, 0},
			{0, 0, 4},
			{4, 4, 0},
			{0, 4, 4},
			{4, 0, 4},
			{2, 2, 2},
			{4, 4, 4},
	};

	int
		dist = INT_MAX;
	unsigned char
		found = 0;
	for (unsigned char i = 0; i != 16; ++i)
	{
		int
			tr = r - CONSOLE_COLOURS[i][0],
			tg = g - CONSOLE_COLOURS[i][1],
			tb = b - CONSOLE_COLOURS[i][2],
			cur = (tr * tr) + (tg * tg) + (tb * tb);
		if (cur < dist)
		{
			dist = cur;
			found = i;
		}
	}

	return NToColour(Shuffle6Colour(found));
}

static WORD Make24Colour(int r, int g, int b, struct stream_s* const stream)
{
	// Use the reduced 6-value colour space mapping, since we've tweaked that a lot already.
	r = r + 25 / 51; // Rounding division, to get the component from 0-5 (inclusive).
	g = g + 25 / 51; // Rounding division, to get the component from 0-5 (inclusive).
	b = b + 25 / 51; // Rounding division, to get the component from 0-5 (inclusive).
	return Make6Colour(r, g, b, stream);
}

static WORD Make256Colour(unsigned char attr, struct stream_s * const stream)
{
	// Greyscales.  Find the nearest.
	if (attr >= 250)
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	else if (attr >= 244)
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	else if (attr >= 238)
		return FOREGROUND_INTENSITY;
	else if (attr >= 232)
		return 0;
	else if (attr >= 16)
	{
		// 216 colours.  Extract the RGB from the colour.  Then scale them to 8-bit.
		attr -= 16;
		unsigned char
			r = attr / 36,
			g = attr % 36 / 6,
			b = attr % 6;
		return Make6Colour(r, g, b, stream);
	}

	return NToColour(attr);
}

static WORD GetColour(unsigned char attr, struct stream_s* const stream)
{
	WORD
		current = stream->CurrentStyle;
	switch (attr)
	{
	case 1:
		return current | FOREGROUND_INTENSITY;
	case 2:
		return current & ~FOREGROUND_INTENSITY;
	case 3:
	case 7:
		// Inverse.  Store the fact that it is inverted somewhere, and only apply it at the very
		// last possible moment.
		stream->DefaultStyle |= COMMON_LVB_REVERSE_VIDEO;
		break;
	case 23:
	case 27:
		stream->DefaultStyle &= ~COMMON_LVB_REVERSE_VIDEO;
		break;
	case 4:
	case 21:
		return current | COMMON_LVB_UNDERSCORE;
	case 22:
		return current & ~FOREGROUND_INTENSITY;
	case 24:
		return current & ~COMMON_LVB_UNDERSCORE;
	case 39:
		return current & ~FOREGROUND | stream->DefaultStyle & FOREGROUND;
	case 49:
		return current & ~BACKGROUND | stream->DefaultStyle & BACKGROUND;
	case 30:
		// Black.
		return current & ~FOREGROUND;
	case 31:
		// Red.
		return current & ~FOREGROUND | FOREGROUND_RED;
	case 32:
		// Green.
		return current & ~FOREGROUND | FOREGROUND_GREEN;
	case 33:
		// Yellow.
		return current & ~FOREGROUND | FOREGROUND_RED | FOREGROUND_GREEN;
	case 34:
		// Blue.
		return current & ~FOREGROUND | FOREGROUND_BLUE;
	case 35:
		// Magenta.
		return current & ~FOREGROUND | FOREGROUND_RED | FOREGROUND_BLUE;
	case 36:
		// Cyan.
		return current & ~FOREGROUND | FOREGROUND_GREEN | FOREGROUND_BLUE;
	case 37:
		// White.
		return current & ~FOREGROUND | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	case 40:
		// Black.
		return current & ~BACKGROUND;
	case 41:
		// Red.
		return current & ~BACKGROUND | BACKGROUND_RED;
	case 42:
		// Green.
		return current & ~BACKGROUND | BACKGROUND_GREEN;
	case 43:
		// Yellow.
		return current & ~BACKGROUND | BACKGROUND_RED | BACKGROUND_GREEN;
	case 44:
		// Blue.
		return current & ~BACKGROUND | BACKGROUND_BLUE;
	case 45:
		// Magenta.
		return current & ~BACKGROUND | BACKGROUND_RED | BACKGROUND_BLUE;
	case 46:
		// Cyan.
		return current & ~BACKGROUND | BACKGROUND_GREEN | BACKGROUND_BLUE;
	case 47:
		// White.
		return current & ~BACKGROUND | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
	case 60 + 30:
		// Black.
		return current & ~FOREGROUND | FOREGROUND_INTENSITY;
	case 60 + 31:
		// Red.
		return current & ~FOREGROUND | FOREGROUND_RED | FOREGROUND_INTENSITY;
	case 60 + 32:
		// Green.
		return current & ~FOREGROUND | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	case 60 + 33:
		// Yellow.
		return current & ~FOREGROUND | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	case 60 + 34:
		// Blue.
		return current & ~FOREGROUND | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case 60 + 35:
		// Magenta.
		return current & ~FOREGROUND | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case 60 + 36:
		// Cyan.
		return current & ~FOREGROUND | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case 60 + 37:
		// White.
		return current & ~FOREGROUND | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case 60 + 40:
		// Black.
		return current & ~BACKGROUND | BACKGROUND_INTENSITY;
	case 60 + 41:
		// Red.
		return current & ~BACKGROUND | BACKGROUND_RED | BACKGROUND_INTENSITY;
	case 60 + 42:
		// Green.
		return current & ~BACKGROUND | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
	case 60 + 43:
		// Yellow.
		return current & ~BACKGROUND | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
	case 60 + 44:
		// Blue.
		return current & ~BACKGROUND | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
	case 60 + 45:
		// Magenta.
		return current & ~BACKGROUND | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
	case 60 + 46:
		// Cyan.
		return current & ~BACKGROUND | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
	case 60 + 47:
		// White.
		return current & ~BACKGROUND | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
	}
	return current;
}

static void GetColours(struct stream_s * const stream)
{
	switch (stream->Attr0)
	{
	case 0:
		// Reset.
		stream->DefaultStyle &= ~COMMON_LVB_REVERSE_VIDEO;
		stream->CurrentStyle = stream->DefaultStyle;
		break;
	case 38:
		// Special foreground colour.
		// TODO: Find the closest working colour from the 16 available.
		switch (stream->Attr1)
		{
		case 5:
			// 256 colours.
			stream->CurrentStyle = stream->CurrentStyle & ~FOREGROUND | Make256Colour(stream->Attr2, stream);
			break;
		case 2:
			// 24-bit colours.
			stream->CurrentStyle = stream->CurrentStyle & ~FOREGROUND | Make24Colour(stream->Attr2, stream->Attr3, stream->Attr4, stream);
			break;
		}
		break;
	case 48:
		// Special background colour.
		switch (stream->Attr1)
		{
		case 5:
			// 256 colours.
			stream->CurrentStyle = stream->CurrentStyle & ~BACKGROUND | Make256Colour(stream->Attr2, stream) << 4;
			break;
		case 2:
			// 24-bit colours.
			stream->CurrentStyle = stream->CurrentStyle & ~BACKGROUND | Make24Colour(stream->Attr2, stream->Attr3, stream->Attr4, stream) << 4;
			break;
		}
		break;
	default:
		// Normal code.
		stream->CurrentStyle = GetColour(stream->Attr0, stream);
		stream->CurrentStyle = GetColour(stream->Attr1, stream);
		stream->CurrentStyle = GetColour(stream->Attr2, stream);
		stream->CurrentStyle = GetColour(stream->Attr3, stream);
		stream->CurrentStyle = GetColour(stream->Attr4, stream);
		break;
	}
}

static int RunStateMachine(wchar_t c, struct stream_s * const stream)
{
	// Run the stream machine.
	switch (stream->State)
	{
	case STATE_NONE:
		if (stream->Coloured)
		{
			switch (c)
			{
			case 29: // '\x1D', Group separator.
				return /*stream->*/OutputC(10, stream); // `\n`
			case 31: // '\x1F', Unit separator.
				return /*stream->*/OutputC(10, stream); // `\n`
			case 30: // '\x1E', Record separator.
				return 0;
			}
		}
		if (c == '\x1B')
		{
			stream->State = STATE_ESC;
			return 0;
		}
		else if ((c & stream->UnicodeMask))
		{
			// Start of a multi-byte character.  Skip all checks on the next one.
			stream->State = STATE_SKIP;
		}
		break;
	case STATE_SKIP:
		if (!(c & stream->UnicodeMask))
		{
			stream->State = STATE_NONE;
		}
		break;
	case STATE_ESC:
		if (c == '[')
		{
			stream->State = STATE_START;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_START:
		if ('0' <= c && c <= '9')
		{
			stream->Attr0 = (unsigned char)(c - '0');
			stream->State = STATE_A00;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_A00:
		if ('0' <= c && c <= '9')
		{
			stream->Attr0 = stream->Attr0 * 10 + (unsigned char)(c - '0');
		}
		else if (c == ';')
			stream->State = STATE_S0;
		//else if (c == ':')
		//	stream->State = STATE_T416;
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_S0:
		if ('0' <= c && c <= '9')
		{
			stream->Attr1 = (unsigned char)(c - '0');
			stream->State = STATE_A10;
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_A10:
		if ('0' <= c && c <= '9')
		{
			stream->Attr1 = stream->Attr1 * 10 + (unsigned char)(c - '0');
		}
		else if (c == ';')
			stream->State = STATE_S1;
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_S1:
		if ('0' <= c && c <= '9')
		{
			stream->Attr2 = (unsigned char)(c - '0');
			stream->State = STATE_A20;
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_A20:
		if ('0' <= c && c <= '9')
		{
			stream->Attr2 = stream->Attr2 * 10 + (unsigned char)(c - '0');
		}
		else if (c == ';')
			stream->State = STATE_S2;
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_S2:
		if ('0' <= c && c <= '9')
		{
			stream->Attr3 = (unsigned char)(c - '0');
			stream->State = STATE_A30;
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_A30:
		if ('0' <= c && c <= '9')
		{
			stream->Attr3 = stream->Attr3 * 10 + (unsigned char)(c - '0');
		}
		else if (c == ';')
			stream->State = STATE_S3;
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_S3:
		if ('0' <= c && c <= '9')
		{
			stream->Attr4 = (unsigned char)(c - '0');
			stream->State = STATE_A40;
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	case STATE_A40:
		if ('0' <= c && c <= '9')
		{
			stream->Attr4 = stream->Attr4 * 10 + (unsigned char)(c - '0');
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout(stream);
			break;
		}
		return 0;
	}
	if (stream->State == STATE_DONE)
	{
		GetColours(stream);
		stream->State = STATE_NONE;
		stream->Attr0 = stream->Attr1 = stream->Attr2 = stream->Attr3 = stream->Attr4 = 0;
		return 0;
	}
	Colour(stream);
	return /*stream->*/OutputC(c, stream);
}

#if defined CONMD_HOOKS
	#define MAYBE_REMOVE_HOOKS() RemoveStreamHooks()
	#define MAYBE_READD_HOOKS() ReaddStreamHooks()
#else
	#define MAYBE_REMOVE_HOOKS() ((void)0)
	#define MAYBE_READD_HOOKS() ((void)0)
#endif
	
int WriteColouredA(char const * s, int n, struct stream_s* const stream)
{
	MAYBE_REMOVE_HOOKS();
	char const
		* cur,
		* start = s,
		* end = s + n;
	// Just in case.
	if (n == 0 || s == NULL)
	{
		MAYBE_READD_HOOKS();
		return 0;
	}
	int
		ret = 0;
	for (; ; )
	{
		while (*s && !(stream->State == STATE_NONE || stream->State == STATE_SKIP))
		{
			RunStateMachine(*s++, stream);
		}
		char const *
			esc = s;
		if (stream->Coloured)
		{
			for (cur = s; cur != end; ++cur)
			{
				switch (*cur)
				{
				case '\x1B': // ESC.
					stream->State = STATE_ESC;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1D': // Group separator.
					stream->State = STATE_EXTRA_NL;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1F': // Unit separator.
					stream->State = STATE_EXTRA_2NL;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1E': // Record separator.
					stream->State = STATE_NONE;
					esc = cur;
					goto WriteColoured_loop_done;
				}
			}
WriteColoured_loop_done:
			if (cur == end)
			{
				break;
			}
		}
		else
		{
			esc = strchr(s, '\x1B');
			if (esc == NULL)
				break;
			stream->State = STATE_ESC;
		}
		// Write the bit before the escape sequence.
		if (esc != s)
		{
			// Restore the correct colours.
			Colour(stream);
			int
				wrote = OutputA(s, (int)(esc - s), stream);
			ret += wrote;
			if (wrote != esc - s)
			{
				MAYBE_READD_HOOKS();
				return (int)(s - start + wrote);
			}
		}
		s = esc + 1;
		switch (stream->State)
		{
		case STATE_EXTRA_2NL:
			OutputC(10, stream);
		case STATE_EXTRA_NL:
			OutputC(10, stream);
			break;
			/*case STATE_EXTRA_SPACE:
			OutputA(' ');
			break;*/
		default:
			continue;
		}
		stream->State = STATE_NONE;
	}
	// Restore the correct colours.
	Colour(stream);
	int
		wrote = OutputA(s, (int)(end - s), stream);
	MAYBE_READD_HOOKS();
	return (int)(s - start + wrote);
}

int WriteColouredW(wchar_t const * s, int n, struct stream_s* const stream)
{
	MAYBE_REMOVE_HOOKS();
	wchar_t const
		* cur,
		* start = s,
		* end = s + n;
	// Just in case.
	if (n == 0 || s == NULL)
	{
		MAYBE_READD_HOOKS();
		return 0;
	}
	int
		ret = 0;
	for (; ; )
	{
		while (*s && !(stream->State == STATE_NONE || stream->State == STATE_SKIP))
		{
			RunStateMachine(*s++, stream);
		}
		wchar_t const *
			esc = s;
		if (stream->Coloured)
		{
			for (cur = s; cur != end; ++cur)
			{
				switch (*cur)
				{
				case '\x1B': // ESC.
					stream->State = STATE_ESC;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1D': // Group separator.
					stream->State = STATE_EXTRA_NL;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1F': // Unit separator.
					stream->State = STATE_EXTRA_2NL;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1E': // Record separator.
					stream->State = STATE_NONE;
					esc = cur;
					goto WriteColoured_loop_done;
				}
			}
WriteColoured_loop_done:
			if (cur == end)
			{
				break;
			}
		}
		else
		{
			esc = wcschr(s, '\x1B');
			if (esc == NULL)
				break;
			stream->State = STATE_ESC;
		}
		// Write the bit before the escape sequence.
		if (esc != s)
		{
			// Restore the correct colours.
			Colour(stream);
			int
				wrote = OutputW(s, (int)(esc - s), stream);
			ret += wrote;
			if (wrote != esc - s)
			{
				MAYBE_READD_HOOKS();
				return (int)(s - start + wrote);
			}
		}
		s = esc + 1;
		switch (stream->State)
		{
		case STATE_EXTRA_2NL:
			OutputC(10, stream);
		case STATE_EXTRA_NL:
			OutputC(10, stream);
			break;
			/*case STATE_EXTRA_SPACE:
			OutputW(' ');
			break;*/
		default:
			continue;
		}
		stream->State = STATE_NONE;
	}
	// Restore the correct colours.
	Colour(stream);
	int
		wrote = OutputW(s, (int)(end - s), stream);
	MAYBE_READD_HOOKS();
	return (int)(s - start + wrote);
}

#endif

