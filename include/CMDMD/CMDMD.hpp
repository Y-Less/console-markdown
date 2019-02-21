#pragma once

#include <string>

namespace cmdmd
{
	void Init();
	::std::string
		Render(std::string const & input);

	::std::string
		Render(char const * input);

	::std::string
		Render(char const * input, size_t len);

	inline namespace Literals
	{
		::std::string
			operator "" _cmdmd(char const * s, size_t len);
	}
};

using namespace cmdmd::Literals;

