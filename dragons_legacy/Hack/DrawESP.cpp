#include "../Minimal-D3D12-Hook-ImGui-1.0.0/Main/mdx12_api.h"
#include "../CppSDK/SDK.hpp"
#include "ESP.h"
#include "Configs.h"
#include "DrawESP.h"

namespace g_DrawESP {
    void DrawESP() {
        SDK::UWorld* World = SDK::UWorld::GetWorld();
        if (!World || !World->GameState) return;

        SDK::APlayerController* LocalPC = g_ESP::GetLocalPC();
        if (!LocalPC || !LocalPC->Pawn) return;

        SDK::APlayerState* LocalPS = LocalPC->PlayerState;
        if (!LocalPS) return;

        ImGuiIO& io = ImGui::GetIO();
        float screenW = io.DisplaySize.x;
        float screenH = io.DisplaySize.y;

        SDK::TArray<SDK::APlayerState*>& PlayerArray = World->GameState->PlayerArray;

        for (int i = 0; i < PlayerArray.Num(); i++) {
            SDK::APlayerState* PS = PlayerArray[i];
            if (!PS || !PS->PawnPrivate || PS->PawnPrivate == LocalPC->Pawn) continue;

            auto TargetActor = reinterpret_cast<SDK::AActor*>(PS->PawnPrivate);
            auto BaseChar = reinterpret_cast<SDK::AChar_Parent_All_C*>(TargetActor);
            if (!BaseChar || BaseChar->IsDead) continue;

            auto PlayerChar = reinterpret_cast<SDK::AChar_Parent_Player_C*>(TargetActor);
            if (!PlayerChar) continue;

            // 确定关系类型
            g_ESP::RelationType relation = g_ESP::GetPlayerRelation(PS, LocalPS);

            // 根据关系选择配置
            bool bDrawBox, bDrawHealthBar, bDrawName, bDrawSpecies, bDrawGrowth, bDrawDistance;
            float* BoxColor;
            float* NameColor;
            float* DistanceColor;

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
            default: // Enemy
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

                // 绘制盒子 - 如果没启用，使用 bTestOnly=true 只获取信息
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

                    if (bDrawSpecies) {
                        std::string speciesName;
                        ImU32 speciesColor;
                        switch (PlayerChar->CharacterSpecies) {
                        case SDK::Enum_PlayerCharacter::NewEnumerator0: speciesName = "FS"; speciesColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); break;
                        case SDK::Enum_PlayerCharacter::NewEnumerator2: speciesName = "SS"; speciesColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 0.4f, 1.0f)); break;
                        case SDK::Enum_PlayerCharacter::NewEnumerator3: speciesName = "ASD"; speciesColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.6f, 1.0f, 0.4f, 1.0f)); break;
                        default:
                            speciesName = "Spec_" + std::to_string((unsigned char)PlayerChar->CharacterSpecies);
                            speciesColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                            break;
                        }
                        flagMgr.AddFlag(rect, speciesName, speciesColor, g_ESP::FlagPos::Right);
                    }

                    if (bDrawGrowth) {
                        std::string stageName = "Unknown";
                        ImU32 stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                        switch (BaseChar->GrowthStage) {
                        case SDK::Enum_GrowthStage::NewEnumerator1: stageName = "Hatchling"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); break;
                        case SDK::Enum_GrowthStage::NewEnumerator2: stageName = "Juvenile"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.5f, 1.0f)); break;
                        case SDK::Enum_GrowthStage::NewEnumerator3: stageName = "Adult"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.6f, 1.0f, 1.0f)); break;
                        case SDK::Enum_GrowthStage::NewEnumerator4: stageName = "Elder"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); break;
                        case SDK::Enum_GrowthStage::NewEnumerator5: stageName = "Ancient"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 1.0f, 1.0f)); break;
                        }
                        flagMgr.AddFlag(rect, stageName, stageColor, g_ESP::FlagPos::Right);
                    }

                    if (bDrawDistance) {
                        float distance = LocalPC->Pawn->GetDistanceTo(TargetActor) / 100.0f;
                        flagMgr.AddFlag(rect, std::to_string((int)distance) + "m", g_Config::GetU32Color(DistanceColor), g_ESP::FlagPos::Right);
                    }
                }
            }
            else if (g_Config::bEnableOOF) {
                std::vector<g_ESP::OOFFlag> oofFlags;

                if (bDrawName && PlayerChar->PlayerName.IsValid()) {
                    oofFlags.push_back({ PlayerChar->PlayerName.ToString(), g_Config::GetU32Color(NameColor) });
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

                if (bDrawSpecies) {
                    std::string speciesName;
                    ImU32 speciesColor;
                    switch (PlayerChar->CharacterSpecies) {
                    case SDK::Enum_PlayerCharacter::NewEnumerator0: speciesName = "FS"; speciesColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); break;
                    case SDK::Enum_PlayerCharacter::NewEnumerator2: speciesName = "SS"; speciesColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 0.4f, 1.0f)); break;
                    case SDK::Enum_PlayerCharacter::NewEnumerator3: speciesName = "ASD"; speciesColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.6f, 1.0f, 0.4f, 1.0f)); break;
                    default:
                        speciesName = "Spec_" + std::to_string((unsigned char)PlayerChar->CharacterSpecies);
                        speciesColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                        break;
                    }
                    oofFlags.push_back({ speciesName, speciesColor });
                }

                if (bDrawGrowth) {
                    std::string stageName = "Unknown";
                    ImU32 stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                    switch (BaseChar->GrowthStage) {
                    case SDK::Enum_GrowthStage::NewEnumerator1: stageName = "Hatchling"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); break;
                    case SDK::Enum_GrowthStage::NewEnumerator2: stageName = "Juvenile"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.5f, 1.0f)); break;
                    case SDK::Enum_GrowthStage::NewEnumerator3: stageName = "Adult"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.6f, 1.0f, 1.0f)); break;
                    case SDK::Enum_GrowthStage::NewEnumerator4: stageName = "Elder"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); break;
                    case SDK::Enum_GrowthStage::NewEnumerator5: stageName = "Ancient"; stageColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 1.0f, 1.0f)); break;
                    }
                    oofFlags.push_back({ stageName, stageColor });
                }

                if (bDrawDistance) {
                    float distance = LocalPC->Pawn->GetDistanceTo(TargetActor) / 100.0f;
                    oofFlags.push_back({ std::to_string((int)distance) + "m", g_Config::GetU32Color(DistanceColor) });
                }

                g_ESP::DrawOutOfFOV(TargetActor, LocalPC, oofFlags);
            }
        }
    }
}