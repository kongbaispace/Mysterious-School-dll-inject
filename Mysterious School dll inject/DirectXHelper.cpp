#include "DirectXHelper.h"

HRESULT DirectXHelper::InitDeviceAndSwapChain(HWND hWnd, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    return D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &sd, ppSwapChain, ppDevice, nullptr, ppContext);
}

void DirectXHelper::CreateRenderTarget(IDXGISwapChain* pSwapChain, ID3D11Device* pDevice, ID3D11RenderTargetView** ppRenderTargetView) {
    ID3D11Texture2D* backBuffer = nullptr;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    pDevice->CreateRenderTargetView(backBuffer, nullptr, ppRenderTargetView);
    backBuffer->Release();
}

void DirectXHelper::ReleaseRenderTarget(ID3D11RenderTargetView*& pRenderTargetView) {
    if (pRenderTargetView) {
        pRenderTargetView->Release();
        pRenderTargetView = nullptr;
    }
}