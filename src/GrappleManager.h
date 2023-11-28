#pragma once
#include "Configs.h"
#include <shared_mutex>

struct GrappleData {
	uint32_t ropeRefID = 0;
	uint32_t projRefID = 0;
	uint32_t targetRefID = 0;
	std::string targetNodeName = "";
	RE::NiPoint3 translate = RE::NiPoint3();
	RE::NiPoint3 velocity = RE::NiPoint3();
	float timeLeft = FLT_MAX;
	bool pulling = false;
	bool requestSync = false;
	GrappleData(uint32_t _rope, uint32_t _proj) {
		ropeRefID = _rope;
		projRefID = _proj;
		timeLeft = Configs::detachSeconds;
	}
	GrappleData(uint32_t _rope, uint32_t _proj, uint32_t _target, std::string _name, RE::NiPoint3 _trans, RE::NiPoint3 _vel, float _time, bool _pulling, bool _reqSync)
	{
		ropeRefID = _rope;
		projRefID = _proj;
		targetRefID = _target;
		targetNodeName = _name;
		translate = _trans;
		velocity = _vel;
		timeLeft = _time;
		pulling = _pulling;
		requestSync = _reqSync;
	}
};

class GrappleManager {
protected:
	static GrappleManager* instance;
	float lastGrappleTime = 0.f;
	RE::TESObjectREFR* CreateRope(RE::Actor* a, RE::Projectile* proj);

public:
	using SharedLock = std::shared_mutex;
	using ReadLocker = std::shared_lock<SharedLock>;
	using WriteLocker = std::unique_lock<SharedLock>;
	SharedLock mapLock;
	std::unordered_map<uint32_t, GrappleData> grappleMap;
	GrappleManager() = default;
	GrappleManager(GrappleManager&) = delete;
	void operator=(const GrappleManager&) = delete;
	static GrappleManager* GetSingleton()
	{
		if (!instance)
			instance = new GrappleManager();
		return instance;
	}
	void OnHookImpact(RE::Projectile* proj);
	void OnUpdate(float deltaTime);
	inline void Reset() {
		lastGrappleTime = 0.f;
	}
	bool PlayerCanGrapple();
	void StartGrapple(RE::Actor* a, RE::Projectile* proj);
	void SetGrappleData(RE::Actor* a, RE::Projectile* proj, RE::Actor* target, RE::NiAVObject* targetNode);
	void DestroyGrapple(RE::Actor* a);
};
