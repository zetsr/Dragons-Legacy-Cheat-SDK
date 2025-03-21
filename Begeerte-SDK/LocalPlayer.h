#pragma once
#include <Windows.h>

namespace LocalPlayer {
    struct Player {
        // 起始点
        byte validFlag;        // +0x0 (216 = valid)

        char padding_0[0x6C7]; // 到 0x6C8 (1767 字节)
        byte SkinIndex;        // +0x6C8
        byte Gender;           // +0x6C9

        byte padding_1;        // Unknown
        byte padding_2;        // Unknown
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
        byte padding_9;        // Unknown
        byte padding_10;       // Unknown
        byte padding_11;       // Unknown
        byte padding_12;       // Unknown
        byte padding_13;       // Unknown
        byte padding_14;       // Unknown
        byte padding_15;       // Unknown
        byte padding_16;       // Unknown
        byte padding_17;       // Unknown

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

        char padding_18[0x2DD]; // +0x6FB 到 0x9D8 (733 字节)
        float ViewAngleX;       // +0x9D8 (x轴)
        float ViewAngleY;       // +0x9DC (y轴, viewAngleX + 0x4)
        float ViewAngleZ;       // +0x9E0 (z轴, viewAngleY + 0x4)

        bool IsValid() const {
            if (validFlag == 216) {
                return true;
            }

            return false;
        }
    };

    Player* GetPlayer(DWORD64 baseAddress);
}