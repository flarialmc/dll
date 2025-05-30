#pragma once

#include <cmath>
#include <sstream>
#include <iomanip>
#include "../Module.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"

typedef std::unordered_map<std::string, std::string> StringMap;

class Coordinates : public Module {

public:

	Coordinates() : Module("Coordinates", "Shows your XYZ position in game.",
		IDR_COORDINATES_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &Coordinates::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &Coordinates::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<bool>("responsivewidth") == nullptr) settings.addSetting("responsivewidth", true);
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{D}  X: {X} Y: {Y} Z: {Z}");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
		Module::defaultConfig("all");
		if (settings.getSettingByName<bool>("showDecimals") == nullptr) settings.addSetting("showDecimals", false);
		if (settings.getSettingByName<float>("decimalCount") == nullptr) settings.addSetting("decimalCount", 2.f);
		if (settings.getSettingByName<bool>("showOtherDimCoords") == nullptr) settings.addSetting("showOtherDimCoords", false);
		if (settings.getSettingByName<bool>("showDimName") == nullptr) settings.addSetting("showDimName", false);
		if (settings.getSettingByName<bool>("useSameDimensionFormat") == nullptr) settings.addSetting("useSameDimensionFormat", true);
		if (settings.getSettingByName<std::string>("defaultDimFormat") == nullptr) settings.addSetting("defaultDimFormat", (std::string)"{dim}");
        if (settings.getSettingByName<std::string>("OverworldFormat") == nullptr) settings.addSetting("OverworldFormat", (std::string)"{dim}");
        if (settings.getSettingByName<std::string>("NetherFormat") == nullptr) settings.addSetting("NetherFormat", (std::string)"{dim}");
        if (settings.getSettingByName<std::string>("TheEndFormat") == nullptr) settings.addSetting("TheEndFormat", (std::string)"{dim}");
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Coordinates");
		this->defaultAddSettings("main");
		this->extraPadding();

		this->addHeader("Text");
		this->defaultAddSettings("text");
		this->addToggle("Show Decimals", "", this->settings.getSettingByName<bool>("showDecimals")->value);
		this->addConditionalSlider(this->settings.getSettingByName<bool>("showDecimals")->value, "Number of Decimals", "", this->settings.getSettingByName<float>("decimalCount")->value, 6.f, 1.f);
		this->extraPadding();

		this->addHeader("Module Settings");
		this->addToggle("Show Other Dimension Coords", "Shows the other dimension\'s coordinates along with overworld coordinates.", this->settings.getSettingByName<bool>("showOtherDimCoords")->value);
		this->addToggle("Show Dimension Name", "Shows the dimension name.", this->settings.getSettingByName<bool>("showDimName")->value);
		this->addToggle("Use Same Dimension Format", "Uses the same format for all dimensions.", this->settings.getSettingByName<bool>("useSameDimensionFormat")->value);
		this->addConditionalTextBox(this->settings.getSettingByName<bool>("useSameDimensionFormat")->value, "Dimension Format", "", this->settings.getSettingByName<std::string>("defaultDimFormat")->value);
		this->addConditionalTextBox(!this->settings.getSettingByName<bool>("useSameDimensionFormat")->value, "Overworld Dimension Format", "", this->settings.getSettingByName<std::string>("OverworldFormat")->value);
		this->addConditionalTextBox(!this->settings.getSettingByName<bool>("useSameDimensionFormat")->value, "Nether Dimension Format", "", this->settings.getSettingByName<std::string>("NetherFormat")->value);
		this->addConditionalTextBox(!this->settings.getSettingByName<bool>("useSameDimensionFormat")->value, "TheEnd Dimension Format", "", this->settings.getSettingByName<std::string>("TheEndFormat")->value);
		this->extraPadding();

		this->addHeader("Colors");
		this->defaultAddSettings("colors");
		this->extraPadding();

		this->addHeader("Misc");
		this->defaultAddSettings("misc");

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	StringMap getCoords(float multiplier) {
		Vec3<float>* pos = SDK::clientInstance->getLocalPlayer()->getPosition();

		int decimalsToShow = this->settings.getSettingByName<bool>("showDecimals")->value ? std::floor(this->settings.getSettingByName<float>("decimalCount")->value) : -1;

		std::string xstr = std::to_string(pos->x * multiplier);
		std::string ystr = std::to_string(pos->y * multiplier);
		std::string zstr = std::to_string(pos->z * multiplier);

		xstr.erase(xstr.size() - (6 - decimalsToShow));
		ystr.erase(ystr.size() - (6 - decimalsToShow));
		zstr.erase(zstr.size() - (6 - decimalsToShow));

		return StringMap{
			{"x", xstr},
			{"y", ystr},
			{"z", zstr}
		};
	}

	std::string formattedDimension(std::string dimension) {
		std::string dimFormat = this->settings.getSettingByName<std::string>(dimension + "Format")->value;
		return std::regex_replace(dimFormat, std::regex("\\{dim\\}"), dimension);
	}

	void onRender(RenderEvent& event) {
		if (this->isEnabled() && SDK::clientInstance->getLocalPlayer() && SDK::getCurrentScreen() == "hud_screen") {
			std::string coord = this->settings.getSettingByName<std::string>("text")->value;
			std::string dimension = SDK::clientInstance->getBlockSource()->getDimension()->getName();

			if (dimension == "TheEnd") {
				StringMap pos = getCoords(1.0f);
				coord = std::regex_replace(coord, std::regex("\\{X\\}"), pos["x"]);
				coord = std::regex_replace(coord, std::regex("\\{Y\\}"), pos["y"]);
				coord = std::regex_replace(coord, std::regex("\\{Z\\}"), pos["z"]);

				if (settings.getSettingByName<bool>("showDimName")->value) {
					coord = std::regex_replace(coord, std::regex("\\{D\\}"), formattedDimension("TheEnd"));
				}
				else {
					coord = std::regex_replace(coord, std::regex("\\{D\\}"), "");
				}

				this->normalRenderCore(33, coord);
			}
			else {
				std::string coord2 = coord;

				float otherDimMultiplier = 0.0f;
				std::string otherDim = "";
				if (dimension == "Overworld") {
					otherDimMultiplier = 0.125f;
					otherDim = "Nether";
				}
				else if (dimension == "Nether") {
					otherDimMultiplier = 8.0f;
					otherDim = "Overworld";
				}

				StringMap defaultPos = getCoords(1.0f);

				coord = std::regex_replace(coord, std::regex("\\{X\\}"), defaultPos["x"]);
				coord = std::regex_replace(coord, std::regex("\\{Y\\}"), defaultPos["y"]);
				coord = std::regex_replace(coord, std::regex("\\{Z\\}"), defaultPos["z"]);

				if (settings.getSettingByName<bool>("showDimName")->value) {
					coord = std::regex_replace(coord, std::regex("\\{D\\}"), formattedDimension(dimension));
				}
				else {
					coord = std::regex_replace(coord, std::regex("\\{D\\}"), "");
				}

				std::string text = coord;

				if (settings.getSettingByName<bool>("showOtherDimCoords")->value) {
					StringMap otherDimPos = getCoords(otherDimMultiplier);
					coord2 = std::regex_replace(coord2, std::regex("\\{X\\}"), otherDimPos["x"]);
					coord2 = std::regex_replace(coord2, std::regex("\\{Y\\}"), otherDimPos["y"]);
					coord2 = std::regex_replace(coord2, std::regex("\\{Z\\}"), otherDimPos["z"]);

					if (settings.getSettingByName<bool>("showDimName")->value) {
						coord2 = std::regex_replace(coord2, std::regex("\\{D\\}"), formattedDimension(otherDim));
					}
					else {
						coord2 = std::regex_replace(coord2, std::regex("\\{D\\}"), "");
					}

					text = coord + "\n" + coord2;
				}

				this->normalRenderCore(33, text);
			}

		}
	}

};
