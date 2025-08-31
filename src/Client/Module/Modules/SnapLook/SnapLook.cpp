#include "SnapLook.hpp"

#include "Events/EventManager.hpp"


void SnapLook::onEnable()
{
    Listen(this, KeyEvent, &SnapLook::onKey)
    Listen(this, MouseEvent, &SnapLook::onMouse)
    Listen(this, PerspectiveEvent, &SnapLook::onGetViewPerspective)
    Module::onEnable();
}

void SnapLook::onDisable()
{
    Deafen(this, KeyEvent, &SnapLook::onKey)
    Deafen(this, MouseEvent, &SnapLook::onMouse)
    Deafen(this, PerspectiveEvent, &SnapLook::onGetViewPerspective)
    Module::onDisable();
}

void SnapLook::defaultConfig()
{
    getKeybind();
    Module::defaultConfig("core");
    setDef("togglable", false);
    setDef("mode", std::string("ThirdPersonFront"));
}

void SnapLook::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("SnapLook");
    addToggle("Togglable", "", "togglable");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
    addDropdown("Mode", "", std::vector<std::string>{
        "Third Person Front",
        "Third Person Back",
        "First Person"}, "mode", true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void SnapLook::onKey(KeyEvent& event)
{
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) keybindActions[0]({});

    if (!getOps<bool>("togglable") && !this->isKeybind(event.keys)) this->active = false;
}

void SnapLook::onMouse(MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press)
        keybindActions[0]({});
    if (!getOps<bool>("togglable") && Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Release)
        this->active = false;
}

void SnapLook::onGetViewPerspective(PerspectiveEvent& event)
{
    if (!this->enabledState) return;
    if (this->active) {

        auto it = perspectiveMap.find(getOps<std::string>("mode"));

        if (it != perspectiveMap.end()) {
            event.setPerspective(it->second);
        }
        else {
            getOps<std::string>("mode") = "Third Person Front";
            event.setPerspective(Perspective::ThirdPersonFront);
        }

    }
}
