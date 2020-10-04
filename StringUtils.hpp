#pragma once

#include <string>

namespace cmdmd
{
	// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
	void ReplaceAll(std::string & source, std::string const & from, std::string const & to);
}

