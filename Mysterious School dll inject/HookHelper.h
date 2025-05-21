#pragma once
#include <d3d11.h>
#include <MinHook.h>
#include "ImGuiHelper.h"
class HookHelper {
public:
    static void Initialize();
    static HRESULT __stdcall HookedPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
    static HRESULT __stdcall HookedResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
    static void Cleanup();
    static DWORD __stdcall Unload(LPVOID lpParam); // 新增的卸载函数声明

private:
    static void* original_present;
    static void* original_resizebuffers;
};