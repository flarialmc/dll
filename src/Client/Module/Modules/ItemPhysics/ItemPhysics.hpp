#pragma once

#include "ItemPhysicsListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"

class ItemPhysics : public Module {
public:
    ItemPhysicsListener* listener;

    ItemPhysics() : Module("ItemPhysics", "Changes rotation behavior of dropped items", IDR_ITEM_PHYSICS_PNG, "") {
        Module::setup();
    }

    void onEnable() override {
        listener = new ItemPhysicsListener("ItemPhysics", this);

        EventHandler::registerListener(listener);

        listener->onEnable();

        Module::onEnable();
    }

    void onDisable() override {
        listener->onDisable();

        EventHandler::unregisterListener("ItemPhysics");

        delete listener;

        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("speed") == nullptr)
            settings.addSetting("speed", 8.f);

        if (settings.getSettingByName<float>("xmul") == nullptr)
            settings.addSetting("xmul", 18.f);

        if (settings.getSettingByName<float>("ymul") == nullptr)
            settings.addSetting("ymul", 16.f);

        if (settings.getSettingByName<float>("zmul") == nullptr)
            settings.addSetting("zmul", 18.f);

        if (settings.getSettingByName<bool>("preserverots"))
            settings.addSetting("preserverots", false);

        if (settings.getSettingByName<bool>("smoothrots"))
            settings.addSetting("smoothrots", true);
    }
};
