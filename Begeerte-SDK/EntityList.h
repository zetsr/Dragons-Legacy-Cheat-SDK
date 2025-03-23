#pragma once
#include <Windows.h>
#include <vector>
#include "Vector.h"
#include "SDK.h"

namespace EntityList {
    // ������ҽṹ���� LocalPlayer.h һ��
    struct Player {
        byte validFlag;         // +0x0 (216 = valid)

        char padding_1[0xA0 - 0x1];
        float PositionOnLandX;  // +0xA0
        float PositionOnLandY;  // +0xA4
        float PositionOnLandZ;  // +0xA8

        char padding_2[0x6C8 - 0xAC];  // float = 4 byte
        byte SkinIndex;        // +0x6C8
        byte Gender;           // +0x6C9

        char padding_3[0x6DA - 0x6CA];
        byte GrowthStage;       // +0x6DA
        byte padding_4;         // +0x6DB (Unknown)
        byte padding_5;         // +0x6DC (Unknown)
        byte padding_6;         // +0x6DD (Unknown)
        byte SavedGrowth;       // +0x6DE

        /*
        * 
        * ���ʵ�巵��1������顣
        * 
        */
        char padding_7[0x6E8 - 0x6DF];
        byte VitalityHealth;            // +0x6E8 VitalityHealth_20_AD4FDC3944007D89AA165EA075C7A6A4
        byte VitalityArmor;             // +0x6E9 VitalityArmor_21_6058776047A93E704725F2B1017143B5
        byte VitalityBile;              // +0x6EA VitalityBile_22_1441154144FB80F99B0EA4ACCF940906
        byte VitalityStamina;           // +0x6EB VitalityStamina_23_4F7A985E444EDC983C8689B3141CAD36
        byte VitalityHunger;            // +0x6EC VitalityHunger_24_EC22292447DC9308AF7CAC8610072A5F
        byte VitalityThirst;            // +0x6ED VitalityThirst_25_9C3850144E3CD527F38898A71F752B8E
        byte VitalityTorpor;            // +0x6EE VitalityTorpor_26_8C2EE49042020674C4A1D4A05033BEE5
        byte DamageBite;                // +0x6EF DamageBite_30_1DF6387A4C50C4FFC3A80EA0664039FB
        byte DamageProjectile;          // +0x6F0 DamageProjectile_31_4AC9E8454D323D359F6CF089EBB5BEA6
        byte DamageSwipe;               // +0x6F1 DamageSwipe_32_7D472C2D4AC0E83F7B3050B9A5C7C389
        byte MitigationBlunt;           // +0x6F2 MitigationBlunt_41_DFC896ED4753A11A442EA489C8412BA5
        byte MitigationPierce;          // +0x6F3 MitigationPierce_42_AA53F66F45D0660054C2F18F6D587327
        byte MitigationFire;            // +0x6F4 MitigationFire_43_CC713117436D25BCC7F49B9EFBE6029B
        byte MitigationFrost;           // +0x6F5 MitigationFrost_44_6F1B808C4FED477721AA2EA984E5B3EA
        byte MitigationAcid;            // +0x6F6 MitigationAcid_45_E41BF8144F4656DDBFACB8915D9BF109
        byte MitigationVenom;           // +0x6F7 MitigationVenom_46_1F3565A44951C3A615F994822E39C4F2
        byte MitigationPlasma;          // +0x6F8 MitigationPlasma_47_1D8376A54DB8117876136FA72072B6A0
        byte MitigationElectricity;     // +0x6F9 MitigationElectricity_50_F0848404440B35626E72B398C528F968
        byte OverallQuality;            // +0x6FA OverallQuality_56_56BD12AD4526647ACEF337ADFD3E2847

        char padding_8[0x718 - 0x6FB];
        byte Character;        // +0x718

        char padding_9[0x739 - 0x719];
        byte Health;           // +0x739

        char padding_10[0x758 - 0x73A];
        byte Alive;            // +0x758

        char padding_11[0xD84 - 0x759];
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

        Vector::Vec3 GetPos() const {
            // ��ȡ��ǰ֡�Ŀ��к͵�������
            Vector::Vec3 currentAir = Vector::CreateVec3(PositionInAirX, PositionInAirY, PositionInAirZ);
            Vector::Vec3 currentLand = Vector::CreateVec3(PositionOnLandX, PositionOnLandY, PositionOnLandZ);

            // ��̬������¼��һ֡������
            static Vector::Vec3 lastAir = currentAir;
            static Vector::Vec3 lastLand = currentLand;

            // ����仯��ֵ�����⸡������������
            const float epsilon = 0.001f;  // �ɸ���ʵ���������

            // ���㵱ǰ֡����һ֡�ľ���仯
            float airDistance = currentAir.Distance(lastAir);
            float landDistance = currentLand.Distance(lastLand);

            // ��̬������¼��һ�η��صĽ������ʼ��Ϊ��������
            static Vector::Vec3 lastResult = currentLand;

            Vector::Vec3 result;
            if (airDistance > landDistance + epsilon) {
                // ��������仯���󣬷��ؿ�������
                result = currentAir;
            }
            else if (landDistance > airDistance + epsilon) {
                // ��������仯���󣬷��ص�������
                result = currentLand;
            }
            else {
                // �仯������ޱ仯��������һ֡�Ľ��
                result = lastResult;
            }

            // ������һ֡������
            lastAir = currentAir;
            lastLand = currentLand;

            // ֻ����ȷ�л�ʱ�Ÿ��� lastResult
            if (airDistance > landDistance + epsilon || landDistance > airDistance + epsilon) {
                lastResult = result;
            }

            return result;
        }

        const wchar_t* GetGender() const {
            auto Gender_Name = L"Unknown";
            auto Gender_Index = Gender;

            if (Gender_Index == 0) {
                Gender_Name = L"F";
            }

            if (Gender_Index == 1) {
                Gender_Name = L"M";
            }

            return Gender_Name;
        }

        const wchar_t* GetCharacter() {
            auto Character_Name = L"Unknown";
            auto Character_Index = Character;

            switch (Character_Index) {
            case SDK::Enum_PlayerCharacter::FlameStalker:
                Character_Name = L"FS";
                break;
            case SDK::Enum_PlayerCharacter::InfernoRavager:
                Character_Name = L"IR";
                break;
            case SDK::Enum_PlayerCharacter::ShadowScale:
                Character_Name = L"SS";
                break;
            case SDK::Enum_PlayerCharacter::AcidSpitter:
                Character_Name = L"ASD";
                break;
            case SDK::Enum_PlayerCharacter::BlitzStriker:
                Character_Name = L"BS";
                break;
            case SDK::Enum_PlayerCharacter::ThornbackCrawler:
                Character_Name = L"ThornbackCrawler";
                break;
            case SDK::Enum_PlayerCharacter::BulbousSpinedScrab:
                Character_Name = L"BulbousSpinedScrab";
                break;
            case SDK::Enum_PlayerCharacter::Biolumin:
                Character_Name = L"Bio";
                break;
            default:
                Character_Name = L"Unknown";
                break;
            }

            return Character_Name;
        }

        const wchar_t* GetGrowthStage() {
            auto GrowthStage_Name = L"Unknown";
            auto GrowthStage_Index = GrowthStage;

            switch (GrowthStage_Index) {
            case SDK::Enum_GrowthStage::Hatchling:
                GrowthStage_Name = L"Hatchling";
                break;
            case SDK::Enum_GrowthStage::Juvenile:
                GrowthStage_Name = L"Juvenile";
                break;
            case SDK::Enum_GrowthStage::Adult:
                GrowthStage_Name = L"Adult";
                break;
            case SDK::Enum_GrowthStage::Elder:
                GrowthStage_Name = L"Elder";
                break;
            case SDK::Enum_GrowthStage::Ancient:
                GrowthStage_Name = L"Ancient";
                break;
            default:
                GrowthStage_Name = L"Unknown";
                break;
            }

            return GrowthStage_Name;
        }

        const wchar_t* GetGeneticGrades(byte GrowthStage) {
            auto GeneticGrades_Name = L"Unknown";
            auto GeneticGrades_Index = GrowthStage;

            switch (GeneticGrades_Index) {
            case SDK::Enum_GeneticGrades::F:
                GeneticGrades_Name = L"F";
                break;
            case SDK::Enum_GeneticGrades::E:
                GeneticGrades_Name = L"E";
                break;
            case SDK::Enum_GeneticGrades::D_Minus:
                GeneticGrades_Name = L"D-";
                break;
            case SDK::Enum_GeneticGrades::D:
                GeneticGrades_Name = L"D";
                break;
            case SDK::Enum_GeneticGrades::D_Plus:
                GeneticGrades_Name = L"D+";
                break;
            case SDK::Enum_GeneticGrades::C_Minus:
                GeneticGrades_Name = L"C-";
                break;
            case SDK::Enum_GeneticGrades::C:
                GeneticGrades_Name = L"C";
                break;
            case SDK::Enum_GeneticGrades::C_Plus:
                GeneticGrades_Name = L"C+";
                break;
            case SDK::Enum_GeneticGrades::B_Minus:
                GeneticGrades_Name = L"B-";
                break;
            case SDK::Enum_GeneticGrades::B:
                GeneticGrades_Name = L"B";
                break;
            case SDK::Enum_GeneticGrades::B_Plus:
                GeneticGrades_Name = L"B+";
                break;
            case SDK::Enum_GeneticGrades::A_Minus:
                GeneticGrades_Name = L"A-";
                break;
            case SDK::Enum_GeneticGrades::A:
                GeneticGrades_Name = L"A";
                break;
            case SDK::Enum_GeneticGrades::A_Plus:
                GeneticGrades_Name = L"A+";
                break;
            case SDK::Enum_GeneticGrades::A_Plus_Plus:
                GeneticGrades_Name = L"A++";
                break;
            case SDK::Enum_GeneticGrades::Enum_Max:
                GeneticGrades_Name = L"Enum_Max";
                break;
            default:
                GeneticGrades_Name = L"Unknown";
                break;
            }

            return GeneticGrades_Name;
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