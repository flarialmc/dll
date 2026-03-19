#pragma once

#include <Utils/Utils.hpp>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <SDK/Client/Actor/EntityContext.hpp>
struct MoveInputState
{
    enum class Flag : int {
        SneakDown               = 0,
        SneakToggleDown         = 1,
        WantDownSlow            = 2,
        WantUpSlow              = 3,
        BlockSelectDown         = 4,
        AscendBlock             = 5,
        DescendBlock            = 6,
        JumpDown                = 7,
        SprintDown              = 8,
        UpLeft                  = 9,
        UpRight                 = 10,
        DownLeft                = 11,
        DownRight               = 12,
        Up                      = 13,
        Down                    = 14,
        Left                    = 15,
        Right                   = 16,
        Ascend                  = 17,
        Descend                 = 18,
        ChangeHeight            = 19,
        LookCenter              = 20,
        SneakInputCurrentlyDown = 21,
        SneakInputWasReleased   = 22,
        SneakInputWasPressed    = 23,
        JumpInputWasReleased    = 24,
        JumpInputWasPressed     = 25,
        JumpInputCurrentlyDown  = 26,
        Count                   = 27,
    };

public:
    // member variables
    // NOLINTBEGIN
    uint32_t                       mFlagValues;
    Vec2<float>                    mAnalogMoveVector;
    uint8_t                        mLookSlightDirField;
    uint8_t                        mLookNormalDirField;
    uint8_t                        mLookSmoothDirField;

    // Helper methods for flag access
    bool getFlag(Flag flag) const {
        return (mFlagValues & (1 << static_cast<int>(flag))) != 0;
    }

    void setFlag(Flag flag, bool value) {
        if (value)
            mFlagValues |= (1 << static_cast<int>(flag));
        else
            mFlagValues &= ~(1 << static_cast<int>(flag));
    }

    // Convenience accessors for directional input
    bool forward() const { return getFlag(Flag::Up); }
    void setForward(bool value) { setFlag(Flag::Up, value); }

    bool backward() const { return getFlag(Flag::Down); }
    void setBackward(bool value) { setFlag(Flag::Down, value); }

    bool left() const { return getFlag(Flag::Left); }
    void setLeft(bool value) { setFlag(Flag::Left, value); }

    bool right() const { return getFlag(Flag::Right); }
    void setRight(bool value) { setFlag(Flag::Right, value); }

    // Accessors for common flags with 'm' prefix naming
    bool mSneakDown() const { return getFlag(Flag::SneakDown); }
    void setMSneakDown(bool value) { setFlag(Flag::SneakDown, value); }

    bool mSprintDown() const { return getFlag(Flag::SprintDown); }
    void setMSprintDown(bool value) { setFlag(Flag::SprintDown, value); }

    bool mJumpDown() const { return getFlag(Flag::JumpDown); }
    void setMJumpDown(bool value) { setFlag(Flag::JumpDown, value); }
};
struct MoveInputComponent : IEntityComponent {
public:
    // MoveInputComponent inner types define
    enum class Flag : int {
        Sneaking                        = 0,
        Sprinting                       = 1,
        WantUp                          = 2,
        WantDown                        = 3,
        Jumping                         = 4,
        AutoJumpingInWater              = 5,
        MoveInputStateLocked            = 6,
        PersistSneak                    = 7,
        AutoJumpEnabled                 = 8,
        IsCameraRelativeMovementEnabled = 9,
        IsRotControlledByMoveDirection  = 10,
        Count                           = 11,
    };

public:
    // member variables
    // NOLINTBEGIN
    MoveInputState              mInputState;
    MoveInputState              mRawInputState;
    uint8_t                     mHoldAutoJumpInWaterTicks;
    Vec2<float>                 mMove;
    Vec2<float>                 mLookDelta;
    Vec2<float>                 mInteractDir;
    Vec3<float>                 mDisplacement;
    Vec3<float>                 mDisplacementDelta;
    Vec3<float>                 mCameraOrientation;
    uint16_t                    mFlagValues;
    std::array<bool, 2>         mIsPaddling;
    // NOLINTEND

    // Helper methods for flag access
    bool getFlag(Flag flag) const {
        return (mFlagValues & (1 << static_cast<int>(flag))) != 0;
    }

    void setFlag(Flag flag, bool value) {
        if (value)
            mFlagValues |= (1 << static_cast<int>(flag));
        else
            mFlagValues &= ~(1 << static_cast<int>(flag));
    }

    // Convenience accessors
    bool getSneaking() const { return getFlag(Flag::Sneaking); }
    void setSneaking(bool value) { setFlag(Flag::Sneaking, value); }

    bool getSprinting() const { return getFlag(Flag::Sprinting); }
    void setSprinting(bool value) { setFlag(Flag::Sprinting, value); }

    bool getJumping() const { return getFlag(Flag::Jumping); }
    void setJumping(bool value) { setFlag(Flag::Jumping, value); }
};

struct MoveInputStateOLD
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

struct MoveInputComponentOLD : IEntityComponent
{
	static constexpr hat::fixed_string type_name = "struct MoveInputComponent";
	MoveInputStateOLD mInputState;
	MoveInputStateOLD mRawInputState;
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

namespace entt
{
	template<>
	struct type_hash<MoveInputComponentOLD>
	{
		[[nodiscard]] static consteval id_type value() noexcept {
			constexpr auto name = MoveInputComponentOLD::type_name;
			return hashed_string::value(name.data(), name.size());
		}

		[[nodiscard]] consteval operator id_type() const noexcept {
			return value();
		}
	};
};