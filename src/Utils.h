#pragma once
#include <Windows.h>

namespace Utils
{
	struct Unk
	{
		uint32_t unk00 = 0xFFFFFFFF;
		uint32_t unk04 = 0x0;
		uint32_t unk08 = 1;
	};

	RE::TESForm* GetFormFromMod(std::string modname, uint32_t formid);
	RE::NiPoint3 GetProjectileNode(RE::Actor* a, RE::PlayerCamera* pcam);
	void MoveBendableSpline(RE::TESObjectREFR* rope, RE::NiPoint3 start, RE::NiPoint3 end, RE::TESObjectCELL* cell, RE::TESWorldSpace* world, bool useQueue = true);
	void MoveRefrToPosition(RE::TESObjectREFR* source, uint32_t* pTargetHandle, RE::TESObjectCELL* parentCell, RE::TESWorldSpace* worldSpace, RE::NiPoint3* position, RE::NiPoint3* rotation);
	RE::TESObjectREFR* PlaceAtMe_Native(RE::BSScript::IVirtualMachine* vm, uint32_t stackId, RE::TESObjectREFR** target, RE::TESForm* form, int32_t count, bool bForcePersist, bool bInitiallyDisabled, bool bDeleteWhenAble);
	bool PlaySound(RE::BGSSoundDescriptorForm* sndr, RE::NiPoint3 pos, RE::NiAVObject* node);
	extern char tempbuf[512];
	char* _MESSAGE(const char* fmt, ...);

	template <class Ty>
	Ty SafeWrite64Function(uintptr_t addr, Ty data)
	{
		DWORD oldProtect;
		void* _d[2];
		memcpy(_d, &data, sizeof(data));
		size_t len = sizeof(_d[0]);

		VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
		Ty olddata;
		memset(&olddata, 0, sizeof(Ty));
		memcpy(&olddata, (void*)addr, len);
		memcpy((void*)addr, &_d[0], len);
		VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
		return olddata;
	}

	extern REL::Relocation<uint32_t*> ptr_invalidhandle;
	extern REL::Relocation<float*> ptr_engineTime;
	extern REL::Relocation<float*> ptr_deltaTime;
}

namespace MathUtils {
	inline RE::NiPoint3 ToDirectionVector(RE::NiMatrix3 mat)
	{
		return RE::NiPoint3(mat.entry[2].pt[0], mat.entry[2].pt[1], mat.entry[2].pt[2]);
	}

	inline RE::NiPoint3 ToUpVector(RE::NiMatrix3 mat)
	{
		return RE::NiPoint3(mat.entry[1].pt[0], mat.entry[1].pt[1], mat.entry[1].pt[2]);
	}

	inline RE::NiPoint3 ToRightVector(RE::NiMatrix3 mat)
	{
		return RE::NiPoint3(mat.entry[0].pt[0], mat.entry[0].pt[1], mat.entry[0].pt[2]);
	}

	inline float DotProduct(RE::NiPoint3 a, RE::NiPoint3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline float Length(RE::NiPoint3 p)
	{
		return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	}

	inline RE::NiPoint3 Normalize(RE::NiPoint3 p)
	{
		RE::NiPoint3 ret = p;
		float l = Length(p);
		if (l == 0) {
			ret.x = 1;
			ret.y = 0;
			ret.z = 0;
		} else {
			ret.x /= l;
			ret.y /= l;
			ret.z /= l;
		}
		return ret;
	}

	inline void SetMatrix33(float a, float b, float c, float d, float e, float f, float g, float h, float i, RE::NiMatrix3& mat)
	{
		mat.entry[0].pt[0] = a;
		mat.entry[0].pt[1] = b;
		mat.entry[0].pt[2] = c;
		mat.entry[1].pt[0] = d;
		mat.entry[1].pt[1] = e;
		mat.entry[1].pt[2] = f;
		mat.entry[2].pt[0] = g;
		mat.entry[2].pt[1] = h;
		mat.entry[2].pt[2] = i;
	}

	inline RE::NiMatrix3 Transpose(RE::NiMatrix3 mat)
	{
		RE::NiMatrix3 trans;
		float a = mat.entry[0].pt[0];
		float b = mat.entry[0].pt[1];
		float c = mat.entry[0].pt[2];
		float d = mat.entry[1].pt[0];
		float e = mat.entry[1].pt[1];
		float f = mat.entry[1].pt[2];
		float g = mat.entry[2].pt[0];
		float h = mat.entry[2].pt[1];
		float i = mat.entry[2].pt[2];
		SetMatrix33(a, d, g,
			b, e, h,
			c, f, i, trans);
		return trans;
	}

	//(Rotation Matrix)^-1 * (World pos - Local Origin)
	inline RE::NiPoint3 WorldToLocal(RE::NiPoint3 wpos, RE::NiPoint3 lorigin, RE::NiMatrix3 rot)
	{
		RE::NiPoint3 lpos = wpos - lorigin;
		RE::NiMatrix3 invrot = Transpose(rot);
		return invrot * lpos;
	}

	inline RE::NiPoint3 LocalToWorld(RE::NiPoint3 lpos, RE::NiPoint3 lorigin, RE::NiMatrix3 rot)
	{
		return rot * lpos + lorigin;
	}
}
