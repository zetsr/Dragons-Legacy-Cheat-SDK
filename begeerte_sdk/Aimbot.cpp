#include "Aimbot.h"
#include <cmath>

namespace Aimbot {
	bool Aimbot::CheckInsideEntity(const Vector::Vec3& head, const Vector::Vec3& point) {
		// ���x�᷶Χ����ȷ������Ҹ�50��λ��
		bool inX = (point.x >= head.x - 200.0f) && (point.x <= head.x + 200.0f);

		// ���y�᷶Χ�����ȷ����������400��λ��
		bool inY = (point.y >= head.y - 1400.0f) && (point.y <= head.y + 200.0f);

		// ���z�᷶Χ���߶ȷ�����������100��λ��
		bool inZ = (point.z >= head.z - 300.0f) && (point.z <= head.z);

	    return inX && inY && inZ;
	}

	void Aimbot::Attack(int delay) {
		INPUT input = { 0 };

		// ����
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &input, sizeof(INPUT));

		Sleep(delay);

		// �ͷ�
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

			// ------------------ ��ع���״̬�����ί�� ShotManager ���� ------------------
			bool nowInAttack = localPlayer->InAttack;

			if (!wasInAttack && nowInAttack) {
				// ������ʼ����¼Ŀ�ꡢ��ʼѪ���;���
				ShotManager::OnTriggerStart(i, player->Health, lp2enemy);
			}
			else if (wasInAttack && !nowInAttack) {
				// �����������Զ����Ӵ�����
				ShotManager::OnTriggerEnd();
			}

			// ------------------ �жϴ�������Ŀ��Ϸ��� ------------------
			if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) {
				if (CheckInsideEntity(player->GetPos(), localplayer_pos)) {
					// ִ�й�������
					Attack(16);

					// ������־�����������
					if (Logs::LogManager::s_instance) {
						Logs::LogManager::s_instance->Printf(L"TriggerBot: Target->%d | Distance: %.1f | Health: %d\n",
							i,
							lp2enemy,
							player->Health
						);
					}
				}
			}

			// ���¹���״̬��¼
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

		static int lockedTarget = -1;  // ����Ŀ�������
		float closestDist = FLT_MAX;   // �������
		int closestEnemy = -1;         // ������˵�����

		// ֻ�ڰ�סMouse5 (VK_XBUTTON2) ʱ����
		if (!(GetAsyncKeyState(VK_XBUTTON2) & 0x8000)) {
			lockedTarget = -1;  // �ɿ�Mouse5ʱ��������
			return;
		}

		Vector::Vec3 localPos(localPlayer->fsPositionX, localPlayer->fsPositionY, localPlayer->fsPositionZ);

		// �����������Ѱ����ЧĿ��
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

			// ����Ѿ�������Ŀ�ֻ꣬����������Ŀ��ľ���
			if (lockedTarget == i) {
				closestDist = distance;
				closestEnemy = i;
				break;
			}

			// Ѱ������ĵ���
			if (distance < closestDist) {
				closestDist = distance;
				closestEnemy = i;
			}
		}

		// ���û���ҵ���ЧĿ�꣬��������������
		if (closestEnemy == -1) {
			lockedTarget = -1;
			return;
		}

		// ���������Ŀ��
		lockedTarget = closestEnemy;
		auto targetPlayer = EntityList::GetPlayer(lockedTarget);
		if (!targetPlayer) return;

		Vector::Vec3 enemyPos = targetPlayer->GetPos();

		// �ֶ�����λ�ò�ֵ
		float deltaX = enemyPos.x - localPos.x;
		float deltaY = enemyPos.y - localPos.y;
		float deltaZ = enemyPos.z - localPos.z;

		// ����Ŀ��Ƕȣ�Pitch��Yaw��
		float yaw = atan2f(deltaY, deltaX) * 180.0f / 3.14159f;
		float pitch = atan2f(-deltaZ, sqrtf(deltaX * deltaX + deltaY * deltaY)) * 180.0f / 3.14159f;

		// ֱ�������ӽǣ���ʹ��ƽ��
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