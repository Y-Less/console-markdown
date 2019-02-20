#include "libsoldout/markdown.h"
#include "ColouredBuffer.hpp"

void DumpColouredBufferExamples();
void WriteMD(char const * str);

int main()
{
	ColouredBuffer<char>::StandardInstall();
	ColouredBuffer<wchar_t>::StandardInstall();
	//DumpColouredBufferExamples();

	//std::cout << "**bold**, _italic_" << std::endl;
	////std::cout << "\x1B[1mbold\x1B[0m, _italic_" << std::endl;
	////std::cout << "\x1B[37mbold\x1B[0m, _italic_" << std::endl;
	////std::cout << "\x1B[37;1mbold\x1B[0m, _italic_" << std::endl;
	//std::cout << "\x1B[5;47;30mbold\x1B[0m, \x1B[37;1mitalic\x1B[0m" << std::endl;
	//
	//
	//void WriteMD(char const * str)




	//std::cout << "\x1B[42;5;30mHeading 1\x1B[0m, _italic_" << std::endl;
	//std::cout << "\x1B[32;1mHeading 2\x1B[0m, _italic_" << std::endl;
	//std::cout << "\x1B[33;1mHeading 3\x1B[0m, _italic_" << std::endl;
	//std::cout << "\x1B[31;1mHeading 4\x1B[0m, _italic_" << std::endl;




	WriteMD(R"(
  
# Heading 1
**hello** Alex _Y-Less_ Cole  

## Heading 2
### Heading 3
#### Heading 4
 
  
____
How are you?
And is this a new paragraph?

```pawn
hook OnPlayerConnect(playerid)
{
	for (new i = 0; i != 10; ++i)
	{
		printf("Markdown!");
	}
}
```

* One
* Two
* Three
* Four

I hope you are OK, like `someCode()` is.

1. One
2. One
3. One1
  
)");

	return 0;
}

