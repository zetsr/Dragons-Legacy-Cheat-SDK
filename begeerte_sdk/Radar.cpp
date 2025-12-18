#include "Radar.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace Radar {

    RadarManager::RadarManager() : m_localPlayerValid(false) {
        m_entities.reserve(64);
    }

    void RadarManager::DrawRadar() {
        if (!m_localPlayerValid) return;

        std::lock_guard<std::mutex> lock(m_entitiesMutex);
        ImDrawList* drawList = ImGui::GetForegroundDrawList();

        ImVec2 basePos = ImVec2(m_radarX, m_radarY);
        float centerX = basePos.x + m_radarWidth / 2.0f;
        float centerY = basePos.y + m_radarWidth / 2.0f;
        float radius = m_radarWidth / 2.0f - 15.0f;

        drawList->AddRectFilled(basePos, ImVec2(basePos.x + m_radarWidth, basePos.y + m_radarWidth), IM_COL32(10, 10, 10, 180));
        drawList->AddRect(basePos, ImVec2(basePos.x + m_radarWidth, basePos.y + m_radarWidth), IM_COL32(255, 255, 255, 50));

        const float entitySize = 5.0f;
        for (const auto& entity : m_entities) {
            ImVec2 screenPos = ImVec2(basePos.x + entity.position.x, basePos.y + entity.position.y);

            if (entity.character == L"FS") {
                DrawPentagonStar(drawList, screenPos, entitySize, entity.color);
            }
            else if (entity.character == L"SS") {
                DrawDiamond(drawList, screenPos, entitySize, entity.color);
            }
            else if (entity.character == L"ASD") {
                DrawTriangle(drawList, screenPos, entitySize, entity.color);
            }
            else {
                DrawDiamond(drawList, screenPos, entitySize, entity.color);
            }
        }

        float yaw = g_cheatdata->ClientViewAngles.y;
        float yawRad = (270.0f - yaw) * (float)M_PI / 180.0f;

        auto drawDirText = [&](const char* text, float angleOffset) {
            float finalRad = yawRad + angleOffset;
            float tx = centerX + radius * sinf(finalRad);
            float ty = centerY - radius * cosf(finalRad);
            ImVec2 textSize = ImGui::CalcTextSize(text);
            drawList->AddText(ImVec2(tx - textSize.x / 2, ty - textSize.y / 2), IM_COL32(255, 255, 255, 255), text);
        };

        drawDirText("N", 0);
        drawDirText("E", (float)M_PI / 2.0f);
        drawDirText("S", (float)M_PI);
        drawDirText("W", 3.0f * (float)M_PI / 2.0f);
    }

    void RadarManager::UpdateRadar(const Vector::Vec3& cameraPos, LocalPlayer::Player* localPlayer) {
        if (cameraPos.IsZero() || !localPlayer || !localPlayer->IsValid() || !localPlayer->IsAlive()) {
            m_localPlayerValid = false;
            return;
        }

        m_localPlayerValid = true;
        Vector::Vec3 localPos = Vector::CreateVec3(localPlayer->fsPositionX, localPlayer->fsPositionY, localPlayer->fsPositionZ);

        DWORD64 client_view_angles = Hook::GetClientViewAnglesAddress();
        g_cheatdata->ClientViewAngles = Vector::GetVec3(client_view_angles);

        std::vector<RadarEntity> newEntities;
        float renderDist = (g_cheatdata->RenderDistance > 0) ? g_cheatdata->RenderDistance : 1000.0f;
        float scale = m_radarWidth / (2.0f * renderDist);
        float yaw = g_cheatdata->ClientViewAngles.y;

        newEntities.push_back({ ImVec2(m_radarWidth / 2.0f, m_radarWidth / 2.0f), IM_COL32(0, 120, 255, 255), L"ME" });

        for (size_t i = 1; i <= EntityList::GetMaxPlayers(); i++) {
            auto player = EntityList::GetPlayer((int)i);
            if (!IsEntityValid(player)) continue;

            RadarEntity re = ComputeRadarEntity(player, localPos, scale, m_radarWidth, yaw);

            if (re.position.x >= 0 && re.position.x <= m_radarWidth && re.position.y >= 0 && re.position.y <= m_radarWidth) {
                newEntities.push_back(re);
            }
        }

        std::lock_guard<std::mutex> lock(m_entitiesMutex);
        m_entities = std::move(newEntities);
    }

    RadarEntity RadarManager::ComputeRadarEntity(const EntityList::Player* player, const Vector::Vec3& localPos, float scale, float radarWidth, float yaw) {
        Vector::Vec3 enemyPos = player->GetPos();
        float dx = enemyPos.x - localPos.x;
        float dy = enemyPos.y - localPos.y;

        float yawRad = (yaw - 270.0f) * (float)M_PI / 180.0f;
        float rotX = dx * cosf(yawRad) + dy * sinf(yawRad);
        float rotY = -dx * sinf(yawRad) + dy * cosf(yawRad);

        float maxHp = (g_cheatdata->MaxHealth > 0) ? (float)g_cheatdata->MaxHealth : 100.0f;
        float healthFactor = std::clamp((float)player->Health / maxHp, 0.0f, 1.0f);
        ImU32 color = IM_COL32((int)(255 * (1.0f - healthFactor)), (int)(255 * healthFactor), 0, 255);

        return { ImVec2(radarWidth / 2.0f + rotX * scale, radarWidth / 2.0f + rotY * scale), color, player->GetCharacter() };
    }

    void RadarManager::DrawPentagonStar(ImDrawList* drawList, ImVec2 center, float size, ImU32 color) {
        ImVec2 pts[5];
        for (int i = 0; i < 5; i++) {
            float angle = (i * 72.0f - 90.0f) * (float)M_PI / 180.0f;
            pts[i] = ImVec2(center.x + cosf(angle) * size, center.y + sinf(angle) * size);
        }
        drawList->AddConvexPolyFilled(pts, 5, color);
    }

    void RadarManager::DrawDiamond(ImDrawList* drawList, ImVec2 center, float size, ImU32 color) {
        ImVec2 pts[4] = {
            ImVec2(center.x, center.y - size), ImVec2(center.x + size, center.y),
            ImVec2(center.x, center.y + size), ImVec2(center.x - size, center.y)
        };
        drawList->AddConvexPolyFilled(pts, 4, color);
    }

    void RadarManager::DrawTriangle(ImDrawList* drawList, ImVec2 center, float size, ImU32 color) {
        drawList->AddTriangleFilled(
            ImVec2(center.x, center.y - size),
            ImVec2(center.x + size, center.y + size * 0.5f),
            ImVec2(center.x - size, center.y + size * 0.5f),
            color
        );
    }

    bool IsEntityValid(const EntityList::Player* player) {
        if (!player || !player->IsValid() || !player->IsAlive() || !player->IsValidPos()) return false;
        return true;
    }
}