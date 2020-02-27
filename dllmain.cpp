// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "ColouredBuffer.hpp"
#include "subhook/subhook.h"

#ifdef CONSOLECOLOUR_EXPORTS
    #define CONSOLECOLOUR_API __declspec(dllexport)
#else
    #define CONSOLECOLOUR_API __declspec(dllimport)
#endif

//cmdmd::ColouredBuffer<wchar_t>
//    * WCOUT,
//    * WCERR;
//
//cmdmd::ColouredBuffer<char>
//    * COUT,
//    * CERR;
//
//extern "C" CONSOLECOLOUR_API int ColouriseStdOut(void)
//{
//    if (!COUT)
//    {
//        COUT = new cmdmd::ColouredBuffer<char>(std::cout, true, false);
//    }
//    if (!WCOUT)
//    {
//        WCOUT = new cmdmd::ColouredBuffer<wchar_t>(std::wcout, true, false);
//    }
//    return 0;
//}
//
//extern "C" CONSOLECOLOUR_API int ColouriseStdErr(void)
//{
//    if (!CERR)
//    {
//        CERR = new cmdmd::ColouredBuffer<char>(std::cerr, true, true);
//    }
//    if (!WCERR)
//    {
//        WCERR = new cmdmd::ColouredBuffer<wchar_t>(std::wcerr, true, true);
//    }
//    return 0;
//}
//
//extern "C" CONSOLECOLOUR_API void StdOut(LPCTSTR line)
//{
//    printf("line");
//    //std::wcout << (wchar_t)line;
//}
//
//extern "C" CONSOLECOLOUR_API void StdErr(LPCTSTR line)
//{
//    printf("err");
//    //std::wcerr << (wchar_t)line;
//}
//
//extern "C" CONSOLECOLOUR_API void FlushOut(void)
//{
//    std::cout.flush();
//    std::wcout.flush();
//}
//
//extern "C" CONSOLECOLOUR_API void FlushErr(void)
//{
//    std::cerr.flush();
//    std::wcerr.flush();
//}

subhook_t
    WriteConsoleA_ = 0,
    WriteConsoleW_ = 0;

BOOL
WINAPI
Hook_WriteConsoleA(
    _In_ HANDLE hConsoleOutput,
    _In_reads_(nNumberOfCharsToWrite) CONST VOID* lpBuffer,
    _In_ DWORD nNumberOfCharsToWrite,
    _Out_opt_ LPDWORD lpNumberOfCharsWritten,
    _Reserved_ LPVOID lpReserved
)
{
    subhook_remove(WriteConsoleA_);
    WriteConsoleA(hConsoleOutput, "Hello ", 6, NULL, NULL);
    if (lpNumberOfCharsWritten) *lpNumberOfCharsWritten = nNumberOfCharsToWrite;
    return TRUE;
    BOOL ret = WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
    subhook_install(WriteConsoleA_);
    return ret;
}

BOOL
WINAPI
Hook_WriteConsoleW(
    _In_ HANDLE hConsoleOutput,
    _In_reads_(nNumberOfCharsToWrite) CONST VOID* lpBuffer,
    _In_ DWORD nNumberOfCharsToWrite,
    _Out_opt_ LPDWORD lpNumberOfCharsWritten,
    _Reserved_ LPVOID lpReserved
)
{
    subhook_remove(WriteConsoleW_);
    WriteConsoleW(hConsoleOutput, L"Hello ", 6, NULL, NULL);
    if (lpNumberOfCharsWritten) *lpNumberOfCharsWritten = nNumberOfCharsToWrite;
    return TRUE;
    BOOL ret = WriteConsoleW(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
    subhook_install(WriteConsoleW_);
    return ret;
}

extern "C" CONSOLECOLOUR_API void InstallConsoleColour(void)
{
    printf("attach\n");
    if (!WriteConsoleA_)
    {
        WriteConsoleA_ = subhook_new((void*)&WriteConsoleA, (void*)&Hook_WriteConsoleA, SUBHOOK_64BIT_OFFSET);
    }
    if (WriteConsoleA_)
    {
        subhook_install(WriteConsoleA_);
    }
    if (!WriteConsoleW_)
    {
        WriteConsoleW_ = subhook_new((void*)&WriteConsoleW, (void*)&Hook_WriteConsoleW, SUBHOOK_64BIT_OFFSET);
    }
    if (WriteConsoleW_)
    {
        subhook_install(WriteConsoleW_);
    }
}

extern "C" CONSOLECOLOUR_API void UninstallConsoleColour(void)
{
    printf("attach\n");
    if (WriteConsoleA_)
    {
        subhook_remove(WriteConsoleA_);
        subhook_free(WriteConsoleA_);
        WriteConsoleA_ = 0;
    }
    if (WriteConsoleW_)
    {
        subhook_remove(WriteConsoleW_);
        subhook_free(WriteConsoleW_);
        WriteConsoleW_ = 0;
    }
}

//extern "C" CONSOLECOLOUR_API int ColouriseStdOutAndStdErr(void)
//{
//    //if (ColouriseStdOut() || ColouriseStdErr())
//    //{
//    //    return 1;
//    //}
//    return 0;
//}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

