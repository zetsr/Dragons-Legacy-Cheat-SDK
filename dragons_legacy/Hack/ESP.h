#pragma once
#include <vector>
#include <string>

namespace g_ESP {
    SDK::APlayerController* GetLocalPC();

    struct BoxRect {
        ImVec2 topLeft;
        ImVec2 bottomRight;
        bool valid = false;
    };

    enum class FlagPos {
        Left,
        Right
    };

    // Flag 管理器
    class FlagManager {
    private:
        float leftY = 0.0f;
        float rightY = 0.0f;

    public:
        void Reset() {
            leftY = 0.0f;
            rightY = 0.0f;
        }

        void AddFlag(BoxRect rect, const std::string& text, ImU32 color, FlagPos pos);
    };

    // 原有函数 - 添加 bTestOnly 参数
    BoxRect DrawBox(SDK::AActor* entity, float r, float g, float b, float a, float width_scale, bool bTestOnly = false);
    void DrawHealthBar(BoxRect rect, float healthPercent, float maxHealth, float a);
    void DrawName(SDK::AActor* entity, BoxRect rect, float r, float g, float b, float a);

    // OOF 结构体
    struct OOFFlag {
        std::string text;
        ImU32 color;
    };

    // OOF 渲染函数
    void DrawOutOfFOV(SDK::AActor* entity, SDK::APlayerController* LocalPC, const std::vector<OOFFlag>& flags);

    // 关系类型枚举
    enum class RelationType {
        Enemy,      // 敌人
        Team,       // 队友
        Clan        // 氏族成员
    };

    // 获取玩家关系
    RelationType GetPlayerRelation(SDK::APlayerState* targetPS, SDK::APlayerState* localPS);
}