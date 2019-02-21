#pragma once

#include <string>

namespace cmdmd
{
	::std::string
		Pawn(::std::string const & s, bool ysi = true);

	inline namespace Literals
	{
		::std::string
			operator "" _pawn_syntax(char const * s, size_t len)
		{
			return Pawn(::std::string(s, len));
		}
	}
};

using namespace cmdmd::Literals;

