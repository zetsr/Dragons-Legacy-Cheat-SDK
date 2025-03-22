#ifndef CHEAT_DATA_H
#define CHEAT_DATA_H

#include "LocalPlayer.h"
#include "Vector.h"

struct CheatData {
    Vector::Vec3 LocalCameraPosition;
    LocalPlayer::Player* LocalPlayer;
    int ClientCurrentTime;
    int ClientCurrentTick;
};

inline static CheatData cheatDataInstance = {
    .LocalCameraPosition = Vector::Vec3(0, 0, 0),
    .LocalPlayer = nullptr,
    .ClientCurrentTime = 0,
    .ClientCurrentTick = 0 
};

// 全局指针，指向静态实例
inline static CheatData* g_cheatdata = &cheatDataInstance;

#endif // CHEAT_DATA_H