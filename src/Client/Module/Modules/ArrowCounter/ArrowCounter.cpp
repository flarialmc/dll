#include "ArrowCounter.hpp"

#include "Events/EventManager.hpp"
#include "Events/Game/TickEvent.hpp"
#include <Client.hpp>

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
    Module::defaultConfig("all");
    if (ModuleManager::initialized) Client::SaveSettings();
}

void ArrowCounter::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Arrow Counter");
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
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
                if (SDK::getCurrentScreen() != "hud_screen") return;
                auto arrowsCount = 0;

                auto inventory = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory();
                if (inventory == nullptr) return;

                auto offhandItem = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();
                if (offhandItem != nullptr)
                    if (offhandItem->getItem() != nullptr)
                        if (offhandItem->getItem()->name == "arrow")
                            arrowsCount = offhandItem->count;


                for (int i = 0; i < 36; i++) {
                    auto item = inventory->getItem(i);

                    if (item->getItem() != nullptr) {
                        if (item->getItem()->name == "arrow") {
                            arrowsCount += item->count;
                        }

                    }
                }

                arrows = arrowsCount;
            }
        }
    }
}

void ArrowCounter::onRender(RenderEvent& event) {
    if (!this->isEnabled()) return;
    auto arrowsStr = FlarialGUI::cached_to_string(arrows);
    this->normalRender(13, arrowsStr);
}