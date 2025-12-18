#include <Windows.h>
#include "Hook.h"
#include "Cheat.h"
#include "ErrorHandler.h"
#include "Logs.h"
#include "Minimal-D3D12-Hook-ImGui/global.h"
#include "Minimal-D3D12-Hook-ImGui/hooks.h"
#include "Minimal-D3D12-Hook-ImGui/render.h"

#pragma comment(lib, "dxgi.lib")

static void InitializeDll() {
    ErrorHandlerNs::RegisterExceptionFilter();
    Hook::Hook_Initialize();
    Cheat::Start();
}

static void CleanupDll() {
    Cheat::Stop();
    Hook::Hook_Cleanup();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        if (CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)AMainThread, hModule, 0, nullptr) == nullptr) {
            return FALSE;
        }

        InitializeDll();
        break;
    case DLL_PROCESS_DETACH:
        if (lpReserved == nullptr) {
            FinalCleanupAll();
            MH_Uninitialize();
            CleanupDll();
        }
        break;
    }
    return TRUE;
}