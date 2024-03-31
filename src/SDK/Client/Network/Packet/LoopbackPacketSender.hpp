#pragma once

#include "Packet.hpp"
#include "../Raknet/RaknetConnector.hpp"

class LoopbackPacketSender {
public:
    BUILD_ACCESS(this, class NetworkSystem*, networkSystem, GET_OFFSET("LoopbackPacketSender::networkSystem"));

    virtual ~LoopbackPacketSender() = 0;

    virtual void send(Packet *packet);

    virtual void sendToServer(Packet *packet);
};

class NetworkSystem {
public:
    BUILD_ACCESS(this, class RemoteConnectorComposite*, remoteConnectorComposite, GET_OFFSET("NetworkSystem::remoteConnectorComposite"));
};

class RemoteConnectorComposite {
public:
    BUILD_ACCESS(this, class RaknetConnector*, rakNetConnector, GET_OFFSET("RemoteConnectorComposite::rakNetConnector"));
};