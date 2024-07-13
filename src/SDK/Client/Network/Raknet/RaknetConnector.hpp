#pragma once


#include "RakPeer.h"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../../Utils/Versions/WinrtUtils.hpp"
#include <string>
#include <vector>
#include <libhat/Access.hpp>


class RaknetConnector {
public:
    BUILD_ACCESS(this, std::string, JoinedIp, GET_OFFSET("RaknetConnector::JoinedIp"));
    BUILD_ACCESS(this, RakPeer*, peer, GET_OFFSET("RaknetConnector::getPeer"));
};