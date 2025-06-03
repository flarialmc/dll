#pragma once

#include "../Module.hpp"
#include <cmath>
#include <utility>

class DirectionHUD : public Module {
public:
	float lerpYaw = 0.f;
	float yaw = 0.f;


	DirectionHUD() : Module("DirectionHUD", "Shows a compass showing your direction",
		IDR_CURSOR_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &DirectionHUD::onRender);
		Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &DirectionHUD::onRender);
		Module::onDisable();
	}

	void defaultConfig() override {
		getKeybind();
		Module::defaultConfig("core");
		Module::defaultConfig("pos");
		Module::defaultConfig("main");

		if (settings.getSettingByName<float>("pixelsPerDegree") == nullptr) settings.addSetting("pixelsPerDegree", 1.5f);
		if (settings.getSettingByName<bool>("wrapFade") == nullptr) settings.addSetting("wrapFade", true);
		if (settings.getSettingByName<float>("fadeDistancePerc") == nullptr) settings.addSetting("fadeDistancePerc", 75.f);
		
		if (settings.getSettingByName<bool>("showScales") == nullptr) settings.addSetting("showScales", true);
		if (settings.getSettingByName<bool>("scaleShadow") == nullptr) settings.addSetting("scaleShadow", true);
		if (settings.getSettingByName<float>("scaleShadowOffset") == nullptr) settings.addSetting("scaleShadowOffset", 0.003f);

		if (settings.getSettingByName<bool>("showCardinalScale") == nullptr) settings.addSetting("showCardinalScale", true);
		if (settings.getSettingByName<float>("cardinalScaleWidth") == nullptr) settings.addSetting("cardinalScaleWidth", 5.f);

		if (settings.getSettingByName<bool>("showOrdinalScale") == nullptr) settings.addSetting("showOrdinalScale", true);
		if (settings.getSettingByName<float>("ordinalScaleWidth") == nullptr) settings.addSetting("ordinalScaleWidth", 2.f);

		if (settings.getSettingByName<bool>("showText") == nullptr) settings.addSetting("showText", true);
		if (settings.getSettingByName<bool>("textShadow") == nullptr) settings.addSetting("textShadow", true);
		if (settings.getSettingByName<float>("textShadowOffset") == nullptr) settings.addSetting("textShadowOffset", 0.003f);

		if (settings.getSettingByName<bool>("showCardinalText") == nullptr) settings.addSetting("showCardinalText", true);
		if (settings.getSettingByName<float>("cardinalTextSize") == nullptr) settings.addSetting("cardinalTextSize", 1.f);
		if (settings.getSettingByName<float>("cardinalTextOffset") == nullptr) settings.addSetting("cardinalTextOffset", 1.f);

		if (settings.getSettingByName<bool>("showOrdinalText") == nullptr) settings.addSetting("showOrdinalText", true);
		if (settings.getSettingByName<float>("ordinalTextSize") == nullptr) settings.addSetting("ordinalTextSize", 0.75f);
		if (settings.getSettingByName<float>("ordinalTextOffset") == nullptr) settings.addSetting("ordinalTextOffset", 1.f);


		if (settings.getSettingByName<std::string>("cardinalScaleCol") == nullptr) settings.addSetting("cardinalScaleCol", (std::string)"fafafa");
		if (settings.getSettingByName<float>("cardinalScaleOpacity") == nullptr) settings.addSetting("cardinalScaleOpacity", 1.0f);
		if (settings.getSettingByName<bool>("cardinalScaleRGB") == nullptr) settings.addSetting("cardinalScaleRGB", false);

		if (settings.getSettingByName<std::string>("ordinalScaleCol") == nullptr) settings.addSetting("ordinalScaleCol", (std::string)"b8b8b8");
		if (settings.getSettingByName<float>("ordinalScaleOpacity") == nullptr) settings.addSetting("ordinalScaleOpacity", 1.0f);
		if (settings.getSettingByName<bool>("ordinalScaleRGB") == nullptr) settings.addSetting("ordinalScaleRGB", false);

		if (settings.getSettingByName<std::string>("cardinalTextCol") == nullptr) settings.addSetting("cardinalTextCol", (std::string)"fafafa");
		if (settings.getSettingByName<float>("cardinalTextOpacity") == nullptr) settings.addSetting("cardinalTextOpacity", 1.0f);
		if (settings.getSettingByName<bool>("cardinalTextRGB") == nullptr) settings.addSetting("cardinalTextRGB", false);

		if (settings.getSettingByName<std::string>("ordinalTextCol") == nullptr) settings.addSetting("ordinalTextCol", (std::string)"b8b8b8");
		if (settings.getSettingByName<float>("ordinalTextOpacity") == nullptr) settings.addSetting("ordinalTextOpacity", 1.0f);
		if (settings.getSettingByName<bool>("ordinalTextRGB") == nullptr) settings.addSetting("ordinalTextRGB", false);

		if (settings.getSettingByName<std::string>("cardinalTextShadowCol") == nullptr) settings.addSetting("cardinalTextShadowCol", (std::string)"000000");
		if (settings.getSettingByName<float>("cardinalTextShadowOpacity") == nullptr) settings.addSetting("cardinalTextShadowOpacity", 1.0f);
		if (settings.getSettingByName<bool>("cardinalTextShadowRGB") == nullptr) settings.addSetting("cardinalTextShadowRGB", false);

		if (settings.getSettingByName<std::string>("ordinalTextShadowCol") == nullptr) settings.addSetting("ordinalTextShadowCol", (std::string)"000000");
		if (settings.getSettingByName<float>("ordinalTextShadowOpacity") == nullptr) settings.addSetting("ordinalTextShadowOpacity", 1.0f);
		if (settings.getSettingByName<bool>("ordinalTextShadowRGB") == nullptr) settings.addSetting("ordinalTextShadowRGB", false);

		if (settings.getSettingByName<std::string>("cardinalScaleShadowCol") == nullptr) settings.addSetting("cardinalScaleShadowCol", (std::string)"000000");
		if (settings.getSettingByName<float>("cardinalScaleShadowOpacity") == nullptr) settings.addSetting("cardinalScaleShadowOpacity", 1.0f);
		if (settings.getSettingByName<bool>("cardinalScaleShadowRGB") == nullptr) settings.addSetting("cardinalScaleShadowRGB", false);

		if (settings.getSettingByName<std::string>("ordinalScaleShadowCol") == nullptr) settings.addSetting("ordinalScaleShadowCol", (std::string)"000000");
		if (settings.getSettingByName<float>("ordinalScaleShadowOpacity") == nullptr) settings.addSetting("ordinalScaleShadowOpacity", 1.0f);
		if (settings.getSettingByName<bool>("ordinalScaleShadowRGB") == nullptr) settings.addSetting("ordinalScaleShadowRGB", false);

		if (settings.getSettingByName<bool>("showDegrees") == nullptr) settings.addSetting("showDegrees", true);
		if (settings.getSettingByName<float>("degreesTextSize") == nullptr) settings.addSetting("degreesTextSize", 1.2f);
		if (settings.getSettingByName<float>("degreesTextOffset") == nullptr) settings.addSetting("degreesTextOffset", 0.5f);
		if (settings.getSettingByName<std::string>("degreesTextCol") == nullptr) settings.addSetting("degreesTextCol", (std::string)"ffffff");
		if (settings.getSettingByName<float>("degreesTextOpacity") == nullptr) settings.addSetting("degreesTextOpacity", 1.0f);
		if (settings.getSettingByName<bool>("degreesTextRGB") == nullptr) settings.addSetting("degreesTextRGB", false);
		if (settings.getSettingByName<bool>("degreesTextShadow") == nullptr) settings.addSetting("degreesTextShadow", true);
		if (settings.getSettingByName<std::string>("degreesTextShadowCol") == nullptr) settings.addSetting("degreesTextShadowCol", (std::string)"000000");
		if (settings.getSettingByName<float>("degreesTextShadowOpacity") == nullptr) settings.addSetting("degreesTextShadowOpacity", 1.0f);
		if (settings.getSettingByName<bool>("degreesTextShadowRGB") == nullptr) settings.addSetting("degreesTextShadowRGB", false);
	
		if (settings.getSettingByName<bool>("showDegrees") == nullptr) settings.addSetting("showDegrees", true);
		if (settings.getSettingByName<float>("degreesTextSize") == nullptr) settings.addSetting("degreesTextSize", 1.2f);
		if (settings.getSettingByName<float>("degreesTextOffset") == nullptr) settings.addSetting("degreesTextOffset", 0.5f);
		if (settings.getSettingByName<std::string>("degreesTextCol") == nullptr) settings.addSetting("degreesTextCol", (std::string)"ffffff");
		if (settings.getSettingByName<float>("degreesTextOpacity") == nullptr) settings.addSetting("degreesTextOpacity", 1.0f);
		if (settings.getSettingByName<bool>("degreesTextRGB") == nullptr) settings.addSetting("degreesTextRGB", false);
		if (settings.getSettingByName<bool>("degreesTextShadow") == nullptr) settings.addSetting("degreesTextShadow", true);
		if (settings.getSettingByName<std::string>("degreesTextShadowCol") == nullptr) settings.addSetting("degreesTextShadowCol", (std::string)"000000");
		if (settings.getSettingByName<float>("degreesTextShadowOpacity") == nullptr) settings.addSetting("degreesTextShadowOpacity", 1.0f);
		if (settings.getSettingByName<bool>("degreesTextShadowRGB") == nullptr) settings.addSetting("degreesTextShadowRGB", false);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Direction HUD");
		defaultAddSettings("main");
		addSlider("Pixels Per Degree", "", settings.getSettingByName<float>("pixelsPerDegree")->value, 20.f, 0.001f);
		addToggle("Wrap Around Fade", "", settings.getSettingByName<bool>("wrapFade")->value);
		addConditionalSlider(settings.getSettingByName<bool>("wrapFade")->value, "Fade Distance", "what percentage of the distance from the center will the HUD start fading", settings.getSettingByName<float>("fadeDistancePerc")->value, 99, 1);


		extraPadding();

		addHeader("Scales");
		addToggle("Show Scales", "Lines indicating N, E, S, W", settings.getSettingByName<bool>("showScales")->value);
		addConditionalToggle(settings.getSettingByName<bool>("showScales")->value, "MORE SCALES!!", "", settings.getSettingByName<bool>("scaleShadow")->value);

		addConditionalToggle(settings.getSettingByName<bool>("showScales")->value, "Scale Shadow", "", settings.getSettingByName<bool>("scaleShadow")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showScales")->value && settings.getSettingByName<bool>("scaleShadow")->value, "Scale Shadow Offset", "How far the shadow will be.", settings.getSettingByName<float>("scaleShadowOffset")->value, 0.02f, 0.001f);

		addConditionalToggle(settings.getSettingByName<bool>("showScales")->value, "Show Cardinal Scales", "", settings.getSettingByName<bool>("showCardinalScale")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showScales")->value && settings.getSettingByName<bool>("showCardinalScale")->value, "Cardinal Scale Width", "", settings.getSettingByName<float>("cardinalScaleWidth")->value, 10.0f);

		addConditionalToggle(settings.getSettingByName<bool>("showScales")->value, "Show Ordinal Scales", "", settings.getSettingByName<bool>("showOrdinalScale")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showScales")->value && settings.getSettingByName<bool>("showOrdinalScale")->value, "Ordinal Scale Width", "", settings.getSettingByName<float>("ordinalScaleWidth")->value, 10.0f);
		extraPadding();

		addHeader("Text");
		addToggle("Show Text", "Text indicating N, E, S, W", settings.getSettingByName<bool>("showText")->value);
		addConditionalToggle(settings.getSettingByName<bool>("showText")->value, "Text Shadow", "", settings.getSettingByName<bool>("textShadow")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showText")->value && settings.getSettingByName<bool>("textShadow")->value, "Text Shadow Offset", "How far the shadow will be.", settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);

		addConditionalToggle(settings.getSettingByName<bool>("showText")->value, "Show Cardinal Text", "", settings.getSettingByName<bool>("showCardinalText")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showText")->value && settings.getSettingByName<bool>("showCardinalText")->value, "Cardinal Text Size", "", settings.getSettingByName<float>("cardinalTextSize")->value, 5.0f);
		addConditionalSlider(settings.getSettingByName<bool>("showText")->value && settings.getSettingByName<bool>("showCardinalText")->value, "Cardinal Text Offset", "", settings.getSettingByName<float>("cardinalTextOffset")->value, 10.0f, 0.0f, false);

		addConditionalToggle(settings.getSettingByName<bool>("showText")->value, "Show Ordinal Text", "", settings.getSettingByName<bool>("showOrdinalText")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showText")->value && settings.getSettingByName<bool>("showOrdinalText")->value, "Ordinal Text Size", "", settings.getSettingByName<float>("ordinalTextSize")->value, 5.0f);
		addConditionalSlider(settings.getSettingByName<bool>("showText")->value && settings.getSettingByName<bool>("showOrdinalText")->value, "Ordinal Text Offset", "", settings.getSettingByName<float>("ordinalTextOffset")->value, 10.0f, 0.0f, false);

		extraPadding();

		addHeader("Degrees");
		addToggle("Show Degrees", "Display the exact angle (0-360)", settings.getSettingByName<bool>("showDegrees")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showDegrees")->value, "Degrees Text Size", "", settings.getSettingByName<float>("degreesTextSize")->value, 3.0f, 0.5f);
		addConditionalSlider(settings.getSettingByName<bool>("showDegrees")->value, "Degrees Text Offset", "", settings.getSettingByName<float>("degreesTextOffset")->value, 5.0f, 0.0f, false);
		addConditionalToggle(settings.getSettingByName<bool>("showDegrees")->value, "Degrees Text Shadow", "", settings.getSettingByName<bool>("degreesTextShadow")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showDegrees")->value, "Degrees Text Color", "", settings.getSettingByName<std::string>("degreesTextCol")->value, settings.getSettingByName<float>("degreesTextOpacity")->value, settings.getSettingByName<bool>("degreesTextRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showDegrees")->value && settings.getSettingByName<bool>("degreesTextShadow")->value, "Degrees Text Shadow Color", "", settings.getSettingByName<std::string>("degreesTextShadowCol")->value, settings.getSettingByName<float>("degreesTextShadowOpacity")->value, settings.getSettingByName<bool>("degreesTextShadowRGB")->value);
		

		extraPadding();

		addHeader("Degrees");
		addToggle("Show Degrees", "Display the exact angle (0-360)", settings.getSettingByName<bool>("showDegrees")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showDegrees")->value, "Degrees Text Size", "", settings.getSettingByName<float>("degreesTextSize")->value, 3.0f, 0.5f);
		addConditionalSlider(settings.getSettingByName<bool>("showDegrees")->value, "Degrees Text Offset", "", settings.getSettingByName<float>("degreesTextOffset")->value, 5.0f, 0.0f, false);
		addConditionalToggle(settings.getSettingByName<bool>("showDegrees")->value, "Degrees Text Shadow", "", settings.getSettingByName<bool>("degreesTextShadow")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showDegrees")->value, "Degrees Text Color", "", settings.getSettingByName<std::string>("degreesTextCol")->value, settings.getSettingByName<float>("degreesTextOpacity")->value, settings.getSettingByName<bool>("degreesTextRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showDegrees")->value && settings.getSettingByName<bool>("degreesTextShadow")->value, "Degrees Text Shadow Color", "", settings.getSettingByName<std::string>("degreesTextShadowCol")->value, settings.getSettingByName<float>("degreesTextShadowOpacity")->value, settings.getSettingByName<bool>("degreesTextShadowRGB")->value);
		
		extraPadding();

		addHeader("Colors");
		addConditionalColorPicker(settings.getSettingByName<bool>("showText")->value, "Cardinal Scale Color", "", settings.getSettingByName<std::string>("cardinalScaleCol")->value, settings.getSettingByName<float>("cardinalScaleOpacity")->value, settings.getSettingByName<bool>("cardinalScaleRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showText")->value, "Ordinal Scale Color", "", settings.getSettingByName<std::string>("ordinalScaleCol")->value, settings.getSettingByName<float>("ordinalScaleOpacity")->value, settings.getSettingByName<bool>("ordinalScaleRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showText")->value, "Cardinal Text Color", "", settings.getSettingByName<std::string>("cardinalTextCol")->value, settings.getSettingByName<float>("cardinalTextOpacity")->value, settings.getSettingByName<bool>("cardinalTextRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showText")->value, "Ordinal Text Color", "", settings.getSettingByName<std::string>("ordinalTextCol")->value, settings.getSettingByName<float>("ordinalTextOpacity")->value, settings.getSettingByName<bool>("ordinalTextRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showText")->value && settings.getSettingByName<bool>("textShadow")->value, "Cardinal Text Shadow Color", "", settings.getSettingByName<std::string>("cardinalTextShadowCol")->value, settings.getSettingByName<float>("cardinalTextShadowOpacity")->value, settings.getSettingByName<bool>("cardinalTextShadowRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showText")->value && settings.getSettingByName<bool>("textShadow")->value, "Ordinal Text Shadow Color", "", settings.getSettingByName<std::string>("ordinalTextShadowCol")->value, settings.getSettingByName<float>("ordinalTextShadowOpacity")->value, settings.getSettingByName<bool>("ordinalTextShadowRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showScales")->value && settings.getSettingByName<bool>("scaleShadow")->value, "Cardinal Scale Shadow Color", "", settings.getSettingByName<std::string>("cardinalScaleShadowCol")->value, settings.getSettingByName<float>("cardinalScaleShadowOpacity")->value, settings.getSettingByName<bool>("cardinalScaleShadowRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("showScales")->value && settings.getSettingByName<bool>("scaleShadow")->value, "Ordinal Scale Shadow Color", "", settings.getSettingByName<std::string>("ordinalScaleShadowCol")->value, settings.getSettingByName<float>("ordinalScaleShadowOpacity")->value, settings.getSettingByName<bool>("ordinalScaleShadowRGB")->value);


		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	// minecraft yaw goes from -180 to 180 
	float calculateDeltaYaw(float currentYaw, float targetYaw) {
		float diff = fmod(targetYaw - currentYaw, 360.0f);
		if (diff < -180.0f) diff += 360.0f;
		else if (diff > 180.0f) diff -= 360.0f;
		return diff;
	}

	float targetYaws[8] = {
		180.f, // N
		-90.f, // E
		0.f, // S
		90.f, // W
		135.f, // NW
		-135.f, // NE
		-45.f, // SE
		45.f // SW
	};

	void onRender(RenderEvent& event) {
		if (!this->isEnabled()) return;
		if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return;
		if (SDK::getCurrentScreen() != "hud_screen") return;

		LocalPlayer* player = SDK::clientInstance->getLocalPlayer();
		ActorRotationComponent* rotationComponent = player->getActorRotationComponent();

		if (!rotationComponent) return;

		if (rotationComponent->rot.y != 0) lerpYaw = rotationComponent->rot.y;

		// cardinal
		float deltaYaw_N = calculateDeltaYaw(lerpYaw, targetYaws[0]);
		float deltaYaw_E = calculateDeltaYaw(lerpYaw, targetYaws[1]);
		float deltaYaw_S = calculateDeltaYaw(lerpYaw, targetYaws[2]);
		float deltaYaw_W = calculateDeltaYaw(lerpYaw, targetYaws[3]);

		// ordinal
		float deltaYaw_NW = calculateDeltaYaw(lerpYaw, targetYaws[4]);
		float deltaYaw_NE = calculateDeltaYaw(lerpYaw, targetYaws[5]);
		float deltaYaw_SE = calculateDeltaYaw(lerpYaw, targetYaws[6]);
		float deltaYaw_SW = calculateDeltaYaw(lerpYaw, targetYaws[7]);

		float uiscale = settings.getSettingByName<float>("uiscale")->value;

		float pixelsPerDegree = Constraints::RelativeConstraint(settings.getSettingByName<float>("pixelsPerDegree")->value * 0.001f) * settings.getSettingByName<float>("uiscale")->value;
		float fullCirclePixelWidth = 360.0f * pixelsPerDegree;

		float cardinalBarWidth = Constraints::RelativeConstraint(settings.getSettingByName<float>("cardinalScaleWidth")->value * 0.001f) * uiscale;
		float ordinalBarWidth = Constraints::RelativeConstraint(settings.getSettingByName<float>("ordinalScaleWidth")->value * 0.001f) * uiscale;

		Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value, this->settings.getSettingByName<float>("percentageY")->value);
		Vec2<float> realcenter;

		float barHeight = Constraints::RelativeConstraint(0.05f * uiscale);
		float CtextSize = Constraints::SpacingConstraint(5.f, barHeight) * this->settings.getSettingByName<float>("cardinalTextSize")->value;
		float OtextSize = Constraints::SpacingConstraint(5.f, barHeight) * this->settings.getSettingByName<float>("ordinalTextSize")->value;

		float cardinalTextOffset = Constraints::RelativeConstraint(settings.getSettingByName<float>("cardinalTextOffset")->value) * uiscale * 0.01f;
		float ordinalTextOffset = Constraints::RelativeConstraint(settings.getSettingByName<float>("ordinalTextOffset")->value) * uiscale * 0.01f;

		if (settingperc.x != 0) realcenter = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
		else realcenter = Constraints::CenterConstraint(fullCirclePixelWidth, barHeight);

		realcenter.x -= fullCirclePixelWidth / 2.f;

		if (ClickGUI::editmenu) {
			FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, fullCirclePixelWidth, barHeight, 33);

			Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, 33, fullCirclePixelWidth, barHeight);

			checkForRightClickAndOpenSettings(realcenter.x, realcenter.y, fullCirclePixelWidth, barHeight);

			vec2.x += fullCirclePixelWidth / 2.f;

			realcenter.x = vec2.x;
			realcenter.y = vec2.y;

			realcenter = realcenter;

			Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y, 0, 0);
			this->settings.setValue("percentageX", percentages.x);
			this->settings.setValue("percentageY", percentages.y);

			realcenter.x -= fullCirclePixelWidth / 2.f;
		}

		float hudCenterX = realcenter.x + (fullCirclePixelWidth / 2.f);

		// cardinal calc pos
		float xPosN = hudCenterX + (deltaYaw_N * pixelsPerDegree);
		float xPosE = hudCenterX + (deltaYaw_E * pixelsPerDegree);
		float xPosS = hudCenterX + (deltaYaw_S * pixelsPerDegree);
		float xPosW = hudCenterX + (deltaYaw_W * pixelsPerDegree);

		// ordinal calc pos
		float xPosNW = hudCenterX + (deltaYaw_NW * pixelsPerDegree);
		float xPosNE = hudCenterX + (deltaYaw_NE * pixelsPerDegree);
		float xPosSE = hudCenterX + (deltaYaw_SE * pixelsPerDegree);
		float xPosSW = hudCenterX + (deltaYaw_SW * pixelsPerDegree);

		// cardinal wrap around
		if (xPosN > hudCenterX + fullCirclePixelWidth / 2.0f) xPosN -= fullCirclePixelWidth;
		if (xPosN < hudCenterX - fullCirclePixelWidth / 2.0f) xPosN += fullCirclePixelWidth;

		if (xPosE > hudCenterX + fullCirclePixelWidth / 2.0f) xPosE -= fullCirclePixelWidth;
		if (xPosE < hudCenterX - fullCirclePixelWidth / 2.0f) xPosE += fullCirclePixelWidth;

		if (xPosS > hudCenterX + fullCirclePixelWidth / 2.0f) xPosS -= fullCirclePixelWidth;
		if (xPosS < hudCenterX - fullCirclePixelWidth / 2.0f) xPosS += fullCirclePixelWidth;

		if (xPosW > hudCenterX + fullCirclePixelWidth / 2.0f) xPosW -= fullCirclePixelWidth;
		if (xPosW < hudCenterX - fullCirclePixelWidth / 2.0f) xPosW += fullCirclePixelWidth;

		// ordinal wrap around
		if (xPosNW > hudCenterX + fullCirclePixelWidth / 2.0f) xPosNW -= fullCirclePixelWidth;
		if (xPosNW < hudCenterX - fullCirclePixelWidth / 2.0f) xPosNW += fullCirclePixelWidth;

		if (xPosNE > hudCenterX + fullCirclePixelWidth / 2.0f) xPosNE -= fullCirclePixelWidth;
		if (xPosNE < hudCenterX - fullCirclePixelWidth / 2.0f) xPosNE += fullCirclePixelWidth;

		if (xPosSE > hudCenterX + fullCirclePixelWidth / 2.0f) xPosSE -= fullCirclePixelWidth;
		if (xPosSE < hudCenterX - fullCirclePixelWidth / 2.0f) xPosSE += fullCirclePixelWidth;

		if (xPosSW > hudCenterX + fullCirclePixelWidth / 2.0f) xPosSW -= fullCirclePixelWidth;
		if (xPosSW < hudCenterX - fullCirclePixelWidth / 2.0f) xPosSW += fullCirclePixelWidth;

		std::vector<std::pair<std::string, float>> directions = {
			std::pair<std::string, float>((std::string)"N", xPosN),
			std::pair<std::string, float>((std::string)"E", xPosE),
			std::pair<std::string, float>((std::string)"S", xPosS),
			std::pair<std::string, float>((std::string)"W", xPosW),
			std::pair<std::string, float>((std::string)"NW", xPosNW),
			std::pair<std::string, float>((std::string)"NE", xPosNE),
			std::pair<std::string, float>((std::string)"SE", xPosSE),
			std::pair<std::string, float>((std::string)"SW", xPosSW)
		};

		D2D_COLOR_F cardinalScaleCol = settings.getSettingByName<bool>("cardinalScaleRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("cardinalScaleCol")->value);
		cardinalScaleCol.a = settings.getSettingByName<float>("cardinalScaleOpacity")->value;

		D2D_COLOR_F ordinalScaleCol = settings.getSettingByName<bool>("ordinalScaleRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("ordinalScaleCol")->value);
		ordinalScaleCol.a = settings.getSettingByName<float>("ordinalScaleOpacity")->value;

		D2D_COLOR_F cardinalTextCol = settings.getSettingByName<bool>("cardinalTextRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("cardinalTextCol")->value);
		cardinalTextCol.a = settings.getSettingByName<float>("cardinalTextOpacity")->value;

		D2D_COLOR_F ordinalTextCol = settings.getSettingByName<bool>("ordinalTextRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("ordinalTextCol")->value);
		ordinalTextCol.a = settings.getSettingByName<float>("ordinalTextOpacity")->value;

		// shadows

		D2D_COLOR_F cardinalTextShadowCol = settings.getSettingByName<bool>("cardinalTextShadowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("cardinalTextShadowCol")->value);
		cardinalTextShadowCol.a = settings.getSettingByName<float>("cardinalTextShadowOpacity")->value;

		D2D_COLOR_F ordinalTextShadowCol = settings.getSettingByName<bool>("ordinalTextShadowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("ordinalTextShadowCol")->value);
		ordinalTextShadowCol.a = settings.getSettingByName<float>("ordinalTextShadowOpacity")->value;

		D2D_COLOR_F cardinalScaleShadowCol = settings.getSettingByName<bool>("cardinalScaleShadowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("cardinalScaleShadowCol")->value);
		cardinalScaleShadowCol.a = settings.getSettingByName<float>("cardinalScaleShadowOpacity")->value;

		D2D_COLOR_F ordinalScaleShadowCol = settings.getSettingByName<bool>("ordinalScaleShadowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("ordinalScaleShadowCol")->value);
		ordinalScaleShadowCol.a = settings.getSettingByName<float>("ordinalScaleShadowOpacity")->value;

		// degrees text
		if (settings.getSettingByName<bool>("showDegrees")->value) {
			// convert to 0-360
			float compassDegrees = fmod((-lerpYaw + 180.0f), 360.0f);
			if (compassDegrees < 0) compassDegrees += 360.0f;
			
			std::string degreesText = std::to_string(static_cast<int>(compassDegrees));
			
			// get text size for positioning
			float degreesTextSize = Constraints::SpacingConstraint(5.f, barHeight) * settings.getSettingByName<float>("degreesTextSize")->value;
			float degreesTextOffset = Constraints::RelativeConstraint(settings.getSettingByName<float>("degreesTextOffset")->value) * uiscale * 0.01f;
			
			// colors
			D2D_COLOR_F degreesTextCol = settings.getSettingByName<bool>("degreesTextRGB")->value ? 
				FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("degreesTextCol")->value);
			degreesTextCol.a = settings.getSettingByName<float>("degreesTextOpacity")->value;
			
			D2D_COLOR_F degreesTextShadowCol = settings.getSettingByName<bool>("degreesTextShadowRGB")->value ? 
				FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("degreesTextShadowCol")->value);
			degreesTextShadowCol.a = settings.getSettingByName<float>("degreesTextShadowOpacity")->value;
			
			ImVec2 textMetrics = FlarialGUI::getFlarialTextSize(
				FlarialGUI::to_wide(degreesText).c_str(),
				0, 0,
				DWRITE_TEXT_ALIGNMENT_CENTER, degreesTextSize,
				DWRITE_FONT_WEIGHT_NORMAL, true);
			
			// shadow
			if (settings.getSettingByName<bool>("degreesTextShadow")->value) {
				FlarialGUI::FlarialTextWithFont(
					hudCenterX + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value) * uiscale,
					realcenter.y - textMetrics.y - degreesTextOffset + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value) * uiscale,
					FlarialGUI::to_wide(degreesText).c_str(),
					0, 0,
					DWRITE_TEXT_ALIGNMENT_CENTER, degreesTextSize,
					DWRITE_FONT_WEIGHT_NORMAL,
					degreesTextShadowCol, true);
			}
			
			// draw the text itself
			FlarialGUI::FlarialTextWithFont(
				hudCenterX,
				realcenter.y - textMetrics.y - degreesTextOffset,
				FlarialGUI::to_wide(degreesText).c_str(),
				0, 0,
				DWRITE_TEXT_ALIGNMENT_CENTER, degreesTextSize,
				DWRITE_FONT_WEIGHT_NORMAL,
				degreesTextCol, true);
		}
		
		for (int i = 0; i < directions.size(); i++) {
			bool isOrdinal = directions[i].first.length() == 2;

			if (settings.getSettingByName<bool>("wrapFade")->value) {
				float distanceFromCenter = std::abs(directions[i].second-hudCenterX);
				float maxDistance = fullCirclePixelWidth / 2;
				float omittedDistance = maxDistance * settings.getSettingByName<float>("fadeDistancePerc")->value / 100;

				D2D1_COLOR_F* cols[8] = {&ordinalScaleCol, &ordinalScaleShadowCol, &cardinalScaleCol, &cardinalScaleShadowCol, &cardinalTextCol, &cardinalTextShadowCol, &ordinalTextCol, &ordinalTextShadowCol};

				if (distanceFromCenter > omittedDistance) {
					for (int j = 0; j < 8; j++) {
						cols[j]->a = 1 - (distanceFromCenter - omittedDistance) / (maxDistance - omittedDistance);
					}
				}
				else {
					for (int j = 0; j < 8; j++) {
						cols[j]->a = 1;
					}
				}
			}
			else {
				cardinalScaleCol.a = settings.getSettingByName<float>("cardinalScaleOpacity")->value;
				ordinalScaleCol.a = settings.getSettingByName<float>("ordinalScaleOpacity")->value;
				cardinalTextCol.a = settings.getSettingByName<float>("cardinalTextOpacity")->value;
				ordinalTextCol.a = settings.getSettingByName<float>("ordinalTextOpacity")->value;
				cardinalTextShadowCol.a = settings.getSettingByName<float>("cardinalTextShadowOpacity")->value;
				ordinalTextShadowCol.a = settings.getSettingByName<float>("ordinalTextShadowOpacity")->value;
				cardinalScaleShadowCol.a = settings.getSettingByName<float>("cardinalScaleShadowOpacity")->value;
				ordinalScaleShadowCol.a = settings.getSettingByName<float>("ordinalScaleShadowOpacity")->value;
			}


			if (settings.getSettingByName<bool>("showScales")->value) {
				if ((!isOrdinal && settings.getSettingByName<bool>("showCardinalScale")->value) ||
					(isOrdinal && settings.getSettingByName<bool>("showOrdinalScale")->value)) {
					if (settings.getSettingByName<bool>("scaleShadow")->value) FlarialGUI::RoundedRect(
						(directions[i].second - (isOrdinal ? ordinalBarWidth : cardinalBarWidth) / 2.f) + Constraints::RelativeConstraint(settings.getSettingByName<float>("scaleShadowOffset")->value) * uiscale,
						realcenter.y + Constraints::RelativeConstraint(settings.getSettingByName<float>("scaleShadowOffset")->value) * uiscale,
						isOrdinal ? ordinalScaleShadowCol : cardinalScaleShadowCol,
						isOrdinal ? ordinalBarWidth : cardinalBarWidth,
						isOrdinal ? barHeight / 2.f : barHeight,
						0, 0
					);

					FlarialGUI::RoundedRect(
						(directions[i].second - (isOrdinal ? ordinalBarWidth : cardinalBarWidth) / 2.f),
						realcenter.y,
						isOrdinal ? ordinalScaleCol : cardinalScaleCol,
						isOrdinal ? ordinalBarWidth : cardinalBarWidth,
						isOrdinal ? barHeight / 2.f : barHeight,
						0, 0
					);
				}
			}

			

			if (settings.getSettingByName<bool>("showText")->value) {
				if ((!isOrdinal && settings.getSettingByName<bool>("showCardinalText")->value) ||
					(isOrdinal && settings.getSettingByName<bool>("showOrdinalText")->value)) {
					ImVec2 textMetrics = FlarialGUI::getFlarialTextSize(
						FlarialGUI::to_wide(directions[i].first).c_str(),
						0, 0,
						DWRITE_TEXT_ALIGNMENT_CENTER, isOrdinal ? OtextSize : CtextSize,
						DWRITE_FONT_WEIGHT_NORMAL, true);

					if (settings.getSettingByName<bool>("textShadow")->value) FlarialGUI::FlarialTextWithFont(
						directions[i].second + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value) * uiscale,
						realcenter.y + ((isOrdinal ? barHeight / 2.f : barHeight) + textMetrics.y / 2.f) * (1.2f * uiscale) + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value) * uiscale + (isOrdinal ? ordinalTextOffset : cardinalTextOffset),
						FlarialGUI::to_wide(directions[i].first).c_str(),
						0, 0,
						DWRITE_TEXT_ALIGNMENT_CENTER, isOrdinal ? OtextSize : CtextSize,
						DWRITE_FONT_WEIGHT_NORMAL,
						isOrdinal ? ordinalTextShadowCol : cardinalTextShadowCol, true);

					FlarialGUI::FlarialTextWithFont(
						directions[i].second,
						realcenter.y + ((isOrdinal ? barHeight / 2.f : barHeight) + textMetrics.y / 2.f) * (1.2f * uiscale) + (isOrdinal ? ordinalTextOffset : cardinalTextOffset),
						FlarialGUI::to_wide(directions[i].first).c_str(),
						0, 0,
						DWRITE_TEXT_ALIGNMENT_CENTER, isOrdinal ? OtextSize : CtextSize,
						DWRITE_FONT_WEIGHT_NORMAL,
						isOrdinal ? ordinalTextCol : cardinalTextCol, true);
				}
			}
		}
	}
};
