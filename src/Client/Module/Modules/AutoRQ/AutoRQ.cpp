#include "AutoRQ.hpp"

#include "Events/EventManager.hpp"
#include "SDK/Client/Network/Packet/SetTitlePacket.hpp"

void AutoRQ::onEnable() {
    Listen(this, PacketEvent, &AutoRQ::onPacketReceive)
    Module::onEnable();
}

void AutoRQ::onDisable() {
    Deafen(this, PacketEvent, &AutoRQ::onPacketReceive)
    Module::onDisable();
}

void AutoRQ::defaultConfig() {
    Module::defaultConfig("core");
    setDef("map", (std::string)"");
    setDef("ReQ", true);
    setDef("solo", false);
    setDef("eliminated", true);
    setDef("hub", false);
    setDef("murderer", false);
    setDef("sheriff", false);
    setDef("innocent", false);
    setDef("hider", false);
    setDef("seeker", false);
    setDef("death", false);
    setDef("runner", false);
    setDef("AutoMapAvoider", false);
    setDef("replace", true);
    setDef("copyCS", false);
    setDef("includecommand", false);
    setDef("promomessage", false);
    setDef("unlocks", false);
    setDef("joined", false);
    setDef("playermessage", false);
    setDef("playermessageplus", false);
    setDef("noteaming", false);
    setDef("friendaccept", false);
    setDef("partyaccept", false);
    
}

void AutoRQ::settingsRender(float settingsOffset) {
    initSettingsPage();
    addHeader("General");
    addToggle("Use /hub instead of /q", "", "hub");
    // this->addDropdown("Command to use", "Command to execute when somthing gets triggered",  std::vector<std::string>{"Re-Q same game", "Q a Random game", "Go back to the hub"}, getOps<std::string>("commandtouse"));

    addHeader("Auto Re Q");
    addToggle("Auto re-queue ", "Find a new game when the current game is over", "ReQ");
    addToggle("Solo mode ", "Re-Q when you finish a game or die and can't respawn.\nNot recomended while in a party.", "solo");
    addToggle("Team Elimination", "Re-Q when the team your on is fully ELIMINATED.", "eliminated");

    addHeader("Map avoider");

    addToggle("Map Avoider", "Automatically finds you a new game when a specific map has won the vote", "AutoMapAvoider");
    // this->addTextBox("Maps", " Input one or more maps using comma's.", getOps<std::string>("text"), 100);
    addButton("Add Map", "Add a map you want to avoid.", "Add Map", [this] {

        // std::string keybindName = "keybind-" + FlarialGUI::cached_to_string(totalKeybinds);
        std::string commandName = "map-" + FlarialGUI::cached_to_string(totalmaps);

        // this->settings.addSetting(keybindName, (std::string)"");
        this->settings.addSetting(commandName, (std::string)"");


        int i = totalmaps;

        Client::SaveSettings();
        FlarialGUI::Notify("New textbox created, input a map to avoid!");
        totalmaps++;
    });

    for (int i = 0; i < totalmaps; ++i) {
        std::string commandSettingName = "map-" + FlarialGUI::cached_to_string(i);

        if (settings.getSettingByName<std::string>(commandSettingName) != nullptr) {
            addTextBox(
                "Map " + FlarialGUI::cached_to_string(i),
                "Input a map you would like to avoid.",
                settings.getSettingByName<std::string>(commandSettingName)->value
            );
        }
    }

    addHeader("Role Avoider");

    addHeader("Murder Mystery");
    addToggle("Murderer", "Re Q when you get murderer", "murderer");
    addToggle("Sheriff", "Re Q when you get sheriff", "sheriff");
    addToggle("Innocent", "Re Q when you get innocent", "innocent");

    addHeader("Hide and Seek");
    addToggle("Hider", "Re Q when you get hider", "hider");
    addToggle("Seeker", "Re Q when you get seeker", "seeker");

    addHeader("Deathrun");
    addToggle("Death", "Re Q when you get death", "death");
    addToggle("Runner", "Re Q when you get runner", "runner");

    addHeader("Copy Custom Server code");
    addToggle("Copy CS code", "Automatically copy custom server code to your clipboard", "copyCS");
    addToggle("Include command", "Include /cs when copying custom server code", "includecommand");

    addHeader("Debloat chat");
    addToggle("Promo message", "Removes all promo/info messages starting with [!]", "promomessage");
    addToggle("Unused Unlocks", "Removes 'You have unused unlocks in your Locker!'", "unlocks");
    addToggle("Player joined", "Removes player joined message", "joined");
    addToggle("Player message", "Removes all messages by non ranked players", "playermessage");
    addToggle("Player message Hive+", "Removes all messages from Hive+ players", "playermessageplus");
    addToggle("No Teaming", "Removes No Teaming!", "noteaming");

    addHeader("Auto accept");
    addToggle("Friend request", "Automatically accept incoming friend requests.", "friendaccept");
    addToggle("Party request", "Automatically accept incoming party requests.", "partyaccept");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void AutoRQ::onPacketReceive(PacketEvent &event) {
    if (!this->isEnabled()) return;
    MinecraftPacketIds id = event.getPacket()->getId();

    if (id == MinecraftPacketIds::SetTitle) {
        auto* pkt = reinterpret_cast<SetTitlePacket*>(event.getPacket());
        if (getOps<bool>("ReQ")) {
            if (getOps<bool>("solo")) {


                if (//pkt->text == "§cYou're a spectator!" || //this brobably isn't needed anymore
                    pkt->text == "§cYou died!" ||
                    pkt->text == "§7You're spectating the §as§eh§6o§cw§7!") {
                    reQ();

                }
            }
        }
        if (getOps<bool>("copyCS"))
        {
            if (!pkt->text.empty() && pkt->text.find(Copy_CS) == std::string::npos) {
                if (pkt->text.length() >= 15 && pkt->text.substr(0, 13) == "§eJoin Code:") {
                    Copy_CS = pkt->text;
                    if (!getOps<bool>("includecommand")) {
                        ImGui::SetClipboardText(pkt->text.substr(17, pkt->text.length()).c_str());
                    }
                    else {
                        ImGui::SetClipboardText(("/cs " + pkt->text.substr(17, pkt->text.length())).c_str());
                    }
                    FlarialGUI::Notify("Compied CS Code to Clipboard");
                }
            }
        }
    }
    if (id == MinecraftPacketIds::Text) {
        auto* pkt = reinterpret_cast<TextPacket*>(event.getPacket());
        if (getOps<bool>("ReQ")) {
            //if(!module->getOps<bool>("solo")) {
            if (pkt->message == "§c§l» §r§c§lGame OVER!") {
                reQ();
                return;
            }
            if (getOps<bool>("eliminated")) {
                if (pkt->message.length() > 27) {
                    if (pkt->message.substr(12, 15) == "You are on the ") {
                        teamcolor = pkt->message.substr(27, pkt->message.length() - 28);
                    }
                }

                if (pkt->message.find("§7has been §cELIMINATED§7!") != std::string::npos && pkt->message.find(teamcolor) != std::string::npos && !teamcolor.empty()) {
                    reQ();
                    FlarialGUI::Notify("Your Team has been ELIMINATED");
                    return;
                }
            }
            if (getOps<bool>("solo")) {

                if (pkt->message.substr(0, 48) == "§a§l» §r§eYou finished all maps and came in" || //gravity
                    pkt->message.substr(0, 30) == "§a§l» §r§eYou finished in") { //deathrun
                    reQ();

                }
            }
        }
        if (getOps<bool>("murderer")) {

            if (pkt->message == "§c§l» §r§c§lMurderer") {
                reQ();
                FlarialGUI::Notify("Found role Murderer");

            }
        }
        if (getOps<bool>("sheriff")) {

            if (pkt->message == "§9§l» §r§9§lSheriff") {
                reQ();
                FlarialGUI::Notify("Found role Sheriff");

            }
        }
        if (getOps<bool>("innocent")) {

            if (pkt->message == "§a§l» §r§a§lInnocent") {
                reQ();
                FlarialGUI::Notify("Found role Innocent");

            }
        }
        if (getOps<bool>("death")) {

            if (pkt->message == "§d§l» §r§bYou are a §cDeath") {
                reQ();
                FlarialGUI::Notify("Found role Death");

            }
        }
        if (getOps<bool>("runner")) {

            if (pkt->message == "§d§l» §r§bYou are a §aRunner") {
                reQ();
                FlarialGUI::Notify("Found role Runner");

            }
        }
        if (getOps<bool>("hider")) {

            if (pkt->message == "§e§l» §rYou are a §eHIDER") {
                reQ();
                FlarialGUI::Notify("Found role Hider");

            }
        }
        if (getOps<bool>("seeker")) {

            if (pkt->message == "§c§l» §rYou are a §cSEEKER") {
                reQ();
                FlarialGUI::Notify("Found role Seeker");

            }
        }
        if (getOps<bool>("AutoMapAvoider")) {
            for (int i = 0; i < totalmaps; ++i)
            {
                std::string count;
                /*if (i > 0) */count = "-" + FlarialGUI::cached_to_string(i);

                std::string map = this->settings.getSettingByName<std::string>("map" + count)->value;
                std::transform(map.begin(), map.end(), map.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
                if (map.empty()) continue;

                std::string message = pkt->message;
                std::transform(message.begin(), message.end(), message.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });

                if (message.find("§b§l» §r§e" + map) != std::string::npos)
                {
                    FlarialGUI::Notify("Found Map: " + map);
                    ImGui::SetClipboardText(pkt->message.c_str());
                    reQ();
                    return;
                }
            }
        }

        if (getOps<bool>("promomessage")) {
            if (pkt->message.find("§6[§e!§6]") != std::string::npos) {
                event.cancel();
            }
        }

        if (getOps<bool>("unlocks")) {
            if (pkt->message == "§a§l» §rYou have unused unlocks in your Locker!") {
                event.cancel();
            }
        }
        if (getOps<bool>("joined")) {
            std::regex pattern("joined\\. §8\\[\\d+/\\d+\\]");

            if (std::regex_search(pkt->message, pattern)) {
                event.cancel();
            }
        }
        if (getOps<bool>("playermessage")) {
            if (pkt->message.contains(" §7§l» §r") && pkt->message.substr(0, 3) == "§7") {
                event.cancel();
            }
        }
        if (getOps<bool>("playermessageplus")) {
            if (pkt->message.contains("§8 [§a+§8] §7§l» §r")) {
                event.cancel();
            }
        }
        if (getOps<bool>("noteaming")) {
            if (pkt->message == "§c§l» §r§c§lNo teaming! §r§6Teamers will be banned.") {
                event.cancel();
            }
        }
        if (getOps<bool>("friendaccept")) {
            if (pkt->message.substr(0, 40) == "§aYou received a friend invite from §b") {

                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                command_packet->command = "/f accept " + pkt->message.substr(40, pkt->message.length() - 44);

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;
                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                FlarialGUI::Notify("Accepted friend invite from: " + pkt->message.substr(40, pkt->message.length() - 44));
            }
        }
        if (getOps<bool>("partyaccept")) {
            if (pkt->message.find("§b wants you to join their party!") != std::string::npos) {

                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                command_packet->command = "/p accept " + pkt->message.substr(6, pkt->message.length() - 40);

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;
                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                FlarialGUI::Notify("Accepted party invite from: " + pkt->message.substr(6, pkt->message.length() - 40));
            }
        }
    }
}

void AutoRQ::reQ() {
    if (!this->isEnabled()) return;
    if (!getOps<bool>("hub")) {
        FlarialGUI::Notify("Finding a new game of " + HiveModeCatcherListener::fullgamemodename);

        std::shared_ptr<Packet> packet = SDK::createPacket(77);
        auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
        command_packet->command = "/q " + HiveModeCatcherListener::currentGame;

        command_packet->origin.type = CommandOriginType::Player;

        command_packet->InternalSource = true;
        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
    }
    else {
        FlarialGUI::Notify("Executing command /hub");
        std::shared_ptr<Packet> packet = SDK::createPacket(77);
        auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

        command_packet->command = "/hub";

        command_packet->origin.type = CommandOriginType::Player;

        command_packet->InternalSource = true;

        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
    }
}
