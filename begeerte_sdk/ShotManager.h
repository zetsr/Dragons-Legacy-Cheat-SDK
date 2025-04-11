#ifndef SHOT_MANAGER_H
#define SHOT_MANAGER_H

#include <queue>

namespace ShotManager {

    struct ShotInfo {
        int targetId;
        int initialHealth;
        float distance;
    };

    struct ShotResult {
        int targetId;
        bool hit;
        int damage;
        int healthRemaining;
        float distance;
    };

    void OnTriggerStart(int targetId, int initialHealth, float distance);
    void OnTriggerEnd();

    bool HasResult();
    const ShotResult& GetLastResult();

} // namespace ShotManager

#endif // SHOT_MANAGER_H
