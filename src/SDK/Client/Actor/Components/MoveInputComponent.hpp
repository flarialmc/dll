#pragma once

#include <Utils/Utils.hpp>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <SDK/Client/Actor/EntityContext.hpp>
struct MoveInputState
{
    bool mSneakDown;
    bool mSneakToggleDown;
    bool mWantDownSlow;
    bool mWantUpSlow;
    bool mBlockSelectDown;
    bool mAscendBlock;
    bool mDescendBlock;
    bool mJumpDown;
    bool mSprintDown;
    bool mUpLeft;
    bool mUpRight;
    bool mDownLeft;
    bool mDownRight;
    bool forward;
    bool backward;
    bool left;
    bool right;
    bool mAscend;
    bool mDescend;
    bool mChangeHeight;
    Vec2<float> mAnalogMoveVector;
    bool mLookCenter;
    u_char mLookSlightDirField;
    u_char mLookNormalDirField;
    u_char mLookSmoothDirField;
    bool mSneakInputCurrentlyDown;
    bool mSneakInputWasReleased;
    bool mSneakInputWasPressed;
    bool mJumpInputWasReleased;
    bool mJumpInputWasPressed;
    bool mJumpInputCurrentlyDown;

};
struct MoveInputComponent : IEntityComponent {

    MoveInputState mInputState;
    MoveInputState mRawInputState;
    std::chrono::steady_clock::time_point mHoldAutoJumpInWaterUntil;
    Vec2<float> mMove;
    Vec2<float> mLookDelta;
    Vec2<float> mInteractDir;
    Vec3<float> mDisplacement;
    Vec3<float> mDisplacementDelta;
    Vec3<float> mCameraOrientation;
    bool sneaking;
    bool sprinting;
    bool mWantUp;
    bool mWantDown;
    bool jumping;
    bool mAutoJumpingInWater;
    bool mMoveInputStateLocked;
    bool mPersistSneak;
    bool mAutoJumpEnabled;
    bool mIsCameraRelativeMovementEnabled;
    bool mIsRotControlledByMoveDirection;
    std::array<bool, 2> mIsPaddling;


};