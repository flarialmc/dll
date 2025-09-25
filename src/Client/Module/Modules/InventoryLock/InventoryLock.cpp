#include "InventoryLock.hpp"


#include "Events/EventManager.hpp"
#include "Events/Events.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "SDK/Client/Render/BaseActorRenderContext.hpp"
#include "Utils/Render/PositionUtils.hpp"

void InventoryLock::onEnable() {
    parser.parseOptionsFile();
    Listen(this, KeyEvent, &InventoryLock::onKey)
    Listen(this, ItemDropEvent, &InventoryLock::onItemDrop)
    Module::onEnable();
}

void InventoryLock::onDisable() {
    Deafen(this, KeyEvent, &InventoryLock::onKey)
    Deafen(this, ItemDropEvent, &InventoryLock::onItemDrop)
    Module::onDisable();
}

void InventoryLock::defaultConfig() {
    Module::defaultConfig("core");
    setDef("toolsonly", false);
    setDef("doubleclickthrow", false);
}

void InventoryLock::settingsRender(float settingsOffset) {

    initSettingsPage();

    addToggle("Tools Only", "Only lock tools from being dropped", "toolsonly");
    addToggle("Double Click Throw", "Throw a locked item by double clicking your drop key", "doubleclickthrow");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void InventoryLock::onKey(KeyEvent& event)
{
    std::string option = parser.options["ctrl_fullkeyboardgameplay"] == "1" ? "keyboard_type_1_key." : "keyboard_type_0_key.";
    auto value = parser.options[option + "drop"];

    if (value == std::to_string(event.getKey()) and SDK::currentScreen == "hud_screen" and event.action == 1)
    {
        if (SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot())->getItem() != nullptr)
        {
            auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot());
            if (getOps<bool>("toolsonly"))
            {

                if (item->getMaxDamage() != 0)
                {
                    if (getOps<bool>("doubleclickthrow"))
                    {
                        auto now = std::chrono::high_resolution_clock::now();
                        auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastThrown).count();

                        if (lastItemThrown == item && timeDiff < 300)
                        {
                            lastThrown = now;
                            lastItemThrown = item;
                            return;
                        }
                        else
                        {
                            lastThrown = now;
                            lastItemThrown = item;
                            event.cancel();
                        }
                    }
                    else
                    {
                        event.cancel();
                    }
                }

            }

            else
            {
                if (getOps<bool>("doubleclickthrow"))
                {
                    auto now = std::chrono::high_resolution_clock::now();
                    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastThrown).count();

                    if (lastItemThrown == item && timeDiff < 300)
                    {
                        lastThrown = now;
                        lastItemThrown = item;
                        return;
                    }
                    else
                    {
                        lastThrown = now;
                        lastItemThrown = item;
                        event.cancel();
                    }
                }
                else
                {
                    event.cancel();
                }
            }
        }
    }
}

void InventoryLock::onItemDrop(ItemDropEvent& event)
{

}
