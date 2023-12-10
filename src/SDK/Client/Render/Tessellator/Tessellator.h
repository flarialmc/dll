#pragma once

#include "VertextFormat.h"
#include "MaterialPtr.h"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class ScreenContext;

class HashedString;

class Tessellator 
{
private:
	char pad_0000[0x184];
	Vec3<float> transformOffset; // 0x184

public:
	void addPostTransformOffset(int x, int y, int z) 
	{
		addPostTransformOffset((float)x, (float)y, (float)z);
	}
	void addPostTransformOffset(float x, float y, float z) 
	{
		this->transformOffset = this->transformOffset.add(Vec3<float>(x, y, z));
	};

	inline void begin(VertextFormat format, int reservedVerticies = 0) 
	{
		using tess_begin_t = void(__fastcall*)(Tessellator*, VertextFormat, bool);
		static tess_begin_t tess_begin = reinterpret_cast<tess_begin_t>(Memory::findSig("48 89 5C 24 ? 56 48 83 EC ? 80 B9 ? ? ? ? ? 45 0F B6"));
		tess_begin(this, format, reservedVerticies);
	}
	inline void vertex(float x, float y, float z) 
	{
		using tess_vertex_t = void(__fastcall*)(Tessellator*, float, float, float);
		static tess_vertex_t tess_vertex = reinterpret_cast<tess_vertex_t>(Memory::findSig("40 57 48 81 EC ? ? ? ? 0F 29 7C ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 8B 81"));
		tess_vertex(this, x, y, z);
	}
	inline void color(float r, float g, float b, float a) 
	{
		using tess_color_t = void(__fastcall*)(Tessellator*, float, float, float, float);
		static tess_color_t tess_color = reinterpret_cast<tess_color_t>(Memory::findSig("80 b9 ? ? ? ? ? 4c 8b c1 75"));
		tess_color(this, r, g, b, a);
	}
	inline void setRotation(float angle, Vec3<float> pivot)
	{
		using tess_setRotation_t = void(__fastcall*)(Tessellator*, float, Vec3<float>);
		static tess_setRotation_t tess_setRotation = reinterpret_cast<tess_setRotation_t>(Memory::findSig("48 89 5C 24 ? 57 48 81 EC ? ? ? ? F3 41 0F 10"));
		tess_setRotation(this, angle, pivot);
	}
	inline void resetTransform(bool a1) 
	{
		using tess_resetTransform_t = void(__fastcall*)(Tessellator*, bool);
		static tess_resetTransform_t tess_resetTransform = reinterpret_cast<tess_resetTransform_t>(Memory::findSig("40 53 48 81 ec ? ? ? ? c6 81 ? ? ? ? ? 48 8b d9 84 d2"));
		tess_resetTransform(this, a1);
	}
	inline void renderMeshImmediately(ScreenContext* ctx, MaterialPtr* material) 
	{
		using meshHelper_renderImm_t = void(__fastcall*)(ScreenContext*, Tessellator*, MaterialPtr*);
		static meshHelper_renderImm_t meshHelper_renderImm = reinterpret_cast<meshHelper_renderImm_t>(Memory::findSig("48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 49 8B F8 48 8B DA 48 8B F1 80 BA ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 44 24 ? 48 C7 44 24 ? ? ? ? ? 0F 28 44 24 ? 66 0F 7F 44 24 ?"));
		meshHelper_renderImm(ctx, this, material);
	}
};
