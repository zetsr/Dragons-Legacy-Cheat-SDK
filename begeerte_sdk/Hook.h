#pragma once
#include <Windows.h>

namespace Hook {
    void Hook_Initialize();
    void Hook_Cleanup();
    DWORD64 GetLocalPlayerAddress();
    DWORD64 GetLocalCameraPositionAddress();
    DWORD64 GetClientCurrentTimeAddress();
    DWORD64 GetClientCurrentTickAddress();
    DWORD64 GetClientViewAnglesAddress();
}