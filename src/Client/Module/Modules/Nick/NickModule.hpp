#pragma once

#include "../Module.hpp"

class NickModule : public Module {
private:
	bool enabled = false;
	static inline std::string original2;

public:
	static inline std::string original;
	static inline std::string backupOri;

	NickModule() : Module("Nick",
		"Hides your username and replace it with something else.\nWorks everywhere (chat, pause, third person, etc)\nOther people will not be able to see your nick.",
		IDR_ICOGNITO_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, DrawTextEvent, &NickModule::onDrawText)
			Listen(this, RaknetTickEvent, &NickModule::onRaknetTick)
			Listen(this, TickEvent, &NickModule::onTick)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, DrawTextEvent, &NickModule::onDrawText)
			Deafen(this, TickEvent, &NickModule::onTick)
			std::string val = original;
		std::string val2;

		if (SDK::clientInstance)
			if (SDK::clientInstance->getLocalPlayer() != nullptr) {
				if (original2 != *SDK::clientInstance->getLocalPlayer()->getNametag()) {
					original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
					backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag();
				}
				if (original2 == getOps<std::string>("nick")) original2 = backupOri;

				val2 = original2;

				SDK::clientInstance->getLocalPlayer()->setNametag(&val2);
				SDK::clientInstance->getLocalPlayer()->getPlayerName() = val;
			}

		Module::onDisable();
	}

	std::map<std::string, std::string> textColors = {
		{"White", "§f"},
		{"Black", "§0"},
		{"Netherite", "§j"},
		{"Gray", "§7"},
		{"Iron", "§i"},
		{"Quartz", "§h"},
		{"Dark Gray", "§8"},
		{"Red", "§c"},
		{"Dark Red", "§4"},
		{"Copper", "§n"},
		{"Redstone", "§m"},
		{"Gold", "§6"},
		{"Material Gold", "§p"},
		{"Yellow", "§e"},
		{"Minecoin Gold", "§g"},
		{"Green", "§a"},
		{"Dark Green", "§2"},
		{"Emerald", "§q"},
		{"Diamond", "§s"},
		{"Aqua", "§b"},
		{"Dark Aqua", "§3"},
		{"Blue", "§9"},
		{"Dark Blue", "§1"},
		{"Lapis", "§t"},
		{"Light Purple", "§d"},
		{"Dark Purple", "§5"},
		{"Amethyst", "§u"}
	};

	void defaultConfig() override {
		Module::defaultConfig("core");
		setDef("nick", (std::string)"Flarial User");
		setDef("bold", false);
		setDef("obfuscated", false);
		setDef("textColor", (std::string)"White");
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Nickname");
		addTextBox("Nickname", "", getOps<std::string>("nick"));
		addToggle("Bold", "", getOps<bool>("bold"));
		addToggle("Obfuscated", "", getOps<bool>("obfuscated"));
		addDropdown("Text Color", "", std::vector<std::string>{
			"White",
				"Black",
				"Netherite",
				"Gray",
				"Iron",
				"Quartz",
				"Dark Gray",
				"Red",
				"Dark Red",
				"Copper",
				"Redstone",
				"Gold",
				"Material Gold",
				"Green",
				"Dark Green",
				"Emerald",
				"Diamond",
				"Aqua",
				"Dark Aqua",
				"Blue",
				"Dark Blue",
				"Lapis",
				"Light Purple",
				"Dark Purple",
				"Amethyst"
		}, getOps<std::string>("textColor"));

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	void onRaknetTick(RaknetTickEvent& event) {
		//        if (module->isEnabled()) {
		//            std::string serverIP = SDK::getServerIP();
		//            if(serverIP.find("cubecraft") != std::string::npos) {
		//                if(!module->restricted) {
		//                    FlarialGUI::Notify("Can't use Nick on " + serverIP); // TODO: move restrictions to API
		//                    module->restricted = true;
		//                }
		//            } else {
		//                module->restricted = false;
		//            }
		//        }
	}

	void onDrawText(DrawTextEvent& event) {
		if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer())
			return;

		if (original.empty())
			original = SDK::clientInstance->getLocalPlayer()->getPlayerName();

		if (this->isEnabled() && !this->restricted) {
			const std::string& localPlayerName = original;
			size_t pos = event.getText()->find(localPlayerName);

			auto it = textColors.find(getOps<std::string>("textColor"));

			if (it == textColors.end()) {
				settings.setValue<std::string>("textColor", "White");
				it = textColors.find(getOps<std::string>("textColor"));
			}

			std::string prefix = it->second;

			prefix += "§o";
			if (getOps<bool>("bold")) prefix += "§l";
			if (getOps<bool>("obfuscated")) prefix += "§k";

			if (pos != std::string::npos) {
				std::string faketxt = *event.getText();
				faketxt.replace(pos, localPlayerName.length(),
					prefix + String::removeColorCodes(getOps<std::string>("nick")) + "§r");
				*event.getText() = faketxt;
			}
		}
	}

	void onTick(TickEvent& event) {
		auto player = SDK::clientInstance->getLocalPlayer();
		if (!player) return;

		if (original.empty())
			original = player->getPlayerName();

		if (original2.empty() || backupOri.empty()) {
			original2 = *player->getNametag();
			backupOri = original2;
		}

		bool currentlyEnabled = this->isEnabled();
		if (enabled != currentlyEnabled) {
			enabled = currentlyEnabled;
			if (!enabled)
				original2 = *player->getNametag();
		}

		if (enabled && !this->restricted) {
			std::string prefix = textColors.find(getOps<std::string>("textColor"))->second;

			prefix += "§o";
			if (getOps<bool>("bold")) prefix += "§l";
			if (getOps<bool>("obfuscated")) prefix += "§k";

			std::string val = prefix + String::removeColorCodes(getOps<std::string>("nick")) + "§r";
			player->setNametag(&val);
			player->getPlayerName() = val;
		}
		else {
			if (original2 == getOps<std::string>("nick"))
				original2 = backupOri;

			player->setNametag(&original2);
			player->getPlayerName() = original;
		}
	}
};