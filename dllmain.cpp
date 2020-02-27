#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "console-colour.h"

#define CONSOLECOLOUR_API __declspec(dllexport)

extern "C" CONSOLECOLOUR_API void InstallConsoleColour(void)
{
    InitStreamHooks();
}

extern "C" CONSOLECOLOUR_API void UninstallConsoleColour(void)
{
    DeinitStreamHooks();
}

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

