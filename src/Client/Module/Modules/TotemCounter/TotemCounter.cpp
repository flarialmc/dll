#include "TotemCounter.hpp"

void TotemCounter::onEnable()
{
    HUDModule::onEnable();
    Listen(this, TickEvent, &TotemCounter::onTick)
}

void TotemCounter::onDisable()
{
    Deafen(this, TickEvent, &TotemCounter::onTick)
    HUDModule::onDisable();
}

void TotemCounter::customConfig()
{
    setDef("onlyRenderWhenHoldingTotem", false);
}

void TotemCounter::customSettings()
{
    addToggle("Only render when holding totem.", "", "onlyRenderWhenHoldingTotem");
}

std::string TotemCounter::getDisplayValue()
{
    if (!shouldRender) return "";
    return FlarialGUI::cached_to_string(totems);
}

void TotemCounter::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (!SDK::hasInstanced || SDK::clientInstance == nullptr) return;

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player || !player->getSupplies()) return;

    auto supplies = player->getSupplies();
    auto inventory = supplies->getInventory();
    if (!inventory) return;

    if (SDK::getCurrentScreen() != "hud_screen") return;

    shouldRender = true;
    if (getOps<bool>("onlyRenderWhenHoldingTotem")) {

        auto selectedSlot = supplies->getSelectedSlot();
        auto itemStack = inventory->getItem(selectedSlot);

        if (!itemStack || !itemStack->getItem()) {
            shouldRender = false;
        } else {
            auto itemName = itemStack->getItem()->name;
            shouldRender = (itemName == "totem_of_undying");
        }
    }

    if (shouldRender) {

        tickCounter++;
        if (tickCounter % 4 == 0) {

            auto totemsCount = 0;
            auto offhandItem = player->getOffhandSlot();

            if (offhandItem && offhandItem->getItem() && offhandItem->getItem()->name == "totem_of_undying") {
                totemsCount = offhandItem->count;
            }

            for (int i = 0; i < 36; i++) {
                auto item = inventory->getItem(i);

                if (item->getItem() != nullptr) {
                    if (item->getItem()->name == "totem_of_undying") {
                        totemsCount += item->count;
                    }

                }
            }

            lastTotemCount = totemsCount;
        }

        totems = lastTotemCount;
    }
}
