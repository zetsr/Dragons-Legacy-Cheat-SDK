#pragma once

namespace g_Config {

    // 辅助函数：将 float[4] 转换为 ImU32
    inline ImU32 GetU32Color(float color[4]) {
        return ImGui::ColorConvertFloat4ToU32(*(ImVec4*)color);
    }

    // --- 敌人 ESP 设置 ---
    inline bool bDrawBox = true;
    inline float BoxColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline bool bDrawHealthBar = true;
    inline bool bDrawName = true;
    inline float NameColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline bool bDrawSpecies = true;
    inline bool bDrawGrowth = true;
    inline bool bDrawDistance = true;
    inline float DistanceColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // --- 队友 ESP 设置 ---
    inline bool bDrawBoxTeam = true;
    inline float BoxColorTeam[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    inline bool bDrawHealthBarTeam = true;
    inline bool bDrawNameTeam = true;
    inline float NameColorTeam[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    inline bool bDrawSpeciesTeam = true;
    inline bool bDrawGrowthTeam = true;
    inline bool bDrawDistanceTeam = true;
    inline float DistanceColorTeam[4] = { 0.0f, 0.5f, 1.0f, 1.0f };

    // --- 氏族成员 ESP 设置 ---
    inline bool bDrawBoxClan = true;
    inline float BoxColorClan[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    inline bool bDrawHealthBarClan = true;
    inline bool bDrawNameClan = true;
    inline float NameColorClan[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    inline bool bDrawSpeciesClan = true;
    inline bool bDrawGrowthClan = true;
    inline bool bDrawDistanceClan = true;
    inline float DistanceColorClan[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

    // --- OOF 设置 (共享) ---
    inline bool bEnableOOF = true;
    inline float OOFColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline float OOFRadius = 0.75f;
    inline float OOFSize = 12.0f;
    inline float OOFBreathSpeed = 2.0f;
    inline float OOFMinAlpha = 0.25f;
    inline float OOFMaxAlpha = 1.0f;
}