#include "SnapLook.hpp"

#include "Events/EventManager.hpp"

SnapLook::SnapLook(): Module("SnapLook", "Quickly look behind you.", IDR_EYE_PNG, "V")
{
    
}

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
    setDef("togglable", false);
    Module::defaultConfig("core");
}

void SnapLook::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("SnapLook");
    addToggle("Togglable", "", "togglable");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void SnapLook::onKey(KeyEvent& event)
{
    if (!this->isEnabled()) return;
    if (SDK::getCurrentScreen() != "hud_screen") return; // only allow in game
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

void SnapLook::onGetViewPerspective(PerspectiveEvent& event) const
{
    if (!this->enabledState) return;
    if (this->active)
        event.setPerspective(Perspective::ThirdPersonFront);
}
