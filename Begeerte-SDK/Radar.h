#pragma once
#include <windows.h>
#include <vector>
#include <mutex>
#include "GDI.h"
#include "CheatData.h"
#include "EntityList.h"
#include "Logs.h"
#include "SDK.h"

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

    private:
        static DWORD WINAPI ThreadProc(LPVOID lpParam);

        struct RadarEntity {
            Gdiplus::PointF position;
            BYTE health;
            bool isAlive;
            float fadeTime;
            Gdiplus::Color color;
        };

        GDI::Renderer m_renderer;
        std::vector<RadarEntity> m_entities;
        std::mutex m_entitiesMutex;
        bool m_localPlayerValid;
        const int m_radarWidth = 200;
    };
}

inline Radar::RadarManager* Radar::RadarManager::s_instance = nullptr;
inline HANDLE Radar::RadarManager::s_threadHandle = nullptr;