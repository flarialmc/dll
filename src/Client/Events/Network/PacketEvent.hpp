#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Network/Packet/Packet.hpp"
#include <memory>

class PacketEvent : public Event, public Cancellable {

    std::shared_ptr<Packet> packet;
    void *packetHandlerDispatcher;
    void *networkIdentifier;
    void *netEventCallback;

public:

    Packet *getPacket() {
        return this->packet.get();
    }

    std::shared_ptr<Packet> getPacketShared() {
        return this->packet;
    }

    void *getPacketHandlerDispatcher() {
        return this->packetHandlerDispatcher;
    }

    void *getNetworkIdentifier() {
        return this->networkIdentifier;
    }

    void *getNetEventCallback() {
        return this->netEventCallback;
    }

    explicit PacketEvent(std::shared_ptr<Packet> pkt, void *dispatcher = nullptr, void *identifier = nullptr, void *callback = nullptr) {
        this->packet = pkt;
        this->packetHandlerDispatcher = dispatcher;
        this->networkIdentifier = identifier;
        this->netEventCallback = callback;
    }
};