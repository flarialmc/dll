#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Network/Packet/Packet.hpp"
#include <memory>

class PacketSendEvent : public Event, public Cancellable {

    Packet *packet;

public:

    Packet *getPacket() {
        return this->packet;
    }

    explicit PacketSendEvent(Packet *e) {
        this->packet = e;
    }
};