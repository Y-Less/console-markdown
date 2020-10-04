#include "StringUtils.hpp"

namespace cmdmd
{
	void ReplaceAll(std::string & source, std::string const & from, std::string const & to)
	{
		std::string newString;
		newString.reserve(source.length()); // Avoids a few memory allocations.

		std::string::size_type lastPos = 0;
		std::string::size_type findPos;

		while(std::string::npos != (findPos = source.find(from, lastPos)))
		{
			newString.append(source, lastPos, findPos - lastPos);
			newString += to;
			lastPos = findPos + from.length();
		}

		// Care for the rest after last occurrence.
		newString += source.substr(lastPos);

		source.swap(newString);
	}
}

