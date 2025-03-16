#pragma once

#include "Packet.hpp"
#include "Utils/Utils.hpp"

class ModalFormRequestPacket : public Packet {
public:
    uint8_t mFormId;
    std::string mFormJSON;
};
