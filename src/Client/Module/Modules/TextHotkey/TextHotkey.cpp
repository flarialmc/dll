#include "TextHotkey.hpp"

#include "Events/EventManager.hpp"
#include "SDK/Client/Network/Packet/TextPacket.hpp"

TextHotkey::TextHotkey() : Module("Text Hotkey", "Send something in chat with a\nclick of a button!",
	IDR_TEXT_BOX_PNG, "")
{

}

void TextHotkey::onEnable()
{
	Listen(this, KeyEvent, &TextHotkey::onKey)
		Module::onEnable();
}

void TextHotkey::onDisable()
{
	Deafen(this, KeyEvent, &TextHotkey::onKey)
		Module::onDisable();
}

void TextHotkey::defaultConfig()
{
	getKeybind();
	Module::defaultConfig("core");
}

void TextHotkey::settingsRender(float settingsOffset)
{
	float x = Constraints::PercentageConstraint(0.019, "left");
	float y = Constraints::PercentageConstraint(0.10, "top");

	const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


	FlarialGUI::ScrollBar(x, y, 140, 5000, 2);
	FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
		Constraints::RelativeConstraint(1.0, "width"),
		Constraints::RelativeConstraint(0.88f, "height"));


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
				if (this->isKeybind(event.keys, i) && this->isKeyPartOfKeybind(event.key, i)) {
					auto player = SDK::clientInstance->getLocalPlayer();
					//std::string xuid = *player->getXuid(&xuid);
					std::shared_ptr<Packet> packet = SDK::createPacket(9);
					auto* akbar = reinterpret_cast<TextPacket*>(packet.get());

					akbar->type = TextPacketType::CHAT;
					akbar->message = std::regex_replace(
						this->settings.getSettingByName<std::string>("text-" + FlarialGUI::cached_to_string(i))->value, std::regex("�"), "§");
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


	for (size_t i = 1; i < totalKeybinds; ++i) {

		std::string header = "Text " + FlarialGUI::cached_to_string(i);
		std::string commandSettingName = "text-" + FlarialGUI::cached_to_string(i);

		if (settings.getSettingByName<std::string>(commandSettingName) != nullptr) {
			this->addHeader(header);
			this->addKeybind("Text Hotkey", "Hold for 2 seconds!", getKeybind(i, true));
			this->addTextBox(
				"Text to Send",
				"There's a spam limit!",
				settings.getSettingByName<std::string>(commandSettingName)->value
			);
			this->addButton("Delete Hotkey", "", "Delete", [this, i]() {
				this->settings.deleteSetting("text-" + FlarialGUI::cached_to_string(i));
				this->settings.deleteSetting("keybind-" + FlarialGUI::cached_to_string(i));
				this->saveSettings();
				});
			this->extraPadding();
		}
	}

	FlarialGUI::UnsetScrollView();

	this->resetPadding();
}

void TextHotkey::onSetup()
{
	for (size_t i = 1; i < totalKeybinds; ++i) {
		keybindActions.push_back([this, i](std::vector<std::any> args) -> std::any {

			KeyEvent event = std::any_cast<KeyEvent>(args[0]);
			std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
			if (duration.count() >= 2.5) {
				if (this->isKeybind(event.keys, i) && this->isKeyPartOfKeybind(event.key, i)) {
					auto player = SDK::clientInstance->getLocalPlayer();
					//std::string xuid = *player->getXuid(&xuid);
					std::shared_ptr<Packet> packet = SDK::createPacket(9);
					auto* akbar = reinterpret_cast<TextPacket*>(packet.get());

					akbar->type = TextPacketType::CHAT;
					akbar->message = std::regex_replace(
						this->settings.getSettingByName<std::string>("text-" + FlarialGUI::cached_to_string(i))->value, std::regex("�"), "§");
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

void TextHotkey::onKey(KeyEvent& event)
{
	if (!this->isEnabled()) return;
	if (!SDK::clientInstance->getLocalPlayer()) return;
	if (this->isEnabled() && totalKeybinds > 0) {
		for (size_t i = 0; i < totalKeybinds - 1; ++i) {
			keybindActions[i]({ std::any(event) });
		}
	}
}
