#include "Globals.h"
#include "Utils.h"

RE::PlayerCharacter* Globals::p = nullptr;
RE::PlayerCamera* Globals::pcam = nullptr;
RE::BGSProjectile* Globals::projForm = nullptr;
RE::BGSBendableSpline* Globals::ropeForm[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
RE::BGSSoundDescriptorForm* Globals::fireSoundForm = nullptr;
RE::SpellItem* Globals::expireSpell = nullptr;
RE::UI* Globals::ui = nullptr;

void Globals::InitializeGlobals() {
	p = RE::PlayerCharacter::GetSingleton();
	pcam = RE::PlayerCamera::GetSingleton();
	projForm = Utils::GetFormFromMod("GrapplingHook.esm", 0x800)->As<RE::BGSProjectile>();
	ropeForm[0] = RE::TESForm::GetFormByID(0x1D971)->As<RE::BGSBendableSpline>();
	ropeForm[1] = RE::TESForm::GetFormByID(0x35A21)->As<RE::BGSBendableSpline>();
	ropeForm[2] = RE::TESForm::GetFormByID(0x37213)->As<RE::BGSBendableSpline>();
	ropeForm[3] = RE::TESForm::GetFormByID(0x21F34)->As<RE::BGSBendableSpline>();
	ropeForm[4] = Utils::GetFormFromMod("GrapplingHook.esm", 0x804)->As<RE::BGSBendableSpline>();
	fireSoundForm = Utils::GetFormFromMod("GrapplingHook.esm", 0x803)->As<RE::BGSSoundDescriptorForm>();
	expireSpell = Utils::GetFormFromMod("GrapplingHook.esm", 0x801)->As<RE::SpellItem>();
	ui = RE::UI::GetSingleton();
}
