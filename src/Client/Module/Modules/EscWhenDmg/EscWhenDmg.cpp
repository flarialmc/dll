//
// Created by tonyv on 11/20/2025.
//

#include "EscWhenDmg.hpp"

#include "Events/Network/PacketEvent.hpp"
#include "SDK/Client/Network/Packet/EntityEventPacket.hpp"

void EscWhenDmg::onSetup()
{
    if (mAddr == NULL) mAddr = GET_SIG_ADDRESS("Keyboard::feed");
}

void EscWhenDmg::onEnable()
{
    Module::onEnable();
    // our code is aids bro
    Listen(this, PacketEvent, &EscWhenDmg::onPacketEvent);
}

void EscWhenDmg::onDisable()
{
    Deafen(this, PacketEvent, &EscWhenDmg::onPacketEvent);
    Module::onDisable();
}

void EscWhenDmg::defaultConfig() {
    Module::defaultConfig("core");
    setDef("chest", true);
    setDef("chat", true);
    setDef("pause", true);
    setDef("inventory", true);
}

void EscWhenDmg::settingsRender(float settingsOffset)
{
    initSettingsPage();
    addToggle("Chest screen", "Esc when take damage while on chest screen", "chest");
    addToggle("Chat screen", "Esc when take damage while on chat screen", "chat");
    addToggle("Pause screen", "Esc when take damage while on pause screen", "pause");
    addToggle("Inventory screen", "Esc when take damage while on inventory screen", "inventory");
    FlarialGUI::UnsetScrollView();
    this->resetPadding();
}

void EscWhenDmg::onPacketEvent(PacketEvent& event) {
    const auto currentScreen = SDK::getCurrentScreen();

    const auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) return;

    if (const auto pkt = event.getPacket(); pkt && pkt->getId() == MinecraftPacketIds::ActorEvent) {
        const auto *eep = reinterpret_cast<EntityEventPacket*>(pkt);

        const auto *rIdComp = player->getRuntimeIDComponent();
        if (!rIdComp) return;


        std::vector<bool> screen_check = {
            getOps<bool>("chest") && currentScreen == "small_chest_screen",
            getOps<bool>("chest") && currentScreen == "large_chest_screen",
            getOps<bool>("chest") && currentScreen == "chest_screen", // just in case
            getOps<bool>("chat") && currentScreen == "chat_screen",
            getOps<bool>("pause") && currentScreen == "pause_screen",
            getOps<bool>("inventory") && currentScreen == "inventory_screen",
        };


        for (const bool check : screen_check) {
            if (eep->RuntimeID == rIdComp->runtimeID && eep->EventID == ActorEvent::Hurt && check) {
                Logger::debug(std::format("EscWhenDmg: Closing screen: {}", currentScreen));
                FlarialGUI::Notify("Closing current screen");
                // Leslie, why did you lie to me?
                if (mAddr == NULL) mAddr = GET_SIG_ADDRESS("Keyboard::feed");
                Memory::CallFunc<void, int, bool>(reinterpret_cast<void *>(mAddr), 27, true);
                Memory::CallFunc<void, int, bool>(reinterpret_cast<void *>(mAddr), 27, false);
                break;
            }
        }
    }
}


