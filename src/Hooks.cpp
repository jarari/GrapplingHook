#include "Hooks.h"
#include "Globals.h"
#include "GrappleManager.h"
#include "InputWatcher.h"
#include "MenuWatcher.h"
#include "Utils.h"
#include "Configs.h"

uintptr_t RunActorUpdatesOrig;
uintptr_t ProcessImpactsOrig;

void Hooks::InitializeHooks()
{
	F4SE::Trampoline& trampoline = F4SE::GetTrampoline();
	RunActorUpdatesOrig = trampoline.write_call<5>(REL::Relocation<uintptr_t>{ REL::ID(556439), 0x17 }.address(), &Hooks::HookedUpdate);
	((InputWatcher*)((uint64_t)Globals::pcam + 0x38))->HookSink();
	Globals::ui->GetEventSource<RE::MenuOpenCloseEvent>()->RegisterSink(new MenuWatcher());
	uintptr_t arrowProj_vtbl = RE::ArrowProjectile::VTABLE[0].address();
	ProcessImpactsOrig = (uintptr_t)Utils::SafeWrite64Function(arrowProj_vtbl + 0x680, &Hooks::HookedProcessImpacts);
}

void Hooks::HookedUpdate(RE::ProcessLists* list, float deltaTime, bool instant)
{
	GrappleManager::GetSingleton()->OnUpdate(deltaTime);

	typedef void (*FnUpdate)(RE::ProcessLists*, float, bool);
	FnUpdate fn = (FnUpdate)RunActorUpdatesOrig;
	if (fn)
		(*fn)(list, deltaTime, instant);
}

void Hooks::HookedProcessImpacts(RE::Projectile* proj)
{
	RE::BGSProjectile* projBase = (RE::BGSProjectile*)proj->GetObjectReference();
	if (projBase == Globals::projForm) {
		GrappleManager::GetSingleton()->OnHookImpact(proj);
	}

	typedef void (*FnProcessImpacts)(RE::Projectile*);
	FnProcessImpacts fn = (FnProcessImpacts)ProcessImpactsOrig;
	if (fn)
		(*fn)(proj);
}
