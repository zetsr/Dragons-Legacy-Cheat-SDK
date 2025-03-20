#pragma once
#include <Windows.h>

/*
* 以下信息来自服务端转储，可能不准确。
* 以下信息来自服务端转储，可能不准确。
* 以下信息来自服务端转储，可能不准确。
*/

namespace SDK {
    namespace Enum_PlayerCharacter {
        enum Type : byte {
            FlameStalker = 0,   // Flame Stalker
            ShadowScale = 2,    // Shadow Scale
            AcidSpitter = 3     // Acid Spitter
        };
    }

    namespace Enum_GrowthStage {
        enum Type : byte {
            Hatchling = 1,      // Hatchling
            Juvenile = 2,       // Juvenile
            Adult = 3,          // Adult
            Elder = 4,          // Elder
            Ancient = 5         // Ancient
        };
    }

    namespace Enum_GeneticGrades {
        enum Type : byte {
            F = 0,                // Enum_GeneticGrades::NewEnumerator13 (F)
            E = 1,                // Enum_GeneticGrades::NewEnumerator0 (E)
            D_Minus = 2,          // Enum_GeneticGrades::NewEnumerator1 (D-)
            D = 3,                // Enum_GeneticGrades::NewEnumerator2 (D)
            D_Plus = 4,           // Enum_GeneticGrades::NewEnumerator3 (D+)
            C_Minus = 5,          // Enum_GeneticGrades::NewEnumerator4 (C-)
            C = 6,                // Enum_GeneticGrades::NewEnumerator5 (C)
            C_Plus = 7,           // Enum_GeneticGrades::NewEnumerator6 (C+)
            B_Minus = 8,          // Enum_GeneticGrades::NewEnumerator7 (B-)
            B = 9,                // Enum_GeneticGrades::NewEnumerator8 (B)
            B_Plus = 10,          // Enum_GeneticGrades::NewEnumerator9 (B+)
            A_Minus = 11,         // Enum_GeneticGrades::NewEnumerator10 (A-)
            A = 12,               // Enum_GeneticGrades::NewEnumerator11 (A)
            A_Plus = 13,          // Enum_GeneticGrades::NewEnumerator12 (A+)
            A_Plus_Plus = 14,     // Enum_GeneticGrades::NewEnumerator14 (A++)
            Enum_Max = 15         // Enum_GeneticGrades::Enum_MAX
        };
    }
}