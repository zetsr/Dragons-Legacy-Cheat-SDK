#pragma once
#include <Windows.h>
#include <vector>

// ������ά������ָ��
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

    // ����ʵ���б� (EntityList) ��ָ��ɨ�����
    namespace EntityList {
        static constexpr DWORD64 MODULE_OFFSET = 0x03070080;  // ʵ���б�Ļ�ַƫ��
        static const std::vector<DWORD64> FIXED_PREFIX_OFFSETS{ 0x38, 0x550 };  // ǰ�ù̶�ƫ��
        static constexpr DWORD64 ENTITY_OFFSET_START = 0x0;   // ʵ��ƫ�Ʒ�Χ���
        static constexpr DWORD64 ENTITY_OFFSET_END = 0x4000;  // ʵ��ƫ�Ʒ�Χ�յ�
        static constexpr DWORD64 ENTITY_OFFSET_STEP = 0x10;   // ʵ��ƫ�Ʋ���
        static const std::vector<DWORD64> FIXED_SUFFIX_OFFSETS{ 0x48, 0x0 };    // ���ù̶�ƫ��
        static constexpr BYTE TARGET_VALUE = 216;             // Ŀ���ֽ�ֵ������ɸѡʵ��
        static constexpr size_t MAX_ENTITY_COUNT = 200;       // ���ʵ������
    }
}