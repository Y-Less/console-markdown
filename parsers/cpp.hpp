#pragma once

#include <string>
#include <string.h>

namespace Syntax
{
	::std::string
		CPP(::std::string const & s);

	inline namespace Literals
	{
		::std::string
			operator "" _cpp_syntax(char const * s, size_t len);
	}
};

