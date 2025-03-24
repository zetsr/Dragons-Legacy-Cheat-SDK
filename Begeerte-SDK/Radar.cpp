#include "Radar.h"
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
            const float size = 8.0f;
            for (const auto& entity : m_entities) {
                if (entity.character == L"FS") {
                    DrawPentagonStar(m_renderer, entity.position, size, entity.color);
                }
                else if (entity.character == L"SS") {
                    DrawDiamond(m_renderer, entity.position, size, entity.color);
                }
                else if (entity.character == L"ASD") {
                    DrawTriangle(m_renderer, entity.position, size, entity.color);
                }
                else {
                    DrawDiamond(m_renderer, entity.position, size, entity.color);
                }
            }
        }

        m_renderer.EndRender();
    }

    void RadarManager::Update(float deltaTime) {
        // 目前没有淡出逻辑，若需要可扩展
    }

    RadarManager::RadarEntity RadarManager::CreateLocalPlayerEntity(LocalPlayer::Player* localPlayer) const {
        return RadarEntity{
            Gdiplus::PointF(m_radarWidth / 2.0f, m_radarWidth / 2.0f),
            200,
            true,
            0.0f,
            Gdiplus::Color(0, 0, 255),
            localPlayer->GetCharacter()
        };
    }

    void RadarManager::UpdateRadar(const Vector::Vec3& cameraPos, LocalPlayer::Player* localPlayer) {
        std::lock_guard<std::mutex> lock(m_entitiesMutex);

        Vector::Vec3 localPos;
        if (!Radar::CheckRadarPrerequisites(m_renderer, cameraPos, localPlayer, m_localPlayerValid, localPos)) {
            m_entities.clear();
            return;
        }

        std::vector<RadarEntity> newEntities;
        newEntities.reserve(static_cast<size_t>(EntityList::GetMaxPlayers()) + 1);

        newEntities.push_back(CreateLocalPlayerEntity(localPlayer));

        float scale = m_radarWidth / (2.0f * g_cheatdata->RenderDistance);
        const float entitySize = 8.0f;

        size_t maxPlayers = EntityList::GetMaxPlayers();
        for (size_t i = 1; i <= maxPlayers; i++) {
            auto player = EntityList::GetPlayer(static_cast<int>(i));
            if (!Radar::IsEntityValid(player)) {
                continue;
            }

            if (Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"Entity (%d) | HP: %d | %s | %s | %s\n",
                    i,
                    player->Health,
                    player->GetCharacter(),
                    player->GetGrowthStage(),
                    player->GetGender()
                );
            }

            Vector::Vec3 enemyPos = player->GetPos();
            RadarEntity radarEntity = RadarManager::ComputeRadarEntity(player, localPos, scale, m_radarWidth);
            radarEntity.character = player->GetCharacter();

            float left = radarEntity.position.X - entitySize;
            float right = radarEntity.position.X + entitySize;
            float top = radarEntity.position.Y - entitySize;
            float bottom = radarEntity.position.Y + entitySize;

            if (right < 0 || left > m_radarWidth || bottom < 0 || top > m_radarWidth) {
                continue;
            }

            if (Radar::IsEntityInRange(enemyPos, localPos, g_cheatdata->RenderDistance) ||
                (left < m_radarWidth && right > 0 && top < m_radarWidth && bottom > 0)) {
                newEntities.push_back(radarEntity);
            }
        }

        m_entities = std::move(newEntities);
    }

    RadarManager::RadarEntity RadarManager::ComputeRadarEntity(const EntityList::Player* player, const Vector::Vec3& localPos, float scale, int radarWidth) {
        Vector::Vec3 enemyPos = player->GetPos();
        float dx = enemyPos.x - localPos.x;
        float dy = enemyPos.y - localPos.y;

        BYTE health = static_cast<BYTE>(player->Health);
        float healthFactor = static_cast<float>(health) / g_cheatdata->MaxHealth;
        healthFactor = std::clamp(healthFactor, 0.0f, 1.0f);
        BYTE red = static_cast<BYTE>(std::round(255 * (1.0f - healthFactor)));
        BYTE green = static_cast<BYTE>(std::round(255 * healthFactor));
        Gdiplus::Color color(red, green, 0);

        Gdiplus::PointF radarPos(radarWidth / 2.0f + dx * scale, radarWidth / 2.0f + dy * scale);

        return RadarEntity{ radarPos, health, true, 0.0f, color, L"" };
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