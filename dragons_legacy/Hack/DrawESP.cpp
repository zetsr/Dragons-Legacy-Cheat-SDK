#include "../Minimal-D3D12-Hook-ImGui-1.0.2/Main/mdx12_api.h"
#include "SDK_Headers.hpp"
#include "ESP.h"
#include "Configs.h"
#include "DrawESP.h"

namespace g_DrawESP {
    // 定义一个简单的结构体来存放名称和颜色
    struct DisplayInfo {
        std::string name;
        ImU32 color;
    };

    // 获取物种信息
    DisplayInfo GetSpeciesInfo(SDK::AChar_Parent_All_C* BaseChar) {
        SDK::FString fName = L"未知";
        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        switch (BaseChar->BiologicalSpecies) {
        case SDK::Enum_Species::NewEnumerator1: fName = L"逐火龙"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator2: fName = L"掠狱龙"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 1.0f, 1.0f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator3: fName = L"伏影龙"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 0.4f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator4: fName = L"酸涎龙"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.6f, 1.0f, 0.4f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator6: fName = L"震雷龙"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.4f, 0.7f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator14: fName = L"顾雏龙"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 1.0f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator16: fName = L"萤灵龙"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 1.0f, 1.0f)); break;
        default: fName = SDK::FString(std::to_wstring((unsigned char)BaseChar->BiologicalSpecies).c_str()); break;
        }
        return { fName.ToString(), color };
    }

    // 获取成长阶段信息
    DisplayInfo GetGrowthStageInfo(SDK::AChar_Parent_All_C* BaseChar) {
        SDK::FString fName = L"未知阶段";
        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        switch (BaseChar->GrowthStage) {
        case SDK::Enum_GrowthStage::NewEnumerator1: fName = L"幼年"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); break;
        case SDK::Enum_GrowthStage::NewEnumerator2: fName = L"青年"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.5f, 1.0f)); break;
        case SDK::Enum_GrowthStage::NewEnumerator3: fName = L"成年"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.6f, 1.0f, 1.0f)); break;
        case SDK::Enum_GrowthStage::NewEnumerator4: fName = L"长者"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); break;
        case SDK::Enum_GrowthStage::NewEnumerator5: fName = L"远古"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 1.0f, 1.0f)); break;
        }
        return { fName.ToString(), color };
    }

    void DrawESP() {
        SDK::UWorld* World = SDK::UWorld::GetWorld();
        if (!World || !World->GameState || !World->PersistentLevel) return;

        SDK::APlayerController* LocalPC = g_ESP::GetLocalPC();
        if (!LocalPC || !LocalPC->Pawn) return;

        SDK::APlayerState* LocalPS = LocalPC->PlayerState;
        if (!LocalPS) return;

        ImGuiIO& io = ImGui::GetIO();
        float screenW = io.DisplaySize.x;
        float screenH = io.DisplaySize.y;

        // 遍历场景中所有 Actor
        SDK::TArray<SDK::AActor*>& Actors = World->PersistentLevel->Actors;

        for (int i = 0; i < Actors.Num(); i++) {
            SDK::AActor* TargetActor = Actors[i];

            // 基础检查
            if (!TargetActor || TargetActor == LocalPC->Pawn) continue;

            bool bIsPlayer = TargetActor->IsA(SDK::AChar_Parent_Player_C::StaticClass());
            // 转换为角色基类
            SDK::AChar_Parent_All_C* BaseChar = reinterpret_cast<SDK::AChar_Parent_All_C*>(TargetActor);

            // 验证是否为有效角色类
            if (!BaseChar || !BaseChar->IsA(SDK::AChar_Parent_All_C::StaticClass())) continue;
            if (BaseChar->IsDead) continue;

            // 处理关系逻辑
            // 默认设置为 Enemy，如果能获取到 PlayerState 则判断 Team/Clan
            g_ESP::RelationType relation = g_ESP::RelationType::Enemy;
            SDK::APlayerState* TargetPS = BaseChar->PlayerState;

            if (TargetPS) {
                relation = g_ESP::GetPlayerRelation(TargetPS, LocalPS);
            }

            bool bDrawBox, bDrawHealthBar, bDrawName, bDrawSpecies, bDrawGrowth, bDrawDistance;
            float* BoxColor;
            float* NameColor;
            float* DistanceColor;

            // 配置匹配
            switch (relation) {
            case g_ESP::RelationType::Team:
                bDrawBox = g_Config::bDrawBoxTeam;
                BoxColor = g_Config::BoxColorTeam;
                bDrawHealthBar = g_Config::bDrawHealthBarTeam;
                bDrawName = g_Config::bDrawNameTeam;
                NameColor = g_Config::NameColorTeam;
                bDrawSpecies = g_Config::bDrawSpeciesTeam;
                bDrawGrowth = g_Config::bDrawGrowthTeam;
                bDrawDistance = g_Config::bDrawDistanceTeam;
                DistanceColor = g_Config::DistanceColorTeam;
                break;
            case g_ESP::RelationType::Clan:
                bDrawBox = g_Config::bDrawBoxClan;
                BoxColor = g_Config::BoxColorClan;
                bDrawHealthBar = g_Config::bDrawHealthBarClan;
                bDrawName = g_Config::bDrawNameClan;
                NameColor = g_Config::NameColorClan;
                bDrawSpecies = g_Config::bDrawSpeciesClan;
                bDrawGrowth = g_Config::bDrawGrowthClan;
                bDrawDistance = g_Config::bDrawDistanceClan;
                DistanceColor = g_Config::DistanceColorClan;
                break;
            default: // Enemy 逻辑
                bDrawBox = g_Config::bDrawBox;
                BoxColor = g_Config::BoxColor;
                bDrawHealthBar = g_Config::bDrawHealthBar;
                bDrawName = g_Config::bDrawName;
                NameColor = g_Config::NameColor;
                bDrawSpecies = g_Config::bDrawSpecies;
                bDrawGrowth = g_Config::bDrawGrowth;
                bDrawDistance = g_Config::bDrawDistance;
                DistanceColor = g_Config::DistanceColor;
                break;
            }

            SDK::FVector2D screenPos;
            bool bProjected = SDK::UGameplayStatics::ProjectWorldToScreen(LocalPC, TargetActor->K2_GetActorLocation(), &screenPos, false);
            bool bInScreenBounds = (screenPos.X > 0 && screenPos.X < screenW&& screenPos.Y > 0 && screenPos.Y < screenH);

            if (bProjected && bInScreenBounds) {
                g_ESP::FlagManager flagMgr;

                g_ESP::BoxRect rect = g_ESP::DrawBox(TargetActor,
                    BoxColor[0] * 255, BoxColor[1] * 255,
                    BoxColor[2] * 255, BoxColor[3] * 255, 0.5f, !bDrawBox);

                if (rect.valid) {
                    if (bDrawHealthBar) {
                        g_ESP::DrawHealthBar(rect, (float)BaseChar->HealthPercent, 200.0f, 255.0f);

                        std::string hpStr = std::to_string((int)BaseChar->HealthPercent);
                        if ((int)BaseChar->HealthPercent <= 0) {
                            hpStr = "Dead";
                        }

                        float hpPercent = BaseChar->HealthPercent / 200.0f;
                        ImVec4 hpColor;
                        if (hpPercent > 0.5f)
                            hpColor = ImVec4((1.0f - hpPercent) * 2.0f, 1.0f, 0.0f, 1.0f);
                        else
                            hpColor = ImVec4(1.0f, hpPercent * 2.0f, 0.0f, 1.0f);
                        flagMgr.AddFlag(rect, hpStr, ImGui::ColorConvertFloat4ToU32(hpColor), g_ESP::FlagPos::Left);
                    }

                    if (bDrawName) {
                        g_ESP::DrawName(TargetActor, rect,
                            NameColor[0] * 255, NameColor[1] * 255,
                            NameColor[2] * 255, NameColor[3] * 255);
                    }

                    if (bDrawSpecies && bIsPlayer) {
                        DisplayInfo info = GetSpeciesInfo(BaseChar);
                        flagMgr.AddFlag(rect, info.name, info.color, g_ESP::FlagPos::Right);
                    }

                    if (bDrawGrowth && bIsPlayer) {
                        DisplayInfo info = GetGrowthStageInfo(BaseChar);
                        flagMgr.AddFlag(rect, info.name, info.color, g_ESP::FlagPos::Right);
                    }

                    if (bDrawDistance) {
                        float distance = (LocalPC && LocalPC->Pawn && TargetActor) ? (LocalPC->Pawn->GetDistanceTo(TargetActor) / 100.0f) : 0.0f;
                        flagMgr.AddFlag(rect, std::to_string((int)distance) + "m", g_Config::GetU32Color(DistanceColor), g_ESP::FlagPos::Right);
                    }
                }
            }
            else if (g_Config::bEnableOOF) {
                std::vector<g_ESP::OOFFlag> oofFlags;

                if (bDrawName) {
                    SDK::FString fName = L"AI生物";
                    if (TargetActor->IsA(SDK::AChar_Parent_Player_C::StaticClass())) {
                        SDK::AChar_Parent_Player_C* PlayerChar = reinterpret_cast<SDK::AChar_Parent_Player_C*>(TargetActor);
                        if (PlayerChar && PlayerChar->PlayerName.IsValid()) {
                            fName = PlayerChar->PlayerName;
                        }
                    }
                    else {
                        switch (BaseChar->BiologicalSpecies) {
                        case SDK::Enum_Species::NewEnumerator11: fName = L"小龙虾"; break;
                        case SDK::Enum_Species::NewEnumerator12: fName = L"螃蟹"; break;
                        case SDK::Enum_Species::NewEnumerator17: fName = L"鳄鱼"; break;
                        }
                    }
                    oofFlags.push_back({ fName.ToString(), g_Config::GetU32Color(NameColor) });
                }

                if (bDrawHealthBar) {
                    std::string hpStr = std::to_string((int)BaseChar->HealthPercent);
                    float hpPercent = BaseChar->HealthPercent / 200.0f;
                    ImVec4 hpColor;
                    if (hpPercent > 0.5f)
                        hpColor = ImVec4((1.0f - hpPercent) * 2.0f, 1.0f, 0.0f, 1.0f);
                    else
                        hpColor = ImVec4(1.0f, hpPercent * 2.0f, 0.0f, 1.0f);
                    oofFlags.push_back({ hpStr, ImGui::ColorConvertFloat4ToU32(hpColor) });
                }

                if (bDrawSpecies && bIsPlayer) {
                    DisplayInfo info = GetSpeciesInfo(BaseChar);
                    oofFlags.push_back({ info.name, info.color });
                }

                if (bDrawGrowth && bIsPlayer) {
                    DisplayInfo info = GetGrowthStageInfo(BaseChar);
                    oofFlags.push_back({ info.name, info.color });
                }

                if (bDrawDistance) {
                    float distance = (LocalPC && LocalPC->Pawn && TargetActor) ? (LocalPC->Pawn->GetDistanceTo(TargetActor) / 100.0f) : 0.0f;
                    oofFlags.push_back({ std::to_string((int)distance) + "m", g_Config::GetU32Color(DistanceColor) });
                }

                g_ESP::DrawOutOfFOV(TargetActor, LocalPC, oofFlags);
            }
        }
    }
}