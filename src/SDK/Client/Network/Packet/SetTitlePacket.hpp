#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "Packet.hpp"
#include <string>

enum SetTitleType {

    Clear,
    Reset_0,
    Title,
    Subtitle,
    Actionbar,
    Times,
    TitleTextObject,
    SubtitleTextObject,
    ActionbarTextObject,

};


class SetTitlePacket : public Packet {

public:
    SetTitlePacket();

    SetTitleType type;         // 0x30
    std::string text;              // 0x50
    float fadeInTime;   // 0x70
    float stayTime;  // 0x88
    float fadeOutTime;

    std::string xuid;
    std::string platformId;
};
