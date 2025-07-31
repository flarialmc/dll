#pragma once

#include "PrimitiveMode.hpp"
#include <Utils/Utils.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

class Tessellator {
    Vec3<float> offset = Vec3<float>();
public:
    void begin(mce::PrimitiveMode vertexFormat = mce::PrimitiveMode::TriangleList, const int maxVertices = 0, const bool buildFaceData = false) {
        using func_t = void(__fastcall*)(Tessellator*, mce::PrimitiveMode, int, bool);
        static auto func = reinterpret_cast<func_t>(GET_SIG_ADDRESS("Tessellator::begin"));
        func(this, vertexFormat, maxVertices, buildFaceData);
    }

    void vertex(float x, float y, float z) {
        using func_t = void(__fastcall*)(Tessellator*, float, float, float);
        static auto func = reinterpret_cast<func_t>(GET_SIG_ADDRESS("Tessellator::vertex"));

        if (applyTransform) {
            glm::vec4 vec = transformMatrix * glm::vec4(x, y, z, 1.0f);
            func(this, vec.x + offset.x, vec.y + offset.y, vec.z + offset.z);
        }
        else {
            x = offset.x + x;
            y = offset.y + y;
            z = offset.z + z;
            func(this, x, y, z);
        }
    }

    void vertexUV(float x, float y, float z, float u, float v) {
        using func_t = void(__fastcall*)(Tessellator*, float, float, float, float, float);
        static auto func = reinterpret_cast<func_t>(GET_SIG_ADDRESS("Tessellator::vertexUV"));
        if (applyTransform) {
            glm::vec4 vec = transformMatrix * glm::vec4(x, y, z, 1.0f);
            func(this, vec.x + offset.x, vec.y + offset.y, vec.z + offset.z, u, v);
        }
        else {
            x = offset.x + x;
            y = offset.y + y;
            z = offset.z + z;
            func(this, x, y, z, u, v);
        }
    }

    void color(float r, float g, float b, float a) {
        using func_t = void(__fastcall*)(Tessellator*, float, float, float, float);
        static auto func = reinterpret_cast<func_t>(GET_SIG_ADDRESS("Tessellator::colorF"));
        func(this, r, g, b, a);
    }

    void setRotation(float angle, Vec3<float> pivot) {
        using func_t = void(__fastcall*)(Tessellator *, float, Vec3<float>);
        static auto func = reinterpret_cast<func_t>(GET_SIG_ADDRESS("Tessellator::setRotation"));
        func(this, angle, pivot);
    }

    void resetTransform(bool a1) {
        using func_t = void(__fastcall*)(Tessellator *, bool);
        static auto func = reinterpret_cast<func_t>(GET_SIG_ADDRESS("Tessellator::resetTransform"));
        func(this, a1);
    }

    void setPostScaleOffset(Vec3<float> Offset) {
        this->offset = Offset;
    }

    Vec3<float> getPostScaleOffset() {
        return offset;
    }

    static inline glm::mat4x4 transformMatrix;
    static inline bool applyTransform;
};