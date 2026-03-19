#pragma once

#include "Packet.hpp"
#include "Utils/VersionUtils.hpp"
#include <string>
#include <optional>

// =============================================================================
// Version 21.130+ (PayloadPacket style)
// =============================================================================

struct ModalFormRequestPacketPayload_v21130 {
    uint32_t mFormId;
    std::string mFormJSON;
};

using ModalFormRequestPacket_v21130 = PayloadPacket<ModalFormRequestPacketPayload_v21130>;

// =============================================================================
// Pre-21.130 (direct Packet inheritance)
// =============================================================================

class ModalFormRequestPacket : public Packet {
public:
    static inline auto ID = MinecraftPacketIds::ShowModalForm;

    unsigned int mFormId;   // this+0x30
    std::string mFormJSON;  // this+0x38
};

// =============================================================================
// Version-agnostic view for reading request packets :3
// =============================================================================

struct ModalFormRequestView {
    uint32_t formId;
    std::string formJSON;

    explicit ModalFormRequestView(void* packetPtr) {
        if (VersionUtils::checkAboveOrEqual(21, 130)) {
            auto* p = static_cast<ModalFormRequestPacket_v21130*>(packetPtr);
            this->formId = p->mFormId;
            this->formJSON = p->mFormJSON;
        } else {
            auto* p = static_cast<ModalFormRequestPacket*>(packetPtr);
            this->formId = p->mFormId;
            this->formJSON = p->mFormJSON;
        }
    }
};

inline std::optional<ModalFormRequestView> getModalFormRequest(void* packetPtr) {
    try {
        return ModalFormRequestView(packetPtr);
    } catch (...) {
        return std::nullopt;
    }
}
