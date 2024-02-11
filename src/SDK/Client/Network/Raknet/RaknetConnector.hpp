#pragma once


#include "RakPeer.h"
#include <string>
#include <vector>


class RaknetConnector {
public:
	BUILD_ACCESS(this, std::string, JoinedIp, 0x438);


	virtual void Destructor();
	virtual std::vector<std::string*> getLocalIps(void);
	virtual std::string* getLocalIp(void);
	virtual uint16_t getPort(void);
	virtual __int64 getRefinedLocalIps(void);
	virtual __int64* getConnectedGameInfo(void);
	virtual void setupNatPunch(bool connectToClient);
	virtual __int64* getNatPunchInfo(void);
	virtual void startNatPunchingClient(std::string const&, short);
	virtual void addConnectionStateListener(__int64*);
	virtual void removeConnectionStateListener(__int64*);
	virtual bool isIPv4Supported(void);
	virtual bool isIPv6Supported(void);
	virtual uint16_t getIPv4Port(void);
	virtual uint16_t getIPv6Port(void);
	virtual void host(__int64 const definition);
	virtual void connect(__int64 const&, __int64 const&);
	virtual void disconnect(void);
	virtual void tick(void);
	virtual void runEvents(void);
	virtual bool isServer(void);
	virtual bool isConnected(struct NetworkIdentifier const&);
	virtual __int64 closeNetworkConnection(struct NetworkIdentifier const&);
	virtual NetworkIdentifier* getNetworkIdentifier(void);
	virtual void setApplicationHandshakeCompleted(NetworkIdentifier const &);
	virtual RakPeer* getPeer(void);
	virtual RakPeer* getPeerConst(void);
	virtual void _onDisable(void);
	virtual void _onEnable(void);

};