#include "Vector.h"

namespace Vector {
    Vec2 GetVec2(DWORD64 baseAddress) {
        if (!baseAddress) return { 0.0f, 0.0f };
        float* ptr = (float*)baseAddress;
        return { ptr[0], ptr[1] };
    }

    Vec3 GetVec3(DWORD64 baseAddress) {
        if (!baseAddress) return { 0.0f, 0.0f, 0.0f };
        float* ptr = (float*)baseAddress;
        return { ptr[0], ptr[1], ptr[2] };
    }

    Vec4 GetVec4(DWORD64 baseAddress) {
        if (!baseAddress) return { 0.0f, 0.0f, 0.0f, 0.0f };
        float* ptr = (float*)baseAddress;
        return { ptr[0], ptr[1], ptr[2], ptr[3] };
    }
}