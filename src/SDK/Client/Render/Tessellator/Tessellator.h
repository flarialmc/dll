#pragma once

#include "VertextFormat.h"
#include "MaterialPtr.h"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class ScreenContext;

class HashedString;

class Tessellator {
private:
    char pad_0000[0x184]{}; // this is not mutliversion
    Vec3<float> transformOffset; // 0x184

public:
    void addPostTransformOffset(int x, int y, int z) {
        addPostTransformOffset((float) x, (float) y, (float) z);
    }

    void addPostTransformOffset(float x, float y, float z) {
        this->transformOffset = this->transformOffset.add(Vec3<float>(x, y, z));
    };

    inline void begin(VertextFormat format, int reservedVerticies = 0) {
        using tess_begin_t = void (__fastcall *)(Tessellator *, VertextFormat, bool);
        static auto tess_begin = reinterpret_cast<tess_begin_t>(Memory::findSig(GET_SIG("Tessellator::begin")));
        tess_begin(this, format, reservedVerticies);
    }

    inline void vertex(float x, float y, float z) {
        using tess_vertex_t = void (__fastcall *)(Tessellator *, float, float, float);
        static auto tess_vertex = reinterpret_cast<tess_vertex_t>(Memory::findSig(GET_SIG("Tessellator::vertex")));
        tess_vertex(this, x, y, z);
    }

    inline void color(float r, float g, float b, float a) {
        using tess_color_t = void (__fastcall *)(Tessellator *, float, float, float, float);
        static auto tess_color = reinterpret_cast<tess_color_t>(Memory::findSig(GET_SIG("Tessellator::colorF")));
        tess_color(this, r, g, b, a);
    }

    inline void setRotation(float angle, Vec3<float> pivot) {
        using tess_setRotation_t = void (__fastcall *)(Tessellator *, float, Vec3<float>);
        static auto tess_setRotation = reinterpret_cast<tess_setRotation_t>(Memory::findSig(GET_SIG("Tessellator::setRotation")));
        tess_setRotation(this, angle, pivot);
    }

    inline void resetTransform(bool a1) {
        using tess_resetTransform_t = void (__fastcall *)(Tessellator *, bool);
        static auto tess_resetTransform = reinterpret_cast<tess_resetTransform_t>(Memory::findSig(GET_SIG("Tessellator::resetTransform")));
        tess_resetTransform(this, a1);
    }

    inline void renderMeshImmediately(ScreenContext *ctx, MaterialPtr *material) {
        using meshHelper_renderImm_t = void (__fastcall *)(ScreenContext *, Tessellator *, MaterialPtr *);
        static auto meshHelper_renderImm = reinterpret_cast<meshHelper_renderImm_t>(Memory::findSig(GET_SIG("MeshHelper::renderImmediately")));
        meshHelper_renderImm(ctx, this, material);
    }
};
