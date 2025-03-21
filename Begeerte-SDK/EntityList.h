#pragma once
#include <Windows.h>
#include <vector>

namespace EntityList {
    // 定义玩家结构，与 LocalPlayer.h 一致
    struct Player {
        byte validFlag;         // +0x0 (216 = valid)
        char padding_0[0x6D9];  // 从 0x1 到 0x6DA，填充 1753 字节

        byte GrowthStage;       // +0x6DA
        byte padding_5;         // +0x6DB (Unknown)
        byte padding_6;         // +0x6DC (Unknown)
        byte padding_7;         // +0x6DD (Unknown)
        byte SavedGrowth;       // +0x6DE

        char padding_1[0x5A];   // 从 0x6DF 到 0x738，填充 90 字节
        byte Health;            // +0x739

        bool IsValid() const {
            return validFlag == 216;
        }
    };

    // 刷新实体列表
    void Update();

    // 获取当前实体数量（最大玩家数量）
    size_t GetMaxPlayers();

    // 获取指定 ID 的实体地址（ID 从 1 开始）
    DWORD64 GetEntity(int id);

    // 获取指定 ID 的 Player 结构（ID 从 1 开始）
    Player* GetPlayer(int id);

    // 获取所有实体的地址列表
    const std::vector<DWORD64>& GetAllEntities();
}