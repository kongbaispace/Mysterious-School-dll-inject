#include "HookHelper.h"
#include "ImGuiHelper.h"

void* HookHelper::original_present = nullptr;
void* HookHelper::original_resizebuffers = nullptr;


void HookHelper::Initialize() {
    // 初始化 MinHook
    if (MH_Initialize() != MH_OK) return;

    // 创建临时交换链，获取虚表
    D3D_FEATURE_LEVEL featureLevel;
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow();  // 确保获取的是正确的窗口句柄
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* pSwapChain;
    ID3D11Device* pDevice;
    ID3D11DeviceContext* pContext;

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &featureLevel, &pContext))) {
        return;
    }

    // 获取虚表地址
    void** vTable = *(void***)pSwapChain;
    void* present = vTable[8];  // Present 函数
    void* resizeBuffers = vTable[13];  // ResizeBuffers 函数

    // Hook 函数
    MH_CreateHook(present, HookedPresent, &original_present);
    MH_CreateHook(resizeBuffers, HookedResizeBuffers, &original_resizebuffers);
    MH_EnableHook(present);
    MH_EnableHook(resizeBuffers);


    pSwapChain->Release();
    pDevice->Release();
    pContext->Release();
}

HRESULT __stdcall HookHelper::HookedPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
    // 初始化 ImGui
    static bool initialized = false;
    if (!initialized) {
        ImGuiHelper::Initialize(swapChain);
        initialized = true;
    }

    // 渲染 ImGui
    ImGuiHelper::Render(swapChain);
    return ((HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT))original_present)(swapChain, syncInterval, flags);
}

HRESULT __stdcall HookHelper::HookedResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
    // 释放当前的渲染目标视图
    if (ImGuiHelper::GetRenderTargetView()) {
        ImGuiHelper::GetRenderTargetView()->Release();
        ImGuiHelper::g_mainRenderTargetView = nullptr;
    }

    // 调用原始 ResizeBuffers
    HRESULT hr = ((HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT))original_resizebuffers)(swapChain, bufferCount, width, height, newFormat, swapChainFlags);

    // 重新创建渲染目标视图
    ID3D11Texture2D* backBuffer = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    ImGuiHelper::GetDevice()->CreateRenderTargetView(backBuffer, nullptr, &ImGuiHelper::g_mainRenderTargetView);
    backBuffer->Release();

    return hr;
}

void HookHelper::Cleanup() {
    // 禁用并移除钩子
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);

    FreeConsole();

    // 释放资源
    ImGuiHelper::Cleanup();

    // 关闭 MinHook
    MH_Uninitialize();
}

DWORD __stdcall HookHelper::Unload(LPVOID lpParam)
{
    HookHelper::Cleanup();

    HMODULE hModule = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCTSTR>(Unload), &hModule);
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}
