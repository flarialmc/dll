#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class Mousestrokes : public Module {

public:

	Mousestrokes() : Module("Mouse Strokes", "Visualizes the position of your mouse.",
		IDR_CURSOR_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &Mousestrokes::onRender);
		Listen(this, MouseEvent, &Mousestrokes::onMouse);
		Module::onEnable();

		std::thread normalizeCursor([&]() {
			while (!Client::disable) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));

				X *= 0.01;
				Y *= 0.01;
			}
			});

		normalizeCursor.detach();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &Mousestrokes::onRender);
		Deafen(this, MouseEvent, &Mousestrokes::onMouse);
		Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
		Module::defaultConfig("core");
		Module::defaultConfig("pos");
		Module::defaultConfig("main");
		Module::defaultConfig("colors");
		if (settings.getSettingByName<std::string>("cursorColor") == nullptr) settings.addSetting("cursorColor", (std::string)"ffffff");
		if (settings.getSettingByName<float>("cursorOpacity") == nullptr) settings.addSetting("cursorOpacity", 1.0f);
		if (settings.getSettingByName<bool>("cursorRGB") == nullptr) settings.addSetting("cursorRGB", false);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		addHeader("Main");
		defaultAddSettings("main");
		extraPadding();

		addHeader("Colors");
		addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value, settings.getSettingByName<bool>("bgRGB")->value);
		addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);
		addColorPicker("Cursor Color", "", settings.getSettingByName<std::string>("cursorColor")->value, settings.getSettingByName<float>("cursorOpacity")->value, settings.getSettingByName<bool>("cursorRGB")->value);

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

	void onRender(RenderEvent& event) {
		if (!this->isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;
		this->normalRender(31, (std::string&)"");
	}

	void onMouse(MouseEvent& event) {
		X += event.getMouseMovementX();
		Y += event.getMouseMovementY();
	}

	struct CircleTrail {
		float x, y, opacity;
	};

	std::vector<CircleTrail> trails = {};

	float MousePosX, MousePosY;

	void normalRender(int index, std::string& value) override {
		if (!SDK::hasInstanced) return;
		if (SDK::clientInstance->getLocalPlayer() == nullptr) return;

		float scale = this->settings.getSettingByName<float>("uiscale")->value;

		Vec2 RectSize = Vec2(Constraints::PercentageConstraint(0.15, "top") * scale, Constraints::PercentageConstraint(0.15, "top") * scale);


		Vec2<float> settingperc = Vec2<float>(
			this->settings.getSettingByName<float>("percentageX")->value,
			this->settings.getSettingByName<float>("percentageY")->value
		);

		Vec2<float> realcenter;

		if (settingperc.x != 0)
			realcenter = Vec2(settingperc.x * (MC::windowSize.x - RectSize.x), settingperc.y * (MC::windowSize.y - RectSize.y));    else
			realcenter = Constraints::CenterConstraint(RectSize.y, RectSize.y * this->settings.getSettingByName<float>(
				"rectheight")->value);

		if (ClickGUI::editmenu) {
			auto height = RectSize.y * this->settings.getSettingByName<float>("rectheight")->value;
			FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, RectSize.x,
				height, index);

			Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, RectSize.x, height);
			checkForRightClickAndOpenSettings(realcenter.x, realcenter.y, RectSize.x, height);

			realcenter.x = vec2.x;
			realcenter.y = vec2.y;

			realcenter = realcenter;

			Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y, RectSize.x, RectSize.y);
			this->settings.setValue("percentageX", percentages.x);
			this->settings.setValue("percentageY", percentages.y);
		}


		Vec2<float> RectPos = realcenter;

		FlarialGUI::lerp<float>(CurrentCursorPos.x, X * (Constraints::PercentageConstraint(1, "top") / 1080) * scale, FlarialGUI::frameFactor * 0.25);
		FlarialGUI::lerp<float>(CurrentCursorPos.y, Y * (Constraints::PercentageConstraint(1, "top") / 1080) * scale, FlarialGUI::frameFactor * 0.25);

		Vec2 CursorPos(RectSize.div(Vec2<float>(2, 2)).add(RectPos).add(CurrentCursorPos));
		Vec2 Centre = RectSize.div(Vec2<float>(2, 2));

		if (CursorPos.x < RectPos.x) CursorPos.x = RectPos.x;
		if (CursorPos.y < RectPos.y) CursorPos.y = RectPos.y;

		if (CursorPos.x > RectPos.add(RectSize).x) CursorPos.x = RectPos.add(RectSize).x;
		if (CursorPos.y > RectPos.add(RectSize).y) CursorPos.y = RectPos.add(RectSize).y;

		CircleTrail balls;

		balls.x = CursorPos.x;
		balls.y = CursorPos.y;
		balls.opacity = 1;

		trails.insert(trails.begin(), balls);

		D2D1_COLOR_F bgColor = settings.getSettingByName<bool>("bgRGB")->value ? FlarialGUI::rgbColor: FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);
		bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;

		D2D1_COLOR_F CursorColor = settings.getSettingByName<bool>("cursorRGB")->value ? FlarialGUI::rgbColor: FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("cursorColor")->value);
		CursorColor.a = settings.getSettingByName<float>("cursorOpacity")->value;

		D2D1_COLOR_F borderColor = settings.getSettingByName<bool>("borderRGB")->value ? FlarialGUI::rgbColor: FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("borderColor")->value);
		borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;


		Vec2<float> rounde = Constraints::RoundingConstraint(this->settings.getSettingByName<float>("rounding")->value *
			settings.getSettingByName<float>("uiscale")->value,
			this->settings.getSettingByName<float>("rounding")->value *
			settings.getSettingByName<float>("uiscale")->value);

		if (settings.getSettingByName<bool>("BlurEffect")->value) {
			FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(RectPos.x, RectPos.y,
				RectPos.x + RectSize.x,
				RectPos.y + RectSize.y), rounde.x, rounde.x));
		}

		FlarialGUI::RoundedRect(RectPos.x, RectPos.y, bgColor, RectSize.x, RectSize.y, rounde.x, rounde.x);

		if (this->settings.getSettingByName<bool>("border")->value) {
			FlarialGUI::RoundedHollowRect(
				RectPos.x,
				RectPos.y,
				Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value *
					settings.getSettingByName<float>("uiscale")->value) / 100.0f,
					"height", true),
				borderColor,
				RectSize.x,
				RectSize.y,
				rounde.x,
				rounde.x
			);
		}

		for (CircleTrail BariNudes : trails) {

			FlarialGUI::Circle(BariNudes.x, BariNudes.y, CursorColor, Constraints::PercentageConstraint(0.0074, "top") * scale);

			CursorColor.a *= 0.9;

			if (CursorColor.a < 0.0001) {
				trails.pop_back();
				return;
			}
		}
	}

	int X = 0;
	int Y = 0;

	Vec2<float> CurrentCursorPos = Vec2<float>(0, 0);
};