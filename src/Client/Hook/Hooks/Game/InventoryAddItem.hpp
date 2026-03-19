#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Container/Inventory.hpp"

class InventoryAddItem : public Hook {
private:
    static bool callback(Inventory* inventory, ItemStack& itemStack);

public:
    using original = bool(__thiscall*)(Inventory* inventory, ItemStack& itemStack);

    static inline original funcOriginal = nullptr;

    InventoryAddItem();

    void enableHook() override;
};
