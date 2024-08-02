#pragma once

#include "../../../../Utils/Utils.hpp"
#include "Packet.hpp"
#include <string>

class NetworkBlockPosition {
public:
    Vec3<int> position;
};

class PlaySoundPacket : public ::Packet {
public:
    std::string mName;   // this+0x30
    Vec3<int> mPos;    // this+0x50
    float mVolume; // this+0x5C
    float mPitch;  // this+0x60

    PlaySoundPacket() = default;
};