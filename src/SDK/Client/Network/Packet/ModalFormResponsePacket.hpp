#pragma once

#include "Packet.hpp"
#include "SDK/Client/Util/json.hpp"

enum class ModalFormCancelReason : signed char {
    UserClosed = 0x0,
    UserBusy = 0x1
};

class ModalFormResponsePacket : public Packet {
public:
    static inline int packetId = 0x65;

    unsigned int mFormId;
    std::optional<MinecraftJson::Value> mFormResponse;
    std::optional<ModalFormCancelReason> mCancelReason;
};
