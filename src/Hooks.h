#pragma once

namespace Hooks {
	void InitializeHooks();
	void HookedUpdate(RE::ProcessLists* list, float deltaTime, bool instant);
	void HookedProcessImpacts(RE::Projectile* proj);
}
