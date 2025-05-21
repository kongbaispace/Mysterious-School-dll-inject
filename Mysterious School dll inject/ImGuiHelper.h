#pragma once
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "regex"
class ImGuiHelper {
public:
    static void Initialize(IDXGISwapChain* swapChain);
    static void menu();
    static void Render(IDXGISwapChain* swapChain);
    static void Cleanup();

    // Getter º¯Êý
    static ID3D11Device* GetDevice() { return g_pd3dDevice; }
    static ID3D11DeviceContext* GetDeviceContext() { return g_pd3dContext; }
    static ID3D11RenderTargetView* GetRenderTargetView() { return g_mainRenderTargetView; }
    static ID3D11Device* g_pd3dDevice;
    static ID3D11DeviceContext* g_pd3dContext;
    static ID3D11RenderTargetView* g_mainRenderTargetView;
};