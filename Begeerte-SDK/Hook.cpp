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

    static DWORD64 LocalPlayerAddress = 0;
    static DWORD64 LocalCameraPositionAddress = 0;
    static DWORD64 ClientCurrentTimeAddress = 0;
    static DWORD64 ClientCurrentTickAddress = 0;

    static void UpdateThread() {
        DWORD64 moduleBase = Memory::GetModuleBase("Dragons-Win64-Shipping.exe");
        std::vector<std::pair<DWORD64, bool>> debugSteps;

        while (isRunning) {
            DWORD64 local_player_base = moduleBase + Offset::LocalPlayerBase::MODULE_OFFSET;
            auto [local_player_address, local_player_success] = PointerScanner::Scan(local_player_base, Offset::LocalPlayerBase::OFFSETS, debugSteps);
            LocalPlayerAddress = local_player_success ? local_player_address : 0;

            DWORD64 local_camera_position_base = moduleBase + Offset::LocalCameraPosition::MODULE_OFFSET;
            auto [local_camera_position_address, local_camera_position_success] = PointerScanner::Scan(local_camera_position_base, Offset::LocalCameraPosition::OFFSETS, debugSteps);
            LocalCameraPositionAddress = local_camera_position_success ? local_camera_position_address : 0;

            DWORD64 client_current_time_base = moduleBase + Offset::ClientCurrentTime::MODULE_OFFSET;
            auto [client_current_time_address, client_current_time_success] = PointerScanner::Scan(client_current_time_base, Offset::ClientCurrentTime::OFFSETS, debugSteps);
            ClientCurrentTimeAddress = client_current_time_success ? client_current_time_address : 0;

            DWORD64 client_current_tick_base = moduleBase + Offset::ClientCurrentTick::MODULE_OFFSET;
            auto [client_current_tick_address, client_current_tick_success] = PointerScanner::Scan(client_current_tick_base, Offset::ClientCurrentTick::OFFSETS, debugSteps);
            ClientCurrentTickAddress = client_current_tick_success ? client_current_tick_address : 0;
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
        return LocalPlayerAddress;
    }

    DWORD64 GetLocalCameraPositionAddress() {
        return LocalCameraPositionAddress;
    }

    DWORD64 GetClientCurrentTimeAddress() {
        return ClientCurrentTimeAddress;
    }

    DWORD64 GetClientCurrentTickAddress() {
        return ClientCurrentTickAddress;
    }
}