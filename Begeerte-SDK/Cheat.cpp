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

namespace Cheat {
    static bool isRunning = false;
    static HANDLE cheatThread = nullptr;

    static void UpdateThread() {
        DWORD64 local_camera_position_address = Hook::GetLocalCameraPositionAddress();
        g_cheatdata->LocalCameraPosition = Vector::GetVec3(local_camera_position_address);

        DWORD64 local_player_address = Hook::GetLocalPlayerAddress();
        g_cheatdata->LocalPlayer = LocalPlayer::GetPlayer(local_player_address);

        DWORD64 client_current_time = Hook::GetClientCurrentTimeAddress();
        if (client_current_time) {
            g_cheatdata->ClientCurrentTime = static_cast<int>(reinterpret_cast<intptr_t>(reinterpret_cast<int*>(client_current_time)));
        }

        DWORD64 client_current_tick = Hook::GetClientCurrentTickAddress();
        if (client_current_tick) {
            g_cheatdata->ClientCurrentTick = static_cast<int>(reinterpret_cast<intptr_t>(reinterpret_cast<int*>(client_current_tick)));
        }
    }

    static void CheatThread() {
        while (isRunning) {
            UpdateThread(); // �������е�ַ

            /*
            * Starting
            */

            // printf("%d\n", g_cheatdata->ClientCurrentTime);
            // printf("%d\n", g_cheatdata->ClientCurrentTick);

            if (g_cheatdata->LocalCameraPosition.IsZero()) { // ��������Чֱ������
                printf("No Camera Position data.\n");
                continue;
            }

            // printf("Camera Position: %.1f, %.1f, %.1f\n", g_cheatdata->LocalCameraPosition.x, g_cheatdata->LocalCameraPosition.y, g_cheatdata->LocalCameraPosition.z);

            auto local_player = g_cheatdata->LocalPlayer;

            if (!local_player) { // ������������Чֱ������
                printf("No player data.\n");
                continue;
            }

            // ʵ���ϲ���������������Ѿ�����ָ����������
            // �������ķ����ˣ�Ψһ�Ŀ�������local_playerָ���˴�����ڴ�ṹ��
            if (!local_player->IsValid()) {
                printf("Player invalid.\n");
                continue;
            }

            if (!local_player->IsAlive()) {
                continue;
            }

            /*
            // ��Ӧ��ʹ��Health�����״̬����ʵ�����޷��ڱ����������֮����¡�
            // �������ķ����ˣ�Ψһ�Ŀ�������local_playerָ���˴�����ڴ�ṹ��
            if (local_player->Health <= 0.0f) {
                printf("Player dead, Health: %.1f\n", local_player->Health);
                continue;
            }
            */

            EntityList::Update();

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

                printf("Entity (%d) | Health: %d | GrowthStage: %d | SavedGrowth: %d | PosLand: (%.1f, %.1f, %.1f) | PosAir: (%.1f, %.1f, %.1f)\n", 
                    i, 
                    player->Health, 
                    player->GrowthStage, 
                    player->SavedGrowth,
                    player->PositionOnLandX,
                    player->PositionOnLandY,
                    player->PositionOnLandZ,
                    player->PositionInAirX,
                    player->PositionInAirY,
                    player->PositionInAirZ

                );
            }

            // local_player->Stamina = 200;

            // ��ȡ����������ֶ�ֵ

            /*
            printf("Player valid:\n");
            printf("  Health: %.1f\n", local_player->Health);
            printf("  Armor: %.1f\n", local_player->Armor);
            printf("  Stamina: %d\n", local_player->Stamina);
            printf("  SkinIndex: %d\n", local_player->SkinIndex);
            printf("  Gender: %d\n", local_player->Gender);
            printf("  Bile: %d\n", local_player->Bile);
            printf("  Hunger: %d\n", local_player->Hunger);
            printf("  Thirsty: %d\n", local_player->Thirsty);
            printf("  Disease: %d\n", local_player->Disease);

            printf("  VitalityHealth: %d\n", local_player->VitalityHealth);
            printf("  VitalityArmor: %d\n", local_player->VitalityArmor);
            printf("  VitalityBile: %d\n", local_player->VitalityBile);
            printf("  VitalityStamina: %d\n", local_player->VitalityStamina);
            printf("  VitalityHunger: %d\n", local_player->VitalityHunger);
            printf("  VitalityThirst: %d\n", local_player->VitalityThirst);
            printf("  VitalityTorpor: %d\n", local_player->VitalityTorpor);

            printf("  DamageBite: %d\n", local_player->DamageBite);
            printf("  DamageProjectile: %d\n", local_player->DamageProjectile);
            printf("  DamageSwipe: %d\n", local_player->DamageSwipe);

            printf("  MitigationBlunt: %d\n", local_player->MitigationBlunt);
            printf("  MitigationPierce: %d\n", local_player->MitigationPierce);
            printf("  MitigationFire: %d\n", local_player->MitigationFire);
            printf("  MitigationFrost: %d\n", local_player->MitigationFrost);
            printf("  MitigationAcid: %d\n", local_player->MitigationAcid);
            printf("  MitigationVenom: %d\n", local_player->MitigationVenom);
            printf("  MitigationPlasma: %d\n", local_player->MitigationPlasma);
            printf("  MitigationElectricity: %d\n", local_player->MitigationElectricity);
            printf("  OverallQuality: %d\n", local_player->OverallQuality);

            printf("  ViewAngles: %.1f, %.1f, %.1f\n", local_player->ViewAngleX, local_player->ViewAngleY, local_player->ViewAngleZ);
            printf("  fsPosition: %.1f, %.1f, %.1f\n", local_player->fsPositionX, local_player->fsPositionY, local_player->fsPositionZ);
            printf("  CameraDistance: %.1f\n", local_player->CameraDistance);
            */
        }
    }

    void Start() {
        if (!isRunning) {
            isRunning = true;
            cheatThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)CheatThread, nullptr, 0, nullptr);
            printf("Cheat thread started.\n");
        }
    }

    void Stop() {
        isRunning = false;
        if (cheatThread) {
            WaitForSingleObject(cheatThread, INFINITE);
            CloseHandle(cheatThread);
            cheatThread = nullptr;
        }
        printf("Cheat thread stopped.\n");
    }
}