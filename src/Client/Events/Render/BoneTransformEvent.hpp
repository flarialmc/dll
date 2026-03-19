#pragma once

#include "../Event.hpp"
#include <cstdint>

// Fired by BoneTransformHook for every bone during actor rendering.
// Listeners can modify the rotation (degrees) before the matrix is computed.
class BoneTransformEvent : public Event {
public:
    uint64_t boneNameHash;  // FNV-1a hash of the bone's HashedString name
    float* rotation;        // pointer to mTransform.rotation [x, y, z] in degrees (mutable)

    BoneTransformEvent(uint64_t hash, float* rot)
        : boneNameHash(hash), rotation(rot) {}
};
