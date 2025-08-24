#include "PlayerNotifier.hpp"

void PlayerNotifier::defaultConfig() {
	getKeybind();
	Module::defaultConfig("core");
	if (!this->settings.getSettingByName<float>("duration")) settings.addSetting<float>("duration", 80);
	if (!this->settings.getSettingByName<std::string>("player0")) settings.addSetting<std::string>("player0", "TheBarii");
	if (!this->settings.getSettingByName<std::string>("player0")) settings.addSetting<bool>("player0Enabled", true);
	if (!this->settings.getSettingByName<std::string>("keybind")) settings.addSetting<std::string>("keybind", "P");
	
}

void PlayerNotifier::onEnable() {
	Module::onEnable();
	Listen(this, TickEvent, &PlayerNotifier::onTick);
	Listen(this, KeyEvent, &PlayerNotifier::onKey)
}

void PlayerNotifier::onDisable() {
	Module::onDisable();
	Deafen(this, TickEvent, &PlayerNotifier::onTick);
	Deafen(this, KeyEvent, &PlayerNotifier::onKey);
}

void PlayerNotifier::onSetup() {
	keybindActions.clear();
	keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
		check();
		return {};
		});
}

void PlayerNotifier::loadSettings(bool softLoad) {

	Module::loadSettings();
	for (const auto& settingPair : settings.settings) {
		const std::string& name = settingPair.first;
		if (name.find("player") != std::string::npos && name.find("Enabled") == std::string::npos) {
			++totalPlayers;
		}
	}

}

void PlayerNotifier::check() {
	std::unordered_map<mcUUID, PlayerListEntry> playerMap = SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap();

	for (const auto entry: playerMap | std::views::values) {
		for (int i = 0; i < totalPlayers; i++) {
			if (!this->settings.getSettingByName<bool>("player" + FlarialGUI::cached_to_string(i) + "Enabled")) continue;
			if (this->settings.getSettingByName<bool>("player" + FlarialGUI::cached_to_string(i) + "Enabled")->value) {
				if (entry.name.find(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value) != std::string::npos) {
					FlarialGUI::Notify(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value + " is online!");
				}
			}
		}
	}
}

void PlayerNotifier::onTick(TickEvent& event) {
	double intervalSeconds = this->settings.getSettingByName<float>("duration")->value;

	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = now - lastRun;

	if ((elapsed.count() >= intervalSeconds || first) && SDK::getCurrentScreen() == "hud_screen") {
		lastRun = now;
		first = false;

		check();

	}
}

void PlayerNotifier::settingsRender(float settingsOffset) {

	initSettingsPage();

	this->addHeader("Player Notifier");
	this->addButton("Add new player", "", "ADD", [&]() {
		this->settings.addSetting<std::string>("player" + FlarialGUI::cached_to_string(totalPlayers), "player" + FlarialGUI::cached_to_string(totalPlayers));
		this->settings.addSetting<bool>("player" + FlarialGUI::cached_to_string(totalPlayers) + "Enabled", true);
		totalPlayers++;
		});
	this->addSlider("Re-check", "(Seconds) After how long should it re-check for players", this->settings.getSettingByName<float>("duration")->value, 500, 1, true);
	this->addKeybind("Re-check Keybind", "Hold for 2 seconds!", getKeybind());
	this->extraPadding();

	for (int i = 0; i < totalPlayers; i++) {
		this->addHeader(this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value);
		this->addToggle("Enabled", "Should it notify you?", this->settings.getSettingByName<bool>("player" + FlarialGUI::cached_to_string(i))->value);
		this->addTextBox("Player Name", "", this->settings.getSettingByName<std::string>("player" + FlarialGUI::cached_to_string(i))->value);
		this->extraPadding();
	}

	FlarialGUI::UnsetScrollView();
	this->resetPadding();
}

void PlayerNotifier::onKey(KeyEvent& event) {
	if (this->isKeyPartOfKeybind(event.key) && (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "f3_screen" || SDK::getCurrentScreen() == "zoom_screen")) {
		if (this->isKeybind(event.keys)) { // key is defo pressed
			keybindActions[0]({});
		}
	}

}
