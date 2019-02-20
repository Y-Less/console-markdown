#pragma once

#include <sstream>

#if defined _WIN32 || defined WIN32
	#define CONMD_WINDOWS 1
#endif

#ifdef CONMD_WINDOWS
	#include <Windows.h>
	#include <iostream>
#endif

template <class C, class T = ::std::char_traits<C>, class A = ::std::allocator<C>>
class ColouredBuffer : public ::std::basic_stringbuf<C, T, A>
{
public:
	static void StandardInstall();

private:
	template <typename U>
	static inline U const * wstrchr(U const * s, U const n)
	{
		return nullptr;
	}

	template <>
	static inline char const * wstrchr<char>(char const * s, char const n)
	{
		return strchr(s, n);
	}

	template <>
	static inline wchar_t const * wstrchr<wchar_t>(wchar_t const * s, wchar_t const n)
	{
		return wcschr(s, n);
	}

	template <typename U>
	static inline ::std::streamsize wstrout(::std::basic_streambuf<U> & buffer, char const * s)
	{
		return 0;
	}

	template <>
	static inline ::std::streamsize wstrout(::std::basic_streambuf<char> & buffer, char const * s)
	{
		return buffer.sputn(s, strlen(s));
	}

	template <>
	static inline ::std::streamsize wstrout(::std::basic_streambuf<wchar_t> & buffer, char const * s)
	{
		wchar_t w[12];
		size_t conv;
		mbstowcs_s(&conv, w, 12, s, 12);
		return buffer.sputn(w, wcslen(w));
	}

private:
	bool const
		coloured_;

	typedef
		::std::basic_stringbuf<C, T, A>
		stringbuf_type;

	typedef
		::std::basic_ostream<C, T>
		ostream_type;

	typedef
		::std::basic_streambuf<C, T>
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

	streambuf_type &
		buffer_;

	ColouredBuffer() = delete;

#ifdef CONMD_WINDOWS
	ostream_type *
		src_;

	uint8_t
		attr0_,
		attr1_,
		attr2_,
		attr3_;

	enum parser_e
	{
		STATE_NONE,
		STATE_ESC,   // Saw `\x1B`.
		STATE_START, // Saw `[`.
		STATE_A00,   // Saw a number.
		STATE_A01,   // Saw two numbers.
		STATE_S0,    // Saw a semi-colon.
		STATE_A10,   // Saw a number.
		STATE_A11,   // Saw two numbers.
		STATE_S1,    // Saw a semi-colon.
		STATE_A20,   // Saw a number.
		STATE_A21,   // Saw two numbers.
		STATE_S2,    // Saw a semi-colon.
		STATE_A30,   // Saw a number.
		STATE_A31,   // Saw two numbers.
		STATE_DONE,  // Complete.
		STATE_EXTRA_NL, // Insert one extra new line.
		STATE_EXTRA_2NL, // Insert two extra new lines.
		STATE_EXTRA_SPACE, // Insert one extra space.
	} state_ = STATE_NONE;

	void Backout()
	{
		// Back out of the buffering.
		char backout[12] = "";
		// Run the state machine.
		switch (state_)
		{
		case STATE_NONE:
			return;
		case STATE_ESC:
			sprintf_s(backout, 12, "\x1B");
			break;
		case STATE_START:
			sprintf_s(backout, 12, "\x1B[");
			break;
		case STATE_A00:
			sprintf_s(backout, 12, "\x1B[%d", attr0_);
			break;
		case STATE_A01:
			sprintf_s(backout, 12, "\x1B[%d", attr0_);
			break;
		case STATE_S0:
			sprintf_s(backout, 12, "\x1B[%d;", attr0_);
			break;
		case STATE_A10:
			sprintf_s(backout, 12, "\x1B[%d;%d", attr0_, attr1_);
			break;
		case STATE_A11:
			sprintf_s(backout, 12, "\x1B[%d;%d", attr0_, attr1_);
			break;
		case STATE_S1:
			sprintf_s(backout, 12, "\x1B[%d;%d;", attr0_, attr1_);
			break;
		case STATE_A20:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d", attr0_, attr1_, attr2_);
			break;
		case STATE_A21:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d", attr0_, attr1_, attr2_);
			break;
		case STATE_S2:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d;", attr0_, attr1_, attr2_);
			break;
		case STATE_A30:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d;%d", attr0_, attr1_, attr2_, attr3_);
			break;
		case STATE_A31:
			sprintf_s(backout, 12, "\x1B[%d;%d;%d;%d", attr0_, attr1_, attr2_, attr3_);
			break;
		}
		if (backout[0])
		{
			Colour();
			wstrout<C>(buffer_, backout);
		}
		state_ = STATE_NONE;
		attr0_ = attr1_ = attr2_ = attr3_ = 0;
	}

	void Colour()
	{
		if (coloured_)
			SetConsoleTextAttribute(stdout_, colour_);
		else
			SetConsoleTextAttribute(stdout_, default_);
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
		stdout_;

	static WORD
		default_;

	static bool
		first_;

	WORD
		colour_;
#endif

public:

#ifdef CONMD_WINDOWS
	ColouredBuffer(streambuf_type & underlying, bool coloured = false)
	:
		buffer_(underlying),
		coloured_(coloured),
		stdout_(GetStdHandle(STD_OUTPUT_HANDLE)),
		src_(nullptr)
	{
		if (!first_)
		{
			CONSOLE_SCREEN_BUFFER_INFO
				info;
			GetConsoleScreenBufferInfo(stdout_, &info);
			default_ = info.wAttributes;
			first_ = true;
		}
		colour_ = default_;
	}

	ColouredBuffer(ostream_type & src, bool coloured = false)
	:
		buffer_(*src.rdbuf()),
		coloured_(coloured),
		stdout_(GetStdHandle(STD_OUTPUT_HANDLE)),
		src_(&src)
	{
		// Insert ourselves.
		src.rdbuf(this);
		if (!first_)
		{
			CONSOLE_SCREEN_BUFFER_INFO
				info;
			GetConsoleScreenBufferInfo(stdout_, &info);
			default_ = info.wAttributes;
			first_ = true;
		}
		colour_ = default_;
	}

	~ColouredBuffer()
	{
		if (src_)
			src_->rdbuf(&buffer_);
		SetConsoleTextAttribute(stdout_, default_);
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
		case STATE_NONE:
			if (coloured_)
			{
				switch (c)
				{
				case '\x1D': // Group separator.
					buffer_.sputc('\n');
				case '\x1F': // Unit separator.
					return buffer_.sputc('\n');
				case '\x1E': // Record separator.
					return traits_type::not_eof(0);
				}
			}
			if (c == '\x1B')
			{
				state_ = STATE_ESC;
				return traits_type::not_eof(0);
			}
		case STATE_ESC:
			if (c == '[')
			{
				state_ = STATE_START;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_START:
			if ('0' <= c && c <= '9')
			{
				attr0_ = (uint8_t)(c - '0');
				state_ = STATE_A00;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_A00:
			if ('0' <= c && c <= '9')
			{
				attr0_ = attr0_ * 10 + (uint8_t)(c - '0');
				state_ = STATE_A01;
			}
			else if (c == ';')
				state_ = STATE_S0;
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_A01:
			if (c == ';')
				state_ = STATE_S0;
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_S0:
			if ('0' <= c && c <= '9')
			{
				attr1_ = (uint8_t)(c - '0');
				state_ = STATE_A10;
			}
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_A10:
			if ('0' <= c && c <= '9')
			{
				attr1_ = attr1_ * 10 + (uint8_t)(c - '0');
				state_ = STATE_A11;
			}
			else if (c == ';')
				state_ = STATE_S1;
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_A11:
			if (c == ';')
				state_ = STATE_S1;
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_S1:
			if ('0' <= c && c <= '9')
			{
				attr2_ = (uint8_t)(c - '0');
				state_ = STATE_A20;
			}
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_A20:
			if ('0' <= c && c <= '9')
			{
				attr2_ = attr2_ * 10 + (uint8_t)(c - '0');
				state_ = STATE_A21;
			}
			else if (c == ';')
				state_ = STATE_S2;
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_A21:
			if (c == ';')
				state_ = STATE_S2;
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_S2:
			if ('0' <= c && c <= '9')
			{
				attr3_ = (uint8_t)(c - '0');
				state_ = STATE_A30;
			}
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_A30:
			if ('0' <= c && c <= '9')
			{
				attr3_ = attr3_ * 10 + (uint8_t)(c - '0');
				state_ = STATE_A31;
			}
			else if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			return traits_type::not_eof(0);
		case STATE_A31:
			if (c == 'm')
			{
				state_ = STATE_DONE;
				break;
			}
			else
			{
				Backout();
				break;
			}
			break;
		}
		if (state_ == STATE_DONE)
		{
			GetColours();
			state_ = STATE_NONE;
			attr0_ = attr1_ = attr2_ = attr3_ = 0;
			return traits_type::not_eof(0);
		}
		Colour();
		return buffer_.sputc((char_type)c);
#else
		return buffer_.sputc(c);
#endif
	}

protected:
	::std::streamsize xsputn(char_type const * s, ::std::streamsize n) override
	{
#ifdef CONMD_WINDOWS
		char_type const
			* cur,
			* start = s,
			* end = s + n;
		// Just in case.
		if (n == 0 || s == nullptr)
			return 0;
		::std::streamsize
			ret = 0;
		for (; ; )
		{
			while (*s && state_ != STATE_NONE)
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
						state_ = STATE_ESC;
						esc = cur;
						goto xsputn_loop_done;
					case '\x1D': // Group separator.
						state_ = STATE_EXTRA_NL;
						esc = cur;
						goto xsputn_loop_done;
					case '\x1F': // Unit separator.
						state_ = STATE_EXTRA_2NL;
						esc = cur;
						goto xsputn_loop_done;
					case '\x1E': // Record separator.
						state_ = STATE_NONE;
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
				esc = wstrchr<C>(s, '\x1B');
				if (esc == nullptr)
					break;
				state_ = STATE_ESC;
			}
			// Write the bit before the escape sequence.
			if (esc != s)
			{
				// Restore the correct colours.
				Colour();
				::std::streamsize
					wrote = buffer_.sputn(s, esc - s);
				ret += wrote;
				if (wrote != esc - s)
					return s - start + wrote;
			}
			s = esc + 1;
			switch (state_)
			{
			case STATE_EXTRA_2NL:
				buffer_.sputc('\n');
			case STATE_EXTRA_NL:
				buffer_.sputc('\n');
				break;
				/*case STATE_EXTRA_SPACE:
				buffer_.sputc(' ');
				break;*/
			default:
				continue;
			}
			state_ = STATE_NONE;
		}
		// Restore the correct colours.
		Colour();
		return s - start + buffer_.sputn(s, end - s);
#else
		return buffer_.sputn(s, n);
#endif
	}

#ifdef CONMD_WINDOWS
	int sync() override
	{
		// If they flush mid sequence there's not a lot we can do about it!  We can't keep buffering
		// characters under the hood, even via a state machine, if they explicitly requested a flush.
		Backout();
		return buffer_.pubsync();
	}
#endif
};

template class ColouredBuffer<char>;
template class ColouredBuffer<wchar_t>;

void
	ColouredBuffer<char>::
	StandardInstall()
{
#ifdef WIN32
	// Redirect `stdout` via us.
	new ColouredBuffer<char>(::std::cout, true);
	new ColouredBuffer<char>(::std::cerr, true);
#endif
}

void
	ColouredBuffer<wchar_t>::
	StandardInstall()
{
#ifdef WIN32
	// Redirect `stdout` via us.
	new ColouredBuffer<wchar_t>(::std::wcout, true);
	new ColouredBuffer<wchar_t>(::std::wcerr, true);
#endif
}

