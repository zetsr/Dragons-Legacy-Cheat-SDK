#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <imgui.h>

namespace Logs {
    struct LogEntry {
        std::wstring text;
        float lifetime;
    };

    class LogManager {
    public:
        LogManager() = default;
        ~LogManager() = default;

        // 核心绘制函数：在 ImGui 渲染循环中调用
        void DrawLogs();

        // 逻辑更新函数：处理日志淡出/消失
        void Update(float deltaTime);

        // 格式化输入日志
        void Printf(const wchar_t* format, ...);

        static LogManager* GetInstance() {
            static LogManager instance;
            return &instance;
        }

    private:
        std::vector<LogEntry> m_logs;
        std::mutex m_logsMutex;

        const int m_maxLogs = 10;
        const float m_logLifetime = 7.0f;
        const float m_offsetX = 205.0f; // 对应原代码 m_radarWidth + m_offsetX
        const float m_offsetY = 3.0f;
        const float m_lineHeight = 20.0f;

        // 辅助工具：将 wstring 转为 ImGui 需要的 UTF-8 string
        std::string WStringToString(const std::wstring& wstr);
    };
}