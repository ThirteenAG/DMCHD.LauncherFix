#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Hooking.Patterns.h"
#include "injector\injector.hpp"

DWORD WINAPI Init(LPVOID bDelay)
{
    auto pattern = hook::pattern("8B 4D 5C 81 F9 ? ? ? ? 0F 87 ? ? ? ? 81 7D");

    if (pattern.count_hint(1).empty() && !bDelay)
    {
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Init, (LPVOID)true, 0, NULL));
        return 0;
    }

    if (bDelay)
    {
        HANDLE hTimer = NULL;
        LARGE_INTEGER liDueTime;
        liDueTime.QuadPart = -100000000LL;
        hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
        SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
        while (pattern.clear().count_hint(1).empty()) { Sleep(0); if (WaitForSingleObject(hTimer, INFINITE) == WAIT_OBJECT_0) { CloseHandle(hTimer);  return 0; } };
    }

    injector::WriteMemory<int32_t>(pattern.get_first(5), INT_MAX, true);
    injector::WriteMemory<int32_t>(pattern.get_first(18), INT_MAX, true);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        Init(NULL);
    }
    return TRUE;
}