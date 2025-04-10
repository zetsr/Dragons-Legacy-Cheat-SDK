#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <mutex>
#include "GDI.h"
#include "CheatData.h"

namespace Logs {
    class LogManager {
    public:
        LogManager();
        ~LogManager();
        void Initialize();
        void Run();
        void Printf(const wchar_t* format, ...);
        void Render();
        void Update(float deltaTime);
        static void Thread();

        static LogManager* s_instance;
        static HANDLE s_threadHandle;

    private:
        static DWORD WINAPI ThreadProc(LPVOID lpParam);
        void UpdateLogs(float deltaTime);

        struct LogEntry {
            std::wstring text;
            float lifetime;
        };

        GDI::Renderer m_renderer;
        std::vector<LogEntry> m_logs;
        std::mutex m_logsMutex;
        const int m_maxLogs = 10;
        const float m_logLifetime = 7.0f;
        const int m_radarWidth = 200;
        const int m_offsetX = 5;
        const int m_offsetY = 5;
    };
}

inline Logs::LogManager* Logs::LogManager::s_instance = nullptr;
inline HANDLE Logs::LogManager::s_threadHandle = nullptr;