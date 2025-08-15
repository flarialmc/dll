#pragma once
#include <string>

class GuiMessage {
public:
    GuiMessage() = default;

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
