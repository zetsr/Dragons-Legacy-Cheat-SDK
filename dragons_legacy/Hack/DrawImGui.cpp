#define NOMINMAX

#include "../Minimal-D3D12-Hook-ImGui-1.0.2/Main/mdx12_api.h"
#include "SDK_Headers.hpp"
#include "ESP.h"
#include "Configs.h"
#include "DrawESP.h"
#include "DrawImGui.h"
#include "GuiStyle.h"

#include <cstdio>
#include <string>
#include <algorithm>
#include <cfloat>

namespace g_DrawImGui {
    void MyImGuiDraw(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags)
    {
        if (!g_GuiStyle::style_initialized) { g_GuiStyle::SetupCustomImGuiStyle(); g_GuiStyle::style_initialized = true; }

        if (g_MDX12::g_MenuState::g_isOpen) {
            ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowBgAlpha(0.95f);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(7.0f / 255.0f, 8.0f / 255.0f, 10.0f / 255.0f, 0.95f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.02f));

            if (ImGui::Begin("Begeerte", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 10.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(1.0f, 1.0f, 1.0f, 0.018f));
                ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.43f, 0.91f, 0.72f, 0.06f));
                ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.43f, 0.91f, 0.72f, 0.12f));
                ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(1.0f, 1.0f, 1.0f, 0.018f));
                ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(1.0f, 1.0f, 1.0f, 0.03f));

                if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_None)) {
                    if (ImGui::BeginTabItem("Visuals")) {
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 12.0f));
                        g_GuiStyle::BeginTabRegion("VisualsRegion");

                        ImGui::TextColored(g_GuiStyle::ThemeColors::ACCENT, "Basic ESP Settings");
                        ImGui::Separator();
                        g_GuiStyle::DrawColorPickerRow("Box", &g_Config::bDrawBox, "BoxCol1", g_Config::BoxColor);
                        g_GuiStyle::DrawColorPickerRow("Name", &g_Config::bDrawName, "NameCol1", g_Config::NameColor);
                        g_GuiStyle::DrawCustomCheckbox("Health Bar", &g_Config::bDrawHealthBar);
                        ImGui::Separator();

                        ImGui::TextColored(g_GuiStyle::ThemeColors::ACCENT, "Additional Information");
                        ImGui::Separator();
                        g_GuiStyle::DrawCustomCheckbox("Species", &g_Config::bDrawSpecies);
                        g_GuiStyle::DrawCustomCheckbox("Growth Stage", &g_Config::bDrawGrowth);
                        g_GuiStyle::DrawColorPickerRow("Distance", &g_Config::bDrawDistance, "DistCol1", g_Config::DistanceColor);
                        ImGui::Separator();

                        g_GuiStyle::DrawColorPickerRow("Out of FOV", &g_Config::bEnableOOF, "OOFCol1", g_Config::OOFColor);
                        if (g_Config::bEnableOOF) {
                            float avail = ImGui::GetContentRegionAvail().x;
                            float frame_h = ImGui::GetFrameHeight();
                            float pad_y = ImGui::GetStyle().FramePadding.y;
                            float btn_sz = frame_h - pad_y * 2.0f;
                            if (btn_sz <= 0.0f) btn_sz = frame_h * 0.8f;
                            float target = ImGui::GetCursorPosX() + avail - btn_sz;
                            ImGui::SetCursorPosX(target);
                            ImGui::Dummy(ImVec2(0, 4.0f));
                            g_GuiStyle::DrawCustomSliderFloat("Arrow Size", &g_Config::OOFSize, 5.0f, 30.0f, "%.1f");
                            g_GuiStyle::DrawCustomSliderFloat("Screen Radius", &g_Config::OOFRadius, 0.5f, 0.95f, "%.2f");
                            g_GuiStyle::DrawCustomSliderFloat("Breath Speed", &g_Config::OOFBreathSpeed, 0.5f, 5.0f, "%.1f");
                            g_GuiStyle::DrawCustomSliderFloat("Min Alpha", &g_Config::OOFMinAlpha, 0.1f, 0.9f, "%.2f");
                            g_GuiStyle::DrawCustomSliderFloat("Max Alpha", &g_Config::OOFMaxAlpha, 0.2f, 1.0f, "%.2f");
                        }

                        g_GuiStyle::EndTabRegion();
                        ImGui::PopStyleVar();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Team")) {
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 12.0f));
                        g_GuiStyle::BeginTabRegion("TeamRegion");

                        ImGui::TextColored(g_GuiStyle::ThemeColors::ACCENT, "Team ESP Settings");
                        ImGui::Separator();
                        g_GuiStyle::DrawColorPickerRow("Box##Team", &g_Config::bDrawBoxTeam, "BoxColTeam", g_Config::BoxColorTeam);
                        g_GuiStyle::DrawColorPickerRow("Name##Team", &g_Config::bDrawNameTeam, "NameColTeam", g_Config::NameColorTeam);
                        g_GuiStyle::DrawCustomCheckbox("Health Bar##Team", &g_Config::bDrawHealthBarTeam);
                        ImGui::Separator();

                        ImGui::TextColored(g_GuiStyle::ThemeColors::ACCENT, "Additional Information");
                        ImGui::Separator();
                        g_GuiStyle::DrawCustomCheckbox("Species##Team", &g_Config::bDrawSpeciesTeam);
                        g_GuiStyle::DrawCustomCheckbox("Growth Stage##Team", &g_Config::bDrawGrowthTeam);
                        g_GuiStyle::DrawColorPickerRow("Distance##Team", &g_Config::bDrawDistanceTeam, "DistColTeam", g_Config::DistanceColorTeam);
                        ImGui::Separator();

                        g_GuiStyle::EndTabRegion();
                        ImGui::PopStyleVar();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Clan")) {
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 12.0f));
                        g_GuiStyle::BeginTabRegion("ClanRegion");

                        ImGui::TextColored(g_GuiStyle::ThemeColors::ACCENT, "Clan ESP Settings");
                        ImGui::Separator();
                        g_GuiStyle::DrawColorPickerRow("Box##Clan", &g_Config::bDrawBoxClan, "BoxColClan", g_Config::BoxColorClan);
                        g_GuiStyle::DrawColorPickerRow("Name##Clan", &g_Config::bDrawNameClan, "NameColClan", g_Config::NameColorClan);
                        g_GuiStyle::DrawCustomCheckbox("Health Bar##Clan", &g_Config::bDrawHealthBarClan);
                        ImGui::Separator();

                        ImGui::TextColored(g_GuiStyle::ThemeColors::ACCENT, "Additional Information");
                        ImGui::Separator();
                        g_GuiStyle::DrawCustomCheckbox("Species##Clan", &g_Config::bDrawSpeciesClan);
                        g_GuiStyle::DrawCustomCheckbox("Growth Stage##Clan", &g_Config::bDrawGrowthClan);
                        g_GuiStyle::DrawColorPickerRow("Distance##Clan", &g_Config::bDrawDistanceClan, "DistColClan", g_Config::DistanceColorClan);
                        ImGui::Separator();

                        g_GuiStyle::EndTabRegion();
                        ImGui::PopStyleVar();
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();

                }

                ImGui::PopStyleColor(5);
                ImGui::PopStyleVar(2);
            }

            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(2);
            ImGui::End();
        }

        g_DrawESP::DrawESP();
    }
}