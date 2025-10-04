#include "ClearScoreboard.hpp"
#include "Events/EventManager.hpp"

void ClearScoreboard::onEnable() {
    targetControl = nullptr;
    Listen(this, SetupAndRenderEvent, &ClearScoreboard::onSetupAndRender)
    Module::onEnable();
}

void ClearScoreboard::onDisable() {
    Module::onDisable();
    restoreScoreboard();
    Deafen(this, SetupAndRenderEvent, &ClearScoreboard::onSetupAndRender)
}

void ClearScoreboard::defaultConfig() {
    Module::defaultConfig("core");
}

void ClearScoreboard::settingsRender(float settingsOffset) {
}

void ClearScoreboard::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!this->isEnabled()) return;
    clearScoreboard();
}

void ClearScoreboard::clearScoreboard() {
    if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;
    if (SDK::getCurrentScreen() != "hud_screen") return;

    // If we don't have the control yet, find it
    if (!targetControl) {
        SDK::screenView->VisualTree->root->forEachChild([this](std::shared_ptr<UIControl>& control) {
            if (control->getLayerName() == "sidebar") {
                control->forEachChild([this](std::shared_ptr<UIControl>& child) {
                    if (child->getLayerName() == "main") {
                        targetControl = child;
                    }
                });
                return true;
            }
            return false;
        });
    }

    // Clear the control if we have it
    if (targetControl) {
        if (targetControl->sizeConstrains.x != 0.f || targetControl->sizeConstrains.y != 0.f) {
            targetControl->sizeConstrains = Vec2{0.f, 0.f};
        }
    }
}

void ClearScoreboard::restoreScoreboard() {
    targetControl = nullptr;
}