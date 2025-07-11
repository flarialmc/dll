#pragma once
#include "Packet.hpp"
#include "../../Util/mce.hpp"

class PlayerSkinPacket : public Packet {
public:
    mcUUID          mUUID;
    PlayerSkin      mSkin;
    std::string     mLocalizedNewSkinName;
    std::string     mLocalizedOldSkinName;
    bool            mTrusted;
};

class PlayerSkinPacket_1_21_90 : public Packet {
    public:
    mcUUID                  mUUID;
    PlayerSkin_1_21_90      mSkin;
    std::string             mLocalizedNewSkinName;
    std::string             mLocalizedOldSkinName;
    bool                    mTrusted;
};