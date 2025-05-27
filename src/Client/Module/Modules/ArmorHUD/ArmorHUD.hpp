#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../Utils/Render/PositionUtils.hpp"

class ArmorHUD : public Module {
private:
	Vec2<float> currentPos;
	bool enabled = false;
	int durabilities[6][2] = { {0,0} };
	// TODO: delete testing variables (or adjust and delete)
	Vec2<float> testOffset = Vec2<float>{ 0,0 };
	float testSpacing = 16;
public:

	ArmorHUD() : Module("ArmorHUD", "Displays the armor you're\ncurrently wearing.",
		IDR_CHESTPLATE_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		if (FlarialGUI::inMenu) {
			std::string s = "To change the position of ArmorHUD, Please click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("editmenubind")->value + " in the settings tab.";
			std::cout << s << std::endl;
			FlarialGUI::Notify(s);
		}
		Listen(this, RenderEvent, &ArmorHUD::onRender)
			Listen(this, SetupAndRenderEvent, &ArmorHUD::onSetupAndRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &ArmorHUD::onRender)
			Deafen(this, SetupAndRenderEvent, &ArmorHUD::onSetupAndRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<float>("textSize") == nullptr) settings.addSetting("textSize", 0.05f);
		Module::defaultConfig();

		if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 2.0f);
		if (settings.getSettingByName<float>("percentageX") == nullptr) {
			settings.addSetting("percentageX", 0.0f);
			settings.addSetting("percentageY", 0.0f);
		}
		if (settings.getSettingByName<bool>("show_offhand") == nullptr) settings.addSetting("show_offhand", true);
		if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);
		if (settings.getSettingByName<bool>("durability_left") == nullptr) settings.addSetting("durability_left", false);
		if (settings.getSettingByName<bool>("percent") == nullptr) settings.addSetting("percent", false);
		if (settings.getSettingByName<bool>("color") == nullptr) settings.addSetting("color", false);
		if (settings.getSettingByName<bool>("showdurability") == nullptr) settings.addSetting("showdurability", false);

		if (settings.getSettingByName<std::string>("colorMain") == nullptr) settings.addSetting("colorMain", (std::string)"FFFFFF");
		if (settings.getSettingByName<float>("colorMain_opacity") == nullptr) settings.addSetting("colorMain_opacity", 1.f);
		if (settings.getSettingByName<bool>("colorMain_rgb") == nullptr) settings.addSetting("colorMain_rgb", false);

		if (settings.getSettingByName<std::string>("staticDurBarColor") == nullptr) settings.addSetting("staticDurBarColor", (std::string)"FFFFFF");
		if (settings.getSettingByName<float>("staticDurBarColor_opacity") == nullptr) settings.addSetting("staticDurBarColor_opacity", 1.f);
		if (settings.getSettingByName<bool>("staticDurBarColor_rgb") == nullptr) settings.addSetting("staticDurBarColor_rgb", false);

		if (settings.getSettingByName<std::string>("specialMaxDurBarColor") == nullptr) settings.addSetting("specialMaxDurBarColor", (std::string)"FFFFFF");
		if (settings.getSettingByName<float>("specialMaxDurBarColor_opacity") == nullptr) settings.addSetting("specialMaxDurBarColor_opacity", 1.f);
		if (settings.getSettingByName<bool>("specialMaxDurBarColor_rgb") == nullptr) settings.addSetting("specialMaxDurBarColor_rgb", false);

		if (settings.getSettingByName<bool>("fillGaps") == nullptr) settings.addSetting("fillGaps", true);

		if (settings.getSettingByName<bool>("colorFull_rgb") == nullptr) settings.addSetting("colorFull_rgb", false);
		if (settings.getSettingByName<float>("textOffsetX") == nullptr) settings.addSetting("textOffsetX", 17.55f);
		if (settings.getSettingByName<float>("textOffsetY") == nullptr) settings.addSetting("textOffsetY", 7.697f);
		if (settings.getSettingByName<bool>("showDurBar") == nullptr) settings.addSetting("showDurBar", true);
		if (settings.getSettingByName<float>("durBarOffsetX") == nullptr) settings.addSetting("durBarOffsetX", 1.92f);
		if (settings.getSettingByName<float>("durBarOffsetY") == nullptr) settings.addSetting("durBarOffsetY", 12.45f);
		if (settings.getSettingByName<float>("durBarOpacity") == nullptr) settings.addSetting("durBarOpacity", 1.f);

		if (settings.getSettingByName<float>("spacing") == nullptr) settings.addSetting("spacing", 1.f);

		if (settings.getSettingByName<bool>("hideMaxDurText") == nullptr) settings.addSetting("hideMaxDurText", false);
		if (settings.getSettingByName<bool>("showSpecialMaxDurBarCol") == nullptr) settings.addSetting("showSpecialMaxDurBarCol", false);
		if (settings.getSettingByName<bool>("showStaticDurBarColor") == nullptr) settings.addSetting("showStaticDurBarColor", false);
		if (settings.getSettingByName<bool>("overrideSpecialMaxDurBarCol") == nullptr) settings.addSetting("overrideSpecialMaxDurBarCol", false);
		if (settings.getSettingByName<bool>("showDurBarMax") == nullptr) settings.addSetting("showDurBarMax", false);

		if (settings.getSettingByName<float>("100color") == nullptr) settings.addSetting("100color", 120.f);
		if (settings.getSettingByName<float>("0color") == nullptr) settings.addSetting("0color", 0.f);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Main");
		this->addSlider("Size", "", this->settings.getSettingByName<float>("uiscale")->value, 5.f, 0.f, true);
		this->addSlider("Spacing", "", this->settings.getSettingByName<float>("spacing")->value, 10.f, 0.f, true);
		this->addToggle("Vertical ArmorHUD", "To switch between a vertical or horizontal layout", this->settings.getSettingByName<bool>("vertical")->value);
		if (this->settings.getSettingByName<bool>("vertical")->value) this->addToggle("Durability to the left", "", this->settings.getSettingByName<bool>("durability_left")->value);
		this->addToggle("Show offhand item", "", this->settings.getSettingByName<bool>("show_offhand")->value);
		this->addToggle("Fill Empty Slots", "Fill gaps when a piece of armor isn't equipped", this->settings.getSettingByName<bool>("fillGaps")->value);
		this->addToggle("Change Color", "", this->settings.getSettingByName<bool>("color")->value);

		this->extraPadding();

		this->addHeader("Durability");
		this->addToggle("Durability Text", "", this->settings.getSettingByName<bool>("showdurability")->value);
		if (this->settings.getSettingByName<bool>("showdurability")->value) {
			this->addSlider("Text Offset X", "", this->settings.getSettingByName<float>("textOffsetX")->value, 50.f, 0.0f, false);
			this->addSlider("Text Offset Y", "", this->settings.getSettingByName<float>("textOffsetY")->value, 50.f, 0.0f, false);
			this->addSlider("Text Size", "", this->settings.getSettingByName<float>("textSize")->value, 0.25f, 0.0f, true);
			this->addToggle("Show Durability in %", "", this->settings.getSettingByName<bool>("percent")->value);
			if (!this->settings.getSettingByName<bool>("percent")->value) this->addToggle("Hide Max Durability Text", "", this->settings.getSettingByName<bool>("hideMaxDurText")->value);
		}
		this->addToggle("Durability Bar", "", this->settings.getSettingByName<bool>("showDurBar")->value);
		if (this->settings.getSettingByName<bool>("showDurBar")->value) {
			this->addSlider("Durability Bar Offset X", "", this->settings.getSettingByName<float>("durBarOffsetX")->value, 50.f, 0.0f, false);
			this->addSlider("Durability Bar Offset Y", "", this->settings.getSettingByName<float>("durBarOffsetY")->value, 50.f, 0.0f, false);
			this->addSlider("Durability Bar Opacity", "", this->settings.getSettingByName<float>("durBarOpacity")->value, 1.f, 0.0f, false);
			this->addToggle("Show Max Durability Bar", "", this->settings.getSettingByName<bool>("showDurBarMax")->value);
		}

		this->extraPadding();
		this->addHeader("Colors");
		this->addToggle("Show Special Max Durability Bar Color", "", this->settings.getSettingByName<bool>("showSpecialMaxDurBarCol")->value);
		if (this->settings.getSettingByName<bool>("showSpecialMaxDurBarCol")->value) this->addColorPicker("Special Max Durability Bar Color", "", settings.getSettingByName<std::string>("specialMaxDurBarColor")->value,
			settings.getSettingByName<float>("specialMaxDurBarColor_opacity")->value,
			settings.getSettingByName<bool>("specialMaxDurBarColor_rgb")->value);
		this->addColorPicker("Normal Color", "", settings.getSettingByName<std::string>("colorMain")->value,
			settings.getSettingByName<float>("colorMain_opacity")->value,
			settings.getSettingByName<bool>("colorMain_rgb")->value);
		this->addToggle("Enable Static Durability Bar Color", "", this->settings.getSettingByName<bool>("showStaticDurBarColor")->value);
		if (this->settings.getSettingByName<bool>("showStaticDurBarColor")->value) {
			this->addColorPicker("Static Durability Bar Color", "", settings.getSettingByName<std::string>("staticDurBarColor")->value,
				settings.getSettingByName<float>("staticDurBarColor_opacity")->value,
				settings.getSettingByName<bool>("staticDurBarColor_rgb")->value);
			this->addToggle("Override Special Max Durability Bar Color", "", this->settings.getSettingByName<bool>("overrideSpecialMaxDurBarCol")->value);
		}
		else {
			this->addSlider("100% Durability Bar Color", "Hue in degrees", this->settings.getSettingByName<float>("100color")->value, 360.f, 0.0f, false);
			this->addSlider("0% Durability Bar Color", "Hue in degrees", this->settings.getSettingByName<float>("0color")->value, 360.f, 0.0f, false);
		}

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	// TODO: Make it look better
	void renderDurability() {
		D2D1_COLOR_F rgbColor = FlarialGUI::rgbColor;

		bool showOffhand = this->settings.getSettingByName<bool>("show_offhand")->value;

		D2D1_COLOR_F mainColor = this->settings.getSettingByName<bool>("colorMain_rgb")->value ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("colorMain")->value);
		mainColor.a = this->settings.getSettingByName<float>("colorMain_opacity")->value;

		D2D1_COLOR_F staticDurBarColor = this->settings.getSettingByName<bool>("staticDurBarColor_rgb")->value ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("staticDurBarColor")->value);
		staticDurBarColor.a = this->settings.getSettingByName<float>("staticDurBarColor_opacity")->value;

		D2D1_COLOR_F specialMaxDurBarColor = this->settings.getSettingByName<bool>("specialMaxDurBarColor_rgb")->value ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("specialMaxDurBarColor")->value);
		specialMaxDurBarColor.a = this->settings.getSettingByName<float>("specialMaxDurBarColor_opacity")->value;

		const float textSize = this->settings.getSettingByName<float>("textSize")->value * 1080;

		bool vertical = this->settings.getSettingByName<bool>("vertical")->value;
		bool durability_left = this->settings.getSettingByName<bool>("durability_left")->value;
		bool fillGaps = this->settings.getSettingByName<bool>("fillGaps")->value;
		bool hideMaxDurText = this->settings.getSettingByName<bool>("hideMaxDurText")->value;

		float maxDurCol = this->settings.getSettingByName<float>("100color")->value;
		float minDurCol = this->settings.getSettingByName<float>("0color")->value;

		float textOffsetX = this->settings.getSettingByName<float>("textOffsetX")->value;
		float textOffsetY = this->settings.getSettingByName<float>("textOffsetY")->value;
		float durBarOffsetX = this->settings.getSettingByName<float>("durBarOffsetX")->value;
		float durBarOffsetY = this->settings.getSettingByName<float>("durBarOffsetY")->value;
		float ospacing = this->settings.getSettingByName<float>("spacing")->value;
		float uiscale = this->settings.getSettingByName<float>("uiscale")->value;
		float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();

		bool showSpecialMaxDurBarCol = this->settings.getSettingByName<bool>("showSpecialMaxDurBarCol")->value;
		bool overrideSpecialMaxDurBarCol = this->settings.getSettingByName<bool>("overrideSpecialMaxDurBarCol")->value;
		bool showStaticDurBarColor = this->settings.getSettingByName<bool>("showStaticDurBarColor")->value;

		if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
			if (SDK::clientInstance->getLocalPlayer() != nullptr)
				if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

					float spacing = testSpacing;

					auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{spacing, spacing}, uiscale);
					spacing = (vertical ? scaledPos.y : scaledPos.x) * ospacing;

					float xmodifier = 0.0f;
					float ymodifier = 0.0f;

					for (int i = 2; i < 6; i++) {
						if (SDK::clientInstance->getLocalPlayer()->getArmor(i - 2)->getItem() != nullptr) {

							std::string text;

							float durPerc = (float)durabilities[i][0] / (float)durabilities[i][1];

							if (durabilities[i][1] != 0) {
								if (this->settings.getSettingByName<bool>("percent")->value) text = FlarialGUI::cached_to_string((int)std::round(durPerc * 100)) + "%";
								else text = FlarialGUI::cached_to_string(durabilities[i][0]) + (hideMaxDurText ? "" : ("/" + FlarialGUI::cached_to_string(durabilities[i][1])));
							}
							else text = FlarialGUI::cached_to_string(SDK::clientInstance->getLocalPlayer()->getArmor(i - 2)->getcount());

							std::wstring widestr = std::wstring(text.begin(), text.end());

							const wchar_t* widecstr = widestr.c_str();

							D2D1_COLOR_F finalColor = mainColor;

							if (durabilities[i][0] != 0 || durabilities[i][1] != 0) finalColor = FlarialGUI::HSVtoColorF(minDurCol + durPerc * (maxDurCol - minDurCol), 1.f, 1.f);
							if (durabilities[i][0] == durabilities[i][1] && durabilities[i][1] != 0 && showSpecialMaxDurBarCol) finalColor = specialMaxDurBarColor;
							if (showStaticDurBarColor) {
								if (durabilities[i][0] == durabilities[i][1] && durabilities[i][1] != 0 && overrideSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[i][0] == durabilities[i][1] && !showSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[i][0] != durabilities[i][1]) finalColor = staticDurBarColor;
							}

							if (this->settings.getSettingByName<bool>("showDurBar")->value && (settings.getSettingByName<bool>("showDurBarMax")->value || durabilities[i][0] != durabilities[i][1]) && durabilities[i][1] != 0) {
								FlarialGUI::RoundedRect(
									currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
									currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
									D2D1::ColorF(D2D1::ColorF::Black),
									guiscale * uiscale * 13.f,
									guiscale * uiscale * 2.f,
									0, 0
									);
								FlarialGUI::RoundedRect(
									currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
									currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
									finalColor,
									durPerc * (guiscale * uiscale * 13.f),
									guiscale * uiscale * 1.f,
									0, 0
								);
							}

							// armor
							if (this->settings.getSettingByName<bool>("showdurability")->value) FlarialGUI::FlarialTextWithFont(
								currentPos.x + (xmodifier + (textOffsetX * uiscale * guiscale)) * (durability_left ? -1 : 1),
								currentPos.y + ymodifier + (textOffsetY * uiscale * guiscale), widecstr,
								16 * uiscale * guiscale,
								0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
								textSize * uiscale * guiscale,
								DWRITE_FONT_WEIGHT_NORMAL, finalColor, true);
							if (fillGaps) {
								if (vertical) ymodifier += spacing;
								else xmodifier += spacing;
							}

						}
						if (!fillGaps) {
							if (vertical) ymodifier += spacing;
							else xmodifier += spacing;
						}
					}

					if (SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
						SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot())->getItem() !=
						nullptr) {
						auto currentItem = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
							SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot());

						std::string text;

						float durPerc = (float)durabilities[0][0] / (float)durabilities[0][1];

						if (durabilities[0][1] != 0) {
							if (this->settings.getSettingByName<bool>("percent")->value) text = FlarialGUI::cached_to_string((int)std::round(durPerc * 100)) + "%";
							else text = FlarialGUI::cached_to_string(durabilities[0][0]) + (hideMaxDurText ? "" : ("/" + FlarialGUI::cached_to_string(durabilities[0][1])));
						}
						else text = FlarialGUI::cached_to_string(currentItem->getcount());

						std::wstring widestr = std::wstring(text.begin(), text.end());

						const wchar_t* widecstr = widestr.c_str();

						D2D1_COLOR_F finalColor = mainColor;

						if (durabilities[0][0] != 0 || durabilities[0][1] != 0) finalColor = FlarialGUI::HSVtoColorF(minDurCol + durPerc * (maxDurCol - minDurCol), 1.f, 1.f);
						if (durabilities[0][0] == durabilities[0][1] && durabilities[0][1] != 0 && showSpecialMaxDurBarCol) finalColor = specialMaxDurBarColor;
						if (showStaticDurBarColor) {
							if (durabilities[0][0] == durabilities[0][1] && durabilities[0][1] != 0 && overrideSpecialMaxDurBarCol) finalColor = staticDurBarColor;
							else if (durabilities[0][0] == durabilities[0][1] && !showSpecialMaxDurBarCol) finalColor = staticDurBarColor;
							else if (durabilities[0][0] != durabilities[0][1]) finalColor = staticDurBarColor;
						}

						
						if (this->settings.getSettingByName<bool>("showDurBar")->value && (settings.getSettingByName<bool>("showDurBarMax")->value || durabilities[0][0] != durabilities[0][1]) && durabilities[0][1] != 0) {
							FlarialGUI::RoundedRect(
								currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
								currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
								D2D1::ColorF(D2D1::ColorF::Black),
								guiscale * uiscale * 13.f,
								guiscale * uiscale * 2.f,
								0, 0
							);
							FlarialGUI::RoundedRect(
								currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
								currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
								finalColor,
								durPerc * (guiscale * uiscale * 13.f),
								guiscale * uiscale * 1.f,
								0, 0
							);
						}

						// main hand
						if (this->settings.getSettingByName<bool>("showdurability")->value) FlarialGUI::FlarialTextWithFont(
							currentPos.x + (xmodifier + (textOffsetX * uiscale * guiscale)) * (durability_left ? -1 : 1),
							currentPos.y + ymodifier + (textOffsetY * uiscale * guiscale), widecstr,
							16 * uiscale * guiscale,
							0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
							textSize * uiscale * guiscale,
							DWRITE_FONT_WEIGHT_NORMAL, finalColor, true);

						if (fillGaps) {
							if (vertical) ymodifier += spacing;
							else xmodifier += spacing;
						}
					}
					if (!fillGaps) {
						if (vertical) ymodifier += spacing;
						else xmodifier += spacing;
					}
					if (showOffhand) {
						if (SDK::clientInstance->getLocalPlayer()->getOffhandSlot() != nullptr &&
							SDK::clientInstance->getLocalPlayer()->getOffhandSlot()->getItem() != nullptr) {

							auto offhandItem = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();

							std::string text;

							float durPerc = (float)durabilities[1][0] / (float)durabilities[1][1];

							if (durabilities[1][1] != 0) {
								if (this->settings.getSettingByName<bool>("percent")->value) text = FlarialGUI::cached_to_string((int)std::round(durPerc * 100)) + "%";
								else text = FlarialGUI::cached_to_string(durabilities[1][0]) + (hideMaxDurText ? "" : ("/" + FlarialGUI::cached_to_string(durabilities[1][1])));
							}
							else text = FlarialGUI::cached_to_string(offhandItem->getcount());

							std::wstring widestr = std::wstring(text.begin(), text.end());

							const wchar_t* widecstr = widestr.c_str();

							D2D1_COLOR_F finalColor = mainColor;

							if (durabilities[1][0] != 0 || durabilities[1][1] != 0) finalColor = FlarialGUI::HSVtoColorF(minDurCol + durPerc * (maxDurCol - minDurCol), 1.f, 1.f);
							if (durabilities[1][0] == durabilities[1][1] && durabilities[1][1] != 0 && showSpecialMaxDurBarCol) finalColor = specialMaxDurBarColor;
							if (showStaticDurBarColor) {
								if (durabilities[1][0] == durabilities[1][1] && durabilities[1][1] != 0 && overrideSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[1][0] == durabilities[1][1] && !showSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[1][0] != durabilities[1][1]) finalColor = staticDurBarColor;
							}


							if (this->settings.getSettingByName<bool>("showDurBar")->value && (settings.getSettingByName<bool>("showDurBarMax")->value || durabilities[1][0] != durabilities[1][1]) && durabilities[1][1] != 0) {
								FlarialGUI::RoundedRect(
									currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
									currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
									D2D1::ColorF(D2D1::ColorF::Black),
									guiscale * uiscale * 13.f,
									guiscale * uiscale * 2.f,
									0, 0
								);
								FlarialGUI::RoundedRect(
									currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
									currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
									finalColor,
									durPerc * (guiscale * uiscale * 13.f),
									guiscale * uiscale * 1.f,
									0, 0
								);
							}

							// offhand
							if (this->settings.getSettingByName<bool>("showdurability")->value) FlarialGUI::FlarialTextWithFont(
								currentPos.x + (xmodifier + (textOffsetX * uiscale * guiscale)) * (durability_left ? -1 : 1),
								currentPos.y + ymodifier + (textOffsetY * uiscale * guiscale), widecstr,
								16 * uiscale * guiscale,
								0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
								textSize * uiscale * guiscale,
								DWRITE_FONT_WEIGHT_NORMAL, finalColor, true);
						}
					}
				}
		}
	}

	void onRender(RenderEvent& event) {
		if (ClientInstance::getTopScreenName() == "hud_screen" &&
			this->isEnabled()) {

			// guiscale * uiscale = 1px in minecraft
			float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
			float uiscale = guiscale * this->settings.getSettingByName<float>("uiscale")->value;

			float s = 16 * uiscale;

			float spacing = this->settings.getSettingByName<float>("spacing")->value;

			Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value, this->settings.getSettingByName<float>("percentageY")->value);

			float width;
			float height;

			if (!this->settings.getSettingByName<bool>("vertical")->value) {
				width = s * 5 * spacing + s;
				height = s;
			}
			else {
				width = s;
				height = s * 5 * spacing + s;
			}

			if (settingperc.x != 0)
				currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
			else
				currentPos = Constraints::CenterConstraint(width, height);

			if (ClickGUI::editmenu) {
				// bounding boxes
				FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 18);
				checkForRightClickAndOpenSettings(currentPos.x, currentPos.y, width, height);
			}

			Vec2<float> vec2;
			// bounding boxes
			vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 18, width, height);


			currentPos.x = vec2.x;
			currentPos.y = vec2.y;

			Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

			this->settings.setValue("percentageX", percentages.x);
			this->settings.setValue("percentageY", percentages.y);

			if (ClickGUI::editmenu) {
				FlarialGUI::UnsetWindowRect();
			}

			renderDurability();
		}

		if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;
	}

	void onSetupAndRender(SetupAndRenderEvent& event) {
		if (this->isEnabled())
			if (ClientInstance::getTopScreenName() == "hud_screen") {
				auto muirc = event.getMuirc();
				BaseActorRenderContext barc(muirc->getScreenContext(), muirc->getClientInstance(),
					muirc->getClientInstance()->getMinecraftGame());

				Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

				float uiscale = this->settings.getSettingByName<float>("uiscale")->value;
				float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();

				float spacing = 16 * uiscale * this->settings.getSettingByName<float>("spacing")->value;

				bool vertical = this->settings.getSettingByName<bool>("vertical")->value;
				bool fillGaps = this->settings.getSettingByName<bool>("fillGaps")->value;

				float xmodifier = 0.0f;
				float ymodifier = 0.0f;

				if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
					if (SDK::clientInstance->getLocalPlayer() != nullptr)
						if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
							for (int i = 2; i < 6; i++) {
								auto armorSlot = SDK::clientInstance->getLocalPlayer()->getArmor(i - 2);

								if (armorSlot->getItem() != nullptr) {

									auto maxDamage = armorSlot->getMaxDamage();
									auto durabilityLeft = maxDamage - armorSlot->getDamageValue();

									durabilities[i][1] = maxDamage;
									durabilities[i][0] = durabilityLeft;

									barc.itemRenderer->renderGuiItemNew(&barc,
										armorSlot,
										0,
										scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
										this->settings.getSettingByName<float>(
											"uiscale")->value, false);

									if (armorSlot->isEnchanted()) {
										barc.itemRenderer->renderGuiItemNew(&barc,
											armorSlot,
											0,
											scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
											this->settings.getSettingByName<float>(
												"uiscale")->value,
											true);
									}

									if (fillGaps) {
										if (vertical) ymodifier += spacing;
										else xmodifier += spacing;
									}
								}
								if (!fillGaps) {
									if (vertical) ymodifier += spacing;
									else xmodifier += spacing;
								}
							}

							if (SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
								SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot())->getItem() !=
								nullptr) {
								auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
									SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot());

								auto maxDamage = item->getMaxDamage();
								auto durabilityLeft = maxDamage - item->getDamageValue();

								durabilities[0][1] = maxDamage;
								durabilities[0][0] = durabilityLeft;

								barc.itemRenderer->renderGuiItemNew(&barc,
									item,
									0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
									this->settings.getSettingByName<float>(
										"uiscale")->value,
									false);

								if (item->isEnchanted()) {
									barc.itemRenderer->renderGuiItemNew(&barc,
										item,
										0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
										this->settings.getSettingByName<float>(
											"uiscale")->value,
										true);
								}
								if (fillGaps) {
									if (vertical) ymodifier += spacing;
									else xmodifier += spacing;
								}
							}

							if (!fillGaps) {
								if (vertical) ymodifier += spacing;
								else xmodifier += spacing;
							}

							auto showOffhand = this->settings.getSettingByName<bool>("show_offhand")->value;
							if (showOffhand) {
								if (SDK::clientInstance->getLocalPlayer()->getOffhandSlot() != nullptr &&
									SDK::clientInstance->getLocalPlayer()->getOffhandSlot()->getItem() != nullptr) {
									auto item = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();

									auto maxDamage = item->getMaxDamage();
									auto durabilityLeft = maxDamage - item->getDamageValue();

									durabilities[1][1] = maxDamage;
									durabilities[1][0] = durabilityLeft;

									barc.itemRenderer->renderGuiItemNew(&barc,
										item,
										0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
										this->settings.getSettingByName<float>(
											"uiscale")->value,
										false);

									if (item->isEnchanted()) {
										barc.itemRenderer->renderGuiItemNew(&barc,
											item,
											0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
											this->settings.getSettingByName<float>(
												"uiscale")->value,
											true);
									}
								}
							}
						}
				}
			}
	}
};