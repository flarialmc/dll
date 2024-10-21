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


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Keybind", "There's a spam limit!", getKeybind());
        this->addTextBox("Text to Send", "", settings.getSettingByName<std::string>("text")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onKey(KeyEvent &event) {
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
        if (duration.count() >= 2.5) {
            if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
                auto player = SDK::clientInstance->getLocalPlayer();
                //std::string xuid = *player->getXuid(&xuid);
                std::shared_ptr<Packet> packet = SDK::createPacket(9);
                auto *akbar = reinterpret_cast<TextPacket *>(packet.get());

                akbar->type = TextPacketType::CHAT;
                akbar->message = std::regex_replace(
                        this->settings.getSettingByName<std::string>("text")->value, std::regex("�"), "§");
                akbar->platformId = "";
                akbar->translationNeeded = false;
                //akbar->xuid = xuid;
                akbar->name = player->getPlayerName();

                SDK::clientInstance->getPacketSender()->sendToServer(akbar);

                last_used = std::chrono::high_resolution_clock::now();
            }
        }
    }
};

