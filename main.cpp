#include <console-markdown/CMDMD.hpp>
#include <iostream>
#include <windows.h>
#include "subhook/subhook.h"

using namespace cmdmd::Literals;

int main()
{
	cmdmd::Init();

	std::string output = R"(
____
  
# Heading 1
**hello** _there_ 

## Heading 2
### Heading 3
#### Heading 4
 
  
____
How are you?
  And is this a new paragraph?

You should need two new lines to make a new paragraph,
 so this should all be wrapped on one line.  Of course that does depend on the width of the console (and not the line length) so check that as well.  We may need more code for wrapping text.

```pawn
hook OnPlayerConnect(playerid)
{
	for (new i = 0; i != 10; ++i)
	{
		printf("Markdown!");
	}
}
```

```cpp
class MyClass : public BaseClass
{
private:
	virtual unsigned int Method(float a, Data const & b) override = 0;
}
```

Output code with:

\`\`\`cpp

// Code goes here.

\`\`\`

    
      0x02000700, 0x00000000, 0x00FFFE00, 0x00000007, 0x00000000, 0x00000000,
      0x02000700, 0x00000000, 0x00FFFE00, 0x00000007, 0x00000000, 0x00000000,
          0x031D2775, 0x00000000, 0x00FFFE00, 0x00000007, 0x00000000, 0x00000000,
          0x02000700, 0x00000000, 0x00FFFE00, 0x00000007, 0x00000000, 0x00000000,
              0x02BE4788, 0x00000000, 0x00FFFE00, 0x00000007, 0x0003C000, 0x00000000,
              0x02010771, 0x00000000, 0x00FFFE00, 0x00000007, 0x00000000, 0x00000000,
                  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
                  0x02000700, 0x00000000, 0x00FFFE00, 0x00000007, 0x00000000, 0x00000000,
                      0x02000700, 0x00000000, 0x00FFFE00, 0x00000007, 0x00000000, 0x00000000,
                      0x029A0FCE, 0x00000000, 0x00FFFE00, 0x00000007, 0x0000C000, 0x00000000,

```
0x02000700, 0x00000000, 0x00FFFE00, 0x00000007,
0x02000700, 0x00000000, 0x00FFFE00, 0x00000007,
0x031D2775, 0x00000000, 0x00FFFE00, 0x00000007,
0x02000700, 0x00000000, 0x00FFFE00, 0x00000007,
0x02BE4788, 0x00000000, 0x00FFFE00, 0x00000007,
0x02010771, 0x00000000, 0x00FFFE00, 0x00000007,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x02000700, 0x00000000, 0x00FFFE00, 0x00000007,
0x02000700, 0x00000000, 0x00FFFE00, 0x00000007,
0x029A0FCE, 0x00000000, 0x00FFFE00, 0x00000007,
```

* One
* Two
* Three
* Four

I hope you are OK, like `someCode()` is.

1. One
2. One
3. One1
  
)"_cmdmd;

    //WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.length(), NULL, NULL);
    std::cout << output;

	return 0;
}

