#include "ClearChat.hpp"

void ClearChat::onEnable() {
    Listen(this, SetupAndRenderEvent, &ClearChat::onSetupAndRender)
    Module::onEnable();
}

void ClearChat::onDisable() {
    Module::onDisable();
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

    SDK::screenView->VisualTree->root->forEachChild([](std::shared_ptr<UIControl>& control) {
        if (control->getLayerName() == "chat_panel") {
            control->forEachChild([](std::shared_ptr<UIControl>& child) {
                if (child->getLayerName() == "chat_background") {
                    if (child->sizeConstrains.x != 0.f || child->sizeConstrains.y != 0.f) {
                        child->sizeConstrains = Vec2{0.f, 0.f};
                    }
                }
            });
            return true;
        }
        return false;
    });
}

