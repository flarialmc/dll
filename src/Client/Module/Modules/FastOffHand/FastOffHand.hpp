#pragma once

#include "../Module.hpp"

class FastOffHand : public Module {
public:
    FastOffHand() : Module("Fast OffHand",
        "Press F to quickly swap a selected item to your offhand slot like Java.",
        IDR_TOTEM_PNG, "F", false, {"fastoffhand", "offhand"}) {
    }

    void onEnable() override;
    void onDisable() override;
    void defaultConfig() override;
    void settingsRender(float settingsOffset) override;

    void onKey(KeyEvent& event);

private:
    void swapToOffHand(int slot);
    bool isItemInInventory(const std::string& itemName, int& outSlot);
};
