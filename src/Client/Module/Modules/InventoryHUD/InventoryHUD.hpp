#pragma once

#include "../Module.hpp"
#include <Events/Render/RenderEvent.hpp>

class InventoryHUD : public Module {
private:
	Vec2<float> currentPos;
	bool enabled = false;
	int durabilities[36][2] = { {0,0} };
public:

	InventoryHUD() : Module("Inventory HUD", "Displays your inventory\non your HUD",
		IDR_CHESTPLATE_PNG, "") {
		currentPos = Vec2<float>{ 0, 0 };
		Module::setup();
	};

	std::map<std::string, DWRITE_TEXT_ALIGNMENT> alignments = {
		{"Left", DWRITE_TEXT_ALIGNMENT_LEADING},
		{"Center", DWRITE_TEXT_ALIGNMENT_CENTER},
		{"Right", DWRITE_TEXT_ALIGNMENT_TRAILING}
	};

	void onEnable() override {
		Listen(this, RenderEvent, &InventoryHUD::onRender)
			Listen(this, SetupAndRenderEvent, &InventoryHUD::onSetupAndRender)
			if (FlarialGUI::inMenu) {
				FlarialGUI::Notify("To change the position of InventoryHUD, Please click " +
					ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
						"editmenubind")->value + " in the settings tab.");
			}
		Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &InventoryHUD::onRender)
			Deafen(this, SetupAndRenderEvent, &InventoryHUD::onSetupAndRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<std::string>("textalignment") == nullptr) settings.addSetting("textalignment", (std::string)"Right");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.05f);
		if (settings.getSettingByName<bool>("textShadow") == nullptr)  settings.addSetting("textShadow", true);
		if (settings.getSettingByName<bool>("showDurBar") == nullptr)  settings.addSetting("showDurBar", true);
		if (settings.getSettingByName<bool>("showStaticDurBarColor") == nullptr)  settings.addSetting("showStaticDurBarColor", false);
		if (settings.getSettingByName<bool>("overrideSpecialMaxDurBarCol") == nullptr)  settings.addSetting("overrideSpecialMaxDurBarCol", false);
		if (settings.getSettingByName<bool>("showSpecialMaxDurBarCol") == nullptr)  settings.addSetting("showSpecialMaxDurBarCol", false);
		if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 1.0f);

		if (settings.getSettingByName<float>("percentageX") == nullptr) settings.addSetting("percentageX", 2.0f);
		if (settings.getSettingByName<float>("percentageY") == nullptr) settings.addSetting("percentageY", 2.0f);

		if (settings.getSettingByName<std::string>("textColor") == nullptr) settings.addSetting("textColor", (std::string)"FFFFFF");
		if (settings.getSettingByName<float>("textColor_opacity") == nullptr) settings.addSetting("textColor_opacity", 1.0f);
		if (settings.getSettingByName<bool>("textColor_rgb") == nullptr) settings.addSetting("textColor_rgb", false);

		if (settings.getSettingByName<std::string>("staticDurBarColor") == nullptr) settings.addSetting("staticDurBarColor", (std::string)"FFFFFF");
		if (settings.getSettingByName<float>("staticDurBarColor_opacity") == nullptr) settings.addSetting("staticDurBarColor_opacity", 1.f);
		if (settings.getSettingByName<bool>("staticDurBarColor_rgb") == nullptr) settings.addSetting("staticDurBarColor_rgb", false);

		if (settings.getSettingByName<std::string>("specialMaxDurBarColor") == nullptr) settings.addSetting("specialMaxDurBarColor", (std::string)"FFFFFF");
		if (settings.getSettingByName<float>("specialMaxDurBarColor_opacity") == nullptr) settings.addSetting("specialMaxDurBarColor_opacity", 1.f);
		if (settings.getSettingByName<bool>("specialMaxDurBarColor_rgb") == nullptr) settings.addSetting("specialMaxDurBarColor_rgb", false);

		if (settings.getSettingByName<bool>("textShadow") == nullptr) settings.addSetting("textShadow", true);

		if (settings.getSettingByName<float>("spacing") == nullptr) settings.addSetting("spacing", 1.f);
		if (settings.getSettingByName<float>("textOffsetX") == nullptr) settings.addSetting("textOffsetX", 16.19f);
		if (settings.getSettingByName<float>("textOffsetY") == nullptr) settings.addSetting("textOffsetY", 13.13f);
		if (settings.getSettingByName<float>("durBarOffsetX") == nullptr) settings.addSetting("durBarOffsetX", 1.92f);
		if (settings.getSettingByName<float>("durBarOffsetY") == nullptr) settings.addSetting("durBarOffsetY", 12.45f);
		if (settings.getSettingByName<float>("durBarOpacity") == nullptr) settings.addSetting("durBarOpacity", 1.f);

		if (settings.getSettingByName<float>("100color") == nullptr) settings.addSetting("100color", 120.f);
		if (settings.getSettingByName<float>("0color") == nullptr) settings.addSetting("0color", 0.f);

		if (settings.getSettingByName<bool>("showDurBarMax") == nullptr) settings.addSetting("showDurBarMax", false);

		Module::defaultConfig();
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
		this->addSlider("Text Size", "", this->settings.getSettingByName<float>("textscale")->value, 0.25f, 0.0f, true);
		this->addSlider("Spacing", "", this->settings.getSettingByName<float>("spacing")->value, 10.f, 0.0f, true);
		this->addToggle("Show Durability", "", this->settings.getSettingByName<bool>("showDurBar")->value);
		this->addConditionalSlider(this->settings.getSettingByName<bool>("showDurBar")->value, "Durability Bar Offset X", "", this->settings.getSettingByName<float>("durBarOffsetX")->value, 50.f, 0.0f, false);
		this->addConditionalSlider(this->settings.getSettingByName<bool>("showDurBar")->value, "Durability Bar Offset Y", "", this->settings.getSettingByName<float>("durBarOffsetY")->value, 50.f, 0.0f, false);
		this->addConditionalSlider(this->settings.getSettingByName<bool>("showDurBar")->value, "Durability Bar Opacity", "", this->settings.getSettingByName<float>("durBarOpacity")->value, 1.f, 0.0f, false);
		this->addConditionalToggle(this->settings.getSettingByName<bool>("showDurBar")->value, "Show Max Durability Bar", "", this->settings.getSettingByName<bool>("showDurBarMax")->value);

		//this->addSlider("Background Padding", "", this->settings.getSettingByName<float>("bgPadding")->value, 25.f, 0.0f, false);
		this->extraPadding();

		this->addHeader("Text");
		this->addSlider("Text Offset X", "", this->settings.getSettingByName<float>("textOffsetX")->value, 50.f, 0.0f, false);
		this->addSlider("Text Offset Y", "", this->settings.getSettingByName<float>("textOffsetY")->value, 50.f, 0.0f, false);
		this->addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);
		this->addToggle("Text Shadow", "Displays a shadow under the text", this->settings.getSettingByName<bool>("textShadow")->value);
		this->addConditionalSlider(this->settings.getSettingByName<bool>("textShadow")->value, "Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);
		this->extraPadding();

		this->addHeader("Colors");
		this->addColorPicker("Text Color", "", settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textColor_opacity")->value, settings.getSettingByName<bool>("textColor_rgb")->value);
		this->addConditionalColorPicker(this->settings.getSettingByName<bool>("textShadow")->value, "Shadow Color", "Text Shadow Color", settings.getSettingByName<std::string>("textShadowCol")->value, settings.getSettingByName<float>("textShadowOpacity")->value, settings.getSettingByName<bool>("textShadowRGB")->value);
		this->addToggle("Show Special Max Durability Bar Color", "", this->settings.getSettingByName<bool>("showSpecialMaxDurBarCol")->value);
		this->addConditionalColorPicker(this->settings.getSettingByName<bool>("showSpecialMaxDurBarCol")->value, "Special Max Durability Bar Color", "", settings.getSettingByName<std::string>("specialMaxDurBarColor")->value, settings.getSettingByName<float>("specialMaxDurBarColor_opacity")->value, settings.getSettingByName<bool>("specialMaxDurBarColor_rgb")->value);
		this->addToggle("Enable Static Durability Bar Color", "", this->settings.getSettingByName<bool>("showStaticDurBarColor")->value);
		this->addConditionalColorPicker(this->settings.getSettingByName<bool>("showStaticDurBarColor")->value, "Static Durability Bar Color", "", settings.getSettingByName<std::string>("staticDurBarColor")->value, settings.getSettingByName<float>("staticDurBarColor_opacity")->value, settings.getSettingByName<bool>("staticDurBarColor_rgb")->value);
		this->addConditionalToggle(this->settings.getSettingByName<bool>("showStaticDurBarColor")->value, "Override Special Max Durability Bar Color", "", this->settings.getSettingByName<bool>("overrideSpecialMaxDurBarCol")->value);
		this->addConditionalSlider(!this->settings.getSettingByName<bool>("showStaticDurBarColor")->value, "100% Durability Bar Color", "Hue in degrees", this->settings.getSettingByName<float>("100color")->value, 360.f, 0.0f, false);
		this->addConditionalSlider(!this->settings.getSettingByName<bool>("showStaticDurBarColor")->value, "0% Durability Bar Color", "Hue in degrees", this->settings.getSettingByName<float>("0color")->value, 360.f, 0.0f, false);
		
		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	void renderText() {
		float uiscale = this->settings.getSettingByName<float>("uiscale")->value;
		float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
		float textScale = this->settings.getSettingByName<float>("textscale")->value * 1080;

		D2D_COLOR_F textColor = this->settings.getSettingByName<bool>("textColor_rgb")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("textColor")->value);
		textColor.a = this->settings.getSettingByName<float>("textColor_opacity")->value;

		D2D1_COLOR_F staticDurBarColor = this->settings.getSettingByName<bool>("staticDurBarColor_rgb")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("staticDurBarColor")->value);
		staticDurBarColor.a = this->settings.getSettingByName<float>("staticDurBarColor_opacity")->value;

		D2D1_COLOR_F specialMaxDurBarColor = this->settings.getSettingByName<bool>("specialMaxDurBarColor_rgb")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("specialMaxDurBarColor")->value);
		specialMaxDurBarColor.a = this->settings.getSettingByName<float>("specialMaxDurBarColor_opacity")->value;

		// we can't add backgrounds :(
		//D2D_COLOR_F bg = this->settings.getSettingByName<bool>("bgRGB")->value ? rgb : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("bgColor")->value);
		//bg.a = this->settings.getSettingByName<float>("bgOpacity")->value;

		float maxDurCol = this->settings.getSettingByName<float>("100color")->value;
		float minDurCol = this->settings.getSettingByName<float>("0color")->value;

		float textOpacity = this->settings.getSettingByName<float>("textOpacity")->value;
		bool textShadow = this->settings.getSettingByName<bool>("textShadow")->value;
		float ospacing = this->settings.getSettingByName<float>("spacing")->value;
		float textOffsetX = this->settings.getSettingByName<float>("textOffsetX")->value;
		float textOffsetY = this->settings.getSettingByName<float>("textOffsetY")->value;

		bool showSpecialMaxDurBarCol = this->settings.getSettingByName<bool>("showSpecialMaxDurBarCol")->value;
		bool overrideSpecialMaxDurBarCol = this->settings.getSettingByName<bool>("overrideSpecialMaxDurBarCol")->value;
		bool showStaticDurBarColor = this->settings.getSettingByName<bool>("showStaticDurBarColor")->value;

		float durBarOffsetX = this->settings.getSettingByName<float>("durBarOffsetX")->value;
		float durBarOffsetY = this->settings.getSettingByName<float>("durBarOffsetY")->value;

		if (this->isEnabled() && ClientInstance::getTopScreenName() == "hud_screen") {
			if (SDK::clientInstance->getLocalPlayer() != nullptr &&
				SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

				float xmodifier = 0.0f;
				float ymodifier = 0.0f;
				int counter = 0;

				//Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

				auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{16, 16}, uiscale);
				float spacing = scaledPos.x * ospacing;

				//float spacing = 16 * uiscale * ospacing;

				for (int i = 9; i < 36; i++) {
					auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(i);
					if (item->getItem() != nullptr) {
						std::string text = FlarialGUI::cached_to_string(item->count);
						std::wstring wText(text.begin(), text.end());

						float textX = currentPos.x + (xmodifier + (textOffsetX * uiscale * guiscale));
						float textY = currentPos.y + ymodifier + (textOffsetY * uiscale * guiscale);

						float durPerc = (float)durabilities[i][0] / (float)durabilities[i][1];

						if (this->settings.getSettingByName<bool>("showDurBar")->value && (settings.getSettingByName<bool>("showDurBarMax")->value || durabilities[i][0] != durabilities[i][1]) && durabilities[i][1] != 0) {
							D2D1_COLOR_F finalColor = D2D_COLOR_F(D2D1::ColorF::White);

							if (durabilities[i][0] != 0 || durabilities[i][1] != 0) finalColor = FlarialGUI::HSVtoColorF(minDurCol + durPerc * (maxDurCol - minDurCol), 1.f, 1.f);
							if (durabilities[i][0] == durabilities[i][1] && durabilities[i][1] != 0 && showSpecialMaxDurBarCol) finalColor = specialMaxDurBarColor;
							if (showStaticDurBarColor) {
								if (durabilities[i][0] == durabilities[i][1] && durabilities[i][1] != 0 && overrideSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[i][0] == durabilities[i][1] && !showSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[i][0] != durabilities[i][1]) finalColor = staticDurBarColor;
							}

							finalColor.a = this->settings.getSettingByName<float>("durBarOpacity")->value;

							FlarialGUI::RoundedRect(
								currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
								currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
								D2D1::ColorF(D2D1::ColorF::Black, this->settings.getSettingByName<float>("durBarOpacity")->value),
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

						if ((float)item->count > 1) {
							if (this->settings.getSettingByName<bool>("textShadow")->value) {
								D2D_COLOR_F shadowCol = settings.getSettingByName<bool>("textShadowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textShadowCol")->value);
								shadowCol.a = settings.getSettingByName<float>("textShadowOpacity")->value;
								FlarialGUI::FlarialTextWithFont(
									textX + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value),
									textY + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value),
									wText.c_str(),
									0,
									0,
									alignments[this->settings.getSettingByName<std::string>("textalignment")->value],
									textScale * uiscale * guiscale, DWRITE_FONT_WEIGHT_NORMAL,
									shadowCol,
									true
								);
							}

							FlarialGUI::FlarialTextWithFont(
								textX, textY, wText.c_str(),
								0, 0,
								alignments[this->settings.getSettingByName<std::string>("textalignment")->value],
								textScale * uiscale * guiscale,
								DWRITE_FONT_WEIGHT_NORMAL,
								textColor, textShadow
							);
						}
					}

					if (counter < 8) {
						xmodifier += spacing;
						counter++;
					}
					else {
						ymodifier += spacing;
						xmodifier = 0.0f;
						counter = 0;
					}
				}
			}
		}
	}

	void onRender(RenderEvent& event) {
		if (ClientInstance::getTopScreenName() == "hud_screen" &&
			this->isEnabled() ||
			ClientInstance::getTopScreenName() == "pause_screen" &&
			this->isEnabled()) {
			float spacing = this->settings.getSettingByName<float>("spacing")->value;

			Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value, this->settings.getSettingByName<float>("percentageY")->value);

			float uiscale = this->settings.getSettingByName<float>("uiscale")->value;
			float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();

			float s = 16 * uiscale * guiscale;

			float height = s * 2 * spacing + s;
			float width = s * 8 * spacing + s;

			if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
			else currentPos = Constraints::CenterConstraint(width, height);

			if (ClickGUI::editmenu) {
				FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 40);
				checkForRightClickAndOpenSettings(currentPos.x, currentPos.y, width, height);
			}

			Vec2<float> vec2;

			vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 40, width, height);

			currentPos.x = vec2.x;
			currentPos.y = vec2.y;

			Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

			this->settings.setValue("percentageX", percentages.x);
			this->settings.setValue("percentageY", percentages.y);

			if (ClickGUI::editmenu) {
				FlarialGUI::UnsetWindowRect();
			}
			renderText();
		}

		if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;
	}

	void onSetupAndRender(SetupAndRenderEvent& event) {
		if (this->isEnabled() && ClientInstance::getTopScreenName() == "hud_screen") {
			auto muirc = event.getMuirc();
			BaseActorRenderContext barc(muirc->getScreenContext(), muirc->getClientInstance(),
				muirc->getClientInstance()->getMinecraftGame());

			float uiscale = this->settings.getSettingByName<float>("uiscale")->value;
			Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

			if (SDK::clientInstance->getLocalPlayer() != nullptr &&
				SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

				float spacing = 16 * uiscale * this->settings.getSettingByName<float>("spacing")->value;
				float xmodifier = 0.0f;
				float ymodifier = 0.0f;
				int counter = 0;

				for (int i = 9; i < 36; i++) {
					auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(i);
					if (item->getItem() != nullptr) {
						float maxDamage = item->getMaxDamage();
						durabilities[i][1] = maxDamage;
						durabilities[i][0] = maxDamage - item->getDamageValue();
						// Render the item
						barc.itemRenderer->renderGuiItemNew(&barc, item, 0,
							scaledPos.x + xmodifier, scaledPos.y + ymodifier,
							1.0f, uiscale, false);

						if (item->isEnchanted()) {
							barc.itemRenderer->renderGuiItemNew(&barc, item, 0,
								scaledPos.x + xmodifier, scaledPos.y + ymodifier,
								1.0f, uiscale, true);
						}
					}

					if (counter < 8) {
						xmodifier += spacing;
						counter++;
					}
					else {
						ymodifier += spacing;
						xmodifier = 0.0f;
						counter = 0;
					}
				}
			}
		}
	}
};