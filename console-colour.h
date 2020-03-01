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

struct console_colour_stream_s;

typedef int (*OutputC_)(void * data, wchar_t c, struct console_colour_stream_s* const stream);
typedef int (*OutputA_)(void * data, char const* c, int len, struct console_colour_stream_s* const stream);
typedef int (*OutputW_)(void * data, wchar_t const* c, int len, struct console_colour_stream_s* const stream);
typedef void (*OutputColour_)(void * data, unsigned short colour, struct console_colour_stream_s* const stream);

struct console_colour_call_s
{
	OutputC_
		OutputC;

	OutputA_
		OutputA;

	OutputW_
		OutputW;

	OutputColour_
		OutputColour;

	void*
		Data;
};

struct console_colour_state_s
{
	wchar_t
		UnicodeMask;

	enum STATE_E
		State;

	bool
		Coloured;

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

struct console_colour_stream_s
{
	struct console_colour_call_s *
		Call;

	struct console_colour_state_s *
		State;
};

#else

// Empty, just to exist.
struct console_colour_stream_s
{
};

#endif

extern struct console_colour_state_s
	gConsoleStreamState;

void ReaddStreamHooks();
void RemoveStreamHooks();

int WriteColouredW(wchar_t const* s, int n, struct console_colour_stream_s* const stream);
int WriteColouredA(char const* s, int n, struct console_colour_stream_s* const stream);
void Backout(struct console_colour_stream_s* const stream);

#ifdef __cplusplus
}
#endif

#endif

