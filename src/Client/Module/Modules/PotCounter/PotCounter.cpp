#include "PotCounter.hpp"

void PotCounter::customInit()
{
    Listen(this, TickEvent, &PotCounter::onTick)
}

void PotCounter::customCleanup()
{
    Deafen(this, TickEvent, &PotCounter::onTick)
}

void PotCounter::customConfig()
{
    setDef("text", (std::string)"{value} Pots");
    setDef("textscale", 0.70f);
}

std::string PotCounter::getDisplayValue()
{
    return FlarialGUI::cached_to_string(pots);
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
