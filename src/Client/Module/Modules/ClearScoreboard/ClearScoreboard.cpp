#include "ClearScoreboard.hpp"

void ClearScoreboard::onEnable() {
    targetControl = nullptr;
    savedOriginalSize = false;
    lastServer = "";
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
    if (!(SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "zoom_screen" || SDK::getCurrentScreen() == "f3_screen")) return;

    // Re-resolve the control on server change since the UI tree gets rebuilt
    if (lastServer.empty() || lastServer != SDK::getServerIP()) {
        targetControl = nullptr;
        savedOriginalSize = false;
        lastServer = SDK::getServerIP();
    }

    // Clear every frame — scoreboard updates reset sizeConstrains
    clearScoreboard();
}

void ClearScoreboard::clearScoreboard() {
    if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;
    if (SDK::getCurrentScreen() != "hud_screen") return;
    if (!SDK::screenView || !SDK::screenView->VisualTree || !SDK::screenView->VisualTree->root) return;

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

    if (targetControl) {
        if (!savedOriginalSize) {
            originalSize = targetControl->sizeConstrains;
            savedOriginalSize = true;
        }
        if (targetControl->sizeConstrains.x != 0.f || targetControl->sizeConstrains.y != 0.f) {
            targetControl->sizeConstrains = Vec2{0.f, 0.f};
        }
    }
}

void ClearScoreboard::restoreScoreboard() {
    if (targetControl && savedOriginalSize) {
        targetControl->sizeConstrains = originalSize;
    }
    targetControl = nullptr;
    savedOriginalSize = false;
}