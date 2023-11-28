#include "Utils.h"

REL::Relocation<uint32_t*> Utils::ptr_invalidhandle{ REL::ID(888641) };
REL::Relocation<float*> Utils::ptr_engineTime{ REL::ID(599343) };
REL::Relocation<float*> Utils::ptr_deltaTime{ REL::ID(1013228) };

RE::TESForm* Utils::GetFormFromMod(std::string modname, uint32_t formid) {
	if (!modname.length() || !formid)
		return nullptr;
	return RE::TESDataHandler::GetSingleton()->LookupForm(formid, modname);
}

RE::NiPoint3 Utils::GetProjectileNode(RE::Actor* a, RE::PlayerCamera* pcam)
{
	if (a && a->Get3D()) {
		RE::NiNode* endPoint = (RE::NiNode*)a->Get3D()->GetObjectByName("ProjectileNode");
		if (!endPoint) {
			endPoint = (RE::NiNode*)a->Get3D()->GetObjectByName("Weapon");
		}
		RE::NiPoint3 newPos = endPoint->world.translate;
		if (a->Get3D(true) == a->Get3D()) {
			RE::NiNode* camera = (RE::NiNode*)a->Get3D()->GetObjectByName("Camera");
			newPos = newPos + pcam->cameraRoot->world.translate - camera->world.translate;
		}
		return newPos;
	}
	return RE::NiPoint3();
}

void Utils::MoveBendableSpline(RE::TESObjectREFR* rope, RE::NiPoint3 start, RE::NiPoint3 end, RE::TESObjectCELL* cell, RE::TESWorldSpace* world, bool useQueue)
{
	RE::TaskQueueInterface* queue = RE::TaskQueueInterface::GetSingleton();

	uint32_t nullHandle = *ptr_invalidhandle;
	RE::NiPoint3 rot;
	RE::NiPoint3 ropePos = (start + end) / 2.f;
	MoveRefrToPosition(rope, &nullHandle, cell, world, &ropePos, &rot);

	float thickness = 1.0f;
	float slack = 0.f;
	RE::ExtraBendableSplineParams* splineParams = (RE::ExtraBendableSplineParams*)rope->extraList->GetByType(RE::EXTRA_DATA_TYPE::kBendableSplineParams);
	if (splineParams) {
		thickness = splineParams->thickness;
	}
	RE::NiPoint3 offset = (start - end) / 2.f;
	rope->extraList->SetBendableSplineInfo(&thickness, &slack, &offset);

	if (useQueue) {
		queue->QueueRebuildBendableSpline(rope, true, nullptr);
	} else {
		rope->RebuildBendableSpline(false, nullptr);
	}
}

void Utils::MoveRefrToPosition(RE::TESObjectREFR* source, uint32_t* pTargetHandle, RE::TESObjectCELL* parentCell, RE::TESWorldSpace* worldSpace, RE::NiPoint3* position, RE::NiPoint3* rotation)
{
	using func_t = decltype(&Utils::MoveRefrToPosition);
	REL::Relocation<func_t> func{ REL::ID(1332434) };
	return func(source, pTargetHandle, parentCell, worldSpace, position, rotation);
}

RE::TESObjectREFR* Utils::PlaceAtMe_Native(RE::BSScript::IVirtualMachine* vm, uint32_t stackId, RE::TESObjectREFR** target, RE::TESForm* form, int32_t count, bool bForcePersist, bool bInitiallyDisabled, bool bDeleteWhenAble)
{
	using func_t = decltype(&Utils::PlaceAtMe_Native);
	REL::Relocation<func_t> func{ REL::ID(984532) };
	return func(vm, stackId, target, form, count, bForcePersist, bInitiallyDisabled, bDeleteWhenAble);
}

bool Utils::PlaySound(RE::BGSSoundDescriptorForm* sndr, RE::NiPoint3 pos, RE::NiAVObject* node)
{
	typedef bool* func_t(Unk, RE::BGSSoundDescriptorForm*, RE::NiPoint3, RE::NiAVObject*);
	REL::Relocation<func_t> func{ REL::ID(376497) };
	Unk u;
	return func(u, sndr, pos, node);
}

char Utils::tempbuf[512] = { 0 };
char* Utils::_MESSAGE(const char* fmt, ...) {
	va_list args;

	va_start(args, fmt);
	vsnprintf(tempbuf, sizeof(tempbuf), fmt, args);
	va_end(args);
	spdlog::log(spdlog::level::warn, tempbuf);

	return tempbuf;
}
