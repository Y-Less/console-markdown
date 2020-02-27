// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "ColouredBuffer.hpp"

#ifdef CONSOLECOLOUR_EXPORTS
    #define CONSOLECOLOUR_API __declspec(dllexport)
#else
    #define CONSOLECOLOUR_API __declspec(dllimport)
#endif

cmdmd::ColouredBuffer<wchar_t>
    * WCOUT,
    * WCERR;

cmdmd::ColouredBuffer<char>
    * COUT,
    * CERR;

extern "C" CONSOLECOLOUR_API int ColouriseStdOut(void)
{
    if (!COUT)
    {
        COUT = new cmdmd::ColouredBuffer<char>(std::cout, true, false);
    }
    if (!WCOUT)
    {
        WCOUT = new cmdmd::ColouredBuffer<wchar_t>(std::wcout, true, false);
    }
    return 0;
}

extern "C" CONSOLECOLOUR_API int ColouriseStdErr(void)
{
    if (!CERR)
    {
        CERR = new cmdmd::ColouredBuffer<char>(std::cerr, true, true);
    }
    if (!WCERR)
    {
        WCERR = new cmdmd::ColouredBuffer<wchar_t>(std::wcerr, true, true);
    }
    return 0;
}

extern "C" CONSOLECOLOUR_API void StdOut(LPCTSTR line)
{
    printf("line");
    //std::wcout << (wchar_t)line;
}

extern "C" CONSOLECOLOUR_API void StdErr(LPCTSTR line)
{
    printf("err");
    //std::wcerr << (wchar_t)line;
}

extern "C" CONSOLECOLOUR_API void FlushOut(void)
{
    std::cout.flush();
    std::wcout.flush();
}

extern "C" CONSOLECOLOUR_API void FlushErr(void)
{
    std::cerr.flush();
    std::wcerr.flush();
}

extern "C" CONSOLECOLOUR_API int ColouriseStdOutAndStdErr(void)
{
    if (ColouriseStdOut() || ColouriseStdErr())
    {
        return 1;
    }
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

