#pragma once

#include "../Module.hpp"

class ReachCounter : public Module {
private:
	float Reach = 0.0f;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
public:
	ReachCounter() : Module("Reach Counter", "Displays your last hit range in blocks.", IDR_REACH_PNG,
		"") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, AttackEvent, &ReachCounter::onAttack)
			Listen(this, TickEvent, &ReachCounter::onTick)
			Listen(this, RenderEvent, &ReachCounter::onRender)
			Module::onEnable();
	}

	void onDisable() override {


		Module::onDisable();

	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"Reach: {value}");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Main");
		this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.0f);
		this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
		this->addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", this->settings.getSettingByName<bool>(
			"BlurEffect")->value);
		this->addConditionalSlider(this->settings.getSettingByName<bool>(
			"border")->value, "Border Thickness", "", this->settings.getSettingByName<float>("borderWidth")->value, 4.f);
		this->addSlider("Rounding", "Rounding of the rectangle", this->settings.getSettingByName<float>("rounding")->value);

		this->extraPadding();

		this->addHeader("Text");
		this->addTextBox("Format", "", settings.getSettingByName<std::string>("text")->value);
		this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.0f);
		this->addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);
		this->addColorPicker("Color", "Text Color", settings.getSettingByName<std::string>("textColor")->value,
			settings.getSettingByName<float>("textOpacity")->value,
			settings.getSettingByName<bool>("textRGB")->value);
		this->addToggle("Text Shadow", "Displays a shadow under the text", settings.getSettingByName<bool>("textShadow")->value);
		this->addColorPicker("Shadow Color", "Text Shadow Color", settings.getSettingByName<std::string>("textShadowCol")->value,
			settings.getSettingByName<float>("textShadowOpacity")->value,
			settings.getSettingByName<bool>("textShadowRGB")->value);
		this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);

		this->extraPadding();

		this->addHeader("Colors");
		this->addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value,
			settings.getSettingByName<float>("bgOpacity")->value,
			settings.getSettingByName<bool>("bgRGB")->value);
		this->addToggle("Background Shadow", "Displays a shadow under the background", settings.getSettingByName<bool>("rectShadow")->value);
		this->addColorPicker("Shadow Color", "Background Shadow Color", settings.getSettingByName<std::string>("rectShadowCol")->value,
			settings.getSettingByName<float>("rectShadowOpacity")->value,
			settings.getSettingByName<bool>("rectShadowRGB")->value);
		this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("rectShadowOffset")->value, 0.02f, 0.001f);

		this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value,
			settings.getSettingByName<float>("borderOpacity")->value,
			settings.getSettingByName<bool>("borderRGB")->value);        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);

		this->extraPadding();

		this->addHeader("Misc Customizations");

		this->addToggle("Reverse Padding X", "For Text Position", this->settings.getSettingByName<bool>(
			"reversepaddingx")->value);

		this->addToggle("Reverse Padding Y", "For Text Position", this->settings.getSettingByName<bool>(
			"reversepaddingy")->value);

		this->addSlider("Padding X", "For Text Position", this->settings.getSettingByName<float>("padx")->value);
		this->addSlider("Padding Y", "For Text Position", this->settings.getSettingByName<float>("pady")->value);

		this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value, 2.f, 0.001f);
		this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value, 2.f, 0.001f);

		this->addToggle("Responsive Rectangle", "Rectangle resizes with text", this->settings.getSettingByName<bool>(
			"responsivewidth")->value);

		this->addSlider("Rotation", "see for yourself!", this->settings.getSettingByName<float>("rotation")->value, 360.f, 0, false);

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	void onAttack(AttackEvent& event) {
		Reach = event.getActor()->getLevel()->getHitResult().distance();
		last_hit = std::chrono::high_resolution_clock::now();
	}

	void onTick(TickEvent& event) {
		std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
		if (duration.count() >= 15) Reach = 0.0f;

	}

	void onRender(RenderEvent& event) {
		if (this->isEnabled()) {

			std::ostringstream oss;
			oss << std::fixed << std::setprecision(2) << Reach;
			std::string reachString = oss.str();

			this->normalRender(9, reachString);

		}
	}
};