#ifndef _CONSOLE_COLOUR
#define _CONSOLE_COLOUR

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CONMD_WINDOWS

#include <Windows.h>

enum STATE_E
{
	STATE_NONE,
	STATE_ESC,         // Saw `\x1B`.
	STATE_START,       // Saw `[`.
	STATE_A00,         // Saw a number.
	STATE_A01,         // Saw two numbers.
	STATE_S0,          // Saw a semi-colon.
	STATE_A10,         // Saw a number.
	STATE_A11,         // Saw two numbers.
	STATE_S1,          // Saw a semi-colon.
	STATE_A20,         // Saw a number.
	STATE_A21,         // Saw two numbers.
	STATE_S2,          // Saw a semi-colon.
	STATE_A30,         // Saw a number.
	STATE_A31,         // Saw two numbers.
	STATE_DONE,        // Complete.
	STATE_EXTRA_NL,    // Insert one extra new line.
	STATE_EXTRA_2NL,   // Insert two extra new lines.
	STATE_EXTRA_SPACE, // Insert one extra space.
	STATE_SKIP,        // Multi-byte character.
};

struct stream_s;

//typedef int (* OutputC_t)(wchar_t c, struct stream_s * const stream);
//typedef int (* OutputA_t)(char const * c, int len, struct stream_s * const stream);
//typedef int (* OutputW_t)(wchar_t const * c, int len, struct stream_s * const stream);

struct stream_s
{
	wchar_t
		UnicodeMask;

	enum STATE_E
		State;

	bool
		//	Wide,
		Coloured,
		Error;

	HANDLE
		Handle;

	WORD
		DefaultStyle,
		CurrentStyle;

	unsigned char
		Attr0,
		Attr1,
		Attr2,
		Attr3;

	//OutputC_t
	//	OutputC;
	//
	//OutputA_t
	//	OutputA;
	//
	//OutputW_t
	//	OutputW;
	//
	//void *
	//	Data;
};

extern struct stream_s
	gCOut,
	gCErr;

#endif

void InitStreamHooks();

#ifdef __cplusplus
}
#endif

#endif

