#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "CompactChatListener.hpp"
#include "../../../Client.hpp"

class CompactChat : public Module {

public:


    CompactChat() : Module("Compact Chat", "toes x3", "\\Flarial\\assets\\block_break_indicator.png", 'o') {

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

