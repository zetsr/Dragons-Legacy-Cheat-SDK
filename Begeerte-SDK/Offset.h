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

    namespace ClientViewAngles {
        static constexpr DWORD64 MODULE_OFFSET = 0x03051260;
        static const std::vector<DWORD64> OFFSETS{ 0x30, 0x2A8 };
    }

    // 定义实体列表 (EntityList) 的指针扫描参数
    namespace EntityList {
        static constexpr DWORD64 MODULE_OFFSET = 0x03070080;  // 实体列表的基址偏移
        static const std::vector<DWORD64> FIXED_PREFIX_OFFSETS{ 0x38, 0x550 };  // 前置固定偏移
        static constexpr DWORD64 ENTITY_OFFSET_START = 0x0;   // 实体偏移范围起点
        static constexpr DWORD64 ENTITY_OFFSET_END = 0x4000;  // 实体偏移范围终点
        static constexpr DWORD64 ENTITY_OFFSET_STEP = 0x10;   // 实体偏移步长
        static const std::vector<DWORD64> FIXED_SUFFIX_OFFSETS{ 0x48, 0x0 };    // 后置固定偏移
        static constexpr BYTE TARGET_VALUE = 216;             // 目标字节值，用于筛选实体
        static constexpr size_t MAX_ENTITY_COUNT = 200;       // 最大实体数量
    }
}