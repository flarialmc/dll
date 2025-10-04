#pragma once

#include "RakPeer.h"
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
class RaknetConnector {
public:
    BUILD_ACCESS(this, std::string, JoinedIp, GET_OFFSET("RaknetConnector::JoinedIp"));
    BUILD_ACCESS(this, std::string, RawIp, GET_OFFSET("RaknetConnector::rawIp"));
    BUILD_ACCESS(this, uint16_t, port, GET_OFFSET("RaknetConnector::port"))
    BUILD_ACCESS(this, RakPeer*, peer, GET_OFFSET("RaknetConnector::getPeer"));

};