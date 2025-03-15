#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Network/PacketEvent.hpp"
#include "SDK/Client/Network/Packet/LoginPacket.hpp"


class PlayerNotifier : public Module {
    int totalPlayers;
public:
    PlayerNotifier() : Module("Player Notifier", "Notifies you when a player is in the server.", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void defaultConfig() override {
        if (!this->settings.getSettingByName<float>("duration")){
            settings.addSetting<float>("duration", 80);
        }
        if (!this->settings.getSettingByName<std::string>("player1")) {
            settings.addSetting<std::string>("player1", "TheBarii");
            settings.addSetting<bool>("player1Enabled", true);
        }
    }
    void onEnable() override {
        Module::onEnable();
        Listen(this, TickEvent, &PlayerNotifier::onTick);
        Listen(this, PacketEvent, &PlayerNotifier::onPacketReceive);

    }

    void onDisable() override {
        Module::onDisable();
        Deafen(this, TickEvent, &PlayerNotifier::onTick);
        Deafen(this, PacketEvent, &PlayerNotifier::onPacketReceive);
    }

    void loadSettings() override {

        Module::loadSettings();
        for (const auto& settingPair : settings.settings) {
            const std::string& name = settingPair.first;
            if (name.find("player") != std::string::npos) {
                ++totalPlayers;
            }
        }
    }

    auto lastRun = std::chrono::steady_clock::now();

    void onTick(TickEvent& event) {
        constexpr double intervalSeconds = 5.0; // Change this to the desired interval

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - lastRun;

        if (elapsed.count() >= intervalSeconds) {
            lastRun = now; // Reset the timer

            std::unordered_map<mcUUID, PlayerListEntry>& playerMap = getPlayerMap();

            for (const auto& [uuid, entry] : playerMap) {
                for (int i = 0; i < totalPlayers; i++) {
                    if (this->settings.getSettingByName<bool>("player" + FlarialGUI::cached_to_string(i) + "Enabled")->value) {
                        if (entry.name.find(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value) != std::string::npos) {
                            FlarialGUI::Notify(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value + " is online!");
                        }
                    }
                }
            }
        }
    }

    /*
    void onPacketReceive(PacketEvent &event) {
        MinecraftPacketIds id = event.getPacket()->getId();
        MinecraftPacketIds::PlayerList
        if (id == MinecraftPacketIds::Login) {
            auto pkt = reinterpret_cast<LoginPacket*>(event.getPacket());
            pkt->
        }
    }
    */

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("General");
        this->addButton("Add new player", "", "ADD", [&]() {
            totalPlayers++;
            this->settings.addSetting<std::string>("player" + FlarialGUI::cached_to_string(totalPlayers), "player" + FlarialGUI::cached_to_string(totalPlayers));
            this->settings.addSetting<bool>("player" + FlarialGUI::cached_to_string(totalPlayers) + "Enabled", true);
        });
        this->addSlider("Recheck", "(Seconds) After how long should it recheck for players", this->settings.getSettingByName<float>("duration")->value, 500, 30, true);

        for (int i = 0; i < totalPlayers; i++) {
            this->addHeader(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value);
            this->addToggle("Enabled", "Should it notify you?", this->settings.getSettingByName<bool>("player" + FlarialGUI::cached_to_string(i) + "Enabled")->value);
            this->addTextBox("Player Name", "", this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value);
            this->extraPadding();
        }

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }
};
