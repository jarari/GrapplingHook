#pragma once
#include <F4SE/API.h>

namespace Serialization {
	void RevertGrappleData([[maybe_unused]] const F4SE::SerializationInterface* sifc);
	void SaveGrappleData(const F4SE::SerializationInterface* sifc);
	void LoadGrappleData(const F4SE::SerializationInterface* sifc);
}
