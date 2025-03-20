#include "Hook.h"
#include "Console.h"
#include "PointerScanner.h"
#include "Offset.h"
#include "LocalPlayer.h"
#include "Memory.h"
#include <thread>
#include <stdio.h>

namespace Hook {
    static bool isRunning = false;
    static HANDLE hookThread = nullptr;
    static DWORD64 localPlayerAddress = 0;

    static void UpdateThread() {
        DWORD64 moduleBase = Memory::GetModuleBase("Dragons-Win64-Shipping.exe");
        std::vector<std::pair<DWORD64, bool>> debugSteps;

        while (isRunning) {
            DWORD64 baseAddress = moduleBase + Offset::LocalPlayerBase::MODULE_OFFSET;
            auto [playerAddress, success] = PointerScanner::Scan(baseAddress, Offset::LocalPlayerBase::OFFSETS, debugSteps);
            localPlayerAddress = success ? playerAddress : 0; // 更新全局地址
        }
    }

    void Initialize() {
        Console::Initialize();
        isRunning = true;
        hookThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)UpdateThread, nullptr, 0, nullptr);
        printf("Hook initialized.\n");
    }

    void Cleanup() {
        isRunning = false;
        if (hookThread) {
            WaitForSingleObject(hookThread, INFINITE);
            CloseHandle(hookThread);
            hookThread = nullptr;
        }
        Console::Cleanup();
    }

    DWORD64 GetLocalPlayerAddress() {
        return localPlayerAddress;
    }
}