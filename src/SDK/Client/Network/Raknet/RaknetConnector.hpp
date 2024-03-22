#pragma once


#include "RakPeer.h"
#include <string>
#include <vector>


class RaknetConnector {
public:
    BUILD_ACCESS(this, std::string, JoinedIp, 0x438);


    virtual void Destructor();

    virtual std::vector<std::string *> getLocalIps();

    virtual std::string *getLocalIp();

    virtual uint16_t getPort();

    virtual __int64 getRefinedLocalIps();

    virtual __int64 *getConnectedGameInfo();

    virtual void setupNatPunch(bool connectToClient);

    virtual __int64 *getNatPunchInfo();

    virtual void startNatPunchingClient(std::string const &, short);

    virtual void addConnectionStateListener(__int64 *);

    virtual void removeConnectionStateListener(__int64 *);

    virtual bool isIPv4Supported();

    virtual bool isIPv6Supported();

    virtual uint16_t getIPv4Port();

    virtual uint16_t getIPv6Port();

    virtual void host(__int64 definition);

    virtual void connect(__int64 const &, __int64 const &);

    virtual void disconnect();

    virtual void tick();

    virtual void runEvents();

    virtual bool isServer();

    virtual bool isConnected(struct NetworkIdentifier const &);

    virtual __int64 closeNetworkConnection(struct NetworkIdentifier const &);

    virtual NetworkIdentifier *getNetworkIdentifier();

    virtual void setApplicationHandshakeCompleted(NetworkIdentifier const &);

    virtual RakPeer *getPeer();

    virtual RakPeer *getPeerConst();

    virtual void _onDisable();

    virtual void _onEnable();

};