#ifndef _CONSOLE_COLOUR
#define _CONSOLE_COLOUR

#if defined _WIN32 || defined WIN32
	#define CONMD_WINDOWS 1
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CONMD_WINDOWS

#include <Windows.h>
#include <stdbool.h>

enum STATE_E
{
	STATE_NONE,
	STATE_ESC,         // Saw `\x1B`.
	STATE_START,       // Saw `[`.
	STATE_A00,         // Saw a number.
	STATE_S0,          // Saw a semi-colon.
	STATE_A10,         // Saw a number.
	STATE_S1,          // Saw a semi-colon.
	STATE_A20,         // Saw a number.
	STATE_S2,          // Saw a semi-colon.
	STATE_A30,         // Saw a number.
	STATE_S3,          // Saw a semi-colon.
	STATE_A40,         // Saw a number.
	STATE_DONE,        // Complete.
	STATE_EXTRA_NL,    // Insert one extra new line.
	STATE_EXTRA_2NL,   // Insert two extra new lines.
	STATE_EXTRA_SPACE, // Insert one extra space.
	STATE_SKIP,        // Multi-byte character.
};

struct stream_s;

struct stream_s
{
	wchar_t
		UnicodeMask;

	enum STATE_E
		State;

	bool
		Coloured;

	HANDLE
		Handle;

	WORD
		DefaultStyle,
		CurrentStyle;

	unsigned char
		Attr0,
		Attr1,
		Attr2,
		Attr3,
		Attr4;
};

#else

// Empty, just to exist.
struct stream_s
{
};

#endif

extern struct stream_s
	gStream;

void InitStreamHooks();
void DeinitStreamHooks();
int WriteColouredW(wchar_t const* s, int n, struct stream_s* const stream);
int WriteColouredA(char const* s, int n, struct stream_s* const stream);
void Backout(struct stream_s* const stream);

#ifdef __cplusplus
}
#endif

#endif

