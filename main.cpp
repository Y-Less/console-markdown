#include "libsoldout/markdown.h"
#include "ColouredBuffer.hpp"

void DumpColouredBufferExamples();

int main()
{
	ColouredBuffer<char>::StandardInstall();
	ColouredBuffer<wchar_t>::StandardInstall();
	DumpColouredBufferExamples();

	std::cout << "**bold**, _italic_" << std::endl;
	//std::cout << "\x1B[1mbold\x1B[0m, _italic_" << std::endl;
	//std::cout << "\x1B[37mbold\x1B[0m, _italic_" << std::endl;
	//std::cout << "\x1B[37;1mbold\x1B[0m, _italic_" << std::endl;
	std::cout << "\x1B[5;47;30mbold\x1B[0m, \x1B[37;1mitalic\x1B[0m" << std::endl;

	return 0;
}

