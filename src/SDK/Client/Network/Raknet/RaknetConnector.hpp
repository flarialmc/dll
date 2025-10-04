#pragma once

#include <Utils/Memory/Memory.hpp>
#include <string>
#include <cstdint>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

// Forward declaration
class RakPeer;

class RaknetConnector {
public:
    BUILD_ACCESS(this, std::string, JoinedIp, GET_OFFSET("RaknetConnector::JoinedIp"));
    BUILD_ACCESS(this, std::string, RawIp, GET_OFFSET("RaknetConnector::rawIp"));
    BUILD_ACCESS(this, uint16_t, port, GET_OFFSET("RaknetConnector::port"))
    BUILD_ACCESS(this, RakPeer*, peer, GET_OFFSET("RaknetConnector::getPeer"));
};