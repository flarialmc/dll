#pragma once

#include "Packet.hpp"
#include "../Raknet/RaknetConnector.hpp"

class LoopbackPacketSender {
public:
    BUILD_ACCESS(this, class NetworkSystem*, networkSystem, 0x20);

    virtual ~LoopbackPacketSender();
    virtual void send(Packet* packet);
    virtual void sendToServer(Packet* packet);
};

class NetworkSystem {
public:
    BUILD_ACCESS(this, class RemoteConnectorComposite*, remoteConnectorComposite, 0x80);
};

class RemoteConnectorComposite {
public:
    BUILD_ACCESS(this, class RaknetConnector*, rakNetConnector, 0x60);
};
