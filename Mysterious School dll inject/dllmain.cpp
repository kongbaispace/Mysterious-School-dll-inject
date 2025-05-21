#include <Windows.h>
#include "HookHelper.h"

DWORD WINAPI InitThread(void*) {
    // 初始化 Hook
    HookHelper::Initialize();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // 创建一个线程来初始化 Hook
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
    }

    return TRUE;
}