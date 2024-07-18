#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../../SDK//Client/Network/Packet/TextPacket.hpp"
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Client.hpp"
#include <variant>
#include <utility>
#include <windows.h>

class CompactChatListener : public Listener {

public:
    Module *module;
    static inline std::vector<std::pair<std::string, int>> dups = {};
    static inline std::string prev;
    bool prevPacketSent = false;
    
    void onDrawText(DrawTextEvent &event) override {
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                size_t pos = event.getText()->find(CompactChatListener::dups.back().first);
                if (pos != std::string::npos) {
                    std::string newtxt = *event.getText();
                    newtxt.replace(pos, CompactChatListener::dups.back().first.length(), std::format("{} [x{}]", CompactChatListener::dups.back().first, CompactChatListener::dups.back().second));
                    *event.getText() = newtxt;
                }
            }
        }
    }

    void onPacketReceive(PacketEvent &event) override {
        if (module->isEnabled()) {
            if (event.getPacket()->getId() == MinecraftPacketIds::Text) {
                std::string msg = reinterpret_cast<TextPacket *>(event.getPacket())->message;

                if (!prevPacketSent) {
                    prevPacketSent = true;
                    return;
                }

                if (msg == prev) {
                    auto last = dups.back();
                    if (last.first == msg) last.second++;
                    else dups.emplace_back(msg, 1);

                    //std::cout << "Duplicate message detected yes" << std::endl;
                }

                if (dups.size() > 100) dups.erase(dups.begin());

                prevPacketSent = false;

                prev = msg;
            }
        }
    }

public:
    explicit CompactChatListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;

        dups.resize(420);
    }
};

