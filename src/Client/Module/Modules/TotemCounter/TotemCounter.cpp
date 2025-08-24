#include "TotemCounter.hpp"

#include "Events/EventManager.hpp"
#include "Events/Game/TickEvent.hpp"


void TotemCounter::onEnable() {
    Listen(this, TickEvent, &TotemCounter::onTick)
    Listen(this, RenderEvent, &TotemCounter::onRender)
    Module::onEnable();
}

void TotemCounter::onDisable() {
    Deafen(this, TickEvent, &TotemCounter::onTick)
    Deafen(this, RenderEvent, &TotemCounter::onRender)
    Module::onDisable();
}

void TotemCounter::defaultConfig() {
    setDef("text", (std::string)"Totems: {value}");
    setDef("onlyRenderWhenHoldingTotem", false);
    Module::defaultConfig("all");

}

void TotemCounter::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Totem Counter");
    addToggle("Only render when holding totem.", "", "onlyRenderWhenHoldingTotem");
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

void TotemCounter::onTick(TickEvent& event) {
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

void TotemCounter::onRender(RenderEvent& event) {
    if (!this->isEnabled() || !shouldRender) return;

    auto totemsStr = FlarialGUI::cached_to_string(totems);
    this->normalRender(13, totemsStr);
}