#pragma once
#include <Windows.h>
#include <vector>

namespace EntityList {
    // 定义玩家结构，与 LocalPlayer.h 一致
    struct Player {
        byte validFlag;         // +0x0 (216 = valid)

        char padding_1[0xA0 - 0x1];
        float PositionOnLandX;  // +0xA0
        float PositionOnLandY;  // +0xA4
        float PositionOnLandZ;  // +0xA8

        char padding_2[0x6DA - 0xAC];  // float = 4 byte
        byte GrowthStage;       // +0x6DA
        byte padding_5;         // +0x6DB (Unknown)
        byte padding_6;         // +0x6DC (Unknown)
        byte padding_7;         // +0x6DD (Unknown)
        byte SavedGrowth;       // +0x6DE

        char padding_3[0x739 - 0x6DF];   // 从 0x6DF 到 0x738
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