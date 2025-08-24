#include "Mousestrokes.hpp"

#include "Client.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"

Mousestrokes::Mousestrokes() : Module("Mouse Strokes", "Visualizes the position of your mouse.",
	IDR_CURSOR_PNG, "")
{

}

void Mousestrokes::onEnable()
{
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

void Mousestrokes::onDisable()
{
	Deafen(this, RenderEvent, &Mousestrokes::onRender);
	Deafen(this, MouseEvent, &Mousestrokes::onMouse);
	Module::onDisable();
}

void Mousestrokes::defaultConfig()
{
	settings.renameSetting("cursorColor", "cursorCol");
	setDef("textscale", 0.8f);
	Module::defaultConfig("core");
	Module::defaultConfig("pos");
	Module::defaultConfig("main");
	Module::defaultConfig("colors");
	setDef("cursor", (std::string)"ffffff", 1.f, false);
	
}

void Mousestrokes::settingsRender(float settingsOffset)
{
	initSettingsPage();


	addHeader("Main");
	defaultAddSettings("main");
	extraPadding();

	addHeader("Colors");
	addConditionalColorPicker(getOps<bool>("showBg"), "Background Color", "", "bg");
	addConditionalColorPicker(getOps<bool>("rectShadow"), "Background Shadow Color", "", "rectShadow");
	addConditionalColorPicker(getOps<bool>("border"), "Border Color", "", "border");
	addColorPicker("Cursor Color", "", "cursor");
	addConditionalColorPicker(getOps<bool>("glow"), "Glow Color", "", "glow");

	FlarialGUI::UnsetScrollView();
	resetPadding();
}

void Mousestrokes::onRender(RenderEvent& event)
{
	if (!this->isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;
	this->normalRender(31, (std::string&)"");
}

void Mousestrokes::onMouse(MouseEvent& event)
{
	if (!this->isEnabled()) return;
	X += event.getMouseMovementX();
	Y += event.getMouseMovementY();
}

void Mousestrokes::normalRender(int index, std::string& value)
{
	if (!this->isEnabled()) return;
	if (!SDK::hasInstanced) return;
	if (SDK::clientInstance->getLocalPlayer() == nullptr) return;

	float scale = getOps<float>("uiscale");

	Vec2 RectSize = Vec2(Constraints::PercentageConstraint(0.15, "top") * scale, Constraints::PercentageConstraint(0.15, "top") * scale);


	Vec2<float> settingperc = Vec2<float>(
		getOps<float>("percentageX"),
		getOps<float>("percentageY")
	);

	Vec2<float> realcenter;

	if (settingperc.x != 0) realcenter = Vec2(settingperc.x * (MC::windowSize.x - RectSize.x), settingperc.y * (MC::windowSize.y - RectSize.y));
	else realcenter = Constraints::CenterConstraint(RectSize.y, RectSize.y);

	if (ClickGUI::editmenu) {
		auto height = RectSize.y;
		FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, RectSize.x, height, index, this->name);

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

	D2D1_COLOR_F CursorColor = getColor("cursor");

	Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding") *
		getOps<float>("uiscale"),
		getOps<float>("rounding") *
		getOps<float>("uiscale"));



	if (getOps<bool>("glow")) FlarialGUI::ShadowRect(
		Vec2<float>(RectPos.x, RectPos.y),
		Vec2<float>(RectSize.x, RectSize.y),
		getColor("glow"), rounde.x,
		(getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

	if (getOps<bool>("BlurEffect"))FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(RectPos.x, RectPos.y,
		RectPos.x + RectSize.x,
		RectPos.y + RectSize.y), rounde.x, rounde.x));

	if (getOps<bool>("rectShadow")) FlarialGUI::RoundedRect(
		RectPos.x + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
		RectPos.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
		getColor("rectShadow"),
		RectSize.x,
		RectSize.y,
		rounde.x,
		rounde.x
	);

	FlarialGUI::RoundedRect(RectPos.x, RectPos.y, getColor("bg"), RectSize.x, RectSize.y, rounde.x, rounde.x);

	if (getOps<bool>("border")) {
		FlarialGUI::RoundedHollowRect(
			RectPos.x,
			RectPos.y,
			Constraints::RelativeConstraint((getOps<float>("borderWidth") *
				getOps<float>("uiscale")) / 100.0f,
				"height", true),
			getColor("border"),
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
