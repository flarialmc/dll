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
    setDef("anchor", (std::string) "Bottom Center");
    setDef("lifetimeFade", true);
    setDef("lifetime", 1.5f);
    setDef("lineHeight", 1.f);
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
    addDropdown("Anchor Subtitles", "", std::vector<std::string>{
                    "Top Left",
                    "Top Center",
                    "Top Right",
                    "Middle Left",
                    "Middle Center",
                    "Middle Right",
                    "Bottom Left",
                    "Bottom Center",
                    "Bottom Right"
                }, "anchor", true);
    addToggle("Fade Out Effect", "", "lifetimeFade");
    addSlider("Sound Lifetime", "", "lifetime", 5.0f);
    extraPadding();

    addHeader("Text");
    addSlider("Text Scale", "", "textscale", 2.0f);
    addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, "textalignment", true);
    addSlider("Line Height", "", "lineHeight", 3.0f);
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

void Subtitles::onSoundEnginePlay(SoundEnginePlayEvent &event) {
    std::string soundDescription = SoundDescriptions::getSoundDescription(event.name);

    std::erase_if(sounds, [&](const Sound &s) {
        return s.name == event.name;
    });

    if (!event.name.empty()) {
        sounds.emplace_back(Sound{
            event.name,
            event.pos,
            "",
            Utils::Utils::Microtime()
        });
    }
}

void Subtitles::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (SDK::getCurrentScreen() != "hud_screen") return;
    if (sounds.empty() && !ClickGUI::editmenu) return;

    updateSoundVec(sounds, getOps<float>("lifetime"));

    std::vector<Sound> soundList = {};
    float longestStringWidth = 0;
    float realTextHeight = 0;

    DWRITE_TEXT_ALIGNMENT alignment = getOps<std::string>("anchor").contains("Left") ? DWRITE_TEXT_ALIGNMENT_LEADING : getOps<std::string>("anchor").contains("Center") ? DWRITE_TEXT_ALIGNMENT_CENTER : DWRITE_TEXT_ALIGNMENT_TRAILING;
    DWRITE_TEXT_ALIGNMENT textAlignment = alignments[getOps<std::string>("textalignment")];

    int curYAlign = getOps<std::string>("anchor").contains("Top") ? 0 : getOps<std::string>("anchor").contains("Middle") ? 1 : 2;

    float paddingX = getOps<float>("padx");
    float paddingY = getOps<float>("pady");

    float textWidth = Constraints::RelativeConstraint(0.7f * getOps<float>("uiscale"));
    float textHeight = Constraints::RelativeConstraint(0.1f * getOps<float>("uiscale"));
    float textSize = Constraints::SpacingConstraint(3.2f, textHeight) * getOps<float>("textscale");

    if (ClickGUI::editmenu)
        soundList = std::vector<Sound>{
            Sound{
                "Sound",
                Vec3<float>{0, 0, 0},
                "< Sound 1 >",
                Utils::Microtime()
            },
            Sound{
                "Sound",
                Vec3<float>{0, 0, 0},
                "< Sound 2 >",
                Utils::Microtime()
            },
            Sound{
                "Sound",
                Vec3<float>{0, 0, 0},
                "< Sound 3 >",
                Utils::Microtime()
            }
        };

    for (auto s: ClickGUI::editmenu ? soundList : sounds) {
        auto sides = s.getSides();
        std::string a;
        if (getOps<bool>("rawMode")) a = s.name;
        else a = s.getDescription();
        if (!ClickGUI::editmenu) {
            s.formatted = std::format("{} {} {}", sides.first, a, sides.second);
            soundList.push_back(s);
        }

        ImVec2 textMetrics = FlarialGUI::getFlarialTextSize(
            FlarialGUI::to_wide(s.formatted).c_str(),
            1000000,
            textHeight,
            alignment,
            Constraints::SpacingConstraint(3.2f, textHeight) * getOps<float>("textscale"), DWRITE_FONT_WEIGHT_NORMAL, true
        );

        if (longestStringWidth < textMetrics.x) longestStringWidth = textMetrics.x;
        realTextHeight = textMetrics.y * getOps<float>("lineHeight");
    }

    Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
    Vec2<float> topleft;

    float heightPadding = (Constraints::SpacingConstraint(2.0, Constraints::SpacingConstraint(0.05f, textHeight)) * getOps<float>("rectheight"));
    float widthPadding = Constraints::SpacingConstraint(2.0, Constraints::SpacingConstraint(0.05f, textWidth)) * getOps<float>("rectwidth");

    float rectWidth = longestStringWidth + widthPadding;
    float rectHeight = (realTextHeight * soundList.size()) + heightPadding;

    int index = 50; // dont let other modules interfere

    if (settingperc.x != 0) topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    else topleft = Constraints::CenterConstraint(rectWidth, rectHeight);

    if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_JUSTIFIED) prevAlignments[index] = alignment;
    if (prevYAlign == -1) prevYAlign = curYAlign;

    if (prevAlignments[index] != alignment) {
        float toAdjust = 0;
        float toAdjustY = 0;
        auto prev = prevAlignments[index];

        if (prev != alignment) {
            if (prev == DWRITE_TEXT_ALIGNMENT_CENTER) {
                toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_LEADING ? -0.5f : 0.5f) * rectWidth;
            } else if (prev == DWRITE_TEXT_ALIGNMENT_LEADING) {
                toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_CENTER ? 0.5f : 1.f) * rectWidth;
            } else if (prev == DWRITE_TEXT_ALIGNMENT_TRAILING) {
                toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_CENTER ? -0.5f : -1.f) * rectWidth;
            }
        }

        if (prevYAlign != curYAlign) {
            if (prevYAlign == 1) {
                toAdjustY = (curYAlign == 0 ? -0.5f : 0.5f) * rectHeight;
            } else if (prevYAlign == 0) {
                toAdjustY = (curYAlign == 1 ? 0.5f : 1.f) * rectHeight;
            } else if (prevYAlign == 2) {
                toAdjustY = (curYAlign == 1 ? -0.5f : -1.f) * rectHeight;
            }
        }

        settings.setValue("percentageX", (topleft.x + toAdjust) / MC::windowSize.x);
        settings.setValue("percentageY", (topleft.y + toAdjustY) / MC::windowSize.y);

        settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
        topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    }

    prevAlignments[index] = alignment;
    prevYAlign = curYAlign;

    if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) topleft.x -= (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);

    if (curYAlign != 0) topleft.y -= (curYAlign == 2 ? rectHeight : rectHeight / 2.f);

    if (ClickGUI::editmenu) {
        FlarialGUI::SetWindowRect(topleft.x, topleft.y, rectWidth, rectHeight, index, this->name);

        checkForRightClickAndOpenSettings(topleft.x, topleft.y, rectWidth, rectHeight);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(topleft.x, topleft.y, index, rectWidth, rectHeight);

        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) vec2.x += (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);
        if (curYAlign != 0) vec2.y += (curYAlign == 2 ? rectHeight : rectHeight / 2.f);

        topleft.x = vec2.x;
        topleft.y = vec2.y;

        Vec2<float> percentages = Constraints::CalculatePercentage(topleft.x, topleft.y, 0, 0);
        settings.setValue("percentageX", percentages.x);
        settings.setValue("percentageY", percentages.y);

        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) topleft.x -= (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);
        if (curYAlign != 0) topleft.y -= (curYAlign == 2 ? rectHeight : rectHeight / 2.f);
    }

    float rotation = getOps<float>("rotation");

    Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding") * getOps<float>("uiscale"), getOps<float>("rounding") * getOps<float>("uiscale"));
    ImVec2 rotationCenter;

    if (rotation > 0.0f) {
        rotationCenter = ImVec2(
            topleft.x + (alignment == DWRITE_TEXT_ALIGNMENT_LEADING ? 0 : alignment == DWRITE_TEXT_ALIGNMENT_CENTER ? rectWidth / 2.f : rectWidth),
            topleft.y + (curYAlign == 0 ? 0 : curYAlign == 1 ? rectHeight / 2.f : rectHeight));
        FlarialGUI::ImRotateStart();
    }

    if (getOps<bool>("glow"))
        FlarialGUI::ShadowRect(
            Vec2<float>(topleft.x, topleft.y),
            Vec2<float>(rectWidth, rectHeight),
            getColor("glow"), rounde.x,
            (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

    if (getOps<bool>("BlurEffect")) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(topleft.x, topleft.y, topleft.x + rectWidth, topleft.y + rectHeight), rounde.x, rounde.x));

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

    for (size_t i = 0; i < soundList.size(); ++i) {
        auto &sound = soundList[i];

        D2D_COLOR_F curCol = getColor("text");
        D2D_COLOR_F curCol_S = getColor("textShadow");

        if (getOps<bool>("lifetimeFade")) {
            curCol.a *= 1.f - (Utils::Microtime() - sound.timestamp) / getOps<float>("lifetime");
            curCol_S.a *= 1.f - (Utils::Microtime() - sound.timestamp) / getOps<float>("lifetime");
        }

        if (getOps<bool>("textShadow"))
            FlarialGUI::FlarialTextWithFont(
                topleft.x + Constraints::SpacingConstraint(paddingX, textWidth) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale") + (widthPadding / 2.f),
                topleft.y + Constraints::SpacingConstraint(paddingY, textWidth) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale") + (heightPadding / 2.f) + (realTextHeight * i),
                FlarialGUI::to_wide(sound.formatted).c_str(),
                rectWidth - widthPadding,
                realTextHeight,
                textAlignment,
                textSize, DWRITE_FONT_WEIGHT_NORMAL,
                curCol_S,
                true
            );

        FlarialGUI::FlarialTextWithFont(
            topleft.x + Constraints::SpacingConstraint(paddingX, textWidth) + (widthPadding / 2.f),
            topleft.y + Constraints::SpacingConstraint(paddingY, textWidth) + (heightPadding / 2.f) + (realTextHeight * i),
            FlarialGUI::to_wide(sound.formatted).c_str(),
            rectWidth - widthPadding,
            realTextHeight,
            textAlignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            curCol,
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
            rectHeight,
            rounde.x,
            rounde.x
        );

    if (rotation > 0.0f) FlarialGUI::ImRotateEnd(rotation + 90.f, rotationCenter);

    if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
}