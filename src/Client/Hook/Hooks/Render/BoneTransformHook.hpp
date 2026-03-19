#pragma once

#include "../Hook.hpp"
#include <unordered_set>

// BoneOrientation layout (from LeviLamina BoneOrientation.h, 224 bytes total)
// See: mc/world/actor/animation/BoneOrientation.h
//   +0x00  int mParentBoneIndex
//   +0x04  SkeletalHierarchyIndex mParentSkeletalHierarchyIndex
//   +0x08  HashedString mName               (48 bytes → ends at 0x38)
//   +0x38  Matrix mMatrix                   (64 bytes → ends at 0x78)
//   +0x78  BoneOrientationTransform mTransform {
//            +0x78  Vec3 position
//            +0x84  Vec3 rotation (degrees)
//            +0x90  Vec3 scale
//          }                                (36 bytes → ends at 0x9C)
//   +0x9C  BoneOrientationTransform mDefaultTransform (36 bytes → ends at 0xC0)
//   +0xC0  unique_ptr<Matrix> mLocalPreTransformMatrix
//   +0xC8  unique_ptr<ExpressionNode> mBoneBindingExpression
//   +0xD0  Vec3 mPivot                      (12 bytes)
//   +0xDC  BoneBindingMethod mBoneBindingMethod
//   +0xDD  BoneAnimationRelativeMode mRotationRelativeMode
//   +0xDE  bool mOverrideStackMatrix
struct BoneOrientationOffsets {
    static constexpr size_t NAME     = 0x08; // HashedString (hash at +0x00 within HS)
    static constexpr size_t ROTATION = 0x84; // mTransform.rotation (Vec3, degrees)
};

// Hooks BoneOrientation::computeBoneTransform — the shared function that both
// the paper doll (via ModelPart::generateBoneTransformMatrices) and the main
// player model (via the skeletal hierarchy batch processor) call to compute
// bone-space transform matrices from BoneOrientation rotation/position/scale.
//
// Modules register bone name hashes via watchBone()/unwatchBone(). Only bones
// in the watch set trigger event dispatch — all others take the fast path.
class BoneTransformHook : public Hook {
private:
    static inline std::unordered_set<uint64_t> sWatchedBones;

    static void __fastcall Callback(
        void* boneOrientation,
        void* offset,
        void* matrix
    );

public:
    typedef void(__fastcall *original)(void*, void*, void*);
    static inline original funcOriginal = nullptr;

    BoneTransformHook();
    void enableHook() override;

    static void watchBone(uint64_t hash)   { sWatchedBones.insert(hash); }
    static void unwatchBone(uint64_t hash) { sWatchedBones.erase(hash); }
    static bool isWatchedBone(uint64_t hash) { return sWatchedBones.contains(hash); }
};
