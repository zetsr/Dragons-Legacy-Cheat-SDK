#include "Logs.h"
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>

namespace Logs {

    void LogManager::DrawLogs() {
        std::lock_guard<std::mutex> lock(m_logsMutex);
        if (m_logs.empty()) return;

        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        float currentY = m_offsetY;

        // 1. 计算背景矩形范围（模拟原有的 MeasureString 效果）
        // 这里我们可以根据行数和粗略宽度画一个简单的背景，或者直接迭代
        float maxWidth = 0.0f;
        for (const auto& log : m_logs) {
            std::string utf8Text = WStringToString(log.text);
            ImVec2 textSize = ImGui::CalcTextSize(utf8Text.c_str());
            if (textSize.x > maxWidth) maxWidth = textSize.x;
        }

        // 绘制半透明背景
        ImVec2 rectPos = ImVec2(m_offsetX - 5.0f, m_offsetY - 5.0f);
        ImVec2 rectSize = ImVec2(maxWidth + 10.0f, m_logs.size() * m_lineHeight + 0.0f);
        drawList->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y), IM_COL32(0, 0, 0, 50));

        // 2. 迭代绘制文字
        for (const auto& log : m_logs) {
            if (log.text.empty()) continue;

            std::string utf8Text = WStringToString(log.text);
            drawList->AddText(ImVec2(m_offsetX, currentY), IM_COL32(255, 255, 255, 255), utf8Text.c_str());

            currentY += m_lineHeight;
        }
    }

    void LogManager::Update(float deltaTime) {
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
        va_end(args);

        std::lock_guard<std::mutex> lock(m_logsMutex);
        if (m_logs.size() >= m_maxLogs) {
            m_logs.erase(m_logs.begin());
        }
        m_logs.push_back({ buffer, 0.0f });
    }

    std::string LogManager::WStringToString(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
}