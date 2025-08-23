#include "BlockBreakIndicator.hpp"

#include "Events/EventManager.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Modules/CPS/CPSCounter.hpp"

void BlockBreakIndicator::onEnable() {
    Listen(this, RenderEvent, &BlockBreakIndicator::onRender)
    Module::onEnable();
}

void BlockBreakIndicator::onDisable() {
    Deafen(this, RenderEvent, &BlockBreakIndicator::onRender)
    Module::onDisable();
}

void BlockBreakIndicator::defaultConfig() {
    settings.renameSetting("barFill", "barFillOpacity", "barFillRGB", "barFill");
    Module::defaultConfig("all");
    setDef("textscale", 1.00f);
    setDef("pbmode", true);
    setDef("onlyShowWhileBreaking", false);
    setDef("orientation", (std::string) "Vertical");
    setDef("pbwidth", 0.91f);
    setDef("pbheight", 0.82f);
    setDef("barFill", (std::string) "a83232", 1.f, false);
    
}

void BlockBreakIndicator::settingsRender(float settingsOffset) {
    initSettingsPage();


    addHeader("Block Break Indicator");
    addToggle("Progress Bar", "Whether to show a Progress Bar or text", "pbmode");
    defaultAddSettings("main");
    extraPadding();

    if (!getOps<bool>("pbmode")) {
        addHeader("Text");
        defaultAddSettings("text");
        extraPadding();
    } else {
        addHeader("Bar");
        addToggle("Only show while breaking", "", "onlyShowWhileBreaking");
        addDropdown("Orientation", "", std::vector<std::string>{"Vertical", "Horizontal"}, "orientation", true);
        addColorPicker("Color", "", "barFill");
        addSlider("Width", "", "pbwidth", 20.f);
        addSlider("Height", "", "pbheight", 10.f);
        extraPadding();
    }

    addHeader("Colors");
    addConditionalColorPicker(!getOps<bool>("pbmode"), "Text Color", "", "text");
    addColorPicker("Background Color", "", "bg");
    addConditionalColorPicker(getOps<bool>("rectShadow"), "Background Shadow Color", "", "rectShadow");
    addConditionalColorPicker(getOps<bool>("textShadow"), "Text Shadow Color", "", "textShadow");
    addConditionalColorPicker(getOps<bool>("border"), "Border Color", "", "border");
    addConditionalColorPicker(getOps<bool>("glow"), "Glow Color", "", "glow");
    extraPadding();

    addHeader("Misc");
    addConditionalToggle(!getOps<bool>("pbmode"), "Reverse Padding X", "For Text Position", "reversepaddingx");
    addConditionalToggle(!getOps<bool>("pbmode"), "Reverse Padding Y", "For Text Position", "reversepaddingy");
    addConditionalSlider(!getOps<bool>("pbmode"), "Padding X", "For Text Position", "padx");
    addConditionalSlider(!getOps<bool>("pbmode"), "Padding Y", "For Text Position", "pady");
    addConditionalSlider(!getOps<bool>("pbmode"), "Rectangle Width", "", "rectwidth", 2.f, 0.001f);
    addConditionalSlider(!getOps<bool>("pbmode"), "Rectangle Height", "", "rectheight", 2.f, 0.001f);
    addConditionalToggle(!getOps<bool>("pbmode"), "Responsive Rectangle", "Rectangle resizes with text", "responsivewidth");
    addSlider("Rotation", "see for yourself!", "rotation", 360.f, 0, false);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void BlockBreakIndicator::normalRender(int index, std::string &value) {
    if (!this->isEnabled()) return;
    if (SDK::getCurrentScreen() != "hud_screen") return;

    if (getOps<bool>("pbmode")) {
        if (getOps<bool>("onlyShowWhileBreaking") && value == "0%") return;

        value.pop_back();
        float percent;
        std::istringstream(value) >> percent;
        percent /= 100.0f;

        float barheight = Constraints::RelativeConstraint(
            0.5f * getOps<float>("uiscale"));
        float barwidth = Constraints::RelativeConstraint(
            0.05f * getOps<float>("uiscale"));

        D2D1_COLOR_F bgColor = getColor("bg");
        D2D1_COLOR_F barFill = getColor("barFill");
        D2D1_COLOR_F outline = getColor("border");

        std::string orientation = getOps<std::string>("orientation");

        Vec2<float> settingperc = Vec2<float>(
            getOps<float>("percentageX"),
            getOps<float>("percentageY")
        );

        Vec2<float> coord;
        Vec2<float> rounde = Constraints::RoundingConstraint(
            getOps<float>("rounding") *
            getOps<float>("uiscale"),
            getOps<float>("rounding") *
            getOps<float>("uiscale"));

        if (settingperc.x != 0)
            coord = Vec2<float>(settingperc.x * MC::windowSize.x, settingperc.y * MC::windowSize.y);
        else
            coord = Constraints::CenterConstraint(barwidth * getOps<float>("pbwidth"),
                                                  barheight * getOps<float>("pbheight"));

        float pbwidth = barwidth * getOps<float>("pbwidth");
        float pbheight = barheight * getOps<float>("pbheight");

        if (orientation == "Horizontal") {
            std::swap(pbwidth, pbheight);
        }

        if (ClickGUI::editmenu) {
            FlarialGUI::SetWindowRect(coord.x, coord.y, pbwidth, pbheight, index, this->name);
            checkForRightClickAndOpenSettings(coord.x, coord.y, pbwidth, pbheight);

            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(coord.x, coord.y, index, pbwidth, pbheight);

            coord.x = vec2.x;
            coord.y = vec2.y;

            coord = coord;

            Vec2<float> percentages = Constraints::CalculatePercentage(coord.x, coord.y);

            this->settings.setValue("percentageX", percentages.x);
            this->settings.setValue("percentageY", percentages.y);
        }

        float filledHeight = (orientation == "Horizontal" ? pbwidth : pbheight) * percent;

        FlarialGUI::lerp(currentHeight, filledHeight, 0.5f * FlarialGUI::frameFactor);

        if (getOps<bool>("glow"))
            FlarialGUI::ShadowRect(Vec2<float>(coord.x, coord.y), Vec2<float>(pbwidth, pbheight), getColor("glow"),
                                   rounde.x,
                                   (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(
                                       0.1f, "top"));

        if (getOps<bool>("BlurEffect"))
            FlarialGUI::BlurRect(
                D2D1::RoundedRect(D2D1::RectF(coord.x, coord.y, coord.x + pbwidth, coord.y + pbheight),
                                  rounde.x, rounde.x));

        if (getOps<bool>("rectShadow")) FlarialGUI::RoundedRect(
            coord.x + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
            coord.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
            getColor("rectShadow"),
            pbwidth,
            pbheight,
            rounde.x,
            rounde.y
        );

        FlarialGUI::RoundedRect(coord.x, coord.y, bgColor, pbwidth, pbheight, rounde.x, rounde.y);

        if (currentHeight > 0.5f) {
            if (orientation == "Horizontal")
                FlarialGUI::RoundedRect(coord.x, coord.y, barFill, currentHeight, pbheight, rounde.x, rounde.y);
            else
                FlarialGUI::RoundedRect(coord.x, coord.y + pbheight - currentHeight, barFill, pbwidth,
                                        currentHeight,
                                        rounde.x, rounde.y);
        }

        if (getOps<bool>("border")) {
            FlarialGUI::RoundedHollowRect(
                coord.x,
                coord.y,
                Constraints::RelativeConstraint((getOps<float>("borderWidth") *
                                                 getOps<float>("uiscale")) / 100.0f,
                                                "height", true),
                outline,
                pbwidth,
                pbheight,
                rounde.x,
                rounde.y
            );
        }
    } else {
        Module::normalRender(index, value);
    }

    if (ClickGUI::editmenu) {
        FlarialGUI::UnsetWindowRect();
    }
}

void BlockBreakIndicator::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (
        SDK::hasInstanced && SDK::clientInstance != nullptr &&
        SDK::clientInstance->getLocalPlayer() != nullptr &&
        SDK::clientInstance->getLocalPlayer()->getGamemode() != nullptr
    ) {
        if (SDK::getCurrentScreen() != "hud_screen") return;

        if (MC::heldLeft) {
            Gamemode *gamemode = SDK::clientInstance->getLocalPlayer()->getGamemode();
            auto progress = gamemode->getLastBreakProgress() * 100;
            if (lastProgress != progress) {
                if (lastProgress < progress || progress == 0.f) {
                    currentProgress = progress;
                }
                lastProgress = progress;
            }
        } else {
            currentProgress = 0.0f;
        }

        std::string progress = std::format("{:.0f}%", currentProgress);
        this->normalRender(16, progress);
    }
}