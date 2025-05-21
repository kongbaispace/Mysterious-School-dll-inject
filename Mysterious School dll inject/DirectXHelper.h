#pragma once
#include <d3d11.h>
#include <dxgi.h>

class DirectXHelper {
public:
    static HRESULT InitDeviceAndSwapChain(HWND hWnd, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
    static void CreateRenderTarget(IDXGISwapChain* pSwapChain, ID3D11Device* pDevice, ID3D11RenderTargetView** ppRenderTargetView);
    static void ReleaseRenderTarget(ID3D11RenderTargetView*& pRenderTargetView);
};