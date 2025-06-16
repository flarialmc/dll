#include "Sprint.hpp"

#include "Events/EventManager.hpp"

Sprint::Sprint(): Module("Toggle Sprint", "Automatically sprints for you!!!", IDR_AUTO_SPRINT_PNG, "CTRL")
{
    
}

void Sprint::onEnable()
{
    Listen(this, KeyEvent, &Sprint::onKey)
    Listen(this, RenderEvent, &Sprint::onRender)
    Listen(this, TickEvent, &Sprint::onTick)
    Module::onEnable();
}

void Sprint::onDisable()
{
    Deafen(this, KeyEvent, &Sprint::onKey)
    Deafen(this, RenderEvent, &Sprint::onRender)
    Deafen(this, TickEvent, &Sprint::onTick)
    Module::onDisable();
}

void Sprint::defaultConfig()
{
    getKeybind();
    Module::defaultConfig("all");
    setDef("status", false);
    setDef("textscale", 0.80f);
    setDef("always", false);
    
}

void Sprint::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));


    addHeader("Toggle Sprint");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
    addToggle("Always Sprint", "Also known as auto sprint", "always");
    addToggle("Show Status", "", "status");
    extraPadding();

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

void Sprint::onSetup()
{
    keybindActions.clear();
    keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
        toggleSprinting = !toggleSprinting;
        return {};
    });
}

void Sprint::onKey(KeyEvent& event)
{
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
        keybindActions[0]({});
    }
}

void Sprint::onRender(RenderEvent& event)
{
    Logger::debug("{}", this->toggleSprinting);
    if (!this->isEnabled()) return;
    if (!this->isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;

    if (!getOps<bool>("status")) return;

    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            std::string text = "Standing";
            if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SNEAKING)) {
                text = "Sneaking";
                this->normalRender(5, text);
            }
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SWIMMING)) {
                text = "Swimming";
                this->normalRender(5, text);
            }
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_GLIDING)) {
                text = "Gliding";
                this->normalRender(5, text);
            }
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SLEEPING)) {
                text = "Sleeping";
                this->normalRender(5, text);
            }
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
                text = "Sprinting";
                this->normalRender(5, text);
            }
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(FLAG_MOVING)) {
                text = "Walking";
                this->normalRender(5, text);
            }
            else {
                this->normalRender(5, text);
            }

        }
    }
}

void Sprint::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

                if (getOps<bool>("always")) {
                    handler->sprinting = true;
                    handler->mInputState.mSprintDown = true;
                    handler->mRawInputState.mSprintDown = true;
                }
                else {
                    handler->sprinting = toggleSprinting;
                    handler->mInputState.mSprintDown = toggleSprinting;
                    handler->mRawInputState.mSprintDown = toggleSprinting;
                }
        }
    }
}
