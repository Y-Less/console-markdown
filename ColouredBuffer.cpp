#include "ColouredBuffer.hpp"

using namespace cmdmd;

#ifdef CONMD_WINDOWS
	bool
		ColouredBuffer<char, true>::
		first_;

	WORD
		ColouredBuffer<char, true>::
		default_;

	bool
		ColouredBuffer<wchar_t, true>::
		first_;

	WORD
		ColouredBuffer<wchar_t, true>::
		default_;

	bool
		ColouredBuffer<char, false>::
		first_;

	WORD
		ColouredBuffer<char, false>::
		default_;

	bool
		ColouredBuffer<wchar_t, false>::
		first_;

	WORD
		ColouredBuffer<wchar_t, false>::
		default_;
#endif

template <>
void
	ColouredBuffer<char, true>::
	StandardInstall()
{
#ifdef CONMD_WINDOWS
	// Redirect `stderr` via us.
	new ColouredBuffer<char, true>(std::cerr, true);
#endif
}

template <>
void
	ColouredBuffer<wchar_t, true>::
	StandardInstall()
{
#ifdef CONMD_WINDOWS
	// Redirect `stderr` via us.
	new ColouredBuffer<wchar_t, true>(std::wcerr, true);
#endif
}

template <>
void
	ColouredBuffer<char, false>::
	StandardInstall()
{
#ifdef CONMD_WINDOWS
	// Redirect `stdout` via us.
	new ColouredBuffer<char, false>(std::cout, true);
#endif
}

template <>
void
	ColouredBuffer<wchar_t, false>::
	StandardInstall()
{
#ifdef CONMD_WINDOWS
	// Redirect `stdout` via us.
	new ColouredBuffer<wchar_t, false>(std::wcout, true);
#endif
}

#ifdef CONMD_WINDOWS
template <>
char const *
	ColouredBuffer<char, true>::
	wstrchr(char const * s, char const n)
{
	return strchr(s, n);
}

template <>
wchar_t const *
	ColouredBuffer<wchar_t, true>::
	wstrchr(wchar_t const * s, wchar_t const n)
{
	return wcschr(s, n);
}

template <>
std::streamsize
	ColouredBuffer<char, true>::
	wstrout(char const * s)
{
	return buffer_.sputn(s, strlen(s));
}

template <>
std::streamsize
	ColouredBuffer<wchar_t, true>::
	wstrout(char const * s)
{
	wchar_t w[12];
	size_t conv;
	mbstowcs_s(&conv, w, 12, s, 12);
	return buffer_.sputn(w, wcslen(w));
}

template <>
char const *
	ColouredBuffer<char, false>::
	wstrchr(char const * s, char const n)
{
	return strchr(s, n);
}

template <>
wchar_t const *
	ColouredBuffer<wchar_t, false>::
	wstrchr(wchar_t const * s, wchar_t const n)
{
	return wcschr(s, n);
}

template <>
std::streamsize
	ColouredBuffer<char, false>::
	wstrout(char const * s)
{
	return buffer_.sputn(s, strlen(s));
}

template <>
std::streamsize
	ColouredBuffer<wchar_t, false>::
	wstrout(char const * s)
{
	wchar_t w[12];
	size_t conv;
	mbstowcs_s(&conv, w, 12, s, 12);
	return buffer_.sputn(w, wcslen(w));
}
#endif

#if defined _DEBUG
	void DumpColouredBufferExamples()
	{
		std::cout << std::endl;
		std::cout << "\x1B[0m        30    31    32    33    34    35    36    37   30;1  31;1  32;1  33;1  34;1  35;1  36;1  37;1  " << std::endl;
		std::cout << std::endl;
		std::cout << "\x1B[0m      \x1B[30;40m #### \x1B[31;40m #### \x1B[32;40m #### \x1B[33;40m #### \x1B[34;40m #### \x1B[35;40m #### \x1B[36;40m #### \x1B[37;40m #### \x1B[30;40;1m #### \x1B[31;40;1m #### \x1B[32;40;1m #### \x1B[33;40;1m #### \x1B[34;40;1m #### \x1B[35;40;1m #### \x1B[36;40;1m #### \x1B[37;40;1m #### " << std::endl;
		std::cout << "\x1B[0m   40 \x1B[30;40m #### \x1B[31;40m #### \x1B[32;40m #### \x1B[33;40m #### \x1B[34;40m #### \x1B[35;40m #### \x1B[36;40m #### \x1B[37;40m #### \x1B[30;40;1m #### \x1B[31;40;1m #### \x1B[32;40;1m #### \x1B[33;40;1m #### \x1B[34;40;1m #### \x1B[35;40;1m #### \x1B[36;40;1m #### \x1B[37;40;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;40m #### \x1B[31;40m #### \x1B[32;40m #### \x1B[33;40m #### \x1B[34;40m #### \x1B[35;40m #### \x1B[36;40m #### \x1B[37;40m #### \x1B[30;40;1m #### \x1B[31;40;1m #### \x1B[32;40;1m #### \x1B[33;40;1m #### \x1B[34;40;1m #### \x1B[35;40;1m #### \x1B[36;40;1m #### \x1B[37;40;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;41m #### \x1B[31;41m #### \x1B[32;41m #### \x1B[33;41m #### \x1B[34;41m #### \x1B[35;41m #### \x1B[36;41m #### \x1B[37;41m #### \x1B[30;41;1m #### \x1B[31;41;1m #### \x1B[32;41;1m #### \x1B[33;41;1m #### \x1B[34;41;1m #### \x1B[35;41;1m #### \x1B[36;41;1m #### \x1B[37;41;1m #### " << std::endl;
		std::cout << "\x1B[0m   41 \x1B[30;41m #### \x1B[31;41m #### \x1B[32;41m #### \x1B[33;41m #### \x1B[34;41m #### \x1B[35;41m #### \x1B[36;41m #### \x1B[37;41m #### \x1B[30;41;1m #### \x1B[31;41;1m #### \x1B[32;41;1m #### \x1B[33;41;1m #### \x1B[34;41;1m #### \x1B[35;41;1m #### \x1B[36;41;1m #### \x1B[37;41;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;41m #### \x1B[31;41m #### \x1B[32;41m #### \x1B[33;41m #### \x1B[34;41m #### \x1B[35;41m #### \x1B[36;41m #### \x1B[37;41m #### \x1B[30;41;1m #### \x1B[31;41;1m #### \x1B[32;41;1m #### \x1B[33;41;1m #### \x1B[34;41;1m #### \x1B[35;41;1m #### \x1B[36;41;1m #### \x1B[37;41;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;42m #### \x1B[31;42m #### \x1B[32;42m #### \x1B[33;42m #### \x1B[34;42m #### \x1B[35;42m #### \x1B[36;42m #### \x1B[37;42m #### \x1B[30;42;1m #### \x1B[31;42;1m #### \x1B[32;42;1m #### \x1B[33;42;1m #### \x1B[34;42;1m #### \x1B[35;42;1m #### \x1B[36;42;1m #### \x1B[37;42;1m #### " << std::endl;
		std::cout << "\x1B[0m   42 \x1B[30;42m #### \x1B[31;42m #### \x1B[32;42m #### \x1B[33;42m #### \x1B[34;42m #### \x1B[35;42m #### \x1B[36;42m #### \x1B[37;42m #### \x1B[30;42;1m #### \x1B[31;42;1m #### \x1B[32;42;1m #### \x1B[33;42;1m #### \x1B[34;42;1m #### \x1B[35;42;1m #### \x1B[36;42;1m #### \x1B[37;42;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;42m #### \x1B[31;42m #### \x1B[32;42m #### \x1B[33;42m #### \x1B[34;42m #### \x1B[35;42m #### \x1B[36;42m #### \x1B[37;42m #### \x1B[30;42;1m #### \x1B[31;42;1m #### \x1B[32;42;1m #### \x1B[33;42;1m #### \x1B[34;42;1m #### \x1B[35;42;1m #### \x1B[36;42;1m #### \x1B[37;42;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;43m #### \x1B[31;43m #### \x1B[32;43m #### \x1B[33;43m #### \x1B[34;43m #### \x1B[35;43m #### \x1B[36;43m #### \x1B[37;43m #### \x1B[30;43;1m #### \x1B[31;43;1m #### \x1B[32;43;1m #### \x1B[33;43;1m #### \x1B[34;43;1m #### \x1B[35;43;1m #### \x1B[36;43;1m #### \x1B[37;43;1m #### " << std::endl;
		std::cout << "\x1B[0m   43 \x1B[30;43m #### \x1B[31;43m #### \x1B[32;43m #### \x1B[33;43m #### \x1B[34;43m #### \x1B[35;43m #### \x1B[36;43m #### \x1B[37;43m #### \x1B[30;43;1m #### \x1B[31;43;1m #### \x1B[32;43;1m #### \x1B[33;43;1m #### \x1B[34;43;1m #### \x1B[35;43;1m #### \x1B[36;43;1m #### \x1B[37;43;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;43m #### \x1B[31;43m #### \x1B[32;43m #### \x1B[33;43m #### \x1B[34;43m #### \x1B[35;43m #### \x1B[36;43m #### \x1B[37;43m #### \x1B[30;43;1m #### \x1B[31;43;1m #### \x1B[32;43;1m #### \x1B[33;43;1m #### \x1B[34;43;1m #### \x1B[35;43;1m #### \x1B[36;43;1m #### \x1B[37;43;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;44m #### \x1B[31;44m #### \x1B[32;44m #### \x1B[33;44m #### \x1B[34;44m #### \x1B[35;44m #### \x1B[36;44m #### \x1B[37;44m #### \x1B[30;44;1m #### \x1B[31;44;1m #### \x1B[32;44;1m #### \x1B[33;44;1m #### \x1B[34;44;1m #### \x1B[35;44;1m #### \x1B[36;44;1m #### \x1B[37;44;1m #### " << std::endl;
		std::cout << "\x1B[0m   44 \x1B[30;44m #### \x1B[31;44m #### \x1B[32;44m #### \x1B[33;44m #### \x1B[34;44m #### \x1B[35;44m #### \x1B[36;44m #### \x1B[37;44m #### \x1B[30;44;1m #### \x1B[31;44;1m #### \x1B[32;44;1m #### \x1B[33;44;1m #### \x1B[34;44;1m #### \x1B[35;44;1m #### \x1B[36;44;1m #### \x1B[37;44;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;44m #### \x1B[31;44m #### \x1B[32;44m #### \x1B[33;44m #### \x1B[34;44m #### \x1B[35;44m #### \x1B[36;44m #### \x1B[37;44m #### \x1B[30;44;1m #### \x1B[31;44;1m #### \x1B[32;44;1m #### \x1B[33;44;1m #### \x1B[34;44;1m #### \x1B[35;44;1m #### \x1B[36;44;1m #### \x1B[37;44;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;45m #### \x1B[31;45m #### \x1B[32;45m #### \x1B[33;45m #### \x1B[34;45m #### \x1B[35;45m #### \x1B[36;45m #### \x1B[37;45m #### \x1B[30;45;1m #### \x1B[31;45;1m #### \x1B[32;45;1m #### \x1B[33;45;1m #### \x1B[34;45;1m #### \x1B[35;45;1m #### \x1B[36;45;1m #### \x1B[37;45;1m #### " << std::endl;
		std::cout << "\x1B[0m   45 \x1B[30;45m #### \x1B[31;45m #### \x1B[32;45m #### \x1B[33;45m #### \x1B[34;45m #### \x1B[35;45m #### \x1B[36;45m #### \x1B[37;45m #### \x1B[30;45;1m #### \x1B[31;45;1m #### \x1B[32;45;1m #### \x1B[33;45;1m #### \x1B[34;45;1m #### \x1B[35;45;1m #### \x1B[36;45;1m #### \x1B[37;45;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;45m #### \x1B[31;45m #### \x1B[32;45m #### \x1B[33;45m #### \x1B[34;45m #### \x1B[35;45m #### \x1B[36;45m #### \x1B[37;45m #### \x1B[30;45;1m #### \x1B[31;45;1m #### \x1B[32;45;1m #### \x1B[33;45;1m #### \x1B[34;45;1m #### \x1B[35;45;1m #### \x1B[36;45;1m #### \x1B[37;45;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;46m #### \x1B[31;46m #### \x1B[32;46m #### \x1B[33;46m #### \x1B[34;46m #### \x1B[35;46m #### \x1B[36;46m #### \x1B[37;46m #### \x1B[30;46;1m #### \x1B[31;46;1m #### \x1B[32;46;1m #### \x1B[33;46;1m #### \x1B[34;46;1m #### \x1B[35;46;1m #### \x1B[36;46;1m #### \x1B[37;46;1m #### " << std::endl;
		std::cout << "\x1B[0m   46 \x1B[30;46m #### \x1B[31;46m #### \x1B[32;46m #### \x1B[33;46m #### \x1B[34;46m #### \x1B[35;46m #### \x1B[36;46m #### \x1B[37;46m #### \x1B[30;46;1m #### \x1B[31;46;1m #### \x1B[32;46;1m #### \x1B[33;46;1m #### \x1B[34;46;1m #### \x1B[35;46;1m #### \x1B[36;46;1m #### \x1B[37;46;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;46m #### \x1B[31;46m #### \x1B[32;46m #### \x1B[33;46m #### \x1B[34;46m #### \x1B[35;46m #### \x1B[36;46m #### \x1B[37;46m #### \x1B[30;46;1m #### \x1B[31;46;1m #### \x1B[32;46;1m #### \x1B[33;46;1m #### \x1B[34;46;1m #### \x1B[35;46;1m #### \x1B[36;46;1m #### \x1B[37;46;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;47m #### \x1B[31;47m #### \x1B[32;47m #### \x1B[33;47m #### \x1B[34;47m #### \x1B[35;47m #### \x1B[36;47m #### \x1B[37;47m #### \x1B[30;47;1m #### \x1B[31;47;1m #### \x1B[32;47;1m #### \x1B[33;47;1m #### \x1B[34;47;1m #### \x1B[35;47;1m #### \x1B[36;47;1m #### \x1B[37;47;1m #### " << std::endl;
		std::cout << "\x1B[0m   47 \x1B[30;47m #### \x1B[31;47m #### \x1B[32;47m #### \x1B[33;47m #### \x1B[34;47m #### \x1B[35;47m #### \x1B[36;47m #### \x1B[37;47m #### \x1B[30;47;1m #### \x1B[31;47;1m #### \x1B[32;47;1m #### \x1B[33;47;1m #### \x1B[34;47;1m #### \x1B[35;47;1m #### \x1B[36;47;1m #### \x1B[37;47;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;47m #### \x1B[31;47m #### \x1B[32;47m #### \x1B[33;47m #### \x1B[34;47m #### \x1B[35;47m #### \x1B[36;47m #### \x1B[37;47m #### \x1B[30;47;1m #### \x1B[31;47;1m #### \x1B[32;47;1m #### \x1B[33;47;1m #### \x1B[34;47;1m #### \x1B[35;47;1m #### \x1B[36;47;1m #### \x1B[37;47;1m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;40;5m #### \x1B[31;40;5m #### \x1B[32;40;5m #### \x1B[33;40;5m #### \x1B[34;40;5m #### \x1B[35;40;5m #### \x1B[36;40;5m #### \x1B[37;40;5m #### \x1B[30;40;1;5m #### \x1B[31;40;1;5m #### \x1B[32;40;1;5m #### \x1B[33;40;1;5m #### \x1B[34;40;1;5m #### \x1B[35;40;1;5m #### \x1B[36;40;1;5m #### \x1B[37;40;1;5m #### " << std::endl;
		std::cout << "\x1B[0m 40;5 \x1B[30;40;5m #### \x1B[31;40;5m #### \x1B[32;40;5m #### \x1B[33;40;5m #### \x1B[34;40;5m #### \x1B[35;40;5m #### \x1B[36;40;5m #### \x1B[37;40;5m #### \x1B[30;40;1;5m #### \x1B[31;40;1;5m #### \x1B[32;40;1;5m #### \x1B[33;40;1;5m #### \x1B[34;40;1;5m #### \x1B[35;40;1;5m #### \x1B[36;40;1;5m #### \x1B[37;40;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;40;5m #### \x1B[31;40;5m #### \x1B[32;40;5m #### \x1B[33;40;5m #### \x1B[34;40;5m #### \x1B[35;40;5m #### \x1B[36;40;5m #### \x1B[37;40;5m #### \x1B[30;40;1;5m #### \x1B[31;40;1;5m #### \x1B[32;40;1;5m #### \x1B[33;40;1;5m #### \x1B[34;40;1;5m #### \x1B[35;40;1;5m #### \x1B[36;40;1;5m #### \x1B[37;40;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;41;5m #### \x1B[31;41;5m #### \x1B[32;41;5m #### \x1B[33;41;5m #### \x1B[34;41;5m #### \x1B[35;41;5m #### \x1B[36;41;5m #### \x1B[37;41;5m #### \x1B[30;41;1;5m #### \x1B[31;41;1;5m #### \x1B[32;41;1;5m #### \x1B[33;41;1;5m #### \x1B[34;41;1;5m #### \x1B[35;41;1;5m #### \x1B[36;41;1;5m #### \x1B[37;41;1;5m #### " << std::endl;
		std::cout << "\x1B[0m 41;5 \x1B[30;41;5m #### \x1B[31;41;5m #### \x1B[32;41;5m #### \x1B[33;41;5m #### \x1B[34;41;5m #### \x1B[35;41;5m #### \x1B[36;41;5m #### \x1B[37;41;5m #### \x1B[30;41;1;5m #### \x1B[31;41;1;5m #### \x1B[32;41;1;5m #### \x1B[33;41;1;5m #### \x1B[34;41;1;5m #### \x1B[35;41;1;5m #### \x1B[36;41;1;5m #### \x1B[37;41;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;41;5m #### \x1B[31;41;5m #### \x1B[32;41;5m #### \x1B[33;41;5m #### \x1B[34;41;5m #### \x1B[35;41;5m #### \x1B[36;41;5m #### \x1B[37;41;5m #### \x1B[30;41;1;5m #### \x1B[31;41;1;5m #### \x1B[32;41;1;5m #### \x1B[33;41;1;5m #### \x1B[34;41;1;5m #### \x1B[35;41;1;5m #### \x1B[36;41;1;5m #### \x1B[37;41;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;42;5m #### \x1B[31;42;5m #### \x1B[32;42;5m #### \x1B[33;42;5m #### \x1B[34;42;5m #### \x1B[35;42;5m #### \x1B[36;42;5m #### \x1B[37;42;5m #### \x1B[30;42;1;5m #### \x1B[31;42;1;5m #### \x1B[32;42;1;5m #### \x1B[33;42;1;5m #### \x1B[34;42;1;5m #### \x1B[35;42;1;5m #### \x1B[36;42;1;5m #### \x1B[37;42;1;5m #### " << std::endl;
		std::cout << "\x1B[0m 42;5 \x1B[30;42;5m #### \x1B[31;42;5m #### \x1B[32;42;5m #### \x1B[33;42;5m #### \x1B[34;42;5m #### \x1B[35;42;5m #### \x1B[36;42;5m #### \x1B[37;42;5m #### \x1B[30;42;1;5m #### \x1B[31;42;1;5m #### \x1B[32;42;1;5m #### \x1B[33;42;1;5m #### \x1B[34;42;1;5m #### \x1B[35;42;1;5m #### \x1B[36;42;1;5m #### \x1B[37;42;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;42;5m #### \x1B[31;42;5m #### \x1B[32;42;5m #### \x1B[33;42;5m #### \x1B[34;42;5m #### \x1B[35;42;5m #### \x1B[36;42;5m #### \x1B[37;42;5m #### \x1B[30;42;1;5m #### \x1B[31;42;1;5m #### \x1B[32;42;1;5m #### \x1B[33;42;1;5m #### \x1B[34;42;1;5m #### \x1B[35;42;1;5m #### \x1B[36;42;1;5m #### \x1B[37;42;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;43;5m #### \x1B[31;43;5m #### \x1B[32;43;5m #### \x1B[33;43;5m #### \x1B[34;43;5m #### \x1B[35;43;5m #### \x1B[36;43;5m #### \x1B[37;43;5m #### \x1B[30;43;1;5m #### \x1B[31;43;1;5m #### \x1B[32;43;1;5m #### \x1B[33;43;1;5m #### \x1B[34;43;1;5m #### \x1B[35;43;1;5m #### \x1B[36;43;1;5m #### \x1B[37;43;1;5m #### " << std::endl;
		std::cout << "\x1B[0m 43;5 \x1B[30;43;5m #### \x1B[31;43;5m #### \x1B[32;43;5m #### \x1B[33;43;5m #### \x1B[34;43;5m #### \x1B[35;43;5m #### \x1B[36;43;5m #### \x1B[37;43;5m #### \x1B[30;43;1;5m #### \x1B[31;43;1;5m #### \x1B[32;43;1;5m #### \x1B[33;43;1;5m #### \x1B[34;43;1;5m #### \x1B[35;43;1;5m #### \x1B[36;43;1;5m #### \x1B[37;43;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;43;5m #### \x1B[31;43;5m #### \x1B[32;43;5m #### \x1B[33;43;5m #### \x1B[34;43;5m #### \x1B[35;43;5m #### \x1B[36;43;5m #### \x1B[37;43;5m #### \x1B[30;43;1;5m #### \x1B[31;43;1;5m #### \x1B[32;43;1;5m #### \x1B[33;43;1;5m #### \x1B[34;43;1;5m #### \x1B[35;43;1;5m #### \x1B[36;43;1;5m #### \x1B[37;43;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;44;5m #### \x1B[31;44;5m #### \x1B[32;44;5m #### \x1B[33;44;5m #### \x1B[34;44;5m #### \x1B[35;44;5m #### \x1B[36;44;5m #### \x1B[37;44;5m #### \x1B[30;44;1;5m #### \x1B[31;44;1;5m #### \x1B[32;44;1;5m #### \x1B[33;44;1;5m #### \x1B[34;44;1;5m #### \x1B[35;44;1;5m #### \x1B[36;44;1;5m #### \x1B[37;44;1;5m #### " << std::endl;
		std::cout << "\x1B[0m 44;5 \x1B[30;44;5m #### \x1B[31;44;5m #### \x1B[32;44;5m #### \x1B[33;44;5m #### \x1B[34;44;5m #### \x1B[35;44;5m #### \x1B[36;44;5m #### \x1B[37;44;5m #### \x1B[30;44;1;5m #### \x1B[31;44;1;5m #### \x1B[32;44;1;5m #### \x1B[33;44;1;5m #### \x1B[34;44;1;5m #### \x1B[35;44;1;5m #### \x1B[36;44;1;5m #### \x1B[37;44;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;44;5m #### \x1B[31;44;5m #### \x1B[32;44;5m #### \x1B[33;44;5m #### \x1B[34;44;5m #### \x1B[35;44;5m #### \x1B[36;44;5m #### \x1B[37;44;5m #### \x1B[30;44;1;5m #### \x1B[31;44;1;5m #### \x1B[32;44;1;5m #### \x1B[33;44;1;5m #### \x1B[34;44;1;5m #### \x1B[35;44;1;5m #### \x1B[36;44;1;5m #### \x1B[37;44;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;45;5m #### \x1B[31;45;5m #### \x1B[32;45;5m #### \x1B[33;45;5m #### \x1B[34;45;5m #### \x1B[35;45;5m #### \x1B[36;45;5m #### \x1B[37;45;5m #### \x1B[30;45;1;5m #### \x1B[31;45;1;5m #### \x1B[32;45;1;5m #### \x1B[33;45;1;5m #### \x1B[34;45;1;5m #### \x1B[35;45;1;5m #### \x1B[36;45;1;5m #### \x1B[37;45;1;5m #### " << std::endl;
		std::cout << "\x1B[0m 45;5 \x1B[30;45;5m #### \x1B[31;45;5m #### \x1B[32;45;5m #### \x1B[33;45;5m #### \x1B[34;45;5m #### \x1B[35;45;5m #### \x1B[36;45;5m #### \x1B[37;45;5m #### \x1B[30;45;1;5m #### \x1B[31;45;1;5m #### \x1B[32;45;1;5m #### \x1B[33;45;1;5m #### \x1B[34;45;1;5m #### \x1B[35;45;1;5m #### \x1B[36;45;1;5m #### \x1B[37;45;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;45;5m #### \x1B[31;45;5m #### \x1B[32;45;5m #### \x1B[33;45;5m #### \x1B[34;45;5m #### \x1B[35;45;5m #### \x1B[36;45;5m #### \x1B[37;45;5m #### \x1B[30;45;1;5m #### \x1B[31;45;1;5m #### \x1B[32;45;1;5m #### \x1B[33;45;1;5m #### \x1B[34;45;1;5m #### \x1B[35;45;1;5m #### \x1B[36;45;1;5m #### \x1B[37;45;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;46;5m #### \x1B[31;46;5m #### \x1B[32;46;5m #### \x1B[33;46;5m #### \x1B[34;46;5m #### \x1B[35;46;5m #### \x1B[36;46;5m #### \x1B[37;46;5m #### \x1B[30;46;1;5m #### \x1B[31;46;1;5m #### \x1B[32;46;1;5m #### \x1B[33;46;1;5m #### \x1B[34;46;1;5m #### \x1B[35;46;1;5m #### \x1B[36;46;1;5m #### \x1B[37;46;1;5m #### " << std::endl;
		std::cout << "\x1B[0m 46;5 \x1B[30;46;5m #### \x1B[31;46;5m #### \x1B[32;46;5m #### \x1B[33;46;5m #### \x1B[34;46;5m #### \x1B[35;46;5m #### \x1B[36;46;5m #### \x1B[37;46;5m #### \x1B[30;46;1;5m #### \x1B[31;46;1;5m #### \x1B[32;46;1;5m #### \x1B[33;46;1;5m #### \x1B[34;46;1;5m #### \x1B[35;46;1;5m #### \x1B[36;46;1;5m #### \x1B[37;46;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;46;5m #### \x1B[31;46;5m #### \x1B[32;46;5m #### \x1B[33;46;5m #### \x1B[34;46;5m #### \x1B[35;46;5m #### \x1B[36;46;5m #### \x1B[37;46;5m #### \x1B[30;46;1;5m #### \x1B[31;46;1;5m #### \x1B[32;46;1;5m #### \x1B[33;46;1;5m #### \x1B[34;46;1;5m #### \x1B[35;46;1;5m #### \x1B[36;46;1;5m #### \x1B[37;46;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;47;5m #### \x1B[31;47;5m #### \x1B[32;47;5m #### \x1B[33;47;5m #### \x1B[34;47;5m #### \x1B[35;47;5m #### \x1B[36;47;5m #### \x1B[37;47;5m #### \x1B[30;47;1;5m #### \x1B[31;47;1;5m #### \x1B[32;47;1;5m #### \x1B[33;47;1;5m #### \x1B[34;47;1;5m #### \x1B[35;47;1;5m #### \x1B[36;47;1;5m #### \x1B[37;47;1;5m #### " << std::endl;
		std::cout << "\x1B[0m 47;5 \x1B[30;47;5m #### \x1B[31;47;5m #### \x1B[32;47;5m #### \x1B[33;47;5m #### \x1B[34;47;5m #### \x1B[35;47;5m #### \x1B[36;47;5m #### \x1B[37;47;5m #### \x1B[30;47;1;5m #### \x1B[31;47;1;5m #### \x1B[32;47;1;5m #### \x1B[33;47;1;5m #### \x1B[34;47;1;5m #### \x1B[35;47;1;5m #### \x1B[36;47;1;5m #### \x1B[37;47;1;5m #### " << std::endl;
		std::cout << "\x1B[0m      \x1B[30;47;5m #### \x1B[31;47;5m #### \x1B[32;47;5m #### \x1B[33;47;5m #### \x1B[34;47;5m #### \x1B[35;47;5m #### \x1B[36;47;5m #### \x1B[37;47;5m #### \x1B[30;47;1;5m #### \x1B[31;47;1;5m #### \x1B[32;47;1;5m #### \x1B[33;47;1;5m #### \x1B[34;47;1;5m #### \x1B[35;47;1;5m #### \x1B[36;47;1;5m #### \x1B[37;47;1;5m #### " << std::endl;
		std::cout << "\x1B[0m" << std::endl;
		std::wcout << std::endl;
		std::wcout << L"\x1B[0m        30    31    32    33    34    35    36    37   30;1  31;1  32;1  33;1  34;1  35;1  36;1  37;1  " << std::endl;
		std::wcout << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;40m #### \x1B[31;40m #### \x1B[32;40m #### \x1B[33;40m #### \x1B[34;40m #### \x1B[35;40m #### \x1B[36;40m #### \x1B[37;40m #### \x1B[30;40;1m #### \x1B[31;40;1m #### \x1B[32;40;1m #### \x1B[33;40;1m #### \x1B[34;40;1m #### \x1B[35;40;1m #### \x1B[36;40;1m #### \x1B[37;40;1m #### " << std::endl;
		std::wcout << L"\x1B[0m   40 \x1B[30;40m #### \x1B[31;40m #### \x1B[32;40m #### \x1B[33;40m #### \x1B[34;40m #### \x1B[35;40m #### \x1B[36;40m #### \x1B[37;40m #### \x1B[30;40;1m #### \x1B[31;40;1m #### \x1B[32;40;1m #### \x1B[33;40;1m #### \x1B[34;40;1m #### \x1B[35;40;1m #### \x1B[36;40;1m #### \x1B[37;40;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;40m #### \x1B[31;40m #### \x1B[32;40m #### \x1B[33;40m #### \x1B[34;40m #### \x1B[35;40m #### \x1B[36;40m #### \x1B[37;40m #### \x1B[30;40;1m #### \x1B[31;40;1m #### \x1B[32;40;1m #### \x1B[33;40;1m #### \x1B[34;40;1m #### \x1B[35;40;1m #### \x1B[36;40;1m #### \x1B[37;40;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;41m #### \x1B[31;41m #### \x1B[32;41m #### \x1B[33;41m #### \x1B[34;41m #### \x1B[35;41m #### \x1B[36;41m #### \x1B[37;41m #### \x1B[30;41;1m #### \x1B[31;41;1m #### \x1B[32;41;1m #### \x1B[33;41;1m #### \x1B[34;41;1m #### \x1B[35;41;1m #### \x1B[36;41;1m #### \x1B[37;41;1m #### " << std::endl;
		std::wcout << L"\x1B[0m   41 \x1B[30;41m #### \x1B[31;41m #### \x1B[32;41m #### \x1B[33;41m #### \x1B[34;41m #### \x1B[35;41m #### \x1B[36;41m #### \x1B[37;41m #### \x1B[30;41;1m #### \x1B[31;41;1m #### \x1B[32;41;1m #### \x1B[33;41;1m #### \x1B[34;41;1m #### \x1B[35;41;1m #### \x1B[36;41;1m #### \x1B[37;41;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;41m #### \x1B[31;41m #### \x1B[32;41m #### \x1B[33;41m #### \x1B[34;41m #### \x1B[35;41m #### \x1B[36;41m #### \x1B[37;41m #### \x1B[30;41;1m #### \x1B[31;41;1m #### \x1B[32;41;1m #### \x1B[33;41;1m #### \x1B[34;41;1m #### \x1B[35;41;1m #### \x1B[36;41;1m #### \x1B[37;41;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;42m #### \x1B[31;42m #### \x1B[32;42m #### \x1B[33;42m #### \x1B[34;42m #### \x1B[35;42m #### \x1B[36;42m #### \x1B[37;42m #### \x1B[30;42;1m #### \x1B[31;42;1m #### \x1B[32;42;1m #### \x1B[33;42;1m #### \x1B[34;42;1m #### \x1B[35;42;1m #### \x1B[36;42;1m #### \x1B[37;42;1m #### " << std::endl;
		std::wcout << L"\x1B[0m   42 \x1B[30;42m #### \x1B[31;42m #### \x1B[32;42m #### \x1B[33;42m #### \x1B[34;42m #### \x1B[35;42m #### \x1B[36;42m #### \x1B[37;42m #### \x1B[30;42;1m #### \x1B[31;42;1m #### \x1B[32;42;1m #### \x1B[33;42;1m #### \x1B[34;42;1m #### \x1B[35;42;1m #### \x1B[36;42;1m #### \x1B[37;42;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;42m #### \x1B[31;42m #### \x1B[32;42m #### \x1B[33;42m #### \x1B[34;42m #### \x1B[35;42m #### \x1B[36;42m #### \x1B[37;42m #### \x1B[30;42;1m #### \x1B[31;42;1m #### \x1B[32;42;1m #### \x1B[33;42;1m #### \x1B[34;42;1m #### \x1B[35;42;1m #### \x1B[36;42;1m #### \x1B[37;42;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;43m #### \x1B[31;43m #### \x1B[32;43m #### \x1B[33;43m #### \x1B[34;43m #### \x1B[35;43m #### \x1B[36;43m #### \x1B[37;43m #### \x1B[30;43;1m #### \x1B[31;43;1m #### \x1B[32;43;1m #### \x1B[33;43;1m #### \x1B[34;43;1m #### \x1B[35;43;1m #### \x1B[36;43;1m #### \x1B[37;43;1m #### " << std::endl;
		std::wcout << L"\x1B[0m   43 \x1B[30;43m #### \x1B[31;43m #### \x1B[32;43m #### \x1B[33;43m #### \x1B[34;43m #### \x1B[35;43m #### \x1B[36;43m #### \x1B[37;43m #### \x1B[30;43;1m #### \x1B[31;43;1m #### \x1B[32;43;1m #### \x1B[33;43;1m #### \x1B[34;43;1m #### \x1B[35;43;1m #### \x1B[36;43;1m #### \x1B[37;43;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;43m #### \x1B[31;43m #### \x1B[32;43m #### \x1B[33;43m #### \x1B[34;43m #### \x1B[35;43m #### \x1B[36;43m #### \x1B[37;43m #### \x1B[30;43;1m #### \x1B[31;43;1m #### \x1B[32;43;1m #### \x1B[33;43;1m #### \x1B[34;43;1m #### \x1B[35;43;1m #### \x1B[36;43;1m #### \x1B[37;43;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;44m #### \x1B[31;44m #### \x1B[32;44m #### \x1B[33;44m #### \x1B[34;44m #### \x1B[35;44m #### \x1B[36;44m #### \x1B[37;44m #### \x1B[30;44;1m #### \x1B[31;44;1m #### \x1B[32;44;1m #### \x1B[33;44;1m #### \x1B[34;44;1m #### \x1B[35;44;1m #### \x1B[36;44;1m #### \x1B[37;44;1m #### " << std::endl;
		std::wcout << L"\x1B[0m   44 \x1B[30;44m #### \x1B[31;44m #### \x1B[32;44m #### \x1B[33;44m #### \x1B[34;44m #### \x1B[35;44m #### \x1B[36;44m #### \x1B[37;44m #### \x1B[30;44;1m #### \x1B[31;44;1m #### \x1B[32;44;1m #### \x1B[33;44;1m #### \x1B[34;44;1m #### \x1B[35;44;1m #### \x1B[36;44;1m #### \x1B[37;44;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;44m #### \x1B[31;44m #### \x1B[32;44m #### \x1B[33;44m #### \x1B[34;44m #### \x1B[35;44m #### \x1B[36;44m #### \x1B[37;44m #### \x1B[30;44;1m #### \x1B[31;44;1m #### \x1B[32;44;1m #### \x1B[33;44;1m #### \x1B[34;44;1m #### \x1B[35;44;1m #### \x1B[36;44;1m #### \x1B[37;44;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;45m #### \x1B[31;45m #### \x1B[32;45m #### \x1B[33;45m #### \x1B[34;45m #### \x1B[35;45m #### \x1B[36;45m #### \x1B[37;45m #### \x1B[30;45;1m #### \x1B[31;45;1m #### \x1B[32;45;1m #### \x1B[33;45;1m #### \x1B[34;45;1m #### \x1B[35;45;1m #### \x1B[36;45;1m #### \x1B[37;45;1m #### " << std::endl;
		std::wcout << L"\x1B[0m   45 \x1B[30;45m #### \x1B[31;45m #### \x1B[32;45m #### \x1B[33;45m #### \x1B[34;45m #### \x1B[35;45m #### \x1B[36;45m #### \x1B[37;45m #### \x1B[30;45;1m #### \x1B[31;45;1m #### \x1B[32;45;1m #### \x1B[33;45;1m #### \x1B[34;45;1m #### \x1B[35;45;1m #### \x1B[36;45;1m #### \x1B[37;45;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;45m #### \x1B[31;45m #### \x1B[32;45m #### \x1B[33;45m #### \x1B[34;45m #### \x1B[35;45m #### \x1B[36;45m #### \x1B[37;45m #### \x1B[30;45;1m #### \x1B[31;45;1m #### \x1B[32;45;1m #### \x1B[33;45;1m #### \x1B[34;45;1m #### \x1B[35;45;1m #### \x1B[36;45;1m #### \x1B[37;45;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;46m #### \x1B[31;46m #### \x1B[32;46m #### \x1B[33;46m #### \x1B[34;46m #### \x1B[35;46m #### \x1B[36;46m #### \x1B[37;46m #### \x1B[30;46;1m #### \x1B[31;46;1m #### \x1B[32;46;1m #### \x1B[33;46;1m #### \x1B[34;46;1m #### \x1B[35;46;1m #### \x1B[36;46;1m #### \x1B[37;46;1m #### " << std::endl;
		std::wcout << L"\x1B[0m   46 \x1B[30;46m #### \x1B[31;46m #### \x1B[32;46m #### \x1B[33;46m #### \x1B[34;46m #### \x1B[35;46m #### \x1B[36;46m #### \x1B[37;46m #### \x1B[30;46;1m #### \x1B[31;46;1m #### \x1B[32;46;1m #### \x1B[33;46;1m #### \x1B[34;46;1m #### \x1B[35;46;1m #### \x1B[36;46;1m #### \x1B[37;46;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;46m #### \x1B[31;46m #### \x1B[32;46m #### \x1B[33;46m #### \x1B[34;46m #### \x1B[35;46m #### \x1B[36;46m #### \x1B[37;46m #### \x1B[30;46;1m #### \x1B[31;46;1m #### \x1B[32;46;1m #### \x1B[33;46;1m #### \x1B[34;46;1m #### \x1B[35;46;1m #### \x1B[36;46;1m #### \x1B[37;46;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;47m #### \x1B[31;47m #### \x1B[32;47m #### \x1B[33;47m #### \x1B[34;47m #### \x1B[35;47m #### \x1B[36;47m #### \x1B[37;47m #### \x1B[30;47;1m #### \x1B[31;47;1m #### \x1B[32;47;1m #### \x1B[33;47;1m #### \x1B[34;47;1m #### \x1B[35;47;1m #### \x1B[36;47;1m #### \x1B[37;47;1m #### " << std::endl;
		std::wcout << L"\x1B[0m   47 \x1B[30;47m #### \x1B[31;47m #### \x1B[32;47m #### \x1B[33;47m #### \x1B[34;47m #### \x1B[35;47m #### \x1B[36;47m #### \x1B[37;47m #### \x1B[30;47;1m #### \x1B[31;47;1m #### \x1B[32;47;1m #### \x1B[33;47;1m #### \x1B[34;47;1m #### \x1B[35;47;1m #### \x1B[36;47;1m #### \x1B[37;47;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;47m #### \x1B[31;47m #### \x1B[32;47m #### \x1B[33;47m #### \x1B[34;47m #### \x1B[35;47m #### \x1B[36;47m #### \x1B[37;47m #### \x1B[30;47;1m #### \x1B[31;47;1m #### \x1B[32;47;1m #### \x1B[33;47;1m #### \x1B[34;47;1m #### \x1B[35;47;1m #### \x1B[36;47;1m #### \x1B[37;47;1m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;40;5m #### \x1B[31;40;5m #### \x1B[32;40;5m #### \x1B[33;40;5m #### \x1B[34;40;5m #### \x1B[35;40;5m #### \x1B[36;40;5m #### \x1B[37;40;5m #### \x1B[30;40;1;5m #### \x1B[31;40;1;5m #### \x1B[32;40;1;5m #### \x1B[33;40;1;5m #### \x1B[34;40;1;5m #### \x1B[35;40;1;5m #### \x1B[36;40;1;5m #### \x1B[37;40;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m 40;5 \x1B[30;40;5m #### \x1B[31;40;5m #### \x1B[32;40;5m #### \x1B[33;40;5m #### \x1B[34;40;5m #### \x1B[35;40;5m #### \x1B[36;40;5m #### \x1B[37;40;5m #### \x1B[30;40;1;5m #### \x1B[31;40;1;5m #### \x1B[32;40;1;5m #### \x1B[33;40;1;5m #### \x1B[34;40;1;5m #### \x1B[35;40;1;5m #### \x1B[36;40;1;5m #### \x1B[37;40;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;40;5m #### \x1B[31;40;5m #### \x1B[32;40;5m #### \x1B[33;40;5m #### \x1B[34;40;5m #### \x1B[35;40;5m #### \x1B[36;40;5m #### \x1B[37;40;5m #### \x1B[30;40;1;5m #### \x1B[31;40;1;5m #### \x1B[32;40;1;5m #### \x1B[33;40;1;5m #### \x1B[34;40;1;5m #### \x1B[35;40;1;5m #### \x1B[36;40;1;5m #### \x1B[37;40;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;41;5m #### \x1B[31;41;5m #### \x1B[32;41;5m #### \x1B[33;41;5m #### \x1B[34;41;5m #### \x1B[35;41;5m #### \x1B[36;41;5m #### \x1B[37;41;5m #### \x1B[30;41;1;5m #### \x1B[31;41;1;5m #### \x1B[32;41;1;5m #### \x1B[33;41;1;5m #### \x1B[34;41;1;5m #### \x1B[35;41;1;5m #### \x1B[36;41;1;5m #### \x1B[37;41;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m 41;5 \x1B[30;41;5m #### \x1B[31;41;5m #### \x1B[32;41;5m #### \x1B[33;41;5m #### \x1B[34;41;5m #### \x1B[35;41;5m #### \x1B[36;41;5m #### \x1B[37;41;5m #### \x1B[30;41;1;5m #### \x1B[31;41;1;5m #### \x1B[32;41;1;5m #### \x1B[33;41;1;5m #### \x1B[34;41;1;5m #### \x1B[35;41;1;5m #### \x1B[36;41;1;5m #### \x1B[37;41;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;41;5m #### \x1B[31;41;5m #### \x1B[32;41;5m #### \x1B[33;41;5m #### \x1B[34;41;5m #### \x1B[35;41;5m #### \x1B[36;41;5m #### \x1B[37;41;5m #### \x1B[30;41;1;5m #### \x1B[31;41;1;5m #### \x1B[32;41;1;5m #### \x1B[33;41;1;5m #### \x1B[34;41;1;5m #### \x1B[35;41;1;5m #### \x1B[36;41;1;5m #### \x1B[37;41;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;42;5m #### \x1B[31;42;5m #### \x1B[32;42;5m #### \x1B[33;42;5m #### \x1B[34;42;5m #### \x1B[35;42;5m #### \x1B[36;42;5m #### \x1B[37;42;5m #### \x1B[30;42;1;5m #### \x1B[31;42;1;5m #### \x1B[32;42;1;5m #### \x1B[33;42;1;5m #### \x1B[34;42;1;5m #### \x1B[35;42;1;5m #### \x1B[36;42;1;5m #### \x1B[37;42;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m 42;5 \x1B[30;42;5m #### \x1B[31;42;5m #### \x1B[32;42;5m #### \x1B[33;42;5m #### \x1B[34;42;5m #### \x1B[35;42;5m #### \x1B[36;42;5m #### \x1B[37;42;5m #### \x1B[30;42;1;5m #### \x1B[31;42;1;5m #### \x1B[32;42;1;5m #### \x1B[33;42;1;5m #### \x1B[34;42;1;5m #### \x1B[35;42;1;5m #### \x1B[36;42;1;5m #### \x1B[37;42;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;42;5m #### \x1B[31;42;5m #### \x1B[32;42;5m #### \x1B[33;42;5m #### \x1B[34;42;5m #### \x1B[35;42;5m #### \x1B[36;42;5m #### \x1B[37;42;5m #### \x1B[30;42;1;5m #### \x1B[31;42;1;5m #### \x1B[32;42;1;5m #### \x1B[33;42;1;5m #### \x1B[34;42;1;5m #### \x1B[35;42;1;5m #### \x1B[36;42;1;5m #### \x1B[37;42;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;43;5m #### \x1B[31;43;5m #### \x1B[32;43;5m #### \x1B[33;43;5m #### \x1B[34;43;5m #### \x1B[35;43;5m #### \x1B[36;43;5m #### \x1B[37;43;5m #### \x1B[30;43;1;5m #### \x1B[31;43;1;5m #### \x1B[32;43;1;5m #### \x1B[33;43;1;5m #### \x1B[34;43;1;5m #### \x1B[35;43;1;5m #### \x1B[36;43;1;5m #### \x1B[37;43;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m 43;5 \x1B[30;43;5m #### \x1B[31;43;5m #### \x1B[32;43;5m #### \x1B[33;43;5m #### \x1B[34;43;5m #### \x1B[35;43;5m #### \x1B[36;43;5m #### \x1B[37;43;5m #### \x1B[30;43;1;5m #### \x1B[31;43;1;5m #### \x1B[32;43;1;5m #### \x1B[33;43;1;5m #### \x1B[34;43;1;5m #### \x1B[35;43;1;5m #### \x1B[36;43;1;5m #### \x1B[37;43;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;43;5m #### \x1B[31;43;5m #### \x1B[32;43;5m #### \x1B[33;43;5m #### \x1B[34;43;5m #### \x1B[35;43;5m #### \x1B[36;43;5m #### \x1B[37;43;5m #### \x1B[30;43;1;5m #### \x1B[31;43;1;5m #### \x1B[32;43;1;5m #### \x1B[33;43;1;5m #### \x1B[34;43;1;5m #### \x1B[35;43;1;5m #### \x1B[36;43;1;5m #### \x1B[37;43;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;44;5m #### \x1B[31;44;5m #### \x1B[32;44;5m #### \x1B[33;44;5m #### \x1B[34;44;5m #### \x1B[35;44;5m #### \x1B[36;44;5m #### \x1B[37;44;5m #### \x1B[30;44;1;5m #### \x1B[31;44;1;5m #### \x1B[32;44;1;5m #### \x1B[33;44;1;5m #### \x1B[34;44;1;5m #### \x1B[35;44;1;5m #### \x1B[36;44;1;5m #### \x1B[37;44;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m 44;5 \x1B[30;44;5m #### \x1B[31;44;5m #### \x1B[32;44;5m #### \x1B[33;44;5m #### \x1B[34;44;5m #### \x1B[35;44;5m #### \x1B[36;44;5m #### \x1B[37;44;5m #### \x1B[30;44;1;5m #### \x1B[31;44;1;5m #### \x1B[32;44;1;5m #### \x1B[33;44;1;5m #### \x1B[34;44;1;5m #### \x1B[35;44;1;5m #### \x1B[36;44;1;5m #### \x1B[37;44;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;44;5m #### \x1B[31;44;5m #### \x1B[32;44;5m #### \x1B[33;44;5m #### \x1B[34;44;5m #### \x1B[35;44;5m #### \x1B[36;44;5m #### \x1B[37;44;5m #### \x1B[30;44;1;5m #### \x1B[31;44;1;5m #### \x1B[32;44;1;5m #### \x1B[33;44;1;5m #### \x1B[34;44;1;5m #### \x1B[35;44;1;5m #### \x1B[36;44;1;5m #### \x1B[37;44;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;45;5m #### \x1B[31;45;5m #### \x1B[32;45;5m #### \x1B[33;45;5m #### \x1B[34;45;5m #### \x1B[35;45;5m #### \x1B[36;45;5m #### \x1B[37;45;5m #### \x1B[30;45;1;5m #### \x1B[31;45;1;5m #### \x1B[32;45;1;5m #### \x1B[33;45;1;5m #### \x1B[34;45;1;5m #### \x1B[35;45;1;5m #### \x1B[36;45;1;5m #### \x1B[37;45;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m 45;5 \x1B[30;45;5m #### \x1B[31;45;5m #### \x1B[32;45;5m #### \x1B[33;45;5m #### \x1B[34;45;5m #### \x1B[35;45;5m #### \x1B[36;45;5m #### \x1B[37;45;5m #### \x1B[30;45;1;5m #### \x1B[31;45;1;5m #### \x1B[32;45;1;5m #### \x1B[33;45;1;5m #### \x1B[34;45;1;5m #### \x1B[35;45;1;5m #### \x1B[36;45;1;5m #### \x1B[37;45;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;45;5m #### \x1B[31;45;5m #### \x1B[32;45;5m #### \x1B[33;45;5m #### \x1B[34;45;5m #### \x1B[35;45;5m #### \x1B[36;45;5m #### \x1B[37;45;5m #### \x1B[30;45;1;5m #### \x1B[31;45;1;5m #### \x1B[32;45;1;5m #### \x1B[33;45;1;5m #### \x1B[34;45;1;5m #### \x1B[35;45;1;5m #### \x1B[36;45;1;5m #### \x1B[37;45;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;46;5m #### \x1B[31;46;5m #### \x1B[32;46;5m #### \x1B[33;46;5m #### \x1B[34;46;5m #### \x1B[35;46;5m #### \x1B[36;46;5m #### \x1B[37;46;5m #### \x1B[30;46;1;5m #### \x1B[31;46;1;5m #### \x1B[32;46;1;5m #### \x1B[33;46;1;5m #### \x1B[34;46;1;5m #### \x1B[35;46;1;5m #### \x1B[36;46;1;5m #### \x1B[37;46;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m 46;5 \x1B[30;46;5m #### \x1B[31;46;5m #### \x1B[32;46;5m #### \x1B[33;46;5m #### \x1B[34;46;5m #### \x1B[35;46;5m #### \x1B[36;46;5m #### \x1B[37;46;5m #### \x1B[30;46;1;5m #### \x1B[31;46;1;5m #### \x1B[32;46;1;5m #### \x1B[33;46;1;5m #### \x1B[34;46;1;5m #### \x1B[35;46;1;5m #### \x1B[36;46;1;5m #### \x1B[37;46;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;46;5m #### \x1B[31;46;5m #### \x1B[32;46;5m #### \x1B[33;46;5m #### \x1B[34;46;5m #### \x1B[35;46;5m #### \x1B[36;46;5m #### \x1B[37;46;5m #### \x1B[30;46;1;5m #### \x1B[31;46;1;5m #### \x1B[32;46;1;5m #### \x1B[33;46;1;5m #### \x1B[34;46;1;5m #### \x1B[35;46;1;5m #### \x1B[36;46;1;5m #### \x1B[37;46;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;47;5m #### \x1B[31;47;5m #### \x1B[32;47;5m #### \x1B[33;47;5m #### \x1B[34;47;5m #### \x1B[35;47;5m #### \x1B[36;47;5m #### \x1B[37;47;5m #### \x1B[30;47;1;5m #### \x1B[31;47;1;5m #### \x1B[32;47;1;5m #### \x1B[33;47;1;5m #### \x1B[34;47;1;5m #### \x1B[35;47;1;5m #### \x1B[36;47;1;5m #### \x1B[37;47;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m 47;5 \x1B[30;47;5m #### \x1B[31;47;5m #### \x1B[32;47;5m #### \x1B[33;47;5m #### \x1B[34;47;5m #### \x1B[35;47;5m #### \x1B[36;47;5m #### \x1B[37;47;5m #### \x1B[30;47;1;5m #### \x1B[31;47;1;5m #### \x1B[32;47;1;5m #### \x1B[33;47;1;5m #### \x1B[34;47;1;5m #### \x1B[35;47;1;5m #### \x1B[36;47;1;5m #### \x1B[37;47;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m      \x1B[30;47;5m #### \x1B[31;47;5m #### \x1B[32;47;5m #### \x1B[33;47;5m #### \x1B[34;47;5m #### \x1B[35;47;5m #### \x1B[36;47;5m #### \x1B[37;47;5m #### \x1B[30;47;1;5m #### \x1B[31;47;1;5m #### \x1B[32;47;1;5m #### \x1B[33;47;1;5m #### \x1B[34;47;1;5m #### \x1B[35;47;1;5m #### \x1B[36;47;1;5m #### \x1B[37;47;1;5m #### " << std::endl;
		std::wcout << L"\x1B[0m" << std::endl;
	}
#endif

