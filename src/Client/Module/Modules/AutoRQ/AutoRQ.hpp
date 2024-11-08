#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"
#include "../Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"


class AutoRQ : public Module {
private:
    bool triggered = false;
public:
    AutoRQ() : Module("Auto RE Q", "Automatically requeues into a game (Hive)",
                      IDR_RE_Q_PNG, "") {
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

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("command") == nullptr)
            settings.addSetting("command", (std::string)"");
        if (settings.getSettingByName<bool>("solo") == nullptr)
            settings.addSetting("solo", false);
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
        if (settings.getSettingByName<std::string>("text") == nullptr) 
            settings.addSetting("text", (std::string) "Input maps, like this");
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
        this->addToggle("Solo mode ", "Re-Q when you finish or die in a game. pls dont use while in a party", this->settings.getSettingByName<bool>("solo")->value);
        this->addToggle("Use /hub instead of /q", "", this->settings.getSettingByName<bool>("hub")->value);

        this->addHeader("Murder Mystery");
        this->addToggle("Murder", "re q when you get murder", this->settings.getSettingByName<bool>("murderer")->value);
        this->addToggle("Sheriff", "re q when you get sheriff", this->settings.getSettingByName<bool>("sheriff")->value);
        this->addToggle("Innocent", "re q when you get innocent", this->settings.getSettingByName<bool>("innocent")->value);

        this->addHeader("Hide and Seek");
        this->addToggle("Hider", "re q when you get hider", this->settings.getSettingByName<bool>("hider")->value);
        this->addToggle("Seeker", "re q when you get seeker", this->settings.getSettingByName<bool>("seeker")->value);

        this->addHeader("Deathrun");
        this->addToggle("Death", "re q when you get death", this->settings.getSettingByName<bool>("death")->value);
        this->addToggle("Runner", "re q when you get runner", this->settings.getSettingByName<bool>("runner")->value);

        this->addHeader("Map avoider");


        this->addToggle("Avoid", "", this->settings.getSettingByName<bool>("AutoMapAvoider")->value);
        this->addTextBox("Maps", "Avoid Maps (Hive). Input one or more maps using comma's.", settings.getSettingByName<std::string>("text")->value);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }


    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }


    void onPacketReceive(PacketEvent &event) {
        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::SetTitle) {
            if (module->settings.getSettingByName<bool>("solo")->value) {
                auto *pkt = reinterpret_cast<SetTitlePacket *>(event.getPacket());

                if (//pkt->text == "§cYou're a spectator!" || //this brobably isn't needed anymore
                    pkt->text == "§cYou died!" ||
                    pkt->text == "§7You're spectating the §as§eh§6o§cw§7!") {
                    reQ();

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (module->settings.getSettingByName<bool>("solo")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message.substr(0,48) == "§a§l» §r§eYou finished all maps and came in" || //gravity
                    pkt->message.substr(0,30) == "§a§l» §r§eYou finished in") { //deathrun
                    reQ();

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (this->settings.getSettingByName<bool>("murderer")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§c§l» §r§c§lMurderer") {
                    reQ();
                    FlarialGUI::Notify("Found role Murderer");

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (this->settings.getSettingByName<bool>("sheriff")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§9§l» §r§9§lSheriff") {
                    reQ();
                    FlarialGUI::Notify("Found role Sheriff");

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (this->settings.getSettingByName<bool>("innocent")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§a§l» §r§a§lInnocent") {
                    reQ();
                    FlarialGUI::Notify("Found role Innocent");

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (this->settings.getSettingByName<bool>("death")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§d§l» §r§bYou are a §cDeath") {
                    reQ();
                    FlarialGUI::Notify("Found role Death");

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (this->settings.getSettingByName<bool>("runner")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§d§l» §r§bYou are a §aRunner") {
                    reQ();
                    FlarialGUI::Notify("Found role Runner");

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (this->settings.getSettingByName<bool>("hider")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§e§l» §rYou are a §eHIDER") {
                    reQ();
                    FlarialGUI::Notify("Found role Hider");

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (this->settings.getSettingByName<bool>("seeker")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§c§l» §rYou are a §cSEEKER") {
                    reQ();
                    FlarialGUI::Notify("Found role Seeker");

                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if (this->settings.getSettingByName<bool>("AutoMapAvoider")->value) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
                std::string maps_to_avoid = this->settings.getSettingByName<std::string>("text")->value;
                std::stringstream ss(maps_to_avoid);
                std::string fixed_value = "§b§l» §r§e";
                std::vector<int> vect;
                std::vector<std::string> result;
                if (pkt->message.substr(0, 15) == fixed_value) {
                    while (ss.good()) {
                        std::string substr;
                        getline(ss, substr, ',');
                        ltrim(substr);
                        result.push_back(substr);
                    }

                    for (std::size_t i = 0; i < result.size(); i++) {
                        std::string evaluate_string = fixed_value + result[i];
                        std::transform(evaluate_string.begin(), evaluate_string.end(), evaluate_string.begin(),
                                       ::tolower);
                        std::transform(pkt->message.begin(), pkt->message.end(), pkt->message.begin(), ::tolower);
                        if (pkt->message.substr(0, (evaluate_string.length())) == evaluate_string) {
                            reQ();
                            FlarialGUI::Notify("Found map: " + result[i]);
                        }
                    }
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            //if(!module->settings.getSettingByName<bool>("solo")->value) {
            if (pkt->message == "§c§l» §r§c§lGame OVER!") {
                reQ();
                return;
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

