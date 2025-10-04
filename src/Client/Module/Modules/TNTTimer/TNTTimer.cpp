#include "TNTTimer.hpp"
#include <GUI/Engine/EngineImGui.hpp>
#include "SDK/Client/Actor/LocalPlayer.hpp"

#include "../Nick/NickModule.hpp"
#include <SDK/Client/Level/Level.hpp>
#include <SDK/Client/Actor/Components/SynchedActorDataComponent.hpp>

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

	setDef("format", std::string("Timer: {value}"));
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
	addTextBox("Format", "Use {value} as placeholder for seconds.", 0, "format");
	addSliderInt("Decimal Places", "0 to 3 digits.", "decimalPlaces", 3, 0);
	extraPadding();

	addHeader("Nametag");
	addToggle("Use Color Codes", "Apply §-color to nametag.", "useColorInNametag");

	bool c = getOps<bool>("useColorInNametag");

	addConditionalSlider(c, "High Threshold (s)", ">= this seconds uses High color.", "highThreshold", 10.0f, 0.0f);
	addConditionalSlider(c, "Mid Threshold (s)", ">= this and < High uses Mid color.", "midThreshold", 10.0f, 0.0f);

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
	addConditionalDropdown(c, "High Color", "", colorOptions, "highColorName", true);
	addConditionalDropdown(c, "Mid Color", "", colorOptions, "midColorName", true);
	addConditionalDropdown(c, "Low Color", "", colorOptions, "lowColorName", true);
	extraPadding();


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

	const std::string& highName = getOps<std::string>("highColorName");
	const std::string& midName = getOps<std::string>("midColorName");
	const std::string& lowName = getOps<std::string>("lowColorName");

	for (const auto& ent : player->getLevel()->getRuntimeActorList()) {
		if (!ent) continue;
		auto exp = ent->getSynchedActorDataComponent();
		if (!exp) continue;

		auto data = exp->mData.mItemsArray[static_cast<unsigned char>(ActorDataIDs::FuseTime)];
		if (!data) continue;

		auto value = reinterpret_cast<DataItem2<int>*>(data)->mValue;
		const float seconds = static_cast<float>(value + 1) / 20.f;

		std::ostringstream oss;
		oss.setf(std::ios::fixed);
		oss << std::setprecision(decimals) << seconds;

		std::string label = getOps<std::string>("format");

		std::string uppercaseSentence;
		std::string search = "{VALUE}";

		for (char c: label) {
			uppercaseSentence += (char) std::toupper(c);
		}

		size_t pos = uppercaseSentence.find(search);
		if (pos != std::string::npos) {
			label.replace(pos, search.length(), oss.str());
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
