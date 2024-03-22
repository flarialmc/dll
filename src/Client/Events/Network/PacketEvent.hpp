#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Network/Packet/Packet.hpp"
#include <memory>

class PacketEvent : public Event, public Cancellable {

    Packet *packet;

public:

    Packet *getPacket() {
        return this->packet;
    }

    explicit PacketEvent(Packet *e) {
        this->packet = e;
    }
};