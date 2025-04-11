#include "ShotManager.h"
#include "EntityList.h"
#include "Logs.h"

namespace ShotManager {

    std::queue<ShotInfo> shotQueue;
    ShotResult lastResult = { -1, false, 0, 0, 0.f };

    void LogHit(const ShotResult& result) {
        if (Logs::LogManager::s_instance) {
            Logs::LogManager::s_instance->Printf(L"[Shot HIT] Target %d | Distance: %.1f | Damage: %d | Health Left: %d\n",
                result.targetId,
                result.distance,
                result.damage,
                result.healthRemaining
            );
        }
    }

    void LogMiss(const ShotResult& result) {
        if (Logs::LogManager::s_instance) {
            Logs::LogManager::s_instance->Printf(L"[Shot MISS] Target %d | Distance: %.1f | No Damage\n",
                result.targetId,
                result.distance
            );
        }
    }

    void OnTriggerStart(int targetId, int initialHealth, float distance) {
        shotQueue.push({ targetId, initialHealth, distance });
    }

    void OnTriggerEnd() {
        if (shotQueue.empty()) return;

        auto shot = shotQueue.front();
        shotQueue.pop();

        auto player = EntityList::GetPlayer(shot.targetId);

        ShotResult result{};
        result.targetId = shot.targetId;
        result.distance = shot.distance;

        if (!player || !player->IsValid() || !player->IsAlive()) {
            result.hit = false;
            result.damage = 0;
            result.healthRemaining = shot.initialHealth;
            LogMiss(result);
        }
        else {
            int currentHealth = player->Health;
            int damage = shot.initialHealth - currentHealth;

            result.hit = damage > 0;
            result.damage = damage > 0 ? damage : 0;
            result.healthRemaining = currentHealth;

            if (result.hit) {
                LogHit(result);
            }
            else {
                LogMiss(result);
            }
        }

        lastResult = result;
    }

    bool HasResult() {
        return lastResult.targetId != -1;
    }

    const ShotResult& GetLastResult() {
        return lastResult;
    }

} // namespace ShotManager
