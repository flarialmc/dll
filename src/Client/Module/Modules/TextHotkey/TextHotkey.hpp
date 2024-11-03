#pragma once

#include "../Module.hpp"

class TextHotkey : public Module {
    std::chrono::time_point<std::chrono::high_resolution_clock> last_used;
public:
    TextHotkey() : Module("Text Hotkey", "Send something in chat with a\nclick of a button!",
                          IDR_TEXT_BOX_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, KeyEvent, &TextHotkey::onKey)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, KeyEvent, &TextHotkey::onKey)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string) "");
    }

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, 800, 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Function");
        this->addButton("Add another Keybind", "Multi-Keybind command support!", "Add", [this] {

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
                        auto player = SDK::clientInstance->getLocalPlayer();
                        //std::string xuid = *player->getXuid(&xuid);
                        std::shared_ptr<Packet> packet = SDK::createPacket(9);
                        auto *akbar = reinterpret_cast<TextPacket *>(packet.get());

                        akbar->type = TextPacketType::CHAT;
                        akbar->message = std::regex_replace(
                                this->settings.getSettingByName<std::string>("text" + count)->value, std::regex("�"), "§");
                        akbar->platformId = "";
                        akbar->translationNeeded = false;
                        //akbar->xuid = xuid;
                        akbar->name = player->getPlayerName();

                        SDK::clientInstance->getPacketSender()->sendToServer(akbar);

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

            std::string header = (i == 0) ? "Text" : "Text " + std::to_string(i);
            std::string commandSettingName = (i == 0) ? "Text" : "Text-" + std::to_string(i);

            if(settings.getSettingByName<std::string>(commandSettingName) != nullptr) {
                this->addHeader(header);

                this->addKeybind("Text Hotkey", "Hold for 2 seconds!", getKeybind(i));

                this->addTextBox(
                    "Text to Send",
                    "There's a spam limit!",
                    settings.getSettingByName<std::string>(commandSettingName)->value
                );
                this->extraPadding();
            }
        }

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
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
                        auto player = SDK::clientInstance->getLocalPlayer();
                        //std::string xuid = *player->getXuid(&xuid);
                        std::shared_ptr<Packet> packet = SDK::createPacket(9);
                        auto *akbar = reinterpret_cast<TextPacket *>(packet.get());

                        akbar->type = TextPacketType::CHAT;
                        akbar->message = std::regex_replace(
                                this->settings.getSettingByName<std::string>("text" + count)->value, std::regex("�"), "§");
                        akbar->platformId = "";
                        akbar->translationNeeded = false;
                        //akbar->xuid = xuid;
                        akbar->name = player->getPlayerName();

                        SDK::clientInstance->getPacketSender()->sendToServer(akbar);

                        last_used = std::chrono::high_resolution_clock::now();
                    }
                }

                return {};

            });
        }
    }
    void onKey(KeyEvent &event) {

        if (this->isEnabled()) {
            for (int i = 0; i <= totalKeybinds; ++i) {
                keybindActions[i]({ std::any(event) });
            }
        }
    }
};

