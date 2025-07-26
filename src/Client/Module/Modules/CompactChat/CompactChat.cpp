#include "CompactChat.hpp"

#include "Events/EventManager.hpp"
#include "SDK/Client/Render/GuiMessage.hpp"
#include "Modules/Nick/NickModule.hpp"

void CompactChat::onEnable() {
    Listen(this, PacketEvent, &CompactChat::onPacketReceive)
    Module::onEnable();
}

void CompactChat::onDisable() {
    Deafen(this, PacketEvent, &CompactChat::onPacketReceive)
    Module::onDisable();
}

void CompactChat::defaultConfig() {
    Module::defaultConfig("core");
    setDef("format", (std::string)"{msg} ({count})");
    setDef("countTextColor", (std::string)"Dark Purple");
}

void CompactChat::settingsRender(float settingsOffset) {


    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Compact Chat");
    addTextBox("Format", "{msg} for message, {count} for count.", 0, "format");
    addDropdown("Count Text Color", "", std::vector<std::string>{
                    "White",
                    "Black",
                    "Netherite",
                    "Gray",
                    "Iron",
                    "Quartz",
                    "Dark Gray",
                    "Red",
                    "Dark Red",
                    "Copper",
                    "Redstone",
                    "Gold",
                    "Material Gold",
                    "Green",
                    "Dark Green",
                    "Emerald",
                    "Diamond",
                    "Aqua",
                    "Dark Aqua",
                    "Blue",
                    "Dark Blue",
                    "Lapis",
                    "Light Purple",
                    "Dark Purple",
                    "Amethyst"
                }, "countTextColor", true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

std::string CompactChat::getCountText(int count) {
    if (textColors.empty()) {
        std::shared_ptr<NickModule> nickModule = std::dynamic_pointer_cast<NickModule>(ModuleManager::getModule("Nick"));
        textColors = nickModule->textColors;
    }

    auto it = textColors.find(getOps<std::string>("countTextColor"));

    if (it == textColors.end()) {
        settings.setValue<std::string>("countTextColor", "Dark Purple");
        it = textColors.find(getOps<std::string>("countTextColor"));
    }

    return std::format("{}x{}§r", it->second, count);
}

void CompactChat::replace(std::string& str, std::string from, std::string to) {
    size_t pos = str.find(from);
    if (pos != std::string::npos) {
        str.erase(pos, from.length());
        str.insert(pos, to);
    }
}

void CompactChat::onPacketReceive(PacketEvent& event) {
    if (!this->isEnabled()) return;
    MinecraftPacketIds id = event.getPacket()->getId();

    if (id == MinecraftPacketIds::Text) {
        auto* pkt = reinterpret_cast<TextPacket*>(event.getPacket());
        std::vector<GuiMessage>& mesgVec = SDK::clientInstance->getGuiData()->getGuiMessages();

        if (mesgVec.empty()) return;

        if (mesg.empty()) mesg = mesgVec.back().fullMsg;

        if (pkt->message == mesgVec.back().msg) {
            count += 1;

            std::string format = getOps<std::string>("format");

            replace(format, "{msg}", mesg);
            replace(format, "{count}", getCountText(count));

            mesgVec.back().fullMsg = "[§c!§r] " + format;
            event.cancel();
        }
        else {
            count = 1;
            mesg = "";
        }

    }
}
