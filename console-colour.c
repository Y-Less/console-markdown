#include "console-colour.h"

enum STATE_E
{
	STATE_NONE,
	STATE_ESC,         // Saw `\x1B`.
	STATE_START,       // Saw `[`.
	STATE_A00,         // Saw a number.
	STATE_A01,         // Saw two numbers.
	STATE_S0,          // Saw a semi-colon.
	STATE_A10,         // Saw a number.
	STATE_A11,         // Saw two numbers.
	STATE_S1,          // Saw a semi-colon.
	STATE_A20,         // Saw a number.
	STATE_A21,         // Saw two numbers.
	STATE_S2,          // Saw a semi-colon.
	STATE_A30,         // Saw a number.
	STATE_A31,         // Saw two numbers.
	STATE_DONE,        // Complete.
	STATE_EXTRA_NL,    // Insert one extra new line.
	STATE_EXTRA_2NL,   // Insert two extra new lines.
	STATE_EXTRA_SPACE, // Insert one extra space.
	STATE_SKIP,        // Multi-byte character.
};

struct stream_s;

//typedef int (* OutputC_t)(wchar_t c, struct stream_s * const stream);
//typedef int (* OutputA_t)(char const * c, int len, struct stream_s * const stream);
//typedef int (* OutputW_t)(wchar_t const * c, int len, struct stream_s * const stream);

struct stream_s
{
	wchar_t
		UnicodeMask;

	enum STATE_E 
		State;

	bool
	//	Wide,
		Coloured,
		Error;

	HANDLE
		Handle;

	WORD
		DefaultStyle,
		CurrentStyle;

	uint8_t
		Attr0,
		Attr1,
		Attr2,
		Attr3;
	
	//OutputC_t
	//	OutputC;
	//
	//OutputA_t
	//	OutputA;
	//
	//OutputW_t
	//	OutputW;
	
	void *
		Data;
};

#ifdef CONMD_WINDOWS

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
	//gWOut = {
	//	0x8000,
	//	STATE_NONE,
	//	true,
	//	true,
	//	false,
	//},
	//gWErr = {
	//	0x8000,
	//	STATE_NONE,
	//	true,
	//	true,
	//	true,
	//},
	gCOut = {
		0x80,
		STATE_NONE,
	//	false,
		true,
		false,
	},
	gCErr = {
		0x80,
		STATE_NONE,
	//	false,
		true,
		true,
	};

void InitStreamHooks()
{
	gCOut->Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	gCErr->Handle = GetStdHandle(STD_ERROR_HANDLE);
}

static void Colour(struct stream_s * const stream)
{
	if (stream->Coloured)
	{
		SetConsoleTextAttribute(stream->Handle, stream->CurrentStyle);
	}
	else
	{
		SetConsoleTextAttribute(stream->Handle, stream->DefaultStyle);
	}
}

static void Backout(struct stream_s * const stream)
{
	// Back out of the buffering.
	char backout[12] = "";
	// Run the state machine.
	switch (stream->State)
	{
	case STATE_SKIP:
		// Skip might be an issue - we are in the middle of a multi-byte character...
	case STATE_NONE:
		return;
	case STATE_ESC:
		sprintf_s(backout, 12, "\x1B");
		break;
	case STATE_START:
		sprintf_s(backout, 12, "\x1B[");
		break;
	case STATE_A00:
		sprintf_s(backout, 12, "\x1B[%d", stream->Attr0);
		break;
	case STATE_A01:
		sprintf_s(backout, 12, "\x1B[%d", stream->Attr0);
		break;
	case STATE_S0:
		sprintf_s(backout, 12, "\x1B[%d;", stream->Attr0);
		break;
	case STATE_A10:
		sprintf_s(backout, 12, "\x1B[%d;%d", stream->Attr0, stream->Attr1);
		break;
	case STATE_A11:
		sprintf_s(backout, 12, "\x1B[%d;%d", stream->Attr0, stream->Attr1);
		break;
	case STATE_S1:
		sprintf_s(backout, 12, "\x1B[%d;%d;", stream->Attr0, stream->Attr1);
		break;
	case STATE_A20:
		sprintf_s(backout, 12, "\x1B[%d;%d;%d", stream->Attr0, stream->Attr1, stream->Attr2);
		break;
	case STATE_A21:
		sprintf_s(backout, 12, "\x1B[%d;%d;%d", stream->Attr0, stream->Attr1, stream->Attr2);
		break;
	case STATE_S2:
		sprintf_s(backout, 12, "\x1B[%d;%d;%d;", stream->Attr0, stream->Attr1, stream->Attr2);
		break;
	case STATE_A30:
		sprintf_s(backout, 12, "\x1B[%d;%d;%d;%d", stream->Attr0, stream->Attr1, stream->Attr2, stream->Attr3);
		break;
	case STATE_A31:
		sprintf_s(backout, 12, "\x1B[%d;%d;%d;%d", stream->Attr0, stream->Attr1, stream->Attr2, stream->Attr3);
		break;
	}
	if (backout[0])
	{
		Colour();
		wstrout(backout);
	}
	stream->State = STATE_NONE;
	stream->Attr0 = stream->Attr1 = stream->Attr2 = stream->Attr3 = 0;
}

static WORD GetColour(uint8_t attr)
{
	switch (attr)
	{
	case 0:
		// Default.
		return default_;
	case 1:
		// Bold.
		return FOREGROUND_INTENSITY;
	case 31:
		// Red.
		return FOREGROUND_RED;
	case 32:
		// Green.
		return FOREGROUND_GREEN;
	case 33:
		// Yellow.
		return FOREGROUND_RED | FOREGROUND_GREEN;
	case 34:
		// Blue.
		return FOREGROUND_BLUE;
	case 35:
		// Magenta.
		return FOREGROUND_RED | FOREGROUND_BLUE;
	case 36:
		// Cyan.
		return FOREGROUND_GREEN | FOREGROUND_BLUE;
	case 37:
		// White.
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	case 5:
		// Blink (replace it).
		return BACKGROUND_INTENSITY;
	case 41:
		// Red.
		return BACKGROUND_RED;
	case 42:
		// Green.
		return BACKGROUND_GREEN;
	case 43:
		// Yellow.
		return BACKGROUND_RED | BACKGROUND_GREEN;
	case 44:
		// Blue.
		return BACKGROUND_BLUE;
	case 45:
		// Magenta.
		return BACKGROUND_RED | BACKGROUND_BLUE;
	case 46:
		// Cyan.
		return BACKGROUND_GREEN | BACKGROUND_BLUE;
	case 47:
		// White.
		return BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
	}
	return 0;
}

static void GetColours(struct stream_s * const stream)
{
	CurrentStyle = GetColour(stream->Attr0);
	if (stream->Attr1)
	{
		CurrentStyle |= GetColour(stream->Attr1);
	}
	if (stream->Attr2)
	{
		CurrentStyle |= GetColour(stream->Attr2);
	}
	if (stream->Attr3)
	{
		CurrentStyle |= GetColour(stream->Attr3);
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
			case 29; // '\x1D', Group separator.
				return /*stream->*/OutputC(10, stream); // `\n`
			case 31; // '\x1F', Unit separator.
				return /*stream->*/OutputC(10, stream); // `\n`
			case 30; // '\x1E', Record separator.
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
			Backout();
			break;
		}
		return 0;
	case STATE_START:
		if ('0' <= c && c <= '9')
		{
			stream->Attr0 = (uint8_t)(c - '0');
			stream->State = STATE_A00;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_A00:
		if ('0' <= c && c <= '9')
		{
			stream->Attr0 = stream->Attr0 * 10 + (uint8_t)(c - '0');
			stream->State = STATE_A01;
		}
		else if (c == ';')
			stream->State = STATE_S0;
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_A01:
		if (c == ';')
			stream->State = STATE_S0;
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_S0:
		if ('0' <= c && c <= '9')
		{
			stream->Attr1 = (uint8_t)(c - '0');
			stream->State = STATE_A10;
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_A10:
		if ('0' <= c && c <= '9')
		{
			stream->Attr1 = stream->Attr1 * 10 + (uint8_t)(c - '0');
			stream->State = STATE_A11;
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
			Backout();
			break;
		}
		return 0;
	case STATE_A11:
		if (c == ';')
			stream->State = STATE_S1;
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_S1:
		if ('0' <= c && c <= '9')
		{
			stream->Attr2 = (uint8_t)(c - '0');
			stream->State = STATE_A20;
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_A20:
		if ('0' <= c && c <= '9')
		{
			stream->Attr2 = stream->Attr2 * 10 + (uint8_t)(c - '0');
			stream->State = STATE_A21;
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
			Backout();
			break;
		}
		return 0;
	case STATE_A21:
		if (c == ';')
			stream->State = STATE_S2;
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_S2:
		if ('0' <= c && c <= '9')
		{
			stream->Attr3 = (uint8_t)(c - '0');
			stream->State = STATE_A30;
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_A30:
		if ('0' <= c && c <= '9')
		{
			stream->Attr3 = stream->Attr3 * 10 + (uint8_t)(c - '0');
			stream->State = STATE_A31;
		}
		else if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		return 0;
	case STATE_A31:
		if (c == 'm')
		{
			stream->State = STATE_DONE;
			break;
		}
		else
		{
			Backout();
			break;
		}
		break;
	}
	if (stream->State == STATE_DONE)
	{
		GetColours(stream);
		stream->State = STATE_NONE;
		stream->Attr0 = stream->Attr1 = stream->Attr2 = stream->Attr3 = 0;
		return 0;
	}
	Colour();
	return /*stream->*/OutputC(c, stream);
}

int WriteColouredA(char const * s, int n) override
{
	char const
		* cur,
		* start = s,
		* end = s + n;
	// Just in case.
	if (n == 0 || s == nullptr)
		return 0;
	int
		ret = 0;
	for (; ; )
	{
		while (*s && !(state_ == STATE_E::NONE || state_ == STATE_E::SKIP))
		{
			RunStateMachine(*s++, stream);
		}
		char const *
			esc = s;
		if (coloured_)
		{
			for (cur = s; cur != end; ++cur)
			{
				switch (*cur)
				{
				case '\x1B': // ESC.
					state_ = STATE_E::ESC;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1D': // Group separator.
					state_ = STATE_E::EXTRA_NL;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1F': // Unit separator.
					state_ = STATE_E::EXTRA_2NL;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1E': // Record separator.
					state_ = STATE_E::NONE;
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
			if (esc == nullptr)
				break;
			state_ = STATE_E::ESC;
		}
		// Write the bit before the escape sequence.
		if (esc != s)
		{
			// Restore the correct colours.
			Colour(stream);
			int
				wrote = OutputA(s, esc - s, stream);
			ret += wrote;
			if (wrote != esc - s)
				return s - start + wrote;
		}
		s = esc + 1;
		switch (state_)
		{
		case STATE_E::EXTRA_2NL:
			OutputC(10, stream);
		case STATE_E::EXTRA_NL:
			OutputC(10, stream);
			break;
			/*case STATE_E::EXTRA_SPACE:
			OutputA(' ');
			break;*/
		default:
			continue;
		}
		state_ = STATE_E::NONE;
	}
	// Restore the correct colours.
	Colour(stream);
	return s - start + OutputA(s, end - s, stream);
}

int WriteColouredW(wchar_t const * s, int n)
{
	wchar_t const
		* cur,
		* start = s,
		* end = s + n;
	// Just in case.
	if (n == 0 || s == nullptr)
		return 0;
	int
		ret = 0;
	for (; ; )
	{
		while (*s && !(state_ == STATE_E::NONE || state_ == STATE_E::SKIP))
		{
			RunStateMachine(*s++, stream);
		}
		wchar_t const *
			esc = s;
		if (coloured_)
		{
			for (cur = s; cur != end; ++cur)
			{
				switch (*cur)
				{
				case '\x1B': // ESC.
					state_ = STATE_E::ESC;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1D': // Group separator.
					state_ = STATE_E::EXTRA_NL;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1F': // Unit separator.
					state_ = STATE_E::EXTRA_2NL;
					esc = cur;
					goto WriteColoured_loop_done;
				case '\x1E': // Record separator.
					state_ = STATE_E::NONE;
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
			esc = wstrchr(s, '\x1B');
			if (esc == nullptr)
				break;
			state_ = STATE_E::ESC;
		}
		// Write the bit before the escape sequence.
		if (esc != s)
		{
			// Restore the correct colours.
			Colour(stream);
			int
				wrote = OutputW(s, esc - s, stream);
			ret += wrote;
			if (wrote != esc - s)
				return s - start + wrote;
		}
		s = esc + 1;
		switch (state_)
		{
		case STATE_E::EXTRA_2NL:
			OutputC(10, stream);
		case STATE_E::EXTRA_NL:
			OutputC(10, stream);
			break;
			/*case STATE_E::EXTRA_SPACE:
			OutputW(' ');
			break;*/
		default:
			continue;
		}
		state_ = STATE_E::NONE;
	}
	// Restore the correct colours.
	Colour(stream);
	return s - start + OutputW(s, end - s, stream);
}

#endif

