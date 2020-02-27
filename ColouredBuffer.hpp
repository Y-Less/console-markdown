#pragma once

#include <sstream>
#include "console-colour.h"

#if defined _WIN32 || defined WIN32
	#define CONMD_WINDOWS 1
#endif

#ifdef CONMD_WINDOWS
	#include <Windows.h>
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
	bool
		coloured_;

	struct stream_s* const
		stream_;

	template <typename C2>
	size_type WriteColouredX(C2 const * data, size_type len);

	template <>
	size_type WriteColouredX<char>(char const * data, size_type len)
	{
		return ::WriteColouredA(data, len, stream_);
	}

	template <>
	size_type WriteColouredX<wchar_t>(wchar_t const * data, size_type len)
	{
		return ::WriteColouredA(data, len, stream_);
	}


#endif

public:

#ifdef CONMD_WINDOWS
	//ColouredBuffer(streambuf_type & underlying, bool coloured, bool err)
	//:
	//	buffer_(underlying),
	//	coloured_(coloured),
	//	console_(GetStdHandle(err ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE)),
	//	src_(nullptr),
	//	attr0_(0),
	//	attr1_(0),
	//	attr2_(0),
	//	attr3_(0)
	//{
	//	CONSOLE_SCREEN_BUFFER_INFO
	//		info;
	//	GetConsoleScreenBufferInfo(console_, &info);
	//	default_ = info.wAttributes;
	//	colour_ = default_;
	//}

	ColouredBuffer(ostream_type & src, bool coloured, bool err)
	:
		buffer_(*src.rdbuf()),
		coloured_(coloured),
		stream_(err ? &gCErr : &gCOut)
	{
	}

	~ColouredBuffer()
	{
		if (src_)
		{
			src_->rdbuf(&buffer_);
		}
	}
#else
	// cons
	//ColouredBuffer(streambuf_type & underlying, bool coloured, bool err)
	//:
	//	buffer_(underlying)
	//{
	//}

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
		return WriteColouredX<char_type>(&s, 1);
#else
		return buffer_.sputc(c);
#endif
	}

protected:
	size_type xsputn(char_type const * s, size_type n) override
	{
#ifdef CONMD_WINDOWS
		return WriteColouredX<char_type>(s, n);
#else
		return buffer_.sputn(s, n);
#endif
	}

	int sync() override
	{
#ifdef CONMD_WINDOWS
		// If they flush mid sequence there's not a lot we can do about it!  We can't keep buffering
		// characters under the hood, even via a state machine, if they explicitly requested a flush.
		::Backout();
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
template class ColouredBuffer<char>;
template class ColouredBuffer<wchar_t>;
};

#ifdef CONMD_WINDOWS
	//#pragma warning(pop)
#endif

