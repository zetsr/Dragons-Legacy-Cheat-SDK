#pragma once
#include <Windows.h>
#include <vector>

// 在这里维护所有指针
namespace Offset {
    namespace LocalPlayerBase {
        static constexpr DWORD64 MODULE_OFFSET = 0x03051260;
        static const std::vector<DWORD64> OFFSETS{ 0x30, 0x270, 0x698, 0xC8, 0x0 };
    }

    namespace LocalCameraPosition {
        static constexpr DWORD64 MODULE_OFFSET = 0x0304B3C0;
        static const std::vector<DWORD64> OFFSETS{ 0x2D0 };
    }

    namespace ClientCurrentTime {
        static constexpr DWORD64 MODULE_OFFSET = 0x2F2752C;
        static const std::vector<DWORD64> OFFSETS{ 0x0 };
    }

    namespace ClientCurrentTick {
        static constexpr DWORD64 MODULE_OFFSET = 0x2F567E4;
        static const std::vector<DWORD64> OFFSETS{ 0x0 };
    }
}