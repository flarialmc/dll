#include "ClearChat.hpp"

void ClearChat::onEnable() {
    targetControl = nullptr;
    savedOriginalSize = false;
    Listen(this, SetupAndRenderEvent, &ClearChat::onSetupAndRender)
    Module::onEnable();
}

void ClearChat::onDisable() {
    Module::onDisable();
    restoreChat();
    Deafen(this, SetupAndRenderEvent, &ClearChat::onSetupAndRender)
}

void ClearChat::defaultConfig() {
    Module::defaultConfig("core");
}

void ClearChat::settingsRender(float settingsOffset) {
}

void ClearChat::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!this->isEnabled()) return;
    clearChat();
}

void ClearChat::clearChat() {
    if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;
    if (SDK::getCurrentScreen() != "hud_screen") return;

    // If we don't have the control yet, find it
    if (!targetControl) {
        SDK::screenView->VisualTree->root->forEachChild([this](std::shared_ptr<UIControl>& control) {
            if (control->getLayerName() == "chat_panel") {
                control->forEachChild([this](std::shared_ptr<UIControl>& child) {
                    if (child->getLayerName() == "chat_background") {
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

void ClearChat::restoreChat() {
    if (targetControl && savedOriginalSize) {
        targetControl->sizeConstrains = originalSize;
    }
    targetControl = nullptr;
    savedOriginalSize = false;
}

