#include "Sprint.hpp"
#include "Modules/Sneak/Sneak.hpp"

void Sprint::customConfig() {
    setDef("status", true);
    setDef("toggleStatus", true);
    setDef("always", false);
    setDef("keybind", (std::string)"");
}

void Sprint::customSettings() {
    addKeybind("Keybind", "", "keybind", true);
    addToggle("Always Sprint", "Also known as auto sprint", "always");
    addToggle("Show Status", "", "status");
    addConditionalToggle(getOps<bool>("status"), "Show Toggle Status", "", "toggleStatus");
    extraPadding();
}

void Sprint::customInit() {
    Listen(this, KeyEvent, &Sprint::onKey)
    Listen(this, MouseEvent, &Sprint::onMouse)
    Listen(this, TickEvent, &Sprint::onTick)
}

void Sprint::customCleanup() {
    Deafen(this, KeyEvent, &Sprint::onKey)
    Deafen(this, MouseEvent, &Sprint::onMouse)
    Deafen(this, TickEvent, &Sprint::onTick)
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

std::string Sprint::getDisplayValue() {
    if (!getOps<bool>("status")) return "";
    if (!SDK::hasInstanced || SDK::clientInstance == nullptr) return "";

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
            return std::format("{} ({})", text, status ? "Toggled": "Vanilla");
        }
        else return text;
    }

    return "";
}

void Sprint::onTick(TickEvent& event) {
    if (!this->isEnabled()) return;
    if (SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            auto handler = SDK::clientInstance->getLocalPlayer()->getHandler();
            if (getOps<bool>("always")) {
                handler.setSprinting(true);
                handler.setMSprintDown(true);
                handler.setRawMSprintDown(true);
            }
            else {
                handler.setSprinting(toggleSprinting);
                handler.setMSprintDown(toggleSprinting);
                handler.setRawMSprintDown(toggleSprinting);
            }
        }
    }
}
