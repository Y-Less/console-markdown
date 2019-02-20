#include "include/CMDMD/CMDMD.hpp"
#include <iostream>

int main()
{
	cmdmd::Init();

	std::cout << cmdmd::Render(R"(
____
  
# Heading 1
**hello** _there_ 

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

