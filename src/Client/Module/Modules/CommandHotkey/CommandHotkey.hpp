#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/CommandRequestPacket.hpp"

class CommandHotkey : public Module {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last_used;
public:
    CommandHotkey() : Module("Command Hotkey", "Send command with a\nclick of a button!",
                             IDR_TEXT_BOX_PNG, "") {

        Module::setup();
    };

    void onEnable() override {
        Listen(this, KeyEvent, &CommandHotkey::onKey)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, KeyEvent, &CommandHotkey::onKey)
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<std::string>("command") == nullptr)
            settings.addSetting("command", (std::string) "");
    }

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Command Keybind", "Hold for 2 seconds!", getKeybind());
        this->addTextBox("Command to Send", "No need for /", settings.getSettingByName<std::string>("command")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onKey(KeyEvent &event) {
        if (this->isEnabled()) {
            std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
            if (duration.count() >= 2.5) {
                if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

                    command_packet->command = this->settings.getSettingByName<std::string>("command")->value;

                    command_packet->origin.type = CommandOriginType::Player;
                    command_packet->InternalSource = true;

                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                    last_used = std::chrono::high_resolution_clock::now();
                }
            }
        }
    }
};

