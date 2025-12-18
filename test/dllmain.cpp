/*
* 
* Credit@ https://github.com/Sh0ckFR/Universal-Dear-ImGui-Hook
* 
*/

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <vector>
#include <mutex>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx12.h"
#include "MinHook/include/MinHook.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// --- 类型定义 ---
typedef HRESULT(STDMETHODCALLTYPE* PFN_Present)(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(STDMETHODCALLTYPE* PFN_Present1)(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags, const DXGI_PRESENT_PARAMETERS* pParams);
typedef void(STDMETHODCALLTYPE* PFN_ExecuteCommandLists)(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);
typedef HRESULT(STDMETHODCALLTYPE* PFN_ResizeBuffers)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

// --- 全局变量 ---
PFN_Present             oPresent = nullptr;
PFN_Present1            oPresent1 = nullptr;
PFN_ExecuteCommandLists oExecuteCommandLists = nullptr;
PFN_ResizeBuffers       oResizeBuffers = nullptr;

ID3D12Device* g_pd3dDevice = nullptr;
ID3D12CommandQueue* g_pd3dCommandQueue = nullptr;
ID3D12DescriptorHeap* g_pd3dRtvDescHeap = nullptr;
ID3D12DescriptorHeap* g_pd3dSrvDescHeap = nullptr;
ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
ID3D12Fence* g_fence = nullptr;
HANDLE                      g_fenceEvent = nullptr;
UINT64                      g_fenceValue = 0;
UINT                        g_bufferCount = 0;

struct FrameContext {
    ID3D12CommandAllocator* CommandAllocator = nullptr;
    ID3D12Resource* Resource = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE Descriptor{};
};

std::vector<FrameContext>   g_FrameContexts;
bool                        g_Initialized = false;
bool                        g_AfterFirstPresent = false;
std::mutex                  g_InitMutex;

// --- 彻底清理所有资源 ---
void CleanupRenderResources() {
    g_Initialized = false;

    if (ImGui::GetCurrentContext()) {
        ImGui_ImplDX12_InvalidateDeviceObjects();
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = nullptr; }
    if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = nullptr; }
    if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = nullptr; }
    if (g_fence) { g_fence->Release(); g_fence = nullptr; }
    if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = nullptr; }

    for (auto& frame : g_FrameContexts) {
        if (frame.Resource) { frame.Resource->Release(); frame.Resource = nullptr; }
        if (frame.CommandAllocator) { frame.CommandAllocator->Release(); frame.CommandAllocator = nullptr; }
    }
    g_FrameContexts.clear();
    g_bufferCount = 0;
}

// --- Hook: ExecuteCommandLists (抓取 CommandQueue) ---
void STDMETHODCALLTYPE hkExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists) {
    if (!g_pd3dCommandQueue && g_AfterFirstPresent && queue) {
        D3D12_COMMAND_QUEUE_DESC desc = queue->GetDesc();
        if (desc.Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
            ID3D12Device* tempDevice = nullptr;
            if (SUCCEEDED(queue->GetDevice(IID_PPV_ARGS(&tempDevice)))) {
                if (!g_pd3dDevice) {
                    g_pd3dDevice = tempDevice;
                }
                else if (tempDevice != g_pd3dDevice) {
                    tempDevice->Release();
                    goto call_original;
                }
                else {
                    tempDevice->Release();
                }
                queue->AddRef();
                g_pd3dCommandQueue = queue;
            }
        }
    }

call_original:
    oExecuteCommandLists(queue, NumCommandLists, ppCommandLists);
}

// --- Hook: Present ---
HRESULT STDMETHODCALLTYPE hkPresent(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {
    g_AfterFirstPresent = true;

    if (!g_pd3dCommandQueue) {
        return oPresent(pSwapChain, SyncInterval, Flags);
    }

    std::lock_guard<std::mutex> lock(g_InitMutex);

    if (!g_Initialized) {
        if (FAILED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice)))) {
            return oPresent(pSwapChain, SyncInterval, Flags);
        }

        DXGI_SWAP_CHAIN_DESC desc{};
        pSwapChain->GetDesc(&desc);
        g_bufferCount = desc.BufferCount;

        // RTV Heap
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
        rtvHeapDesc.NumDescriptors = g_bufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        g_pd3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_pd3dRtvDescHeap));

        // SRV Heap (ImGui 字体纹理)
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        g_pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&g_pd3dSrvDescHeap));

        // Frame contexts
        g_FrameContexts.resize(g_bufferCount);
        UINT rtvIncrementSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

        for (UINT i = 0; i < g_bufferCount; ++i) {
            g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_FrameContexts[i].CommandAllocator));
            pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_FrameContexts[i].Resource));
            g_FrameContexts[i].Descriptor = rtvHandle;
            g_pd3dDevice->CreateRenderTargetView(g_FrameContexts[i].Resource, nullptr, rtvHandle);
            rtvHandle.ptr += rtvIncrementSize;
        }

        // Command list
        g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_FrameContexts[0].CommandAllocator, nullptr, IID_PPV_ARGS(&g_pd3dCommandList));
        g_pd3dCommandList->Close();

        // Fence
        g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence));
        g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        // ImGui 初始化
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(desc.OutputWindow);
        ImGui_ImplDX12_Init(g_pd3dDevice, g_bufferCount,
            desc.BufferDesc.Format,
            g_pd3dSrvDescHeap,
            g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
            g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

        // 强制构建字体图集
        unsigned char* pixels = nullptr;
        int width = 0, height = 0;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height); 
        // DX12 后端会在 NewFrame() 中自动上传纹理

        g_Initialized = true;
    }

    UINT backBufferIdx = pSwapChain->GetCurrentBackBufferIndex();
    FrameContext& frameCtx = g_FrameContexts[backBufferIdx];

    // 等待上一帧完成
    if (g_fence->GetCompletedValue() < g_fenceValue) {
        g_fence->SetEventOnCompletion(g_fenceValue, g_fenceEvent);
        WaitForSingleObject(g_fenceEvent, 2000);
    }

    frameCtx.CommandAllocator->Reset();
    g_pd3dCommandList->Reset(frameCtx.CommandAllocator, nullptr);

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = frameCtx.Resource;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    g_pd3dCommandList->ResourceBarrier(1, &barrier);

    g_pd3dCommandList->OMSetRenderTargets(1, &frameCtx.Descriptor, FALSE, nullptr);

    ID3D12DescriptorHeap* ppHeaps[] = { g_pd3dSrvDescHeap };
    g_pd3dCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 示例菜单
    ImGui::Begin("DX12 ImGui Hook");
    ImGui::Text("Hook Status: Active & Stable");
    ImGui::Text("Resolution change: Supported");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    g_pd3dCommandList->ResourceBarrier(1, &barrier);

    g_pd3dCommandList->Close();

    oExecuteCommandLists(g_pd3dCommandQueue, 1, (ID3D12CommandList* const*)&g_pd3dCommandList);
    g_pd3dCommandQueue->Signal(g_fence, ++g_fenceValue);

    return oPresent(pSwapChain, SyncInterval, Flags);
}

// --- Hook: ResizeBuffers ---
HRESULT STDMETHODCALLTYPE hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    std::lock_guard<std::mutex> lock(g_InitMutex);

    if (g_Initialized) {
        CleanupRenderResources();  // 彻底释放旧资源
    }

    return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

// --- 注入线程 ---
DWORD WINAPI MainThread(LPVOID) {
    Sleep(5000);

    MH_Initialize();

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProcW, 0, 0, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"TempDX12", nullptr };
    RegisterClassEx(&wc);
    HWND tempWnd = CreateWindow(wc.lpszClassName, L"Temp", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

    ID3D12Device* tempDevice = nullptr;
    ID3D12CommandQueue* tempQueue = nullptr;
    IDXGIFactory4* factory = nullptr;
    IDXGISwapChain* tempSwapChain = nullptr;

    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&tempDevice));
    D3D12_COMMAND_QUEUE_DESC qdesc = { D3D12_COMMAND_LIST_TYPE_DIRECT };
    tempDevice->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&tempQueue));
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = tempWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    factory->CreateSwapChain(tempQueue, &sd, &tempSwapChain);

    void** swapChainVTable = *(void***)tempSwapChain;
    void** queueVTable = *(void***)tempQueue;

    MH_CreateHook(swapChainVTable[8], hkPresent, (LPVOID*)&oPresent);
    MH_CreateHook(swapChainVTable[13], hkResizeBuffers, (LPVOID*)&oResizeBuffers);
    MH_CreateHook(queueVTable[10], hkExecuteCommandLists, (LPVOID*)&oExecuteCommandLists);

    MH_EnableHook(MH_ALL_HOOKS);

    if (tempSwapChain) tempSwapChain->Release();
    if (factory) factory->Release();
    if (tempQueue) tempQueue->Release();
    if (tempDevice) tempDevice->Release();
    DestroyWindow(tempWnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }
    return TRUE;
}