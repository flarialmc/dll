#include "ProjectileCounter.hpp"
#include "Client.hpp"
#include "SDK/LocalPlayer.hpp"

void ProjectileCounter::onEnable() {
    Listen(this, RenderEvent, &ProjectileCounter::onRender);
    Module::onEnable();
}

void ProjectileCounter::onDisable() {
    Deafen(this, RenderEvent, &ProjectileCounter::onRender);
    Module::onDisable();
}

void ProjectileCounter::defaultConfig() {
    setDef("show_arrows", true);
    setDef("show_pearls", true);
    setDef("show_snowballs", true);
    setDef("textscale", 2.0f);
    setDef("textShadow", true);
    Module::defaultConfig("all");
}

void ProjectileCounter::settingsRender(float settingsOffset) {
    initSettingsPage();
    
    addHeader("Visibility");
    addToggle("Show Arrows", "Toggle arrow count", "show_arrows");
    addToggle("Show Pearls", "Toggle ender pearl count", "show_pearls");
    addToggle("Show Snowballs", "Toggle snowball count", "show_snowballs");
    
    extraPadding();
    addHeader("Appearance");
    addSlider("Text Scale", "", "textscale", 1.0f, 5.0f); 
    addToggle("Text Shadow", "", "textShadow");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

int ProjectileCounter::getItemCount(std::string itemName) {
    auto player = SDK::getLocalPlayer();
    if (!player) return 0;

    auto inventory = player->getSupplies()->getContainer();
    int count = 0;

    for (int i = 0; i < inventory->getContainerSize(); i++) {
        auto itemStack = inventory->getItem(i);
        if (itemStack && itemStack->getItem()) {
            std::string name = itemStack->getItem()->getRawNameId();
            if (name.find(itemName) != std::string::npos) {
                count += itemStack->getStackSize();
            }
        }
    }
    return count;
}

void ProjectileCounter::onRender(RenderEvent& event) {
    if (!isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;

    std::string finalDisplay = "";

    if (getOps<bool>("show_pearls")) {
        finalDisplay += "Ender Pearls: " + std::to_string(getItemCount("ender_pearl")) + " ";
    }

    if (getOps<bool>("show_snowballs")) {
        finalDisplay += "Snowballs: " + std::to_string(getItemCount("snowball")) + " ";
    }

    if (getOps<bool>("show_arrows")) {
        finalDisplay += "Arrows: " + std::to_string(getItemCount("arrow"));
    }

    if (!finalDisplay.empty()) {
        normalRenderCore(1, finalDisplay);
    }
}
