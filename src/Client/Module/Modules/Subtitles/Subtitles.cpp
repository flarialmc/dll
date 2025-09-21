#include "Subtitles.hpp"
#include "SoundDescriptions.hpp"

#include "Events/Game/SoundEnginePlayEvent.hpp"
#include "Client/GUI/Engine/Constraints.hpp"
#include "Client.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"



void Subtitles::onEnable() {
    Listen(this, RenderEvent, &Subtitles::onRender)
    Listen(this, SoundEnginePlayEvent, &Subtitles::onSoundEnginePlay)
    Module::onEnable();
}

void Subtitles::onDisable() {
    Deafen(this, RenderEvent, &Subtitles::onRender)
    Deafen(this, SoundEnginePlayEvent, &Subtitles::onSoundEnginePlay)
    Module::onDisable();
}

void Subtitles::defaultConfig() {
    setDef("enabled", false);
    Module::defaultConfig("all");
    setDef("rawMode", false);
    setDef("removeUseless", false);
    setDef("simplifySounds", false);
}

void Subtitles::settingsRender(float settingsOffset) {
    initSettingsPage();

    /*
    Not using defaultAddSettings("all") because some values are hardcoded
    alignment -> center
    responsive rect -> true
    */

    addToggle("Raw Mode", "For example, shows mob.panda.idle.worried instead of \'Panda whimpers\'", "rawMode");
    /* TBD
    addToggle("Remove Useless Sounds", "", "removeUseless");
    addToggle("Simplify Sounds", "For example, clubs all 22 ambient sounds into one.", "simplifySounds");
    */
    extraPadding();

    addHeader("Main");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    addSlider("Text Scale", "", "textscale", 2.0f);
    addToggle("Text Shadow", "Displays a shadow under the text", "textShadow");
    addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    addToggle("Reverse Padding X", "For Text Position", "reversepaddingx");
    addToggle("Reverse Padding Y", "For Text Position", "reversepaddingy");
    addSlider("Padding X", "For Text Position", "padx", 1.f);
    addSlider("Padding Y", "For Text Position", "pady", 1.f);
    addSlider("Rectangle Width", "", "rectwidth", 2.f, 0.001f);
    addSlider("Rectangle Height", "", "rectheight", 2.f, 0.001f);
    addSlider("Rotation", "", "rotation", 360.f, 0, false);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Subtitles::onSoundEnginePlay(SoundEnginePlayEvent& event) {

    std::string soundDescription = SoundDescriptions::getSoundDescription(event.name);

    std::erase_if(sounds, [&](const Sound& s) {
        return s.name == event.name;
    });

    if (!event.name.empty()) {
        sounds.emplace_back(Sound{
            event.name,
            event.pos,
            Microtime()
        });
    }

}

void Subtitles::normalRenderCore(int index, std::string &text) {
    if (!this->isEnabled()) return;
    if (sounds.empty()) return;

    if (ClickGUI::editmenu) text = "< Sound 1  \n  Sound 2  \n  Sound 3 >";

    float rotation = getOps<float>("rotation");
    DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
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

    float rectWidth = (textMetrics.x + Constraints::SpacingConstraint(2.0, realspacing)) * getOps<float>("rectwidth");
    float rectHeight = textHeight * getOps<float>("rectheight") * sounds.size() / 2.f;
    if (ClickGUI::editmenu) rectHeight = textHeight * getOps<float>("rectheight") * 1.5f;

    Vec2<float> topleft;

    if (settingperc.x != 0) topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    else topleft = Constraints::CenterConstraint(rectWidth, rectHeight);

    if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_JUSTIFIED) prevAlignments[index] = alignment;

    if (prevAlignments[index] != alignment) {
        float toAdjust = 0;
        if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_CENTER) {
            toAdjust = rectWidth / 2.f;
        }

        settings.setValue("percentageX", (topleft.x + toAdjust) / MC::windowSize.x);

        settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
        topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    }

    prevAlignments[index] = alignment;

    topleft.x -= rectWidth / 2.f;

    if (ClickGUI::editmenu) {

        FlarialGUI::SetWindowRect(topleft.x, topleft.y, rectWidth, rectHeight, index, this->name);

        checkForRightClickAndOpenSettings(topleft.x, topleft.y, rectWidth, rectHeight);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(topleft.x, topleft.y, index, rectWidth, rectHeight);

        vec2.x += rectWidth / 2.f;

        topleft.x = vec2.x;
        topleft.y = vec2.y;

        Vec2<float> percentages = Constraints::CalculatePercentage(topleft.x, topleft.y, 0, 0);
        settings.setValue("percentageX", percentages.x);
        settings.setValue("percentageY", percentages.y);

        topleft.x -= rectWidth / 2.f;
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

    if (false) {
        auto txtCol = getColor("text");
        float guiScale = SDK::clientInstance->getGuiData()->getGuiScale();

        SDK::pushDrawTextQueueEntry(DrawTextQueueEntry(
            text,
            RectangleArea(
                (topleft.x + Constraints::SpacingConstraint(paddingX, textWidth)) / guiScale,
                0,
                (topleft.y + Constraints::SpacingConstraint(paddingY, textWidth)) / guiScale,
                0),
            MCCColor(txtCol.r, txtCol.g, txtCol.b, txtCol.a),
            static_cast<ui::TextAlignment>(alignment),
            TextMeasureData(getOps<float>("textscale") * 2.f, getOps<bool>("textShadow"), false),
            CaretMeasureData{-1, 0}
        ));
    } else {
        FlarialGUI::FlarialTextWithFont(
            topleft.x + Constraints::SpacingConstraint(paddingX, textWidth),
            topleft.y + Constraints::SpacingConstraint(paddingY, textWidth),
            FlarialGUI::to_wide(text).c_str(),
            rectWidth,
            rectHeight,
            alignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            getColor("text"),
            true
        );
    }


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

void Subtitles::onRender(RenderEvent& event) {
    if (!this->isEnabled()) return;
    if (SDK::getCurrentScreen() != "hud_screen") return;

    updateSoundVec(sounds, 1.f);

    std::string text;

    for (auto s : sounds) {
        auto sides = s.getSides();
        std::string a;
        if (getOps<bool>("rawMode")) a = s.name;
        else a = s.getDescription();
        text += std::format("{} {} {}\n", sides.first, a, sides.second);
    }

    getOps<float>("rectheight") *= 1.f;

    this->normalRenderCore(36, text);
}

