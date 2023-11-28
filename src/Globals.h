#pragma once

#ifndef HAVOKtoFO4
#define HAVOKtoFO4 69.99124f
#endif

namespace Globals {
	extern RE::PlayerCharacter* p;
	extern RE::PlayerCamera* pcam;
	extern RE::BGSProjectile* projForm;
	extern RE::BGSBendableSpline* ropeForm;
	extern RE::BGSSoundDescriptorForm* fireSoundForm;
	extern RE::SpellItem* expireSpell;
	extern RE::UI* ui;

	void InitializeGlobals();
}
