#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Network/PacketEvent.hpp"
#include "SDK/Client/Network/Packet/LoginPacket.hpp"


class PlayerNotifier : public Module {
    int totalPlayers = 0;
public:
    PlayerNotifier() : Module("Player Notifier", "Notifies you when a player is in the server.", IDR_CURSOR_PNG, "P") {
        Module::setup();
        defaultConfig();
        loadSettings();

    };

    void defaultConfig() override {
        Module::defaultConfig();
        if (!this->settings.getSettingByName<float>("duration")){
            settings.addSetting<float>("duration", 80);
        }
        if (!this->settings.getSettingByName<std::string>("player0")) {
            settings.addSetting<std::string>("player0", "TheBarii");
            settings.addSetting<bool>("player0Enabled", true);
        }

        if (!this->settings.getSettingByName<std::string>("keybind")) {
            settings.addSetting<std::string>("keybind", "P");
        }
    }
    void onEnable() override {
        Module::onEnable();
        Listen(this, TickEvent, &PlayerNotifier::onTick);
        Listen(this, KeyEvent, &PlayerNotifier::onKey)
    }

    void onDisable() override {
        Module::onDisable();
        Deafen(this, TickEvent, &PlayerNotifier::onTick);
        Deafen(this, KeyEvent, &PlayerNotifier::onKey);
    }

    void onSetup() override {
        keybindActions.clear();
        keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
            check();
            return {};
        });
    }

    void loadSettings() override {

        Module::loadSettings();
        for (const auto& settingPair : settings.settings) {
            const std::string& name = settingPair.first;
            if (name.find("player") != std::string::npos && name.find("Enabled") == std::string::npos) {
                ++totalPlayers;
            }
        }

    }

    std::chrono::time_point<std::chrono::high_resolution_clock> lastRun = std::chrono::steady_clock::now();
    bool first = true;

    void check() {
        std::unordered_map<mcUUID, PlayerListEntry>& playerMap = SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap();

        for (const auto& [uuid, entry] : playerMap) {
            for (int i = 0; i < totalPlayers; i++) {
                if(!this->settings.getSettingByName<bool>("player" + FlarialGUI::cached_to_string(i) + "Enabled")) continue;
                if (this->settings.getSettingByName<bool>("player" + FlarialGUI::cached_to_string(i) + "Enabled")->value) {
                    if (entry.name.find(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value) != std::string::npos) {
                        FlarialGUI::Notify(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value + " is online!");
                    }
                }
            }
        }
    }

    void onTick(TickEvent& event) {
        double intervalSeconds = this->settings.getSettingByName<float>("duration")->value;

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - lastRun;

        if ((elapsed.count() >= intervalSeconds || first) && SDK::getCurrentScreen() == "hud_screen") {
            lastRun = now;
            first = false;

            check();

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
            this->settings.addSetting<std::string>("player" + FlarialGUI::cached_to_string(totalPlayers), "player" + FlarialGUI::cached_to_string(totalPlayers));
            this->settings.addSetting<bool>("player" + FlarialGUI::cached_to_string(totalPlayers) + "Enabled", true);
            totalPlayers++;
        });
        this->addSlider("Re-check", "(Seconds) After how long should it re-check for players", this->settings.getSettingByName<float>("duration")->value, 500, 1, true);
        this->addKeybind("Re-check Keybind", "Hold for 2 seconds!", getKeybind());
        for (int i = 0; i < totalPlayers; i++) {
            this->addHeader(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value);
            this->addToggle("Enabled", "Should it notify you?", this->settings.getSettingByName<bool>("player" + FlarialGUI::cached_to_string(i) + "Enabled")->value);
            this->addTextBox("Player Name", "", this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value);
            this->extraPadding();
        }

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }
    void onKey(KeyEvent &event) {
        if (this->isKeyPartOfKeybind(event.key)) {
            if (this->isKeybind(event.keys)) { // key is defo pressed
                keybindActions[0]({});
            }
        }

    };
};
