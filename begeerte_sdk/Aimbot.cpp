#include "Aimbot.h"
#include <cmath>

namespace Aimbot {
	bool Aimbot::CheckInsideEntity(const Vector::Vec3& head, const Vector::Vec3& point) {
		// 检查x轴范围（宽度方向，左右各50单位）
		bool inX = (point.x >= head.x - 200.0f) && (point.x <= head.x + 200.0f);

		// 检查y轴范围（长度方向，向后延伸400单位）
		bool inY = (point.y >= head.y - 1400.0f) && (point.y <= head.y + 200.0f);

		// 检查z轴范围（高度方向，向下延伸100单位）
		bool inZ = (point.z >= head.z - 300.0f) && (point.z <= head.z);

	    return inX && inY && inZ;
	}

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

		if (!localPlayer) return;
		if (!localPlayer->fsPositionX && !localPlayer->fsPositionY && !localPlayer->fsPositionZ) return;

		auto localplayer_pos = Vector::Vec3(localPlayer->fsPositionX, localPlayer->fsPositionY, localPlayer->fsPositionZ);

		static bool wasInAttack = false;

		for (int i = 1; i <= EntityList::GetMaxPlayers(); i++) {
			auto entity = EntityList::GetEntity(i);
			if (entity == 0) continue;

			auto player = EntityList::GetPlayer(i);
			if (!player || !player->IsValid() || !player->IsAlive() || !player->IsValidPos()) continue;

			float lp2enemy = player->GetPos().Distance(localplayer_pos);

			// ------------------ 监控攻击状态变更，委托 ShotManager 管理 ------------------
			bool nowInAttack = localPlayer->InAttack;

			if (!wasInAttack && nowInAttack) {
				// 攻击开始，记录目标、初始血量和距离
				ShotManager::OnTriggerStart(i, player->Health, lp2enemy);
			}
			else if (wasInAttack && !nowInAttack) {
				// 攻击结束，自动出队处理结果
				ShotManager::OnTriggerEnd();
			}

			// ------------------ 判断触发键和目标合法性 ------------------
			if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) {
				if (CheckInsideEntity(player->GetPos(), localplayer_pos)) {
					// 执行攻击命令
					Attack(16);

					// 本地日志输出（保留）
					if (Logs::LogManager::s_instance) {
						Logs::LogManager::s_instance->Printf(L"TriggerBot: Target->%d | Distance: %.1f | Health: %d\n",
							i,
							lp2enemy,
							player->Health
						);
					}
				}
			}

			// 更新攻击状态记录
			wasInAttack = nowInAttack;
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