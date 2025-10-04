#include "Stopwatch.hpp"

#include "Client/GUI/Engine/Constraints.hpp"
#include "Client.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Utils/Utils.hpp"
#include "Events/EventManager.hpp"


void Stopwatch::onSetup() {
	keybindActions.clear();

	keybindActions.push_back([this](std::vector<std::any> args) -> std::any {

		KeyEvent event = std::any_cast<KeyEvent>(args[0]);
		if (this->isKeybind(event.keys, 1) && this->isKeyPartOfKeybind(event.key, 1)) { // start/pause/resume
			if (isRunning) {
				elapsed += Utils::Microtime() - start;
				isRunning = false;
			}
			else {
				start = Utils::Microtime();
				isRunning = true;
			}

		}
		else if (this->isKeybind(event.keys, 2) && this->isKeyPartOfKeybind(event.key, 2)) { // reset
			if (!isRunning || getOps<bool>("allowResetWhileRunning")) {
				elapsed = 0;
				isRunning = false;
			}
			else {
				FlarialGUI::Notify("You can change this in settings.");
				FlarialGUI::Notify("You cannot reset the stopwatch while it is running.");
			}
		}
		return {};

	});

}

void Stopwatch::onEnable() {
	Listen(this, KeyEvent, &Stopwatch::onKey)
	Listen(this, RenderEvent, &Stopwatch::onRender)
		Module::onEnable();
}

void Stopwatch::onDisable() {
	Deafen(this, KeyEvent, &Stopwatch::onKey)
	Listen(this, RenderEvent, &Stopwatch::onRender)
		Module::onDisable();
}

void Stopwatch::defaultConfig() {
	getKeybind(1);
	getKeybind(2);
	Module::defaultConfig("all");
	// TODO: Rename these
	setDef("keybind-1", std::string("P")); // start/pause/resume
	setDef("keybind-2", std::string("L")); // reset
	setDef("allowResetWhileRunning", false);

	setDef("runningText", (std::string) "00ff00", 1.f, false);
	setDef("pausedText", (std::string) "ffa500", 1.f, false);
	setDef("defaultText", (std::string) "fafafa", 1.f, false);
}

void Stopwatch::settingsRender(float settingsOffset) {
	initSettingsPage();

	addKeybind("Start/Pause/Resume Keybind", "", "keybind-1", true);
	addKeybind("Reset Keybind", "", "keybind-2", true);
	addToggle("Allow Resetting While Running", "Turning this on allows you to reset the stopwatch without pausing it.", "allowResetWhileRunning");
	extraPadding();

	addHeader("Main");
	defaultAddSettings("main");
	extraPadding();

	addHeader("Text");
	defaultAddSettings("text");
	extraPadding();

	addHeader("Colors");
	addColorPicker("Running Color", "", "runningText");
	addColorPicker("Paused Color", "", "pausedText");
	addColorPicker("Default Color", "", "defaultText");
	addConditionalColorPicker(getOps<bool>("showBg"), "Background Color", "", "bg");
	addConditionalColorPicker(getOps<bool>("rectShadow"), "Background Shadow Color", "", "rectShadow");
	addConditionalColorPicker(getOps<bool>("textShadow"), "Text Shadow Color", "", "textShadow");
	addConditionalColorPicker(getOps<bool>("border"), "Border Color", "", "border");
	addConditionalColorPicker(getOps<bool>("glow"), "Glow Color", "", "glow");
	extraPadding();

	addHeader("Misc");
	defaultAddSettings("misc");

	FlarialGUI::UnsetScrollView();
	this->resetPadding();
}

void Stopwatch::normalRenderCore(int index, std::string &text) {
    if (!this->isEnabled()) return;
    float rotation = getOps<float>("rotation");
    DWRITE_TEXT_ALIGNMENT alignment = alignments[getOps<std::string>("textalignment")];
    bool responsivewidth = getOps<bool>("responsivewidth");
    float paddingX = getOps<float>("padx");
    float paddingY = getOps<float>("pady");

    if (getOps<bool>("reversepaddingx")) paddingX = -(getOps<float>("padx"));
    if (getOps<bool>("reversepaddingy")) paddingY = -(getOps<float>("pady"));

    float textWidth = Constraints::RelativeConstraint(0.7f * getOps<float>("uiscale"));
    float textHeight = Constraints::RelativeConstraint(0.1f * getOps<float>("uiscale"));
    float textSize = Constraints::SpacingConstraint(3.2f, textHeight) * getOps<float>("textscale");

    Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));

    float realspacing = Constraints::SpacingConstraint(0.05f, textWidth);

    ImVec2 textMetrics = FlarialGUI::getFlarialTextSize(
        FlarialGUI::to_wide(text).c_str(),
        1000000,
        textHeight,
        alignment,
        textSize, DWRITE_FONT_WEIGHT_NORMAL, true
    );

    float rectWidth = (
        !responsivewidth
            ? (Constraints::RelativeConstraint(0.225f * getOps<float>("uiscale")) * getOps<float>("rectwidth"))
            : (textMetrics.x + Constraints::SpacingConstraint(2.0, realspacing)) * getOps<float>("rectwidth")
    );
    float rectHeight = textHeight * getOps<float>("rectheight");

    Vec2<float> topleft;

    if (settingperc.x != 0) topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    else topleft = Constraints::CenterConstraint(rectWidth, rectHeight);

    if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_JUSTIFIED) prevAlignments[index] = alignment;

    if (prevAlignments[index] != alignment) {
        float toAdjust = 0;
        if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_CENTER) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_LEADING) toAdjust = rectWidth / -2.f;
            else toAdjust = rectWidth / 2.f;
        } else if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_LEADING) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_CENTER) toAdjust = rectWidth / 2.f;
            else toAdjust = rectWidth;
        } else if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_TRAILING) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_CENTER) toAdjust = rectWidth / -2.f;
            else toAdjust = -rectWidth;
        }

        settings.setValue("percentageX", (topleft.x + toAdjust) / MC::windowSize.x);

        settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
        topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    }

    prevAlignments[index] = alignment;

    if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
        if (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING) topleft.x -= rectWidth;
        else topleft.x -= rectWidth / 2.f;
    }

    if (ClickGUI::editmenu) {
        FlarialGUI::SetWindowRect(topleft.x, topleft.y, rectWidth, rectHeight, index, this->name);

        checkForRightClickAndOpenSettings(topleft.x, topleft.y, rectWidth, rectHeight);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(topleft.x, topleft.y, index, rectWidth, rectHeight);

        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING) vec2.x += rectWidth;
            else vec2.x += rectWidth / 2.f;
        }

        topleft.x = vec2.x;
        topleft.y = vec2.y;

        Vec2<float> percentages = Constraints::CalculatePercentage(topleft.x, topleft.y, 0, 0);
        settings.setValue("percentageX", percentages.x);
        settings.setValue("percentageY", percentages.y);

        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING) topleft.x -= rectWidth;
            else topleft.x -= rectWidth / 2.f;
        }
    }

    Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding") * getOps<float>("uiscale"), getOps<float>("rounding") * getOps<float>("uiscale"));
    ImVec2 rotationCenter;

    if (rotation > 0.0f) {
        rotationCenter = ImVec2(topleft.x + rectWidth / 2.0f, topleft.y + rectHeight / 2.0f);
        FlarialGUI::ImRotateStart();
    }

    if (getOps<bool>("glow"))
        FlarialGUI::ShadowRect(
            Vec2<float>(topleft.x, topleft.y),
            Vec2<float>(rectWidth, textHeight * getOps<float>("rectheight")),
            getColor("glow"), rounde.x,
            (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

    float blur = Client::settings.getSettingByName<float>("blurintensity")->value;
    if (getOps<bool>("BlurEffect")) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(topleft.x, topleft.y, topleft.x + rectWidth, topleft.y + rectHeight), rounde.x, rounde.x));
    //Blur::RenderBlur(SwapchainHook::mainRenderTargetView, 3, blur, topleft.x, topleft.y, rectWidth, rectHeight, rounde.x);


    if (getOps<bool>("rectShadow"))
        FlarialGUI::RoundedRect(
            topleft.x + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
            topleft.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
            getColor("rectShadow"),
            rectWidth,
            rectHeight,
            rounde.x,
            rounde.x
        );

    if (getOps<bool>("showBg"))
        FlarialGUI::RoundedRect(
            topleft.x,
            topleft.y,
            getColor("bg"),
            rectWidth,
            rectHeight,
            rounde.x,
            rounde.x
        );

    if (getOps<bool>("textShadow"))
        FlarialGUI::FlarialTextWithFont(
            topleft.x + Constraints::SpacingConstraint(paddingX, textWidth) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
            topleft.y + Constraints::SpacingConstraint(paddingY, textWidth) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
            FlarialGUI::to_wide(text).c_str(),
            rectWidth,
            rectHeight,
            alignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            getColor("textShadow"),
            true
        );

	D2D1_COLOR_F col;
	if (isRunning) col = getColor("runningText");
	else if (elapsed != 0) col = getColor("pausedText");
	else col = getColor("defaultText");
	FlarialGUI::FlarialTextWithFont(
		topleft.x + Constraints::SpacingConstraint(paddingX, textWidth),
		topleft.y + Constraints::SpacingConstraint(paddingY, textWidth),
		FlarialGUI::to_wide(text).c_str(),
		rectWidth,
		rectHeight,
		alignment,
		textSize, DWRITE_FONT_WEIGHT_NORMAL,
		col,
		true
	);

    if (getOps<bool>("border"))
        FlarialGUI::RoundedHollowRect(
            topleft.x,
            topleft.y,
            Constraints::RelativeConstraint((getOps<float>("borderWidth") * getOps<float>("uiscale")) / 100.0f, "height", true),
            getColor("border"),
            rectWidth,
            textHeight * getOps<float>("rectheight"),
            rounde.x,
            rounde.x
        );

    if (rotation > 0.0f) FlarialGUI::ImRotateEnd(rotation + 90.f, rotationCenter);

    if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
}

std::string Stopwatch::getFormattedTime(double seconds) {
	if (seconds == 0) return "0.000";

	int hours = static_cast<int>(seconds) / 3600;
	int mins = (static_cast<int>(seconds) % 3600) / 60;
	int secs = static_cast<int>(seconds) % 60;
	int millis = static_cast<int>(std::round((seconds - std::floor(seconds)) * 1000));

	if (millis == 1000) {
		++secs;
		millis = 0;
		if (secs == 60) {
			++mins;
			secs = 0;
			if (mins == 60) {
				++hours;
				mins = 0;
			}
		}
	}

	if (hours > 0) return std::format("{}:{}:{}.{:03}", hours, mins, secs, millis);
	else if (mins > 0) return std::format("{}:{}.{:03}", mins, secs, millis);
	else  return std::format("{}.{:03}", secs, millis);
}

void Stopwatch::onKey(KeyEvent& event) {
	if (!SDK::clientInstance->getLocalPlayer()) return;
	if (this->isEnabled() && SDK::getCurrentScreen() == "hud_screen") {
		this->keybindActions[0]({ std::any(event) });
	}
}

void Stopwatch::onRender(RenderEvent& event) {
	if (SDK::getCurrentScreen() != "hud_screen") return;

	double duration = isRunning ? Utils::Microtime() - start + elapsed : elapsed;
	std::string text = getFormattedTime(duration);

	this->normalRender(37, text);
}
