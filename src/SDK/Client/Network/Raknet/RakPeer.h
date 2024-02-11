#pragma once
#include <stdint.h>


//Lite Loader stuff begins here


struct NetworkIdentifier {
private:
	char pad[0xA0];
};

struct SystemAddress {
	char filler[17 * 8];
};

struct RakNetGUID {
	NetworkIdentifier* networkIdentifier;
	unsigned __int16 systemIndex;

public:
	bool operator!=(const RakNetGUID& a1) {
		return *this != a1;
	}
};

struct AddressOrGUID {
	RakNetGUID guid;
	SystemAddress addr;
};
// LiteLoader end

class RakPeer {
private:
	virtual void destructor();

public:
	virtual void InitializeConfiguration(std::unique_ptr<__int64>);
	virtual __int64 Startup(uint32_t, __int64 *, uint32_t, int);
	virtual bool InitializeSecurity(char const *, char const *, bool);
	virtual void DisableSecurity(void);
	virtual void AddToSecurityExceptionList(char const *);
	virtual void RemoveFromSecurityExceptionList(char const *);
	virtual bool IsInSecurityExceptionList(char const *);
	virtual void SetMaximumIncomingConnections(uint16_t);
	virtual uint32_t GetMaximumIncomingConnections(void);
	virtual uint16_t NumberOfConnections(void);
	virtual void SetIncomingPassword(char const *, int);
	virtual void GetIncomingPassword(char *, int *);
	virtual __int64 Connect(char const *, uint16_t, char const *, int, __int64 *, uint32_t, uint32_t, uint32_t, uint32_t);
	virtual __int64 ConnectWithSocket(char const *, uint16_t, char const *, int, __int64 *, __int64 *, uint32_t, uint32_t, uint32_t);
	virtual void Shutdown(uint32_t, unsigned char, __int64);
    virtual void SetApplicationHandshakeCompleted(AddressOrGUID);
	virtual bool IsActive(void);
	virtual bool GetConnectionList(__int64 *, uint16_t *);
	virtual uint32_t GetNextSendReceipt(void);
	virtual uint32_t IncrementNextSendReceipt(void);
	virtual uint32_t Send(__int64 const *, __int64, __int64, char, __int64, bool, uint32_t);
	virtual uint32_t Send(char const *, int, __int64, __int64, char, __int64, bool, uint32_t);
	virtual void SendLoopback(char const *, int);
	virtual uint32_t SendList(char const **, int const *, int, __int64, __int64, char, __int64, bool, uint32_t);
	virtual __int64 Receive(void);
	virtual void DeallocatePacket(__int64 *);
	virtual __int64 getAmplifier(void);
	virtual void CloseConnection(__int64, bool, unsigned char, __int64);
	virtual __int64 GetConnectionState(__int64);
	virtual void CancelConnectionAttempt(__int64);
	virtual int GetIndexFromSystemAddress(__int64);
	virtual SystemAddress GetSystemAddressFromIndex(uint32_t);
	virtual RakNetGUID GetGUIDFromIndex(uint32_t);
	virtual void GetSystemList(__int64 &, __int64 &);
	virtual void AddToBanList(char const *, uint32_t);
	virtual void RemoveFromBanList(char const *);
	virtual void ClearBanList(void);
	virtual bool IsBanned(char const *);
	virtual void SetLimitIPConnectionFrequency(bool);
	virtual bool Ping(char const *, uint16_t, bool, uint32_t);
	virtual void Ping(__int64);
	virtual void SendNatTraversalMessage(__int64);
	virtual int GetAveragePing(AddressOrGUID);
	virtual int GetLastPing(AddressOrGUID);
	virtual int GetLowestPing(AddressOrGUID);
	virtual void SetOccasionalPing(bool);
	virtual uint64_t GetClockDifferential(__int64);
	virtual void SetOfflinePingResponse(char const *, uint32_t);
	virtual void GetOfflinePingResponse(char **, uint32_t *);
	virtual __int64 GetInternalID(__int64, int);
	virtual void SetInternalID(__int64, int);
	virtual __int64 GetExternalID(__int64);
	virtual RakNetGUID GetMyGUID(void);
	virtual void resetMyGUID(void);
	virtual SystemAddress GetMyBoundAddress(int);
	virtual RakNetGUID GetGuidFromSystemAddress(SystemAddress);
	virtual SystemAddress GetSystemAddressFromGuid(RakNetGUID);
	virtual bool GetClientPublicKeyFromSystemAddress(SystemAddress, char *);
	virtual void SetTimeoutTime(uint32_t, __int64);
	virtual uint32_t GetTimeoutTime(__int64);
	virtual int GetMTUSize(__int64);
	virtual int GetNumberOfAdapters(void);
	virtual __int64 GetLocalAdapter(uint32_t);
	virtual int GetNumberOfAddresses(void);
	virtual const char *GetLocalIP(uint32_t);
	virtual bool IsLocalIP(char const *);
	virtual void AllowConnectionResponseIPMigration(bool);
	virtual bool AdvertiseSystem(char const *, uint16_t, char const *, int, uint32_t);
	virtual void SetSplitMessageProgressInterval(int);
	virtual __int64 GetSplitMessageProgressInterval(void);
	virtual void SetUnreliableTimeout(uint32_t);
	virtual void SendTTL(char const *, uint16_t, int, uint32_t);
	virtual void AttachPlugin(__int64 *);
	virtual void DetachPlugin(__int64 *);
	virtual void PushBackPacket(__int64 *, bool);
	virtual void ChangeSystemAddress(__int64, __int64 const &);
	virtual __int64 AllocatePacket(uint32_t);
	virtual __int64 GetSocket(__int64);
	virtual void GetSockets(__int64 &);
	virtual void ReleaseSockets(__int64 &);
	virtual void WriteOutOfBandHeader(__int64 *);
	virtual void SetUserUpdateThread(void (*)(__int64 *, void *), void *);
	virtual void SetIncomingDatagramEventHandler(bool (*)(__int64 *));
	virtual void ApplyNetworkSimulator(float, short, short);
	virtual void SetPerConnectionOutgoingBandwidthLimit(uint32_t);
	virtual bool IsNetworkSimulatorActive(void);
	virtual bool GetStatistics(uint32_t, __int64 *);
	virtual __int64 GetStatistics(__int64, __int64 *);
	virtual void GetStatisticsList(__int64 &, __int64 &, __int64 &);
	virtual uint32_t GetReceiveBufferSize(void);
	virtual bool RunUpdateCycle(__int64 &);
	virtual bool SendOutOfBand(char const *, uint16_t, char const *, uint32_t, uint32_t);

	int getAvgPing() {
		if (this->NumberOfConnections() == 0)
			return -1;

		RakNetGUID guid = this->GetGUIDFromIndex(0);
		SystemAddress sysAddr = this->GetSystemAddressFromIndex(0);

		AddressOrGUID aog;
		aog.guid = guid;
		aog.addr = sysAddr;

		return this->GetAveragePing(aog);
	}

	int getPing() {
		if (this->NumberOfConnections() == 0)
			return -1;

		RakNetGUID guid = this->GetGUIDFromIndex(0);
		SystemAddress sysAddr = this->GetSystemAddressFromIndex(0);

		AddressOrGUID aog;
		aog.guid = guid;
		aog.addr = sysAddr;

		return this->GetLastPing(aog);
	}
};