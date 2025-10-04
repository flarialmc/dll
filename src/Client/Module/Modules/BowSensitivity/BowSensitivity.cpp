#include "BowSensitivity.hpp"
#include <SDK/Client/Item/Item.hpp>
#include <SDK/Client/Container/PlayerInventory.hpp>
#include <GUI/Engine/EngineImGui.hpp>
#include "../../../../SDK/Client/Actor/LocalPlayer.hpp"


void BowSensitivity::onEnable() {
    Listen(this, TickEvent, &BowSensitivity::onTick)
    Listen(this, TurnDeltaEvent, &BowSensitivity::onTurnDeltaEvent)
    Module::onEnable();
}

void BowSensitivity::onDisable() {
    Deafen(this, TickEvent, &BowSensitivity::onTick)
    Deafen(this, TurnDeltaEvent, &BowSensitivity::onTurnDeltaEvent)
    Module::onDisable();
}

void BowSensitivity::defaultConfig() {
    Module::defaultConfig("core");
    setDef("lowerUniformly", true);
    setDef("lowsens", 30.f);
    setDef("lowsensX", 30.f);
    setDef("lowsensY", 30.f);
}

void BowSensitivity::settingsRender(float settingsOffset) {
    initSettingsPage();

    addToggle("Lower Sensitivity Uniformly", "Decreases the sensitivity of the X and Y axes to the same extent.", "lowerUniformly");
    addConditionalSlider(getOps<bool>("lowerUniformly"), "Percentage", "Lowers sensitivity by this much percentage.", "lowsens", 100.f, 1.f, false);
    addConditionalSlider(!getOps<bool>("lowerUniformly"), "Percentage X", "Lowers X axis sensitivity by this much percentage.", "lowsensX", 100.f, 1.f, false);
    addConditionalSlider(!getOps<bool>("lowerUniformly"), "Percentage Y", "Lowers Y axis sensitivity by this much percentage.", "lowsensY", 100.f, 1.f, false);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void BowSensitivity::onTick(TickEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::hasInstanced || SDK::clientInstance == nullptr) {
        shouldLower = false;
        return;
    }

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player || !player->getSupplies()) {
        shouldLower = false;
        return;
    }

    auto supplies = player->getSupplies();
    auto inventory = supplies->getInventory();
    if (!inventory) {
        shouldLower = false;
        return;
    }

    if (SDK::getCurrentScreen() != "hud_screen") {
        shouldLower = false;
        return;
    }

    auto selectedSlot = supplies->getSelectedSlot();
    auto itemStack = inventory->getItem(selectedSlot);

    if (!itemStack || !itemStack->getItem()) {
        shouldLower = false;
    } else {
        auto itemName = itemStack->getItem()->name;
        shouldLower = (itemName == "bow");
    }

}

void BowSensitivity::onTurnDeltaEvent(TurnDeltaEvent &event) {
    if (this->isEnabled() && shouldLower && MC::heldRight) {
        if (getOps<bool>("lowerUniformly")) {
            event.delta.x *= (1.f - getOps<float>("lowsens") / 100.f);
            event.delta.y *= (1.f - getOps<float>("lowsens") / 100.f);
        }
        else {
            event.delta.x *= (1.f - getOps<float>("lowsensX") / 100.f);
            event.delta.y *= (1.f - getOps<float>("lowsensY") / 100.f);
        }
    }
}
