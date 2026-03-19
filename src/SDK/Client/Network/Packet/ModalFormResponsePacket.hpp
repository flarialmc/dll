#pragma once

#include "Packet.hpp"
#include "Utils/VersionUtils.hpp"
#include "SDK/Client/Util/json.hpp"
#include <optional>

enum class ModalFormCancelReason : uint8_t {
    UserClosed = 0,
    UserBusy = 1
};

// =============================================================================
// Version 21.130+ (PayloadPacket style)
// =============================================================================

struct ModalFormResponsePacketPayload_v21130 {
    uint32_t mFormId;
    std::optional<MinecraftJson::Value> mJSONResponse;
    std::optional<ModalFormCancelReason> mFormCancelReason;
};

using ModalFormResponsePacket_v21130 = PayloadPacket<ModalFormResponsePacketPayload_v21130>;

// =============================================================================
// Pre-21.130 (direct Packet inheritance)
// =============================================================================

class ModalFormResponsePacket : public Packet {
public:
    static inline auto ID = MinecraftPacketIds::ModalFormResponse;

    unsigned int                         mFormId{};           // this+0x30
    std::optional<MinecraftJson::Value>  mJSONResponse;     // this+0x38
    std::optional<ModalFormCancelReason> mFormCancelReason; // this+0x50
};

// =============================================================================
// Helper functions for crafting response packets :3
// =============================================================================

// Sets up a response packet with a button index (for simple form buttons)
inline void craftModalFormResponse(void* packetPtr, uint32_t formId, int buttonIndex) {
    MinecraftJson::Value val;
    memset(&val, 0, sizeof(val));
    val.mValue.mInt = buttonIndex;
    val.mType = MinecraftJson::ValueType::Int;

    if (VersionUtils::checkAboveOrEqual(21, 130)) {
        auto* p = static_cast<ModalFormResponsePacket_v21130*>(packetPtr);
        p->mFormId = formId;
        p->mJSONResponse = val;
        p->mFormCancelReason = std::nullopt;
    } else {
        auto* p = static_cast<ModalFormResponsePacket*>(packetPtr);
        p->mFormId = formId;
        p->mJSONResponse = val;
        p->mFormCancelReason = std::nullopt;
    }
}

// Sets up a cancelled response packet
inline void craftModalFormCancelled(void* packetPtr, uint32_t formId, ModalFormCancelReason reason = ModalFormCancelReason::UserClosed) {
    if (VersionUtils::checkAboveOrEqual(21, 130)) {
        auto* p = static_cast<ModalFormResponsePacket_v21130*>(packetPtr);
        p->mFormId = formId;
        p->mJSONResponse = std::nullopt;
        p->mFormCancelReason = reason;
    } else {
        auto* p = static_cast<ModalFormResponsePacket*>(packetPtr);
        p->mFormId = formId;
        p->mJSONResponse = std::nullopt;
        p->mFormCancelReason = reason;
    }
}
