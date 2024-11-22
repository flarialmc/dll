#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "CompactChatListener.hpp"
#include "../../../Client.hpp"

class CompactChat : public Module {

public:


    CompactChat() : Module("Compact Chat", "Tired of spammers? Try this out!\nPuts repeated messages into 1 message.",
                           IDR_BLOCK_BREAK_INDICATOR_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new CompactChatListener("CompactChatListener", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("CompactChatListener");
        Module::onDisable();
    }
};

