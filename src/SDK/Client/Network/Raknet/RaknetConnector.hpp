#pragma once


#include "RakPeer.h"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include <string>
#include <vector>


class RaknetConnector {
public:
    BUILD_ACCESS(this, std::string, JoinedIp, GET_OFFSET("RaknetConnector::JoinedIp"));
    BUILD_ACCESS(this, RakPeer *, peer, GET_OFFSET("RaknetConnector::getPeer"));
};