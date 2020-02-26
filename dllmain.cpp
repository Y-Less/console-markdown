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

#define BUFSIZE 1024

void OpenTemp()
{
    CHAR szTempFileName[MAX_PATH];
    CHAR lpTempPathBuffer[MAX_PATH];
    //char  chBuffer[BUFSIZE];
    
    DWORD dwRetVal = GetTempPathA(MAX_PATH, lpTempPathBuffer); // buffer for path 
    if (dwRetVal > MAX_PATH || (dwRetVal == 0))
    {
        return;// NULL;
    }

    UINT uRetVal = GetTempFileNameA(lpTempPathBuffer, "stdout", 0, szTempFileName);
    if (uRetVal == 0)
    {
        return;// NULL;
    }

    freopen(szTempFileName, "w", stdout);
}

//#pragma comment(linker, "/EXPORT:ColouriseStdOut=_ColouriseStdOut@0")
extern "C" CONSOLECOLOUR_API int ColouriseStdOut(void)
{
    OpenTemp();
//    new cmdmd::ColouredBuffer<char>(std::cout, true);
//    new cmdmd::ColouredBuffer<wchar_t>(std::wcout, true);
    return 0;
}

//#pragma comment(linker, "/EXPORT:ColouriseStdErr=_ColouriseStdErr@0")
extern "C" CONSOLECOLOUR_API int ColouriseStdErr(void)
{
    new cmdmd::ColouredBuffer<char>(std::cerr, true);
    new cmdmd::ColouredBuffer<wchar_t>(std::wcerr, true);
    return 0;
}

//#pragma comment(linker, "/EXPORT:ColouriseStdOutAndStdErr=_ColouriseStdOutAndStdErr@0")
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

