#include "ArrowCounter.hpp"

void ArrowCounter::customConfig() {
    setDef("onlyRenderWhenHoldingBowOrCrossbow", false);
}

void ArrowCounter::customSettings() {
    addToggle("Only render when holding bow or crossbow", "", "onlyRenderWhenHoldingBowOrCrossbow");
}

void ArrowCounter::customInit() {
    Listen(this, TickEvent, &ArrowCounter::onTick)
}

void ArrowCounter::customCleanup() {
    Deafen(this, TickEvent, &ArrowCounter::onTick)
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

std::string ArrowCounter::getDisplayValue() {
    if (!shouldRender) return "";
    return FlarialGUI::cached_to_string(arrows);
}
