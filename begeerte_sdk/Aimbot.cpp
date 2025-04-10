#include "Aimbot.h"
#include <cmath>

namespace Aimbot {
	void Aimbot::Attack(int delay) {
		INPUT input = { 0 };

		// 按下
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &input, sizeof(INPUT));

		Sleep(delay);

		// 释放
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &input, sizeof(INPUT));
	}

	void Aimbot::TriggerBot() {
		auto localPlayer = LocalPlayer::GetPlayer(Hook::GetLocalPlayerAddress());

		if (!localPlayer) {
			return;
		}

		if (!localPlayer->fsPositionX || !localPlayer->fsPositionY || !localPlayer->fsPositionZ) {
			return;
		}

		for (int i = 1; i <= EntityList::GetMaxPlayers(); i++) {
			auto entity = EntityList::GetEntity(i);

			if (entity == 0) {
				continue;
			}

			auto player = EntityList::GetPlayer(i);

			if (!player) {
				continue;
			}

			if (!player->IsValid()) {
				continue;
			}

			if (!player->IsAlive()) {
				continue;
			}

			if (!player->IsValidPos()) {
				continue;
			}

			if (!player->IsValidPos()) {
				continue;
			}

			float lp2enemy = player->GetPos().Distance(Vector::Vec3(localPlayer->fsPositionX, localPlayer->fsPositionY, localPlayer->fsPositionZ));

			if (localPlayer->InAttack) {
				return;
			}

			if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) {
				if (lp2enemy <= 200.f) {
					Attack(16);
					if (Logs::LogManager::s_instance) {
						Logs::LogManager::s_instance->Printf(L"TriggerBot: Target->%d | Distance: %.1f | Pitch: %.1f | Yaw: %.1f\n",
							i,
							lp2enemy,
							localPlayer->ViewAngleX,
							localPlayer->ViewAngleY
						);
					}
				}
			}
		}
	}

	void Aimbot::LegitBot() {
		auto localPlayer = LocalPlayer::GetPlayer(Hook::GetLocalPlayerAddress());
		auto ClientViewAngles = Vector::GetVec3(Hook::GetClientViewAnglesAddress());

		if (!localPlayer) {
			return;
		}

		if (ClientViewAngles.IsZero()) {
			return;
		}

		static int lockedTarget = -1;  // 锁定目标的索引
		float closestDist = FLT_MAX;   // 最近距离
		int closestEnemy = -1;         // 最近敌人的索引

		// 只在按住Mouse5 (VK_XBUTTON2) 时激活
		if (!(GetAsyncKeyState(VK_XBUTTON2) & 0x8000)) {
			lockedTarget = -1;  // 松开Mouse5时重置锁定
			return;
		}

		Vector::Vec3 localPos(localPlayer->fsPositionX, localPlayer->fsPositionY, localPlayer->fsPositionZ);

		// 遍历所有玩家寻找有效目标
		for (int i = 1; i <= EntityList::GetMaxPlayers(); i++) {
			auto player = EntityList::GetPlayer(i);

			if (!player) {
				continue;
			}

			if (!player->IsValid()) {
				continue;
			}

			if (!player->IsAlive()) {
				continue;
			}

			if (!player->IsValidPos()) {
				continue;
			}

			Vector::Vec3 enemyPos = player->GetPos();
			float distance = enemyPos.Distance(localPos);

			// 如果已经锁定了目标，只更新与锁定目标的距离
			if (lockedTarget == i) {
				closestDist = distance;
				closestEnemy = i;
				break;
			}

			// 寻找最近的敌人
			if (distance < closestDist) {
				closestDist = distance;
				closestEnemy = i;
			}
		}

		// 如果没有找到有效目标，重置锁定并返回
		if (closestEnemy == -1) {
			lockedTarget = -1;
			return;
		}

		// 锁定最近的目标
		lockedTarget = closestEnemy;
		auto targetPlayer = EntityList::GetPlayer(lockedTarget);
		if (!targetPlayer) return;

		Vector::Vec3 enemyPos = targetPlayer->GetPos();

		// 手动计算位置差值
		float deltaX = enemyPos.x - localPos.x;
		float deltaY = enemyPos.y - localPos.y;
		float deltaZ = enemyPos.z - localPos.z;

		// 计算目标角度（Pitch和Yaw）
		float yaw = atan2f(deltaY, deltaX) * 180.0f / 3.14159f;
		float pitch = atan2f(-deltaZ, sqrtf(deltaX * deltaX + deltaY * deltaY)) * 180.0f / 3.14159f;

		// 直接设置视角，不使用平滑
		ClientViewAngles.x = pitch;
		ClientViewAngles.y = yaw;

		if (Logs::LogManager::s_instance) {
			Logs::LogManager::s_instance->Printf(L"LegitBot: Target->%d | Distance: %.1f | Pitch: %.1f | Yaw: %.1f\n",
				lockedTarget,
				closestDist,
				pitch,
				yaw
			);
		}
	}
}