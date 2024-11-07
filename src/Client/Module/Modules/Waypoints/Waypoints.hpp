#pragma once

#include "../Module.hpp"


class Waypoints : public Module {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last_used;
public:
    Waypoints() : Module("Waypoints", "Allows you to mark points in your world.", IDR_MAGNIFY_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &Waypoints::onRender)
        Module::onEnable();
    }

    void onSetup() override {

        for (int i = 0; i < totalKeybinds; ++i) {
            keybindActions.push_back([this, i](std::vector<std::any> args) -> std::any {

                KeyEvent event = std::any_cast<KeyEvent>(args[0]);
                std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
                if (duration.count() >= 2.5) {
                    std::string count;
                    if (i > 0) count = "-" + std::to_string(i);
                    if (this->isKeybind(event.keys, i) && this->isKeyPartOfKeybind(event.key, i)) {
                        std::shared_ptr<Packet> packet = SDK::createPacket(77);
                        auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                        command_packet->command = this->settings.getSettingByName<std::string>("command" + count)->value;
                        command_packet->origin.type = CommandOriginType::Player;
                        command_packet->InternalSource = true;
                        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                        last_used = std::chrono::high_resolution_clock::now();
                    }
                }

                return {};

                });
        }
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &Waypoints::onRender)
        Module::onDisable();
    }

    void defaultConfig() override {

    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, 5000, 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Function");
        this->addButton("Add another Waypoint", "Multi-Keybind command support!", "Add", [this] {

            std::string keybindName = "keybind-" + std::to_string(totalKeybinds);
            std::string commandName = "command-" + std::to_string(totalKeybinds);

            this->settings.addSetting(keybindName, (std::string)"");
            this->settings.addSetting(commandName, (std::string)"");


            keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
                KeyEvent event = std::any_cast<KeyEvent>(args[0]);
                std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
                if (duration.count() >= 2.5) {
                    std::string count;
                    if (totalKeybinds > 0) count = "-" + std::to_string(totalKeybinds);
                    if (this->isKeybind(event.keys, totalKeybinds) && this->isKeyPartOfKeybind(event.key, totalKeybinds)) {
                        std::shared_ptr<Packet> packet = SDK::createPacket(77);
                        auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                        command_packet->command = this->settings.getSettingByName<std::string>("command" + count)->value;
                        command_packet->origin.type = CommandOriginType::Player;
                        command_packet->InternalSource = true;
                        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                        last_used = std::chrono::high_resolution_clock::now();
                    }
                }
                return {};
                });

            totalKeybinds++;
            FlarialGUI::Notify("Added! Scroll down for options.");
            this->saveSettings();
        });


        for (int i = 0; i < totalKeybinds; ++i) {

            std::string header = (i == 0) ? "Command" : "Command " + std::to_string(i);
            std::string commandSettingName = (i == 0) ? "command" : "command-" + std::to_string(i);

            if (settings.getSettingByName<std::string>(commandSettingName) != nullptr) {
                this->addHeader(header);

                this->addKeybind("Command Keybind", "Hold for 2 seconds!", getKeybind(i));

                this->addTextBox(
                    "Command to Send",
                    "No need for /, And there's a spam limit!",
                    settings.getSettingByName<std::string>(commandSettingName)->value
                );
                this->extraPadding();
            }
        }

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onRender(RenderEvent& event) {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
            !SDK::clientInstance->getLocalPlayer()->getLevel())
            return;

        if (FlarialGUI::inMenu || SDK::getCurrentScreen() != "hud_screen") return;

        Vec2<float> screen;

        if (Matrix::WorldToScreen(Vec3{ 0.0F, 0.0F, 0.0F }, screen)) {
            FlarialGUI::FlarialTextWithFont(screen.x, screen.y, L"waypoint 1", 500, 0, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.215f, "height", true), DWRITE_FONT_WEIGHT_NORMAL, FlarialGUI::rgbColor, true);
        }
    }

    void addWaypoint() {
        FlarialGUI::Notify("added new waypoint");
    }
};
