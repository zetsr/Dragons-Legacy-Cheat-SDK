#pragma once

#include "../Minimal-D3D12-Hook-ImGui-1.0.2/Main/mdx12_api.h"
#include "SDK_Headers.hpp"

namespace g_DrawESP {
    struct DisplayInfo {
        std::string name;
        ImU32 color;
    };

    DisplayInfo GetSpeciesInfo(SDK::AChar_Parent_All_C* BaseChar) {
        SDK::FString fName = L"Î´ÖªÎïÖÖ";
        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        switch (BaseChar->BiologicalSpecies) {
        case SDK::Enum_Species::NewEnumerator1: fName = L"Öð»ðÁú"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator2: fName = L"ÂÓÓüÁú"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 1.0f, 1.0f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator3: fName = L"·üÓ°Áú"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 0.4f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator4: fName = L"ËáÏÑÁú"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.6f, 1.0f, 0.4f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator6: fName = L"ÕðÀ×Áú"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.4f, 0.7f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator14: fName = L"¹Ë³ûÁú"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 1.0f, 1.0f)); break;
        case SDK::Enum_Species::NewEnumerator16: fName = L"Ó©ÁéÁú"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 1.0f, 1.0f)); break;
        default: fName = SDK::FString(std::to_wstring((unsigned char)BaseChar->BiologicalSpecies).c_str()); break;
        }
        return { fName.ToString(), color };
    }

    DisplayInfo GetGrowthStageInfo(SDK::AChar_Parent_All_C* BaseChar) {
        SDK::FString fName = L"Î´ÖªÄêÁä";
        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        switch (BaseChar->GrowthStage) {
        case SDK::Enum_GrowthStage::NewEnumerator1: fName = L"Ó×Äê"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); break;
        case SDK::Enum_GrowthStage::NewEnumerator2: fName = L"ÇàÄê"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.5f, 1.0f)); break;
        case SDK::Enum_GrowthStage::NewEnumerator3: fName = L"³ÉÄê"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.6f, 1.0f, 1.0f)); break;
        case SDK::Enum_GrowthStage::NewEnumerator4: fName = L"³¤Õß"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); break;
        case SDK::Enum_GrowthStage::NewEnumerator5: fName = L"Ô¶¹Å"; color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 1.0f, 1.0f)); break;
        }
        return { fName.ToString(), color };
    }
}