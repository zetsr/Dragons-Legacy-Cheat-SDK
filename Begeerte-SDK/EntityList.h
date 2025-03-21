#pragma once
#include <Windows.h>
#include <vector>

namespace EntityList {
    // ������ҽṹ���� LocalPlayer.h һ��
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

        char padding_3[0x739 - 0x6DF];   // �� 0x6DF �� 0x738
        byte Health;            // +0x739

        bool IsValid() const {
            return validFlag == 216;
        }
    };

    // ˢ��ʵ���б�
    void Update();

    // ��ȡ��ǰʵ��������������������
    size_t GetMaxPlayers();

    // ��ȡָ�� ID ��ʵ���ַ��ID �� 1 ��ʼ��
    DWORD64 GetEntity(int id);

    // ��ȡָ�� ID �� Player �ṹ��ID �� 1 ��ʼ��
    Player* GetPlayer(int id);

    // ��ȡ����ʵ��ĵ�ַ�б�
    const std::vector<DWORD64>& GetAllEntities();
}