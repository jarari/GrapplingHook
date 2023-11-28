#include "Globals.h"
#include "Utils.h"

RE::PlayerCharacter* Globals::p = nullptr;
RE::PlayerCamera* Globals::pcam = nullptr;
RE::BGSProjectile* Globals::projForm = nullptr;
RE::BGSBendableSpline* Globals::ropeForm = nullptr;
RE::BGSSoundDescriptorForm* Globals::fireSoundForm = nullptr;
RE::SpellItem* Globals::expireSpell = nullptr;
RE::UI* Globals::ui = nullptr;

void Globals::InitializeGlobals() {
	p = RE::PlayerCharacter::GetSingleton();
	pcam = RE::PlayerCamera::GetSingleton();
	projForm = Utils::GetFormFromMod("GrapplingHook.esm", 0x800)->As<RE::BGSProjectile>();
	ropeForm = RE::TESForm::GetFormByID(0x1D971)->As<RE::BGSBendableSpline>();
	fireSoundForm = Utils::GetFormFromMod("GrapplingHook.esm", 0x803)->As<RE::BGSSoundDescriptorForm>();
	expireSpell = Utils::GetFormFromMod("GrapplingHook.esm", 0x801)->As<RE::SpellItem>();
	ui = RE::UI::GetSingleton();
}
