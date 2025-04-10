#ifndef AIMBOT_H
#define AIMBOT_H

#include <Windows.h>
#include "EntityList.h"
#include "Hook.h"
#include "CheatData.h"
#include "Logs.h"

namespace Aimbot {
	class Aimbot {
	public:
		static void Attack(int delay);
		static void TriggerBot();
		static void LegitBot();
	};
}

#endif // AIMBOT_H