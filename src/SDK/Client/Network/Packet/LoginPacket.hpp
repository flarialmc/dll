#pragma once

#include "Packet.hpp"
#include "Utils/Utils.hpp"
#include <pointer.hpp>
#include "../../Util/json.hpp"



class WebToken {
public:
    // member variables
    // NOLINTBEGIN
    std::string mHeader;
    MinecraftJson::Value mHeaderInfo;
    std::string mData;
    MinecraftJson::Value mDataInfo;
    std::string mSignature;
};

class UnverifiedCertificate {
public:
    // member variables
    // NOLINTBEGIN
    WebToken const                           mRawToken;
    std::unique_ptr<UnverifiedCertificate>   mParentUnverifiedCertificate;
};

class Certificate {
public:
    // member variables
    // NOLINTBEGIN
    UnverifiedCertificate          mUnverifiedCertificate;
    std::unique_ptr<Certificate> mParentCertificate;
    bool                             mIsValid;
    bool                             mIsSelfSigned;
};
    // NOLINTEND

class GameServerToken {
public:
    // member variables
    // NOLINTBEGIN
    std::unique_ptr<Certificate >> mCertificate;
};

enum class SubClientId : char {
    PrimaryClient = 0,
    Client2 = 1,
    Client3 = 2,
    Client4 = 3,
    ExtraIdSlotStart = 100,
    EditorUI = 101,
};

class ConnectionRequest {
public:
    // member variables
    // NOLINTBEGIN
    std::unique_ptr<WebToken>                mRawToken;
    std::unique_ptr<UnverifiedCertificate>   mCertificateData;
    GameServerToken                          mGameServerToken;
    SubClientId                              mClientSubId;
};

class LoginPacket : public Packet {
public:
    // member variables
    // NOLINTBEGIN
    int                                    mClientNetworkVersion;
    std::unique_ptr<ConnectionRequest>     mConnectionRequest;
};