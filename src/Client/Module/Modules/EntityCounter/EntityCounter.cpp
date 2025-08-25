#include "EntityCounter.hpp"
#include "Events/EventManager.hpp"

void EntityCounter::onEnable() {
    Listen(this, RenderEvent, &EntityCounter::onRender);
    Listen(this, SetupAndRenderEvent, &EntityCounter::onSetupAndRender);
    Module::onEnable();
}

void EntityCounter::onDisable() {
    Deafen(this, RenderEvent, &EntityCounter::onRender);
    Deafen(this, SetupAndRenderEvent, &EntityCounter::onSetupAndRender);
    Module::onDisable();
}

void EntityCounter::defaultConfig() {
    setDef("text", (std::string)"{value} Entities");
    setDef("textscale", 0.70f);
    Module::defaultConfig("all");
    
}

void EntityCounter::settingsRender(float settingsOffset) {

    initSettingsPage();


    addHeader("Main");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void EntityCounter::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled()) return;
    if (SDK::clientInstance->getLocalPlayer()) {
        entityCount = (int)SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList().size();
    }
}

void EntityCounter::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (SDK::clientInstance->getLocalPlayer() && SDK::getCurrentScreen() == "hud_screen") {

        std::string str = std::format("{}", entityCount);

        this->normalRender(24, str);
    }
}
