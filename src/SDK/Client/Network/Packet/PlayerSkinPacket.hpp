#pragma once
#include "Packet.hpp"
#include "../../Util/mce.hpp"

class PlayerSkinPacket : public Packet {
public:
    mcUUID          mUUID;
    PlayerSkin      mSkin;
    std::string     mLocalizedNewSkinName;
    std::string     mLocalizedOldSkinName;
};
