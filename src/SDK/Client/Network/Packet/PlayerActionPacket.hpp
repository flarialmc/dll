#pragma once

#include "Packet.hpp"
#include "PlaySoundPacket.hpp"  // For NetworkBlockPosition
#include "../../Level/HitResult.hpp"  // For BlockPos
#include "../../../../Utils/Utils.hpp"  // For Vec3

// Player action types - used for various player actions including block interactions
#pragma once

enum class PlayerActionType : int {
    Unknown                         = -1,
    StartDestroyBlock               = 0,
    AbortDestroyBlock               = 1,
    StopDestroyBlock                = 2,
    GetUpdatedBlock                 = 3,
    DropItem                        = 4,
    StartSleeping                   = 5,
    StopSleeping                    = 6,
    Respawn                         = 7,
    StartJump                       = 8,
    StartSprinting                  = 9,
    StopSprinting                   = 10,
    StartSneaking                   = 11,
    StopSneaking                    = 12,
    CreativeDestroyBlock            = 13,
    ChangeDimensionAck              = 14,
    StartGliding                    = 15,
    StopGliding                     = 16,
    DenyDestroyBlock                = 17,
    CrackBlock                      = 18,
    ChangeSkin                      = 19,
    DeprecatedUpdatedEnchantingSeed = 20,
    StartSwimming                   = 21,
    StopSwimming                    = 22,
    StartSpinAttack                 = 23,
    StopSpinAttack                  = 24,
    InteractWithBlock               = 25,
    PredictDestroyBlock             = 26,
    ContinueDestroyBlock            = 27,
    StartItemUseOn                  = 28,
    StopItemUseOn                   = 29,
    HandledTeleport                 = 30,
    MissedSwing                     = 31,
    StartCrawling                   = 32,
    StopCrawling                    = 33,
    StartFlying                     = 34,
    StopFlying                      = 35,
    DeprecatedClientAckServerData   = 36,
    StartUsingItem                  = 37,
    Count                           = 38,
};

// Payload structure for PlayerActionPacket
struct PlayerActionPacketPayload {
    NetworkBlockPosition mPos;          // 0x0 - Block position (12 bytes)
    int mResultPos_x;                   // 0xC - Result position X (padding/result)
    int mResultPos_y;                   // 0x10
    int mResultPos_z;                   // 0x14
    int mFace;                          // 0x18 - Block face (Down=0, Up=1, North=2, South=3, West=4, East=5)
    PlayerActionType mAction;           // 0x1C - Action type
    int64_t mRuntimeId;                 // 0x20 - Actor runtime ID
    bool mIsFromServerPlayerMovementSystem;  // 0x28

    PlayerActionPacketPayload() = default;

    PlayerActionPacketPayload(const BlockPos& pos, PlayerActionType action, int64_t runtimeId, int face = 0)
        : mAction(action), mRuntimeId(runtimeId), mFace(face), mIsFromServerPlayerMovementSystem(false) {
        mPos.position = Vec3<int>(pos.x, pos.y, pos.z);
        mResultPos_x = pos.x;
        mResultPos_y = pos.y;
        mResultPos_z = pos.z;
    }
};

// PlayerActionPacket - used for block interactions including note block attacks
class PlayerActionPacket : public PayloadPacket<PlayerActionPacketPayload> {
public:
    PlayerActionPacket() = default;

    PlayerActionPacket(const BlockPos& pos, PlayerActionType action, int64_t runtimeId, int face = 0)
        : PayloadPacket(pos, action, runtimeId, face) {}
};
