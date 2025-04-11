#include "Logs.h"
#include <stdio.h>
#include <stdarg.h>

namespace Logs {
    LogManager::LogManager() {}

    LogManager::~LogManager() {
        if (s_threadHandle) {
            WaitForSingleObject(s_threadHandle, INFINITE);
            CloseHandle(s_threadHandle);
            s_threadHandle = nullptr;
        }
        s_instance = nullptr;
    }

    void LogManager::Initialize() {
        m_renderer.Initialize();
    }

    void LogManager::Run() {
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

    void LogManager::Render() {
        m_renderer.BeginRender();

        if (!m_logs.empty()) {
            // Create and validate Graphics object
            Gdiplus::Graphics graphics(m_renderer.GetDeviceContext());
            if (graphics.GetLastStatus() != Gdiplus::Ok) {
                m_renderer.EndRender();
                return;  // Exit if Graphics object creation fails
            }

            // Create and validate Font object
            Gdiplus::Font font(L"Verdana", 10, Gdiplus::FontStyleBold);
            if (font.GetLastStatus() != Gdiplus::Ok) {
                m_renderer.EndRender();
                return;  // Exit if Font object creation fails
            }

            float maxWidth = 0.0f;
            for (const auto& log : m_logs) {
                if (log.text.empty()) {
                    continue;  // Skip empty strings to avoid issues
                }
                Gdiplus::RectF rect;
                Gdiplus::Status status = graphics.MeasureString(log.text.c_str(), -1, &font, Gdiplus::PointF(0, 0), &rect);
                if (status == Gdiplus::Ok && rect.Width > maxWidth) {
                    maxWidth = rect.Width;
                }
            }

            float rectX = static_cast<float>(m_radarWidth + m_offsetX - 10);
            float rectY = static_cast<float>(m_offsetY - 10);
            float rectWidth = maxWidth + 20;
            float rectHeight = static_cast<float>(m_logs.size() * 20 + 20);
            m_renderer.DrawRect(rectX, rectY, rectWidth, rectHeight, Gdiplus::Color(50, 0, 0, 0));
        }

        float drawY = static_cast<float>(m_offsetY);
        for (const auto& log : m_logs) {
            if (!log.text.empty()) {  // Only draw non-empty strings
                m_renderer.DrawText(log.text.c_str(), static_cast<float>(m_radarWidth + m_offsetX), drawY, Gdiplus::Color(255, 255, 255));
            }
            drawY += 20;
        }

        m_renderer.EndRender();
    }

    void LogManager::Update(float deltaTime) {
        UpdateLogs(deltaTime);
    }

    void LogManager::UpdateLogs(float deltaTime) {
        std::lock_guard<std::mutex> lock(m_logsMutex);
        for (auto it = m_logs.begin(); it != m_logs.end();) {
            it->lifetime += deltaTime;
            if (it->lifetime >= m_logLifetime) {
                it = m_logs.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void LogManager::Printf(const wchar_t* format, ...) {
        wchar_t buffer[256];
        va_list args;
        va_start(args, format);
        vswprintf_s(buffer, format, args);
        wprintf(L"%s", buffer);
        va_end(args);

        LogEntry entry = { buffer, 0.0f };
        std::lock_guard<std::mutex> lock(m_logsMutex);
        if (m_logs.size() >= m_maxLogs) {
            m_logs.erase(m_logs.begin());
        }
        m_logs.push_back(entry);
    }

    void LogManager::Thread() {
        if (s_instance != nullptr) {
            return;
        }

        s_instance = new LogManager();
        s_threadHandle = CreateThread(nullptr, 0, ThreadProc, s_instance, 0, nullptr);
        if (s_threadHandle == nullptr) {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    DWORD WINAPI LogManager::ThreadProc(LPVOID lpParam) {
        LogManager* instance = static_cast<LogManager*>(lpParam);
        instance->Initialize();
        instance->Run();
        return 0;
    }
}