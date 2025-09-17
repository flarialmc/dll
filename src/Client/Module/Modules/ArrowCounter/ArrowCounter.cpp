#include "ArrowCounter.hpp"

#include "Client/GUI/Engine/Engine.hpp"
#include "SDK/Client/Actor/LocalPlayer.hpp"
#include "SDK/Client/Container/PlayerInventory.hpp"
#include "SDK/Client/Container/Inventory.hpp"
#include "SDK/Client/Item/Item.hpp"

#include "Events/Game/TickEvent.hpp"


void ArrowCounter::onEnable() {
    Listen(this, TickEvent, &ArrowCounter::onTick)
    Listen(this, RenderEvent, &ArrowCounter::onRender)
    Module::onEnable();
}

void ArrowCounter::onDisable() {
    Deafen(this, TickEvent, &ArrowCounter::onTick)
    Deafen(this, RenderEvent, &ArrowCounter::onRender)
    Module::onDisable();
}

void ArrowCounter::defaultConfig() {
    setDef("text", (std::string)"Arrows: {value}");
    setDef("onlyRenderWhenHoldingBowOrCrossbow", false);
    Module::defaultConfig("all");

}

void ArrowCounter::settingsRender(float settingsOffset) {
    initSettingsPage();

    addToggle("Only render when holding bow or crossbow", "", "onlyRenderWhenHoldingBowOrCrossbow");
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

void ArrowCounter::onTick(TickEvent& event) {
    if (!this->isEnabled()) return;
    if (!SDK::hasInstanced || SDK::clientInstance == nullptr) return;

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player || !player->getSupplies()) return;

    auto supplies = player->getSupplies();
    auto inventory = supplies->getInventory();
    if (!inventory) return;

    if (SDK::getCurrentScreen() != "hud_screen") return;

    // Only render when holding bow or crossbow if setting is enabled
    shouldRender = true;
    if (getOps<bool>("onlyRenderWhenHoldingBowOrCrossbow")) {

        auto selectedSlot = supplies->getSelectedSlot();
        auto itemStack = inventory->getItem(selectedSlot);

        if (!itemStack || !itemStack->getItem()) {
            shouldRender = false;
        } else {
            auto itemName = itemStack->getItem()->name;
            shouldRender = (itemName == "bow" || itemName == "crossbow");
        }
    }

    if (shouldRender) {

        // Cache arrow count by updating every 4 ticks
        tickCounter++;
        if (tickCounter % 4 == 0) {

            auto arrowsCount = 0;
            auto offhandItem = player->getOffhandSlot();

            if (offhandItem && offhandItem->getItem() && offhandItem->getItem()->name == "arrow") {
                arrowsCount = offhandItem->count;
            }

            for (int i = 0; i < 36; i++) {
                auto item = inventory->getItem(i);

                if (item->getItem() != nullptr) {
                    if (item->getItem()->name == "arrow") {
                        arrowsCount += item->count;
                    }

                }
            }

            lastArrowCount = arrowsCount;
        }

        arrows = lastArrowCount;
    }
}

void ArrowCounter::onRender(RenderEvent& event) {
    if (!this->isEnabled() || !shouldRender) return;

    auto arrowsStr = FlarialGUI::cached_to_string(arrows);
    this->normalRender(13, arrowsStr);
}