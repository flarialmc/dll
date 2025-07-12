#pragma once

#include <Utils/Utils.hpp>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

class GuiMessage {
public: GuiMessage() = default;
private:
    char pad_0[8];
public:
    std::string msg;
private:
    char pad_1[40];
public:
    std::string ttsMsg;
    std::string author;
    std::string fullMsg;
private:
    char pad_2[40];
public:
    std::string xuid;
private:
    char pad_3[16];
};

class GuiData {
public:
    BUILD_ACCESS(this, Vec2<float>, ScreenSize, GET_OFFSET("GuiData::ScreenSize"));
    BUILD_ACCESS(this, Vec2<float>, ScreenSizeScaled, GET_OFFSET("GuiData::ScreenSizeScaled"));
    BUILD_ACCESS(this, float, GuiScale, GET_OFFSET("GuiData::GuiScale"));
    BUILD_ACCESS(this, float, screenResRounded, GET_OFFSET("GuiData::screenResRounded"));
    BUILD_ACCESS(this, float, sliderAmount, GET_OFFSET("GuiData::sliderAmount"));
    BUILD_ACCESS(this, float, scalingMultiplier, GET_OFFSET("GuiData::scalingMultiplier"));
    BUILD_ACCESS_REF(this, std::vector<GuiMessage>, GuiMessages, GET_OFFSET("GuiData::GuiMessages"));

    void displayClientMessage(const std::string &str);
};