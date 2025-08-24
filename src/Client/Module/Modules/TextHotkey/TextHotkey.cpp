#include "TextHotkey.hpp"


void TextHotkey::onEnable() {
	Listen(this, KeyEvent, &TextHotkey::onKey);
	Module::onEnable();
}

void TextHotkey::onDisable() {
	Deafen(this, KeyEvent, &TextHotkey::onKey);
	Module::onDisable();
}

void TextHotkey::defaultConfig() {
	getKeybind();
	Module::defaultConfig("core");
	
}

void TextHotkey::settingsRender(float settingsOffset) {
	initSettingsPage();

	this->addHeader("Text Hotkey");
	this->addButton("Add another Keybind", "Multi-Keybind command support!", "Add", [this] {

		std::string keybindName = "keybind-" + FlarialGUI::cached_to_string(totalKeybinds);
		std::string commandName = "text-" + FlarialGUI::cached_to_string(totalKeybinds);

		this->settings.addSetting(keybindName, (std::string)"");
		this->settings.addSetting(commandName, (std::string)"");

		int i = totalKeybinds;
		keybindActions.push_back([this, i](std::vector<std::any> args) -> std::any {
			KeyEvent event = std::any_cast<KeyEvent>(args[0]);
			std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
			if (duration.count() >= 2.5) {
				std::string count;
				if (i > 0) count = "-" + FlarialGUI::cached_to_string(i);
				if (this->isKeybind(event.keys, i) && this->isKeyPartOfKeybind(event.key, i)) {
					auto player = SDK::clientInstance->getLocalPlayer();
					//std::string xuid = *player->getXuid(&xuid);
					std::shared_ptr<Packet> packet = SDK::createPacket(9);
					auto* akbar = reinterpret_cast<TextPacket*>(packet.get());

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
		Client::SaveSettings();
		});


	for (int i = 0; i < totalKeybinds; ++i) {

		std::string header = (i == 0) ? "Text" : "Text " + FlarialGUI::cached_to_string(i);
		std::string commandSettingName = (i == 0) ? "text" : "text-" + FlarialGUI::cached_to_string(i);

		if (settings.getSettingByName<std::string>(commandSettingName) != nullptr) {
			this->addHeader(header);

			this->addKeybind("Text Hotkey", "Hold for 2 seconds!", getKeybind(i));

			this->addTextBox(
				"Text to Send",
				"There's a spam limit!",
				settings.getSettingByName<std::string>(commandSettingName)->value
			);
			this->addButton("Delete Hotkey", "", "Delete", [this, i, commandSettingName]() {
				this->settings.deleteSetting(commandSettingName);
				this->settings.deleteSetting("keybind-" + FlarialGUI::cached_to_string(i));
				Client::SaveSettings();
				});
			this->extraPadding();
		}
	}

	FlarialGUI::UnsetScrollView();

	this->resetPadding();
}

void TextHotkey::onSetup() {

	for (int i = 0; i < totalKeybinds; ++i) {
		keybindActions.push_back([this, i](std::vector<std::any> args) -> std::any {

			KeyEvent event = std::any_cast<KeyEvent>(args[0]);
			std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
			if (duration.count() >= 2.5) {
				std::string count;
				if (i > 0) count = "-" + FlarialGUI::cached_to_string(i);
				if (this->isKeybind(event.keys, i) && this->isKeyPartOfKeybind(event.key, i)) {
					auto player = SDK::clientInstance->getLocalPlayer();
					//std::string xuid = *player->getXuid(&xuid);
					std::shared_ptr<Packet> packet = SDK::createPacket(9);
					auto* akbar = reinterpret_cast<TextPacket*>(packet.get());

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

void TextHotkey::onKey(KeyEvent& event) {
	if (!SDK::clientInstance->getLocalPlayer()) return;
	if (this->isEnabled()) {
		for (int i = 0; i <= totalKeybinds; ++i) {
			keybindActions[i]({ std::any(event) });
		}
	}
}
