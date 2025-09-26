#include "TotemCounter.hpp"


#include "Events/Game/TickEvent.hpp"
#include "SDK/Client/Network/Packet/EntityEventPacket.hpp"
#include "SDK/Client/Network/Packet/SetActorMotionPacket.hpp"

void TotemCounter::onEnable() {
    Listen(this, TickEvent, &TotemCounter::onTick)
    Listen(this, RenderEvent, &TotemCounter::onRender)
    Listen(this, PacketEvent, &TotemCounter::onPacketEvent)

    // flush pops map
    popsById.clear();

    Module::onEnable();
}

void TotemCounter::onDisable() {
    Deafen(this, TickEvent, &TotemCounter::onTick)
    Deafen(this, RenderEvent, &TotemCounter::onRender)
    Deafen(this, PacketEvent, &TotemCounter::onPacketEvent)
    Module::onDisable();
}

void TotemCounter::defaultConfig() {
    setDef("text", (std::string)"Totems: {value}");
    setDef("textUsed", (std::string)"Pops: {value}");
    setDef("onlyRenderWhenHoldingTotem", false);
    setDef("mode", (std::string)"Current");
    setDef("notifyOnPop", false);
    setDef("listenForOthers", false);
    Module::defaultConfig("all");
}

void TotemCounter::settingsRender(float settingsOffset) {
    initSettingsPage();

    addDropdown("Mode", "Choose the working mode.", std::vector<std::string>{"Default", "Pops"}, "mode", true);
    addToggle("Only render when holding totem.", "", "onlyRenderWhenHoldingTotem");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    // defaultAddSettings("text");
    addTextBox("Normal Format", "Use {value} for the value.", 0, "text");
    addTextBox("Pops Format", "Use {value} for the value.", 0, "textUsed");
    addSlider("Text Scale", "", "textscale", 2.0f);
    addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, "textalignment", true);
    addToggle("Text Shadow", "Displays a shadow under the text", "textShadow");
    addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    addHeader("Chat Notfications");
    addButton("Clear Stored Pops", "I'm keeping this here until I figure out a proper way for this.", "Click Here", [this](){popsById.clear();});
    addToggle("Notify on Pop", "Notifies in chat when a totem pop happens", "notifyOnPop");
    addConditionalToggle(getOps<bool>("notifyOnPop") ,"Notifier: Listen for Others", "Notifies in chat when other player(s) pop their totems as well.", "listenForOthers");
    extraPadding();


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

    if (SDK::getCurrentScreen() == "progress_screen") popsById.clear();
}

void TotemCounter::onPacketEvent(PacketEvent& event)
{
    auto eep = reinterpret_cast<EntityEventPacket*>(event.getPacket());
    // is enabled, notification on, packet is actorevent, ID is talismanactivate, server is not the hive (is there even a minigame that actually uses it?)
    if (!isEnabled() || !getOps<bool>("notifyOnPop") || eep->getId() != MinecraftPacketIds::ActorEvent || SDK::getServerIP() == "geo.hivebedrock.network") return;

    auto id = eep->RuntimeID;
    auto actor = SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeEntity(id, false);
    if (!actor) return Logger::debug("what the skibidi toilet");

    if (eep->EventID == ActorEvent::TalismanActivate)
    {

        // Workaround until I figure out how to properly filter packets.
        static std::unordered_map<uint64_t, std::chrono::steady_clock::time_point> lastPopAt;
        const auto now = std::chrono::steady_clock::now();
        auto& t = lastPopAt[id];
        if (now - t < std::chrono::milliseconds(100)) return;
        t = now;

        int pops = ++popsById[id];

        const std::string name = Utils::sanitizeName(actor->getNametag() ? *actor->getNametag() : std::string("blahaj"));

        if (id == SDK::clientInstance->getLocalPlayer()->getRuntimeIDComponent()->runtimeID) SDK::clientInstance->getGuiData()->displayClientMessage("[§bTo§dte§fm Co§dunt§ber§r] You popped §7" + std::to_string(pops) + "§r totem" + (pops > 1 ? "s" : "") + ".");
        else if (getOps<bool>("listenForOthers")) SDK::clientInstance->getGuiData()->displayClientMessage("[§bTo§dte§fm Co§dunt§ber§r] " + name + " popped §7" + std::to_string(pops) + "§r totem" + (pops > 1 ? "s" : "") + "!");
    } else if (eep->EventID == ActorEvent::Death)
    {
        SDK::clientInstance->getGuiData()->displayClientMessage(name + " died after popping " + std::to_string(popsById[id]) + " totems.");
        popsById[id] = 0;
    }

    // todo: remove this
    if (event.getPacket()->getId() == MinecraftPacketIds::SetActorMotion)
    {
        auto pkt = reinterpret_cast<SetActorMotionPacket*>(event.getPacket());
        if (pkt->mRuntimeID == SDK::clientInstance->getLocalPlayer()->getRuntimeIDComponent()->runtimeID)
        {
            SDK::clientInstance->getGuiData()->displayClientMessage("Received Motion: " + std::to_string(pkt->mMotion.x) + ", " + std::to_string(pkt->mMotion.y) + ", " + std::to_string(pkt->mMotion.z));
        }
    }
}

// I GIVE UP!!
void TotemCounter::onRender(RenderEvent& event) {
    if (!this->isEnabled() || !shouldRender) return;

    auto totemsStr = FlarialGUI::cached_to_string(totems);
    this->normalRender(35, totemsStr);
}
