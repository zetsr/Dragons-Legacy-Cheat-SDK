﻿#include <Windows.h>
#include "Hook.h"
#include "Cheat.h"
#include "SDK.h"

static void InitializeDll() {
    Hook::Initialize();
    Cheat::Start();
}

static void CleanupDll() {
    Cheat::Stop();
    Hook::Cleanup();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        InitializeDll();
        break;
    case DLL_PROCESS_DETACH:
        CleanupDll();
        break;
    }
    return TRUE;
}