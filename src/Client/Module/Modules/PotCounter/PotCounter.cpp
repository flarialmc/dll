#include "PotCounter.hpp"

#include "Events/EventManager.hpp"

PotCounter::PotCounter(): Module("Pot Counter", "Counts how much potions are\nin your inventory.",
                                 IDR_POTION_PNG, "")
{
    
}

void PotCounter::onEnable()
{
    Listen(this, TickEvent, &PotCounter::onTick)
    Listen(this, RenderEvent, &PotCounter::onRender)
    Module::onEnable();
}

void PotCounter::onDisable()
{
    Deafen(this, TickEvent, &PotCounter::onTick)
    Deafen(this, RenderEvent, &PotCounter::onRender)
    Module::onDisable();
}

void PotCounter::defaultConfig()
{
    setDef("text", (std::string)"{value} Pots");
    setDef("textscale", 0.70f);
    Module::defaultConfig("all");
    
}

void PotCounter::settingsRender(float settingsOffset)
{

    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));


    addHeader("Main");
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

void PotCounter::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            auto potsCount = 0;
            if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
                auto inventory = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory();

                if (inventory != nullptr) {
                    for (int i = 0; i < 36; i++) {
                        auto item = inventory->getItem(i);

                        if (item->getItem() != nullptr) {
                            if (item->getItem()->name == "splash_potion") {
                                potsCount++;
                            }
                        }
                    }
                }

                pots = potsCount;
            }
        }
    }
}

void PotCounter::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;
    auto potsStr = FlarialGUI::cached_to_string(pots);
    this->normalRender(14, potsStr);
}
