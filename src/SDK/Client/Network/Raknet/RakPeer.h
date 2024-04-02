#pragma once

#include <memory>
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"


//Lite Loader stuff begins here


struct NetworkIdentifier {
private:
    char pad[0xA0];
};

struct SystemAddress {
    char filler[17 * 8];
};

struct RakNetGUID {
    NetworkIdentifier *networkIdentifier;
    unsigned __int16 systemIndex;

public:
    bool operator!=(const RakNetGUID &a1) {
        return *this != a1;
    }
};

struct AddressOrGUID {
    RakNetGUID guid;
    SystemAddress addr;
};
// LiteLoader end

class RakPeer {
public:

    uint16_t NumberOfConnections(){
        static int off = GET_OFFSET("RakPeer::NumberOfConnections");
        return Memory::CallVFuncI<uint16_t>(off, this);
    };

    SystemAddress GetSystemAddressFromIndex(uint32_t index){
        static int off = GET_OFFSET("RakPeer::GetSystemAddressFromIndex");
        return Memory::CallVFuncI<SystemAddress, uint32_t>(off, this, index);
    };

    RakNetGUID GetGUIDFromIndex(uint32_t index){
        static int off = GET_OFFSET("RakPeer::GetGUIDFromIndex");
        return Memory::CallVFuncI<RakNetGUID, uint32_t>(off, this, index);
    };

    int GetAveragePing(AddressOrGUID connection){
        static int off = GET_OFFSET("RakPeer::GetAveragePing");
        return Memory::CallVFuncI<int, AddressOrGUID>(off, this, connection);
    };

    int GetLastPing(AddressOrGUID connection){
        static int off = GET_OFFSET("RakPeer::GetLastPing");
        return Memory::CallVFuncI<int, AddressOrGUID>(off, this, connection);
    };

    int getAvgPing() {
        if (this->NumberOfConnections() == 0)
            return -1;

        RakNetGUID guid = this->GetGUIDFromIndex(0);
        SystemAddress sysAddr = this->GetSystemAddressFromIndex(0);

        AddressOrGUID aog{};
        aog.guid = guid;
        aog.addr = sysAddr;

        return this->GetAveragePing(aog);
    }

    int getPing() {
        if (this->NumberOfConnections() == 0)
            return -1;

        RakNetGUID guid = this->GetGUIDFromIndex(0);
        SystemAddress sysAddr = this->GetSystemAddressFromIndex(0);

        AddressOrGUID aog{};
        aog.guid = guid;
        aog.addr = sysAddr;

        return this->GetLastPing(aog);
    }
};