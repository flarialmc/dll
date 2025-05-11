#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"
#include "../Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"


class AutoRQ : public Module {
private:
    bool triggered = false;
    std::string teamcolor= "THIS IS NEEDED TRUST!!!!!!!!!!!!!!";
    std::string Copy_CS = "THIS IS NEEDED EVEN MORE TRUST!!!!";
public:
    AutoRQ() : Module("Hive Utils", "Handy utilities for The Hive partnered server",
                      IDR_HIVE_PNG, "", false, false) {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, PacketEvent, &AutoRQ::onPacketReceive)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, PacketEvent, &AutoRQ::onPacketReceive)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<std::string>("map") == nullptr)
            settings.addSetting("map", (std::string)"");
        if (settings.getSettingByName<bool>("ReQ") == nullptr)
            settings.addSetting("ReQ", true);
        if (settings.getSettingByName<bool>("solo") == nullptr)
            settings.addSetting("solo", false);
        if (settings.getSettingByName<bool>("eliminated") == nullptr)
            settings.addSetting("eliminated", true);
        if (settings.getSettingByName<bool>("hub") == nullptr)
            settings.addSetting("hub", false);
        if (settings.getSettingByName<bool>("murderer") == nullptr)
            settings.addSetting("murderer", false);
        if (settings.getSettingByName<bool>("sheriff") == nullptr)
            settings.addSetting("sheriff", false);
        if (settings.getSettingByName<bool>("innocent") == nullptr)
            settings.addSetting("innocent", false);
        if (settings.getSettingByName<bool>("hider") == nullptr)
            settings.addSetting("hider", false);
        if (settings.getSettingByName<bool>("seeker") == nullptr)
            settings.addSetting("seeker", false);
        if (settings.getSettingByName<bool>("death") == nullptr)
            settings.addSetting("death", false);
        if (settings.getSettingByName<bool>("runner") == nullptr)
            settings.addSetting("runner", false);
        if (settings.getSettingByName<bool>("AutoMapAvoider") == nullptr)
            settings.addSetting("AutoMapAvoider", false);
        if (settings.getSettingByName<bool>("replace") == nullptr)
            settings.addSetting("replace", true);
        if (settings.getSettingByName<bool>("copyCS") == nullptr)
            settings.addSetting("copyCS", false);
        if (settings.getSettingByName<bool>("includecommand") == nullptr)
            settings.addSetting("includecommand", false);
        if (settings.getSettingByName<bool>("promomessage") == nullptr)
            settings.addSetting("promomessage", false);
        if (settings.getSettingByName<bool>("unlocks") == nullptr)
            settings.addSetting("unlocks", false);
        if (settings.getSettingByName<bool>("joined") == nullptr)
            settings.addSetting("joined", false);
        if (settings.getSettingByName<bool>("playermessage") == nullptr)
            settings.addSetting("playermessage", false);
        if (settings.getSettingByName<bool>("playermessageplus") == nullptr)
            settings.addSetting("playermessageplus", false);
        if (settings.getSettingByName<bool>("noteaming") == nullptr)
            settings.addSetting("noteaming", false);
        if (settings.getSettingByName<bool>("friendaccept") == nullptr)
            settings.addSetting("friendaccept", false);
        if (settings.getSettingByName<bool>("partyaccept") == nullptr)
            settings.addSetting("partyaccept", false);
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));
        this->addHeader("General");
        this->addToggle("Use /hub instead of /q", "", this->settings.getSettingByName<bool>("hub")->value);
        // this->addDropdown("Command to use", "Command to execute when somthing gets triggered",  std::vector<std::string>{"Re-Q same game", "Q a Random game", "Go back to the hub"}, this->settings.getSettingByName<std::string>("commandtouse")->value);

        this->addHeader("Auto Re Q");
        this->addToggle("Auto re-queue ", "Find a new game when the current game is over", this->settings.getSettingByName<bool>("ReQ")->value);
        this->addToggle("Solo mode ", "Re-Q when you finish a game or die and can't respawn.\nNot recomended while in a party.", this->settings.getSettingByName<bool>("solo")->value);
        this->addToggle("Team Elimination", "Re-Q when the team your on is fully ELIMINATED.", this->settings.getSettingByName<bool>("eliminated")->value);

        this->addHeader("Map avoider");

        this->addToggle("Map Avoider", "Automatically finds you a new game when a specific map has won the vote", this->settings.getSettingByName<bool>("AutoMapAvoider")->value);
        // this->addTextBox("Maps", " Input one or more maps using comma's.", settings.getSettingByName<std::string>("text")->value, 100);
        this->addButton("Add Map", "Add a map you want to avoid.", "Add Map", [this] {

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
                this->addTextBox(
                    "Map " + FlarialGUI::cached_to_string(i),
                    "Input a map you would like to avoid.",
                    settings.getSettingByName<std::string>(commandSettingName)->value
                );
            }
        }

        this->addHeader("Role Avoider");

        this->addHeader("Murder Mystery");
        this->addToggle("Murderer", "re q when you get murderer", this->settings.getSettingByName<bool>("murderer")->value);
        this->addToggle("Sheriff", "re q when you get sheriff", this->settings.getSettingByName<bool>("sheriff")->value);
        this->addToggle("Innocent", "re q when you get innocent", this->settings.getSettingByName<bool>("innocent")->value);

        this->addHeader("Hide and Seek");
        this->addToggle("Hider", "re q when you get hider", this->settings.getSettingByName<bool>("hider")->value);
        this->addToggle("Seeker", "re q when you get seeker", this->settings.getSettingByName<bool>("seeker")->value);

        this->addHeader("Deathrun");
        this->addToggle("Death", "re q when you get death", this->settings.getSettingByName<bool>("death")->value);
        this->addToggle("Runner", "re q when you get runner", this->settings.getSettingByName<bool>("runner")->value);

        this->addHeader("Copy Custom Server code");
        this->addToggle("Copy CS code", "Automatically copy custom server code to your clipboard", this->settings.getSettingByName<bool>("copyCS")->value);
        this->addToggle("Include command", "Include /cs when copying custom server code", this->settings.getSettingByName<bool>("includecommand")->value);

        this->addHeader("Debloat chat");
        this->addToggle("Promo message", "Removes all promo/info messages starting with [!]", this->settings.getSettingByName<bool>("promomessage")->value);
        this->addToggle("Unused Unlocks", "Removes 'You have unused unlocks in your Locker!'", this->settings.getSettingByName<bool>("unlocks")->value);
        this->addToggle("player joined", "Removes player joined message", this->settings.getSettingByName<bool>("joined")->value);
        this->addToggle("player message", "Removes all messages by non ranked players", this->settings.getSettingByName<bool>("playermessage")->value);
        this->addToggle("player message Hive+", "Removes all messages from Hive+ players", this->settings.getSettingByName<bool>("playermessageplus")->value);
        this->addToggle("No Teaming", "Removes No Teaming!", this->settings.getSettingByName<bool>("noteaming")->value);

        this->addHeader("Auto accept");
        this->addToggle("Friend request", "Automatically accept incoming friend requests.", this->settings.getSettingByName<bool>("friendaccept")->value);
        this->addToggle("Party request", "Automatically accept incoming party requests.", this->settings.getSettingByName<bool>("partyaccept")->value);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }


    void onPacketReceive(PacketEvent &event) {
        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::SetTitle) {
            auto *pkt = reinterpret_cast<SetTitlePacket *>(event.getPacket());
            if (this->settings.getSettingByName<bool>("ReQ")->value) {
                if (this->settings.getSettingByName<bool>("solo")->value) {


                    if (//pkt->text == "§cYou're a spectator!" || //this brobably isn't needed anymore
                        pkt->text == "§cYou died!" ||
                        pkt->text == "§7You're spectating the §as§eh§6o§cw§7!") {
                        reQ();

                    }
                }
            }
            if (this->settings.getSettingByName<bool>("copyCS")->value)
            {
                if (!pkt->text.empty() && pkt->text.find(Copy_CS) == std::string::npos){
                    if (pkt->text.length() >= 15 && pkt->text.substr(0, 13) == "§eJoin Code:") {
                        Copy_CS = pkt->text;
                        if (!this->settings.getSettingByName<bool>("includecommand")->value) {
                            ImGui::SetClipboardText(pkt->text.substr(17, pkt->text.length()).c_str());
                        }
                        else{
                            ImGui::SetClipboardText(("/cs " + pkt->text.substr(17, pkt->text.length())).c_str());
                        }
                        FlarialGUI::Notify("Compied CS Code to Clipboard");
                    }
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            if (this->settings.getSettingByName<bool>("ReQ")->value) {
                //if(!module->settings.getSettingByName<bool>("solo")->value) {
                if (pkt->message == "§c§l» §r§c§lGame OVER!") {
                    reQ();
                    return;
                }
                if (this->settings.getSettingByName<bool>("eliminated")->value){
                    if (pkt->message.length() > 27){
                        if (pkt->message.substr(12, 15) == "You are on the "){
                            teamcolor = pkt->message.substr(27, pkt->message.length() - 28);
                        }
                    }

                    if (pkt->message.find("§7has been §cELIMINATED§7!") != std::string::npos && pkt->message.find(teamcolor) != std::string::npos && !teamcolor.empty()){
                        reQ();
                        FlarialGUI::Notify("Your Team has been ELIMINATED");
                        return;
                    }
                }
                if (this->settings.getSettingByName<bool>("solo")->value) {

                    if (pkt->message.substr(0,48) == "§a§l» §r§eYou finished all maps and came in" || //gravity
                        pkt->message.substr(0,30) == "§a§l» §r§eYou finished in") { //deathrun
                        reQ();

                    }
                }
            }
            if (this->settings.getSettingByName<bool>("murderer")->value) {

                if (pkt->message == "§c§l» §r§c§lMurderer") {
                    reQ();
                    FlarialGUI::Notify("Found role Murderer");

                }
            }
            if (this->settings.getSettingByName<bool>("sheriff")->value) {

                if (pkt->message == "§9§l» §r§9§lSheriff") {
                    reQ();
                    FlarialGUI::Notify("Found role Sheriff");

                }
            }
            if (this->settings.getSettingByName<bool>("innocent")->value) {

                if (pkt->message == "§a§l» §r§a§lInnocent") {
                    reQ();
                    FlarialGUI::Notify("Found role Innocent");

                }
            }
            if (this->settings.getSettingByName<bool>("death")->value) {

                if (pkt->message == "§d§l» §r§bYou are a §cDeath") {
                    reQ();
                    FlarialGUI::Notify("Found role Death");

                }
            }
            if (this->settings.getSettingByName<bool>("runner")->value) {

                if (pkt->message == "§d§l» §r§bYou are a §aRunner") {
                    reQ();
                    FlarialGUI::Notify("Found role Runner");

                }
            }
            if (this->settings.getSettingByName<bool>("hider")->value) {

                if (pkt->message == "§e§l» §rYou are a §eHIDER") {
                    reQ();
                    FlarialGUI::Notify("Found role Hider");

                }
            }
            if (this->settings.getSettingByName<bool>("seeker")->value) {

                if (pkt->message == "§c§l» §rYou are a §cSEEKER") {
                    reQ();
                    FlarialGUI::Notify("Found role Seeker");

                }
            }
            if (this->settings.getSettingByName<bool>("AutoMapAvoider")->value) {
                for (int i = 0; i < totalmaps; ++i)
                {
                    std::string count;
                    /*if (i > 0) */count = "-" + FlarialGUI::cached_to_string(i);

                    std::string map = this->settings.getSettingByName<std::string>("map" + count)->value;
                    std::transform(map.begin(), map.end(), map.begin(), [](unsigned char c) {
                        return std::tolower(c);
                    });
                    if (map.empty()) continue;

                    std::string message =  pkt->message;
                    std::transform(message.begin(), message.end(), message.begin(), [](unsigned char c) {
                        return std::tolower(c);
                    });

                    if (message.find("§b§l» §r§e" + map)!= std::string::npos)
                    {
                        FlarialGUI::Notify("Found Map: " + map);
                        ImGui::SetClipboardText(pkt->message.c_str());
                        reQ();
                        return;
                    }
                }
            }

            if (this->settings.getSettingByName<bool>("promomessage")->value) {
                if (pkt->message.find("§6[§e!§6]")!= std::string::npos){
                    event.cancel();
                }
            }

            if (this->settings.getSettingByName<bool>("unlocks")->value) {
                if (pkt->message == "§a§l» §rYou have unused unlocks in your Locker!"){
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("joined")->value) {
                std::regex pattern("joined\\. §8\\[\\d+/\\d+\\]");

                if (std::regex_search(pkt->message, pattern)) {
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("playermessage")->value) {
                if (pkt->message.contains(" §7§l» §r") && pkt->message.substr(0, 3) == "§7"){
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("playermessageplus")->value) {
                if (pkt->message.contains("§8 [§a+§8] §7§l» §r")){
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("noteaming")->value) {
                if (pkt->message == "§c§l» §r§c§lNo teaming! §r§6Teamers will be banned."){
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("friendaccept")->value) {
                if(pkt->message.substr(0, 40) == "§aYou received a friend invite from §b"){

                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                    command_packet->command = "/f accept " + pkt->message.substr(40,  pkt->message.length()-44);

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;
                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                    FlarialGUI::Notify("Accepted friend invite from: " + pkt->message.substr(40,  pkt->message.length()-44));
                }
            }
            if (this->settings.getSettingByName<bool>("partyaccept")->value) {
                if (pkt->message.find("§b wants you to join their party!")!= std::string::npos){

                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                    command_packet->command = "/p accept " + pkt->message.substr(6, pkt->message.length()-40);

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;
                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                    FlarialGUI::Notify("Accepted party invite from: " + pkt->message.substr(6, pkt->message.length()-40));
                }
            }
        }
    }

    void reQ() {
        if (!this->settings.getSettingByName<bool>("hub")->value) {
            FlarialGUI::Notify("Finding a new game of " + HiveModeCatcherListener::fullgamemodename);

            std::shared_ptr<Packet> packet = SDK::createPacket(77);
            auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
            command_packet->command = "/q " + HiveModeCatcherListener::currentGame;

            command_packet->origin.type = CommandOriginType::Player;

            command_packet->InternalSource = true;
            SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
        } else {
            FlarialGUI::Notify("Executing command /hub");
            std::shared_ptr<Packet> packet = SDK::createPacket(77);
            auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

            command_packet->command = "/hub";

            command_packet->origin.type = CommandOriginType::Player;

            command_packet->InternalSource = true;

            SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
        }
    }
};

