#pragma once
#include <Windows.h>

namespace Hook {
    void Initialize();
    void Cleanup();
    DWORD64 GetLocalPlayerAddress();
    DWORD64 GetLocalCameraPositionAddress();
    DWORD64 GetClientCurrentTimeAddress();
    DWORD64 GetClientCurrentTickAddress();
}