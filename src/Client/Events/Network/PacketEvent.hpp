#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Network/Packet/Packet.hpp"
#include <memory>

class PacketEvent : public Event {

    std::shared_ptr<Packet> packet;

public:
    std::shared_ptr<Packet> getPacket() {
        return this->packet;
    }

    explicit PacketEvent(std::shared_ptr<Packet> e) {
        this->packet = e;
    }
};