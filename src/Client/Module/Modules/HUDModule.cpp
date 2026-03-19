#include "HUDModule.hpp"
#include "ClickGUI/ClickGUI.hpp"

HUDModule::HUDModule(
    int renderIndex,
    const std::string& name,
    const std::string& description,
    int icon,
    const std::string& defaultKeybind,
    const std::vector<std::string>& aliases
) : Module(name, description, icon, defaultKeybind, false, aliases),
    renderIndex(renderIndex)
{
}

void HUDModule::onEnable() {
    Listen(this, RenderEvent, &HUDModule::onRender)
    Module::onEnable();
    customInit();
}

void HUDModule::onDisable() {
    Deafen(this, RenderEvent, &HUDModule::onRender)
    Module::onDisable();
    customCleanup();
}

void HUDModule::defaultConfig() {
    // Set default text format - modules can override in customConfig()
    setDef("text", std::string("{value}"));

    // Call standard config with all settings
    Module::defaultConfig("all");

    // Allow modules to add their own settings
    customConfig();
}

void HUDModule::settingsRender(float settingsOffset) {
    // Initialize settings page
    initSettingsPage();

    // Custom settings first (if module needs to add settings before main)
    customSettings();

    // Standard sections in order
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

    // Cleanup
    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HUDModule::onRender(RenderEvent& event) {
    if (!this->isEnabled() || ClickGUI::blurActive) {
        return;
    }

    // Get the display value from the derived class
    std::string value = getDisplayValue();

    // Render using the standard normalRender method
    this->normalRender(renderIndex, value);
}
