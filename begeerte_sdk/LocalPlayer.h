#pragma once
#include <Windows.h>
#include "SDK.h"

namespace LocalPlayer {
    struct Player {
        // 起始点
        byte validFlag;        // +0x0 (216 = valid)

        char padding_0[0x6C7]; // 到 0x6C8 (1767 字节)
        byte SkinIndex;        // +0x6C8
        byte Gender;           // +0x6C9

        byte padding_1;        // +0x6CA
        byte padding_2;        // +0x6CB
        float Health;          // +0x6CC
        float Armor;           // +0x6D0
        byte Stamina;          // +0x6D4
        byte Bile;             // +0x6D5
        byte Hunger;           // +0x6D6
        byte Thirsty;          // +0x6D7
        byte Disease;          // +0x6D8

        byte padding_3;        // +0x6D9 // 0 = Normal, else = 动画停止更新
        byte GrowthStage;      // +0x6DA
        byte padding_5;        // Unknown +0x6DB
        byte padding_6;        // Unknown +0x6DC
        byte padding_7;        // Unknown +0x6DD
        byte SavedGrowth;      // +0x6DE
        char padding_9[0x6E8 - 0x6DF];        // Unknown

        // char padding_3[0xF];          // +0x6D9 到 0x6E8 (15 字节)

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

        char padding_10[0x718 - 0x6FB];
        byte Character;         // +0x718

        char padding_11[0x758 - 0x719];
        byte Alive;             // +0x758

        char padding_12[0x86C - 0x759];
        float CameraDistance;   // +0x86C

        char padding_13[0x9D8 - 0x870];
        float ViewAngleX;       // +0x9D8 (x轴)
        float ViewAngleY;       // +0x9DC (y轴, viewAngleX + 0x4)
        float ViewAngleZ;       // +0x9E0 (z轴, viewAngleY + 0x4)

        char padding_14[0xBB8 - 0x9E4];
        byte InAttack;          // 0xBB8
        char padding_15[0xEAC - 0xBB9];
        float fsPositionX;      // +0xEAC
        float fsPositionY;      // +0xEB0
        float fsPositionZ;      // +0xEB4

        bool IsValid() const {
            return validFlag == 216;
        }

        bool IsAlive() const {
            return Alive == 1;
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
            case SDK::Enum_PlayerCharacter::Biolumin:
                Character_Name = L"Bio";
                break;
            default:
                Character_Name = L"Unknown";
                break;
            }

            return Character_Name;
        }

        const wchar_t* GetGrowthStage() const {
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

        const wchar_t* GetGeneticGrades(byte GrowthStage) const {
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

    Player* GetPlayer(DWORD64 baseAddress);
}