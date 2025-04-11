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
            QueryPerformanceCounter(&currentTime); // �޸�����
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
            float yaw = g_cheatdata->ClientViewAngles.y; // ��ȡ yaw ���ڼ�ͷ�ͷ�����
            float yawRad = (270.0f - yaw) * 3.1415926535f / 180.0f; // ��ת������ 270�� Ϊ��
            if (yawRad < 0.0f) yawRad += 2 * 3.1415926535f;

            // �״�����
            float centerX = static_cast<float>(m_radarWidth) / 2.0f;
            float centerY = static_cast<float>(m_radarWidth) / 2.0f;
            float radius = static_cast<float>(m_radarWidth) / 2.0f - 15.0f; // �߽�뾶�������ռ����ĸ

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

            // ��̬���ƶ������������� yaw ��ת
            Gdiplus::Color textColor(255, 255, 255); // ��ɫ
            const float PI = 3.1415926535f;

            // �� (0�� ����� yawRad)
            float northX = centerX + radius * sinf(yawRad);
            float northY = centerY - radius * cosf(yawRad);
            m_renderer.DrawText(L"N", northX - 5.0f, northY - 5.0f, textColor);

            // �� (90�� ����� yawRad)
            float eastX = centerX + radius * sinf(yawRad + PI / 2.0f);
            float eastY = centerY - radius * cosf(yawRad + PI / 2.0f);
            m_renderer.DrawText(L"E", eastX - 5.0f, eastY - 5.0f, textColor);

            // �� (180�� ����� yawRad)
            float southX = centerX + radius * sinf(yawRad + PI);
            float southY = centerY - radius * cosf(yawRad + PI);
            m_renderer.DrawText(L"S", southX - 5.0f, southY - 5.0f, textColor);

            // �� (270�� ����� yawRad)
            float westX = centerX + radius * sinf(yawRad + 3.0f * PI / 2.0f);
            float westY = centerY - radius * cosf(yawRad + 3.0f * PI / 2.0f);
            m_renderer.DrawText(L"W", westX - 5.0f, westY - 5.0f, textColor);
        }

        m_renderer.EndRender();
    }

    void RadarManager::Update(float deltaTime) {
        // Ŀǰû�е����߼�������Ҫ����չ
    }

    RadarManager::RadarEntity RadarManager::CreateLocalPlayerEntity(LocalPlayer::Player* localPlayer) const {
        return RadarEntity{
            Gdiplus::PointF(static_cast<float>(m_radarWidth) / 2.0f, static_cast<float>(m_radarWidth) / 2.0f),
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

        DWORD64 client_view_angles = Hook::GetClientViewAnglesAddress();
        g_cheatdata->ClientViewAngles = Vector::GetVec3(client_view_angles);

        std::vector<RadarEntity> newEntities;
        newEntities.reserve(static_cast<size_t>(EntityList::GetMaxPlayers()) + 1);

        newEntities.push_back(CreateLocalPlayerEntity(localPlayer));

        float scale = static_cast<float>(m_radarWidth) / (2.0f * g_cheatdata->RenderDistance);
        const float entitySize = 8.0f;
        float yaw = g_cheatdata->ClientViewAngles.y;

        size_t maxPlayers = EntityList::GetMaxPlayers();
        for (size_t i = 1; i <= maxPlayers; i++) {
            auto player = EntityList::GetPlayer(static_cast<int>(i));
            if (!Radar::IsEntityValid(player)) {
                continue;
            }
            /*
            if (Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"Entity (%d) | HP: %d | %s | %s | %s\n",
                    i,
                    player->Health,
                    player->GetCharacter(),
                    player->GetGrowthStage(),
                    player->GetGender()
                );
            }
            */

            Vector::Vec3 enemyPos = player->GetPos();
            RadarEntity radarEntity = RadarManager::ComputeRadarEntity(player, localPos, scale, m_radarWidth, yaw);
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

    RadarManager::RadarEntity RadarManager::ComputeRadarEntity(const EntityList::Player* player, const Vector::Vec3& localPos, float scale, int radarWidth, float yaw) {
        Vector::Vec3 enemyPos = player->GetPos();
        float dx = enemyPos.x - localPos.x;
        float dy = enemyPos.y - localPos.y;

        float adjustedYaw = yaw - 270.0f;
        if (adjustedYaw < 0.0f) adjustedYaw += 360.0f;
        float yawRad = adjustedYaw * 3.1415926535f / 180.0f;

        float rotatedX = dx * cosf(yawRad) + dy * sinf(yawRad);
        float rotatedY = -dx * sinf(yawRad) + dy * cosf(yawRad);

        BYTE health = static_cast<BYTE>(player->Health);
        float healthFactor = static_cast<float>(health) / g_cheatdata->MaxHealth;
        healthFactor = std::clamp(healthFactor, 0.0f, 1.0f);
        BYTE red = static_cast<BYTE>(std::round(255 * (1.0f - healthFactor)));
        BYTE green = static_cast<BYTE>(std::round(255 * healthFactor));
        Gdiplus::Color color(red, green, 0);

        Gdiplus::PointF radarPos(static_cast<float>(radarWidth) / 2.0f + rotatedX * scale, static_cast<float>(radarWidth) / 2.0f + rotatedY * scale);

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