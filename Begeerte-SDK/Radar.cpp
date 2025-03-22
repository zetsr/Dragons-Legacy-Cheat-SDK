#include "Radar.h"
#include <algorithm>
#include <cmath>

namespace Radar {
    RadarManager::RadarManager() : m_localPlayerValid(false) {}

    RadarManager::~RadarManager() {
        if (s_threadHandle) {
            WaitForSingleObject(s_threadHandle, INFINITE);
            CloseHandle(s_threadHandle);
            s_threadHandle = nullptr;
        }
        s_instance = nullptr;
    }

    void RadarManager::Initialize() {
        m_renderer.Initialize();
    }

    void RadarManager::Run() {
        LARGE_INTEGER frequency, lastTime;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&lastTime);

        MSG msg = { 0 };
        while (true) {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) break;
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);
            float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
            lastTime = currentTime;

            Update(deltaTime);
            Render();
        }
    }

    void RadarManager::Render() {
        m_renderer.BeginRender();

        std::lock_guard<std::mutex> lock(m_entitiesMutex);
        if (m_localPlayerValid && !m_entities.empty()) {
            for (const auto& entity : m_entities) {
                Gdiplus::PointF points[4] = {
                    Gdiplus::PointF(entity.position.X, entity.position.Y - 10.0f),
                    Gdiplus::PointF(entity.position.X + 10.0f, entity.position.Y),
                    Gdiplus::PointF(entity.position.X, entity.position.Y + 10.0f),
                    Gdiplus::PointF(entity.position.X - 10.0f, entity.position.Y)
                };
                m_renderer.DrawPolygon(points, 4, entity.color);
            }
        }

        m_renderer.EndRender();
    }

    void RadarManager::Update(float deltaTime) {
        // 目前没有淡出逻辑，若需要可扩展
    }

    void RadarManager::UpdateRadar(const Vector::Vec3& cameraPos, LocalPlayer::Player* localPlayer) {
        if (!m_renderer.GetDeviceContext()) {
            if (Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"Radar HDC or MemDC invalid.\n");
            }
            return;
        }

        if (cameraPos.IsZero()) {
            if (Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"No Camera Position data in Radar.\n");
            }
            return;
        }

        std::lock_guard<std::mutex> lock(m_entitiesMutex);

        if (!localPlayer || !localPlayer->IsValid() || !localPlayer->IsAlive()) {
            if (m_localPlayerValid && Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"No player data or player invalid in Radar, clearing entities.\n");
            }
            m_entities.clear();
            m_localPlayerValid = false;
            return;
        }

        // 本地玩家有效，更新实体
        m_localPlayerValid = true;
        Vector::Vec3 localPos = Vector::CreateVec3(localPlayer->fsPositionX, localPlayer->fsPositionY, localPlayer->fsPositionZ);

        m_entities.clear();
        m_entities.reserve(static_cast<size_t>(EntityList::GetMaxPlayers()) + 1);

        RadarEntity localEntity = { Gdiplus::PointF(m_radarWidth / 2.0f, m_radarWidth / 2.0f), 200, true, 0.0f, Gdiplus::Color(0, 0, 255) };
        m_entities.push_back(localEntity);

        float scale = m_radarWidth / g_cheatdata->RenderDistance;
        size_t maxPlayers = EntityList::GetMaxPlayers();

        auto Age_Name = L"Unknown";

        for (size_t i = 1; i <= maxPlayers; i++) {
            auto player = EntityList::GetPlayer(static_cast<int>(i));
            if (!player || !player->IsValid() || !player->IsAlive() || !player->IsValidPos()) continue;

            switch (player->GrowthStage) {
            case SDK::Enum_GrowthStage::Hatchling:
                Age_Name = L"Hatchling";
                break;
            case SDK::Enum_GrowthStage::Juvenile:
                Age_Name = L"Juvenile";
                break;
            case SDK::Enum_GrowthStage::Adult:
                Age_Name = L"Adult";
                break;
            case SDK::Enum_GrowthStage::Elder:
                Age_Name = L"Elder";
                break;
            case SDK::Enum_GrowthStage::Ancient:
                Age_Name = L"Ancient";
                break;
            default:
                Age_Name = L"Unknown";
                break;
            }

            if (Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"Entity (%d) | HP: %d | Age: %s\n",
                    i,
                    player->Health,
                    Age_Name
                );
            }

            /*
            if (Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"Entity (%d) | HP: %d | GrowthStage: %d | SavedGrowth: %d | Pos: (%.1f, %.1f, %.1f) | Distance: %.1f\n",
                    i,
                    player->Health,
                    player->GrowthStage,
                    player->SavedGrowth,
                    player->GetPos().x,
                    player->GetPos().y,
                    player->GetPos().z,
                    Vector::CreateVec3(player->GetPos().x, player->GetPos().y, player->GetPos().z).Distance(
                        Vector::CreateVec3(localPlayer->fsPositionX, localPlayer->fsPositionY, localPlayer->fsPositionZ)
                    ));
            }
            */

            Vector::Vec3 enemyPos = player->GetPos();
            BYTE health = player->Health;
            float healthFactor = static_cast<float>(health) / g_cheatdata->MaxHealth;
            healthFactor = std::clamp(healthFactor, 0.0f, 1.0f);

            BYTE red = static_cast<BYTE>(std::round(255 * (1.0f - healthFactor)));
            BYTE green = static_cast<BYTE>(std::round(255 * healthFactor));
            Gdiplus::Color color(red, green, 0);

            float dx = enemyPos.x - localPos.x;
            float dy = enemyPos.y - localPos.y;
            Gdiplus::PointF radarPos(m_radarWidth / 2.0f + dx * scale, m_radarWidth / 2.0f + dy * scale);

            radarPos.X = max(0.0f, min(static_cast<float>(m_radarWidth), radarPos.X));
            radarPos.Y = max(0.0f, min(static_cast<float>(m_radarWidth), radarPos.Y));

            RadarEntity enemy = { radarPos, health, true, 0.0f, color };
            m_entities.push_back(enemy);
        }
    }

    void RadarManager::Thread() {
        if (s_instance != nullptr) {
            return;
        }

        s_instance = new RadarManager();
        s_threadHandle = CreateThread(nullptr, 0, ThreadProc, s_instance, 0, nullptr);
        if (s_threadHandle == nullptr) {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    DWORD WINAPI RadarManager::ThreadProc(LPVOID lpParam) {
        RadarManager* instance = static_cast<RadarManager*>(lpParam);
        instance->Initialize();
        instance->Run();
        return 0;
    }
}