#include "Cheat.h"
#include "Hook.h"
#include "Console.h"
#include "Memory.h"
#include <thread>
#include <stdio.h>
#include "SDK.h"

namespace Cheat {
    static bool isRunning = false;
    static HANDLE cheatThread = nullptr;

    static void CheatThread() {
        while (isRunning) {
            DWORD64 playerAddress = Hook::GetLocalPlayerAddress();
            LocalPlayer::Player* player = LocalPlayer::GetPlayer(playerAddress);

            if (!player) {
                printf("No player data.\n");
                continue;
            }

            if (!player->isValid()) {
                printf("Player invalid.\n"); // ʵ���ϲ���������������Ѿ�����ָ����������
                continue;
            }

            if (player->Health <= 0.0f) {
                printf("Player dead, Health: %.1f\n", player->Health); // ��Ӧ��ʹ��Health�����״̬���������Ѫ����ֵ��������֮��ֹͣ���£��޷����㡣
                continue;
            }

            // ��ȡ����������ֶ�ֵ
            printf("Player valid:\n");
            printf("  Health: %.1f\n", player->Health);
            printf("  Armor: %.1f\n", player->Armor);
            printf("  Stamina: %d\n", player->Stamina);
            printf("  SkinIndex: %d\n", player->SkinIndex);
            printf("  Gender: %d\n", player->Gender);
            printf("  Bile: %d\n", player->Bile);
            printf("  Hunger: %d\n", player->Hunger);
            printf("  Thirsty: %d\n", player->Thirsty);
            printf("  Disease: %d\n", player->Disease);

            printf("  VitalityHealth: %d\n", player->VitalityHealth);
            printf("  VitalityArmor: %d\n", player->VitalityArmor);
            printf("  VitalityBile: %d\n", player->VitalityBile);
            printf("  VitalityStamina: %d\n", player->VitalityStamina);
            printf("  VitalityHunger: %d\n", player->VitalityHunger);
            printf("  VitalityThirst: %d\n", player->VitalityThirst);
            printf("  VitalityTorpor: %d\n", player->VitalityTorpor);

            printf("  DamageBite: %d\n", player->DamageBite);
            printf("  DamageProjectile: %d\n", player->DamageProjectile);
            printf("  DamageSwipe: %d\n", player->DamageSwipe);

            printf("  MitigationBlunt: %d\n", player->MitigationBlunt);
            printf("  MitigationPierce: %d\n", player->MitigationPierce);
            printf("  MitigationFire: %d\n", player->MitigationFire);
            printf("  MitigationFrost: %d\n", player->MitigationFrost);
            printf("  MitigationAcid: %d\n", player->MitigationAcid);
            printf("  MitigationVenom: %d\n", player->MitigationVenom);
            printf("  MitigationPlasma: %d\n", player->MitigationPlasma);
            printf("  MitigationElectricity: %d\n", player->MitigationElectricity);
            printf("  OverallQuality: %d\n", player->OverallQuality);

            printf("  ViewAngles: %.1f, %.1f, %.1f\n", player->viewAngleX, player->viewAngleY, player->viewAngleZ);
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