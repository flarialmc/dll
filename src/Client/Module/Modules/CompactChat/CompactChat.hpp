#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "CompactChatListener.hpp"
#include "../../../Client.hpp"

class CompactChat : public Module {

public:


    CompactChat() : Module("Compact Chat", "Tired of spammers? Try this out!\nPuts repeated messages into 1 message.", "\\Flarial\\assets\\block_break_indicator.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        EventHandler::registerListener(new CompactChatListener("CompactChatListener", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("CompactChatListener");

        Module::onDisable();

    }

    void SettingsRender() override {

        

    }
};

