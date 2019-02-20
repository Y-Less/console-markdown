#include "libsoldout/markdown.h"
#include "ColouredBuffer.hpp"

void DumpColouredBufferExamples();

int main()
{
	ColouredBuffer<char>::StandardInstall();
	ColouredBuffer<wchar_t>::StandardInstall();
	DumpColouredBufferExamples();
	return 0;
}

