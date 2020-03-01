#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "console-colour.h"
#include "subhook/subhook.h"

#define CONSOLECOLOUR_API __declspec(dllexport)

extern "C"
{
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
		if (nNumberOfCharsToWrite == 0)
		{
			if (lpNumberOfCharsWritten)
			{
				*lpNumberOfCharsWritten = 0;
			}
			return TRUE;
		}
		gStream.Handle = hConsoleOutput;
		int
			num = WriteColouredA((char const*)lpBuffer, nNumberOfCharsToWrite, &gStream);
		if (lpNumberOfCharsWritten)
		{
			*lpNumberOfCharsWritten = nNumberOfCharsToWrite;
		}
		return TRUE;
		//if (lpNumberOfCharsWritten)
		//{
		//	*lpNumberOfCharsWritten = num;
		//}
		//return num != 0;
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
		if (nNumberOfCharsToWrite == 0)
		{
			if (lpNumberOfCharsWritten)
			{
				*lpNumberOfCharsWritten = 0;
			}
			return TRUE;
		}
		gStream.Handle = hConsoleOutput;
		int
			num = WriteColouredW((wchar_t const*)lpBuffer, nNumberOfCharsToWrite, &gStream);
		if (lpNumberOfCharsWritten)
		{
			*lpNumberOfCharsWritten = nNumberOfCharsToWrite;
		}
		return TRUE;
		//if (lpNumberOfCharsWritten)
		//{
		//	*lpNumberOfCharsWritten = num;
		//}
		//return num != 0;
	}

	subhook_t
		WriteConsoleA_ = 0,
		WriteConsoleW_ = 0;

	void RemoveStreamHooks()
	{
		subhook_remove(WriteConsoleA_);
		subhook_remove(WriteConsoleW_);
	}

	void ReaddStreamHooks()
	{
		subhook_install(WriteConsoleW_);
		subhook_install(WriteConsoleA_);
	}

	CONSOLECOLOUR_API void InstallConsoleColour(void)
	{
		CONSOLE_SCREEN_BUFFER_INFO
			info;
		HANDLE
			handle = GetStdHandle(STD_OUTPUT_HANDLE);

		GetConsoleScreenBufferInfo(handle, &info);
		gStream.CurrentStyle = gStream.DefaultStyle = info.wAttributes;

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

	CONSOLECOLOUR_API void UninstallConsoleColour(void)
	{
		HANDLE
			handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, gStream.DefaultStyle);
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

