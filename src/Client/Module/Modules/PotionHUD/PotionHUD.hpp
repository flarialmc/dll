#pragma once

#include "../Module.hpp"

class PotionHUD : public Module {
private:
	Vec2<float> currentPos{};
	bool enabled = false;

	float testSpacing = 20;
public:

	PotionHUD() : Module("PotionHUD", "Displays your potion effects",
		IDR_POTION_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &PotionHUD::onRender)
			Listen(this, RenderPotionHUDEvent, &PotionHUD::onRenderPotionHUD)
			Listen(this, SetupAndRenderEvent, &PotionHUD::onSetupAndRender)
			if (FlarialGUI::inMenu) {
				FlarialGUI::Notify("To change the position of PotionHUD, Please click " +
					ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
						"editmenubind")->value + " in the settings tab.");
			}
		Module::onEnable();
	}

	void onDisable() override {

		Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 1.0f);
		if (settings.getSettingByName<float>("textSize") == nullptr) settings.addSetting("textSize", 0.05f);
		if (settings.getSettingByName<std::string>("colorMain") == nullptr) settings.addSetting("colorMain", (std::string)"FFFFFF");
		if (settings.getSettingByName<float>("colorMain_opacity") == nullptr) settings.addSetting("colorMain_opacity", 1.0f);
		if (settings.getSettingByName<bool>("colorMain_rgb") == nullptr) settings.addSetting("colorMain_rgb", false);
		if (settings.getSettingByName<std::string>("colorLow") == nullptr) settings.addSetting("colorLow", (std::string)"FF0000");
		if (settings.getSettingByName<float>("colorLow_opacity") == nullptr) settings.addSetting("colorLow_opacity", 1.0f);
		if (settings.getSettingByName<bool>("colorLow_rgb") == nullptr) settings.addSetting("colorLow_rgb", false);
		if (settings.getSettingByName<float>("textSize") == nullptr) settings.addSetting("textSize", 0.14f);
		if (settings.getSettingByName<float>("spacing") == nullptr) settings.addSetting("spacing", 1.f);
		if (settings.getSettingByName<bool>("textShadow") == nullptr) settings.addSetting("textShadow", true);
		if (settings.getSettingByName<float>("textOffsetX") == nullptr) settings.addSetting("textOffsetX", 2.f);
		if (settings.getSettingByName<bool>("showText") == nullptr) settings.addSetting("showText", true);
		if (settings.getSettingByName<bool>("textLeft") == nullptr) settings.addSetting("textLeft", false);
		if (settings.getSettingByName<bool>("bottomUp") == nullptr) settings.addSetting("bottomUp", false);
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

		this->addHeader("Visual");
		this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 3.f, 0.f, true);
		this->addToggle("Bottom Up Mode", "New effects appear ontop instead", this->settings.getSettingByName<bool>("bottomUp")->value);
		this->addSlider("Spacing", "", this->settings.getSettingByName<float>("spacing")->value, 10.f, 0.0f, true);
		this->addToggle("Show Text", "", this->settings.getSettingByName<bool>("showText")->value);
		if (this->settings.getSettingByName<bool>("showText")->value) {
			this->addSlider("Text Size", "", this->settings.getSettingByName<float>("textSize")->value, 0.25f, 0.0f, true);
			this->addSlider("Text Offset X", "", this->settings.getSettingByName<float>("textOffsetX")->value, 50.f, 0.0f, false);
			this->addToggle("Text to the left", "", this->settings.getSettingByName<bool>("textLeft")->value);
			this->addToggle("Text Shadow", "Displays a shadow under the text", this->settings.getSettingByName<bool>("textShadow")->value);
			if (this->settings.getSettingByName<bool>("textShadow")->value) this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);
		}
		this->extraPadding();

		this->addHeader("Colors");
		this->addColorPicker("Main Color", "", this->settings.getSettingByName<std::string>("colorMain")->value,
			settings.getSettingByName<float>("colorMain_opacity")->value,
			settings.getSettingByName<bool>("colorMain_rgb")->value);
		if (this->settings.getSettingByName<bool>("textShadow")->value) this->addColorPicker("Shadow Color", "Text Shadow Color", settings.getSettingByName<std::string>("textShadowCol")->value,
			settings.getSettingByName<float>("textShadowOpacity")->value,
			settings.getSettingByName<bool>("textShadowRGB")->value);
		this->addColorPicker("Effect About to Expire", "", settings.getSettingByName<std::string>("colorLow")->value,
			settings.getSettingByName<float>("colorLow_opacity")->value,
			settings.getSettingByName<bool>("colorLow_rgb")->value);

		FlarialGUI::UnsetScrollView();

		this->resetPadding();
	}

	void renderText() {
		auto rgb1 = this->settings.getSettingByName<bool>("colorLow_rgb")->value;
		auto rgb2 = this->settings.getSettingByName<bool>("colorMain_rgb")->value;

		auto rgbColor = FlarialGUI::rgbColor;

		auto lowColor = rgb1 ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("colorLow")->value);
		auto mainColor = rgb2 ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("colorMain")->value);

		lowColor.a = settings.getSettingByName<float>("colorLow_opacity")->value;
		mainColor.a = settings.getSettingByName<float>("colorMain_opacity")->value;

		int warning_seconds = 5;

		float ospacing = this->settings.getSettingByName<float>("spacing")->value;

		bool toTheLeft = this->settings.getSettingByName<bool>("textLeft")->value;
		const float uiscale = this->settings.getSettingByName<float>("uiscale")->value;
		const float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
		const float textSize = this->settings.getSettingByName<float>("textSize")->value * 1080;
		const float textWidth = Constraints::RelativeConstraint(this->settings.getSettingByName<float>("textSize")->value, "height", true);
		const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

		if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
			if (SDK::clientInstance->getLocalPlayer() != nullptr)
				if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
					if (settings.getSettingByName<bool>("showText")->value) {
						auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{16, 16}, uiscale);
						float spacing = scaledPos.y * ospacing;

						float xmodifier = 0.0f;
						float ymodifier = 0.0f;

						float xoffset = 0.0f;
						float yoffset = 0.0f;

						std::vector<UnifiedMobEffectData> effects = SDK::clientInstance->getLocalPlayer()->getMobEffects();
						for (const auto& effect : effects) {
							if (!effect.isValid()) continue;
							float textX = currentPos.x + (toTheLeft ? 0 : (16 * uiscale * guiscale)) + (this->settings.getSettingByName<float>("textOffsetX")->value * guiscale * uiscale) * (toTheLeft ? -1 : 1);
							float textY = currentPos.y + ymodifier;

							std::string text = effect.getNameAndTime();
							std::stringstream ss(text);
							std::string effectName = effect.getName() + " " + MobEffect::amplifierToString(effect.getAmplifier()); // effect name
							std::string effectTImeLeft = effect.getTime(); // effect timeLeft

							std::wstring widestr1 = std::wstring(effectName.begin(), effectName.end());
							const wchar_t* widecstr1 = widestr1.c_str();

							std::wstring widestr2 = std::wstring(effectTImeLeft.begin(), effectTImeLeft.end());
							const wchar_t* widecstr2 = widestr2.c_str();

							if (this->settings.getSettingByName<bool>("textShadow")->value) {
								D2D_COLOR_F shadowCol = this->settings.getSettingByName<bool>("textShadowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("textShadowCol")->value);
								shadowCol.a = this->settings.getSettingByName<float>("textShadowOpacity")->value;
								FlarialGUI::FlarialTextWithFont(
									textX + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value),
									textY + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value) - FlarialGUI::getFlarialTextSize(widecstr1, 0,
										16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
										textSize * guiscale * uiscale,
										DWRITE_FONT_WEIGHT_BOLD, true).y / 4.f,
									widecstr1,
									0,
									16 * uiscale * guiscale,
									toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
									textSize * guiscale * uiscale, DWRITE_FONT_WEIGHT_BOLD,
									shadowCol,
									true
								);
								FlarialGUI::FlarialTextWithFont(
									textX + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value),
									textY + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value) + FlarialGUI::getFlarialTextSize(widecstr2, 0,
										16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
										(textSize * guiscale * uiscale) * 0.8f,
										DWRITE_FONT_WEIGHT_BOLD, true).y / 1.6f,
									widecstr2,
									0,
									16 * uiscale * guiscale,
									toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
									(textSize * guiscale * uiscale) * 0.8f, DWRITE_FONT_WEIGHT_NORMAL,
									shadowCol,
									true
								);
							}
							FlarialGUI::FlarialTextWithFont(
								textX,
								textY - FlarialGUI::getFlarialTextSize(widecstr1, 0,
									16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
									textSize * guiscale * uiscale,
									DWRITE_FONT_WEIGHT_BOLD, true).y / 4.f,
								widecstr1, 0,
								16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
								textSize * guiscale * uiscale,
								DWRITE_FONT_WEIGHT_BOLD, effect.duration / 20 <= warning_seconds ? lowColor : mainColor, true);
							FlarialGUI::FlarialTextWithFont(
								textX,
								textY + FlarialGUI::getFlarialTextSize(widecstr2, 0,
									16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
									(textSize * guiscale * uiscale) * 0.8f,
									DWRITE_FONT_WEIGHT_BOLD, true).y / 1.6f,
								widecstr2, 0,
								16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
								(textSize * guiscale * uiscale) * 0.8f,
								DWRITE_FONT_WEIGHT_NORMAL, effect.duration / 20 <= warning_seconds ? lowColor : mainColor, true);
							ymodifier += spacing * (this->settings.getSettingByName<bool>("bottomUp")->value ? -1 : 1);
						}
					}
				}
		}
	}

	void onRender(RenderEvent& event) {
		if (ClientInstance::getTopScreenName() == "hud_screen" && this->isEnabled()) {
			float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
			float uiscale = this->settings.getSettingByName<float>("uiscale")->value;
			float spacing = this->settings.getSettingByName<float>("spacing")->value;

			Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value, this->settings.getSettingByName<float>("percentageY")->value);
			float s = 16 * uiscale * guiscale;

			float width = s;
			float height = s;

			if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
			else currentPos = Constraints::CenterConstraint(width, height);

			if (ClickGUI::editmenu) {
				// bounding boxes
				FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 23);
			}

			Vec2<float> vec2;
			// bounding boxes
			vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 23, width, height);


			currentPos.x = vec2.x;
			currentPos.y = vec2.y;

			Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

			this->settings.setValue("percentageX", percentages.x);
			this->settings.setValue("percentageY", percentages.y);

			if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();

			renderText();
		}

		if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;
	}

	void onRenderPotionHUD(RenderPotionHUDEvent& event) {
		event.cancel();
	}

	void onSetupAndRender(SetupAndRenderEvent& event) {
		if (this->isEnabled())
			if (ClientInstance::getTopScreenName() == "hud_screen") {
				Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
				float uiscale = this->settings.getSettingByName<float>("uiscale")->value;

				if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
					auto ui_icon_scale = uiscale / 2.f;
					float spacing = 16 * uiscale * this->settings.getSettingByName<float>("spacing")->value;

					float xmodifier = 0.0f;
					float ymodifier = 0.0f;

					auto muirc = event.getMuirc();
					auto effects = SDK::clientInstance->getLocalPlayer()->getMobEffects();
					for (const auto& effect : effects) {
						if (!effect.isValid()) continue;
						auto location = effect.getTextureLocation();
						auto texture = muirc->createTexture(location, false);
						auto position = Vec2<float>(scaledPos.x + xmodifier, scaledPos.y + ymodifier);
						auto size = Vec2<float>(16.0f * uiscale, 16.0f * uiscale);
						static auto uvPos = Vec2<float>(0.f, 0.f);
						static auto uvSize = Vec2<float>(1.0f, 1.0f);
						muirc->drawImage(texture, position, size, uvPos, uvSize);
						static auto color = mce::Color();
						static auto flushLayer = HashedString("ui_flush");
						muirc->flushImages(color, 1.0f, flushLayer);
						ymodifier += spacing * (this->settings.getSettingByName<bool>("bottomUp")->value ? -1 : 1);
					}
				}
			}
	}
};