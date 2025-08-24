#include "Sprint.hpp"

#include "Events/EventManager.hpp"
#include "Modules/Sneak/Sneak.hpp"

void Sprint::onEnable() {
    Listen(this, KeyEvent, &Sprint::onKey)
    Listen(this, MouseEvent, &Sprint::onMouse)
    Listen(this, RenderEvent, &Sprint::onRender)
    Listen(this, TickEvent, &Sprint::onTick)
    Module::onEnable();
}

void Sprint::onDisable() {
    Deafen(this, KeyEvent, &Sprint::onKey)
    Deafen(this, MouseEvent, &Sprint::onMouse)
    Deafen(this, RenderEvent, &Sprint::onRender)
    Deafen(this, TickEvent, &Sprint::onTick)
    Module::onDisable();
}

void Sprint::defaultConfig() {
    getKeybind();
    Module::defaultConfig("all");
    setDef("status", true);
    setDef("toggleStatus", true);
    setDef("textscale", 0.80f);
    setDef("always", false);
    
}

void Sprint::settingsRender(float settingsOffset) {
    initSettingsPage();


    addHeader("Toggle Sprint");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
    addToggle("Always Sprint", "Also known as auto sprint", "always");
    addToggle("Show Status", "", "status");
    addConditionalToggle(getOps<bool>("status"), "Show Toggle Status", "", "toggleStatus");
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

void Sprint::onSetup() {
    keybindActions.clear();
    keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
        toggleSprinting = !toggleSprinting;
        return {};
    });
}

void Sprint::onKey(KeyEvent& event) {
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
        keybindActions[0]({});
    }
}

void Sprint::onMouse(MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press) {
        keybindActions[0]({});
    }
}

void Sprint::onRender(RenderEvent& event) {
    if (!this->isEnabled()) return;
    if (!this->isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;

    if (!getOps<bool>("status")) return;

    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

        std::shared_ptr<Sneak> toggleSneak = std::dynamic_pointer_cast<Sneak>(ModuleManager::getModule("Toggle Sneak"));

        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            std::string text = "Standing";
            bool showStatus = true;
            bool status = toggleSprinting;

            if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SNEAKING)) {
                text = "Sneaking";

                if (toggleSneak != nullptr && toggleSneak->getOps<bool>("enabled")) status = toggleSneak->toggled;
                else showStatus = false;
            }
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SWIMMING)) text = "Swimming";
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_GLIDING)) {
                text = "Gliding";
                showStatus = false;
            }
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SLEEPING)) {
                text = "Sleeping";
                showStatus = false;
            }
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SPRINTING)) text = "Sprinting";
            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(FLAG_MOVING)) text = "Walking";

            if (getOps<bool>("toggleStatus") && showStatus) {
                std::string text2 = std::format("{} ({})", text, status ? "Toggled": "Vanilla");
                this->normalRender(5, text2);
            }
            else this->normalRender(5, text);

        }
    }
}

void Sprint::onTick(TickEvent& event) {
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
