#pragma once

#include <sstream>

#if defined _WIN32 || defined WIN32
	#define CONMD_WINDOWS 1
#endif

#ifdef CONMD_WINDOWS
	#include <Windows.h>
	#include <iostream>
#endif

namespace cmdmd
{
template <class C, bool E, class T = std::char_traits<C>, class A = std::allocator<C>>
class ColouredBuffer : public std::basic_stringbuf<C, T, A>
{
public:
	static void StandardInstall();

private:
	typedef
		std::basic_stringbuf<C, T, A>
		stringbuf_type;

	typedef
		std::basic_ostream<C, T>
		ostream_type;

	typedef
		std::basic_streambuf<C, T>
		streambuf_type;

	typedef
		typename T::int_type
		int_type;
	
	typedef
		typename T::pos_type
		pos_type;

	typedef
		typename T::off_type
		off_type;

	typedef
		C
		char_type;

	typedef
		T
		traits_type;

	typedef
		std::streamsize
		size_type;

	streambuf_type &
		buffer_;

	ColouredBuffer() = delete;

#ifdef CONMD_WINDOWS
	char_type const * wstrchr(char_type const * s, char_type const n);

	size_type wstrout(char const * s);

	bool const
		coloured_;

	ostream_type *
		src_;

	uint8_t
		attr0_,
		attr1_,
		attr2_,
		attr3_;

	enum class STATE_E
	{
		NONE,
		ESC,   // Saw `\x1B`.
		START, // Saw `[`.
		A00,   // Saw a number.
		A01,   // Saw two numbers.
		S0,    // Saw a semi-colon.
		A10,   // Saw a number.
		A11,   // Saw two numbers.
		S1,    // Saw a semi-colon.
		A20,   // Saw a number.
		A21,   // Saw two numbers.
		S2,    // Saw a semi-colon.
		A30,   // Saw a number.
		A31,   // Saw two numbers.
		DONE,  // Complete.
		EXTRA_NL, // Insert one extra new line.
		EXTRA_2NL, // Insert two extra new lines.
		EXTRA_SPACE, // Insert one extra space.
		SKIP,  // Multi-byte character.
	} state_ = STATE_E::NONE;

	void Backout()
	{
		// Back out of the buffering.
		char backout[12] = "";
		// Run the state machine.
		switch (state_)
		{
		case STATE_E::NONE:
		case STATE_E::SKIP:
			return;
		case STATE_E::ESC:
			sprintf_s(backout, 12, "\x1B");
			break;
		case STATE_E::START:
			sprintf_s(backout, 12, "\x1B[");
			break;
		case STATE_E::A00:
			sprintf_s(backout, 12, "\x1B[%d", attr0_);
			break;
		case STATE_E::A01:
			sprintf_s(backout, 12, "\x1B[%d", attr0_);
			break;
		case STATE_E::S0:
			sprintf_s(backout, 12, "\x1B[%d;", attr0_);
			break;
		case STATE_E::A10:
			sprintf_s(backout, 12, "\x1B[%d;%d", attr0_, attr1_);
			break;
		case STATE_E::A11:
			sprintf_s(backout, 12, "\x1B[%d;%d", attr0_, attr1_);
			break;
		case STATE_E::S1:
			sprintf_s(backout, 12, "\x1B[%d;%d;", attr0_, attr1_);
			break;
		case STATE_E::A20:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d", attr0_, attr1_, attr2_);
			break;
		case STATE_E::A21:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d", attr0_, attr1_, attr2_);
			break;
		case STATE_E::S2:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d;", attr0_, attr1_, attr2_);
			break;
		case STATE_E::A30:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d;%d", attr0_, attr1_, attr2_, attr3_);
			break;
		case STATE_E::A31:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d;%d", attr0_, attr1_, attr2_, attr3_);
			break;
		}
		if (backout[0])
		{
			Colour();
			wstrout(backout);
		}
		state_ = STATE_E::NONE;
		attr0_ = attr1_ = attr2_ = attr3_ = 0;
	}

	void Colour()
	{
		if (coloured_)
			SetConsoleTextAttribute(console_, colour_);
		else
			SetConsoleTextAttribute(console_, default_);
	}

	WORD GetColour(uint8_t attr)
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

	void GetColours()
	{
		colour_ = GetColour(attr0_);
		if (attr1_)
			colour_ |= GetColour(attr1_);
		if (attr2_)
			colour_ |= GetColour(attr2_);
		if (attr3_)
			colour_ |= GetColour(attr3_);
	}

	HANDLE
		console_;

	static WORD
		default_;

	static bool
		first_;

	WORD
		colour_;
#endif

	template <typename C2>
	inline bool Unicode(C2 c);

	template <>
	inline bool Unicode<char>(char c)
	{
		return (c & 0x80) != 0;
	}

	template <>
	inline bool Unicode<wchar_t>(wchar_t c)
	{
		return (c & 0x8000) != 0;
	}

#ifdef CONMD_WINDOWS
	template <typename C2>
	size_type DoWriteConsole(C2 const * c, size_type len);

	template <>
	size_type DoWriteConsole<wchar_t>(wchar_t const * c, size_type len)
	{
		DWORD
			ret = 0;
		WriteConsoleW(console_, c, (DWORD)len, &ret, NULL);
		return (size_type)ret;
	}

	template <>
	size_type DoWriteConsole<char>(char const * c, size_type len)
	{
		DWORD
			ret = 0;
		WriteConsoleA(console_, c, (DWORD)len, &ret, NULL);
		return (size_type)ret;
	}
#else
	template <typename C2>
	size_type DoWriteConsole(C2 const * c, size_type len)
	{
		if (len == 1)
		{
			buffer_.sputc(*c);
		}
		else
		{
			buffer_.sputn(c, len);
		}
	}
#endif

	template <typename C2>
	size_type DoWriteConsole(C2 const c)
	{
		return DoWriteConsole<C2>(&c, 1);
	}

public:

#ifdef CONMD_WINDOWS
	ColouredBuffer(streambuf_type & underlying, bool coloured = false)
	:
		buffer_(underlying),
		coloured_(coloured),
		console_(GetStdHandle(E ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE)),
		src_(nullptr),
		attr0_(0),
		attr1_(0),
		attr2_(0),
		attr3_(0)
	{
		if (!first_)
		{
			CONSOLE_SCREEN_BUFFER_INFO
				info;
			GetConsoleScreenBufferInfo(console_, &info);
			default_ = info.wAttributes;
			first_ = true;
		}
		colour_ = default_;
	}

	ColouredBuffer(ostream_type & src, bool coloured = false)
	:
		buffer_(*src.rdbuf()),
		coloured_(coloured),
		console_(GetStdHandle(E ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE)),
		src_(&src),
		attr0_(0),
		attr1_(0),
		attr2_(0),
		attr3_(0)
	{
		// Insert ourselves.
		src.rdbuf(this);
		if (!first_)
		{
			CONSOLE_SCREEN_BUFFER_INFO
				info;
			GetConsoleScreenBufferInfo(console_, &info);
			default_ = info.wAttributes;
			first_ = true;
		}
		colour_ = default_;
	}

	~ColouredBuffer()
	{
		if (src_)
			src_->rdbuf(&buffer_);
		SetConsoleTextAttribute(console_, default_);
	}
#else
	// cons
	ColouredBuffer(streambuf_type & underlying, bool coloured)
	:
		buffer_(underlying)
	{
	}

	// cons
	ColouredBuffer(ostream_type & src, bool coloured)
	:
		buffer_(*src.rdbuf())
	{
	}

	// dest
	~ColouredBuffer()
	{
	}
#endif

	int_type overflow(int_type c) override
	{
#ifdef CONMD_WINDOWS
		// Run the state machine.
		switch (state_)
		{
		case STATE_E::NONE:
			if (coloured_)
			{
				switch (c)
				{
				case '\x1D': // Group separator.
					DoWriteConsole<char_type>('\n');
				case '\x1F': // Unit separator.
					return (int_type)DoWriteConsole<char_type>('\n');
				case '\x1E': // Record separator.
					return traits_type::not_eof(0);
				}
			}
			if (c == '\x1B')
			{
				state_ = STATE_E::ESC;
				return traits_type::not_eof(0);
			}
			else if (Unicode<char_type>(c))
			{
				// Start of a multi-byte character.  Skip all checks on the next one.
				state_ = STATE_E::SKIP;
			}
			break;
		case STATE_E::SKIP:
			if (!Unicode<char_type>(c))
			{
				state_ = STATE_E::NONE;
			}
			break;
		case STATE_E::ESC:
			if (c == '[')
			{
				state_ = STATE_E::START;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::START:
			if ('0' <= c && c <= '9')
			{
				attr0_ = (uint8_t)(c - '0');
				state_ = STATE_E::A00;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::A00:
			if ('0' <= c && c <= '9')
			{
				attr0_ = attr0_ * 10 + (uint8_t)(c - '0');
				state_ = STATE_E::A01;
			}
			else if (c == ';')
				state_ = STATE_E::S0;
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::A01:
			if (c == ';')
				state_ = STATE_E::S0;
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::S0:
			if ('0' <= c && c <= '9')
			{
				attr1_ = (uint8_t)(c - '0');
				state_ = STATE_E::A10;
			}
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::A10:
			if ('0' <= c && c <= '9')
			{
				attr1_ = attr1_ * 10 + (uint8_t)(c - '0');
				state_ = STATE_E::A11;
			}
			else if (c == ';')
				state_ = STATE_E::S1;
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::A11:
			if (c == ';')
				state_ = STATE_E::S1;
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::S1:
			if ('0' <= c && c <= '9')
			{
				attr2_ = (uint8_t)(c - '0');
				state_ = STATE_E::A20;
			}
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::A20:
			if ('0' <= c && c <= '9')
			{
				attr2_ = attr2_ * 10 + (uint8_t)(c - '0');
				state_ = STATE_E::A21;
			}
			else if (c == ';')
				state_ = STATE_E::S2;
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::A21:
			if (c == ';')
				state_ = STATE_E::S2;
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::S2:
			if ('0' <= c && c <= '9')
			{
				attr3_ = (uint8_t)(c - '0');
				state_ = STATE_E::A30;
			}
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::A30:
			if ('0' <= c && c <= '9')
			{
				attr3_ = attr3_ * 10 + (uint8_t)(c - '0');
				state_ = STATE_E::A31;
			}
			else if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_E::A31:
			if (c == 'm')
			{
				state_ = STATE_E::DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			break;
		}
		if (state_ == STATE_E::DONE)
		{
			GetColours();
			state_ = STATE_E::NONE;
			attr0_ = attr1_ = attr2_ = attr3_ = 0;
			return traits_type::not_eof(0);
		}
		Colour();
		return (int_type)DoWriteConsole<char_type>((char_type)c);
#else
		return (int_type)DoWriteConsole<char_type>(c);
#endif
	}

protected:
	size_type xsputn(char_type const * s, size_type n) override
	{
#ifdef CONMD_WINDOWS
		char_type const
			* cur,
			* start = s,
			* end = s + n;
		// Just in case.
		if (n == 0 || s == nullptr)
			return 0;
		size_type
			ret = 0;
		for (; ; )
		{
			while (*s && !(state_ == STATE_E::NONE || state_ == STATE_E::SKIP))
			{
				overflow(*s++);
			}
			char_type const *
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
						goto xsputn_loop_done;
					case '\x1D': // Group separator.
						state_ = STATE_E::EXTRA_NL;
						esc = cur;
						goto xsputn_loop_done;
					case '\x1F': // Unit separator.
						state_ = STATE_E::EXTRA_2NL;
						esc = cur;
						goto xsputn_loop_done;
					case '\x1E': // Record separator.
						state_ = STATE_E::NONE;
						esc = cur;
						goto xsputn_loop_done;
					}
				}
xsputn_loop_done:
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
				Colour();
				size_type
					wrote = DoWriteConsole<char_type>(s, esc - s);
				ret += wrote;
				if (wrote != esc - s)
					return s - start + wrote;
			}
			s = esc + 1;
			switch (state_)
			{
			case STATE_E::EXTRA_2NL:
				DoWriteConsole<char_type>('\n');
			case STATE_E::EXTRA_NL:
				DoWriteConsole<char_type>('\n');
				break;
				/*case STATE_E::EXTRA_SPACE:
				DoWriteConsole<char_type>(' ');
				break;*/
			default:
				continue;
			}
			state_ = STATE_E::NONE;
		}
		// Restore the correct colours.
		Colour();
		return s - start + DoWriteConsole<char_type>(s, end - s);
#else
		return DoWriteConsole<char_type>(s, n);
#endif
	}

	int sync() override
	{
#ifdef CONMD_WINDOWS
		// If they flush mid sequence there's not a lot we can do about it!  We can't keep buffering
		// characters under the hood, even via a state machine, if they explicitly requested a flush.
		Backout();
#endif
		return buffer_.pubsync();
	}
};

#ifdef CONMD_WINDOWS
	#pragma warning(push)
	#pragma warning(disable: 4661)
#endif

template class ColouredBuffer<char, true>;
template class ColouredBuffer<wchar_t, true>;
template class ColouredBuffer<char, false>;
template class ColouredBuffer<wchar_t, false>;
};

#ifdef CONMD_WINDOWS
	//#pragma warning(pop)
#endif

