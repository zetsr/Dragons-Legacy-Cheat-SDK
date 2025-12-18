#include "Cheat.h"
#include "Hook.h"
#include "Console.h"
#include "Memory.h"
#include <thread>
#include <stdio.h>
#include "SDK.h"
#include "Vector.h"
#include "CheatData.h"
#include "EntityList.h"
#include "Logs.h"
#include "Radar.h"
#include "Minimal-D3D12-Hook-ImGui/global.h"

namespace Cheat {
    static bool isRunning = false;
    static HANDLE cheatThread = nullptr;

    static void UpdateThread() {
        DWORD64 local_camera_position_address = Hook::GetLocalCameraPositionAddress();
        g_cheatdata->LocalCameraPosition = Vector::GetVec3(local_camera_position_address);

        DWORD64 local_player_address = Hook::GetLocalPlayerAddress();
        g_cheatdata->LocalPlayer = LocalPlayer::GetPlayer(local_player_address);

        DWORD64 client_current_time = Hook::GetClientCurrentTimeAddress();
        g_cheatdata->ClientCurrentTime = static_cast<int>(reinterpret_cast<intptr_t>(reinterpret_cast<int*>(client_current_time)));

        DWORD64 client_current_tick = Hook::GetClientCurrentTickAddress();
        g_cheatdata->ClientCurrentTick = static_cast<int>(reinterpret_cast<intptr_t>(reinterpret_cast<int*>(client_current_tick)));

        DWORD64 client_view_angles = Hook::GetClientViewAnglesAddress();
        g_cheatdata->ClientViewAngles = Vector::GetVec3(client_view_angles);
    }

    static void CheatThread() {
        while (isRunning) {
            UpdateThread();
            EntityList::Update();

            if (g_2d_radar) {
                Radar::RadarManager::GetInstance()->UpdateRadar(g_cheatdata->LocalCameraPosition, g_cheatdata->LocalPlayer);
            }

            if (g_cheatdata->LocalCameraPosition.IsZero()) {
                // Logs::LogManager::GetInstance()->Printf("No Camera Position data.\n");
                continue;
            }

            auto local_player = g_cheatdata->LocalPlayer;

            if (!local_player) {
                // Logs::LogManager::GetInstance()->Printf("No player data.\n");
                continue;
            }

            if (!local_player->IsValid()) {
                // Logs::LogManager::GetInstance()->Printf("Player invalid.\n");
                continue;
            }

            if (!local_player->IsAlive()) {
                // Logs::LogManager::GetInstance()->Printf("Player dead.\n");
                continue;
            }

            for (int i = 1; i <= EntityList::GetMaxPlayers(); i++) {

                auto entity = EntityList::GetEntity(i);

                if (entity == 0) {
                    continue;
                }

                auto player = EntityList::GetPlayer(i);

                if (!player) {
                    continue;
                }

                if (!player->IsValid()) {
                    continue;
                }

                if (!player->IsAlive()) {
                    continue;
                }

                if (!player->IsValidPos()) {
                    continue;
                }

                Logs::LogManager::GetInstance()->Printf(L"Entity (%d) | HP: %d | GrowthStage: %d | SavedGrowth: %d | Pos: (%.1f, %.1f, %.1f) | Distance: %.1f\n",
                        i,
                        player->Health,
                        player->GrowthStage,
                        player->SavedGrowth,
                        player->GetPos().x,
                        player->GetPos().y,
                        player->GetPos().z,
                        Vector::CreateVec3(player->GetPos().x, player->GetPos().y, player->GetPos().z).Distance
                        (
                            Vector::CreateVec3(local_player->fsPositionX, local_player->fsPositionY, local_player->fsPositionZ)
                        )
                    );

            }
        }
    }

    void Start() {
        if (!isRunning) {
            isRunning = true;
            cheatThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)CheatThread, nullptr, 0, nullptr);
            Logs::LogManager::GetInstance()->Printf(L"Cheat thread started.\n");
        }
    }

    void Stop() {
        isRunning = false;
        if (cheatThread) {
            WaitForSingleObject(cheatThread, INFINITE);
            CloseHandle(cheatThread);
            cheatThread = nullptr;
        }
        Logs::LogManager::GetInstance()->Printf(L"Cheat thread stopped.\n");
    }
}