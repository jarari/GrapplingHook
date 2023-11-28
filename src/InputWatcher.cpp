#include "Configs.h"
#include "Globals.h"
#include "GrappleManager.h"
#include "InputWatcher.h"
#include "Utils.h"
#include <Windows.h>
#include <Xinput.h>

std::unordered_map<uint64_t, InputWatcher::FnPerformInputProcessing> InputWatcher::fnHash;

uint32_t InputWatcher::GamepadMaskToKeycode(uint32_t keyMask)
{
	switch (keyMask) {
	case XINPUT_GAMEPAD_DPAD_UP:
		return kGamepadButtonOffset_DPAD_UP;
	case XINPUT_GAMEPAD_DPAD_DOWN:
		return kGamepadButtonOffset_DPAD_DOWN;
	case XINPUT_GAMEPAD_DPAD_LEFT:
		return kGamepadButtonOffset_DPAD_LEFT;
	case XINPUT_GAMEPAD_DPAD_RIGHT:
		return kGamepadButtonOffset_DPAD_RIGHT;
	case XINPUT_GAMEPAD_START:
		return kGamepadButtonOffset_START;
	case XINPUT_GAMEPAD_BACK:
		return kGamepadButtonOffset_BACK;
	case XINPUT_GAMEPAD_LEFT_THUMB:
		return kGamepadButtonOffset_LEFT_THUMB;
	case XINPUT_GAMEPAD_RIGHT_THUMB:
		return kGamepadButtonOffset_RIGHT_THUMB;
	case XINPUT_GAMEPAD_LEFT_SHOULDER:
		return kGamepadButtonOffset_LEFT_SHOULDER;
	case XINPUT_GAMEPAD_RIGHT_SHOULDER:
		return kGamepadButtonOffset_RIGHT_SHOULDER;
	case XINPUT_GAMEPAD_A:
		return kGamepadButtonOffset_A;
	case XINPUT_GAMEPAD_B:
		return kGamepadButtonOffset_B;
	case XINPUT_GAMEPAD_X:
		return kGamepadButtonOffset_X;
	case XINPUT_GAMEPAD_Y:
		return kGamepadButtonOffset_Y;
	case 0x9:
		return kGamepadButtonOffset_LT;
	case 0xA:
		return kGamepadButtonOffset_RT;
	default:
		return kMaxMacros;  // Invalid
	}
}

void InputWatcher::ProcessButtonEvent(RE::ButtonEvent* evn)
{
	if (evn->eventType != RE::INPUT_EVENT_TYPE::kButton) {
		if (evn->next)
			ProcessButtonEvent((RE::ButtonEvent*)evn->next);
		return;
	}

	uint32_t keyMask = evn->idCode;
	uint32_t keyCode;
	// Mouse
	if (evn->device.get() == RE::INPUT_DEVICE::kMouse) {
		keyCode = kMacro_NumKeyboardKeys + keyMask;
	}
	// Gamepad
	else if (evn->device.get() == RE::INPUT_DEVICE::kGamepad) {
		keyCode = GamepadMaskToKeycode(keyMask);
	}
	// Keyboard
	else
		keyCode = keyMask;

	// Valid scancode?
	if (keyCode >= kMaxMacros)
		return;

	if (keyCode == Configs::keyFireHook && evn->value > 0 && evn->heldDownSecs == 0.f) {
		GrappleManager* gm = GrappleManager::GetSingleton();
		auto grappleData = gm->grappleMap.find(Globals::p->formID);
		if (grappleData == gm->grappleMap.end()) {
			if (gm->PlayerCanGrapple()) {
				RE::NiPoint3 projPos = Utils::GetProjectileNode(Globals::p, Globals::pcam);
				RE::ObjectRefHandle ref = RE::TESDataHandler::GetSingleton()->CreateProjectileAtLocation(Globals::projForm, projPos, Globals::p->data.angle, Globals::p->parentCell, Globals::p->parentCell->worldSpace);
				if (ref.get()) {
					RE::Projectile* proj = ref.get()->As<RE::Projectile>();
					proj->shooter = Globals::p->GetHandle();
					Utils::PlaySound(Globals::fireSoundForm, Globals::pcam->cameraRoot->world.translate, Globals::pcam->cameraRoot.get());
					gm->StartGrapple(Globals::p, proj);
				}
			}
		} else {
			gm->DestroyGrapple(Globals::p);
		}
	}

	if (evn->next)
		ProcessButtonEvent((RE::ButtonEvent*)evn->next);
}

void InputWatcher::HookedPerformInputProcessing(const RE::InputEvent* a_queueHead)
{
	if (!RE::UI::GetSingleton()->menuMode && a_queueHead) {
		ProcessButtonEvent((RE::ButtonEvent*)a_queueHead);
	}
	FnPerformInputProcessing fn = fnHash.at(*(uint64_t*)this);
	if (fn) {
		(this->*fn)(a_queueHead);
	}
}

void InputWatcher::HookSink()
{
	uint64_t vtable = *(uint64_t*)this;
	auto it = fnHash.find(vtable);
	if (it == fnHash.end()) {
		FnPerformInputProcessing fn = Utils::SafeWrite64Function(vtable, &InputWatcher::HookedPerformInputProcessing);
		fnHash.insert(std::pair<uint64_t, FnPerformInputProcessing>(vtable, fn));
	}
}
