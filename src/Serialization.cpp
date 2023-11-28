#include "GrappleManager.h"
#include "Serialization.h"

#define GRAPPLEDATA_VERSION 1

void Serialization::RevertGrappleData(const F4SE::SerializationInterface* sifc)
{
	GrappleManager::WriteLocker lock(GrappleManager::GetSingleton()->mapLock);
	auto& map = GrappleManager::GetSingleton()->grappleMap;
	for (auto& [actorFormID, grappleData] : map) {
		RE::Actor* a = RE::TESForm::GetFormByID(actorFormID)->As<RE::Actor>();
		RE::TESObjectREFR* rope = (RE::TESObjectREFR*)RE::TESForm::GetFormByID(grappleData.ropeRefID);
		if (a) {
			lock.unlock();
			GrappleManager::GetSingleton()->DestroyGrapple(a);
			lock.lock();
		} else {
			if (rope) {
				if (!rope->IsDeleted()) {
					rope->MarkAsDeleted();
				}
			}
			map.erase(actorFormID);
		}
	}
	map.clear();
}

void Serialization::SaveGrappleData(const F4SE::SerializationInterface* sifc)
{
	GrappleManager::ReadLocker lock(GrappleManager::GetSingleton()->mapLock);
	sifc->OpenRecord('GPDT', GRAPPLEDATA_VERSION);
	auto& map = GrappleManager::GetSingleton()->grappleMap;

	uint32_t size = (uint32_t)map.size();
	sifc->WriteRecordData(&size, sizeof(size));

	for (auto& [actorFormID, grappleData] : map) {
		sifc->WriteRecordData(&actorFormID, sizeof(actorFormID));
		sifc->WriteRecordData(&grappleData.ropeRefID, sizeof(grappleData.ropeRefID));
		sifc->WriteRecordData(&grappleData.projRefID, sizeof(grappleData.projRefID));
		sifc->WriteRecordData(&grappleData.targetRefID, sizeof(grappleData.targetRefID));
		uint32_t length = (uint32_t)grappleData.targetNodeName.length();
		sifc->WriteRecordData(&length, sizeof(length));
		sifc->WriteRecordData(grappleData.targetNodeName.data(), length);
		sifc->WriteRecordData(&grappleData.translate, sizeof(grappleData.translate));
		sifc->WriteRecordData(&grappleData.velocity, sizeof(grappleData.velocity));
		sifc->WriteRecordData(&grappleData.timeLeft, sizeof(grappleData.timeLeft));
		sifc->WriteRecordData(&grappleData.pulling, sizeof(grappleData.pulling));
	}
}

void Serialization::LoadGrappleData(const F4SE::SerializationInterface* sifc)
{
	RevertGrappleData(sifc);

	GrappleManager::WriteLocker lock(GrappleManager::GetSingleton()->mapLock);
	auto& map = GrappleManager::GetSingleton()->grappleMap;
	uint32_t sig{ 0 }, ver{ 0 }, len{ 0 };
	while (sifc->GetNextRecordInfo(sig, ver, len)) {
		if (sig != 'GPDT' || ver != GRAPPLEDATA_VERSION) {
			continue;
		}

		uint32_t size{ 0 };
		sifc->ReadRecordData(&size, sizeof(size));

		for (uint32_t i = 0; i < size; i++) {
			uint32_t actorFormID{ 0 }, ropeRefID{ 0 }, projRefID{ 0 }, targetRefID{ 0 }, length{ 0 };
			RE::NiPoint3 translate{}, velocity{};
			std::string targetNodeName;
			float timeLeft{ 0.f };
			bool pulling{ false };
			sifc->ReadRecordData(&actorFormID, sizeof(actorFormID));
			sifc->ReadRecordData(&ropeRefID, sizeof(ropeRefID));
			sifc->ReadRecordData(&projRefID, sizeof(projRefID));
			sifc->ReadRecordData(&targetRefID, sizeof(targetRefID));
			sifc->ReadRecordData(&length, sizeof(length));
			targetNodeName.reserve(length);
			sifc->ReadRecordData(&targetNodeName, length);
			sifc->ReadRecordData(&translate, sizeof(translate));
			sifc->ReadRecordData(&velocity, sizeof(velocity));
			sifc->ReadRecordData(&timeLeft, sizeof(timeLeft));
			sifc->ReadRecordData(&pulling, sizeof(pulling));
			map.insert(std::pair<uint32_t, GrappleData>(actorFormID, GrappleData(ropeRefID, projRefID, targetRefID, targetNodeName, translate, velocity, timeLeft, pulling, true)));
		}
	}
}
