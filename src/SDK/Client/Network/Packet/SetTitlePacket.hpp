#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "Packet.hpp"
#include <string>

enum SetTitleType {

    Clear = 0x0,
    Reset_0 = 0x1,
    Title = 0x2,
    Subtitle = 0x3,
    Actionbar = 0x4,
    Times = 0x5,
    TitleTextObject = 0x6,
    SubtitleTextObject = 0x7,
    ActionbarTextObject = 0x8,

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
