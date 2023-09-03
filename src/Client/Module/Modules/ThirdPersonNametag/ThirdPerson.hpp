#pragma once

#include "../Module.hpp"
#include "ThirdPersonListener.hpp"
#include "../../../Events/EventHandler.hpp"


class ThirdPerson : public Module {

public:

    ThirdPerson() : Module("Nametag", "Shows your nametag for you while\nin 3rd person mode.", "\\Flarial\\assets\\nametag.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        std::vector<uint8_t> original;
        original.resize(6);
        memcpy(original.data(), (LPVOID)Memory::findSig("0F 84 ? ? ? ? 49 8B D7 48 8B CE E8 ? ? ? ? 84 C0"), 6);

        EventHandler::registerListener(new ThirdPersonListener("ThirdPersonListener", this, original));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    virtual void DefaultConfig() override {

    }

    void onDisable() override {
        ThirdPersonListener::unpatch();
        Module::onDisable();
    }

    void SettingsRender() override {



    }
};