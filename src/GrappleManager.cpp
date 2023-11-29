#include "Configs.h"
#include "GrappleManager.h"
#include "Utils.h"
#include <Globals.h>

GrappleManager* GrappleManager::instance = nullptr;

void GrappleManager::OnHookImpact(RE::Projectile* proj)
{
	RE::Projectile::ImpactData& ipct = proj->impacts[0];
	if (proj->impacts.size() > 0 &&
		!ipct.processed) {
		RE::Actor* a = nullptr;
		if (proj->shooter.get().get()) {
			a = proj->shooter.get().get()->As<RE::Actor>();
		}
		if (a) {
			float dist = MathUtils::Length(ipct.location - a->data.location);
			if (dist < Configs::maxDistance) {
				RE::Actor* target = nullptr;
				if (ipct.collidee.get().get()) {
					target = ipct.collidee.get().get()->As<RE::Actor>();
				}
				if (target) {
					SetGrappleData(a, proj, target, target->GetClosestBone(proj->data.location, proj->data.angle));
				} else {
					SetGrappleData(a, proj, nullptr, nullptr);
				}
			} else {
				DestroyGrapple(a);
			}
		}
	}
}

void GrappleManager::OnUpdate(float deltaTime)
{
	WriteLocker lock(mapLock);
	float compensation = *Utils::ptr_deltaTime / 0.03333333f;
	for (auto& [actorFormID, grappleData] : grappleMap) {
		RE::Actor* a = RE::TESForm::GetFormByID(actorFormID)->As<RE::Actor>();
		RE::TESObjectREFR* rope = (RE::TESObjectREFR*)RE::TESForm::GetFormByID(grappleData.ropeRefID);
		RE::Projectile* proj = (RE::Projectile*)RE::TESForm::GetFormByID(grappleData.projRefID);
		RE::Actor* target = (RE::Actor*)RE::TESForm::GetFormByID(grappleData.targetRefID);
		if (a && (proj || (target && target->Get3D()))) {
			if (grappleData.timeLeft < 0) {
				lock.unlock();
				DestroyGrapple(a);
				lock.lock();
				continue;
			}
			RE::NiPoint3 endPos = RE::NiPoint3();
			if (proj) {
				endPos = proj->data.location;
			} else if (target) {
				RE::NiAVObject* node = target->Get3D()->GetObjectByName(grappleData.targetNodeName);
				if (!node) {
					lock.unlock();
					DestroyGrapple(a);
					lock.lock();
					continue;
				}
				endPos = MathUtils::LocalToWorld(grappleData.translate, node->world.translate, node->world.rotate);
			}
			float dist = MathUtils::Length(endPos - a->data.location - RE::NiPoint3(0, 0, 80.f));
			if (dist > Configs::maxDistance) {
				lock.unlock();
				DestroyGrapple(a);
				lock.lock();
				continue;
			}
			RE::NiPoint3 newPos = Utils::GetProjectileNode(a, Globals::pcam);
			Utils::MoveBendableSpline(rope, newPos, endPos, a->parentCell, a->parentCell->worldSpace);

			if (grappleData.pulling) {
				if (dist <= Configs::detachDistance) {
					lock.unlock();
					DestroyGrapple(a);
					lock.lock();
					continue;
				} else {
					HMODULE hAVF = GetModuleHandleA("ActorVelocityFramework.dll");
					if (hAVF) {
						if (a->currentProcess && a->currentProcess->middleHigh) {
							RE::NiPointer<RE::bhkCharacterController> con = a->currentProcess->middleHigh->charController;
							if (con.get()) {
								RE::NiPoint3 dir = endPos - a->data.location - RE::NiPoint3(0, 0, 80.f);
								dir.z = max(0, dir.z);
								RE::NiPoint3 dirNorm = MathUtils::Normalize(dir);
								RE::NiPoint3 velMod = con->initialVelocity;
								float curSpeed = MathUtils::DotProduct(dirNorm, velMod);
								float maxZSpeed = Configs::maxZSpeed * sqrt(compensation);
								float maxSpeed = Configs::maxSpeed * sqrt(compensation);
								float maxVelocity = Configs::maxVelocity;
								typedef void (*AddVelocity)(std::monostate, RE::Actor*, float, float, float);
								RE::NiPoint3 finalVel = RE::NiPoint3();
								AddVelocity fnAddVelocity = (AddVelocity)GetProcAddress(hAVF, "AddVelocity");
								if (!grappleData.requestSync) {
									if (curSpeed < maxSpeed) {
										float accel = min(maxSpeed - curSpeed, maxVelocity);
										finalVel = finalVel + RE::NiPoint3(dirNorm.x * accel, dirNorm.y * accel, 0);
									}
									if (velMod.z < maxZSpeed) {
										finalVel = finalVel + RE::NiPoint3(0, 0, min(dirNorm.z * maxVelocity, maxZSpeed - velMod.z));
									}
									if (MathUtils::Length(finalVel) > 0) {
										fnAddVelocity(std::monostate{}, a, finalVel.x, finalVel.y, finalVel.z);
									}
									grappleData.velocity = velMod + finalVel;
								} else {
									finalVel = grappleData.velocity - velMod;
									fnAddVelocity(std::monostate{}, a, finalVel.x, finalVel.y, finalVel.z);
									grappleData.requestSync = false;
								}
							}
						}
					}
				}
			}
			grappleData.timeLeft -= *Utils::ptr_deltaTime;
		} else {
			if (rope) {
				if (!rope->IsDeleted()) {
					rope->MarkAsDeleted();
				}
			}
			grappleMap.erase(actorFormID);
		}
	}
}

bool GrappleManager::PlayerCanGrapple()
{
	return *Utils::ptr_engineTime - lastGrappleTime > Configs::hookCooldown;
}

RE::TESObjectREFR* GrappleManager::CreateRope(RE::Actor* a, RE::Projectile* proj)
{
	RE::TESObjectREFR* rope = nullptr;
	RE::GameVM* gvm = RE::GameVM::GetSingleton();
	if (gvm) {
		rope = Utils::PlaceAtMe_Native(gvm->GetVM().get(), 0, (RE::TESObjectREFR**)&proj, Globals::ropeForm, 1, true, false, false);
		if (rope) {
			RE::NiPoint3 newPos = Utils::GetProjectileNode(a, Globals::pcam);

			float thickness = 3.0f;
			float slack = 0.01f;
			rope->extraList->SetBendableSplineInfo(&thickness, &slack);

			Utils::MoveBendableSpline(rope, newPos, proj->data.location, a->parentCell, a->parentCell->worldSpace);
		}
	}
	return rope;
}

void GrappleManager::StartGrapple(RE::Actor* a, RE::Projectile* proj)
{
	RE::TESObjectREFR* rope = CreateRope(a, proj);
	if (rope) {
		WriteLocker lock(mapLock);
		
		if (GrappleManager::grappleMap.find(a->formID) != GrappleManager::grappleMap.end()) {
			lock.unlock();
			DestroyGrapple(a);
			lock.lock();
		}
		GrappleManager::grappleMap.insert(std::pair<uint32_t, GrappleData>(a->formID, GrappleData(rope->formID, proj->formID)));
		lastGrappleTime = *Utils::ptr_engineTime;
	}
}

void GrappleManager::SetGrappleData(RE::Actor* a, RE::Projectile* proj, RE::Actor* target, RE::NiAVObject* targetNode)
{
	WriteLocker lock(mapLock);

	auto grappleData = GrappleManager::grappleMap.find(a->formID);
	if (grappleData != GrappleManager::grappleMap.end()) {
		if (target) {
			grappleData->second.targetRefID = target->formID;
			grappleData->second.targetNodeName = targetNode->name;
			grappleData->second.translate = MathUtils::WorldToLocal(proj->data.location, targetNode->world.translate, targetNode->world.rotate);
		} else {
			grappleData->second.projRefID = proj->formID;
		}
		grappleData->second.pulling = true;
	}
}

void GrappleManager::DestroyGrapple(RE::Actor* a)
{
	WriteLocker lock(mapLock);

	auto grappleData = GrappleManager::grappleMap.find(a->formID);
	if (grappleData != GrappleManager::grappleMap.end()) {
		RE::Projectile* proj = (RE::Projectile*)RE::TESForm::GetFormByID(grappleData->second.projRefID);
		if (proj && proj->As<RE::Projectile>()) {
			if (!proj->IsDeleted()) {
				proj->MarkAsDeleted();
			}
		}
		RE::TESObjectREFR* rope = (RE::TESObjectREFR*)RE::TESForm::GetFormByID(grappleData->second.ropeRefID);
		if (rope && rope->As<RE::TESObjectREFR>()) {
			if (!rope->IsDeleted()) {
				rope->MarkAsDeleted();
			}
		}
		grappleMap.erase(grappleData);
	}
}
