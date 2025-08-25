#include "CommandHotkey.hpp"

#include "Client.hpp"
#include "Events/EventManager.hpp"


void CommandHotkey::addHotkey(int index) {
	keybindActions.push_back([this, index](std::vector<std::any> args) -> std::any {

		KeyEvent event = std::any_cast<KeyEvent>(args[0]);
		std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
		if (duration.count() >= 2.5) {
			std::string count;
			if (index > 0) count = "-" + FlarialGUI::cached_to_string(index);
			if (this->isKeybind(event.keys, index) && this->isKeyPartOfKeybind(event.key, index)) {
				std::string serverIP = SDK::getServerIP();
				if (serverIP.find("plutonium") != std::string::npos) {
					FlarialGUI::Notify("Can't use Command Hotkey on " + serverIP);
					this->restricted = true;
				}
				else {
					this->restricted = false;
				}
				if (!this->restricted) {
					std::shared_ptr<Packet> packet = SDK::createPacket(77);
					auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
					command_packet->command = this->settings.getSettingByName<std::string>("command" + count)->value;
					command_packet->origin.type = CommandOriginType::Player;
					command_packet->InternalSource = true;
					SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

					last_used = std::chrono::high_resolution_clock::now();
				}
			}
		}
		return {};

	});
}

void CommandHotkey::onSetup() {

	for (int i = 0; i < totalKeybinds; ++i) {
		addHotkey(i);
	}
}

void CommandHotkey::onEnable() {
	Listen(this, KeyEvent, &CommandHotkey::onKey)
		Module::onEnable();
}

void CommandHotkey::onDisable() {
	Deafen(this, KeyEvent, &CommandHotkey::onKey)
		Module::onDisable();
}

void CommandHotkey::defaultConfig() {
	getKeybind();
	Module::defaultConfig("core");
	if (settings.getSettingByName<std::string>("command") == nullptr) settings.addSetting("command", (std::string)"");
}

void CommandHotkey::settingsRender(float settingsOffset) {
	initSettingsPage();

	this->addHeader("Command Hotkey");
	this->addButton("Add another Keybind", "Multi-Keybind command support!", "Add", [&] {

		std::string keybindName = "keybind-" + FlarialGUI::cached_to_string(totalKeybinds);
		std::string commandName = "command-" + FlarialGUI::cached_to_string(totalKeybinds);

		this->settings.addSetting(keybindName, (std::string)"");
		this->settings.addSetting(commandName, (std::string)"");


		int i = totalKeybinds;
		addHotkey(i);

		totalKeybinds++;
		FlarialGUI::Notify("Added! Scroll down for options.");
		
	});


	for (int i = 0; i < totalKeybinds; ++i) {

		std::string header = (i == 0) ? "Command" : "Command " + FlarialGUI::cached_to_string(i);
		std::string commandSettingName = (i == 0) ? "command" : "command-" + FlarialGUI::cached_to_string(i);

		if (settings.getSettingByName<std::string>(commandSettingName) != nullptr) {
			this->addHeader(header);
			this->addKeybind("Command Keybind", "Hold for 2 seconds!", getKeybind(i));
			this->addTextBox(
				"Command to Send",
				"No need for /, And there's a spam limit!",
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

void CommandHotkey::onKey(KeyEvent& event) {
	if (!SDK::clientInstance->getLocalPlayer()) return;
	if (this->isEnabled() && (
		SDK::getCurrentScreen() == "hud_screen" ||
		SDK::getCurrentScreen() == "zoom_screen" ||
		SDK::getCurrentScreen() == "f3_screen"
	)) {
		for (int i = 0; i <= totalKeybinds; ++i) {
			keybindActions[i]({ std::any(event) });
		}
	}
}
