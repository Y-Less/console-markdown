#pragma once

#include <sstream>
#include "console-colour.h"

#ifdef CONMD_WINDOWS
	#include <iostream>
#endif

namespace cmdmd
{
template <class C, class T = std::char_traits<C>, class A = std::allocator<C>>
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
	bool const
		coloured_;

	ostream_type*
		src_;

	HANDLE
		handle_;

	struct console_colour_stream_s
		stream_;

	struct console_colour_call_s
		call_;

	template <typename C2>
	size_type WriteColouredX(C2 const * data, size_type len);

	template <>
	size_type WriteColouredX<char>(char const * data, size_type len)
	{
		return ::WriteColouredA(data, (int)len, &stream_);
	}

	template <>
	size_type WriteColouredX<wchar_t>(wchar_t const * data, size_type len)
	{
		return ::WriteColouredW(data, (int)len, &stream_);
	}

	static int OutputC(void* data, wchar_t c, struct console_colour_stream_s* const stream);
	static int OutputA(void* data, char const* c, int len, struct console_colour_stream_s* const stream);
	static int OutputW(void* data, wchar_t const* c, int len, struct console_colour_stream_s* const stream);
	static void OutputColour(void* data, unsigned short colour, struct console_colour_stream_s* const stream);

#endif

public:

#ifdef CONMD_WINDOWS
	// cons
	ColouredBuffer(ostream_type & src, bool coloured, bool err = false)
	:
		buffer_(*src.rdbuf()),
		coloured_(coloured),
		src_(&src),
		handle_(GetStdHandle(err ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE)),
		call_{
			&OutputC,
			&OutputA,
			&OutputW,
			&OutputColour,
			(void *)this
		},
		stream_{
			&call_,
			&gConsoleStreamState,
		}
	{
		src_->rdbuf(this);
		CONSOLE_SCREEN_BUFFER_INFO
			info;
		HANDLE
			handle = GetStdHandle(STD_OUTPUT_HANDLE);

		GetConsoleScreenBufferInfo(handle, &info);
		stream_.State->CurrentStyle = stream_.State->DefaultStyle = info.wAttributes;
	}

	// dest
	~ColouredBuffer()
	{
		if (src_)
		{
			src_->rdbuf(&buffer_);
		}
		HANDLE
			handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, stream_.State->DefaultStyle);
	}
#else
	// cons
	ColouredBuffer(ostream_type & src, bool coloured, bool err)
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
		char_type
			s = (char_type)c;
		return (int)WriteColouredX<char_type>(&s, 1);
#else
		return buffer_.sputc(c);
#endif
	}

protected:
	size_type xsputn(char_type const * s, size_type n) override
	{
#ifdef CONMD_WINDOWS
		return (int)WriteColouredX<char_type>(s, n);
#else
		return buffer_.sputn(s, n);
#endif
	}

	int sync() override
	{
#ifdef CONMD_WINDOWS
		// If they flush mid sequence there's not a lot we can do about it!  We can't keep buffering
		// characters under the hood, even via a state machine, if they explicitly requested a flush.
		::Backout(&stream_);
		return 0;
#else
		return buffer_.pubsync();
#endif
	}
};

#ifdef CONMD_WINDOWS
	#pragma warning(push)
	#pragma warning(disable: 4661)
#endif

template class ColouredBuffer<char>;
template class ColouredBuffer<wchar_t>;

#ifdef CONMD_WINDOWS
	#pragma warning(pop)
#endif
};






