#include "setup_imgui.h"
#include "../Logs.h"
#include "../Radar.h"

void SetupImGui(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (menu::isOpen) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(1.0f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        if (ImGui::Begin("Begeerte", nullptr, flags)) {
            if (ImGui::CollapsingHeader("Aimbot")) {
                ImGui::Indent(10.0f);

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.75f), "Coming soon");

                ImGui::Unindent(10.0f);
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Visuals", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(10.0f);


                ImGui::Checkbox("2D Radar", &g_2d_radar);

                ImGui::Unindent(10.0f);
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(10.0f);

                ImGui::Checkbox("Draw logs", &g_draw_logs);

                ImGui::Unindent(10.0f);
                ImGui::Spacing();
            }

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.75f), "Press F1 to toggle menu");

            // ============ 底部文字：真正贴在主窗口最底部 ============
            float windowHeight = ImGui::GetWindowHeight();
            float textHeight = ImGui::GetTextLineHeight();
            ImGui::SetCursorPosY(windowHeight - textHeight - ImGui::GetStyle().WindowPadding.y);
            // 左下角
            ImGui::TextColored(ImVec4(0.40f, 0.55f, 0.85f, 0.8f), "github.com/zetsr");
            // 右下角
            const char *version_text = "v1.1.15 - 2025/12/19";
            float versionWidth = ImGui::CalcTextSize(version_text).x;
            ImGui::SameLine(ImGui::GetWindowWidth() - versionWidth - ImGui::GetStyle().WindowPadding.x);
            ImGui::TextColored(ImVec4(0.40f, 0.55f, 0.85f, 0.8f), version_text);
        }

        ImGui::End();
    }

    if (g_2d_radar) {
        Radar::RadarManager::GetInstance()->DrawRadar();
    }

    if (g_draw_logs) {
        Logs::LogManager::GetInstance()->Update(ImGui::GetIO().DeltaTime);
        Logs::LogManager::GetInstance()->DrawLogs();
    }

}