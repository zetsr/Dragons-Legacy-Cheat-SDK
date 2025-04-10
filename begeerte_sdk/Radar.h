#pragma once
#include <windows.h>
#include <vector>
#include <mutex>
#include <algorithm>
#include <cstddef>
#include <limits>
#include "GDI.h"
#include "CheatData.h"
#include "EntityList.h"
#include "Logs.h"
#include "SDK.h"
#include "Hook.h"

namespace Radar {
    class RadarManager {
    public:
        RadarManager();
        ~RadarManager();
        void Initialize();
        void Run();
        void UpdateRadar(const Vector::Vec3& cameraPos, LocalPlayer::Player* localPlayer);
        void Render();
        void Update(float deltaTime);
        static void Thread();

        static RadarManager* s_instance;
        static HANDLE s_threadHandle;

        struct RadarEntity {
            Gdiplus::PointF position;
            BYTE health;
            bool isAlive;
            float fadeTime;
            Gdiplus::Color color;
            std::wstring character;
        };

    private:
        static DWORD WINAPI ThreadProc(LPVOID lpParam);

        RadarEntity CreateLocalPlayerEntity(LocalPlayer::Player* localPlayer) const;

        GDI::Renderer m_renderer;
        std::vector<RadarEntity> m_entities;
        std::mutex m_entitiesMutex;
        bool m_localPlayerValid;
        const int m_radarWidth = 200;

        static RadarEntity ComputeRadarEntity(const EntityList::Player* player, const Vector::Vec3& localPos, float scale, int radarWidth, float yaw);

        // 形状绘制函数实现（内联）
        inline void DrawPentagonStar(GDI::Renderer& renderer, const Gdiplus::PointF& position, float size, const Gdiplus::Color& color) const {
            Gdiplus::PointF points[5] = {
                Gdiplus::PointF(position.X, position.Y - size),                     // 顶点
                Gdiplus::PointF(position.X + size * 0.951f, position.Y - size * 0.309f), // 右上
                Gdiplus::PointF(position.X + size * 0.588f, position.Y + size * 0.809f), // 右下
                Gdiplus::PointF(position.X - size * 0.588f, position.Y + size * 0.809f), // 左下
                Gdiplus::PointF(position.X - size * 0.951f, position.Y - size * 0.309f)  // 左上
            };
            renderer.DrawPolygon(points, 5, color);
        }

        inline void DrawDiamond(GDI::Renderer& renderer, const Gdiplus::PointF& position, float size, const Gdiplus::Color& color) const {
            Gdiplus::PointF points[4] = {
                Gdiplus::PointF(position.X, position.Y - size),
                Gdiplus::PointF(position.X + size, position.Y),
                Gdiplus::PointF(position.X, position.Y + size),
                Gdiplus::PointF(position.X - size, position.Y)
            };
            renderer.DrawPolygon(points, 4, color);
        }

        inline void DrawTriangle(GDI::Renderer& renderer, const Gdiplus::PointF& position, float size, const Gdiplus::Color& color) const {
            Gdiplus::PointF points[3] = {
                Gdiplus::PointF(position.X, position.Y - size),
                Gdiplus::PointF(position.X + size, position.Y + size * 0.5f),
                Gdiplus::PointF(position.X - size, position.Y + size * 0.5f)
            };
            renderer.DrawPolygon(points, 3, color);
        }

        inline void DrawArrow(GDI::Renderer& renderer, const Gdiplus::PointF& position, float size, float yaw, const Gdiplus::Color& color) const {
            float yawRad = (yaw - 270.0f) * 3.1415926535f / 180.0f; // 调整 yaw 以 270° 为正北
            if (yawRad < 0.0f) yawRad += 2 * 3.1415926535f;

            Gdiplus::PointF points[3] = {
                Gdiplus::PointF(position.X, position.Y - size),              // 顶部
                Gdiplus::PointF(position.X - size * 0.5f, position.Y + size * 0.5f), // 左下
                Gdiplus::PointF(position.X + size * 0.5f, position.Y + size * 0.5f)  // 右下
            };

            for (int i = 0; i < 3; i++) {
                float dx = points[i].X - position.X;
                float dy = points[i].Y - position.Y;
                points[i].X = position.X + dx * cosf(yawRad) + dy * sinf(yawRad);
                points[i].Y = position.Y - dx * sinf(yawRad) + dy * cosf(yawRad);
            }

            renderer.DrawPolygon(points, 3, color);
        }
    };

    inline bool IsEntityValid(const EntityList::Player* player) {
        if (!player) return false;
        if (!player->IsValid()) return false;
        if (player->Character == SDK::Enum_PlayerCharacter::ThornbackCrawler) return false;
        if (player->Character == SDK::Enum_PlayerCharacter::BulbousSpinedScrab) return false;
        if (player->Character == SDK::Enum_PlayerCharacter::InfernoRavager) return false;
        if (player->Character == SDK::Enum_PlayerCharacter::BlitzStriker) return false;
        if (player->Character == SDK::Enum_PlayerCharacter::Biolumin) return false;
        if (player->GetCharacter() == L"Unknown") return false;
        if (!player->IsAlive()) return false;
        if (!player->IsValidPos()) return false;
        return true;
    }

    inline bool CheckRadarPrerequisites(const GDI::Renderer& renderer, const Vector::Vec3& cameraPos, LocalPlayer::Player* localPlayer, bool& localPlayerValid, Vector::Vec3& outLocalPos) {
        if (!renderer.GetDeviceContext()) {
            if (Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"Radar HDC or MemDC invalid.\n");
            }
            return false;
        }

        if (cameraPos.IsZero()) {
            if (Logs::LogManager::s_instance) {
                // Logs::LogManager::s_instance->Printf(L"No Camera Position data in Radar.\n");
            }
            return false;
        }

        if (!localPlayer || !localPlayer->IsValid() || !localPlayer->IsAlive()) {
            if (localPlayerValid && Logs::LogManager::s_instance) {
                Logs::LogManager::s_instance->Printf(L"No player data or player invalid in Radar, clearing entities.\n");
            }
            localPlayerValid = false;
            return false;
        }

        localPlayerValid = true;
        outLocalPos = Vector::CreateVec3(localPlayer->fsPositionX, localPlayer->fsPositionY, localPlayer->fsPositionZ);
        return true;
    }

    inline bool IsEntityInRange(const Vector::Vec3& entityPos, const Vector::Vec3& localPos, float renderDistance) {
        float dx = entityPos.x - localPos.x;
        float dy = entityPos.y - localPos.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        return distance <= renderDistance;
    }
}

inline Radar::RadarManager* Radar::RadarManager::s_instance = nullptr;
inline HANDLE Radar::RadarManager::s_threadHandle = nullptr;