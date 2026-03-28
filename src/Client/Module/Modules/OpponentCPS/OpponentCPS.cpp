#include "OpponentCPS.hpp"

void OpponentCPS::customConfig() {
    setDef("showCps", true);
    setDef("keybind", (std::string)"O");
}

void OpponentCPS::customSettings() {
    addKeybind("Toggle Key", "", "keybind", true);
    addToggle("Show CPS on Name", "Displays (L | R) on the right", "showCps");
}

void OpponentCPS::customInit() {
    this->setEnabled(false);
    Listen(this, RenderNameEvent, &OpponentCPS::onRenderName)
    Listen(this, KeyEvent, &OpponentCPS::onKey)
}

void OpponentCPS::customCleanup() {
    Deafen(this, RenderNameEvent, &OpponentCPS::onRenderName)
    Deafen(this, KeyEvent, &OpponentCPS::onKey)
}

void OpponentCPS::onKey(KeyEvent& event) {
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
        this->setEnabled(!this->isEnabled());
    }
}

void OpponentCPS::onRenderName(RenderNameEvent& event) {
    if (!this->isEnabled() || !getOps<bool>("showCps")) return;

    auto player = event.getPlayer();
    
    if (player != nullptr && !player->isLocalPlayer()) {
        int left = player->getLeftCPS(); 
        int right = player->getRightCPS();

        std::string formattedCps = std::format(" ({} | {})", left, right);
        
        event.setName(event.getName() + formattedCps);
    }
}

std::string OpponentCPS::getDisplayValue() {
    return this->isEnabled() ? "Monitoring" : "";
}
