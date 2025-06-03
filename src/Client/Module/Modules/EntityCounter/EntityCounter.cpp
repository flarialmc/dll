#include "EntityCounter.hpp"

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

    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));


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
    if (SDK::clientInstance->getLocalPlayer()) {
        entityCount = (int)SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList().size();
    }
}

void EntityCounter::onRender(RenderEvent &event) {
    if (SDK::clientInstance->getLocalPlayer() && SDK::getCurrentScreen() == "hud_screen") {

        std::string str = std::format("{}", entityCount);

        this->normalRender(24, str);
    }
}
