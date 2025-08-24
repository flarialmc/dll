#include "NickModule.hpp"

#include "Events/EventManager.hpp"

NickModule::NickModule(): Module("Nick",
                                 "Hides your username and replace it with something else.\nWorks everywhere (chat, pause, third person, etc)\nOther people will not be able to see your nick.",
                                 IDR_ICOGNITO_PNG, "")
{
    
}

void NickModule::onEnable()
{
    Listen(this, DrawTextEvent, &NickModule::onDrawText)
    Listen(this, RaknetTickEvent, &NickModule::onRaknetTick)
    Listen(this, TickEvent, &NickModule::onTick)
    Module::onEnable();
}

void NickModule::onDisable()
{
    Deafen(this, DrawTextEvent, &NickModule::onDrawText)
    Deafen(this, TickEvent, &NickModule::onTick)
    std::string val = original;
    std::string val2;

    if (SDK::clientInstance)
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            if (original2 != *SDK::clientInstance->getLocalPlayer()->getNametag()) {
                original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
                backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag();
            }
            if (original2 == getOps<std::string>("nick")) original2 = backupOri;

            val2 = original2;

            SDK::clientInstance->getLocalPlayer()->setNametag(&val2);
            SDK::clientInstance->getLocalPlayer()->getPlayerName() = val;
        }

    Module::onDisable();
}

void NickModule::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("nick", (std::string)"Flarial User");
    setDef("bold", false);
    setDef("obfuscated", false);
    setDef("textColor", (std::string)"White");
    
}

void NickModule::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Nickname");
    addTextBox("Nickname", "", 0, "nick");
    addToggle("Bold", "", "bold");
    addToggle("Obfuscated", "", "obfuscated");
    addDropdown("Text Color", "", std::vector<std::string>{
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
                }, "textColor", true);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void NickModule::onRaknetTick(RaknetTickEvent& event)
{
    //        if (module->isEnabled()) {
    //            std::string serverIP = SDK::getServerIP();
    //            if(serverIP.find("cubecraft") != std::string::npos) {
    //                if(!module->restricted) {
    //                    FlarialGUI::Notify("Can't use Nick on " + serverIP); // TODO: move restrictions to API
    //                    module->restricted = true;
    //                }
    //            } else {
    //                module->restricted = false;
    //            }
    //        }
}

void NickModule::onDrawText(DrawTextEvent& event)
{
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer())
        return;

    if (original.empty())
        original = SDK::clientInstance->getLocalPlayer()->getPlayerName();

    if (this->isEnabled() && !this->restricted) {
        const std::string& localPlayerName = original;
        size_t pos = event.getText()->find(localPlayerName);

        auto it = textColors.find(getOps<std::string>("textColor"));

        if (it == textColors.end()) {
            settings.setValue<std::string>("textColor", "White");
            it = textColors.find(getOps<std::string>("textColor"));
        }

        std::string prefix = it->second;

        prefix += "§o";
        if (getOps<bool>("bold")) prefix += "§l";
        if (getOps<bool>("obfuscated")) prefix += "§k";

        if (pos != std::string::npos) {
            std::string faketxt = *event.getText();
            faketxt.replace(pos, localPlayerName.length(),
                            prefix + String::removeColorCodes(getOps<std::string>("nick")) + "§r");
            *event.getText() = faketxt;
        }
    }
}

void NickModule::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) return;

    if (original.empty())
        original = player->getPlayerName();

    if (original2.empty() || backupOri.empty()) {
        original2 = *player->getNametag();
        backupOri = original2;
    }

    bool currentlyEnabled = this->isEnabled();
    if (enabled != currentlyEnabled) {
        enabled = currentlyEnabled;
        if (!enabled)
            original2 = *player->getNametag();
    }

    if (enabled && !this->restricted) {
        std::string prefix = textColors.find(getOps<std::string>("textColor"))->second;

        prefix += "§o";
        if (getOps<bool>("bold")) prefix += "§l";
        if (getOps<bool>("obfuscated")) prefix += "§k";

        std::string val = prefix + String::removeColorCodes(getOps<std::string>("nick")) + "§r";
        player->setNametag(&val);
        player->getPlayerName() = val;
    }
    else {
        if (original2 == getOps<std::string>("nick"))
            original2 = backupOri;

        player->setNametag(&original2);
        player->getPlayerName() = original;
    }
}
