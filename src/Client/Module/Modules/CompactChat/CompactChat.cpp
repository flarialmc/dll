#include "CompactChat.hpp"

#include "Events/EventManager.hpp"
#include "SDK/Client/Render/GuiMessage.hpp"
#include "Modules/Nick/NickModule.hpp"

void CompactChat::onEnable() {
    Listen(this, PacketEvent, &CompactChat::onPacketReceive)
    Listen(this, ChatScreenControllerTickEvent, &CompactChat::onChatScreenControllerTickEvent)
    Module::onEnable();
}

void CompactChat::onDisable() {
    Deafen(this, PacketEvent, &CompactChat::onPacketReceive)
    Deafen(this, ChatScreenControllerTickEvent, &CompactChat::onChatScreenControllerTickEvent)
    Module::onDisable();
}

void CompactChat::defaultConfig() {
    Module::defaultConfig("core");
    setDef("format", (std::string)"{msg} {count}");
    setDef("countTextColor", (std::string)"Dark Purple");
    setDef("showBrackets", true);
    setDef("bracketStyle", (std::string)"( )");
    setDef("bracketColor", (std::string)"White");
}

void CompactChat::settingsRender(float settingsOffset) {


    initSettingsPage();

    addHeader("Compact Chat");
    addTextBox("Format", "{msg} for message, {count} for count.", 0, "format");
    addToggle("Show Brackets", "", "showBrackets");
    addConditionalDropdown(getOps<bool>("showBrackets"), "Bracket Style", "", std::vector<std::string>{
                    "( )",
                    "{ }",
                    "[ ]",
                    "< >",
                }, "bracketStyle", true);

    extraPadding();

    addHeader("Colors");

    std::vector<std::string> colors = {
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
    };

    addDropdown("Count Text Color", "", colors, "countTextColor", true);
    addConditionalDropdown(getOps<bool>("showBrackets"), "Bracket Color", "", colors, "bracketColor", true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

std::string CompactChat::getCountText(int count) {

    auto countCol = NickModule::textColors.find(getOps<std::string>("countTextColor"));

    if (countCol == NickModule::textColors.end()) {
        settings.setValue<std::string>("countTextColor", "Dark Purple");
        countCol = NickModule::textColors.find(getOps<std::string>("countTextColor"));
    }

    auto bracketCol = NickModule::textColors.find(getOps<std::string>("bracketColor"));

    if (bracketCol == NickModule::textColors.end()) {
        settings.setValue<std::string>("bracketColor", "White");
        bracketCol = NickModule::textColors.find(getOps<std::string>("bracketColor"));
    }

    if (getOps<bool>("showBrackets")) {
        if (getOps<std::string>("bracketStyle").empty()) settings.setValue<std::string>("bracketStyle", "( )");
        std::string bracket = getOps<std::string>("bracketStyle");
        return std::format("§r{}{}§r{}x{}§r{}{}§r", bracketCol->second, bracket[0], countCol->second, count, bracketCol->second, bracket[2]);
    }
    return std::format("§r{}x{}§r", countCol->second, count);
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

            mesgVec.back().fullMsg = format;

            refreshChat = true;
            event.cancel();
        }
        else {
            count = 1;
            mesg = "";
        }

    }
}

void CompactChat::onChatScreenControllerTickEvent(ChatScreenControllerTickEvent& event) {
    if (refreshChat) {
        event.getChatScreenController()->refreshChatMessages = true;
        refreshChat = false;
    }
}