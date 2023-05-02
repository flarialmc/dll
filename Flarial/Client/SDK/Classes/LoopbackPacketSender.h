#pragma once
#include "Packet.h"
#include "Packets/TextPacket.h"
#include "Packets/CommandRequestPacket.h"

class RaknetInstance {
public:
	auto getServerStaticIp(void) -> std::string {
		if (this->serverIp.length() <= 0)
			return "";

		if ((uintptr_t*)((uintptr_t)(this) + 0x458) != nullptr)
			if (Utils::isValidDomain((char*)((uintptr_t)(this) + 0x458)))
				return std::string((const char*)((uintptr_t)(this) + 0x458));
			else if ((char**)((uintptr_t)(this) + 0x458) != nullptr)
				return std::string((*(char**)((uintptr_t)(this) + 0x458)));

		return "";
	};
public:
	BUILD_ACCESS(this, std::string, serverName, 0x530);
	BUILD_ACCESS(this, std::string, serverIp, 0x438);
public:
	AS_FIELD(std::string, serverStaticIp, getServerStaticIp);
private:
	virtual ~RaknetInstance(void);
public:
	virtual auto getLocalIps(void) -> std::vector<std::string>;
	virtual auto getPort(void) -> unsigned __int16;
	virtual auto getRefinedLocalIps(void) -> std::vector<__int64*>;
	virtual auto getConnectedGameInfo(void) -> class GameConnectionInfo*;
	virtual auto setupNatPunch(bool) -> void;
	virtual auto getNatPunchInfo(void) -> class NatPunchInfo*;
	virtual auto startNatPunchingClient(std::string const&, unsigned short) -> void;
	virtual auto addConnectionStateListener(class ConnectionStateListener*) -> void;
	virtual auto removeConnectionStateListener(class ConnectionStateListener*) -> void;
	virtual auto isIPv4Supported(void) -> bool;
	virtual auto isIPv6Supported(void) -> bool;
	virtual auto getIPv4Port(void) -> unsigned __int16;
	virtual auto getIPv6Port(void) -> unsigned __int16;
	virtual auto host(class ConnectionDefinition*) -> bool;
	virtual auto connect(class GameConnectionInfo*, class GameConnectionInfo*) -> bool;
	virtual auto disconnect(void) -> void;
	virtual auto tick(void) -> void;
	virtual auto runEvents(void) -> void;
	virtual auto isServer(void) -> bool;
	virtual auto closeNetworkConnection(class NetworkConnection*) -> void;
	virtual auto getTransportIdentifier(void) -> unsigned __int64;
};

class Raknet {
public:
	BUILD_ACCESS(this, RaknetInstance*, instance, 0x8);
};

class NetworkHandler {
public:
	BUILD_ACCESS(this, Raknet*, raknet, 0x50);
};

class LoopbackPacketSender {
public:
	BUILD_ACCESS(this, NetworkHandler*, networkHandler, 0x10);
private:
	virtual ~LoopbackPacketSender(void);
public:
	virtual auto send(class Packet*) -> void;
	virtual auto sendToServer(class Packet*) -> void;
	virtual auto sendToClient(class NetworkIdentifier const&, class Packet const&, enum SubClientId) -> void;
	virtual auto sendToClient(class UserEntityIdentifierComponent*, class Packet*) -> void;
	virtual auto sendToClients(std::vector<class NetworkIdentifierWithSubId, class std::allocator<class NetworkIdentifierWithSubId>> const&, class Packet const&) -> void;
	virtual auto sendBroadcast(class NetworkIdentifier*, enum SubClientId, class Packet*) -> void;
	virtual auto sendBroadcast(class Packet*) -> void;
	virtual auto flush(class NetworkIdentifier const&, class std::function<void(void)>&&) -> void;
};