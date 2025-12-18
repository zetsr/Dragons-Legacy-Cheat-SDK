#pragma once
#include <vector>
#include <mutex>
#include <string>
#include <algorithm>
#include <imgui.h>
#include "CheatData.h"
#include "EntityList.h"
#include "Logs.h"
#include "SDK.h"
#include "Hook.h"

namespace Radar {
    struct RadarEntity {
        ImVec2 position;
        ImU32 color;
        std::wstring character;
    };

    class RadarManager {
    public:
        RadarManager();
        ~RadarManager() = default;

        // 获取单例实例
        static RadarManager* GetInstance() {
            static RadarManager instance;
            return &instance;
        }

        // 核心渲染函数
        void DrawRadar();

        // 数据更新函数
        void UpdateRadar(const Vector::Vec3& cameraPos, LocalPlayer::Player* localPlayer);

    private:
        std::vector<RadarEntity> m_entities;
        std::mutex m_entitiesMutex;
        bool m_localPlayerValid;

        // 雷达配置
        const float m_radarWidth = 200.0f;
        const float m_radarX = 0.0f;
        const float m_radarY = 0.0f;

        // 内部形状绘制
        void DrawPentagonStar(ImDrawList* drawList, ImVec2 center, float size, ImU32 color);
        void DrawDiamond(ImDrawList* drawList, ImVec2 center, float size, ImU32 color);
        void DrawTriangle(ImDrawList* drawList, ImVec2 center, float size, ImU32 color);

        static RadarEntity ComputeRadarEntity(const EntityList::Player* player, const Vector::Vec3& localPos, float scale, float radarWidth, float yaw);
    };

    bool IsEntityValid(const EntityList::Player* player);
}