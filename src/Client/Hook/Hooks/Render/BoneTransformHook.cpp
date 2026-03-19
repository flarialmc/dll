#include "BoneTransformHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/EventManager.hpp"
#include "../../../Events/Render/BoneTransformEvent.hpp"

BoneTransformHook::BoneTransformHook()
    : Hook("BoneTransformHook",
            GET_SIG_ADDRESS("BoneOrientation::computeBoneTransform")) {}

void BoneTransformHook::enableHook() {
    this->autoHook((void*)Callback, (void**)&funcOriginal);
}

void __fastcall BoneTransformHook::Callback(
    void* boneOrientation,
    void* offset,
    void* matrix)
{
    // Read bone name hash from BoneOrientation::mName (HashedString at +0x08)
    const uint64_t nameHash = *reinterpret_cast<uint64_t*>(
        reinterpret_cast<uint8_t*>(boneOrientation) + BoneOrientationOffsets::NAME);

    // Fast path: only dispatch if any module has registered interest in this bone.
    if (!BoneTransformHook::isWatchedBone(nameHash)) {
        return funcOriginal(boneOrientation, offset, matrix);
    }

    // Read rotation from BoneOrientation::mTransform.rotation (degrees)
    float* rotation = reinterpret_cast<float*>(
        reinterpret_cast<uint8_t*>(boneOrientation) + BoneOrientationOffsets::ROTATION);

    // Save original rotation
    const float origRx = rotation[0];
    const float origRy = rotation[1];
    const float origRz = rotation[2];

    // Fire event — listeners can modify rotation[] in place
    auto event = nes::make_holder<BoneTransformEvent>(nameHash, rotation);
    eventMgr.trigger(event);

    funcOriginal(boneOrientation, offset, matrix);

    // Restore original rotation so we don't permanently mutate the data
    rotation[0] = origRx;
    rotation[1] = origRy;
    rotation[2] = origRz;
}
