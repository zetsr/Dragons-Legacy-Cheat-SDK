#pragma once
#include <Windows.h>
#include <vector>

// ������ά������ָ��
namespace Offset {
    namespace LocalPlayerBase {
        static constexpr DWORD64 MODULE_OFFSET = 0x03051260;
        static const std::vector<DWORD64> OFFSETS{ 0x30, 0x270, 0x698, 0xC8, 0x0 };
    }
}