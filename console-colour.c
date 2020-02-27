#include "console-colour.h"

#ifdef CONMD_WINDOWS

#include <stdio.h>
#include "subhook/subhook.h"

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

subhook_t
	WriteConsoleA_ = 0,
	WriteConsoleW_ = 0;

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
	}
	if (backout[0])
	{
		Colour(stream);
		OutputA(backout, (int)strlen(backout), stream);
	}
	stream->State = STATE_NONE;
	stream->Attr0 = stream->Attr1 = stream->Attr2 = stream->Attr3 = 0;
}

static WORD GetColour(unsigned char attr, struct stream_s* const stream)
{
	switch (attr)
	{
	case 0:
		// Default.
		return stream->DefaultStyle;
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
	stream->CurrentStyle = GetColour(stream->Attr0, stream);
	if (stream->Attr1)
	{
		stream->CurrentStyle |= GetColour(stream->Attr1, stream);
	}
	if (stream->Attr2)
	{
		stream->CurrentStyle |= GetColour(stream->Attr2, stream);
	}
	if (stream->Attr3)
	{
		stream->CurrentStyle |= GetColour(stream->Attr3, stream);
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
		stream->Attr0 = stream->Attr1 = stream->Attr2 = stream->Attr3 = 0;
		return 0;
	}
	Colour(stream);
	return /*stream->*/OutputC(c, stream);
}

int WriteColouredA(char const * s, int n, struct stream_s* const stream)
{
	subhook_remove(WriteConsoleA_);
	subhook_remove(WriteConsoleW_);
	char const
		* cur,
		* start = s,
		* end = s + n;
	// Just in case.
	if (n == 0 || s == NULL)
	{
		subhook_install(WriteConsoleW_);
		subhook_install(WriteConsoleA_);
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
				subhook_install(WriteConsoleW_);
				subhook_install(WriteConsoleA_);
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
	subhook_install(WriteConsoleW_);
	subhook_install(WriteConsoleA_);
	return (int)(s - start + wrote);
}

int WriteColouredW(wchar_t const * s, int n, struct stream_s* const stream)
{
	subhook_remove(WriteConsoleA_);
	subhook_remove(WriteConsoleW_);
	wchar_t const
		* cur,
		* start = s,
		* end = s + n;
	// Just in case.
	if (n == 0 || s == NULL)
	{
		subhook_install(WriteConsoleW_);
		subhook_install(WriteConsoleA_);
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
				subhook_install(WriteConsoleW_);
				subhook_install(WriteConsoleA_);
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
	subhook_install(WriteConsoleW_);
	subhook_install(WriteConsoleA_);
	return (int)(s - start + wrote);
}

BOOL
WINAPI
Hook_WriteConsoleA(
	_In_ HANDLE hConsoleOutput,
	_In_reads_(nNumberOfCharsToWrite) CONST VOID* lpBuffer,
	_In_ DWORD nNumberOfCharsToWrite,
	_Out_opt_ LPDWORD lpNumberOfCharsWritten,
	_Reserved_ LPVOID lpReserved
)
{
	if (nNumberOfCharsToWrite == 0)
	{
		if (lpNumberOfCharsWritten)
		{
			*lpNumberOfCharsWritten = 0;
		}
		return TRUE;
	}
	gStream.Handle = hConsoleOutput;
	int
		num = WriteColouredA((char const *)lpBuffer, nNumberOfCharsToWrite, &gStream);
	if (lpNumberOfCharsWritten)
	{
		*lpNumberOfCharsWritten = nNumberOfCharsToWrite;
	}
	return TRUE;
	//if (lpNumberOfCharsWritten)
	//{
	//	*lpNumberOfCharsWritten = num;
	//}
	//return num != 0;
}

BOOL
WINAPI
Hook_WriteConsoleW(
	_In_ HANDLE hConsoleOutput,
	_In_reads_(nNumberOfCharsToWrite) CONST VOID* lpBuffer,
	_In_ DWORD nNumberOfCharsToWrite,
	_Out_opt_ LPDWORD lpNumberOfCharsWritten,
	_Reserved_ LPVOID lpReserved
)
{
	if (nNumberOfCharsToWrite == 0)
	{
		if (lpNumberOfCharsWritten)
		{
			*lpNumberOfCharsWritten = 0;
		}
		return TRUE;
	}
	gStream.Handle = hConsoleOutput;
	int
		num = WriteColouredW((wchar_t const*)lpBuffer, nNumberOfCharsToWrite, &gStream);
	if (lpNumberOfCharsWritten)
	{
		*lpNumberOfCharsWritten = nNumberOfCharsToWrite;
	}
	return TRUE;
	//if (lpNumberOfCharsWritten)
	//{
	//	*lpNumberOfCharsWritten = num;
	//}
	//return num != 0;
}

void InitStreamHooks()
{
	CONSOLE_SCREEN_BUFFER_INFO
		info;
	HANDLE
		handle = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(handle, &info);
	gStream.CurrentStyle = gStream.DefaultStyle = info.wAttributes;

	if (!WriteConsoleA_)
	{
		WriteConsoleA_ = subhook_new((void*)&WriteConsoleA, (void*)&Hook_WriteConsoleA, SUBHOOK_64BIT_OFFSET);
	}
	if (WriteConsoleA_)
	{
		subhook_install(WriteConsoleA_);
	}
	if (!WriteConsoleW_)
	{
		WriteConsoleW_ = subhook_new((void*)&WriteConsoleW, (void*)&Hook_WriteConsoleW, SUBHOOK_64BIT_OFFSET);
	}
	if (WriteConsoleW_)
	{
		subhook_install(WriteConsoleW_);
	}
}

void DeinitStreamHooks()
{
	HANDLE
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, gStream.DefaultStyle);
	if (WriteConsoleA_)
	{
		subhook_remove(WriteConsoleA_);
		subhook_free(WriteConsoleA_);
		WriteConsoleA_ = 0;
	}
	if (WriteConsoleW_)
	{
		subhook_remove(WriteConsoleW_);
		subhook_free(WriteConsoleW_);
		WriteConsoleW_ = 0;
	}
}

#else

void InitStreamHooks()
{
}

#endif

