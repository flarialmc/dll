#include "CPSCounter.hpp"

#include "Client.hpp"
#include <Modules/Misc/Input/GUIMouseListener.hpp>

void CPSCounter::onEnable() {
    Listen(this, RenderEvent, &CPSCounter::onRender)
    Module::onEnable();
}

void CPSCounter::onDisable() {
    Deafen(this, RenderEvent, &CPSCounter::onRender)
    Module::onDisable();
}

void CPSCounter::defaultConfig() {
    setDef("text", (std::string)"CPS: {value}");
    setDef("rightcps", false);
    Module::defaultConfig("all");
    
}

void CPSCounter::settingsRender(float settingsOffset) {

    initSettingsPage();


    addHeader("CPS Counter");
    defaultAddSettings("main");
    addToggle("Right Click CPS", "", "rightcps");
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


void CPSCounter::onRender(RenderEvent& event)  {
    if (this->isEnabled()) {
        if (!getOps<bool>("rightcps")) {
            std::string leftCPS = FlarialGUI::cached_to_string(GUIMouseListener::GetLeftCPS());
            this->normalRender(1, leftCPS);
        }
        else {
            std::string leftAndRightCPS = FlarialGUI::cached_to_string(GUIMouseListener::GetLeftCPS()) + " | " + FlarialGUI::cached_to_string(GUIMouseListener::GetRightCPS());
            this->normalRender(1, leftAndRightCPS);
        }
    }
}