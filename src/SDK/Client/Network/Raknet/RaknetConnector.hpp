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

    int getPing() {
        static auto offset = GET_OFFSET("RaknetConnector::getPeer");
        if(WinrtUtils::check(20,60)) {
            return hat::member_at<RakPeer1_20_60*>(this, offset)->getPing();
        } else {
            return hat::member_at<RakPeer1_20_50*>(this, offset)->getPing();
        }
    }
};