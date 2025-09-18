#include "TabList.hpp"
#include "Modules/Nick/NickModule.hpp"

// Function declarations from TabListTextures.cpp
void InitializeAsyncLoading();
void InitializeDX12Uploader();
void CleanupPlayerHeadTextures();

// Event handling functions for TabList module

void TabList::onEnable() {
    Listen(this, RenderEvent, &TabList::onRender)
    Listen(this, KeyEvent, &TabList::onKey)
    Listen(this, MouseEvent, &TabList::onMouse)
    InitializeAsyncLoading();
    // START OF CHANGE: Initialize DX12 Uploader
    InitializeDX12Uploader();
    // END OF CHANGE
    Module::onEnable();
}

void TabList::onDisable() {
    Deafen(this, RenderEvent, &TabList::onRender)
    Deafen(this, KeyEvent, &TabList::onKey)
    Deafen(this, MouseEvent, &TabList::onMouse)
    //CleanupPlayerHeadTextures();
    CleanupPlayerHeadTextures();
    Module::onDisable();
}

void TabList::defaultConfig() {
    this->settings.changeType<float, int>("maxColumn");
    setDef("enabled", true);
    setDef("uiscale", 0.65f);
    setDef("playerCount", true);
    setDef("serverPing", true);
    setDef("worldName", true);
    setDef("maxColumn", 10);
    setDef("togglable", false);
    setDef("showHeads", true);
    setDef("textalignment", (std::string) "Left");
    setDef("textShadow", false);
    setDef("textShadowOffset", 0.003f);
    getKeybind();
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    Module::defaultConfig("main");
    Module::defaultConfig("colors");
    setDef("alphaOrder", true);
    setDef("flarialFirst", true);
}

void TabList::onSetup() {
    keybindActions.clear();

    keybindActions.push_back([this](std::vector<std::any> args)-> std::any {
        if (SDK::getCurrentScreen() != "hud_screen" &&
            SDK::getCurrentScreen() != "zoom_screen" &&
            SDK::getCurrentScreen() != "f3_screen" && this->name != "ClickGUI"
        )
            return {};
        this->active = !this->active;
        refreshCache = true;
        return {};
    });
}

void TabList::settingsRender(float settingsOffset) {
    initSettingsPage();

    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center"}, "textalignment", true);
    addToggle("Text Shadow", "Displays a shadow under the text", "textShadow");
    addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Misc");
    addToggle("Togglable", "", "togglable");
    addToggle("Show Player Heads", "", "showHeads");
    addToggle("Player Count", "", "playerCount");
    addSliderInt("Max Players per Column", "", "maxColumn", 30, 1);
    addToggle("World Name", "", "worldName");
    addToggle("Server Ping", "", "serverPing");
    addToggle("Alphabetical Order", "", "alphaOrder");
    addToggle("Flarial First", "Prioritize Flarial users (Dev > Staff > Gamer > Supporter > Booster > Default) at the top", "flarialFirst");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
    extraPadding();

    addHeader("Colors");
    addConditionalColorPicker(getOps<bool>("showBg"), "Background Color", "", "bg");
    addConditionalColorPicker(getOps<bool>("rectShadow"), "Background Shadow Color", "", "rectShadow");
    addColorPicker("Text Color", "", "text");
    addConditionalColorPicker(getOps<bool>("textShadow"), "Text Shadow Color", "", "textShadow");
    addConditionalColorPicker(getOps<bool>("border"), "Border Color", "", "border");
    addConditionalColorPicker(getOps<bool>("glow"), "Glow Color", "", "glow");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void TabList::onMouse(const MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press) keybindActions[0]({});
    if (!getOps<bool>("togglable") && Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Release) this->active = false;
}

void TabList::onKey(const KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) keybindActions[0]({});

    if (!getOps<bool>("togglable") && !this->isKeybind(event.keys)) this->active = false;
}