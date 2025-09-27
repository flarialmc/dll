#include "TNTTimer.hpp"

#include "../Nick/NickModule.hpp"

void TNTTimer::onEnable() {
	Listen(this, TickEvent, &TNTTimer::onTickEvent)
	Module::onEnable();
}

void TNTTimer::onDisable() {
	Deafen(this, TickEvent, &TNTTimer::onTickEvent)
	Module::onDisable();
}

void TNTTimer::defaultConfig() {
	Module::defaultConfig("core");

	setDef("format", std::string("Timer: {}"));
	setDef("decimalPlaces", 2);

	setDef("highThreshold", 2.5f);
	setDef("midThreshold", 1.0f);

	setDef("useColorInNametag", true);

	setDef("highColorName", std::string("Green"));
	setDef("midColorName", std::string("Gold"));
	setDef("lowColorName", std::string("Red"));
}

void TNTTimer::settingsRender(float settingsOffset) {
	initSettingsPage();

	addHeader("Timer Format");
	addTextBox("Format", "Use {} as placeholder for seconds.", 0, "format");
	addSliderInt("Decimal Places", "0 to 3 digits.", "decimalPlaces", 3, 0);
	extraPadding();

	addHeader("Color by Time");
	addSlider("High Threshold (s)", ">= this seconds uses High color.", "highThreshold", 10.0f, 0.0f);
	addSlider("Mid Threshold (s)", ">= this and < High uses Mid color.", "midThreshold", 10.0f, 0.0f);

	std::vector<std::string> colorOptions{
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
	};
	addDropdown("High Color", "", colorOptions, "highColorName", true);
	addDropdown("Mid Color", "", colorOptions, "midColorName", true);
	addDropdown("Low Color", "", colorOptions, "lowColorName", true);
	extraPadding();

	addHeader("Nametag");
	addToggle("Use Color Codes", "Apply §-color to nametag.", "useColorInNametag");

	FlarialGUI::UnsetScrollView();
	resetPadding();
}

// TODO: There might be a cleaner way to handle this by processing only explosive entities.
void TNTTimer::onTickEvent(TickEvent& event) {
	if (!this->isEnabled()) return;
	if (!SDK::clientInstance) return;
	auto player = SDK::clientInstance->getLocalPlayer();
	if (!player) return;

	const float highTh = getOps<float>("highThreshold");
	const float midTh = getOps<float>("midThreshold");
	const int   decimals = std::clamp(getOps<int>("decimalPlaces"), 0, 3);
	const bool  useColor = getOps<bool>("useColorInNametag");
	const std::string& fmt = getOps<std::string>("format");

	const std::string& highName = getOps<std::string>("highColorName");
	const std::string& midName = getOps<std::string>("midColorName");
	const std::string& lowName = getOps<std::string>("lowColorName");

	for (const auto& ent : player->getLevel()->getRuntimeActorList()) {
		if (!ent) continue;
		auto exp = ent->getSynchedActorDataComponent();
		if (!exp) continue;

		auto data = exp->mData.mItemsArray[(unsigned char)ActorDataIDs::FuseTime];
		if (!data) continue;

		auto value = reinterpret_cast<DataItem2<int>*>(data)->mValue;
		const float seconds = (value + 1) / 20.f;

		std::ostringstream oss;
		oss.setf(std::ios::fixed);
		oss << std::setprecision(decimals) << seconds;

		std::string label = fmt;
		if (auto pos = label.find("{}"); pos != std::string::npos) {
			label.replace(pos, 2, oss.str());
		}
		else {
			label += " " + oss.str();
		}

		const std::string& chosenName = (seconds >= highTh) ? highName : (seconds >= midTh) ? midName : lowName;

		// NOTE: This will overwrite existing nametags on explosives.
		// Since most explosives disappear after detonation, this behavior should be safe.
		if (useColor) {
			auto it = NickModule::textColors.find(chosenName);
			const std::string& prefix = (it != NickModule::textColors.end()) ? it->second : NickModule::textColors["White"];

			std::string tagged = prefix + label + "§r";
			ent->setNametag(&tagged);
		}
		else {
			ent->setNametag(&label);
		}
	}
}
