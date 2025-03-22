#pragma once
#include <Windows.h>
#include <vector>
#include "Vector.h"

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
        byte padding_3;         // +0x6DB (Unknown)
        byte padding_4;         // +0x6DC (Unknown)
        byte padding_5;         // +0x6DD (Unknown)
        byte SavedGrowth;       // +0x6DE

        char padding_6[0x739 - 0x6DF];
        byte Health;            // +0x739

        char padding_7[0x758 - 0x73A];
        byte Alive;            // +0x758

        char padding_8[0xD84 - 0x759];
        float PositionInAirX;  // +0xD84
        float PositionInAirY;  // +0xD88
        float PositionInAirZ;  // +0xD9C

        // ����������Ч����Һͷ����ʵ��
        bool IsValid() const {
            return validFlag == 216;
        }

        // ʵ���ϻ������˷����ʵ�壬�����
        bool IsAlive() const {
            return Alive == 1;
        }

        bool IsValidPos() const {
            return !Vector::CreateVec3(PositionOnLandX, PositionOnLandY, PositionOnLandZ).IsZero() ||
                !Vector::CreateVec3(PositionInAirX, PositionInAirY, PositionInAirZ).IsZero();
        }

        static Vector::Vec3 lastPosition1;
        static Vector::Vec3 lastPosition2;

        Vector::Vec3 GetPos() const {
            return Vector::CreateVec3(PositionInAirX, PositionInAirY, PositionInAirZ).Distance(lastPosition1) >
                Vector::CreateVec3(PositionOnLandX, PositionOnLandY, PositionOnLandZ).Distance(lastPosition2) ?
                (const_cast<Vector::Vec3&>(lastPosition1) = Vector::CreateVec3(PositionInAirX, PositionInAirY, PositionInAirZ)) :
                (const_cast<Vector::Vec3&>(lastPosition2) = Vector::CreateVec3(PositionOnLandX, PositionOnLandY, PositionOnLandZ));
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