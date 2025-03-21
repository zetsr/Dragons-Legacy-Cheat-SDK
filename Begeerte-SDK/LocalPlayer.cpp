#include "LocalPlayer.h"
#include "Memory.h"

namespace LocalPlayer {
    Player* GetPlayer(DWORD64 baseAddress) {
        if (!baseAddress) return nullptr;
        return (Player*)baseAddress; // 直接返回结构指针
    }
}