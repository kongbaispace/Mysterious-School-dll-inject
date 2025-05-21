#include "ImGuiHelper.h"
#include "memory.h"
#include "HookHelper.h"
#include "engine.h"
#include "function.hpp"

ID3D11Device* ImGuiHelper::g_pd3dDevice = nullptr;
ID3D11DeviceContext* ImGuiHelper::g_pd3dContext = nullptr;
ID3D11RenderTargetView* ImGuiHelper::g_mainRenderTargetView = nullptr;
HWND g_hwnd=nullptr;

uint64_t base=NULL;
UWorld* world=nullptr;
uint64_t uworld = NULL;
view_matrix_t matrix = {0,0,0};

vec4 clipCoords;
FVector pos,NDC;
int width = 0;
int height = 0;

// �洢ԭʼ���ڹ��̺���
WNDPROC origin_wndProc;

// ImGui �� Win32 ��Ϣ������
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

// �Զ��崰�ڹ��̣����ڴ��� ImGui �������¼�
LRESULT __stdcall WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // ��� ImGui ��Ϣ�������Ѵ�����¼����򷵻� true
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
        return true;
    }

    // ����ԭʼ���ڹ���
    return CallWindowProc(origin_wndProc, hwnd, uMsg, wParam, lParam);
}

void GetWindowClientSize(HWND hWnd, int& width, int& height) {
    RECT rect;
    if (GetClientRect(hWnd, &rect)) {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
    else {
        std::cerr << "Failed to get client rect." << std::endl;
    }
}

void ImGuiHelper::Initialize(IDXGISwapChain* swapChain) {
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain->GetDesc(&desc);

    // ��ȡ�豸��������
    swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice);
    g_pd3dDevice->GetImmediateContext(&g_pd3dContext);

    // ������ȾĿ����ͼ
    ID3D11Texture2D* backBuffer = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    g_pd3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_mainRenderTargetView);
    backBuffer->Release();

    // ��ʼ�� ImGui
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(desc.OutputWindow);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:/Windows/Fonts/msyh.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    ImGui::StyleColorsDark();
    g_hwnd = desc.OutputWindow;
    origin_wndProc = (WNDPROC)SetWindowLongPtr(desc.OutputWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);  // �滻ԭ���Ĵ��ڹ���

    //�������̨
    if (AllocConsole()) {
        // �ض��� stdout �� stderr ������̨
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);  // ���������̨
        freopen_s(&fp, "CONOUT$", "w", stderr);  // �������������̨

        std::cout << "�ѷ������̨��" << std::endl;

    }
    else {
        std::cerr << "��������̨ʧ�ܣ�" << std::endl;
    }
}

bool done = true;

int id = 0;
auto GetNameById = [](int32_t id) -> std::string {
    // FNameEntryHandle Handle(id); ��Ϊֱ��ʹ�� id
    int32_t Block = (id >> 16) & 0x7FFF;   // ��ţ������16λ��Offset����15λ��Block��
    int32_t Offset = id & 0xFFFF;          // ƫ����

    HMODULE ModuleBase = GetModuleHandle(nullptr);
    uint64_t NamePoolBase = reinterpret_cast<uint64_t>(ModuleBase) + 0x49F3280;

    // ��ȡ��ָ��
    uint64_t nameptr = ReadLong(NamePoolBase + 0x10 + (static_cast<uint64_t>(Block) * 8));
    if (!nameptr) {
        return "NULL!";
    }

    // ƫ�Ƶ������ַ���λ��
    nameptr += 0x2 * Offset;

    // ��ȡ�ַ������ȣ���λ���ܰ�����־λ��
    uint32_t namelen = ReadUShort(nameptr) >> 6;

    // ��ȡ�ַ�������
    char buffer[1024] = { 0 };
    memcpy(buffer, reinterpret_cast<void*>(nameptr + 0x2), namelen);

    return std::string(buffer, namelen);
    };


DWORD64 objectaddress;

void ImGuiHelper::menu()
{
    base = (uint64_t)GetModuleHandle(NULL);
    std::stringstream s1, s2;

    uworld = ReadLong(base + 0x4B77870);
    world = (UWorld*)*(reinterpret_cast<uint64_t*>(base + 0x4B77870));
    TUObjectArray* TUobject = (TUObjectArray*)(base + 0x4A2F5D0);

    objectaddress = ReadLong(base + 0x4A2F5D0);

    //matrix
    DWORD64 canvas = ReadLong(base + 0x0466F3E0);
    canvas=ReadLong(canvas + 0x20);
    ReadMatrixArray(canvas + 0x280, matrix.matrix);


    for (int i = 0; i < world->PersistentLevel->Actors.NumElements; i++)
    {
        DWORD64 objectaddress = (uint64_t)world->PersistentLevel->Actors.Data[i];
        if (!objectaddress)
            continue;
        std::string objectName = world->PersistentLevel->Actors.Data[i]->GetName().c_str();
        
        FVector seccess = world->PersistentLevel->Actors.Data[i]->K2_GetActorLocation().WorldToScreen(matrix, width, height);
        if (seccess.X == -1.f && seccess.Y == -1.f && seccess.Z== 0.f)
            continue;
        ImGui::GetForegroundDrawList()->AddText({ seccess.X,seccess.Y }, ImColor(255, 255, 255), objectName.c_str());

        //FVector origin = ;
        //FVector headpos = { origin.x, origin.y, origin.z + 75.f };

        //std::string lowerObjectName = toLower(objectName);
        //
        //
        //std::string player = toLower("char");
        //if (lowerObjectName.find(player) != std::string::npos)
        //{
        //    const DWORD64 HEALTH_OFFSET = 0x7AC;

        //    // ������ָ���Ƿ�ƥ��
        //    if (objectaddress != getmyselfaddress())
        //    {
        //        // ���ƾ��κ��ı�
        //        ImGui::GetForegroundDrawList()->AddRect(
        //            ImVec2(screenHeadPos.x - width / 2, screenHeadPos.y),
        //            ImVec2(ckzpm.x, ckzpm.y + ckzpm.z),
        //            IM_COL32(0, 225, 0, 255)
        //        );
        //        ImGui::GetForegroundDrawList()->AddText(
        //            ImVec2(ckzpm.x + (ckzpm.w / 2), ckzpm.y + ckzpm.z - 15.0f),
        //            IM_COL32(0, 255, 239, 255),
        //            "���"
        //        );

        //        // ������ҵ��������͵�ǰ����
        //    }
        //    if (objectaddress == getmyselfaddress()) {
        //        float maxHealth = 10000.0f;
        //        WriteFloat(objectaddress + HEALTH_OFFSET - 0x4, maxHealth);
        //        WriteFloat(objectaddress + HEALTH_OFFSET , maxHealth);
        //    }
        //}
        //std::string NPC = toLower("danren");
        //if (lowerObjectName.find(NPC) != std::string::npos)
        //{
        //    ImGui::GetForegroundDrawList()->AddLine({ (float)width / 2,0 }, { ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f }, ImColor(255, 255, 255));
        //    ImGui::GetForegroundDrawList()->AddText({ ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f }, ImColor(0, 255, 239), "NPC");
        //    ImGui::GetForegroundDrawList()->AddRect({ ckzpm.x + ckzpm.w ,ckzpm.y }, { ckzpm.x ,ckzpm.y + ckzpm.z }, ImColor(0, 225, 0));
        //}
        
    }


    s1 << u8"�������" << std::hex << std::showbase << GetmySelfAddress();
    s2 << u8"GWorld��" << std::hex << std::showbase << uworld;

    ImGui::Begin("Example Window");
    ImGui::Text(s1.str().c_str());
    ImGui::Text(s2.str().c_str());
    ImGui::Text(std::to_string(world->PersistentLevel->Actors.NumElements).c_str());
    ImGui::InputInt(u8"ID", &id);

    if (ImGui::Button(u8"���ͳ�����"))
    {
        transmission(TUobject, 1);
    }

    if (ImGui::Button(u8"������¥"))
    {
        transmission(TUobject, 2);
    }

    if (ImGui::Button(u8"�޸�����"))
    {
        void* ld = TUObjectArray::FindObject(TUobject, "Engine.PostProcessVolume");
        printf("PostProcessVolume��%p\n",ld);
        printf("AutoExposureBias��%f\n", *(float*)((uintptr_t)ld + 0x260 + 0x0314));
    }
    if (ImGui::Button(u8"�޸�FOV")) {
        std::regex target_regex("CameraActor", std::regex_constants::icase);
        for (int i = 0; i < world->PersistentLevel->Actors.NumElements; i++) {
            DWORD64 OBJaddress = (uint64_t)world->PersistentLevel->Actors.Data[i];
            if (!objectaddress)
                continue;
            std::string objectName = world->PersistentLevel->Actors.Data[i]->GetName().c_str();
            if (std::regex_search(objectName, target_regex)) {
                printf("CameraActor��%p\n", OBJaddress);
                UObject* CameraComponent = (UObject*)ReadLong(OBJaddress + 0x228);
                void* SetFieldOfView = TUObjectArray::FindObject(TUobject, "SetFieldOfView");
                struct { float InFieldOfView=150.f; } Parms;
                CameraComponent->ProcessEvent(SetFieldOfView, &Parms);
                break;
            }

        }

    }

    if (ImGui::Button(u8"�˳�"))
        CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)HookHelper::Unload, NULL, NULL, NULL)); // ���̵߳���ж�غ���

    ImGui::End();
}

void ImGuiHelper::Render(IDXGISwapChain* swapChain) {
    GetWindowClientSize(g_hwnd, width,height);
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    menu();

    

    ImGui::Render();
    g_pd3dContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiHelper::Cleanup() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}