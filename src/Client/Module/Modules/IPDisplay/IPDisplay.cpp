#include "IPDisplay.hpp"

void IPDisplay::onEnable() {
    Listen(this, RenderEvent, &IPDisplay::onRender)
    Module::onEnable();
}

void IPDisplay::onDisable() {
    Deafen(this, RenderEvent, &IPDisplay::onRender)
    Module::onDisable();
}

void IPDisplay::defaultConfig() {
    setDef("responsivewidth", true);
    setDef("textscale", 0.80f);
    Module::defaultConfig("all");
    setDef("port", false);
    
}

void IPDisplay::settingsRender(float settingsOffset) {
    initSettingsPage();


    addHeader("IP Display");
    defaultAddSettings("main");
    addToggle("Show Port", "", "port");
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

void IPDisplay::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    std::string IPStr = SDK::getServerIP();
    std::string port = SDK::getServerPort();
    std::string fullstr = IPStr;
    if (getOps<bool>("port")) fullstr += ":" + port;
    this->normalRender(10, fullstr);
}
